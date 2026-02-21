#include <sstream>
#include <string>

#include "t81/axion/ethics.hpp"
#include "t81/axion/reasons.hpp"
#include "t81/config.hpp"
#include "t81/tisc/opcodes.hpp"

namespace t81::axion {

Verdict check_ethics(EthicsPrinciple p, const SyscallContext& ctx) {
  using Op = t81::tisc::Opcode;

  switch (p) {
    case EthicsPrinciple::NonHarm: {
      // Θ₁: Non-Harm.
      // Ensure no operation fundamentally threatens the integrity of the host system.
      // The VM sandbox provides primary isolation, but ethics layer reinforces it.
      // For example, deny explicit Halt if not authorized?
      // Currently assuming VM traps handle direct harm.
      break;
    }
    case EthicsPrinciple::NonCoercion: {
      // Θ₂: Non-Coercion.
      // Users/Agents should not be forced into states without policy consent.
      // If policy is missing, we are in "wild" mode, which might be coercive.
      // In strict mode, we would deny execution without a policy.
      break;
    }
    case EthicsPrinciple::Truth: {
      // Θ₃: Truth & Verifiability.
      // Operations claiming to verify facts (AxVerify) must be respected.
      // If AxVerify fails, it's a truth violation, but handled at runtime.
      break;
    }
    case EthicsPrinciple::Interpretability: {
      // Θ₄: Interpretability (and Contraction & Convergence).
      // Deep recursion obscures interpretability.
      // We flag high recursion depth as a potential risk.
      // @spec-ref: spec/axion-kernel.md#3-recursion-controls
      if (ctx.recursion_depth > 243) {
        // Warning: High recursion depth impacts interpretability.
        return {VerdictKind::Warn, "Theta-4: High recursion depth (interpretability risk)"};
      }
      break;
    }
    case EthicsPrinciple::IdentityIntegrity: {
      // Θ₅: Identity Integrity.
      // Protects canonical identity tensors from unauthorized mutation.
      // TSet on a read-only tensor? VM handles this via handle tags, but ethics could be stricter.
      break;
    }
    case EthicsPrinciple::EthicalPriority: {
      // Θ₆: Ethical Priority (Ethics > Performance).
      // We do not skip checks for performance reasons.
      // This principle justifies the overhead of these checks.
      break;
    }
    case EthicsPrinciple::EntropyContainment: {
      // Θ₇: Entropy Containment.
      // Hard enforcement of recursion and resource limits to prevent unbounded growth.
      // VM has kHardRecursionCeiling (729), but we reinforce it here as an ethical invariant.
      // @spec-ref: spec/axion-kernel.md#3.4-catastrophic-recursion-detection
      if (ctx.recursion_depth >= T81_HARD_RECURSION_CEILING) {
        return {VerdictKind::Deny,
                std::string(reasons::kRecursionCeiling) + " (Theta-7 Entropy Containment)"};
      }
      // Also check instruction count if it suggests an infinite loop (though VM halts eventually).
      break;
    }
    case EthicsPrinciple::CanonicalConsistency: {
      // Θ₈: Canonical Consistency.
      // Operations must preserve canonical forms.
      // e.g. T3_K encoding validity. Handled by VM import logic.
      break;
    }
    case EthicsPrinciple::TransparentExecution: {
      // Θ₉: Transparent Execution (Aletheia Sovereignty).
      // All execution must be auditable.
      // If Axion logging is disabled (not possible in current VM), deny.
      // We ensure that Meta* ops are logged (which they are).
      // @spec-ref: spec/axion-kernel.md#1.9-axion-api--policy-enforcement
      if (ctx.next_opcode == Op::MetaWrite && !ctx.policy) {
        // Writing to meta-memory (code/registers) without a policy is highly risky/opaque.
        return {VerdictKind::Warn, "Theta-9: MetaWrite without policy (transparency risk)"};
      }
      break;
    }
  }

  return {VerdictKind::Allow, "Ethics check passed"};
}

}  // namespace t81::axion
