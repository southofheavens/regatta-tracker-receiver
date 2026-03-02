#ifndef __RECEIVE_SERVER_H__
#define __RECEIVE_SERVER_H__

#include <rgt/devkit/Connections.h>

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
        ServerApplication::initialize(self);

        const Poco::Util::LayeredConfiguration & cfg = ReceiveServer::config();

        redisPool_ = RGT::Devkit::connectToRedis(cfg.getString("redis.host"), cfg.getString("redis.port"),
            cfg.getUInt16("redis.min_sessions"), cfg.getUInt16("redis.max_sessions"));
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
            new ReceiveFactory(cfg, *redisPool_), 
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

private:
    using RedisClientObjectPool = Poco::ObjectPool<Poco::Redis::Client, Poco::Redis::Client::Ptr>;

    std::unique_ptr<RedisClientObjectPool> redisPool_;
};

} // namespace RGT::Receiver

#endif // __RECEIVE_SERVER_H__
