#ifndef __UPLOAD_HANDLER_H__
#define __UPLOAD_HANDLER_H__

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTMLForm.h>

#include <ReceivePartHandler.h>

namespace FQW::Receiver::Handlers
{

class UploadHandler : public Poco::Net::HTTPRequestHandler 
{
public:
    UploadHandler() = default;

    void handleRequest(Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) final
    {
        try 
        {
            Receiver::Handlers::ReceivePartHandler partHandler;
            Poco::Net::HTMLForm form(request, request.stream(), partHandler);

            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.send() << "Receive uploaded successfully";
        }
        catch (const Poco::Exception& ex) 
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.send() << "Error: " << ex.displayText();
        }
    }
};

} // namespace FQW::Receiver::Handlers

#endif // __UPLOAD_HANDLER_H__
