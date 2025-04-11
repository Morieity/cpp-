#ifndef BIGINTEGER_H
#define BIGINTEGER_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

class BigInteger {
private:
    std::vector<int> digits; // 按位存储，低位在前
    bool negative;           // 符号位

    // 去除前导零
    void removeLeadingZeros();

public:
    // 构造函数
    BigInteger();
    BigInteger(long long num);
    BigInteger(const std::string& str);

    // 转换为字符串
    std::string toString() const;

    // 加法
    BigInteger operator+(const BigInteger& other) const;

    // 减法
    BigInteger operator-(const BigInteger& other) const;

    // 乘法
    BigInteger operator*(const BigInteger& other) const;

    // 除法
    std::pair<BigInteger, BigInteger> divmod(const BigInteger& divisor) const;
    BigInteger operator/(const BigInteger& other) const;
    BigInteger operator%(const BigInteger& other) const;

    // 比较运算符
    bool operator<(const BigInteger& other) const;
    bool operator<=(const BigInteger& other) const;
    bool operator>(const BigInteger& other) const;
    bool operator>=(const BigInteger& other) const;
    bool operator==(const BigInteger& other) const;
    bool operator!=(const BigInteger& other) const;

    // 求幂
    BigInteger pow(int exponent) const;
};

#endif // BIGINTEGER_H