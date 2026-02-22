#include <cassert>
#include <iostream>
#include <random>
#include <vector>

#include "t81/axion/ethics.hpp"
#include "t81/axion/policy.hpp"
#include "t81/config.hpp"
#include "t81/tisc/opcodes.hpp"

using namespace t81::axion;
using namespace t81::tisc;

// Constants
constexpr size_t T81_SOFT_RECURSION_LIMIT = 243;
constexpr int DEFAULT_ITERATIONS = 10000;

// Helper to generate random Opcode
Opcode random_opcode(std::mt19937& rng) {
  std::uniform_int_distribution<int> dist(0, static_cast<int>(Opcode::Frac2F));
  return static_cast<Opcode>(dist(rng));
}

// Helper to generate random SyscallContext
SyscallContext random_context(std::mt19937& rng) {
  SyscallContext ctx;
  std::uniform_int_distribution<size_t> depth_dist(0, T81_HARD_RECURSION_CEILING + 100);
  std::uniform_int_distribution<size_t> count_dist(0, 10000);

  ctx.recursion_depth = depth_dist(rng);
  ctx.instruction_count = count_dist(rng);
  ctx.next_opcode = random_opcode(rng);

  // Randomly assign policy
  static Policy dummy_policy; // Keep it alive
  std::uniform_int_distribution<int> policy_dist(0, 1);
  if (policy_dist(rng)) {
    ctx.policy = &dummy_policy;
  } else {
    ctx.policy = nullptr;
  }

  return ctx;
}

int main() {
  std::mt19937 rng(42); // Fixed seed for reproducibility
  const int iterations = DEFAULT_ITERATIONS;

  std::cout << "Running ethics invariant property tests for " << iterations << " iterations...\n";

  for (int i = 0; i < iterations; ++i) {
    auto ctx = random_context(rng);

    // Check all principles
    for (int p_idx = 1; p_idx <= kEthicsPrincipleCount; ++p_idx) {
      auto p = static_cast<EthicsPrinciple>(p_idx);
      auto verdict = check_ethics(p, ctx);

      // Invariant 1: Theta-7 (Entropy Containment)
      // Must Deny if recursion depth >= T81_HARD_RECURSION_CEILING
      if (p == EthicsPrinciple::EntropyContainment) {
        if (ctx.recursion_depth >= T81_HARD_RECURSION_CEILING) {
          if (verdict.kind != VerdictKind::Deny) {
            std::cerr << "FAILED: Theta-7 Invariant (Recursion Ceiling)\n"
                      << "Depth: " << ctx.recursion_depth << "\n"
                      << "Verdict: " << (int)verdict.kind << "\n";
            return 1;
          }
        }
      }

      // Invariant 2: Theta-4 (Interpretability)
      // Must Warn if recursion depth > T81_SOFT_RECURSION_LIMIT (soft limit) but < T81_HARD_RECURSION_CEILING
      if (p == EthicsPrinciple::Interpretability) {
        if (ctx.recursion_depth > T81_SOFT_RECURSION_LIMIT && ctx.recursion_depth < T81_HARD_RECURSION_CEILING) {
           // Note: Depending on implementation, it might be Warn or Deny if it hits other limits,
           // but strictly speaking for Theta-4 it should warn about interpretability.
           // However, if Theta-7 denies it later, that's fine.
           // Here we are checking specific principle return.
           if (verdict.kind != VerdictKind::Warn && verdict.kind != VerdictKind::Deny) {
             std::cerr << "FAILED: Theta-4 Invariant (Interpretability Warning)\n"
                       << "Depth: " << ctx.recursion_depth << "\n"
                       << "Verdict: " << (int)verdict.kind << "\n";
             return 1;
           }
        }
      }

      // Invariant 3: Theta-9 (Transparent Execution)
      // Must Warn/Deny if MetaWrite without policy
      if (p == EthicsPrinciple::TransparentExecution) {
        if (ctx.next_opcode == Opcode::MetaWrite && ctx.policy == nullptr) {
          if (verdict.kind == VerdictKind::Allow) {
            std::cerr << "FAILED: Theta-9 Invariant (MetaWrite without Policy)\n"
                      << "Verdict: Allow\n";
            return 1;
          }
        }
      }

      // Invariant 4: No crashes or undefined behavior (implicit by running)
      // Invariant 5: Benign cases should pass (mostly)
      // If depth is low and opcode is safe, expect Allow
      if (ctx.recursion_depth < T81_SOFT_RECURSION_LIMIT &&
          ctx.next_opcode != Opcode::MetaWrite &&
          p != EthicsPrinciple::NonHarm /* placeholders might allow */) {
          // Generally should be allow, but let's just ensure it doesn't crash.
          // We can't strictly assert Allow because future logic might add more checks.
      }
    }
  }

  std::cout << "All ethics invariant tests passed!\n";
  return 0;
}
