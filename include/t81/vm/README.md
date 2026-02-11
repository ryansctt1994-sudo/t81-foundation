# `include/t81/vm`

Public VM interfaces for HanoiVM.

## Key Headers
- `vm.hpp`: VM interface and constructors.
- `state.hpp`: machine state model.
- `traps.hpp`: trap/fault enums and helpers.

## Consumers
- Runtime implementation in `src/vm`
- CLI/run flows that embed VM execution
- Unit/integration tests validating opcode behavior

## Compatibility
- Treat these headers as stable API surfaces.
- Prefer additive changes; document breaking changes in `CHANGELOG.md`.
