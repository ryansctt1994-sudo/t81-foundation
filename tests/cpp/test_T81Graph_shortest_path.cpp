#include <cmath>
#include <cstdlib>
#include <iostream>
#include "t81/types/T81Graph.hpp"

using namespace t81;

// Helper to check float equality
bool approx_eq(double a, double b, double eps = 1e-3) { return std::abs(a - b) < eps; }

void check(bool cond, const char* msg) {
  if (!cond) {
    std::cerr << "Check failed: " << msg << "\n";
    std::exit(1);
  }
}

void test_shortest_path() {
  // 0 -> 1 (w=1)
  // 0 -> 2 (w=4)
  // 1 -> 2 (w=2)
  // 1 -> 3 (w=6)
  // 2 -> 3 (w=3)

  // Shortest paths from 0:
  // 0: 0
  // 1: 1
  // 2: 1 + 2 = 3 (better than 4)
  // 3: 3 + 3 = 6 (better than 1->3 which is 1+6=7)

  T81Graph<4, 4> g;
  using W = T81Float<72, 9>;

  g.add_edge(0, 1, W(1));
  g.add_edge(0, 2, W(4));
  g.add_edge(1, 2, W(2));
  g.add_edge(1, 3, W(6));
  g.add_edge(2, 3, W(3));

  auto dists = g.shortest_path(0);

  check(approx_eq(dists(0).to_double(), 0.0), "dist 0");
  check(approx_eq(dists(1).to_double(), 1.0), "dist 1");
  check(approx_eq(dists(2).to_double(), 3.0), "dist 2");
  check(approx_eq(dists(3).to_double(), 6.0), "dist 3");

  std::cout << "test_shortest_path passed\n";
}

int main() {
  test_shortest_path();
  return 0;
}
