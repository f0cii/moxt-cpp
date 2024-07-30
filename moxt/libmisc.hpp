#ifndef MOXT_LIBMISC_HPP
#define MOXT_LIBMISC_HPP

#include "common.hpp"

// SEQ_FUNC void test_identity_pool();
// SEQ_FUNC void test_ondemand_parser_pool();

SEQ_FUNC int seq_ct_init();

SEQ_FUNC int64_t seq_get_next_cache_key();

SEQ_FUNC const char *seq_set_string_in_cache(int64_t key, const char *value,
                                             size_t valueLen);

SEQ_FUNC const char *seq_get_string_in_cache(int64_t key, size_t *resultLen);

SEQ_FUNC bool seq_free_string_in_cache(int64_t key);

SEQ_FUNC void *seq_new_crate_limiter(int maxCount, uint64_t windowSize);

SEQ_FUNC bool seq_crate_limiter_allow_and_record_request(void *ptr);

SEQ_FUNC void seq_delete_crate_limiter(void *ptr);

#endif