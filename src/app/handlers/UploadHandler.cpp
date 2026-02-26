#include <handlers/UploadHandler.h>
#include <rgt/devkit/Tokens.h>
#include <rgt/devkit/General.h>
#include <rgt/devkit/RGTException.h>
#include <rgt/devkit/RequestProcessing.h>
#include <Poco/DateTime.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/DateTimeParser.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>

namespace
{

bool isLongitudeCorrect(const double & longitude) noexcept
{
    if (longitude < -180 or longitude > 180) {
        return false;
    }
    return true;
}

bool isLatitudeCorrect(const double & latitude) noexcept
{
    if (latitude < -90 or latitude > 90) {
        return false;
    }
    return true;
}

} // namespace

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
        throw RGT::Devkit::RGTException("The value for the key \"time\" must be a string",
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

    Poco::DateTime dt;
    int tzd = 0;
    if (not Poco::DateTimeParser::tryParse(Poco::DateTimeFormat::ISO8601_FRAC_FORMAT, payload.isoTimestamp, dt, tzd))
    {
        throw RGT::Devkit::RGTException("Time must be presented in ISO8601 format with fractional seconds. Examples: "
            "2005-01-01T12:00:00.000000+01:00, 2005-01-01T11:00:00.000000Z",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }
    Poco::Timestamp::TimeVal microseconds = dt.timestamp().epochMicroseconds();

    if (not isLongitudeCorrect(payload.longitude)) {
        throw RGT::Devkit::RGTException("Longitude can take a value from -180 to 180",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    if (not isLatitudeCorrect(payload.latitude)) {
        throw RGT::Devkit::RGTException("Latitude can take a value from -90 to 90",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    
}

} // namespace RGT::Receiver
