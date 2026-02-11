# `src/vm`

Implementation of the HanoiVM runtime.

## Scope
- Instruction execution loop
- VM state transitions and trap handling
- Runtime loading/dispatch plumbing

## Key Files
- `vm.cpp`: core interpreter loop and opcode dispatch.

## Related Interfaces
- `include/t81/vm/vm.hpp`
- `include/t81/vm/state.hpp`
- `include/t81/vm/traps.hpp`

## Notes
- VM behavior must remain deterministic for identical program + inputs.
- Any opcode semantic change should be mirrored in tests under `tests/cpp`.
