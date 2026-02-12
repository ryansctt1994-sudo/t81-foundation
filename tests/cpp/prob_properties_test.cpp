#include <cstdint>
#include <iostream>
#include <random>

#include "t81/core/T81Prob.hpp"
#include "t81/core/T81Int.hpp"

int main() {
  using t81::T81Prob27;
  using t81::T81Int;

  auto expect = [](bool cond, const char* msg) -> bool {
    if (!cond) {
      std::cerr << "prob_properties_test failure: " << msg << "\n";
      return false;
    }
    return true;
  };

  // Fixed seed for deterministic replay.
  std::mt19937_64 rng(0x815A5A5AULL);
  // Range within safe 27-trit capacity (approx 42 bits)
  // We use a smaller range to avoid overflow during addition in the test loop
  std::uniform_int_distribution<std::int64_t> dist(-1'000'000, 1'000'000);

  // Special values
  const T81Prob27 zero = T81Prob27::zero();
  const T81Prob27 min_inf = T81Prob27::minus_infinity();
  const T81Prob27 max_inf = T81Prob27::plus_infinity();

  for (int i = 0; i < 2500; ++i) {
    const std::int64_t ai = dist(rng);
    const std::int64_t bi = dist(rng);
    const std::int64_t ci = dist(rng);

    // Construct raw log-odds directly to test arithmetic properties
    const T81Prob27 a{T81Int<27>(ai)};
    const T81Prob27 b{T81Int<27>(bi)};
    const T81Prob27 c{T81Int<27>(ci)};

    // Commutativity: a + b == b + a
    if (!expect((a + b) == (b + a), "commutativity failed")) return 1;

    // Associativity: (a + b) + c == a + (b + c)
    if (!expect(((a + b) + c) == (a + (b + c)), "associativity failed")) return 1;

    // Add/sub inverse law: (a + b) - b == a
    if (!expect(((a + b) - b) == a, "add/sub inverse law failed")) return 1;

    // Negation involution: -(-a) == a
    if (!expect(-(-a) == a, "double negation failed")) return 1;

    // Identity: a + zero() == a (zero is log-odds 0)
    // T81Prob::zero() acts as additive identity in log-odds space (representing p=0.5)
    if (!expect((a + zero) == a, "additive identity failed")) return 1;

    // Monotonicity: a > b => a + c > b + c
    if (a > b) {
        if (!expect((a + c) > (b + c), "monotonicity failed")) return 1;
    }
  }

  // Test special values
  if (!expect((min_inf + zero) == min_inf, "min_inf identity failed")) return 1;
  // Note: max_inf + zero might overflow if implemented naively, but T81Int handles saturation or overflow.
  // In T81Prob, operator+ delegates to T81Int operator+.
  // T81Int::kMaxValue + 0 is kMaxValue.
  if (!expect((max_inf + zero) == max_inf, "max_inf identity failed")) return 1;

  // max_inf > any finite
  T81Prob27 finite(T81Int<27>(1000000));
  if (!expect(max_inf > finite, "max_inf comparison failed")) return 1;
  if (!expect(min_inf < finite, "min_inf comparison failed")) return 1;

  std::cout << "prob_properties_test ok\n";
  return 0;
}
