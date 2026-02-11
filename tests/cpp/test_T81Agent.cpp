#include "t81/core/T81Agent.hpp"
#include <cassert>
#include <iostream>

using namespace t81;

void test_identity() {
    auto id = T81Symbol::intern("Socrates");
    T81Agent agent(id);
    assert(agent.identity() == id);
    std::cout << "test_identity PASSED\n";
}

int main() {
    test_identity();
    return 0;
}
