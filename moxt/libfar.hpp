#ifndef MOXT_LIBFAR_HPP
#define MOXT_LIBFAR_HPP

#include "common.hpp"
#include "libc.hpp"
#include <cstddef>
#include <variant>

class FuncArgResult {
  public:
    void set(const std::string &key, int64_t value);

    void set(const std::string &key, double value);

    void set(const std::string &key, bool value);

    void set(const std::string &key, std::string &value);

    int64_t get_int64(const std::string &key);

    double get_double(const std::string &key);

    bool get_bool(const std::string &key);

    std::string *get_string(const std::string &key);

    void free();

    size_t size();

  private:
    std::map<std::string, std::variant<int64_t, double, bool, std::string>>
        data;
};

SEQ_FUNC FuncArgResult *seq_far_new();

SEQ_FUNC void *seq_nullptr();

SEQ_FUNC void seq_far_free(FuncArgResult *p);

SEQ_FUNC size_t seq_far_size(FuncArgResult *p);

SEQ_FUNC bool seq_far_set_int(FuncArgResult *p, const char *key, size_t key_len,
                              int64_t value);

SEQ_FUNC bool seq_far_set_float(FuncArgResult *p, const char *key,
                                size_t key_len, double value);

SEQ_FUNC bool seq_far_set_bool(FuncArgResult *p, const char *key,
                               size_t key_len, bool value);

SEQ_FUNC bool seq_far_set_string(FuncArgResult *p, const char *key,
                                 size_t key_len, const char *value,
                                 size_t value_len);

SEQ_FUNC int64_t seq_far_get_int(FuncArgResult *p, const char *key,
                                 size_t key_len);

SEQ_FUNC double seq_far_get_float(FuncArgResult *p, const char *key,
                                  size_t key_len);

SEQ_FUNC bool seq_far_get_bool(FuncArgResult *p, const char *key,
                               size_t key_len);

SEQ_FUNC const char *seq_far_get_string(FuncArgResult *p, const char *key,
                                        size_t key_len, size_t *n);

#endif