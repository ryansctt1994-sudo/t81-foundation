#include "t81/core/T81BigInt.hpp"
#include <iostream>
#include <cassert>

using namespace t81;
using namespace t81::v1;

void test_multilimb_addition() {
    // 3^40 is larger than int64_t max (~9e18 vs 1e19)
    // Wait, 3^39 is max safe. 3^40 > 2^63-1.

    T81BigInt a(1000000000000000000LL); // 1e18
    T81BigInt b(1000000000000000000LL); // 1e18

    for (int i = 0; i < 20; ++i) {
        a = a + b;
    }

    // a should now be ~2e19, which is larger than int64_t max.
    // It should have 2 limbs if we used a smaller limb size,
    // or it should at least not throw and be correct if we used T81Int<81>.

    std::cout << "BigInt str: " << a.str() << std::endl;
    assert(!a.is_zero());

    T81BigInt c = a - b;
    assert(c < a);

    std::cout << "test_multilimb_addition passed!" << std::endl;
}

int main() {
    test_multilimb_addition();
    return 0;
}
