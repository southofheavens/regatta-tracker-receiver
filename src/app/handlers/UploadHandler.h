#ifndef __UPLOAD_HANDLER_H__
#define __UPLOAD_HANDLER_H__

#include <rgt/devkit/HTTPRequestHandler.h>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Util/LayeredConfiguration.h>

namespace RGT::Receiver
{

class UploadHandler : public RGT::Devkit::HTTPRequestHandler 
{
public:
    UploadHandler(Poco::Util::LayeredConfiguration & cfg) : cfg_{cfg}
    {
    }

private:
    virtual void requestPreprocessing(Poco::Net::HTTPServerRequest & request) final;

    virtual std::any extractPayloadFromRequest(Poco::Net::HTTPServerRequest & request) final;

    virtual void requestProcessing(Poco::Net::HTTPServerRequest & request, Poco::Net::HTTPServerResponse & response) final;

private:
    Poco::Util::LayeredConfiguration & cfg_;

    struct Payload
    {
        std::string isoTimestamp;
        double longitude;
        double latitude;
    };
};

} // namespace RGT::Receiver

#endif // __UPLOAD_HANDLER_H__
