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

    // Case 1: Sufficient capacity (N=42). Should succeed now.
    {
        T81Int<42> v(std::numeric_limits<std::int64_t>::min());
        std::string s = v.to_trit_string();
        assert(s == "0" + expected_trits);

        // Should NOT throw now as index 41 is 0 and index 40 is handled.
        try {
            std::int64_t res = v.to_int64();
            assert(res == std::numeric_limits<std::int64_t>::min());
        } catch (const std::overflow_error& e) {
            std::cerr << "Unexpected overflow for N=42: " << e.what() << "\n";
            assert(false);
        }
    }

    // Case 2: Exact capacity (N=41). Should succeed.
    {
        T81Int<41> v(std::numeric_limits<std::int64_t>::min());
        assert(v.to_trit_string() == expected_trits);
        std::int64_t res = v.to_int64();
        assert(res == std::numeric_limits<std::int64_t>::min());
    }

    // Case 3: Insufficient capacity (N=40). Constructor throws.
    // INT64_MIN requires 41 trits.
    {
        bool threw = false;
        try {
             T81Int<40> v(std::numeric_limits<std::int64_t>::min());
        } catch (const std::overflow_error&) {
            threw = true;
        }
        assert(threw);
    }
}

void test_int64_max_edge_case() {
    std::cout << "Testing INT64_MAX edge case...\n";

    // Case 1: N=41. Should succeed.
    {
        T81Int<41> v(std::numeric_limits<std::int64_t>::max());
        std::int64_t res = v.to_int64();
        assert(res == std::numeric_limits<std::int64_t>::max());
    }

    // Case 2: N=40. Constructor throws.
    // INT64_MAX requires 41 trits.
    {
        bool threw = false;
        try {
             T81Int<40> v(std::numeric_limits<std::int64_t>::max());
        } catch (const std::overflow_error&) {
            threw = true;
        }
        assert(threw);
    }
}

int main() {
    std::cout << "T81Int tests...\n";

    test_int64_min_edge_case();
    test_int64_max_edge_case();

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
