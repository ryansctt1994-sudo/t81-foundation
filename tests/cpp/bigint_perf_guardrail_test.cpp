#include "t81/bigint.hpp"

#include <cassert>
#include <chrono>
#include <iostream>

int main() {
  using namespace t81;
  using Clock = std::chrono::steady_clock;

  // Guardrail target: this exercises the single-limb fast paths heavily.
  T81BigInt a = T81BigInt::from_i64(123456789);
  T81BigInt b = T81BigInt::from_i64(-98765432);
  T81BigInt acc = T81BigInt::from_i64(0);

  constexpr int kIters = 200000;
  const auto t0 = Clock::now();
  for (int i = 0; i < kIters; ++i) {
    acc += a;
    acc -= b;
    acc *= T81BigInt::from_i64(1); // multiplication fast path should keep this cheap.
  }
  const auto t1 = Clock::now();
  const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

  // Deterministic correctness check.
  const auto expected = (a - b) * T81BigInt::from_i64(kIters);
  assert(acc == expected);

  // Coarse performance guardrail to catch major regressions.
  // This bound is intentionally loose for CI variability.
  constexpr long long kMaxMs = 8000;
  if (elapsed_ms > kMaxMs) {
    std::cerr << "bigint perf guardrail failed: elapsed_ms=" << elapsed_ms
              << " > " << kMaxMs << "\n";
    return 1;
  }

  std::cout << "bigint perf guardrail passed (" << elapsed_ms << " ms)\n";
  return 0;
}
