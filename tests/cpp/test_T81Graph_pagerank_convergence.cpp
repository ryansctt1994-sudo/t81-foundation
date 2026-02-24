#include <cassert>
#include <iostream>
#include "t81/types/T81Graph.hpp"

using namespace t81;

int main() {
  // 3 nodes loop: 0->1->2->0
  T81Graph<3, 3> g;
  g.add_edge(0, 1);
  g.add_edge(1, 2);
  g.add_edge(2, 0);

  // Uniform distribution should be stationary 1/3
  auto ranks = pagerank(g, 100, Weight81(1e-9));

  double sum = 0;
  for (size_t i = 0; i < 3; ++i) {
    double r = ranks(i).to_double();
    sum += r;
    assert(std::abs(r - 1.0 / 3.0) < 0.01);
  }
  (void)sum;
  assert(std::abs(sum - 1.0) < 0.01);

  std::cout << "pagerank convergence test passed" << std::endl;
  return 0;
}
