#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <iostream>
#include <vector>

#include "t81/core/T81Complex.hpp"
#include "t81/core/T81Float.hpp"
#include "t81/core/T81Graph.hpp"
#include "t81/core/T81List.hpp"
#include "t81/core/T81Matrix.hpp"
#include "t81/core/T81Tensor.hpp"

#define TEST_ASSERT(cond)                                                                 \
  do {                                                                                    \
    if (!(cond)) {                                                                        \
      std::cerr << "Assertion failed: " << #cond << " at " << __FILE__ << ":" << __LINE__ \
                << std::endl;                                                             \
      std::exit(1);                                                                       \
    }                                                                                     \
  } while (0)

#define TEST_ASSERT_FLOAT(val, expected)                                                        \
  do {                                                                                          \
    if (std::abs((val).to_double() - (expected).to_double()) > 1e-4) {                          \
      std::cerr << "Assertion failed: " << #val << " == " << #expected << " at " << __FILE__    \
                << ":" << __LINE__ << std::endl;                                                \
      std::cerr << "  Value: " << (val).to_double() << ", Expected: " << (expected).to_double() \
                << std::endl;                                                                   \
      std::exit(1);                                                                             \
    }                                                                                           \
  } while (0)

using namespace t81;

void test_complex_features() {
  std::cout << "Testing T81Complex features...\n";
  using C = T81Complex<18>;
  using F = C::FloatType;

  // Division
  C a(F::from_double(1.0), F::from_double(2.0));
  C b(F::from_double(3.0), F::from_double(4.0));
  // a/b = (1+2i)/(3+4i) = (1+2i)(3-4i)/25 = (3 -4i +6i +8)/25 = (11+2i)/25 = 0.44 + 0.08i
  C div = a / b;
  TEST_ASSERT_FLOAT(div.re, F::from_double(0.44));
  TEST_ASSERT_FLOAT(div.im, F::from_double(0.08));

  // Sqrt
  C z(F::from_double(3.0), F::from_double(4.0));
  C s = sqrt(z);
  // sqrt(3+4i) = 2+i or -2-i
  // 2+i: 4 + 4i - 1 = 3 + 4i. Correct.
  // T81Float usually returns positive real part for sqrt? std::sqrt does principal branch.
  TEST_ASSERT_FLOAT(s.re, F::from_double(2.0));
  TEST_ASSERT_FLOAT(s.im, F::from_double(1.0));

  // Exp
  C z2(F::from_double(0.0), F::from_double(3.14159265359));  // i*pi
  C e = exp(z2);
  // exp(i*pi) = -1
  TEST_ASSERT_FLOAT(e.re, F::from_double(-1.0));
  TEST_ASSERT(std::abs(e.im.to_double()) < 1e-4);
}

void test_matrix_features() {
  std::cout << "Testing T81Matrix features...\n";
  using Mat2 = T81Matrix<float81, 2, 2>;

  Mat2 m;
  m(0, 0) = float81(4);
  m(0, 1) = float81(7);
  m(1, 0) = float81(2);
  m(1, 1) = float81(6);

  // Det = 4*6 - 7*2 = 24 - 14 = 10
  TEST_ASSERT_FLOAT(m.determinant(), float81(10));

  // Inverse
  // inv = 1/10 * {{6, -7}, {-2, 4}} = {{0.6, -0.7}, {-0.2, 0.4}}
  Mat2 inv = m.inverse();
  TEST_ASSERT_FLOAT(inv(0, 0), float81(0.6));
  TEST_ASSERT_FLOAT(inv(0, 1), float81(-0.7));
  TEST_ASSERT_FLOAT(inv(1, 0), float81(-0.2));
  TEST_ASSERT_FLOAT(inv(1, 1), float81(0.4));

  // Check M * Inv = I
  auto prod = m * inv;
  TEST_ASSERT_FLOAT(prod(0, 0), float81(1));
  TEST_ASSERT(std::abs(prod(0, 1).to_double()) < 1e-4);
  TEST_ASSERT(std::abs(prod(1, 0).to_double()) < 1e-4);
  TEST_ASSERT_FLOAT(prod(1, 1), float81(1));
}

void test_tensor_features() {
  std::cout << "Testing T81Tensor features...\n";
  // Triu/Tril
  T81Tensor<float81, 2, 3, 3> t;
  // 1 2 3
  // 4 5 6
  // 7 8 9
  int val = 1;
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j) t(i, j) = float81(val++);

  auto u = triu(t);
  // 1 2 3
  // 0 5 6
  // 0 0 9
  TEST_ASSERT_FLOAT(u(0, 0), float81(1));
  TEST_ASSERT_FLOAT(u(1, 0), float81(0));
  TEST_ASSERT_FLOAT(u(1, 1), float81(5));

  auto l = tril(t);
  // 1 0 0
  // 4 5 0
  // 7 8 9
  TEST_ASSERT_FLOAT(l(0, 0), float81(1));
  TEST_ASSERT_FLOAT(l(0, 1), float81(0));
  TEST_ASSERT_FLOAT(l(1, 0), float81(4));

  // Diag extract
  auto d = diag(t);
  // 1 5 9
  TEST_ASSERT_FLOAT(d(0), float81(1));
  TEST_ASSERT_FLOAT(d(1), float81(5));
  TEST_ASSERT_FLOAT(d(2), float81(9));

  // Diag create
  auto d2 = diag(d);
  // 1 0 0
  // 0 5 0
  // 0 0 9
  TEST_ASSERT_FLOAT(d2(0, 0), float81(1));
  TEST_ASSERT_FLOAT(d2(0, 1), float81(0));
  TEST_ASSERT_FLOAT(d2(1, 1), float81(5));

  // Clamp
  auto c = clamp(t, float81(4), float81(6));
  // 4 4 4
  // 4 5 6
  // 6 6 6
  TEST_ASSERT_FLOAT(c(0, 0), float81(4));
  TEST_ASSERT_FLOAT(c(1, 1), float81(5));
  TEST_ASSERT_FLOAT(c(2, 2), float81(6));
}

void test_list_features() {
  std::cout << "Testing T81List features...\n";
  T81List<int> l{1, 2, 3, 4, 5};

  // Map: square
  auto squares = l.map([](int x) { return x * x; });
  TEST_ASSERT(squares.size() == 5);
  TEST_ASSERT(squares[0] == 1);
  TEST_ASSERT(squares[1] == 4);
  TEST_ASSERT(squares[4] == 25);

  // Filter: even
  auto evens = l.filter([](int x) { return x % 2 == 0; });
  TEST_ASSERT(evens.size() == 2);
  TEST_ASSERT(evens[0] == 2);
  TEST_ASSERT(evens[1] == 4);

  // Reduce: sum
  int sum = l.reduce(0, [](int acc, int x) { return acc + x; });
  TEST_ASSERT(sum == 15);
}

void test_graph_pagerank() {
  std::cout << "Testing T81Graph PageRank...\n";
  // 3 nodes: 0->1, 1->2, 2->0 (cycle)
  T81Graph<3> g;
  g.add_edge(0, 1);
  g.add_edge(1, 2);
  g.add_edge(2, 0);

  auto pr = pagerank(g, 50);
  // Should be equal 0.333...
  TEST_ASSERT(std::abs(pr(0).to_double() - 0.333333) < 1e-2);
  TEST_ASSERT(std::abs(pr(1).to_double() - 0.333333) < 1e-2);
  TEST_ASSERT(std::abs(pr(2).to_double() - 0.333333) < 1e-2);

  // 2 nodes: 0->1, 1 is sink
  T81Graph<2> g2;
  g2.add_edge(0, 1);

  // With sink handling:
  // 0 distributes 0.85 to 1.
  // 1 distributes 0.85 * (1/2 to 0, 1/2 to 1) via sink logic.
  // Plus teleport 0.15 uniform.
  // It should converge to something valid, summing to 1.
  auto pr2 = pagerank(g2, 50);
  double sum = pr2(0).to_double() + pr2(1).to_double();
  TEST_ASSERT(std::abs(sum - 1.0) < 1e-2);
  // Node 1 should have more rank.
  TEST_ASSERT(pr2(1).to_double() > pr2(0).to_double());
}

int main() {
  test_complex_features();
  test_matrix_features();
  test_tensor_features();
  test_list_features();
  test_graph_pagerank();
  std::cout << "All core features tests PASSED!\n";
  return 0;
}
