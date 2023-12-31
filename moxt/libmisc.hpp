#ifndef MOXT_LIBMISC_HPP
#define MOXT_LIBMISC_HPP

#include "common.hpp"

// SEQ_FUNC void test_identity_pool();
// SEQ_FUNC void test_ondemand_parser_pool();

SEQ_FUNC int seq_ct_init();
// // 生成 nanoid
// SEQ_FUNC size_t seq_nanoid(char *result);

// // 读取全局对象地址值
// SEQ_FUNC void seq_store_object_address(int64_t id, int64_t ptr);

// // 存储全局对象地址值
// SEQ_FUNC int64_t seq_retrieve_object_address(int64_t id);

// SEQ_FUNC std::map<std::string, std::string> *seq_ssmap_new();

// SEQ_FUNC void seq_ssmap_free(std::map<std::string, std::string> *p);

// SEQ_FUNC bool seq_ssmap_set(std::map<std::string, std::string> *p,
//                             const char *name, const char *value);

// SEQ_FUNC const char *seq_ssmap_get(std::map<std::string, std::string> *p,
//                                    const char *name, size_t *n);

// SEQ_FUNC size_t seq_ssmap_size(std::map<std::string, std::string> *p);

// SEQ_FUNC size_t seq_hmac_sha256(const char *secretKey, size_t secretKey_len,
//                                 const char *message, size_t message_len,
//                                 uint8_t *result);

// SEQ_FUNC size_t seq_base64_encode(uint8_t *input, size_t n, uint8_t *result);

// SEQ_FUNC size_t seq_hex_encode(uint8_t *input, size_t n, uint8_t *result);

// SEQ_FUNC void seq_test_hmac_sha256(const char *secretKey, size_t
// secretKey_len,
//                                    const char *message, size_t message_len);

// SEQ_FUNC int64_t seq_atoi(const char *s, size_t s_len);

// SEQ_FUNC int64_t seq_strtoi(const char *s, size_t s_len);

// SEQ_FUNC double seq_strtod(const char *s, size_t s_len);

SEQ_FUNC int64_t seq_get_next_cache_key();

SEQ_FUNC const char *seq_set_string_in_cache(int64_t key, const char *value,
                                             size_t valueLen);

SEQ_FUNC const char *seq_get_string_in_cache(int64_t key, size_t *resultLen);

SEQ_FUNC bool seq_free_string_in_cache(int64_t key);

#endif