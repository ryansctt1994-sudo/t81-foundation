# Cognitive Tiers Spec Compliance (2026-02-26)

Status: Partial Compliance (Major Gaps Closed)
Date (UTC): 2026-02-26
Owner: Project Management / Runtime

## Scope

Comparison baseline:

- `spec/cognitive-tiers.md`
- Tier runtime/frontend implementation in VM and language pipeline

## Closed in This Pass

1. Explicit Tier faulting path
   - Added explicit VM trap `TierFault`.
   - Tier-gated failures now trap with `TierFault` instead of generic `SecurityFault`.
2. Tier recursion ceilings aligned to spec
   - Enforced recursion ceilings by active tier:
     - Tier1: 1
     - Tier2: 10
     - Tier3: 81
     - Tier4: 243
     - Tier5: 729
   - `Call` path now promotes deterministically as needed or faults if policy denies/limit exceeded.
3. Tier-opcode gating
   - Enforced minimum required tier for tiered opcodes:
     - Tier2 reflective: `Refl*`
     - Tier3 recursive: `Recurse/Contract/Entropy/Depth/Terminate`
     - Tier4 distributed: `Gossip/Merge/TickSync/Coherence`
     - Tier5 infinite: `Inf*`
4. Static `@tier(n)` behavior verification
   - Parser/AST now carry function tier attribute.
   - Semantic analyzer validates function bodies against declared tier and fails on mismatch.
5. Tier fault metadata hardening
   - Tier-fault reasons now include tier/call-depth/recurse-depth/pc/value and recent trace context.

## Validation

- `t81lang_conformance_baseline_test`
- `t81lang_conformance_edge_semantics_test`
- `test_tier3_opcodes`
- `t81_vm_tier_promotion_test`
- `t81_tier4_distributed_test`
- `t81_infinite_opcodes_test`
- `t81_vm_new_opcodes_test`

All above passed on the 2026-02-26 run after enforcement updates.

## Residual Gaps (Still Open)

1. Full metric-governed enforcement is incomplete
   - Spec metrics call out branching entropy, symbolic complexity, and shape complexity as normative.
   - Current enforcement is strongest on recursion/tiered opcode surfaces; metric gates are not yet complete.
2. Tier demotion policy is not fully implemented
   - Spec defines conservative tier-down transitions.
   - Current runtime heavily emphasizes deterministic promotion and faulting.
3. Tier declaration verification breadth
   - `@tier` checks now catch key tiered constructs, but not every possible complexity expression path.

## Next Closure Candidates

1. Add explicit runtime metric accounting + hard policy gates for:
   - branching entropy
   - symbolic complexity
   - shape complexity (`product(shape) * rank`)
2. Implement conservative demotion state machine tied to verified convergence conditions.
3. Expand `@tier` static checks to include tensor-rank and effect-surface restrictions by tier.
