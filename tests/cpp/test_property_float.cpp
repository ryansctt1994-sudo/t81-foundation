#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include "t81/core/T81Float.hpp"

using namespace t81::v1;

// Use a standard size
using Float = T81Float<18, 9>;

Float random_float(std::mt19937_64& rng) {
  std::uniform_real_distribution<double> dist(-1e10, 1e10);
  double val = dist(rng);
  // occasionally return special values
  std::uniform_int_distribution<int> special(0, 100);
  if (special(rng) == 0) return Float::zero();
  if (special(rng) == 1) return Float::inf();
  // We avoid generating NaE randomly for arithmetic tests as they propagate
  return Float::from_double(val);
}

int main() {
  std::mt19937_64 rng(67890);
  int iterations = 10000;

  std::cout << "Running float property tests for " << iterations << " iterations...\n";

  for (int i = 0; i < iterations; ++i) {
    Float a = random_float(rng);
    Float b = random_float(rng);

    if (a.is_nae() || b.is_nae()) continue;

    // 1. Commutativity of Addition
    // We expect exact bitwise equality for commutative operations
    Float sum1 = a + b;
    Float sum2 = b + a;
    if (sum1 != sum2) {
       // NaE != NaE, but if inputs are not NaE, sum shouldn't be NaE unless overflow/inf interactions
       if (!sum1.is_nae() && !sum2.is_nae()) {
          std::cerr << "FAILED: Float Commutativity of Addition\n";
          return 1;
       }
    }

    // 2. Commutativity of Multiplication
    Float prod1 = a * b;
    Float prod2 = b * a;
    if (prod1 != prod2) {
       if (!prod1.is_nae() && !prod2.is_nae()) {
          std::cerr << "FAILED: Float Commutativity of Multiplication\n";
          return 1;
       }
    }

    // 3. Double Negation
    if (!a.is_nae()) {
        Float negneg = -(-a);
        if (negneg != a) {
            std::cerr << "FAILED: Float Double Negation\n";
            return 1;
        }
    }

    // 4. Additive Identity
    if (!a.is_nae()) {
        if (a + Float::zero() != a) {
             std::cerr << "FAILED: Float Additive Identity\n";
             return 1;
        }
    }

    // 5. Multiplicative Identity
    if (!a.is_nae()) {
        Float one = Float::from_double(1.0);
        Float prod = a * one;
        if (prod != a) {
             std::cerr << "FAILED: Float Multiplicative Identity\n"
                       << "a = " << a.to_double() << "\n"
                       << "a * 1 = " << prod.to_double() << "\n"
                       << "a.exp=" << a.exponent() << " a.sig=" << a.mantissa().significant_trits()
                       << " a.mant=" << a.mantissa().to_string() << "\n"
                       << "prod.exp=" << prod.exponent() << " prod.sig=" << prod.mantissa().significant_trits()
                       << " prod.mant=" << prod.mantissa().to_string() << "\n";
             return 1;
        }
    }

    // 6. Multiplicative Zero
    if (!a.is_nae() && !a.is_inf()) {
        if ((a * Float::zero()).is_zero() == false) {
             std::cerr << "FAILED: Float Multiplicative Zero\n";
             return 1;
        }
    }
  }

  std::cout << "All float property tests passed!\n";
  return 0;
}
