#include "libskiplist.hpp"
#include "fixed_12.hpp"

SEQ_FUNC skiplist_t *seq_skiplist_new(bool isForward) {
    if (isForward) {
        // 999999.999999999999
        auto f = fixed_12_new_string("999999.999999999999");
        // 100000.0
        // auto f = fixed_12_new_string("100000.0");
        // printf("f: %ld\n", f.fp);
        // printf("fs: %s\n", fixed_12_string(f).c_str());
        return new skiplist_t(f, true);
    } else {
        auto f = fixed_12_new_string("0.0");
        // printf("f: %ld\n", f.fp);
        return new skiplist_t(f, false);
    }
}

SEQ_FUNC void seq_skiplist_free(skiplist_t *list) {
    delete list;
    list = nullptr;
}

SEQ_FUNC bool seq_skiplist_insert(skiplist_t *list, int64_t key, int64_t value,
                                  bool updateIfExists) {
    return list->insert(key, value, updateIfExists);
}

SEQ_FUNC int64_t seq_skiplist_remove(skiplist_t *list, int64_t key) {
    int64_t value;
    auto ok = list->remove(key, value);
    if (ok) {
        return value;
    } else {
        return fixed_12_zero;
    }
}

SEQ_FUNC int64_t seq_skiplist_search(skiplist_t *list, int64_t key) {
    auto node = list->search(key);
    if (node != nullptr) {
        return node->getValue();
    } else {
        return fixed_12_zero;
    }
}

SEQ_FUNC void seq_skiplist_dump(skiplist_t *list) { list->dumpAllNodes(); }

SEQ_FUNC void seq_skiplist_top_n(skiplist_t *list, int n, int64_t *keys,
                                 int64_t *values, size_t *resSize) {
    std::vector<std::pair<int64_t, int64_t>> v;
    list->topN(n, v);
    auto size = v.size();
    for (int i = 0; i < size; i++) {
        auto &item = v[i];
        keys[i] = item.first;
        values[i] = item.second;
    }
    *resSize = size;
}