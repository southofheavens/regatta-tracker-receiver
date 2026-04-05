#pragma once

#include <RGT/Devkit/Subsystems/RedisSubsystem.h>

#include <iostream>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Exception.h>
#include <Poco/ObjectPool.h>
#include <Poco/Redis/Client.h>

#include <ReceiveFactory.h>

namespace RGT::Receiver
{

class ReceiveServer : public Poco::Util::ServerApplication
{
private:
    void initialize(Application & self) final
    {
        loadConfiguration();

        Poco::Util::Application::addSubsystem(new RGT::Devkit::Subsystems::RedisSubsystem());

        ServerApplication::initialize(self);
    }

    void uninitialize() final
    { ServerApplication::uninitialize(); }

    int main(const std::vector<std::string>&) final
    try
    {
        Poco::Util::LayeredConfiguration & cfg = ReceiveServer::config();

        Poco::Net::ServerSocket svs(cfg.getUInt16("server.port"));

        auto & redisSubsystem = Poco::Util::Application::getSubsystem<Devkit::Subsystems::RedisSubsystem>();
        
        Poco::Net::HTTPServer srv
        (
            new ReceiveFactory(cfg, redisSubsystem.getPool()), 
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
