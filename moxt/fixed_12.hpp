#pragma once

#include "common.hpp"
#include "libc.hpp"

// 快速反序列化int64
// https://kholdstare.github.io/technical/2020/05/26/faster-integer-parsing.html

// typedef struct {
//     int64_t fp;
// } fixed_12_t;

typedef int64_t fixed_12_t;

struct OrderBookLevel {
    fixed_12_t price;
    fixed_12_t qty;
};

SEQ_FUNC fixed_12_t fixed_12_new();

SEQ_FUNC fixed_12_t fixed_12_new_int(int64_t value);

SEQ_FUNC fixed_12_t fixed_12_new_double(double value);

SEQ_FUNC fixed_12_t fixed_12_new_string_n(const char *cstr, size_t len);

SEQ_FUNC fixed_12_t fixed_12_new_string_view(const std::string_view& s);

SEQ_FUNC fixed_12_t fixed_12_new_string(const char *cstr);

SEQ_FUNC int64_t fixed_12_fp(const fixed_12_t &fixed);

SEQ_FUNC int64_t fixed_12_int_part(const fixed_12_t &fixed);

SEQ_FUNC int64_t fixed_12_frac_part(const fixed_12_t &fixed);

SEQ_FUNC double fixed_12_float64(const fixed_12_t &fixed);

std::string fixed_12_string_fp(const int64_t fp);

std::string fixed_12_string(const fixed_12_t fixed);

SEQ_FUNC size_t fixed_12_string_res(const fixed_12_t fixed, char *result);

SEQ_FUNC fixed_12_t fixed_12_add(const fixed_12_t a, const fixed_12_t b);

SEQ_FUNC fixed_12_t fixed_12_subtract(const fixed_12_t a, const fixed_12_t b);

SEQ_FUNC fixed_12_t fixed_12_multiply(const fixed_12_t a, const fixed_12_t b);

SEQ_FUNC fixed_12_t fixed_12_divide(const fixed_12_t a, const fixed_12_t b);

SEQ_FUNC bool fixed_12_equal(const fixed_12_t a, const fixed_12_t b);

SEQ_FUNC bool fixed_12_not_equal(const fixed_12_t a, const fixed_12_t b);

SEQ_FUNC bool fixed_12_less_than(const fixed_12_t a, const fixed_12_t b);

SEQ_FUNC bool fixed_12_less_than_or_equal(const fixed_12_t a,
                                          const fixed_12_t b);

SEQ_FUNC bool fixed_12_greater_than(const fixed_12_t a, const fixed_12_t b);

SEQ_FUNC bool fixed_12_greater_than_or_equal(const fixed_12_t a,
                                             const fixed_12_t b);

SEQ_FUNC void test_fixed_12();

const fixed_12_t fixed_12_zero = fixed_12_new_string("0.0");

// 最大值: 999999.999999999999
