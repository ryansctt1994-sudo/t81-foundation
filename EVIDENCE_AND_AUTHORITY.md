# Evidence and Authority Boundary

Status date: 2026-09-04

T81 distinguishes normative specification precedence from empirical evidence and operational authority.

## Non-collapse rules

```text
normative_spec_precedence != operational_authority
frozen_version != verified_truth
local_repro_gate != independent_reproduction
policy_enforcement != safety_guarantee
deterministic_surface != whole-system_determinism
mathematical_definition != empirical_validation
security_feature != cryptographic_assurance
```

## Documentation precedence

A normative specification can be the source of truth for what a T81 implementation is *supposed* to do. Calling a spec normative or frozen means project changes are governed by version/change-control rules. It does not mean the specification is infallible, externally certified, empirically complete, safe for high-stakes use, or operationally authoritative.

## Determinism claims

A bit-exact or deterministic claim must identify the exact operation/surface, input domain, implementation, commit, toolchain/platform set, and reproduction evidence. Determinism demonstrated on a registry/core profile does not transfer automatically to JIT paths, GPU/accelerator paths, model execution, external libraries, I/O, concurrency, or untested platforms.

## Governance and safety claims

The Axion/policy layer may enforce configured rules in the implementation surfaces covered by its tests. That is evidence about enforcement behavior, not proof that the policy itself is ethically correct, complete, secure, or sufficient for a high-stakes domain.

## Promotion

Use `specified`, `version-frozen`, `implemented`, `locally reproduced`, and `independently reproduced` as distinct states. Reserve `verified`, `validated`, `safe`, `production-ready`, `high-assurance`, and similar terms for claims with evidence appropriate to those meanings.

No repository component gains operational authority merely because documentation labels it normative, frozen, high priority, or part of the foundation layer.
