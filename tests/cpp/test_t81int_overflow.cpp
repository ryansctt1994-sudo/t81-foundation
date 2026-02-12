#include "t81/core/T81Int.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <vector>

using namespace t81;

// Helper to check if a function throws specific exception
template <typename E, typename F>
bool throws_overflow(F&& f, std::string& out_msg) {
    try {
        f();
        return false;
    } catch (const E& e) {
        out_msg = e.what();
        return true;
    } catch (...) {
        out_msg = "Unknown exception";
        return false;
    }
}

int main() {
    std::cout << "Running T81Int overflow tests...\n";

    bool all_passed = true;

    auto test_safe_limit = [&]() {
        // kMaxSafeTrits is 39. So indices 0..38 are allowed.
        T81Int<45> safe_val;
        // Set all safe trits to P (1)
        // Value = sum(3^0 ... 3^38) = (3^39 - 1) / 2
        for (int i = 0; i < 39; ++i) {
            safe_val[i] = Trit::P;
        }

        try {
            std::int64_t v = safe_val.to_int64();
            // Expected roughly 2e18. int64 max is 9e18. Should be safe.
            if (v <= 0) {
                std::cerr << "FAIL: Safe value conversion returned non-positive: " << v << "\n";
                return false;
            }
            std::cout << "PASS: Max safe trits (0..38) conversion succeeded. Value: " << v << "\n";
            return true;
        } catch (const std::exception& e) {
            std::cerr << "FAIL: Max safe trits conversion threw: " << e.what() << "\n";
            return false;
        }
    };

    auto test_boundary_throw = [&]() {
        // kMaxSafeTrits is 39. So index 39 is disallowed.
        // Even if 3^39 fits in int64_t, the implementation rejects it.
        // We verify this rejection to ensure overflow checks are active.
        T81Int<45> unsafe_val;
        unsafe_val[39] = Trit::P; // Set 40th trit. 3^39.

        std::string msg;
        if (throws_overflow<std::overflow_error>([&]{ [[maybe_unused]] auto _ = unsafe_val.to_int64(); }, msg)) {
             if (msg.find("value out of range") != std::string::npos) {
                 std::cout << "PASS: Index 39 threw 'value out of range' as expected.\n";
                 return true;
             } else {
                 std::cerr << "FAIL: Index 39 threw wrong message: " << msg << "\n";
                 return false;
             }
        } else {
            std::cerr << "FAIL: Index 39 did not throw overflow_error\n";
            return false;
        }
    };

    auto test_huge_value = [&]() {
        // Definitely overflowing value
        T81Int<60> huge_val;
        huge_val[55] = Trit::P; // 3^55
        std::string msg;
        if (throws_overflow<std::overflow_error>([&]{ [[maybe_unused]] auto _ = huge_val.to_int64(); }, msg)) {
            std::cout << "PASS: Huge value threw as expected.\n";
            return true;
        } else {
            std::cerr << "FAIL: Huge value did not throw\n";
            return false;
        }
    };

    // Run tests
    if (!test_safe_limit()) all_passed = false;
    if (!test_boundary_throw()) all_passed = false;
    if (!test_huge_value()) all_passed = false;

    if (all_passed) {
        std::cout << "All T81Int overflow tests passed!\n";
        return 0;
    } else {
        std::cerr << "Some tests failed!\n";
        return 1;
    }
}
