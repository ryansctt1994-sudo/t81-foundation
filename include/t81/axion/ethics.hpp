#pragma once

#include <string_view>
#include "t81/axion/context.hpp"
#include "t81/axion/verdict.hpp"

namespace t81::axion {

/**
 * @enum EthicsPrinciple
 * @brief Represents one of the Nine Immutable Principles (Θ₁–Θ₉).
 */
enum class EthicsPrinciple {
  NonHarm = 1,             ///< Θ₁ — Foundational Ethics Layer (Non-Harm)
  NonCoercion = 2,         ///< Θ₂ — Transparency & Interpretability Layer (Non-Coercion)
  Truth = 3,               ///< Θ₃ — Canonical Identity Layer (Truth & Verifiability)
  Interpretability = 4,    ///< Θ₄ — Contraction & Convergence Layer (Interpretability)
  IdentityIntegrity = 5,   ///< Θ₅ — Distributed Coherence Layer (Identity Integrity)
  EthicalPriority = 6,     ///< Θ₆ — Symbolic/Reflective Integrity Layer (Ethics > Performance)
  EntropyContainment = 7,  ///< Θ₇ — Infinite-Tier Protection Layer (Entropy Containment)
  CanonicalConsistency =
      8,  ///< Θ₈ — Formalization & Deterministic Semantics Layer (Canonical Consistency)
  TransparentExecution = 9  ///< Θ₉ — Aletheia Sovereignty Layer (Transparent Execution)
};

constexpr int kEthicsPrincipleCount = 9;

/**
 * @brief Returns the name of the given ethics principle.
 */
constexpr std::string_view to_string(EthicsPrinciple p) {
  switch (p) {
    case EthicsPrinciple::NonHarm:
      return "Theta-1 (Non-Harm)";
    case EthicsPrinciple::NonCoercion:
      return "Theta-2 (Non-Coercion)";
    case EthicsPrinciple::Truth:
      return "Theta-3 (Truth)";
    case EthicsPrinciple::Interpretability:
      return "Theta-4 (Interpretability)";
    case EthicsPrinciple::IdentityIntegrity:
      return "Theta-5 (Identity Integrity)";
    case EthicsPrinciple::EthicalPriority:
      return "Theta-6 (Ethical Priority)";
    case EthicsPrinciple::EntropyContainment:
      return "Theta-7 (Entropy Containment)";
    case EthicsPrinciple::CanonicalConsistency:
      return "Theta-8 (Canonical Consistency)";
    case EthicsPrinciple::TransparentExecution:
      return "Theta-9 (Transparent Execution)";
  }
  return "Unknown Principle";
}

/**
 * @brief Evaluates a syscall against a specific ethics principle.
 *
 * @param p The ethics principle to check.
 * @param ctx The syscall context.
 * @return A verdict indicating whether the operation is allowed by the principle.
 */
Verdict check_ethics(EthicsPrinciple p, const SyscallContext& ctx);

}  // namespace t81::axion
