#include <handlers/UploadHandler.h>
#include <Utils.h>

#include <sstream>

#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/SAX/InputSource.h>
#include <Poco/SAX/SAXException.h>

#include <rgt/devkit/General.h>
#include <rgt/devkit/Tokens.h>

namespace
{ 

// Максимальный размер gpx файла - 10 megabytes
constexpr uint64_t max_file_size = 1024 * 1024 * 10;
// Размер буфера при копировании из istream в ostringstream
constexpr uint16_t buffer_size = 8096;

} // namespace

namespace RGT::Receiver
{

void UploadHandler::handleRequest(Poco::Net::HTTPServerRequest & request,
    Poco::Net::HTTPServerResponse & response) 
try
{
    if (request.getContentType().find("application/gpx+xml") == std::string::npos) {
        throw RGT::Devkit::RGTException("Content-Type must be application/gpx+xml",
            Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
    }

    std::string accessToken = RGT::Devkit::Tokens::extractTokenFromRequest(request);

    RGT::Devkit::Tokens::Payload payload = RGT::Devkit::Tokens::extractPayload(accessToken);

    std::istream & inputStream = request.stream();
    
    // Проверка размера файла
    std::ostringstream oss;
    uint64_t currentFileSize = 0;
    while (inputStream) 
    {
        char buffer[buffer_size];
        inputStream.read(buffer, buffer_size);
        currentFileSize += inputStream.gcount();
        if (currentFileSize > max_file_size) {
            throw RGT::Devkit::RGTException("Max file size is 10 megabyte", 
                Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
        }
        oss.write(buffer, inputStream.gcount());
    }

    // Валидация принятого от пользователя файла
    if (not RGT::Receiver::Utils::gpxFileValidate(oss.str())) {
        throw RGT::Devkit::RGTException("The file must match the schema GPX 1.1 from www.topografix.com", 
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    // Отправляем файл в S3 хранилище
    RGT::Receiver::Utils::uploadFileToS3(
        "gpx-files",                
        "track123.gpx",             
        oss.str(),                  
        "http://127.0.0.1:9000",    
        "minioadmin",               
        "minioadmin"                
    );
    
    RGT::Devkit::sendJsonResponse(response, "OK", "OK");
}
catch (RGT::Devkit::RGTException & e)
{
    response.setStatusAndReason(e.status());
    RGT::Devkit::sendJsonResponse(response, "error", e.what());
}
catch (...)
{
    response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    RGT::Devkit::sendJsonResponse(response, "error", "Internal server error. Try repeating the request");
}

} // namespace RGT::Receiver
