#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <chrono>

// 自定义大整数类
class BigInteger {
private:
    std::vector<int> digits; // 按位存储，低位在前
    bool negative;           // 符号位

public:
    // 构造函数
    BigInteger() : negative(false) {
        digits.push_back(0);
    }

    BigInteger(long long num) {
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

    BigInteger(const std::string& str) {
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
    void removeLeadingZeros() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
        
        if (digits.size() == 1 && digits[0] == 0) {
            negative = false;
        }
    }

    // 转换为字符串
    std::string toString() const {
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
    BigInteger operator+(const BigInteger& other) const {
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
    BigInteger operator-(const BigInteger& other) const {
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
    BigInteger operator*(const BigInteger& other) const {
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

    // 除法
    std::pair<BigInteger, BigInteger> divmod(const BigInteger& divisor) const {
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

    BigInteger operator/(const BigInteger& other) const {
        return divmod(other).first;
    }

    BigInteger operator%(const BigInteger& other) const {
        return divmod(other).second;
    }

    // 比较运算符
    bool operator<(const BigInteger& other) const {
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

    bool operator<=(const BigInteger& other) const {
        return *this < other || *this == other;
    }

    bool operator>(const BigInteger& other) const {
        return !(*this <= other);
    }

    bool operator>=(const BigInteger& other) const {
        return !(*this < other);
    }

    bool operator==(const BigInteger& other) const {
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

    bool operator!=(const BigInteger& other) const {
        return !(*this == other);
    }

    // 求幂
    BigInteger pow(int exponent) const {
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
};

// 定义平方根函数（使用牛顿迭代法）
BigInteger sqrt(const BigInteger& n) {
    if (n <= BigInteger(0)) {
        return BigInteger(0);
    }
    
    BigInteger x = n;
    BigInteger y = (x + n / x) / BigInteger(2);
    
    // 牛顿迭代法求平方根
    while (y < x) {
        x = y;
        y = (x + n / x) / BigInteger(2);
    }
    
    return x;
}

// 辅助函数：阶乘计算
BigInteger factorial(int n) {
    BigInteger result(1);
    for (int i = 2; i <= n; ++i) {
        result = result * BigInteger(i);
    }
    return result;
}

// 使用二项式系数计算阶乘组合
BigInteger binomialCoefficient(int n, int k) {
    if (k < 0 || k > n) return BigInteger(0);
    if (k == 0 || k == n) return BigInteger(1);
    
    BigInteger result(1);
    
    // 优化：使用较小的k值计算
    if (k > n - k) k = n - k;
    
    for (int i = 0; i < k; ++i) {
        result = result * BigInteger(n - i);
        result = result / BigInteger(i + 1);
    }
    
    return result;
}

// Chudnovsky算法计算π
std::string calculatePi(int digits) {
    std::cout << "开始计算π（小数点后" << digits << "位）..." << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 计算需要的项数
    int terms = (int)(digits / 14.1) + 5; // 每项约产生14位，额外加5项以确保精度
    std::cout << "使用Chudnovsky算法，计算" << terms << "项..." << std::endl;
    
    const BigInteger C = BigInteger(640320);
    const BigInteger C3_OVER_24 = (C * C * C) / BigInteger(24);
    
    BigInteger sum(0);
    
    for (int k = 0; k < terms; ++k) {
        // 计算Chudnovsky公式的分子
        BigInteger MK = factorial(6 * k);
        MK = MK * (BigInteger(545140134) * BigInteger(k) + BigInteger(13591409));
        
        // 计算分母
        BigInteger NK = factorial(3 * k);
        NK = NK * factorial(k).pow(3);
        NK = NK * C3_OVER_24.pow(k);
        
        // 根据k的奇偶性确定符号
        if (k % 2 == 1) {
            MK = BigInteger(0) - MK;
        }
        
        // 执行除法并累加结果
        BigInteger term = MK / NK;
        sum = sum + term;
        
        // 进度报告
        if (k % 5 == 0 || k == terms - 1) {
            std::cout << "已完成 " << k + 1 << "/" << terms << " 项 (" 
                      << std::fixed << std::setprecision(1) << (100.0 * (k + 1) / terms) << "%)" << std::endl;
        }
    }
    
    // 应用Chudnovsky公式的常数系数
    BigInteger sqrtTerm = sqrt(BigInteger(10005));
    BigInteger numerator = BigInteger(426880) * sqrtTerm;
    BigInteger pi = numerator / sum;
    
    // 计时结束
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "计算完成！用时 " << elapsed.count() << " 秒" << std::endl;
    
    // 格式化结果
    std::string piStr = pi.toString();
    
    // 插入小数点（π的整数部分是3）
    if (piStr.length() > 1) {
        piStr.insert(1, ".");
    }
    
    // 确保结果长度符合要求
    if (piStr.length() > digits + 2) { // +2是因为"3."占两个字符
        piStr = piStr.substr(0, digits + 2);
    } else {
        // 如果精度不足，用0填充
        while (piStr.length() < digits + 2) {
            piStr += "0";
        }
    }
    
    return piStr;
}

// 优化的Chudnovsky算法实现
std::string calculatePiOptimized(int digits) {
    std::cout << "使用优化的Chudnovsky算法计算π（小数点后" << digits << "位）..." << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 计算需要的项数
    int terms = (int)(digits / 14.1) + 5;
    std::cout << "计算" << terms << "项..." << std::endl;
    
    // Chudnovsky算法的常数
    const BigInteger A = BigInteger(13591409);
    const BigInteger B = BigInteger(545140134);
    const BigInteger C = BigInteger(640320);
    const BigInteger D = BigInteger(426880);
    const BigInteger E = BigInteger(10005);
    const BigInteger C3_24 = (C * C * C) / BigInteger(24);
    
    BigInteger sumA = A;
    BigInteger sumB = BigInteger(1);
    
    // 二进制分割法计算Pi
    for (int k = 1; k < terms; ++k) {
        // 迭代计算Chudnovsky公式
        BigInteger numerator = BigInteger(6 * k - 5) * BigInteger(6 * k - 4) * BigInteger(6 * k - 3);
        BigInteger term_A = A + B * BigInteger(k);
        
        // 计算迭代项
        BigInteger denominator = BigInteger(k * k * k) * C3_24;
        
        sumA = sumA * denominator + term_A * sumB * numerator;
        sumB = sumB * denominator;
        
        // 定期显示进度
        if (k % 10 == 0 || k == terms - 1) {
            std::cout << "已完成 " << k << "/" << terms << " 项 (" 
                      << std::fixed << std::setprecision(1) << (100.0 * k / terms) << "%)" << std::endl;
        }
    }
    
    // 应用最终系数
    BigInteger sqrtE = sqrt(E);
    BigInteger numerator = D * sqrtE * sumB;
    BigInteger pi = numerator / sumA;
    
    // 计时结束
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "计算完成！用时 " << elapsed.count() << " 秒" << std::endl;
    
    // 格式化结果
    std::string piStr = pi.toString();
    
    // 插入小数点
    if (piStr.length() > 1) {
        piStr.insert(1, ".");
    }
    
    // 确保结果长度符合要求
    if (piStr.length() > digits + 2) {
        return piStr.substr(0, digits + 2);
    } else {
        // 如果精度不足，用0填充
        while (piStr.length() < digits + 2) {
            piStr += "0";
        }
        return piStr;
    }
}

// 使用Ramanujan公式计算π
std::string calculatePiRamanujan(int digits) {
    std::cout << "使用Ramanujan公式计算π（小数点后" << digits << "位）..." << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 计算需要的项数
    int terms = (int)(digits / 8.0) + 2; // Ramanujan公式每项约产生8位
    std::cout << "计算" << terms << "项..." << std::endl;
    
    const BigInteger FOUR = BigInteger(4);
    const BigInteger SQRT8 = BigInteger(2) * sqrt(BigInteger(2));
    const BigInteger NINEONEFOUR = BigInteger(9801);
    
    BigInteger sum(0);
    
    for (int k = 0; k < terms; ++k) {
        BigInteger numerator = factorial(4 * k) * (BigInteger(1103) + BigInteger(26390) * BigInteger(k));
        BigInteger denominator = factorial(k).pow(4) * BigInteger(396).pow(4 * k);
        
        BigInteger term = numerator / denominator;
        sum = sum + term;
        
        if (k % 5 == 0 || k == terms - 1) {
            std::cout << "已完成 " << k + 1 << "/" << terms << " 项 (" 
                      << std::fixed << std::setprecision(1) << (100.0 * (k + 1) / terms) << "%)" << std::endl;
        }
    }
    
    // 应用Ramanujan公式的常数系数
    BigInteger pi = (SQRT8 / NINEONEFOUR) / sum;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "计算完成！用时 " << elapsed.count() << " 秒" << std::endl;
    
    // 格式化结果
    std::string piStr = pi.toString();
    
    // 插入小数点
    if (piStr.length() > 1) {
        piStr.insert(1, ".");
    }
    
    // 确保结果长度符合要求
    if (piStr.length() > digits + 2) {
        return piStr.substr(0, digits + 2);
    } else {
        while (piStr.length() < digits + 2) {
            piStr += "0";
        }
        return piStr;
    }
}

int main(int argc, char* argv[]) {
    int digits = 100; // 默认计算100位小数
    std::string algorithm = "chudnovsky"; // 默认使用Chudnovsky算法
    
    // 处理命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-d" || arg == "--digits") {
            if (i + 1 < argc) {
                try {
                    digits = std::stoi(argv[i + 1]);
                    if (digits <= 0) {
                        std::cerr << "位数必须为正整数" << std::endl;
                        return 1;
                    }
                    ++i;
                } catch (const std::exception& e) {
                    std::cerr << "无效的位数: " << argv[i + 1] << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "请在 " << arg << " 参数后指定位数" << std::endl;
                return 1;
            }
        } else if (arg == "-a" || arg == "--algorithm") {
            if (i + 1 < argc) {
                algorithm = argv[i + 1];
                ++i;
            } else {
                std::cerr << "请在 " << arg << " 参数后指定算法" << std::endl;
                return 1;
            }
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "用法: " << argv[0] << " [选项]" << std::endl;
            std::cout << "选项:" << std::endl;
            std::cout << "  -d, --digits N      计算π到小数点后N位" << std::endl;
            std::cout << "  -a, --algorithm ALG 使用指定算法 (chudnovsky, optimized, ramanujan)" << std::endl;
            std::cout << "  -h, --help          显示此帮助信息" << std::endl;
            return 0;
        } else {
            try {
                // 兼容简单地直接提供位数的情况
                digits = std::stoi(arg);
                if (digits <= 0) {
                    std::cerr << "位数必须为正整数" << std::endl;
                    return 1;
                }
            } catch (const std::exception& e) {
                std::cerr << "未知参数: " << arg << std::endl;
                return 1;
            }
        }
    }
    
    std::cout << "计算π到小数点后" << digits << "位，使用" << algorithm << "算法" << std::endl;
    
    std::string pi;
    
    // 根据选择的算法计算π
    if (algorithm == "chudnovsky") {
        pi = calculatePi(digits);
    } else if (algorithm == "optimized") {
        pi = calculatePiOptimized(digits);
    } else if (algorithm == "ramanujan") {
        pi = calculatePiRamanujan(digits);
    } else {
        std::cerr << "未知算法: " << algorithm << std::endl;
        std::cerr << "支持的算法: chudnovsky, optimized, ramanujan" << std::endl;
        return 1;
    }
    
    // 输出结果
    std::cout << "π = " << pi << std::endl;
    
    // 将结果保存到文件
    std::string filename = "pi_" + std::to_string(digits) + "_digits.txt";
    std::ofstream file(filename);
    if (file.is_open()) {
        file << pi;
        file.close();
        std::cout << "结果已保存到 " << filename << std::endl;
    } else {
        std::cerr << "无法创建文件" << std::endl;
    }
    
    return 0;
}