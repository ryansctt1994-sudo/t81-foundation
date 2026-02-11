#include "t81/core/T81Stream.hpp"
#include <cassert>
#include <iostream>

using namespace t81;

void test_naturals() {
    auto n = naturals();
    auto it = n.begin();
    assert((*it).to_int64() == 0);
    std::cout << "test_naturals PASSED\n";
}

int main() {
    test_naturals();
    return 0;
}
