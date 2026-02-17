#include <cstdlib>
#include <iostream>
#include <random>
#include <vector>
#include "t81/core/T81BigInt.hpp"

using namespace t81::v1;

// Helper to generate random BigInt
T81BigInt random_bigint(std::mt19937_64& rng, int max_limbs) {
  std::uniform_int_distribution<int> dist_len(1, max_limbs);
  int len = dist_len(rng);

  T81BigInt res(0);
  std::uniform_int_distribution<int64_t> dist_val;

  // Base 2^60
  T81BigInt shift(1LL << 60);

  for (int i = 0; i < len; ++i) {
    res = res * shift + T81BigInt(dist_val(rng) & 0x0FFFFFFFFFFFFFFF);
  }

  // Random sign
  if (dist_val(rng) % 2) res = -res;
  return res;
}

int main() {
  std::mt19937_64 rng(12345);
  int iterations = 1000;

  for (int i = 0; i < iterations; ++i) {
    T81BigInt a = random_bigint(rng, 4);  // Up to ~240 bits
    T81BigInt b = random_bigint(rng, 3);

    if (b.is_zero()) continue;

    auto [q, r] = T81BigInt::div_mod(a, b);

    // Property 1: a = q*b + r
    T81BigInt check = q * b + r;
    if (check != a) {
      std::cerr << "Fuzz failure: a != q*b + r\n";
      std::cerr << "a=" << a.to_string() << "\n";
      std::cerr << "b=" << b.to_string() << "\n";
      std::cerr << "q=" << q.to_string() << "\n";
      std::cerr << "r=" << r.to_string() << "\n";
      std::cerr << "check=" << check.to_string() << "\n";
      return 1;
    }

    // Property 2: 0 <= r < |b|
    if (r.is_negative()) {
      std::cerr << "Fuzz failure: r is negative\n";
      std::cerr << "r=" << r.to_string() << "\n";
      return 1;
    }
    if (r >= b.abs()) {
      std::cerr << "Fuzz failure: r >= |b|\n";
      std::cerr << "r=" << r.to_string() << "\n";
      std::cerr << "b=" << b.to_string() << "\n";
      return 1;
    }
  }

  std::cout << "fuzz_bigint_div passed (" << iterations << " iterations)\n";
  return 0;
}
