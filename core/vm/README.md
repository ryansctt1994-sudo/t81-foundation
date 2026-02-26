# `core/vm`

Implementation of the T81VM runtime.

## Scope
- Instruction execution loop
- VM state transitions and trap handling
- Runtime loading/dispatch plumbing

## Key Files
- `vm.cpp`: interpreter dispatch integration and opcode-family orchestration.
- `tensor_helpers.cpp`: tensor decode/load/compute and checked trap helpers.
- `policy_trace_bridge.cpp`: Axion syscall context/reason/event bridge helpers.
- `runtime_state_helpers.cpp`: deterministic system-register/signature synchronization.
- `gc_helpers.cpp`: GC mark/sweep and heap compaction helpers.

## Related Interfaces
- `include/t81/vm/vm.hpp`
- `include/t81/vm/state.hpp`
- `include/t81/vm/traps.hpp`

## Notes
- VM behavior must remain deterministic for identical program + inputs.
- **Exception:** Floating-point division and transcendental operations (`FSIN`, `FDIV`, etc.) rely on host `double` precision and may not be bit-exact across platforms.
- Any opcode semantic change should be mirrored in tests under `tests/cpp`.
