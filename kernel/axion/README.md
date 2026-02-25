# `kernel/axion`

Implementation of Axion runtime/policy plumbing.

## Scope
- Axion engine API wiring
- Runtime verdict/policy integration points
- C/C++ bridge entry points used by CLI/runtime paths

## Key Files
- `engine.cpp`: Axion execution/policy core hooks.
- `axion_api.cpp`: API surface used by higher layers.

## Related Interfaces
- `include/t81/axion/api.hpp`
- `include/t81/axion/verdict.hpp`

## Notes
- Current scope includes stubs and incremental hardening paths.
- Preserve deterministic decision behavior for equal traces/policies.
