#ifndef __RECEIVE_SERVER_H__
#define __RECEIVE_SERVER_H__

#include <iostream>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Exception.h>

#include <ReceiveFactory.h>

namespace RGT::Receiver
{

class ReceiveServer : public Poco::Util::ServerApplication
{
protected:
    void initialize(Application & self) final
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    void uninitialize() final
    {
        ServerApplication::uninitialize();
    }

    int main(const std::vector<std::string>&) final
    try
    {
        Poco::Util::LayeredConfiguration & cfg = ReceiveServer::config();

        Poco::Net::ServerSocket svs(cfg.getUInt16("server.port"));
        
        Poco::Net::HTTPServer srv
        (
            new ReceiveFactory(cfg), 
            svs, 
            new Poco::Net::HTTPServerParams
        );

        srv.start();
        
        waitForTerminationRequest();
        
        srv.stop();
        
        return Application::EXIT_OK;
    }
    catch (const Poco::Exception & e) {
        std::cerr << e.displayText() << '\n';
    }
    catch (const std::exception & e) {
        std::cerr << e.what() << '\n';
    }
};

} // namespace RGT::Receiver

#endif // __RECEIVE_SERVER_H__
