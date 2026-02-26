# VM Layer (T81VM Interpreter)

Status: Active  
Last Verified (UTC): 2026-02-26  
Maturity: `Stable` interpreter, `Experimental` trace/JIT path

> **Architecture File Style Guide**
> - **Terminology mapping:** "Interpreter" -> `core/vm/vm.cpp` (`Interpreter` class); "VM state" -> `include/t81/vm/state.hpp`; "Trap" -> `include/t81/vm/traps.hpp`; "Axion check" -> `eval_axion_call(...)` in `core/vm/vm.cpp`.
> - **Code/spec links:** use direct file links; avoid inferred behavior not present in code/spec.
> - **Diagram conventions:** Mermaid only; decision diamonds are policy/trap points.
> - **Maturity labels:** `Frozen`, `Stable`, `Experimental`, `Stubbed`.

## Purpose and Responsibilities

Execute TISC instructions deterministically through the reference interpreter path, maintain machine state, and enforce policy checks before privileged or sensitive effects.

Primary interface: [`include/t81/vm/vm.hpp`](../../../include/t81/vm/vm.hpp) (`IVirtualMachine`, `make_interpreter_vm`).

## Maturity

`Stable` for interpreter execution path in bounded deterministic profile.  
`Experimental` for trace/JIT acceleration path pending full equivalence status.

## Principal Data Structures and Interfaces

- `State` and `ThreadContext`: registers, memory segments, handles, trace and Axion event logs  
  [`include/t81/vm/state.hpp`](../../../include/t81/vm/state.hpp)
- Trap taxonomy (`DecodeFault`, `BoundsFault`, `SecurityFault`, etc.)  
  [`include/t81/vm/traps.hpp`](../../../include/t81/vm/traps.hpp)
- Interpreter implementation and dispatch switch  
  [`core/vm/vm.cpp`](../../../core/vm/vm.cpp)
- Internal helper modules for memory/GC/policy-bridge/tensor/value operations  
  [`core/vm/internal/`](../../../core/vm/internal)

## Internal Structure (Interpreter)

```mermaid
flowchart TD
    VM[Interpreter]
    STEP[step()]
    LOAD[load_program()]
    RUN[run_to_halt()]
    STATE[State + ThreadContext]
    AX[Axion Engine]
    JIT[JitCompiler + compiled_traces]
    HELP[internal helpers]

    VM --> LOAD
    VM --> STEP
    VM --> RUN
    LOAD --> STATE
    STEP --> STATE
    STEP --> AX
    STEP --> JIT
    STEP --> HELP
```
Diagram source: [`../diagrams/vm-internal-structure.mmd`](../diagrams/vm-internal-structure.mmd)

## Dispatch / Enforcement Flow

```mermaid
flowchart TD
    A[step()] --> B{halted or no runnable context?}
    B -- yes --> Z[return]
    B -- no --> C[deterministic fault injection check]
    C --> D{compiled trace at PC?}
    D -- yes --> E[Axion check: trace enter]
    E --> F[execute trace with per-insn policy callback]
    F --> G[Axion check: trace exit/deopt]
    G --> H{deopt?}
    H -- yes --> I[invalidate trace, continue interpreter]
    H -- no --> Z
    D -- no --> J[fetch insn, increment counters]
    I --> J
    J --> K[hotspot accounting + optional trace compile]
    K --> L[Axion step check]
    L --> M{verdict deny?}
    M -- yes --> N[record event -> SecurityFault]
    M -- no --> O[switch(opcode)]
    O --> P{trap?}
    P -- yes --> Q[log trace+event -> return trap]
    P -- no --> R[sync system registers, rotate context]
    R --> Z
```
Diagram source: [`../diagrams/vm-dispatch-flow.mmd`](../diagrams/vm-dispatch-flow.mmd)

## Key Invariants / Guarantees

1. Axion policy is evaluated before instruction effects (`kStep`) and around JIT trace transitions (`kJitTraceEnter`, `kJitTraceExit`, `kJitTraceDeopt`).
2. Out-of-bounds memory paths are converted to deterministic bounds faults with recorded reason strings (through `log_bounds_fault` helpers).
3. Trap results are explicit `std::expected<void, Trap>` results from `step()` and `run_to_halt()`.
4. Context scheduling is deterministic round-robin over active contexts.
5. GC runs on a deterministic interval counter (`kGcInterval`) with explicit Axion event logging.

## Principal Failure Modes and Handling

| Failure mode | Trigger surface | Handling path |
| :--- | :--- | :--- |
| `DecodeFault` | invalid register/handle/decode preconditions | trap from opcode branch in `step()` |
| `BoundsFault` | invalid stack/heap/tensor/meta access | `log_bounds_fault(...)` + trap |
| `TypeFault` | mismatched `ValueTag`/operand expectations | trap in opcode handling |
| `SecurityFault` | Axion deny verdict or blocked privileged opcodes | Axion event + trap |
| `TierFault` | cognitive-tier constraint violations | deterministic deny reason + trap |

## Determinism Boundary Notes

- Interpreter path is in DCP scope (see profile/registry docs).
- JIT components exist in the runtime but should be treated as non-DCP until equivalence status is explicitly upgraded.
- Floating-point transcendentals/division remain bounded-exception surfaces per VM spec.

## Indeterminate

- End-to-end formal proof of interpreter/JIT equivalence is not asserted here.
- This document does not classify every opcode as DCP/non-DCP; it describes VM-layer behavior only.

## Evidence

- [`core/vm/vm.cpp`](../../../core/vm/vm.cpp)
- [`include/t81/vm/vm.hpp`](../../../include/t81/vm/vm.hpp)
- [`include/t81/vm/state.hpp`](../../../include/t81/vm/state.hpp)
- [`include/t81/vm/traps.hpp`](../../../include/t81/vm/traps.hpp)
- [`spec/t81vm-spec.md`](../../../spec/t81vm-spec.md)
- [`docs/product/DETERMINISTIC_CORE_PROFILE.md`](../../product/DETERMINISTIC_CORE_PROFILE.md)
- [`docs/governance/DETERMINISM_SURFACE_REGISTRY.md`](../../governance/DETERMINISM_SURFACE_REGISTRY.md)

## Acceptance Criteria

- Every behavioral claim maps to at least one item in **Evidence**.
- Trap and policy enforcement paths are documented explicitly.
- Determinism claims are bounded and do not exceed DCP scope.
- All diagrams render in GitHub Markdown.
