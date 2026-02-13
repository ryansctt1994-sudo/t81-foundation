#include "t81/core/T81Fraction.hpp"
#include <cstdlib>
#include <iostream>
#include <cmath>

using namespace t81;
using namespace t81::v1;

// Use 27 trits for testing, similar to T81Float<27, 9> mantissa size
// 3^27 is large enough for most test cases.
using Frac = T81Fraction<27>;
using Int = T81Int<27>;

void check(bool condition, const char* msg) {
    if (!condition) {
        std::cerr << "FAIL: " << msg << std::endl;
        std::exit(1);
    }
}

void test_canonicalization_basics() {
    std::cout << "Testing canonicalization basics...\n";

    // 1. Zero denominator -> 0/1
    {
        Frac f(Int(5), Int(0));
        check(f.num().is_zero(), "5/0 -> num should be 0");
        check(f.den() == Int(1), "5/0 -> den should be 1");
    }

    // 2. Zero numerator -> 0/1
    {
        Frac f(Int(0), Int(5));
        check(f.num().is_zero(), "0/5 -> num should be 0");
        check(f.den() == Int(1), "0/5 -> den should be 1");
    }
    {
        Frac f(Int(0), Int(-5));
        check(f.num().is_zero(), "0/-5 -> num should be 0");
        check(f.den() == Int(1), "0/-5 -> den should be 1");
    }

    // 3. Negative denominator -> sign flip
    {
        Frac f(Int(1), Int(-2));
        check(f.num() == Int(-1), "1/-2 -> num should be -1");
        check(f.den() == Int(2), "1/-2 -> den should be 2");
    }
    {
        Frac f(Int(-1), Int(-2));
        check(f.num() == Int(1), "-1/-2 -> num should be 1");
        check(f.den() == Int(2), "-1/-2 -> den should be 2");
    }

    // 4. GCD reduction
    {
        // 2/4 -> 1/2
        Frac f(Int(2), Int(4));
        check(f.num() == Int(1), "2/4 -> num should be 1");
        check(f.den() == Int(2), "2/4 -> den should be 2");
    }
    {
        // -2/4 -> -1/2
        Frac f(Int(-2), Int(4));
        check(f.num() == Int(-1), "-2/4 -> num should be -1");
        check(f.den() == Int(2), "-2/4 -> den should be 2");
    }
    {
        // 2/-4 -> -1/2
        Frac f(Int(2), Int(-4));
        check(f.num() == Int(-1), "2/-4 -> num should be -1");
        check(f.den() == Int(2), "2/-4 -> den should be 2");
    }
    {
        // -2/-4 -> 1/2
        Frac f(Int(-2), Int(-4));
        check(f.num() == Int(1), "-2/-4 -> num should be 1");
        check(f.den() == Int(2), "-2/-4 -> den should be 2");
    }
    {
        // 3/9 -> 1/3
        Frac f(Int(3), Int(9));
        check(f.num() == Int(1), "3/9 -> num should be 1");
        check(f.den() == Int(3), "3/9 -> den should be 3");
    }
    {
        // 12/18 -> 2/3
        Frac f(Int(12), Int(18));
        check(f.num() == Int(2), "12/18 -> num should be 2");
        check(f.den() == Int(3), "12/18 -> den should be 3");
    }
}

void test_arithmetic_canonicalization() {
    std::cout << "Testing arithmetic canonicalization...\n";

    Frac half(Int(1), Int(2));
    Frac third(Int(1), Int(3));

    // 1/2 + 1/3 = 5/6
    Frac sum = half + third;
    check(sum.num() == Int(5), "1/2 + 1/3 -> num should be 5");
    check(sum.den() == Int(6), "1/2 + 1/3 -> den should be 6");

    // 1/3 + 1/3 = 2/3 (already reduced)
    Frac two_thirds = third + third;
    check(two_thirds.num() == Int(2), "1/3 + 1/3 -> num should be 2");
    check(two_thirds.den() == Int(3), "1/3 + 1/3 -> den should be 3");

    // 1/3 + 2/3 = 3/3 = 1/1
    Frac one = third + Frac(Int(2), Int(3));
    check(one.num() == Int(1), "1/3 + 2/3 -> num should be 1");
    check(one.den() == Int(1), "1/3 + 2/3 -> den should be 1");

    // 1/2 - 1/2 = 0/1
    Frac zero = half - half;
    check(zero.num().is_zero(), "1/2 - 1/2 -> num should be 0");
    check(zero.den() == Int(1), "1/2 - 1/2 -> den should be 1");

    // 2/3 * 3/4 = 6/12 = 1/2
    Frac prod = Frac(Int(2), Int(3)) * Frac(Int(3), Int(4));
    check(prod.num() == Int(1), "2/3 * 3/4 -> num should be 1");
    check(prod.den() == Int(2), "2/3 * 3/4 -> den should be 2");

    // 1/2 / 1/2 = 1/1
    Frac quot = half / half;
    check(quot.num() == Int(1), "1/2 / 1/2 -> num should be 1");
    check(quot.den() == Int(1), "1/2 / 1/2 -> den should be 1");
}

void test_from_double() {
    std::cout << "Testing from_double...\n";

    // 0.5 -> 1/2
    Frac f1 = Frac::from_double(0.5);
    check(f1.num() == Int(1), "0.5 -> num should be 1");
    check(f1.den() == Int(2), "0.5 -> den should be 2");

    // 0.333333333333 -> 1/3 (approx)
    // from_double is best-effort with continued fractions
    Frac f2 = Frac::from_double(1.0/3.0);
    check(f2.num() == Int(1), "1/3 -> num should be 1");
    check(f2.den() == Int(3), "1/3 -> den should be 3");

    // -0.5 -> -1/2
    Frac f3 = Frac::from_double(-0.5);
    check(f3.num() == Int(-1), "-0.5 -> num should be -1");
    check(f3.den() == Int(2), "-0.5 -> den should be 2");

    // 0.0 -> 0/1
    Frac f4 = Frac::from_double(0.0);
    check(f4.num().is_zero(), "0.0 -> num should be 0");
    check(f4.den() == Int(1), "0.0 -> den should be 1");
}

int main() {
    try {
        test_canonicalization_basics();
        test_arithmetic_canonicalization();
        test_from_double();
        std::cout << "test_T81Fraction PASSED\n";
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
