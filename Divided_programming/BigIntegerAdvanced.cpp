#include "BigInteger.h"
#include <stdexcept>

// 除法
std::pair<BigInteger, BigInteger> BigInteger::divmod(const BigInteger& divisor) const {
    if (divisor.digits.size() == 1 && divisor.digits[0] == 0) {
        throw std::runtime_error("Division by zero");
    }

    if (*this < divisor) {
        return { BigInteger(0), *this };
    }

    BigInteger a = *this;
    BigInteger b = divisor;
    a.negative = false;
    b.negative = false;

    BigInteger quotient;
    BigInteger remainder;

    quotient.digits.resize(a.digits.size(), 0);

    for (int i = a.digits.size() - 1; i >= 0; --i) {
        remainder.digits.insert(remainder.digits.begin(), a.digits[i]);
        remainder.removeLeadingZeros();

        int q = 0;
        int l = 0, r = 9;
        while (l <= r) {
            int mid = (l + r) / 2;
            BigInteger temp = b * BigInteger(mid);
            if (temp <= remainder) {
                q = mid;
                l = mid + 1;
            } else {
                r = mid - 1;
            }
        }

        quotient.digits[i] = q;
        remainder = remainder - b * BigInteger(q);
    }

    quotient.removeLeadingZeros();
    quotient.negative = negative != divisor.negative;
    remainder.negative = negative;

    return { quotient, remainder };
}

BigInteger BigInteger::operator/(const BigInteger& other) const {
    return divmod(other).first;
}

BigInteger BigInteger::operator%(const BigInteger& other) const {
    return divmod(other).second;
}

// 相等和不等运算符
bool BigInteger::operator==(const BigInteger& other) const {
    if (negative != other.negative) {
        return false;
    }

    if (digits.size() != other.digits.size()) {
        return false;
    }

    for (size_t i = 0; i < digits.size(); ++i) {
        if (digits[i] != other.digits[i]) {
            return false;
        }
    }

    return true;
}

bool BigInteger::operator!=(const BigInteger& other) const {
    return !(*this == other);
}

// 求幂
BigInteger BigInteger::pow(int exponent) const {
    if (exponent < 0) {
        throw std::runtime_error("Negative exponent not supported");
    }

    if (exponent == 0) {
        return BigInteger(1);
    }

    if (exponent == 1) {
        return *this;
    }

    if (exponent % 2 == 0) {
        BigInteger half = pow(exponent / 2);
        return half * half;
    } else {
        return *this * pow(exponent - 1);
    }
}