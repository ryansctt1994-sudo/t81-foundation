# Chapter 8: The Axion Safety Kernel

## 8.1 Formal Definition

**Status: Implemented & Tested**

The **Axion Kernel** is the guardian of the T81 runtime. Unlike traditional operating systems where security is enforced at the system call boundary (User/Kernel mode switch), Axion enforces security at the **instruction level**.

Formally, the Axion Kernel is a function $\alpha$ that maps the current machine state $S$ and the proposed operation $Op$ to a verdict $V$:
$$
\alpha: (S, Op) \to \{\text{Allow}, \text{Deny}, \text{Warn}, \text{Defer}\}
$$

This evaluation happens **before** the state transition $S \xrightarrow{Op} S'$ occurs. If $\alpha(S, Op) = \text{Deny}$, the transition is aborted, and the machine traps with a `SecurityFault`.

## 8.2 The Policy Model

**Status: Implemented**

Policies are declarative rulesets that define the constraints for a specific execution context. A policy does not say *what* to compute, but *how* it is allowed to compute.

### 8.2.1 Policy Language (S-Expressions)
Axion policies are defined using a Lisp-like S-expression syntax, ensuring easy parsing and canonicalization.

**Example: A Strict Tier 1 Policy**
```lisp
(policy
  (tier 1)                  ; Restrict to Symbolic Tier (No recursion, no reflection)
  (max-instructions 10000)  ; Hard gas limit
  (max-stack 256)           ; Stack depth limit
  (max-tensors 0)           ; No tensor allocations allowed
  (allowed-tensor-hashes []) ; No external weights allowed
)
```

**Example: A Tier 3 AI Inference Policy**
```lisp
(policy
  (tier 3)
  (max-recursion 1024)
  (max-tensors 50)
  (max-tensor-elements 1000000)
  (allowed-tensor-hashes [
    "canon:sha3:a7f..." ; Specific allowed model weights
  ])
)
```

### 8.2.2 Capabilities
Capabilities are granular permissions granted to a process.
*   **NetAccess**: Ability to use `IoNet` handles (Tier 4).
*   **MetaWrite**: Ability to modify the Meta segment (Reflection).
*   **InfExpand**: Ability to instantiate infinite forms (Tier 5).

## 8.3 Instruction Interception

**Status: Implemented & Tested**

The Axion Kernel is integrated directly into the VM's fetch-decode-execute loop.

### 8.3.1 The Interceptor Hook
In `src/vm/vm.cpp`, the main loop invokes the policy engine:

```cpp
// Pseudocode of the Interpreter Loop
while (!halted) {
    Opcode op = fetch();

    // 1. Axion Check
    Verdict v = axion->evaluate(ctx);
    if (v == Verdict::Deny) {
        throw SecurityFault(v.reason);
    }

    // 2. Execution
    execute(op);

    // 3. Audit Logging
    if (v == Verdict::Warn || policy.audit_all) {
        trace.log(op, v, state_hash);
    }
}
```

### 8.3.2 Zero-Cost Abstractions?
No. T81 explicitly rejects "Zero-Cost Abstractions" if they compromise safety. The Axion check imposes a performance overhead. This is a deliberate design choice: **Correctness > Performance**. However, for JIT-compiled traces, the policy checks are performed once during trace recording and baked into the optimized trace as guarded assertions, reducing runtime overhead significantly.

## 8.4 The Audit Log (Trace)

**Status: Implemented & Tested**

The **Trace** is the cryptographic proof of what happened. It is not just a debug log; it is a Merkle chain of events.

### 8.4.1 Trace Structure
Each entry in the log contains:
1.  **Tick**: The logical clock time.
2.  **Opcode**: The instruction executed.
3.  **Verdict**: The Axion decision.
4.  **StateHash**: A SHA3-256 hash of the relevant machine state *after* the operation.

$$
H_{t} = \text{Hash}(H_{t-1} || \text{Op}_t || \text{Verdict}_t || \text{StateDiff}_t)
$$

The final hash $H_n$ is the **Proof of Execution**. If two parties run the same code and get the same $H_n$, they are cryptographically guaranteed to have reached the exact same state via the exact same path.

## 8.5 Cognitive Promotion

**Status: Implemented**

A program starts at a specific Cognitive Tier (usually Tier 1). It may request **Promotion** to a higher tier to perform more complex operations.

*   **Request**: The program executes a `Promote` opcode with a signed capability token.
*   **Evaluation**: Axion validates the token against the policy.
*   **Result**: If allowed, the VM's `tier_status` is updated, unlocking new opcodes (e.g., `Recurse` or `Gossip`).

**Tier Escalation Path**:
1.  **Tier 1**: Safe, bounded, polynomial time.
2.  **Tier 2**: Dynamic, reflective.
3.  **Tier 3**: Recursive, exponential time potential (requires gas limits).
4.  **Tier 4**: Non-local, network dependent (requires consensus limits).
5.  **Tier 5**: Infinite (requires strict containment).

> **Verification**: `tests/cpp/test_ethics.cpp` verifies that attempts to use Tier 3 opcodes in a Tier 1 policy result in a `SecurityFault`.
