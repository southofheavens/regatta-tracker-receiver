#include <Utils.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

#include <rgt/devkit/General.h>

#include <Poco/TemporaryFile.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/URI.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/StreamCopier.h>
#include <Poco/Crypto/DigestEngine.h>
#include <Poco/DigestEngine.h>
#include <libxml/xmlschemas.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

namespace 
{

const std::string xsd_filename = "gpx.xsd";

std::string toHex(const std::string & data)
{
    std::ostringstream oss;
    for (unsigned char c : data) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    }
    return oss.str();
}

std::string sha256Hex(const std::string & data)
{
    Poco::Crypto::DigestEngine engine("SHA256");
    engine.update(data);
    return Poco::DigestEngine::digestToHex(engine.digest());
}

std::string hmacSha256(const std::string & key, const std::string & data)
{
    unsigned char result[EVP_MAX_MD_SIZE];
    unsigned int len = 0;

    HMAC(
        EVP_sha256(),
        key.data(), (int)key.size(),
        reinterpret_cast<const unsigned char*>(data.data()),
        data.size(),
        result,
        &len
    );

    return std::string(reinterpret_cast<char*>(result), len);
}

} // namespace 

namespace RGT::Receiver::Utils
{

bool gpxFileValidate(const std::string & data)
{
    Poco::TemporaryFile tempGpx;

    try {
        tempGpx.createFile();
    } 
    catch (...) {
        throw RGT::Devkit::RGTException("Internal server error. Try repeating the request", 
            Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    }

    std::ofstream ofs(tempGpx.path(), std::ios::binary);
    if (not ofs.is_open()) {
        throw RGT::Devkit::RGTException("Internal server error. Try repeating the request", 
            Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    }

    ofs << data;
    ofs.close();

    //

    xmlSchemaParserCtxtPtr parser_ctxt;
    xmlSchemaPtr schema;
    xmlSchemaValidCtxtPtr valid_ctxt;
    xmlDocPtr doc;
    int ret;

    try
    {
        if ((parser_ctxt = xmlSchemaNewParserCtxt(xsd_filename.c_str())) == NULL) {
            throw std::exception();
        }
        if ((schema = xmlSchemaParse(parser_ctxt)) == NULL) {
            throw std::exception();
        }
        if ((valid_ctxt = xmlSchemaNewValidCtxt(schema)) == NULL) {
            throw std::exception();
        }
        if ((doc = xmlReadFile(tempGpx.path().c_str(), NULL, 
            XML_PARSE_NONET | XML_PARSE_NOENT | XML_PARSE_NOBLANKS)) == NULL) 
        {
            throw std::exception();
        }
    }
    catch (...)
    {
        if (parser_ctxt == NULL) {
            xmlSchemaFreeParserCtxt(parser_ctxt);
        }
        if (schema == NULL) {
            xmlSchemaFree(schema);
        }
        if (valid_ctxt == NULL) {
            xmlSchemaFreeValidCtxt(valid_ctxt);
        }
        if (doc == NULL) {
            xmlFreeDoc(doc);
        }

        throw RGT::Devkit::RGTException("An error occurred while validating the received file", 
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    ret = xmlSchemaValidateDoc(valid_ctxt, doc);

    xmlFreeDoc(doc);
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);

    return (ret == 0); 
} 

/// @brief Загружает GPX-файл в S3/MinIO через HTTP PUT
/// @param bucket Название бакета
/// @param objectName Имя файла в бакете
/// @param data Содержимое файла
/// @param endpoint URL сервера MinIO/S3, например "http://127.0.0.1:9000"
/// @param accessKey Access Key
/// @param secretKey Secret Key
/// @throw RGTException при ошибке
void uploadFileToS3(const std::string& bucket, const std::string& objectName, const std::string& data,
    const std::string& endpoint, const std::string& accessKey, const std::string& secretKey)
try
{
    Poco::URI uri(endpoint);
    std::string host = uri.getHost();
    int port = uri.getPort();
    bool https = uri.getScheme() == "https";

    std::string path = "/" + bucket + "/" + objectName;

    Poco::DateTime now;
    std::string amzDate = Poco::DateTimeFormatter::format(now, "%Y%m%dT%H%M%SZ");
    std::string dateStamp = Poco::DateTimeFormatter::format(now, "%Y%m%d");

    std::string payloadHash = sha256Hex(data);

    std::ostringstream canonicalRequest;
    canonicalRequest
        << "PUT\n"
        << path << "\n\n"
        << "host:" << host << "\n"
        << "x-amz-content-sha256:" << payloadHash << "\n"
        << "x-amz-date:" << amzDate << "\n\n"
        << "host;x-amz-content-sha256;x-amz-date\n"
        << payloadHash;

    std::string canonicalHash = sha256Hex(canonicalRequest.str());

    std::string scope = dateStamp + "/us-east-1/s3/aws4_request";

    std::ostringstream stringToSign;
    stringToSign
        << "AWS4-HMAC-SHA256\n"
        << amzDate << "\n"
        << scope << "\n"
        << canonicalHash;

    std::string kDate    = hmacSha256("AWS4" + secretKey, dateStamp);
    std::string kRegion  = hmacSha256(kDate, "us-east-1");
    std::string kService = hmacSha256(kRegion, "s3");
    std::string kSigning = hmacSha256(kService, "aws4_request");

    std::string signature = toHex(hmacSha256(kSigning, stringToSign.str()));

    std::ostringstream auth;
    auth << "AWS4-HMAC-SHA256 "
         << "Credential=" << accessKey << "/" << scope << ", "
         << "SignedHeaders=host;x-amz-content-sha256;x-amz-date, "
         << "Signature=" << signature;

    Poco::Net::HTTPClientSession session(host, port);
    session.setKeepAlive(true);

    Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_PUT, path);
    req.set("Host", host);
    req.set("x-amz-date", amzDate);
    req.set("x-amz-content-sha256", payloadHash);
    req.set("Authorization", auth.str());
    req.setContentLength(data.size());

    std::ostream& os = session.sendRequest(req);
    os << data;

    Poco::Net::HTTPResponse resp;
    std::istream& rs = session.receiveResponse(resp);

    if (resp.getStatus() >= Poco::Net::HTTPResponse::HTTP_MULTIPLE_CHOICES)
    {
        std::ostringstream err;
        Poco::StreamCopier::copyStream(rs, err);
        throw std::runtime_error(
            "S3 upload failed: " + resp.getReason() + " " + err.str());
    }
}
catch (const std::exception & e)
{
    throw RGT::Devkit::RGTException(
        "Failed to upload file to S3/MinIO: " + std::string(e.what()),
        Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR
    );
}

} // namespace RGT::Receiver::Utils
