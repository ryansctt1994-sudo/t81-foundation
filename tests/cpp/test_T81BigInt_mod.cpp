#include "t81/core/T81BigInt.hpp"
#include <iostream>
#include <cstdlib>

using namespace t81::v1;

void check(bool cond, const char* msg) {
    if (!cond) {
        std::cerr << "Check failed: " << msg << "\n";
        std::exit(1);
    }
}

void test_pow_mod() {
    // 2^10 mod 1000 = 1024 mod 1000 = 24
    T81BigInt base(2);
    T81BigInt exp(10);
    T81BigInt mod(1000);

    T81BigInt res = T81BigInt::pow_mod(base, exp, mod);
    check(res == T81BigInt(24), "2^10 % 1000");

    // 3^5 mod 10 = 243 mod 10 = 3
    check(T81BigInt::pow_mod(T81BigInt(3), T81BigInt(5), T81BigInt(10)) == T81BigInt(3), "3^5 % 10");

    // Large example: 123^45 % 67
    // pow(123, 45, 67) -> 62
    check(T81BigInt::pow_mod(T81BigInt(123), T81BigInt(45), T81BigInt(67)) == T81BigInt(62), "123^45 % 67");

    std::cout << "test_pow_mod passed\n";
}

int main() {
    test_pow_mod();
    return 0;
}
