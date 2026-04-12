#include <RGT/Devkit/General.h>
#include <RGT/Devkit/RGTException.h>

#include <Utils.h>

#include <Poco/Redis/PoolableConnectionFactory.h>

namespace RGT::Receiver
{

bool saveUserLocation(RedisClientObjectPool & redisPool, const uint64_t & userId, const double & longitude, 
    const double & latitude, const uint64_t & microsecondsSinceEpoch)
{
    static std::string luaScript = RGT::Devkit::readLuaScript("lua_scripts/save_location.lua");

    Poco::Redis::Array cmd;
    cmd << "EVAL"
        << luaScript
        << "1"
        << std::format("user_participation:{}", userId)
        << std::to_string(longitude)
        << std::to_string(latitude)
        << std::to_string(microsecondsSinceEpoch);

    try
    {
        Poco::Redis::PooledConnection pc(redisPool, 500);
        Poco::Redis::Client::Ptr redisClient = static_cast<Poco::Redis::Client::Ptr>(pc);
        if (redisClient == nullptr)
        {
            // TODO лог
            throw std::exception{};
        }

        Poco::Int64 resultOfCmd = redisClient->execute<Poco::Int64>(cmd); 
        return resultOfCmd != 0 ? true : false;
    }
    catch (...) {
        throw RGT::Devkit::RGTException("Internal server error. Try repeating the request.",
            Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    }
}

} // namespace RGT::Receiver
