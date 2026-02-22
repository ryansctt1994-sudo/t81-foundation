# Chapter 9: Cognitive Tiers and Distributed Compute

## 9.1 The Cognitive Tier Model

**Status: Implemented**

T81 organizes computational capability into **Cognitive Tiers**. This taxonomy allows the system to bound the "danger" or "cost" of a computation. A program must explicitly request promotion to higher tiers to access advanced capabilities.

| Tier | Name | Capability | Constraint | Axion Policy |
| :--- | :--- | :--- | :--- | :--- |
| **1** | **Symbolic** | Basic arithmetic, fixed loops. | $O(N)$ or Polynomial time. | Default. No recursion. |
| **2** | **Reflective** | Self-inspection, dynamic dispatch. | Can inspect own source. | Requires `max-reflections`. |
| **3** | **Recursive** | General recursion, proof generation. | Turing Complete (Halting risk). | Requires `max-recursion`. |
| **4** | **Distributed** | Gossip, Consensus, State Merging. | Network latency, CAP theorem. | Requires `NetAccess`. |
| **5** | **Infinite** | Geometric series, non-terminating forms. | Unbounded. | Requires `InfExpand`. |

### 9.1.1 Promotion Mechanism
A process starts at Tier 1. To escalate:
1.  **Request**: `Promote` opcode with a capability token.
2.  **Audit**: Axion validates the request against the active policy.
3.  **Grant**: If successful, the VM unlocks the corresponding opcodes (e.g., `Recurse`, `InfExpand`).

## 9.2 Distributed Compute (Tier 4)

**Status: Experimental**

Tier 4 extends the T81VM across network boundaries. It treats the network not as a socket abstraction, but as a **Distributed Shared Memory** system governed by consensus.

### 9.2.1 State Merging
When two nodes compute on the same dataset, their states $S_A$ and $S_B$ may diverge. Tier 4 provides a mechanism to merge these states deterministically using **CRDTs (Conflict-free Replicated Data Types)** or **Paxos-like Consensus**.

*   **Gossip Protocol**: Nodes exchange `StateHash` digests.
*   **Convergence**: If $Hash(S_A) \neq Hash(S_B)$, the nodes exchange the full trace.
*   **Resolution**: Since execution is deterministic, the node with the longer valid trace (Proof of Work/Time) is typically considered authoritative, or a merge function is applied.

### 9.2.2 The "Time Travel" Attack
In a distributed system, a malicious node might withhold a state transition and release it later to invalidate others' work. Tier 4 mitigates this by requiring **Lamport Timestamps** on all state transitions. A transition $S_t \to S_{t+1}$ is only valid if signed by a quorum of nodes or if the timestamp is strictly monotonic and verified.

## 9.3 Trace-Based JIT Compilation

**Status: Implemented (Local)**

While primarily a performance optimization, the **Trace-JIT** is conceptually a "Cognitive Promotion" of code.

1.  **Observation**: The VM watches the execution of Tier 1/2 code.
2.  **Hypothesis**: "This loop $L$ will execute $N$ times with types $T$."
3.  **Synthesis**: The JIT compiles a specialized, optimized version of $L$.
4.  **Verification**: The optimized code includes **Guards** to ensure the hypothesis remains true.

This process mirrors the cognitive act of "learning": converting explicit, slow reasoning (interpretation) into implicit, fast intuition (compiled code).

## 9.4 Infinite Forms (Tier 5)

**Status: Implemented (Geometric)**

Tier 5 deals with **Infinite Forms**—computations that do not terminate but converge to a value. T81 provides explicit support for analytic continuation and series summation.

### 9.4.1 The Infinite Object
An `InfiniteCanonicalForm` is a handle to a mathematical series, defined by:
*   **First Term ($a$)**
*   **Ratio ($r$)** (for Geometric series) or **Generator Function** ($f(n)$).

### 9.4.2 Collapse and Convergence
The `InfCollapse` opcode attempts to resolve an infinite form to a finite value.
For a Geometric Series $\sum_{n=0}^{\infty} ar^n$:
1.  **Check Convergence**: If $|r| < 1$, the series converges.
2.  **Compute Limit**: $S = \frac{a}{1-r}$.
3.  **Result**: The infinite object is replaced by the finite `T81Fraction` $S$.

If the series diverges ($|r| \ge 1$), `InfCollapse` returns a `Divergent` signature, allowing the program to handle the singularity gracefully rather than hanging.

> **Verification**: `src/cog/tier5/infinite.cpp` implements the convergence test and summation logic.
