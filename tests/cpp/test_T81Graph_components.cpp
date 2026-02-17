#include <cstdlib>
#include <iostream>
#include "t81/core/T81Graph.hpp"

using namespace t81;

void check(bool cond, const char* msg) {
  if (!cond) {
    std::cerr << "Check failed: " << msg << "\n";
    std::exit(1);
  }
}

void test_connected_components() {
  // Graph with 6 nodes.
  // Component 1: 0 <-> 1 -> 2
  // Component 2: 3 <-> 4
  // Component 3: 5

  T81Graph<6, 3> g;
  // 0 -> 1
  g.add_edge(0, 1);
  // 1 -> 0, 1 -> 2
  g.add_edge(1, 0);
  g.add_edge(1, 2);
  // 2 -> (no edges, but connected via 1)

  // 3 -> 4
  g.add_edge(3, 4);
  // 4 -> 3
  g.add_edge(4, 3);

  // 5 isolated

  auto comps = g.connected_components();

  // Expected IDs:
  // 0, 1, 2 should have ID 0.
  // 3, 4 should have ID 3.
  // 5 should have ID 5.

  check(comps(0) == T81Int<81>(0), "node 0");
  check(comps(1) == T81Int<81>(0), "node 1");
  check(comps(2) == T81Int<81>(0), "node 2");

  check(comps(3) == T81Int<81>(3), "node 3");
  check(comps(4) == T81Int<81>(3), "node 4");

  check(comps(5) == T81Int<81>(5), "node 5");

  std::cout << "test_connected_components passed\n";
}

int main() {
  test_connected_components();
  return 0;
}
