#include "t81/bigint.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

using namespace t81;

void test_canonical_invariants() {
    [[maybe_unused]] T81BigInt a= T81BigInt::from_i64(0);
    assert(a.is_zero());
    // Internal check: d_ should be empty for zero
    // But d_ is private. We rely on to_string() and is_zero().
    assert(a.to_string() == "0");

    [[maybe_unused]] T81BigInt b= T81BigInt::from_i64(81);
    // 81 in base 81 is 1.0 (MSB first)
    assert(b.to_string() == "1.0");

    [[maybe_unused]] T81BigInt c= b - b;
    assert(c.is_zero());
    assert(c.to_string() == "0");
}

void test_base81_roundtrip() {
    [[maybe_unused]] std::vector<int64_t> test_cases= {0, 1, -1, 80, 81, 82, 6560, 6561, 6562, -6561, 123456789, -123456789};
    for (auto v : test_cases) {
        [[maybe_unused]] T81BigInt a= T81BigInt::from_i64(v);
        [[maybe_unused]] std::string s= a.to_base81_string();
        [[maybe_unused]] T81BigInt b= T81BigInt::from_base81_string(s);
        assert(a == b);
    }
}

void test_carries_and_borrows() {
    T81BigInt a(80);
    T81BigInt b(1);
    [[maybe_unused]] T81BigInt c= a + b;
    assert(c.to_string() == "1.0");

    [[maybe_unused]] T81BigInt d= c - b;
    assert(d == a);

    [[maybe_unused]] T81BigInt e= T81BigInt::from_i64(-1);
    [[maybe_unused]] T81BigInt f= T81BigInt::from_i64(-80);
    [[maybe_unused]] T81BigInt g= e + f;
    assert(g.to_string() == "-1.0");
}

void test_long_division() {
    // Exact division
    [[maybe_unused]] T81BigInt a= T81BigInt::from_i64(999999);
    [[maybe_unused]] T81BigInt b= T81BigInt::from_i64(3);
    [[maybe_unused]] T81BigInt q= T81BigInt::div(a, b);
    assert(q.to_int64() == 333333);

    // Modulo
    [[maybe_unused]] T81BigInt a2= T81BigInt::from_i64(1000000);
    [[maybe_unused]] T81BigInt r= T81BigInt::mod(a2, b);
    assert(r.to_int64() == 1);

    // DivMod
    [[maybe_unused]] auto dm= divmod(a2, b);
    assert(dm.q.to_int64() == 333333);
    assert(dm.r.to_int64() == 1);
}

void test_property_addition_subtraction() {
    for (int64_t i = -1000; i < 1000; ++i) {
        for (int64_t j = -1000; j < 1000; j += 100) {
            T81BigInt a(i);
            T81BigInt b(j);
            [[maybe_unused]] T81BigInt c= a + b;
            [[maybe_unused]] T81BigInt d= c - b;
            assert(d == a);
        }
    }
}

int main() {
    test_canonical_invariants();
    test_base81_roundtrip();
    test_carries_and_borrows();
    test_long_division();
    test_property_addition_subtraction();
    std::cout << "T81BigInt V1 Baseline Tests Passed!" << std::endl;
    return 0;
}
