#ifndef __UTILS_H__
#define __UTILS_H__

#include <Poco/ObjectPool.h>
#include <Poco/Redis/Client.h>

namespace RGT::Receiver::Utils
{

using RedisClientObjectPool = Poco::ObjectPool<Poco::Redis::Client, Poco::Redis::Client::Ptr>;

bool saveUserLocation(RedisClientObjectPool & redisPool, const uint64_t & userId, const double & longitude, 
    const double & latitude, const uint64_t & microsecondsSinceEpoch);

} // namespace RGT::Receiver::Utils

#endif // __UTILS_H__
