#include <handlers/UploadHandler.h>
#include <rgt/devkit/Tokens.h>
#include <rgt/devkit/General.h>
#include <rgt/devkit/RGTException.h>
#include <rgt/devkit/RequestProcessing.h>
#include <Poco/DateTime.h>
#include <Poco/Net/HTTPResponse.h>

namespace RGT::Receiver
{

void UploadHandler::requestPreprocessing(Poco::Net::HTTPServerRequest & request)
{
    RGT::Devkit::checkContentLength(request, cfg_.getUInt16("max_request_body_size"));
    RGT::Devkit::checkContentLengthIsNull(request);
    RGT::Devkit::checkContentType(request, "application/json");
}

std::any UploadHandler::extractPayloadFromRequest(Poco::Net::HTTPServerRequest & request)
{
    Poco::JSON::Object::Ptr json = RGT::Devkit::extractJsonObjectFromRequest(request);

    Poco::Dynamic::Var dvTimeIso = RGT::Devkit::extractValueFromJson(json, "time");
    std::string timeIso;
    try {
        timeIso = dvTimeIso.extract<std::string>();
    }
    catch(...) {
        throw RGT::Devkit::RGTException("The value for the key \"time\" must be a string in ISO 8601 format",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    Poco::Dynamic::Var dvLongitude = RGT::Devkit::extractValueFromJson(json, "longitude");
    double longitude;
    try {
        longitude = dvLongitude.extract<double>();
    }
    catch(...) {
        throw RGT::Devkit::RGTException("The value for the key \"longitude\" must be a floating-point number",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    Poco::Dynamic::Var dvLatitude = RGT::Devkit::extractValueFromJson(json, "latitude");
    double latitude;
    try {
        latitude = dvLatitude.extract<double>();
    }
    catch(...) {
        throw RGT::Devkit::RGTException("The value for the key \"latitude\" must be a floating-point number",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    return Payload
    {
        .isoTimestamp = timeIso,
        .longitude = longitude,
        .latitude = latitude
    };
}

void UploadHandler::requestProcessing(Poco::Net::HTTPServerRequest & request, Poco::Net::HTTPServerResponse & response)
{
    Payload payload = std::any_cast<Payload>(payload_);
}

} // namespace RGT::Receiver
