#pragma once

#include <Poco/ObjectPool.h>
#include <Poco/Redis/Client.h>

#include <RGT/Devkit/Types.h>

namespace RGT::Receiver
{

using RedisClientObjectPool = Poco::ObjectPool<Poco::Redis::Client, Poco::Redis::Client::Ptr>;

bool saveUserLocation(RedisClientObjectPool & redisPool, const Devkit::UserId & userId, const double & longitude, 
    const double & latitude, const uint64_t & microsecondsSinceEpoch);

} // namespace RGT::Receiver
