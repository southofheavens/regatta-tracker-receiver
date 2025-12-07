#include <iostream>
#include <fstream>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/PartHandler.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/StreamCopier.h>
#include <Poco/Exception.h>

class GPXPartHandler : public Poco::Net::PartHandler 
{
public:
    GPXPartHandler() = default;

    void handlePart(const Poco::Net::MessageHeader& header,
        std::istream& stream) override
    {
        std::string disp;
        if (header.has("Content-Disposition")) {
            disp = header["Content-Disposition"];
        }

        std::string filename = "upload.gpx";
        auto pos = disp.find("filename=");
        if (pos != std::string::npos) 
        {
            filename = disp.substr(pos + 10);
            if (!filename.empty() && filename.back() == '"') {
                filename.pop_back();
            }
            if (!filename.empty() && filename.front() == '"') {
                filename.erase(0, 1);
            }
        }

        std::cout << "Receiving file: " << filename << std::endl;

        std::ofstream out(filename, std::ios::binary);
        Poco::StreamCopier::copyStream(stream, out);

        std::cout << "File saved.\n";
    }
};

class UploadHandler : public Poco::Net::HTTPRequestHandler 
{
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override
    {
        try 
        {
            if (request.getMethod() != Poco::Net::HTTPRequest::HTTP_POST) 
            {
                response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
                response.send() << "Use POST /upload";
                return;
            }

            GPXPartHandler partHandler;
            Poco::Net::HTMLForm form(request, request.stream(), partHandler);

            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.send() << "GPX uploaded successfully";
        }
        catch (const Poco::Exception& ex) 
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.send() << "Error: " << ex.displayText();
        }
    }
};

class GPXFactory : public Poco::Net::HTTPRequestHandlerFactory 
{
public:
    Poco::Net::HTTPRequestHandler* createRequestHandler(
        const Poco::Net::HTTPServerRequest& request) override
    {
        if (request.getURI() == "/upload") {
            return new UploadHandler;
        }

        return nullptr;
    }
};

class GPXServer : public Poco::Util::ServerApplication
{
protected:
    void initialize(Application& self) override
    {
        ServerApplication::initialize(self);
    }

    void uninitialize() override
    {
        ServerApplication::uninitialize();
    }

    int main(const std::vector<std::string>&) override
    {
        try
        {
            Poco::Net::ServerSocket svs(8081);
            
            Poco::Net::HTTPServer srv
            (
                new GPXFactory, 
                svs, 
                new Poco::Net::HTTPServerParams
            );

            srv.start();
            std::cout << "Сервер запущен на порту 8081..." << std::endl;
            
            waitForTerminationRequest();
            
            srv.stop();
            
            return Application::EXIT_OK;
        }
        catch (const Poco::Exception& e) {
            std::cerr << e.displayText() << '\n';
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
};

POCO_SERVER_MAIN(GPXServer)
