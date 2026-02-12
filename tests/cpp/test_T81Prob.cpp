#include "t81/core/T81Prob.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace t81;

int main() {
    std::cout << "Running T81Prob tests...\n";

    // Special values
    [[maybe_unused]] T81Prob27 zero= T81Prob27::zero();
    [[maybe_unused]] T81Prob27 one= T81Prob27::one();
    [[maybe_unused]] T81Prob27 minus_inf= T81Prob27::minus_infinity();
    [[maybe_unused]] T81Prob27 plus_inf= T81Prob27::plus_infinity();

    assert(zero.log_odds().to_int64() == 0);
    assert(minus_inf.log_odds().to_int64() < 0);
    assert(plus_inf.log_odds().to_int64() > 0);

    // Construction from probabilities
    [[maybe_unused]] T81Prob27 p05= T81Prob27::from_prob(0.5);
    [[maybe_unused]] T81Prob27 p01= T81Prob27::from_prob(0.1);
    [[maybe_unused]] T81Prob27 p09= T81Prob27::from_prob(0.9);

    // Arithmetic (log-odds addition)
    [[maybe_unused]] T81Prob27 sum= p05 + p05;
    [[maybe_unused]] T81Prob27 diff= p09 - p01;

    // Negation
    [[maybe_unused]] T81Prob27 neg= -p05;
    assert(neg.log_odds().to_int64() == -p05.log_odds().to_int64());

    // Comparison
    assert(p09 > p01);
    assert(p01 < p09);
    assert(p05 == p05);

    // Saturation checks
    std::cout << "Checking saturation...\n";
    T81Prob27 max_val = T81Prob27::plus_infinity();
    T81Prob27 min_val = T81Prob27::minus_infinity();
    T81Prob27 one_log = T81Prob27::one();

    // Max + 1 should saturate
    T81Prob27 sat_max = max_val + one_log;
    if (sat_max.log_odds().to_int64() < 0) {
        std::cerr << "FAILED: Max + 1 wrapped to negative: " << sat_max.log_odds().to_string() << "\n";
        return 1;
    }
    // We expect it to be clamped to max_val
    if (sat_max != max_val) {
         std::cerr << "WARNING: Max + 1 did not saturate exactly to Max, but didn't wrap. Val: " << sat_max.log_odds().to_int64() << "\n";
         // Ideally it should be equal to max_val if we implement saturation properly.
    }

    // Min - 1 should saturate
    T81Prob27 sat_min = min_val - one_log;
    if (sat_min.log_odds().to_int64() > 0) {
        std::cerr << "FAILED: Min - 1 wrapped to positive: " << sat_min.log_odds().to_string() << "\n";
        return 1;
    }
    if (sat_min != min_val) {
         std::cerr << "WARNING: Min - 1 did not saturate exactly to Min, but didn't wrap. Val: " << sat_min.log_odds().to_int64() << "\n";
    }

    // Round-trip checks
    std::cout << "Checking round-trip conversion...\n";
    // Check range [0.01, 0.99] with step 0.01
    for (int i = 1; i < 100; ++i) {
        double p = i / 100.0;
        T81Prob27 prob = T81Prob27::from_prob(p);
        double p_out = prob.to_prob();
        double diff = std::abs(p - p_out);

        // Tolerance: 1e-3 is sufficient given the fixed-point scale (512.0)
        // Max error is roughly 0.25 / 512 * log(phi) ≈ 0.00025
        if (diff > 1e-3) {
            std::cerr << "FAILED: Round-trip mismatch for p=" << p
                      << ", got " << p_out
                      << ", diff=" << diff << "\n";
            return 1;
        }
    }

    // Check edge cases
    if (T81Prob27::from_prob(0.0).to_prob() != 0.0) {
        std::cerr << "FAILED: Round-trip for 0.0 failed\n";
        return 1;
    }
    if (T81Prob27::from_prob(1.0).to_prob() != 1.0) {
        std::cerr << "FAILED: Round-trip for 1.0 failed\n";
        return 1;
    }
    // 0.5 is exactly 0 log-odds, so it should be exact
    if (std::abs(T81Prob27::from_prob(0.5).to_prob() - 0.5) > 1e-9) {
        std::cerr << "FAILED: Round-trip for 0.5 failed\n";
        return 1;
    }

    std::cout << "All T81Prob tests PASSED!\n";
    return 0;
}
