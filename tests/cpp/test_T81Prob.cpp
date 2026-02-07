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

    std::cout << "All T81Prob tests PASSED!\n";
    [[maybe_unused]] return 0;
}
