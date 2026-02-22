# Chapter 9: Cognitive Tiers and Distributed Compute

## 9.1 The Cognitive Tier Model

**Status: Implemented**

T81 organizes computational complexity into a hierarchy of **Cognitive Tiers**. This allows the Axion Kernel to reason about the *intent* and *capabilities* of a program before execution.

| Tier | Name | Capabilities | Recursion Limit |
| :--- | :--- | :--- | :--- |
| **0** | **Base** | Basic arithmetic (`Add`, `Sub`), linear flow. | 0 |
| **1** | **Symbolic** | Tensor ops, basic loops. | 81 |
| **2** | **Reflective** | `MetaRead`, `MetaReflect`. | 243 |
| **3** | **Recursive** | Self-modification, proof generation. | 1024 (Policy) |
| **4** | **Distributed** | Gossip, State Merging. | N/A |
| **5** | **Infinite** | Geometric Series, non-terminating forms. | N/A |

> **Implementation**: Tier logic is modularized in `src/cog/tier[1-5]/`.

## 9.2 Distributed Compute (Tier 4)

**Status: Implemented & Tested**

The **Distributed Tier** enables multiple T81VM instances to operate as a coherent swarm.

### 9.2.1 Gossip Protocol
Nodes exchange state updates via a deterministic gossip protocol using `Gossip` and `Merge` opcodes.
*   **Message Format**: `(Tag, Payload, LamportTick, NodeID)`.
*   **Merge Strategy**: CRDT-like merging based on `TickSync` timestamps.
*   **Determinism**: Given the same sequence of message arrivals, the final merged state is identical on all nodes.

> **Reference**: `src/cog/tier4/distributed.cpp` and `tests/cpp/test_tier4_distributed.cpp`.

### 9.2.2 Logical Clocks (TickSync)
The VM maintains a Lamport logical clock (`R75`).
*   **Internal Tick**: Increments on every instruction.
*   **Sync**: Updates on message receipt: `Tick = max(LocalTick, RemoteTick) + 1`.
*   **Coherence**: The `Coherence` opcode returns the drift between local and global ticks.

## 9.3 Infinite Forms (Tier 5)

**Status: Implemented & Tested**

Tier 5 introduces "Infinite" data structures, such as geometric series.
*   **Representation**: A finite generator `(a, r)` represents the series $a + ar + ar^2 + \dots$.
*   **Operations**: `InfExpand` computes the $N$-th term. `InfConverge` checks if $|r| < 1$.
*   **Signature**: `InfSignature` generates a unique hash of the *generating function*, not the infinite data.

> **Reference**: `src/cog/tier5/infinite.cpp` and `tests/cpp/test_infinite_opcodes.cpp`.

## 9.4 Verification Checklist

*   [ ] **Promotion**: Does attempting recursion > 81 without permission fail? (Verified by `tests/cpp/axion_recursion_guardrails_test.cpp`)
*   [ ] **TickSync**: Does the logical clock increment exactly once per instruction? (Verified by `tests/cpp/axion_instruction_counter_test.cpp`)
*   [ ] **Tier 5**: Do `InfExpand` and `InfConverge` produce correct results? (Verified by `tests/cpp/tier5_test.cpp`)
