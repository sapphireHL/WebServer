#ifndef REDISCLIENT_H
#define REDISCLIENT_H

#include <cpp_redis/cpp_redis>

class RedisClient
{
    public:
        RedisClient();
        ~RedisClient();

    void redisHset(const std::string& key, const std::string& filed, const std::string& value);
    std::string redisHget(const std::string& key, const std::string& filed);
    bool redisExist(const std::string& key);
    void redisBgsave();

    private:
        cpp_redis::client redisClient;
};

#endif // REDISCLIENT_H
