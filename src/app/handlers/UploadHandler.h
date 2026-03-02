#ifndef __UPLOAD_HANDLER_H__
#define __UPLOAD_HANDLER_H__

#include <rgt/devkit/HTTPRequestHandler.h>
#include <rgt/devkit/JWTPayload.h>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/LayeredConfiguration.h>
#include <Poco/ObjectPool.h>
#include <Poco/Redis/Client.h>

namespace RGT::Receiver
{

class UploadHandler : public RGT::Devkit::HTTPRequestHandler 
{
private:
    using RedisClientObjectPool = Poco::ObjectPool<Poco::Redis::Client, Poco::Redis::Client::Ptr>;

public:
    UploadHandler(Poco::Util::LayeredConfiguration & cfg, RedisClientObjectPool & redisPool) 
        : cfg_{cfg}
        , redisPool_{redisPool}
    {
    }

private:
    virtual void requestPreprocessing(Poco::Net::HTTPServerRequest & request) final;

    virtual std::any extractPayloadFromRequest(Poco::Net::HTTPServerRequest & request) final;

    virtual void requestProcessing(Poco::Net::HTTPServerRequest & request, Poco::Net::HTTPServerResponse & response) final;

private:
    struct RequiredPayload
    {
        RGT::Devkit::JWTPayload tokenPayload;

        std::string isoTimestamp;
        double longitude;
        double latitude;
    };

    Poco::Util::LayeredConfiguration & cfg_;
    RedisClientObjectPool            & redisPool_;
};

} // namespace RGT::Receiver

#endif // __UPLOAD_HANDLER_H__
