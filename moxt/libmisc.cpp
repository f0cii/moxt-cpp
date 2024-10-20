#include "libmisc.hpp"
#include "common.hpp"
#include "moxt/utils/floatutil.hpp"
#include "libc.hpp"
#include "photon/common/callback.h"
#include "simdjson.h"
#include <absl/strings/ascii.h>
#include <absl/strings/escaping.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/string_view.h>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <nanobench.h>
#include <photon/common/identity-pool.h>
#include <string>
#include <tuple>

#include <std23/function_ref.h>

#include "moxt/app.hpp"
#include "moxt/httpx/asio_ioc.hpp"
#include "moxt/utils/idgen.hpp"
#include <csignal>
#include <execinfo.h>
#include <iostream>

#include "moxt/utils/c_rate_limiter.hpp"
#include "moxt/utils/cast.hpp"
#include "moxt/utils/snowflake.hpp"
#include <openssl/err.h>
#include <openssl/opensslv.h>
#include <parallel_hashmap/phmap.h>
#include <snmalloc/snmalloc.h>
#include <utility>

using phmap::flat_hash_map;

static int _constructor(int **ptr) {
    // printf("_constructor 10\n");
    *ptr = new int(100);
    // printf("_constructor 11\n");
    return 0;
}

static int _destructor(int *ptr) {
    delete ptr;
    return 0;
}

static int asdf = 323;

IdentityPool<int, 100> *new_pool() {
    // Callback<int **> _ctor1(_constructor);
    // Callback<int *> _dtor1(_destructor);

    // int asdf = 323;

    // auto _ctor = [&](int **ptr) -> int {
    //     *ptr = new int(asdf++);
    //     return 0;
    // };
    // auto _dtor = [&](int *ptr) -> int {
    //     delete ptr;
    //     return 0;
    // };

    auto _ctor = [](int **ptr) -> int {
        *ptr = new int(asdf++);
        return 0;
    };
    auto _dtor = [](int *ptr) -> int {
        delete ptr;
        return 0;
    };

    auto *pool = new IdentityPool<int, 100>(_ctor, _dtor);
    pool->enable_autoscale();
    // auto *pool = new IdentityPool<int, 100>(_constructor, _destructor);
    return pool;
}

// ondemand::parser

typedef simdjson::ondemand::parser ondemand_parser;

static int _ondemand_parser_constructor(ondemand_parser **ptr) {
    // printf("_constructor 10\n");
    *ptr = new ondemand_parser(1024 * 640);
    // printf("_constructor 11\n");
    return 0;
}

static int _ondemand_parser_destructor(ondemand_parser *ptr) {
    delete ptr;
    return 0;
}

IdentityPool<ondemand_parser, 100> *new_ondemand_parser_pool() {
    auto _ctor = [](ondemand_parser **ptr) -> int {
        *ptr = new ondemand_parser(1024 * 640);
        return 0;
    };
    auto _dtor = [](ondemand_parser *ptr) -> int {
        delete ptr;
        return 0;
    };

    auto *pool = new IdentityPool<ondemand_parser, 100>(_ctor, _dtor);
    pool->enable_autoscale();
    // auto *pool = new IdentityPool<int, 100>(_constructor, _destructor);
    return pool;
}

SEQ_FUNC void test_identity_pool() {
    auto *pool = new_pool();

    std::vector<int *> vec;
    for (int i = 0; i < 10; i++) {
        auto x = pool->get();
        vec.emplace_back(x);
    }

    for (auto x : vec) {
        pool->put(x);
    }

    ankerl::nanobench::Bench().minEpochIterations(10000000).run("pool", [&] {
        auto x = pool->get();
        pool->put(x);
        ankerl::nanobench::doNotOptimizeAway(x);
    });

    ankerl::nanobench::Bench().minEpochIterations(10000000).run("raw", [&] {
        int *x;
        _constructor(&x);
        _destructor(x);
        ankerl::nanobench::doNotOptimizeAway(x);
    });

    // delete_identity_pool(&pool);
}

SEQ_FUNC void test_ondemand_parser_pool() {
    auto *pool = new_ondemand_parser_pool();

    std::vector<ondemand_parser *> vec;
    for (int i = 0; i < 10; i++) {
        auto x = pool->get();
        vec.emplace_back(x);
    }

    for (auto x : vec) {
        pool->put(x);
    }

    ankerl::nanobench::Bench().minEpochIterations(10000000).run("od_pool", [&] {
        auto x = pool->get();
        pool->put(x);
        ankerl::nanobench::doNotOptimizeAway(x);
    });

    ankerl::nanobench::Bench().minEpochIterations(10000000).run("od_raw", [&] {
        ondemand_parser *x;
        _ondemand_parser_constructor(&x);
        _ondemand_parser_destructor(x);
        ankerl::nanobench::doNotOptimizeAway(x);
    });

    // delete_identity_pool(&pool);
}

using namespace snmalloc;

using std23::nontype;

void handler(int sig) {
    void *array[10];
    int size;

    // 获取堆栈中的地址
    size = backtrace(array, 10);

    // 打印堆栈信息
    std::cerr << "Error: signal " << sig << ":\n";
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

SEQ_FUNC int seq_ct_init() {
    // 在主线程的初始化代码中调用
    SSL_load_error_strings();
    SSL_library_init();

    // 设置信号处理程序
    // signal(SIGSEGV, handler);

    app_init();

    return 0;
}

SEQ_FUNC size_t seq_nanoid(char *result) {
    auto id = IDGen::getInstance().id();
    auto n = id.length();
    memcpy(result, id.c_str(), n);
    result[n] = '\0';
    return n;
}

Snowflake &GetSnowflakeInstance() {
    static Snowflake inst = [] {
        Snowflake::initWorkerId();
        return Snowflake();
    }();
    return inst;
}

SEQ_FUNC int64_t seq_snowflake_id() { return GetSnowflakeInstance().getId(); }

// 存储全局对象地址值
static std::unordered_map<int64_t, int64_t> globalIntMap;

SEQ_FUNC void seq_set_global_int(int64_t key, int64_t ptr) {
    globalIntMap[key] = ptr;
}

SEQ_FUNC int64_t seq_get_global_int(int64_t key) {
    auto it = globalIntMap.find(key);
    if (it != globalIntMap.end()) {
        return it->second;
    } else {
        return 0;
    }
}

// 存储全局对象地址值
static std::unordered_map<int64_t, std::string> globalStringMap;

SEQ_FUNC void seq_set_global_string(int64_t key, const char *str,
                                    size_t strLen) {
    globalStringMap[key] = std::string(str, strLen);
}

SEQ_FUNC const char *seq_get_global_string(int64_t key, size_t *strLen) {
    auto it = globalStringMap.find(key);
    if (it != globalStringMap.end()) {
        *strLen = it->second.length();
        return it->second.c_str();
    } else {
        *strLen = 0;
        return nullptr; // 返回 nullptr 表示未找到对应的字符串
    }
}

SEQ_FUNC std::map<std::string, std::string> *seq_ssmap_new() {
    return mem_new<std::map<std::string, std::string>>();
}

SEQ_FUNC void seq_ssmap_free(std::map<std::string, std::string> *p) {
    mem_delete(p);
}

SEQ_FUNC bool seq_ssmap_set(std::map<std::string, std::string> *p,
                            const char *name, const char *value) {
    (*p)[name] = value;
    return true;
}

SEQ_FUNC const char *seq_ssmap_get(std::map<std::string, std::string> *p,
                                   const char *name, size_t *n) {
    try {
        std::string &value = (*p)[name];
        *n = value.length();
        return value.c_str();
    } catch (const std::out_of_range &e) {
        *n = 0;
        return "";
    }
}

SEQ_FUNC size_t seq_ssmap_size(std::map<std::string, std::string> *p) {
    return p->size();
}

std::string ByteArrayToHex(const uint8_t *bytes, size_t size) {
    std::string hex_string;
    for (size_t i = 0; i < size; i++) {
        absl::StrAppend(&hex_string, absl::Hex(bytes[i]));
    }
    return hex_string;
}

SEQ_FUNC size_t seq_hmac_sha256(const char *secretKey, size_t secretKey_len,
                                const char *message, size_t message_len,
                                uint8_t *result) {
    unsigned char *digest;
    unsigned int digestLen;

    // SHA256哈希运算
    const EVP_MD *md = EVP_sha256();

    // 初始化HMAC上下文
    HMAC_CTX *hmacCtx = HMAC_CTX_new();
    HMAC_Init_ex(hmacCtx, secretKey, (int)secretKey_len, md, nullptr);

    // 计算HMAC
    HMAC_Update(hmacCtx, (unsigned char *)message, message_len);

    // 获取结果
    digest = (unsigned char *)malloc(EVP_MAX_MD_SIZE);
    HMAC_Final(hmacCtx, digest, &digestLen);
    HMAC_CTX_free(hmacCtx);

    // digest保存了结果,digestLen是长度
    memcpy(result, digest, digestLen);

    // 释放内存
    free(digest);

    return digestLen;
}

SEQ_FUNC size_t seq_base64_encode(uint8_t *input, size_t n, uint8_t *result) {
    std::string_view text((const char *)input, n);

    std::string output;
    absl::Base64Escape(text, &output);

    auto length = output.length();

    memcpy(result, output.c_str(), length);
    memset(result + length, 0, 1);

    return length;
}

SEQ_FUNC size_t seq_hex_encode(uint8_t *input, size_t n, uint8_t *result) {
    std::string_view text((const char *)input, n);

    std::string output = absl::BytesToHexString(text);

    auto length = output.length();

    memcpy(result, output.c_str(), length);
    memset(result + length, 0, 1);

    return length;
}

SEQ_FUNC void seq_test_hmac_sha256(const char *secretKey, size_t secretKey_len,
                                   const char *message, size_t message_len) {
    char buf[64];
    auto n = seq_hmac_sha256(secretKey, secretKey_len, message, message_len,
                             (uint8_t *)buf);
    printf("n=%ld\n", n);
    auto s = ByteArrayToHex((const uint8_t *)buf, n);
    printf("hex=%s\n", s.c_str());
}

SEQ_FUNC int64_t seq_strtoi(const char *s, size_t s_len) {
    return strtoi(std::string_view(s, s_len));
}

SEQ_FUNC double seq_strtod(const char *s, size_t s_len) {
    return strtod(std::string_view(s, s_len));
}

SEQ_FUNC void seq_test_spdlog() {
    // spdlog::set_level(spdlog::level::debug);
    // spdlog::info("Welcome to spdlog!");
    // spdlog::error("Some error message with arg: {}", 1);
    // std::string hello = "hello";
    // std::string_view sv = "hello";
    // spdlog::error("Some error message with arg: {}, {}", hello, sv);
    // spdlog::info(hello);
}

static flat_hash_map<int64_t, std::string *> readableStringCache;
static std::atomic<int64_t> readableStringCacheKey = 0;

SEQ_FUNC int64_t seq_get_next_cache_key() {
    return readableStringCacheKey.fetch_add(1);
}

SEQ_FUNC const char *seq_set_string_in_cache(int64_t key, const char *value,
                                             size_t valueLen) {
    auto it = readableStringCache.find(key);
    if (it != readableStringCache.end()) {
        delete it->second;
        it->second = new std::string(value, valueLen);
    } else {
        readableStringCache.emplace(key, new std::string(value, valueLen));
    }
    return readableStringCache[key]->c_str();
}

SEQ_FUNC const char *seq_get_string_in_cache(int64_t key, size_t *resultLen) {
    auto it = readableStringCache.find(key);
    if (it != readableStringCache.end()) {
        *resultLen = it->second->size();
        return it->second->c_str();
    } else {
        *resultLen = 0;
        return nullptr;
    }
}

SEQ_FUNC bool seq_free_string_in_cache(int64_t key) {
    auto it = readableStringCache.find(key);
    if (it != readableStringCache.end()) {
        delete it->second;
        readableStringCache.erase(it);
        return true;
    }
    return false;
}

// CRateLimiter
SEQ_FUNC void *seq_new_crate_limiter(int maxCount, uint64_t windowSize) {
    return new CRateLimiter(maxCount, windowSize);
}

SEQ_FUNC bool seq_crate_limiter_allow_and_record_request(void *ptr) {
    CRateLimiter *p = static_cast<CRateLimiter *>(ptr);
    return p->allowAndRecordRequest();
}

SEQ_FUNC void seq_delete_crate_limiter(void *ptr) {
    CRateLimiter *p = static_cast<CRateLimiter *>(ptr);
    delete p;
}