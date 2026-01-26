#ifndef __RECEIVE_SERVER_H__
#define __RECEIVE_SERVER_H__

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Exception.h>

#include <ReceiveFactory.h>

namespace FQW::Receiver
{

class ReceiveServer : public Poco::Util::ServerApplication
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
                new ReceiveFactory, 
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

} // namespace FQW::Receiver

#endif // __RECEIVE_SERVER_H__
