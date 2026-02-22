# Chapter 8: The Axion Safety Kernel

## 8.1 Formal Definition

**Status: Implemented & Tested**

The **Axion Kernel** is the capability-based supervisor that governs the execution of the T81VM. It enforces a strict separation between *mechanism* (TISC opcodes) and *policy* (safety constraints).

Formally, Axion is a function $\mathcal{A}: (S, I) \to \{ \text{Allow}, \text{Deny}, \text{Warn}, \text{Defer} \}$, where $S$ is the current VM state and $I$ is the proposed instruction.

## 8.2 The Policy Model

**Status: Implemented**

Axion policies are declarative rulesets that define the permissible envelope of execution.

### 8.2.1 Policy Grammar
A policy document consists of:
1.  **Directives**: Global constraints (e.g., `max_stack_depth`, `max_cycles`).
2.  **Syscalls**: Permission grants for specific operations (`io.net`, `fs.read`).
3.  **Tier Limits**: Maximum allowed Cognitive Tier.
4.  **Ethics**: Configuration for the Nine Principles ($\Theta_1 \dots \Theta_9$).

```yaml
policy:
  version: "1.0"
  directives:
    max_stack_depth: 1024
    max_cycles: 1000000
    allow_recursion: true
  syscalls:
    - allow: "io.print"
    - deny: "fs.write"
  tiers:
    max_tier: 3
```

## 8.3 Instruction Interception

**Status: Implemented**

The T81VM invokes Axion before executing sensitive instructions. This interception mechanism is the primary enforcement point.

### 8.3.1 The Syscall Interface
The VM calls `eval_axion_call` (`src/vm/vm.cpp`) with a context containing:
*   `caller`: The executing module.
*   `syscall`: The operation identifier (e.g., `kAxRead`, `kMetaWrite`).
*   `payload`: Arguments or target addresses.
*   `pc`: Current program counter.

### 8.3.2 Verdicts
Axion returns a `Verdict` struct:
*   **Allow**: The operation proceeds.
*   **Deny**: The operation is blocked, and the VM traps with `SecurityFault`.
*   **Warn**: The operation proceeds, but a warning is logged in the trace.
*   **Defer**: The decision is deferred to a higher-tier logic.

## 8.4 The Audit Log (Trace)

**Status: Implemented**

Every significant Axion decision is recorded in the **Axion Trace**. This log is an append-only sequence of `AxionEvent` records.

> **Reference**: See `include/t81/axion/api.hpp` for the `AxionEvent` and `Verdict` definitions.

## 8.5 Cognitive Promotion

**Status: Implemented**

Axion manages the escalation of privileges through **Cognitive Tiers**. When a program attempts to exceed its current tier's limits (e.g., recursion depth > 81), the VM checks the policy. If allowed, the tier is promoted; otherwise, it traps.

> **Verification**: See `Opcode::Call` handling in `src/vm/vm.cpp`.

## 8.6 Capability Model

**Status: Implemented**

Axion implements an Object-Capability (OCap) model. Resources (files, network sockets) are represented as unforgeable handles.
*   **Creation**: Only authorized syscalls can create handles.
*   **Use**: Opcodes operate on handles, not raw addresses.
*   **Revocation**: Handles can be revoked by the policy at any time.

## 8.7 Verification Checklist

*   [ ] **Interception**: Do all opcodes in `src/vm/vm.cpp` that touch memory/IO call `eval_axion_call`? (Verified by inspection)
*   [ ] **Verdict**: Does `VerdictKind::Deny` always result in a `SecurityFault`? (Verified by `tests/cpp/vm_fault_test.cpp`)
*   [ ] **Trace**: Is every Axion decision logged with a correct `tag` and `value`? (Verified by `tests/cpp/axion_log_determinism_test.cpp`)
