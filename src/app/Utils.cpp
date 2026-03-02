#include <rgt/devkit/General.h>

#include <Utils.h>

namespace RGT::Receiver::Utils
{

bool saveUserLocation(RedisClientObjectPool & redisPool, const uint64_t & userId, const double & longitude, 
    const double & latitude, const uint64_t & microsecondsSinceEpoch)
{
    static std::string luaScript = RGT::Devkit::readLuaScript("lua_scripts/save_location.lua");

    
}

} // namespace RGT::Receiver::Utils
