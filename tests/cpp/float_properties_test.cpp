#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>

#include "t81/types/T81Float.hpp"

using namespace t81;

// Using T81Float27_9 for testing
using F = T81Float<27, 9>;

void check(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << "\n";
    std::exit(1);
  }
}

bool is_close(double a, double b) {
  if (std::abs(a) < 1e-12 && std::abs(b) < 1e-12) return true;
  double rel = std::abs(a - b) / std::max(std::abs(a), std::abs(b));
  return rel < 1e-12;
}

int main() {
  std::cout << "Running float_properties_test...\n";

  std::mt19937_64 rng(0xF81AC710ULL);  // Deterministic seed
  std::uniform_real_distribution<double> dist(-1000.0, 1000.0);
  std::uniform_real_distribution<double> dist_small(-1.0, 1.0);

  const F zero = F::zero();
  const F one = F::from_double(1.0);
  const F inf = F::inf();
  const F nae = F::nae();

  // 1. Special values checks
  check(zero.is_zero(), "Zero is zero");
  check(!zero.is_inf(), "Zero is not inf");
  check(!zero.is_nae(), "Zero is not nae");

  check(inf.is_inf(), "Inf is inf");
  check(!inf.is_zero(), "Inf is not zero");
  check(!inf.is_nae(), "Inf is not nae");

  check(nae.is_nae(), "NaE is nae");
  check(!nae.is_zero(), "NaE is not zero");
  check(!nae.is_inf(), "NaE is not inf");  // Check specific impl logic

  // 2. Identity properties with random values
  for (int i = 0; i < 1000; ++i) {
    double d = dist(rng);
    F f = F::from_double(d);

    // Additive identity: x + 0 = x
    F f_plus_zero = f + zero;
    // Check approximate equality due to normalization differences
    check(is_close(f_plus_zero.to_double(), f.to_double()), "Additive identity (value)");

    // Multiplicative identity: x * 1 = x
    F f_times_one = f * one;
    check(is_close(f_times_one.to_double(), f.to_double()), "Multiplicative identity (value)");

    // Negation: -(-x) = x
    F neg_neg_f = -(-f);
    check(is_close(neg_neg_f.to_double(), f.to_double()), "Negation involution");

    // Additive inverse: x + (-x) = 0
    F f_plus_neg_f = f + (-f);
    check(f_plus_neg_f.is_zero(), "Additive inverse");
  }

  // 3. Round-trip consistency
  for (int i = 0; i < 1000; ++i) {
    double d = dist(rng);
    F f = F::from_double(d);
    double d2 = f.to_double();
    F f2 = F::from_double(d2);

    check(f.to_double() == f2.to_double(), "Round-trip stability T81Float -> double -> T81Float");
  }

  // 4. Commutativity of addition and multiplication
  for (int i = 0; i < 1000; ++i) {
    double d1 = dist(rng);
    double d2 = dist(rng);
    F a = F::from_double(d1);
    F b = F::from_double(d2);

    F ab = a + b;
    F ba = b + a;
    check(ab.to_double() == ba.to_double(), "Addition commutativity");

    F ab_mul = a * b;
    F ba_mul = b * a;
    check(ab_mul.to_double() == ba_mul.to_double(), "Multiplication commutativity");
  }

  std::cout << "float_properties_test ok\n";
  return 0;
}
