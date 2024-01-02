#include "libskiplist.hpp"
#include "fixed12.hpp"

SEQ_FUNC skiplist_t *seq_skiplist_new(bool isForward) {
    if (isForward) {
        // 999999.999999999999
        // auto f = fixed_12_new_string("999999.999999999999");
        // return new skiplist_t(f, true);
        Fixed12 f = Fixed12::newSV("999999.999999999999");
        return new skiplist_t(f.toValue(), true);
    } else {
        Fixed12 f = Fixed12::newSV("0.0");
        // auto f = fixed_12_new_string("0.0");
        // printf("f: %ld\n", f.fp);
        // return new skiplist_t(f, false);
        return new skiplist_t(f.toValue(), true);
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
        return Fixed12Zero.toValue();
    }
}

SEQ_FUNC int64_t seq_skiplist_search(skiplist_t *list, int64_t key) {
    auto node = list->search(key);
    if (node != nullptr) {
        return node->getValue();
    } else {
        return Fixed12Zero.toValue();
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

SEQ_FUNC Node<fixed12_t, fixed12_t> *
seq_skiplist_begin(const skiplist_t *skipList) {
    return skipList->begin();
}

SEQ_FUNC Node<fixed12_t, fixed12_t> *seq_skiplist_end() { return nullptr; }

SEQ_FUNC Node<fixed12_t, fixed12_t> *
seq_skiplist_next(const skiplist_t *skipList,
                  Node<fixed12_t, fixed12_t> *node) {
    return skipList->next(node);
}

SEQ_FUNC void seq_skiplist_node_value(Node<fixed12_t, fixed12_t> *node,
                                      fixed12_t *key, fixed12_t *value) {
    *key = node->getKey();
    *value = node->getValue();
}
