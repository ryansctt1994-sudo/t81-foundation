/**
 * @file test_T81Graph_algorithms.cpp
 * @brief Tests for T81Graph cycle detection and topological sort.
 */

#include <iostream>
#include <vector>
#include <cassert>
#include "t81/core/T81Graph.hpp"
#include "t81/core/T81Int.hpp"

using namespace t81;

// Helper macro for assertions
#define EXPECT(cond, msg) \
  if (!(cond)) { \
    std::cerr << "FAIL: " << msg << " (" << #cond << ")\n"; \
    std::exit(1); \
  }

void test_cycle_detection() {
  std::cout << "Testing cycle detection..." << std::endl;

  // Acyclic graph
  // 0 -> 1 -> 2
  {
    T81Graph<3, 2> g;
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    EXPECT(!g.has_cycle(), "Graph 0->1->2 should be acyclic");
  }

  // Simple cycle
  // 0 -> 1 -> 0
  {
    T81Graph<2, 2> g;
    g.add_edge(0, 1);
    g.add_edge(1, 0);
    EXPECT(g.has_cycle(), "Graph 0->1->0 should have a cycle");
  }

  // Self loop
  // 0 -> 0
  {
    T81Graph<1, 1> g;
    g.add_edge(0, 0);
    EXPECT(g.has_cycle(), "Self-loop should be detected as a cycle");
  }

  // Disconnected components with cycle
  // 0 -> 1    2 -> 3 -> 2
  {
    T81Graph<4, 2> g;
    g.add_edge(0, 1);
    g.add_edge(2, 3);
    g.add_edge(3, 2);
    EXPECT(g.has_cycle(), "Disconnected component with cycle should be detected");
  }

  // Diamond (acyclic)
  //   1
  // 0/ \3
  //  \ /
  //   2
  {
    T81Graph<4, 2> g;
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 3);
    g.add_edge(2, 3);
    EXPECT(!g.has_cycle(), "Diamond graph should be acyclic");
  }

  std::cout << "Cycle detection passed!" << std::endl;
}

void test_topological_sort() {
  std::cout << "Testing topological sort..." << std::endl;

  // Simple line: 0 -> 1 -> 2
  {
    T81Graph<3, 2> g;
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    auto sorted = g.topological_sort();
    EXPECT(sorted.has_value(), "Line graph should have topological sort");

    auto& res = *sorted;
    // Expected: 0, 1, 2
    EXPECT(res(0).to_int64() == 0, "First element should be 0");
    EXPECT(res(1).to_int64() == 1, "Second element should be 1");
    EXPECT(res(2).to_int64() == 2, "Third element should be 2");
  }

  // Fork: 0 -> 1, 0 -> 2
  {
    T81Graph<3, 2> g;
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    auto sorted = g.topological_sort();
    EXPECT(sorted.has_value(), "Fork graph should have topological sort");

    auto& res = *sorted;
    // 0 must come before 1 and 2
    int64_t p0 = -1, p1 = -1, p2 = -1;
    for(size_t i=0; i<3; ++i) {
        if (res(i).to_int64() == 0) p0 = i;
        if (res(i).to_int64() == 1) p1 = i;
        if (res(i).to_int64() == 2) p2 = i;
    }
    EXPECT(p0 < p1, "0 must precede 1");
    EXPECT(p0 < p2, "0 must precede 2");
  }

  // Cycle: 0 -> 1 -> 0
  {
    T81Graph<2, 2> g;
    g.add_edge(0, 1);
    g.add_edge(1, 0);
    auto sorted = g.topological_sort();
    EXPECT(!sorted.has_value(), "Cyclic graph should not have topological sort");
  }

  std::cout << "Topological sort passed!" << std::endl;
}

int main() {
  test_cycle_detection();
  test_topological_sort();
  return 0;
}
