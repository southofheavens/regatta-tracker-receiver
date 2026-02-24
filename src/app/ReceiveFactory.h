#ifndef __RECEIVE_FACTORY_H__
#define __RECEIVE_FACTORY_H__

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Exception.h>

#include <handlers/UploadHandler.h>

namespace RGT::Receiver
{

class ReceiveFactory : public Poco::Net::HTTPRequestHandlerFactory 
{
public:
    ReceiveFactory(Poco::Util::LayeredConfiguration & cfg) : cfg_{cfg}
    {
    }

private:
    Poco::Net::HTTPRequestHandler * createRequestHandler(const Poco::Net::HTTPServerRequest & request) final
    {
        if (request.getURI() == "/upload" and request.getMethod() == "POST") {
            return new RGT::Receiver::UploadHandler(cfg_);
        }
        else {
            // return new ErrorHandler;
        }
    }

private:
    Poco::Util::LayeredConfiguration & cfg_;
};

} // namespace RGT::Receiver

#endif // __RECEIVE_FACTORY_H__
