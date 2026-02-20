#include <cassert>
#include <iostream>
#include <string>

#include "t81/cog/tier5/infinite.hpp"
#include "t81/core/T81Fraction.hpp"

using t81::cog::v5::InfiniteCanonicalForm;
using t81::cog::v5::SeriesType;
using t81::v1::T81Frac81;

using Int = T81Frac81::Int;

void test_geometric_convergent() {
  InfiniteCanonicalForm form;
  form.type = SeriesType::Geometric;

  // a = 1
  form.first_term = T81Frac81(1);
  // r = 1/2
  form.ratio = T81Frac81(Int(1), Int(2));

  form.collapse();

  assert(form.is_convergent);
  // Sum = 1 / (1 - 1/2) = 2
  T81Frac81 expected(2);

  if (form.sum == expected) {
    std::cout << "Geometric Convergent: Passed (Sum = 2)\n";
  } else {
    std::cerr << "Geometric Convergent: Failed (Expected 2, got " << form.sum.to_double() << ")\n";
    std::abort();
  }
}

void test_geometric_divergent() {
  InfiniteCanonicalForm form;
  form.type = SeriesType::Geometric;

  // a = 1
  form.first_term = T81Frac81(1);
  // r = 2
  form.ratio = T81Frac81(2);

  form.collapse();

  assert(!form.is_convergent);
  std::cout << "Geometric Divergent: Passed\n";
}

void test_geometric_divergent_neg() {
  InfiniteCanonicalForm form;
  form.type = SeriesType::Geometric;

  // a = 1
  form.first_term = T81Frac81(1);
  // r = -2
  form.ratio = T81Frac81(-2);

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
