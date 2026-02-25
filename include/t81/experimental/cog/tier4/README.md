# `include/t81/experimental/cog/tier4`

Tier-4 cognitive loop and planning interfaces.

## Key Headers
- `planner.hpp`: tier-selection heuristic via `TaskMetadata`.
- `self_model.hpp`: self-model state representation.
- `tier4_loop.hpp`: observe/reflect/refine loop and VM snapshot ingestion.

## Primary Concepts
- Tier promotion based on complexity/resource/reflection signals.
- Reflection trace capture for auditing and refinement.
- Integration hooks with Axion engine and VM reflection snapshots.

## Notes
- Keep policy-facing behavior auditable and deterministic.
- Changes to tier selection heuristics should include regression tests.
