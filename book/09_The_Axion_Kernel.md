# Chapter 9: The Axion Safety Kernel

## 9.1 Formal Definition

**Status: Implemented**

The **Axion Safety Kernel** is the reference monitor for the T81VM. It is formally defined as a function $\alpha$ that maps the current machine state $S$ and the proposed operation $Op$ to a verdict $V$.

$$
\alpha: (S, Op) \to V \in \{\text{Allow}, \text{Deny}, \text{Warn}, \text{Defer}\}
$$

Unlike a firewall which filters network packets, Axion filters *instructions*. It operates with zero-trust: the bytecode is assumed to be malicious until proven compliant with the policy.

## 9.2 The Policy Model

**Status: Implemented**

A Policy $\mathcal{P}$ is a configuration object loaded at runtime (either embedded in the bytecode or provided by the host). It defines the limits of execution.

```json
{
  "max_memory": 1048576,
  "max_stack_depth": 512,
  "max_instructions": 1000000,
  "allowed_syscalls": ["print", "read_tensor"],
  "cognitive_tier_limit": 3,
  "ethics_level": "standard"
}
```

### 9.2.1 Capabilities
Axion uses a **Capability-Based Security** model. Certain operations require specific "Capability Tokens" to be present in the active context.
*   **NetAccess**: Ability to open sockets (Tier 4).
*   **IoRead**: Ability to read from CanonFS.
*   **IoWrite**: Ability to write new blobs (rare).
*   **MetaWrite**: Ability to modify the Meta segment (Reflection).
*   **InfExpand**: Ability to instantiate infinite forms (Tier 5).

## 9.3 Instruction Interception

**Status: Implemented & Tested**

The Axion Kernel is integrated directly into the VM's fetch-decode-execute loop. This integration is non-bypassable.

### 9.3.1 The Interceptor Hook
In `src/vm/vm.cpp`, the main loop invokes the policy engine:

```cpp
// Pseudocode of the Interpreter Loop
while (!halted) {
    Opcode op = fetch();

    // 1. Axion Check
    Verdict v = axion->evaluate(ctx, op);
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

### 9.3.2 Zero-Cost Abstractions?
No. T81 explicitly rejects "Zero-Cost Abstractions" if they compromise safety. The Axion check imposes a performance overhead. This is a deliberate design choice: **Correctness > Performance**. However, for JIT-compiled traces, the policy checks are performed once during trace recording and baked into the optimized trace as guarded assertions, reducing runtime overhead significantly (amortized $O(1)$).

## 9.4 The Audit Log (Trace)

**Status: Implemented & Tested**

The **Trace** is the cryptographic proof of what happened. It is not just a debug log; it is a Merkle chain of events.

### 9.4.1 Trace Structure
Each entry in the log contains:
1.  **Tick**: The logical clock time.
2.  **Opcode**: The instruction executed.
3.  **Verdict**: The Axion decision.
4.  **StateHash**: A SHA3-256 hash of the relevant machine state *after* the operation.

$$
H_{t} = \text{Hash}(H_{t-1} || \text{Op}_t || \text{Verdict}_t || \text{StateDiff}_t)
$$

The final hash $H_n$ is the **Proof of Execution**. If two parties run the same code and get the same $H_n$, they are cryptographically guaranteed to have reached the exact same state via the exact same path.

## 9.5 Cognitive Promotion

**Status: Implemented**

A program starts at a specific Cognitive Tier (usually Tier 1). It may request **Promotion** to a higher tier to perform more complex operations.

*   **Request**: The program executes a `Promote` opcode with a signed capability token.
*   **Evaluation**: Axion validates the token against the policy.
*   **Result**: If allowed, the VM's `tier_status` is updated, unlocking new opcodes (e.g., `Recurse` or `Gossip`).

**Tier Escalation Path**:
1.  **Tier 1**: Safe, bounded, polynomial time. (Default)
2.  **Tier 2**: Dynamic, reflective. (Requires explicit flag)
3.  **Tier 3**: Recursive, exponential time potential (requires gas limits).
4.  **Tier 4**: Non-local, network dependent (requires consensus limits).
5.  **Tier 5**: Infinite (requires strict containment).

> **Verification**: `tests/cpp/test_ethics.cpp` verifies that attempts to use Tier 3 opcodes in a Tier 1 policy result in a `SecurityFault`.
