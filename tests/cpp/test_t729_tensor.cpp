#include <cmath>
#include <cstdlib>
#include <iostream>
#include "t81/core/T729Tensor.hpp"

using namespace t81;
using namespace t81::holotensor;

void check(bool cond, const char* msg) {
  if (!cond) {
    std::cerr << "Check failed: " << msg << "\n";
    std::exit(1);
  }
}

// Helper to check float equality
bool approx_eq(double a, double b, double eps = 1e-6) { return std::abs(a - b) < eps; }

void test_aliases() {
  // Verify HoloVector is 1D with 729 elements
  static_assert(HoloVector::rank() == 1, "HoloVector rank check");
  static_assert(HoloVector::size() == 729, "HoloVector size check");

  // Verify HoloMatrix is 2D with 729x729 elements
  static_assert(HoloMatrix::rank() == 2, "HoloMatrix rank check");
  static_assert(HoloMatrix::size() == 729 * 729, "HoloMatrix size check");

  std::cout << "test_aliases passed\n";
}

void test_zeros_fill() {
  auto z = zeros<1, 10>();
  for (size_t i = 0; i < 10; ++i) {
    check(z(i).is_zero(), "zeros check");
  }

  T81Float<72, 9> val(3.14);
  auto f = fill<1, 5>(val);
  for (size_t i = 0; i < 5; ++i) {
    check(f(i) == val, "fill check");
  }

  std::cout << "test_zeros_fill passed\n";
}

void test_contract() {
  // Create two small vectors
  T729Tensor<1, 3> a;
  a(0) = T81Float<72, 9>(1.0);
  a(1) = T81Float<72, 9>(2.0);
  a(2) = T81Float<72, 9>(3.0);

  T729Tensor<1, 3> b;
  b(0) = T81Float<72, 9>(4.0);
  b(1) = T81Float<72, 9>(5.0);
  b(2) = T81Float<72, 9>(6.0);

  // Dot product: 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
  auto res = contract(a, b);

  check(approx_eq(res.to_double(), 32.0), "contract check");

  std::cout << "test_contract passed\n";
}

void test_inherited_ops() {
  // Test basic arithmetic inherited from T81Tensor
  T729Tensor<1, 2> t1;
  t1(0) = T81Float<72, 9>(10.0);
  t1(1) = T81Float<72, 9>(20.0);

  T729Tensor<1, 2> t2;
  t2(0) = T81Float<72, 9>(2.0);
  t2(1) = T81Float<72, 9>(4.0);

  auto sum = t1 + t2;
  check(approx_eq(sum(0).to_double(), 12.0), "add 0");
  check(approx_eq(sum(1).to_double(), 24.0), "add 1");

  auto prod = t1 * t2;
  check(approx_eq(prod(0).to_double(), 20.0), "mul 0");
  check(approx_eq(prod(1).to_double(), 80.0), "mul 1");

  std::cout << "test_inherited_ops passed\n";
}

int main() {
  test_aliases();
  test_zeros_fill();
  test_contract();
  test_inherited_ops();
  return 0;
}
