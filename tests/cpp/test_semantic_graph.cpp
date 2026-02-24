#include <cmath>
#include <cstdlib>
#include <iostream>
#include "t81/types/T81Graph.hpp"
#include "t81/types/T81Symbol.hpp"

using namespace t81;

void check(bool cond, const char* msg) {
  if (!cond) {
    std::cerr << "Check failed: " << msg << "\n";
    std::exit(1);
  }
}

bool approx_eq(double a, double b, double eps = 1e-4) { return std::abs(a - b) < eps; }

void test_find_by_label() {
  using G = T81Graph<10, 5>;
  G g;

  T81Symbol sym_a = T81Symbol::intern("A");
  T81Symbol sym_b = T81Symbol::intern("B");

  g.label(0, sym_a);
  g.label(1, sym_b);
  g.label(2, sym_a);

  auto found = g.find_by_label(sym_a);

  check(found[0] == 0, "Find A first");
  check(found[1] == 2, "Find A second");
  check(found[2] == G::NodeID(-1), "End of list");

  std::cout << "test_find_by_label passed\n";
}

void test_transitive_inference() {
  using G = T81Graph<10, 5>;
  using W = T81Float<72, 9>;
  G g;

  // 0 -> 1 (w=0.5)
  // 1 -> 2 (w=0.8)
  // Infers 0 -> 2 (w=0.4)

  g.add_edge(0, 1, W(0.5));
  g.add_edge(1, 2, W(0.8));

  G inf = g.transitive_closure_step();

  W w01 = inf.weight(0, 1);
  W w12 = inf.weight(1, 2);
  W w02 = inf.weight(0, 2);

  check(approx_eq(w01.to_double(), 0.5), "Preserve 0->1");
  check(approx_eq(w12.to_double(), 0.8), "Preserve 1->2");
  check(approx_eq(w02.to_double(), 0.4), "Infer 0->2");

  std::cout << "test_transitive_inference passed\n";
}

int main() {
  test_find_by_label();
  test_transitive_inference();
  return 0;
}
