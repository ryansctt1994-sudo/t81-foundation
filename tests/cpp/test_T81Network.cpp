#include "t81/core/T81Network.hpp"
#include "t81/core/T81Agent.hpp"
#include <cassert>
#include <iostream>

using namespace t81;

void test_network_connection_failure() {
    auto agent = T81Agent(T81Symbol::intern("NetAgent"));
    T81Endpoint remote(T81String("127.0.0.1"), 9999);

    auto res = T81Network::connect(remote, agent);
    assert(!res.is_ok());
    assert(res.error().code == T81Symbol::intern("CANNOT_CONNECT"));
    std::cout << "test_network_connection_failure PASSED\n";
}

int main() {
    test_network_connection_failure();
    std::cout << "All T81Network tests PASSED!\n";
    return 0;
}
