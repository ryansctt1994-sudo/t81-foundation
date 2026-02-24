#include <cassert>
#include <iostream>
#include "t81/types/T81Graph.hpp"

using namespace t81;

int main() {
  // 0->1->2, 0->3, 2->3
  T81Graph<4, 3> g;
  g.add_edge(0, 1);
  g.add_edge(1, 2);
  g.add_edge(0, 3);
  g.add_edge(2, 3);

  // BFS from 0:
  // 0: dist 0
  // 1: dist 1 (from 0)
  // 3: dist 1 (from 0)
  // 2: dist 2 (from 1)

  auto dists = g.bfs(0);
  (void)dists;

  assert(dists(0).to_int64() == 0);
  assert(dists(1).to_int64() == 1);
  assert(dists(3).to_int64() == 1);
  assert(dists(2).to_int64() == 2);

  // BFS from 1:
  // 0: unreachable (-1)
  // 1: dist 0
  // 2: dist 1
  // 3: dist 2

  auto dists1 = g.bfs(1);
  (void)dists1;
  assert(dists1(0).to_int64() == -1);
  assert(dists1(1).to_int64() == 0);
  assert(dists1(2).to_int64() == 1);
  assert(dists1(3).to_int64() == 2);

  std::cout << "bfs test passed" << std::endl;
  return 0;
}
