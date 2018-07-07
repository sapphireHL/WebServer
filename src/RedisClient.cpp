#include "RedisClient.h"
#include <iostream>

using namespace std;

RedisClient::RedisClient()
{
    //ctor
    redisClient.connect("127.0.0.1", 6379,
                [](const std::string& host, std::size_t port, cpp_redis::client::connect_state status)
                {
                    if(status == cpp_redis::client::connect_state::dropped
                       || status == cpp_redis::client::connect_state::failed){
                        cout << "redis server connect failed from " << host << ":" << port << endl;
                    }
                    else{
                        cout << "redis server connected  from " << host << ":" << port << endl;
                    }
                });
    redisClient.hset("admin", "username", "admin");
    redisClient.hset("admin", "password", "admin");
    redisClient.sync_commit();
}

RedisClient::~RedisClient()
{
    //dtor
    redisClient.disconnect();
}

string RedisClient::redisHget(const string& key, const string& field)
{
    string res;
    redisClient.hget(key, field, [&](cpp_redis::reply& reply){
                        if(reply.is_string()){
                            res = reply.as_string();
                        }
                     });
    redisClient.sync_commit();
    return res;
}

bool RedisClient::redisExist(const string& key)
{
    bool res;
    vector<string> keys(1, key);
    redisClient.exists(keys, [&](cpp_redis::reply& reply){
                            if(reply.is_integer()){
                                res = reply.as_integer();
                            }
                       });
    redisClient.sync_commit();
    return res;
}

void RedisClient::redisHset(const string& key, const string& field, const string& value)
{
    redisClient.hset(key, field, value);
    redisClient.sync_commit();
}

void RedisClient::redisBgsave()
{
    redisClient.bgsave();
    redisClient.sync_commit();
}
