-- KEYS[1]: "user_participation:{user_id}"
-- ARGV[1]: longitude, ARGV[2]: latitude, ARGV[3]: microseconds

local key = KEYS[1]
local value = ARGV[1] .. ";" .. ARGV[2] .. ";" .. ARGV[3]

if redis.call("EXISTS", key) == 0 then
    return 0
else
    redis.call("RPUSH", key, value)
    return 1
end
