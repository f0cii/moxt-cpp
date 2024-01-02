#pragma once

#include "common.hpp"
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cstdio>
#include <iostream>
#include <string>
#include <string_view>

namespace mp = boost::multiprecision;

using uint128_t = boost::multiprecision::uint128_t;
using uint256_t = boost::multiprecision::uint256_t;

using big_int = mp::cpp_int;
using float_50 = mp::number<mp::cpp_dec_float<50>>; // 50是小数点后的位数

using cpp_dec_float_50 = boost::multiprecision::cpp_dec_float_50;

// 返回小数位: 0.001 -> 4
int decimalPlaces(double value);

constexpr int64_t FIXED_SCALE = 1000000000000;
constexpr int MAX_FRAC_BITS = 12;

class Fixed12 {
  protected:
    Fixed12(int64_t value) : value_(value) {}

  public:
    Fixed12() : value_(0) {}

    static Fixed12 newV(int64_t value) { return Fixed12(value); }

    static Fixed12 newI(int64_t value) { return Fixed12(value * FIXED_SCALE); }

    static Fixed12 newD(double value) {
        return Fixed12(static_cast<int64_t>(value * FIXED_SCALE));
    }

    static Fixed12 newSV(const std::string_view &s);

    static Fixed12 newS(const std::string &s);

    bool isZero() const { return value_ == 0; }

    int64_t toValue() const { return value_; }

    int64_t toInt() const { return value_ / FIXED_SCALE; }

    int64_t toFractionalPart() const { return value_ % FIXED_SCALE; }

    double toDouble() const {
        return static_cast<double>(value_) / FIXED_SCALE;
    }

    Fixed12 roundToFractional(int64_t scale) const {
        return Fixed12(static_cast<int64_t>(
            ::round(value_ / static_cast<double>(scale)) * scale));
    }

    Fixed12 round(int decimalPlaces) const {
        int64_t scale = std::pow(10, MAX_FRAC_BITS - decimalPlaces);
        return Fixed12(static_cast<int64_t>(
            ::round(value_ / static_cast<double>(scale)) * scale));
    }

    std::string toString() const;

    Fixed12 operator+(const Fixed12 &other) const {
        return Fixed12(value_ + other.value_);
    }

    Fixed12 operator-(const Fixed12 &other) const {
        return Fixed12(value_ - other.value_);
    }

    Fixed12 operator*(const Fixed12 &other) const {
        auto value = static_cast<big_int>(value_) *
                     static_cast<big_int>(other.value_) /
                     static_cast<big_int>(FIXED_SCALE);
        return Fixed12(value.convert_to<int64_t>());
    }

    Fixed12 operator/(const Fixed12 &other) const {
        auto value =
            (static_cast<big_int>(value_) * static_cast<big_int>(FIXED_SCALE)) /
            static_cast<big_int>(other.value_);
        return Fixed12(value.convert_to<int64_t>());
    }

    bool operator==(const Fixed12 &other) const {
        return value_ == other.value_;
    }

    bool operator!=(const Fixed12 &other) const {
        return value_ != other.value_;
    }

    bool operator<(const Fixed12 &other) const { return value_ < other.value_; }

    bool operator<=(const Fixed12 &other) const {
        return value_ <= other.value_;
    }

    bool operator>(const Fixed12 &other) const { return value_ > other.value_; }

    bool operator>=(const Fixed12 &other) const {
        return value_ >= other.value_;
    }

  private:
    int64_t value_;
    // static constexpr int64_t FIXED_SCALE = 1000000000000;
    // static constexpr int MAX_FRAC_BITS = 12;
};

typedef int64_t fixed12_t;

struct OrderBookLevel {
    Fixed12 price;
    Fixed12 qty;
};

const Fixed12 Fixed12Zero = Fixed12::newI(0);
const Fixed12 Fixed12One = Fixed12::newI(1);
const Fixed12 Fixed12Two = Fixed12::newI(2);

SEQ_FUNC int64_t fixed_12_int_part(const fixed12_t &fixed);

SEQ_FUNC int64_t fixed_12_frac_part(const fixed12_t &fixed);

SEQ_FUNC fixed12_t fixed_12_new_string_n(const char *cstr, size_t len);

SEQ_FUNC fixed12_t fixed_12_new_string_view(const std::string_view &s);

SEQ_FUNC size_t fixed_12_string_res(const fixed12_t fixed, char *result);

// 自定义*
SEQ_FUNC int64_t seq_fixed_mul(int64_t a, int64_t b);

// 自定义/
SEQ_FUNC int64_t seq_fixed_truediv(int64_t a, int64_t b);

SEQ_FUNC int64_t seq_fixed_round_to_fractional(int64_t a, int64_t scale);

SEQ_FUNC int64_t seq_fixed_round(int64_t a, int decimalPlaces);

int test_fixed12();
