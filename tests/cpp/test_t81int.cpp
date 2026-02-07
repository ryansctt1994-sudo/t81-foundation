#include "t81/core/T81Int.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace t81;

int main() {
    std::cout << "T81Int tests...\n";

    T81Int<8> a{5}, b{-3};
    assert((a + b).to_binary<int>() == 2);
    assert((a - b).to_binary<int>() == 8);

    T81Int<32> x{10}, y{3};
    assert(x / y == T81Int<32>(3));
    assert(x % y == T81Int<32>(1));

    [[maybe_unused]] bool threw= false;
    try { T81Int<32>(1) / T81Int<32>(0); }
    catch (...) { threw = true; }
    assert(threw); bool overflow_trapped = false;
    try {
        [[maybe_unused]] auto v= T81Int<8>::kMaxValue;
        [[maybe_unused]] auto backup= T81Int<8>(1);
        [[maybe_unused]] auto _= v + backup;
            } catch (const std::overflow_error&) {
        overflow_trapped = true;
    }
    assert(overflow_trapped); std::cout << "All T81Int tests passed!\n";
}
