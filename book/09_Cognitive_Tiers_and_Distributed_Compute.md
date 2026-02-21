# Chapter 9: Cognitive Tiers and Distributed Compute

## 9.1 The Cognitive Tier Model

T81 organizes computational complexity into a hierarchy of **Cognitive Tiers**. This allows the Axion Kernel to reason about the *intent* and *capabilities* of a program before execution.

| Tier | Name | Capabilities | Recursion Limit |
| :--- | :--- | :--- | :--- |
| **0** | **Base** | Basic arithmetic (`Add`, `Sub`), linear flow. | 0 |
| **1** | **Symbolic** | Tensor ops, basic loops. | 81 |
| **2** | **Reflective** | `MetaRead`, `MetaReflect`. | 243 |
| **3** | **Recursive** | Self-modification, proof generation. | 1024 (Policy) |
| **4** | **Distributed** | Gossip, State Merging. | N/A |
| **5** | **Infinite** | Geometric Series, non-terminating forms. | N/A |

### 9.1.1 Tier Escalation
Escalation is explicit. A program must request higher tier privileges via specific opcodes (`Call`, `Recurse`, `MetaReflect`). Axion policies determine if the promotion is granted.

## 9.2 Distributed Compute (Tier 4)

The **Distributed Tier** enables multiple T81VM instances to operate as a coherent swarm.

### 9.2.1 Gossip Protocol
Nodes exchange state updates via a deterministic gossip protocol.
*   **Message Format**: `(Tag, Payload, LamportTick, NodeID)`.
*   **Merge Strategy**: CRDT-like (Conflict-free Replicated Data Type) merging based on `TickSync` timestamps.
*   **Determinism**: Given the same sequence of message arrivals, the final merged state is identical on all nodes.

### 9.2.2 Logical Clocks (TickSync)
The VM maintains a Lamport logical clock (`R75`).
*   **Internal Tick**: Increments on every instruction.
*   **Sync**: Updates on message receipt: `Tick = max(LocalTick, RemoteTick) + 1`.
*   **Coherence**: The `Coherence` opcode returns the drift between local and global ticks, allowing the application to throttle or adapt.

> **Note**: While network latency is non-deterministic, the *reaction* to messages is deterministic based on their arrival order and content.

## 9.3 Trace-Based JIT Compilation

T81VM implements a **Trace-JIT** to optimize hot code paths without sacrificing determinism.

### 9.3.1 JIT Equivalence Model
The JIT compiler guarantees **Behavioral Equivalence** to the interpreter.
1.  **Tracing**: The VM records a sequence of executed instructions (a "trace") during interpretation.
2.  **Compilation**: The trace is optimized and compiled to native code.
3.  **Guards**: The compiled code includes guards that verify assumptions (e.g., type tags, loop bounds).
4.  **Deoptimization**: If a guard fails, execution instantly falls back to the interpreter at the exact same program counter (`GuardDeopt`).

> **Source**: `src/vm/jit_compiler.cpp`.

### 9.3.2 Divergence Detection
If the JIT-compiled code diverges from the canonical interpreter behavior (e.g., due to a compiler bug), the Axion Kernel detects this via trace mismatches (in debug/audit mode) or simply by the `GuardDeopt` mechanism ensuring correctness at runtime.

## 9.4 Infinite Forms (Tier 5)

Tier 5 introduces "Infinite" data structures, such as geometric series.
*   **Representation**: A finite generator `(a, r)` represents the series $a + ar + ar^2 + \dots$.
*   **Operations**: `InfExpand` computes the $N$-th term. `InfConverge` checks if $|r| < 1$.
*   **Signature**: `InfSignature` generates a unique hash of the *generating function*, not the infinite data.

## 9.5 Verification Checklist

*   [ ] **Promotion**: Does attempting recursion > 81 without permission fail?
*   [ ] **TickSync**: Does the logical clock increment exactly once per instruction?
*   [ ] **JIT**: Do optimized traces produce identical memory and register states as the interpreter?
