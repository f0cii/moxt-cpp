#include "libfar.hpp"

void FuncArgResult::set(const std::string &key, int64_t value) {
    data[key] = value;
}

void FuncArgResult::set(const std::string &key, double value) {
    data[key] = value;
}

void FuncArgResult::set(const std::string &key, bool value) {
    data[key] = value;
}

void FuncArgResult::set(const std::string &key, std::string &value) {
    data[key] = value;
}

int64_t FuncArgResult::get_int64(const std::string &key) {
    auto it = data.find(key);
    if (it != data.end()) {
        if (auto *v = std::get_if<int64_t>(&it->second)) {
            return *v;
        }
    }
    return 0;
}

double FuncArgResult::get_double(const std::string &key) {
    auto it = data.find(key);
    if (it != data.end()) {
        if (auto *v = std::get_if<double>(&it->second)) {
            return *v;
        }
    }
    return 0;
}

bool FuncArgResult::get_bool(const std::string &key) {
    auto it = data.find(key);
    if (it != data.end()) {
        if (auto *v = std::get_if<bool>(&it->second)) {
            return *v;
        }
    }
    return false;
}

std::string *FuncArgResult::get_string(const std::string &key) {
    auto it = data.find(key);
    if (it != data.end()) {
        if (auto *v = std::get_if<std::string>(&it->second)) {
            return &*v;
        }
    }
    return nullptr;
}

void FuncArgResult::free() { data.clear(); }

size_t FuncArgResult::size() { return data.size(); }

SEQ_FUNC FuncArgResult *seq_far_new() { return new FuncArgResult(); }

SEQ_FUNC void *seq_nullptr() { return nullptr; }

SEQ_FUNC void seq_far_free(FuncArgResult *p) {
    if (p != nullptr) {
        p->free();
        delete p;
        p = nullptr;
    }
}

SEQ_FUNC size_t seq_far_size(FuncArgResult *p) { return p->size(); }

SEQ_FUNC bool seq_far_set_int(FuncArgResult *p, const char *key, size_t key_len,
                              int64_t value) {
    auto key_str = std::string(key, key_len);
    p->set(key_str, value);
    return true;
}

SEQ_FUNC bool seq_far_set_float(FuncArgResult *p, const char *key,
                                size_t key_len, double value) {
    auto key_str = std::string(key, key_len);
    p->set(key_str, value);
    return true;
}

SEQ_FUNC bool seq_far_set_bool(FuncArgResult *p, const char *key,
                               size_t key_len, bool value) {
    auto key_str = std::string(key, key_len);
    p->set(key_str, value);
    return true;
}

SEQ_FUNC bool seq_far_set_string(FuncArgResult *p, const char *key,
                                 size_t key_len, const char *value,
                                 size_t value_len) {
    auto key_str = std::string(key, key_len);
    auto value_ = std::string(value, value_len);
    p->set(key_str, value_);
    return true;
}

SEQ_FUNC int64_t seq_far_get_int(FuncArgResult *p, const char *key,
                                 size_t key_len) {
    std::string keyStr(key, key_len);
    return p->get_int64(keyStr);
}

SEQ_FUNC double seq_far_get_float(FuncArgResult *p, const char *key,
                                  size_t key_len) {
    std::string keyStr(key, key_len);
    return p->get_double(keyStr);
}

SEQ_FUNC bool seq_far_get_bool(FuncArgResult *p, const char *key,
                               size_t key_len) {
    std::string keyStr(key, key_len);
    return p->get_bool(keyStr);
}

SEQ_FUNC const char *seq_far_get_string(FuncArgResult *p, const char *key,
                                        size_t key_len, size_t *n) {
    std::string keyStr(key, key_len);
    auto s = p->get_string(keyStr);
    *n = s->length();
    return s->c_str();
}