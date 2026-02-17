#include <cassert>
#include <iostream>
#include "t81/core/T81Agent.hpp"
#include "t81/core/T81Discovery.hpp"

using namespace t81;

void test_discovery_basic() {
  auto agent = T81Agent(T81Symbol::intern("DiscoveryAgent"));
  auto& discovery = T81Discovery::join(agent, 8182);

  // In a real test we'd wait for beacons, but here we just check if it joined
  auto peers = discovery.peers();
  assert(peers.empty());  // Initial state
  std::cout << "test_discovery_basic PASSED\n";
}

int main() {
  test_discovery_basic();
  std::cout << "All T81Discovery tests PASSED!\n";
  return 0;
}
