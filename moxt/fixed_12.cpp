#include "fixed_12.hpp"
#include "moxt/utils/cast.hpp"
#include <nanobench.h>

constexpr int64_t FIXED_SCALE = 1000000000000;

constexpr int MAX_FRAC_BITS = 12;

SEQ_FUNC fixed_12_t fixed_12_new() {
    fixed_12_t fixed = 0;
    return fixed;
}

SEQ_FUNC fixed_12_t fixed_12_new_int(int64_t value) {
    fixed_12_t fixed = value * FIXED_SCALE;
    return fixed;
}

SEQ_FUNC fixed_12_t fixed_12_new_double(double value) {
    fixed_12_t fixed = (int64_t)(value * FIXED_SCALE);
    return fixed;
}

SEQ_FUNC fixed_12_t fixed_12_new_string_n(const char *cstr, size_t len) {
    std::string_view s(cstr, len);
    return fixed_12_new_string_view(s);
}

SEQ_FUNC fixed_12_t fixed_12_new_string_view(const std::string_view &s) {
    size_t period = s.find('.');
    long long i;
    long long f;
    int64_t sign = 1;
    if (period == std::string_view::npos) {
        i = strtoi(s);
        if (i < 0) {
            sign = -1;
            i = -i;
        }
    } else {
        if (period > 0) {
            i = strtoi(s.substr(0, period));
            if (i < 0) {
                sign = -1;
                i = -i;
            }
        }

        std::string fs(s.substr(period + 1));
        fs += std::string(12 - fs.length(), '0');
        f = strtoi(fs.substr(0, 12));
    }
    fixed_12_t fixed = sign * (i * FIXED_SCALE + f);
    return fixed;
}

SEQ_FUNC fixed_12_t fixed_12_new_string(const char *cstr) {
    std::string_view s(cstr, strlen(cstr));
    return fixed_12_new_string_view(s);
}

SEQ_FUNC int64_t fixed_12_fp(const fixed_12_t &fixed) { return fixed; }

SEQ_FUNC int64_t fixed_12_int_part(const fixed_12_t &fixed) {
    return fixed / FIXED_SCALE;
}

SEQ_FUNC int64_t fixed_12_frac_part(const fixed_12_t &fixed) {
    return fixed % FIXED_SCALE;
}

SEQ_FUNC double fixed_12_float64(const fixed_12_t &fixed) {
    return (double)fixed / FIXED_SCALE;
}

std::string fixed_12_string_fp(const int64_t fp) { return fixed_12_string(fp); }

std::string fixed_12_string(const fixed_12_t fixed) {
    char result[16] = {0};

    int index = 0;

    // Handle negative
    int64_t intPart = fixed_12_int_part(fixed);
    do {
        result[index++] = '0' + intPart % 10;
        intPart /= 10;
    } while (intPart > 0);

    // Reverse the integer part
    for (int i = 0, j = index - 1; i < j; i++, j--) {
        char temp = result[i];
        result[i] = result[j];
        result[j] = temp;
    }

    // Check if there is a decimal point

    int64_t fracPart_ = fixed_12_frac_part(fixed);

    if (fracPart_ > 0) {
        // Add the decimal point
        result[index++] = '.';

        // Convert the fractional part to a string
        std::string fracPart = std::to_string(fracPart_);
        int fracPartLength = fracPart.length();
        int zerosToAdd = MAX_FRAC_BITS - fracPartLength;
        if (zerosToAdd > 0) {
            // Add leading zeros if necessary
            for (int i = 0; i < zerosToAdd; i++) {
                result[index++] = '0';
            }
        }

        // 计算小数位末尾0个数
        int fracPartN = 0;
        for (int i = fracPartLength - 1; i >= 0; i--) {
            if (fracPart[i] == '0') {
                fracPartN++;
            } else {
                break;
            }
        }

        // Copy the non-zero digits of the fractional part
        for (int i = 0; i < fracPartLength - 1; i++) {
            result[index++] = fracPart[i];
        }
    }

    // Null-terminate
    result[index] = '\0';

    return std::string(result);
}

SEQ_FUNC size_t fixed_12_string_res(const fixed_12_t fixed, char *result) {
    // char result[16]; // Assuming the maximum length of the resulting string
    // is
    //  15 characters
    int index = 0;

    // Handle negative
    int64_t intPart = fixed_12_int_part(fixed);
    do {
        result[index++] = '0' + intPart % 10;
        intPart /= 10;
    } while (intPart > 0);

    // Reverse the integer part
    for (int i = 0, j = index - 1; i < j; i++, j--) {
        char temp = result[i];
        result[i] = result[j];
        result[j] = temp;
    }

    // Check if there is a decimal point

    int64_t fracPart_ = fixed_12_frac_part(fixed);

    if (fracPart_ > 0) {
        // Add the decimal point
        result[index++] = '.';

        // Convert the fractional part to a string
        std::string fracPart = std::to_string(fracPart_);
        int fracPartLength = fracPart.length();
        int zerosToAdd = MAX_FRAC_BITS - fracPartLength;
        if (zerosToAdd > 0) {
            // Add leading zeros if necessary
            for (int i = 0; i < zerosToAdd; i++) {
                result[index++] = '0';
            }
        }

        // 计算小数位末尾0个数
        int fracPartN = 0;
        for (int i = fracPartLength - 1; i >= 0; i--) {
            if (fracPart[i] == '0') {
                fracPartN++;
            } else {
                break;
            }
        }

        // Copy the non-zero digits of the fractional part
        for (int i = 0; i < fracPartLength - 1; i++) {
            result[index++] = fracPart[i];
        }
    }

    // Null-terminate
    result[index] = '\0';

    return strlen(result);
}

SEQ_FUNC fixed_12_t fixed_12_add(const fixed_12_t a, const fixed_12_t b) {
    fixed_12_t result = a + b;
    return result;
}

SEQ_FUNC fixed_12_t fixed_12_subtract(const fixed_12_t a, const fixed_12_t b) {
    fixed_12_t result = a - b;
    return result;
}

SEQ_FUNC fixed_12_t fixed_12_multiply(const fixed_12_t a, const fixed_12_t b) {
    fixed_12_t result = (a * b) / FIXED_SCALE;
    return result;
}

SEQ_FUNC fixed_12_t fixed_12_divide(const fixed_12_t a, const fixed_12_t b) {
    fixed_12_t result = (a * FIXED_SCALE) / b;
    return result;
}

SEQ_FUNC bool fixed_12_equal(const fixed_12_t a, const fixed_12_t b) {
    return a == b;
}

SEQ_FUNC bool fixed_12_not_equal(const fixed_12_t a, const fixed_12_t b) {
    return a != b;
}

SEQ_FUNC bool fixed_12_less_than(const fixed_12_t a, const fixed_12_t b) {
    return a < b;
}

SEQ_FUNC bool fixed_12_less_than_or_equal(const fixed_12_t a,
                                          const fixed_12_t b) {
    return a <= b;
}

SEQ_FUNC bool fixed_12_greater_than(const fixed_12_t a, const fixed_12_t b) {
    return a > b;
}

SEQ_FUNC bool fixed_12_greater_than_or_equal(const fixed_12_t a,
                                             const fixed_12_t b) {
    return a >= b;
}

SEQ_FUNC void test_fixed_12() {
    auto a = fixed_12_new_string("3.25");
    auto b = fixed_12_new_string("5.12");
    auto c = fixed_12_add(a, b);
    // char str[16];
    // fixed_12_string(c, str);
    // char str[16];
    auto str = fixed_12_string(c);
    printf("c=%s\n", str.c_str());

    char s1[16];
    fixed_12_string_res(c, s1);
    printf("s1=%s\n", s1);

    ankerl::nanobench::Bench().run("d", [&] { // minEpochIterations
        auto a = fixed_12_new_string("3.25");
        auto b = fixed_12_new_string("5.12");
        auto c = fixed_12_add(a, b);
        // char str[16];
        // fixed_12_string(c, str);
        // auto s = fixed_12_string(c);
        char s2[16];
        fixed_12_string_res(c, s2);
        ankerl::nanobench::doNotOptimizeAway(s2);
    });
}