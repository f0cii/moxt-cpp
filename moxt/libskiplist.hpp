#ifndef MOXT_LIBSLIST_HPP
#define MOXT_LIBSLIST_HPP

#include "common.hpp"
#include "skiplist/skiplist.hpp"

SEQ_FUNC skiplist_t *seq_skiplist_new(bool isForward);

SEQ_FUNC void seq_skiplist_free(skiplist_t *list);

SEQ_FUNC bool seq_skiplist_insert(skiplist_t *list, int64_t key, int64_t value,
                                  bool updateIfExists);

SEQ_FUNC int64_t seq_skiplist_remove(skiplist_t *list, int64_t key);

// SEQ_FUNC int64_t seq_skiplist_search(skiplist_t *list, int64_t key);

SEQ_FUNC void seq_skiplist_dump(skiplist_t *list);



// // resSize: 返回数据的长度
// SEQ_FUNC void seq_skiplist_top_n(skiplist_t *list, int n, int64_t *keys,
//                                  int64_t *values, size_t *resSize);

#endif