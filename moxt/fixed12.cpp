#include "fixed12.hpp"
#include "moxt/utils/cast.hpp"
#include <cstdio>

int decimalPlaces(double value) {
    if (value == 0.0)
        return 0;

    double absValue = fabs(value);

    // 取整位数即decimal places
    return std::ceil(log10(1.0 / absValue));
}

Fixed12 Fixed12::newSV(const std::string_view &s) {
    int64_t v = fixed12_new_string_view(s);
    return Fixed12(v);
}

Fixed12 Fixed12::newS(const std::string &s) {
    int64_t v = fixed12_new_string_view(s);
    return Fixed12(v);
}

std::string Fixed12::toString() const {
    char result[17] = {0};
    auto n = seq_fixed12_to_string(value_, result);
    return std::string(result, n);
}

double roundDouble(double price, double tickSize) {
    return round(price / tickSize) * tickSize;
}

// Example usage:
int test_fixed12() {
    Fixed12 fixed1 = Fixed12();
    Fixed12 fixed2 = Fixed12::newD(42.0);
    Fixed12 fixed3 = Fixed12::newD(3.14);
    Fixed12 fixed4 = Fixed12::newSV("123.456");

    std::cout << "fixed1: " << fixed1.toString() << std::endl;
    std::cout << "fixed2: " << fixed2.toString() << std::endl;
    std::cout << "fixed3: " << fixed3.toString() << std::endl;
    std::cout << "fixed4: " << fixed4.toString() << std::endl;

    Fixed12 sum = fixed2 + fixed3;
    std::cout << "Sum: " << sum.toString() << std::endl;

    auto f = Fixed12::newSV("10000.1") + Fixed12::newSV("10000.2");
    std::cout << "f: " << f.toString() << std::endl;
    double a = 10000.1;
    double b = 10000.2;
    auto g = a + b;

    for (int i = 1; i <= 20; ++i) {
        printf("Decimal places: %2d, g: %.*f\n", i, i, g);
    }

    auto w = Fixed12::newSV("3000.0") * Fixed12::newSV("1.01");
    std::cout << "w: " << w.toString() << std::endl;

    auto e = Fixed12::newSV("3000.0") / Fixed12::newSV("1.01");
    std::cout << "e: " << e.toString() << std::endl;

    std::cout << "0.0: " << Fixed12::newSV("0.0").toString() << std::endl;
    std::cout << "0.1: " << Fixed12::newSV("0.1").toString() << std::endl;
    std::cout << "1.0: " << Fixed12::newSV("1.0").toString() << std::endl;
    std::cout << "1.0999: " << Fixed12::newSV("1.0999").toString() << std::endl;

    auto d = roundDouble(100.123456789, 0.001);
    printf("Decimal places: %.8f\n", d);

    std::cout
        << "1.0999 1: "
        << Fixed12::newSV("1.0999").roundToFractional(10000000000).toString()
        << std::endl;

    std::cout << "1.0999 2: " << Fixed12::newSV("1.123456").round(2).toString()
              << std::endl;

    std::cout << decimalPlaces(0.1) << std::endl;
    std::cout << decimalPlaces(0.01) << std::endl;
    std::cout << decimalPlaces(0.001) << std::endl;
    std::cout << decimalPlaces(0.0001) << std::endl;
    std::cout << decimalPlaces(0.000000000001) << std::endl;

    return 0;
}

SEQ_FUNC int64_t fixed12_int_part(const fixed12_t &fixed) {
    return fixed / FIXED_SCALE;
}

SEQ_FUNC int64_t fixed12_frac_part(const fixed12_t &fixed) {
    return fixed % FIXED_SCALE;
}

SEQ_FUNC fixed12_t seq_fixed12_new_string(const char *cstr, size_t len) {
    std::string_view s(cstr, len);
    return fixed12_new_string_view(s);
}

SEQ_FUNC fixed12_t fixed12_new_string_view(const std::string_view &s) {
    long long i;
    long long f;
    int64_t sign = (s[0] == '-') ? -1 : 1;
    std::string_view s_ = (sign == -1) ? s.substr(1) : s;
    size_t period = s_.find('.');

    if (period == std::string_view::npos) {
        i = strtoi(s_);
    } else {
        if (period > 0) {
            i = strtoi(s_.substr(0, period));
        }

        std::string fs(s_.substr(period + 1));
        fs += std::string(12 - fs.length(), '0');
        f = strtoi(fs.substr(0, 12));
    }
    return sign * (i * FIXED_SCALE + f);
}

SEQ_FUNC size_t seq_fixed12_to_string(const fixed12_t fixed, char *result) {
    int index = 0;

    bool isNegative = fixed < 0;
    fixed12_t fixed_ = isNegative ? -fixed : fixed;
    int64_t intPart = fixed12_int_part(fixed_);

    do {
        result[index++] = '0' + intPart % 10;
        intPart /= 10;
    } while (intPart > 0);

    if (isNegative) {
        result[index++] = '-';
    }

    // Reverse the integer part
    for (int i = 0, j = index - 1; i < j; i++, j--) {
        char temp = result[i];
        result[i] = result[j];
        result[j] = temp;
    }

    // Check if there is a decimal point

    int64_t fracPart_ = fixed12_frac_part(fixed_);

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
        for (int i = 0; i < fracPartLength - fracPartN; i++) {
            result[index++] = fracPart[i];
        }
    }

    // Null-terminate
    result[index] = '\0';

    return strlen(result);
}

SEQ_FUNC int64_t seq_fixed_mul(int64_t a, int64_t b) {
    auto value = static_cast<big_int>(a) * static_cast<big_int>(b) /
                 static_cast<big_int>(FIXED_SCALE);
    return value.convert_to<int64_t>();
}

SEQ_FUNC int64_t seq_fixed_truediv(int64_t a, int64_t b) {
    auto value = (static_cast<big_int>(a) * static_cast<big_int>(FIXED_SCALE)) /
                 static_cast<big_int>(b);
    return value.convert_to<int64_t>();
}

SEQ_FUNC int64_t seq_fixed_round_to_fractional(int64_t a, int64_t scale) {
    return static_cast<int64_t>(::round(a / static_cast<double>(scale)) *
                                scale);
}

SEQ_FUNC int64_t seq_fixed_round(int64_t a, int decimalPlaces) {
    int64_t scale = std::pow(10, MAX_FRAC_BITS - decimalPlaces);
    return static_cast<int64_t>(::round(a / static_cast<double>(scale)) *
                                scale);
}