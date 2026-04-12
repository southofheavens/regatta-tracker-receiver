#include <Handlers/UploadHandler.h>
#include <RGT/Devkit/RGTException.h>
#include <Poco/DateTime.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/DateTimeParser.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Utils.h>

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

namespace RGT::Receiver::Handlers
{

void UploadHandler::requestPreprocessing(Poco::Net::HTTPServerRequest & request)
{
    HTTPRequestHandler::checkContentLength(request, cfg_.getUInt16("max_request_body_size"));
    HTTPRequestHandler::checkContentLengthIsNull(request);
    HTTPRequestHandler::checkContentType(request, "application/json");
}

void UploadHandler::extractPayloadFromRequest(Poco::Net::HTTPServerRequest & request)
{
    std::string accessToken = HTTPRequestHandler::extractTokenFromRequest(request);
    RGT::Devkit::JWTPayload jwtPayload = HTTPRequestHandler::extractPayload(accessToken);

    Poco::JSON::Object::Ptr json = HTTPRequestHandler::extractJsonObjectFromRequest(request);

    Poco::Dynamic::Var dvTimeIso = HTTPRequestHandler::extractValueFromJson(json, "time");
    std::string timeIso;
    try {
        timeIso = dvTimeIso.extract<std::string>();
    }
    catch(...) {
        throw RGT::Devkit::RGTException("The value for the key \"time\" must be a string",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    Poco::Dynamic::Var dvLongitude = HTTPRequestHandler::extractValueFromJson(json, "longitude");
    double longitude;
    try {
        longitude = dvLongitude.extract<double>();
    }
    catch(...) {
        throw RGT::Devkit::RGTException("The value for the key \"longitude\" must be a floating-point number",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    Poco::Dynamic::Var dvLatitude = HTTPRequestHandler::extractValueFromJson(json, "latitude");
    double latitude;
    try {
        latitude = dvLatitude.extract<double>();
    }
    catch(...) {
        throw RGT::Devkit::RGTException("The value for the key \"latitude\" must be a floating-point number",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    requestPayload_.tokenPayload = jwtPayload;
    requestPayload_.isoTimestamp = timeIso;
    requestPayload_.longitude = longitude;
    requestPayload_.latitude = latitude;
}

void UploadHandler::requestProcessing(Poco::Net::HTTPServerRequest & request, Poco::Net::HTTPServerResponse & response)
{
    if (not (requestPayload_.tokenPayload.role == "Participant")) {
        throw RGT::Devkit::RGTException("Only participant can upload location",
            Poco::Net::HTTPResponse::HTTP_FORBIDDEN);
    }

    Poco::DateTime dt;
    int tzd = 0;
    if (not Poco::DateTimeParser::tryParse(Poco::DateTimeFormat::ISO8601_FRAC_FORMAT, requestPayload_.isoTimestamp, dt, tzd))
    {
        throw RGT::Devkit::RGTException("Time must be presented in ISO8601 format with fractional seconds. Examples: "
            "2005-01-01T12:00:00.000000+01:00, 2005-01-01T11:00:00.000000Z",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }
    dt.makeUTC(tzd);
    Poco::Timestamp::TimeVal microseconds = dt.timestamp().epochMicroseconds();

    if (not isLongitudeCorrect(requestPayload_.longitude)) {
        throw RGT::Devkit::RGTException("Longitude can take a value from -180 to 180",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    if (not isLatitudeCorrect(requestPayload_.latitude)) {
        throw RGT::Devkit::RGTException("Latitude can take a value from -90 to 90",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    bool saveResult = RGT::Receiver::saveUserLocation(redisPool_, requestPayload_.tokenPayload.sub,
        requestPayload_.longitude, requestPayload_.latitude, microseconds);

    if (saveResult) {
        HTTPRequestHandler::sendJsonResponse(response, "OK", "OK");
    }
    else 
    {
        HTTPRequestHandler::sendJsonResponse(response, "OK", "The race either didn't start or ended. "
            "The received coordinates were not saved");
    }
}

} // namespace RGT::Receiver::Handlers
