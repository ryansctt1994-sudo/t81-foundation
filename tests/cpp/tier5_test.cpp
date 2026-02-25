#include <cassert>
#include <iostream>
#include <string>

#include "t81/experimental/cog/tier5/infinite.hpp"
#include "t81/fraction.hpp"

using t81::T81BigInt;
using t81::T81Fraction;
using t81::cog::v5::InfiniteCanonicalForm;
using t81::cog::v5::SeriesType;

void test_geometric_convergent() {
  InfiniteCanonicalForm form;
  form.type = SeriesType::Geometric;

  // a = 1
  form.first_term = T81Fraction::from_int(1);
  // r = 1/2
  form.ratio = T81Fraction(T81BigInt::from_int64(1), T81BigInt::from_int64(2));

  form.collapse();

  assert(form.is_convergent);
  // Sum = 1 / (1 - 1/2) = 2
  T81Fraction expected = T81Fraction::from_int(2);

  // Manual equality check until operator== is added to T81Fraction
  if (t81::T81Fraction::cmp(form.sum, expected) == 0) {
    std::cout << "Geometric Convergent: Passed (Sum = 2)\n";
  } else {
    // Basic double conversion for debug print
    // Assuming to_float<53, 11> is available on BigInt or manually construct
    // For now, just print the fraction string
    std::cerr << "Geometric Convergent: Failed (Expected 2, got " << form.sum.to_string() << ")\n";
    std::abort();
  }
}

void test_geometric_divergent() {
  InfiniteCanonicalForm form;
  form.type = SeriesType::Geometric;

  // a = 1
  form.first_term = T81Fraction::from_int(1);
  // r = 2
  form.ratio = T81Fraction::from_int(2);

  form.collapse();

  assert(!form.is_convergent);
  std::cout << "Geometric Divergent: Passed\n";
}

void test_geometric_divergent_neg() {
  InfiniteCanonicalForm form;
  form.type = SeriesType::Geometric;

  // a = 1
  form.first_term = T81Fraction::from_int(1);
  // r = -2
  form.ratio = T81Fraction::from_int(-2);

  form.collapse();

  assert(!form.is_convergent);
  std::cout << "Geometric Divergent (Negative ratio): Passed\n";
}

void test_unknown_type() {
  InfiniteCanonicalForm form;
  form.type = SeriesType::Unknown;

  form.collapse();

  assert(form.is_convergent);
  assert(form.convergence_signature == "COLLAPSED_INFINITY");
  std::cout << "Unknown Type: Passed (Stub behavior)\n";
}

int main() {
  std::cout << "Running Tier 5 Infinite Series Tests...\n";

  test_geometric_convergent();
  test_geometric_divergent();
  test_geometric_divergent_neg();
  test_unknown_type();

  std::cout << "All Tier 5 tests passed!\n";
  return 0;
}
