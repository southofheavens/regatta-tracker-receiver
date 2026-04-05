#pragma once

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Exception.h>

#include <Handlers/UploadHandler.h>

namespace RGT::Receiver
{

class ReceiveFactory : public Poco::Net::HTTPRequestHandlerFactory 
{
private:
    using RedisClientObjectPool = Poco::ObjectPool<Poco::Redis::Client, Poco::Redis::Client::Ptr>;

public:
    ReceiveFactory(Poco::Util::LayeredConfiguration & cfg, RedisClientObjectPool & redisPool) 
        : cfg_{cfg}
        , redisPool_{redisPool}
    {
    }

private:
    Poco::Net::HTTPRequestHandler * createRequestHandler(const Poco::Net::HTTPServerRequest & request) final
    {
        if (request.getURI() == "/upload" and request.getMethod() == "POST") {
            return new RGT::Receiver::Handlers::UploadHandler(cfg_, redisPool_);
        }
        else {
            // return new ErrorHandler;
        }
    }

private:
    Poco::Util::LayeredConfiguration & cfg_;
    RedisClientObjectPool            & redisPool_;
};

} // namespace RGT::Receiver
