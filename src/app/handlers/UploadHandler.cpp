#include <handlers/UploadHandler.h>
#include <rgt/devkit/Tokens.h>
#include <rgt/devkit/General.h>

namespace 
{

/// @brief Примитивная валидация запроса
/// @param req ссылка на запрос
/// @param cfg ссылка на конфиг
/// @throw RGT::Devkit::RGTException если запрос некорректен
void primitiveRequestValidate(Poco::Net::HTTPServerRequest & req, Poco::Util::LayeredConfiguration & cfg)
{
    if (req.getContentLength() == Poco::Net::HTTPMessage::UNKNOWN_CONTENT_LENGTH) {
        throw RGT::Devkit::RGTException("Content length is unknown", 
            Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
    }

    if (req.getContentLength64() > cfg.getUInt16("max_request_body_size")) {
        throw RGT::Devkit::RGTException("Content size must not exceed 1 kilobyte",
            Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    }

    if (req.getContentLength() == 0) {
        throw RGT::Devkit::RGTException("Content length is zero", 
            Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
    }

    if (req.getContentType().find("application/json") == std::string::npos) {
        throw RGT::Devkit::RGTException("Content-Type must be application/json", 
            Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
    }
}

} // namespace

namespace RGT::Receiver
{

void UploadHandler::handleRequest(Poco::Net::HTTPServerRequest & request, 
    Poco::Net::HTTPServerResponse & response)
{
    // Проводим примитивную валидацию запроса 
    primitiveRequestValidate(request, cfg_);

}

} // namespace RGT::Receiver
