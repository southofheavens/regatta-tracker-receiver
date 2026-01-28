#ifndef __UPLOAD_HANDLER_H__
#define __UPLOAD_HANDLER_H__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTMLForm.h>

#include <ReceivePartHandler.h>

namespace FQW::Receiver
{

class UploadHandler : public Poco::Net::HTTPRequestHandler 
{
public:
    UploadHandler() = default;

    void handleRequest(Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) final;
};

} // namespace FQW::Receiver

#endif // __UPLOAD_HANDLER_H__
