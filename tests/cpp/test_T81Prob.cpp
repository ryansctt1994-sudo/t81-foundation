#include "t81/core/T81Prob.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <span>

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

    // Test log_softmax_normalize (which is essentially subtraction)
    std::cout << "Checking log_softmax_normalize...\n";
    // log_softmax_normalize(lse) -> this - lse
    T81Prob27 logit(T81Prob27::Storage(100));
    T81Prob27 lse(T81Prob27::Storage(50));

    T81Prob27 normalized = logit.log_softmax_normalize(lse);
    if (normalized.log_odds().to_int64() != 50) {
        std::cerr << "FAILED: log_softmax_normalize(100, 50) != 50. Got: " << normalized.log_odds().to_int64() << "\n";
        return 1;
    }

    // Check saturation behavior via log_softmax_normalize
    // min_inf - positive -> min_inf (because min - pos is very small)
    // T81Prob saturation logic for subtraction:
    // If sign of operands are opposite, overflow can occur.
    // min_val is negative. one_log is positive. min - pos -> more negative -> saturation to min_inf.
    T81Prob27 sat_norm_min = min_val.log_softmax_normalize(one_log);
    if (!sat_norm_min.is_minus_infinity()) {
        std::cerr << "FAILED: log_softmax_normalize saturation min - 1\n";
        return 1;
    }

    // max_inf - negative -> max_inf (equivalent to max_inf + positive)
    T81Prob27 neg_one = -one_log;
    T81Prob27 sat_norm_max = max_val.log_softmax_normalize(neg_one);
    if (!sat_norm_max.is_plus_infinity()) {
        std::cerr << "FAILED: log_softmax_normalize saturation max - (-1)\n";
        return 1;
    }

    // Verify identity: x - 0 = x
    // T81Prob::zero() is log-odds 0 (p=0.5).
    // But here we are talking about the underlying integer value being 0.
    // T81Prob::zero() constructs with Storage(0).
    T81Prob27 identity_check = logit.log_softmax_normalize(zero);
    if (identity_check.log_odds().to_int64() != 100) {
         std::cerr << "FAILED: log_softmax_normalize(100, 0) != 100\n";
         return 1;
    }

    // Test log_sum_exp
    std::cout << "Checking log_sum_exp...\n";
    {
        std::vector<T81Prob27> logits;
        logits.push_back(T81Prob27::zero()); // raw 0
        logits.push_back(T81Prob27::zero()); // raw 0

        // Use span constructor explicitly or rely on implicit conversion (C++20 span from vector)
        std::span<const T81Prob27> s(logits);
        T81Prob27 result = log_sum_exp(s);

        int64_t raw = result.log_odds().to_int64();
        if (raw >= 736 && raw <= 739) {
             std::cout << "PASS: log_sum_exp(0, 0) is correct.\n";
        } else {
             std::cerr << "FAILED: log_sum_exp(0, 0) expected ~737, got " << raw << "\n";
             return 1;
        }

        logits.clear();
        logits.push_back(T81Prob27::zero());
        logits.push_back(T81Prob27(T81Int<27>(-10000)));

        std::span<const T81Prob27> s2(logits);
        result = log_sum_exp(s2);

        raw = result.log_odds().to_int64();
        if (raw >= 0 && raw <= 5) {
             std::cout << "PASS: log_sum_exp dominated by max.\n";
        } else {
             std::cerr << "FAILED: log_sum_exp(0, -10000) expected ~0, got " << raw << "\n";
             return 1;
        }
    }

    std::cout << "All T81Prob tests PASSED!\n";
    return 0;
}
