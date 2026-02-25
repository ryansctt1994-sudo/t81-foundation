# `experimental/tiers/cog/tier3` (T2187)

Directory documentation for `experimental/tiers/cog/tier3`.

## Purpose
This module implements Tier 3 of the Cognitive Framework, corresponding to T2187 (Hyper-Recursive Cognition).

## Key Components
- `Recursor`: Manages deep recursion stacks with strict bounds.
- `ContractionProof`: Ensures each recursive step reduces system entropy.

## Responsibilities
- Guarantee termination of cognitive loops through entropy contraction.
- Monitor recursion depth and enforce safety limits.

## Expectations
- All recursive calls must be accompanied by a valid contraction proof.
- Entropy must decrease monotonically within a recursive context.

## See Also
- `include/t81/experimental/cog/tier3/recursive.hpp`
