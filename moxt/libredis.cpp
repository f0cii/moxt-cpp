#include "libredis.hpp"
#include <sw/redis++/redis++.h>

using namespace sw::redis;

typedef Redis *RedisPtr;

SEQ_FUNC void seq_test_redis() {
    auto password = getenv("REDIS_PASSWORD");

    ConnectionOptions connection_options;
    connection_options.host = "1.94.26.93";
    connection_options.port = 6379;
    connection_options.password = password;
    connection_options.db = 0;

    // Optional. Timeout before we successfully send request to or receive
    // response from redis. By default, the timeout is 0ms, i.e. never timeout
    // and block until we send or receive successfuly. NOTE: if any command is
    // timed out, we throw a TimeoutError exception.
    connection_options.socket_timeout = std::chrono::milliseconds(3 * 1000);

    // Connect to Redis server with a single connection.
    Redis redis(connection_options);
    // redis.rpush("", "");
    redis.set("test", "test123");
    auto value = redis.get("test");
    if (value) {
        printf("value=%s\n", value.value().c_str());
    } else {
        printf("value is null\n");
    }
}

SEQ_FUNC RedisPtr seq_redis_new(const char *host, size_t hostLen, int port,
                                const char *password, size_t passwordLen,
                                int db, int timeout_ms) {
    auto hostStr = std::string(host, hostLen);
    auto pwdStr = std::string(password, passwordLen);

    ConnectionOptions connection_options;
    connection_options.host = hostStr;
    connection_options.port = port;
    connection_options.password = pwdStr;
    connection_options.db = db;

    // Optional. Timeout before we successfully send request to or receive
    // response from redis. By default, the timeout is 0ms, i.e. never timeout
    // and block until we send or receive successfuly. NOTE: if any command is
    // timed out, we throw a TimeoutError exception.
    connection_options.socket_timeout = std::chrono::milliseconds(timeout_ms);

    return new Redis(connection_options);
}

SEQ_FUNC bool seq_redis_set(RedisPtr redis, const char *key, size_t keyLen,
                            const char *value, size_t valueLen) {
    return redis->set(std::string(key, keyLen), std::string(value, valueLen));
}

SEQ_FUNC bool seq_redis_get(RedisPtr redis, const char *key, size_t keyLen,
                            char *value, size_t *valueLen) {
    auto result = redis->get(std::string(key, keyLen));
    if (result) {
        *valueLen = result->size();
        memcpy(value, result->data(), *valueLen);
        return true;
    }
    return false;
}

SEQ_FUNC bool seq_redis_expire(RedisPtr redis, const char *key, size_t keyLen,
                               int expireTime) {
    return redis->expire(std::string(key, keyLen), expireTime);
}

SEQ_FUNC int64_t seq_redis_rpush(RedisPtr redis, const char *key, size_t keyLen,
                                 const char *value, size_t valueLen) {
    return redis->rpush(std::string(key, keyLen), std::string(value, valueLen));
}

SEQ_FUNC size_t seq_redis_lrange(RedisPtr redis, const char *key, size_t keyLen,
                                 char *values[], size_t *valuesLen,
                                 size_t maxLen) {
    std::vector<OptionalString> result;
    redis->lrange(std::string(key, keyLen), 0, -1, std::back_inserter(result));
    size_t count = 0;
    for (const auto &val : result) {
        if (count >= maxLen)
            break;
        if (val) {
            values[count] = new char[val->size() + 1];
            memcpy(values[count], val->data(), val->size());
            values[count][val->size()] = '\0';
            valuesLen[count] = val->size();
            count++;
        }
    }
    return count;
}

SEQ_FUNC int64_t seq_redis_hset(RedisPtr redis, const char *key, size_t keyLen,
                                const char *field, size_t fieldLen,
                                const char *value, size_t valueLen) {
    return redis->hset(std::string(key, keyLen), std::string(field, fieldLen),
                       std::string(value, valueLen));
}

SEQ_FUNC size_t seq_redis_hgetall(RedisPtr redis, const char *key,
                                  size_t keyLen, char *fields[],
                                  size_t *fieldsLen, char *values[],
                                  size_t *valuesLen, size_t maxLen) {
    std::unordered_map<std::string, std::string> result;
    redis->hgetall(std::string(key, keyLen),
                   std::inserter(result, result.begin()));
    size_t count = 0;
    for (const auto &pair : result) {
        if (count >= maxLen)
            break;
        fields[count] = new char[pair.first.size() + 1];
        memcpy(fields[count], pair.first.data(), pair.first.size());
        fields[count][pair.first.size()] = '\0';
        fieldsLen[count] = pair.first.size();

        values[count] = new char[pair.second.size() + 1];
        memcpy(values[count], pair.second.data(), pair.second.size());
        values[count][pair.second.size()] = '\0';
        valuesLen[count] = pair.second.size();
        count++;
    }
    return count;
}

SEQ_FUNC int64_t seq_redis_sadd(RedisPtr redis, const char *key, size_t keyLen,
                                const char *member, size_t memberLen) {
    return redis->sadd(std::string(key, keyLen),
                       std::string(member, memberLen));
}

SEQ_FUNC size_t seq_redis_smembers(RedisPtr redis, const char *key,
                                   size_t keyLen, char *members[],
                                   size_t *membersLen, size_t maxLen) {
    std::unordered_set<std::string> result;
    redis->smembers(std::string(key, keyLen),
                    std::inserter(result, result.begin()));
    size_t count = 0;
    for (const auto &member : result) {
        if (count >= maxLen)
            break;
        members[count] = new char[member.size() + 1];
        memcpy(members[count], member.data(), member.size());
        members[count][member.size()] = '\0';
        membersLen[count] = member.size();
        count++;
    }
    return count;
}

SEQ_FUNC int64_t seq_redis_zadd(RedisPtr redis, const char *key, size_t keyLen,
                                const char *member, size_t memberLen,
                                double score) {
    return redis->zadd(std::string(key, keyLen), std::string(member, memberLen),
                       score);
}

SEQ_FUNC void seq_redis_free(RedisPtr redis) { delete redis; }