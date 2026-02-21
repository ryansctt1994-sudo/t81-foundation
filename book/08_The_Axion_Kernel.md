# Chapter 8: The Axion Safety Kernel

## 8.1 Formal Definition

The **Axion Kernel** is the capability-based supervisor that governs the execution of the T81VM. It enforces a strict separation between *mechanism* (TISC opcodes) and *policy* (safety constraints).

Formally, Axion is a function $\mathcal{A}: (S, I) \to \{ \text{Allow}, \text{Deny}, \text{Warn}, \text{Defer} \}$, where $S$ is the current VM state and $I$ is the proposed instruction.

## 8.2 The Policy Model

Axion policies are declarative rulesets that define the permissible envelope of execution. Policies are typically serialized as JSON or YAML and loaded at runtime.

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

The T81VM invokes Axion before executing sensitive instructions. This interception mechanism is the primary enforcement point.

### 8.3.1 The Syscall Interface
The VM calls `eval_axion_call` with a `SyscallContext`:
*   `caller`: The executing module.
*   `syscall`: The operation identifier (e.g., `kAxRead`, `kMetaWrite`).
*   `payload`: Arguments or target addresses.
*   `pc`: Current program counter.
*   `next_opcode`: The instruction about to execute.

> **Source**: `src/vm/vm.cpp`, method `Interpreter::eval_axion_call`.

### 8.3.2 Verdicts
Axion returns a `Verdict` struct:
*   **Allow**: The operation proceeds.
*   **Deny**: The operation is blocked, and the VM traps with `SecurityFault`.
*   **Warn**: The operation proceeds, but a warning is logged in the trace.
*   **Defer**: The decision is deferred to a higher-tier logic (e.g., a Tier 5 meta-policy).

## 8.4 The Audit Log (Trace)

Every significant Axion decision is recorded in the **Axion Trace**. This log is an append-only sequence of `AxionEvent` records.

### 8.4.1 Event Structure
```cpp
struct AxionEvent {
    Opcode opcode;
    int32_t tag;
    int64_t value;
    Verdict verdict;
};
```
(See `src/axion/engine.hpp`)

This trace provides a cryptographic proof that the execution adhered to the active policy.

## 8.5 Cognitive Promotion

Axion manages the escalation of privileges through **Cognitive Tiers**.

### 8.5.1 Promotion Logic
When a program attempts to exceed its current tier's limits (e.g., recursion depth > 81), the VM requests a promotion via `try_promote`.
*   Axion checks if the policy allows the target tier.
*   If allowed, the `tier_status` is updated, and a promotion event is logged.
*   If denied, the VM traps.

> **Verification**: See `Opcode::Call` handling in `src/vm/vm.cpp`.

## 8.6 Capability Model

Axion implements an Object-Capability (OCap) model. Resources (files, network sockets) are represented as unforgeable handles.
*   **Creation**: Only authorized syscalls can create handles.
*   **Use**: Opcodes operate on handles, not raw addresses.
*   **Revocation**: Handles can be revoked by the policy at any time.

This ensures that a compromised TISC program cannot access resources it was not explicitly granted.

## 8.7 Verification Checklist

*   [ ] **Interception**: Do all opcodes in `vm.cpp` that touch memory/IO call `eval_axion_call`?
*   [ ] **Verdict**: Does `VerdictKind::Deny` always result in a `SecurityFault`?
*   [ ] **Trace**: Is every Axion decision logged with a correct `tag` and `value`?
