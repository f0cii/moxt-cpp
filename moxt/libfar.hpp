#ifndef MOXT_LIBFAR_HPP
#define MOXT_LIBFAR_HPP

#include "common.hpp"
#include "libc.hpp"
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

#endif