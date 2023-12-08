#include "libc.hpp"
#include "common.hpp"
#include "fixed_12.hpp"
#include "moxt/utils/floatutil.hpp"
#include <absl/strings/ascii.h>
#include <absl/strings/escaping.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/string_view.h>
#include <chrono>
#include <cstddef>

#include <std23/function_ref.h>

#include "moxt/app.hpp"
#include "moxt/httpx/asio_ioc.hpp"
#include "moxt/utils/idgen.hpp"
#include <csignal>
#include <execinfo.h>
#include <iostream>

#include <openssl/err.h>
#include <openssl/opensslv.h>
#include <snmalloc/override/new.cc>
#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

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
    signal(SIGSEGV, handler);

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

// 存储全局对象地址值
static std::unordered_map<int64_t, int64_t> globalObjectMap;

SEQ_FUNC void seq_store_object_address(int64_t id, int64_t ptr) {
    globalObjectMap[id] = ptr;
}

SEQ_FUNC int64_t seq_retrieve_object_address(int64_t id) {
    auto it = globalObjectMap.find(id);
    if (it != globalObjectMap.end()) {
        return it->second;
    } else {
        return 0;
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

    // auto s = ByteArrayToHex((const uint8_t *)secretKey, secretKey_len);
    // printf("secretKey=%s\n", s.c_str());

    // auto s1 = ByteArrayToHex((const uint8_t *)message, message_len);
    // printf("message=%s\n", s1.c_str());

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

    // printf("digestLen=%d\n", digestLen);

    // auto s = ByteArrayToHex((const uint8_t *)result, digestLen);
    // printf("digest=%s\n", s.c_str());

    // std::string_view sv((const char *)digest, digestLen);
    // std::string output;
    // absl::Base64Escape(sv, &output);

    // printf("%s\n", output.c_str());

    // 释放内存
    free(digest);

    return digestLen;
}

SEQ_FUNC size_t seq_base64_encode(uint8_t *input, size_t n, uint8_t *result) {
    std::string_view text((const char *)input, n);

    // auto s = ByteArrayToHex((const uint8_t *)input, n);
    // printf("seq_base64_encode input=%s\n", s.c_str());

    std::string output;
    absl::Base64Escape(text, &output);

    auto length = output.length();

    memcpy(result, output.c_str(), length);
    memset(result + length, 0, 1);

    return length;
}

SEQ_FUNC size_t seq_hex_encode(uint8_t *input, size_t n, uint8_t *result) {
    std::string_view text((const char *)input, n);

    // auto s = ByteArrayToHex((const uint8_t *)input, n);
    // printf("seq_base64_encode input=%s\n", s.c_str());

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
    return stringToInt64(std::string_view(s, s_len));
}

SEQ_FUNC double seq_strtod(const char *s, size_t s_len) {
    return strtod(std::string_view(s, s_len));
}

// https://github.com/intel/intel-ipsec-mb/blob/main/test/wycheproof-app/hmac_sha1_test.json.c

// struct mac_test {
//     size_t keySize; /* bits */
//     size_t tagSize; /* bits */
//     size_t tcId;
//     const char *key;
//     const char *msg;
//     const char *tag;
//     int resultValid;
//     size_t msgSize; /* bits */
//     const char *iv;
//     size_t ivSize; /* bits */
// };

// static int process_job(IMB_MGR *p_mgr) {
//     IMB_JOB *job = IMB_SUBMIT_JOB(p_mgr);

//     if (!job) {
//         const int err = imb_get_errno(p_mgr);

//         /* check for error */
//         if (err != 0)
//             return 0;

//         /* flush to get the job processed */
//         job = IMB_FLUSH_JOB(p_mgr);

//         /* if flush returns nothing then it's an error */
//         if (!job)
//             return 0;
//     }

//     /* if returned job is not complete then it's an error */
//     if (job->status != IMB_STATUS_COMPLETED)
//         return 0;

//     return 1;
// }

// static int test_hmac_sha256(IMB_MGR *p_mgr) {
//     DECLARE_ALIGNED(uint8_t hmac_ipad[IMB_SHA256_DIGEST_SIZE_IN_BYTES], 16);
//     DECLARE_ALIGNED(uint8_t hmac_opad[IMB_SHA256_DIGEST_SIZE_IN_BYTES], 16);
//     uint8_t tag[IMB_SHA256_DIGEST_SIZE_IN_BYTES];

//     while (IMB_FLUSH_JOB(p_mgr) != nullptr)
//         ;

//     mac_test v = {
//         160,
//         160,
//         29,
//         "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10"
//         "\x11\x12\x13",
//         "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
//         "\xe5\x64\x16\x00\xce\xdd\x7e\x12\x06\x3d\xea\xea\x07\x88\x78\x5f\x56"
//         "\x11\x35\x20",
//         0,
//         128,
//         nullptr,
//         0};

//     // for (; v->msg != NULL; v++, run_vectors++) {
//     IMB_ASSERT((v->tagSize / 8) <= sizeof(tag));

//     /* test JOB API */
//     IMB_JOB *job = IMB_GET_NEXT_JOB(p_mgr);

//     job->hash_alg = IMB_AUTH_HMAC_SHA_256;
//     job->cipher_mode = IMB_CIPHER_NULL;
//     job->cipher_direction = IMB_DIR_ENCRYPT;
//     job->chain_order = IMB_ORDER_HASH_CIPHER;
//     job->src = (const uint8_t *)v.msg;
//     job->hash_start_src_offset_in_bytes = 0;
//     job->msg_len_to_hash_in_bytes = v.msgSize / 8;
//     job->auth_tag_output = tag;
//     job->auth_tag_output_len_in_bytes = v.tagSize / 8;

//     // // Set hashed key arrays
//     // job->u.HMAC._hashed_auth_key_xor_ipad = hmac_ipad;
//     // job->u.HMAC._hashed_auth_key_xor_opad = hmac_opad;

//     // Set tag length in bits
//     // job->auth_tag_output_len_in_bits = v.tagSize;

//     imb_hmac_ipad_opad(p_mgr, IMB_AUTH_HMAC_SHA_256, v.key, v.keySize / 8,
//                        hmac_ipad, hmac_opad);

//     job->u.HMAC._hashed_auth_key_xor_ipad = hmac_ipad;
//     job->u.HMAC._hashed_auth_key_xor_opad = hmac_opad;

//     /* clear space where computed TAG is put into */
//     memset(tag, 0, sizeof(tag));

//     if (!process_job(p_mgr)) {
//         printf("mac_submit_and_check 3\n");
//         if (v.resultValid) {
//             // print_mac_test(v);
//             printf("JOB-API submit/flush error!\n");
//             printf("ERROR: %s\n", imb_get_strerror(imb_get_errno(p_mgr)));
//             return 0;
//         } else {
//             /* error was expected */
//             return 1;
//         }
//     }

//     auto s = ByteArrayToHex((const uint8_t *)tag, 32);
//     printf("hex=%s\n", s.c_str());

//     return 1;
// }

// SEQ_FUNC void seq_test_hmac_sha256_2() {
//     IMB_MGR *mb_mgr = nullptr;

//     /* IMB API: Allocate MB_MGR */
//     mb_mgr = alloc_mb_mgr(0);

//     if (mb_mgr == nullptr) {
//         printf("Could not allocate memory for IMB_MGR\n");
//         return;
//     }

//     /* IMB API: Initialize MB_MGR, detecting best implementation to use */
//     init_mb_mgr_auto(mb_mgr, nullptr);

//     test_hmac_sha256(mb_mgr);

//     free_mb_mgr(mb_mgr);
// }
