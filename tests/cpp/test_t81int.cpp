#undef NDEBUG
#include "t81/core/T81Int.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <string>

using namespace t81;

// Helper function to test the INT64_MIN edge case
void test_int64_min_edge_case() {
    std::cout << "Testing INT64_MIN edge case...\n";
    // INT64_MIN = -9223372036854775808
    // Requires 41 trits.
    // Trit string (most significant first): -+-+---00---00+-0-++--+0-0-0+0+0--+0+-00+
    const std::string expected_trits = "-+-+---00---00+-0-++--+0-0-0+0+0--+0+-00+";

    // Case 1: Sufficient capacity (N=42)
    // Should have leading zero since N=42 and we need 41 trits
    {
        T81Int<42> v(std::numeric_limits<std::int64_t>::min());
        std::string s = v.to_trit_string();
        assert(s == "0" + expected_trits);

        // to_int64() should throw because N > 39 (kMaxSafeTrits) and we have non-zero trits beyond index 38
        bool threw = false;
        try {
            [[maybe_unused]] auto i = v.to_int64();
        } catch (const std::overflow_error&) {
            threw = true;
        }
        assert(threw);
    }

    // Case 2: Exact capacity (N=41)
    {
        T81Int<41> v(std::numeric_limits<std::int64_t>::min());
        assert(v.to_trit_string() == expected_trits);
    }

    // Case 3: Insufficient capacity (N=39)
    // Should throw std::overflow_error in constructor/assign_from_int64
    {
        bool threw = false;
        try {
             T81Int<39> v(std::numeric_limits<std::int64_t>::min());
        } catch (const std::overflow_error&) {
            threw = true;
        }
        assert(threw);
    }
}

int main() {
    std::cout << "T81Int tests...\n";

    test_int64_min_edge_case();

    T81Int<8> a{5}, b{-3};
    assert((a + b).to_binary<int>() == 2);
    assert((a - b).to_binary<int>() == 8);

    T81Int<32> x{10}, y{3};
    assert(x / y == T81Int<32>(3));
    assert(x % y == T81Int<32>(1));

    [[maybe_unused]] bool threw= false;
    try { T81Int<32>(1) / T81Int<32>(0); }
    catch (...) { threw = true; }
    assert(threw);

    bool overflow_trapped = false;
    try {
        [[maybe_unused]] auto v= T81Int<8>::kMaxValue;
        [[maybe_unused]] auto backup= T81Int<8>(1);
        [[maybe_unused]] auto _= v + backup;
            } catch (const std::overflow_error&) {
        overflow_trapped = true;
    }
    assert(overflow_trapped);

    std::cout << "All T81Int tests passed!\n";
}
