#include "t81/types/T81Agent.hpp"
#include "t81/types/T81Entropy.hpp"
#include "t81/types/T81Network.hpp"
#undef NDEBUG
#include <cassert>
#include <iostream>

using namespace t81;

void test_network_connection_failure() {
  T81List<T81Entropy> fuel;
  fuel.push_back(acquire_entropy());

  auto agent = T81Agent(T81Symbol::intern("NetAgent"), std::move(fuel));
  T81Endpoint remote("127.0.0.1", 9999);

  auto res = T81Network::connect(remote, agent);
  assert(!res.is_ok());
  if (res.error().code != T81Symbol::intern("CANNOT_CONNECT")) {
    std::cerr << "Expected CANNOT_CONNECT, got: " << res.error().code.to_string() << "\n";
    std::cerr << "Message: " << res.error().message.str() << "\n";
  }
  assert(res.error().code == T81Symbol::intern("CANNOT_CONNECT"));
  std::cout << "test_network_connection_failure PASSED\n";
}

int main() {
  test_network_connection_failure();
  std::cout << "All T81Network tests PASSED!\n";
  return 0;
}
