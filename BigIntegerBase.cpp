#include "BigInteger.h"
#include <cmath>
#include <cctype>

// 构造函数
BigInteger::BigInteger() : negative(false) {
    digits.push_back(0);
}

BigInteger::BigInteger(long long num) {
    negative = (num < 0);
    num = std::abs(num);

    if (num == 0) {
        digits.push_back(0);
    } else {
        while (num > 0) {
            digits.push_back(num % 10);
            num /= 10;
        }
    }
}

BigInteger::BigInteger(const std::string& str) {
    negative = false;
    digits.clear();

    int start = 0;
    if (str[0] == '-') {
        negative = true;
        start = 1;
    }

    for (int i = str.length() - 1; i >= start; --i) {
        if (std::isdigit(str[i])) {
            digits.push_back(str[i] - '0');
        }
    }

    if (digits.empty()) {
        digits.push_back(0);
    }

    removeLeadingZeros();
}

// 去除前导零
void BigInteger::removeLeadingZeros() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }

    if (digits.size() == 1 && digits[0] == 0) {
        negative = false;
    }
}

// 转换为字符串
std::string BigInteger::toString() const {
    std::string result = "";
    if (negative && !(digits.size() == 1 && digits[0] == 0)) {
        result += "-";
    }

    for (int i = digits.size() - 1; i >= 0; --i) {
        result += std::to_string(digits[i]);
    }

    return result;
}

// 加法
BigInteger BigInteger::operator+(const BigInteger& other) const {
    if (negative != other.negative) {
        if (negative) {
            BigInteger temp = *this;
            temp.negative = false;
            return other - temp;
        } else {
            BigInteger temp = other;
            temp.negative = false;
            return *this - temp;
        }
    }

    BigInteger result;
    result.digits.clear();

    int carry = 0;
    size_t maxSize = std::max(digits.size(), other.digits.size());

    for (size_t i = 0; i < maxSize || carry; ++i) {
        int sum = carry;
        if (i < digits.size()) sum += digits[i];
        if (i < other.digits.size()) sum += other.digits[i];

        result.digits.push_back(sum % 10);
        carry = sum / 10;
    }

    result.negative = negative;
    result.removeLeadingZeros();
    return result;
}

// 减法
BigInteger BigInteger::operator-(const BigInteger& other) const {
    if (negative != other.negative) {
        BigInteger temp = other;
        temp.negative = !temp.negative;
        return *this + temp;
    }

    if (negative) {
        BigInteger a = *this;
        BigInteger b = other;
        a.negative = false;
        b.negative = false;
        return b - a;
    }

    if (*this < other) {
        BigInteger result = other - *this;
        result.negative = true;
        return result;
    }

    BigInteger result;
    result.digits.clear();

    int borrow = 0;

    for (size_t i = 0; i < digits.size(); ++i) {
        int diff = digits[i] - borrow;
        if (i < other.digits.size()) diff -= other.digits[i];

        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }

        result.digits.push_back(diff);
    }

    result.removeLeadingZeros();
    return result;
}

// 乘法
BigInteger BigInteger::operator*(const BigInteger& other) const {
    if (digits.size() == 1 && digits[0] == 0 || other.digits.size() == 1 && other.digits[0] == 0) {
        return BigInteger(0);
    }

    BigInteger result;
    result.digits.resize(digits.size() + other.digits.size(), 0);

    for (size_t i = 0; i < digits.size(); ++i) {
        int carry = 0;
        for (size_t j = 0; j < other.digits.size() || carry; ++j) {
            long long curr = result.digits[i + j] + carry;
            if (j < other.digits.size()) {
                curr += (long long)digits[i] * other.digits[j];
            }

            result.digits[i + j] = curr % 10;
            carry = curr / 10;
        }
    }

    result.negative = negative != other.negative;
    result.removeLeadingZeros();
    return result;
}

// 比较运算符 (<, <=, >, >=)
bool BigInteger::operator<(const BigInteger& other) const {
    if (negative != other.negative) {
        return negative;
    }

    if (digits.size() != other.digits.size()) {
        return negative ? digits.size() > other.digits.size() : digits.size() < other.digits.size();
    }

    for (int i = digits.size() - 1; i >= 0; --i) {
        if (digits[i] != other.digits[i]) {
            return negative ? digits[i] > other.digits[i] : digits[i] < other.digits[i];
        }
    }

    return false;
}

bool BigInteger::operator<=(const BigInteger& other) const {
    return *this < other || *this == other;
}

bool BigInteger::operator>(const BigInteger& other) const {
    return !(*this <= other);
}

bool BigInteger::operator>=(const BigInteger& other) const {
    return !(*this < other);
}