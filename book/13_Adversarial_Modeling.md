# Chapter 13: Adversarial Modeling and Determinism Attacks

## 13.1 Threat Model

**Status: Theoretical**

T81 assumes a hostile environment. The **Host** (OS, Hardware, Operator) is considered an adversary that may attempt to:
1.  **Introduce Entropy**: Inject randomness into deterministic execution.
2.  **Forge States**: Claim a computation reached state $S'$ when it actually reached $S$.
3.  **Deny Service**: Consume infinite resources.
4.  **Leak Information**: Expose private data via side channels.

## 13.2 Compiler-Level Attacks

**Attack Vector**: "Trojan Source" / Homoglyphs.
**Description**: An attacker uses Unicode control characters (e.g., Right-to-Left Override) to make source code appear different to humans than to the compiler.
**Mitigation**: The T81 Lexer enforces a strict subset of UTF-8. Control characters and non-printable characters are rejected during tokenization.

**Attack Vector**: Token Reordering / Optimization Drift.
**Description**: A malicious compiler might reorder instructions in a way that preserves semantics on one architecture but not another (e.g., due to memory model differences).
**Mitigation**: The T81 Compiler emits a **Canonical AST**. The IR generation phase is deterministic and platform-agnostic. The `t81lang_repro_gate` verifies that the compiler output is bit-identical across runs.

## 13.3 VM and GC Attack Vectors

**Attack Vector**: Rowhammer / Bit Flips.
**Description**: Physical attacks on DRAM to flip bits in sensitive memory (e.g., changing a `Deny` verdict to `Allow`).
**Mitigation**: T81 uses **Opaque Handles** and **Memory Segmentation**. Critical kernel structures are stored in isolated pages (where possible) and validated by checksums. However, software cannot fully mitigate hardware faults without ECC memory.

**Attack Vector**: Garbage Collection Nondeterminism.
**Description**: If GC runs based on wall-clock time or memory pressure, execution traces will diverge across runs.
**Mitigation**: The T81 GC is **deterministic**. It is triggered solely by allocation counts (`bytes_allocated > threshold`). This ensures that GC pauses happen at the exact same instruction on every run.

**Attack Vector**: Timing Side-Channels.
**Description**: Observing the time it takes to compute a function (e.g., modular exponentiation) to infer secret keys.
**Mitigation**: `dmath` aims for constant-time implementations for cryptographic primitives, but general-purpose arithmetic is not guaranteed to be constant-time. T81 focuses on *functional* determinism, not *temporal* determinism (constant cycles).

## 13.4 CanonFS and Hash Attacks

**Attack Vector**: Hash Collision / Preimage.
**Description**: Finding two different inputs $A \neq B$ such that $Hash(A) = Hash(B)$.
**Mitigation**: T81 uses **SHA3-256** (Keccak), which is resistant to length-extension attacks and collision attacks. The canonical serialization rules (sorting keys, normalizing floats) minimize the attack surface by reducing the input space of valid objects.

## 13.5 Distributed Tier Time-Travel Attack

**Attack Vector**: State Withholding / Replay.
**Description**: In Tier 4, a node computes a state transition $S_t \to S_{t+1}$ but withholds it, releasing it later to invalidate other nodes' progress (a "selfish mining" equivalent).
**Mitigation**:
1.  **Lamport Timestamps**: Every transition must causally follow the previous one.
2.  **Consensus Quorums**: A state is only finalized when signed by $2/3$ of the cognitive cluster.
3.  **Trace Merging**: If branches diverge, the deterministic merge function resolves conflicts based on total computational work (trace length).

## 13.6 Determinism Breach Postmortem Template

**Status: Process**

If a determinism breach is detected (i.e., `t81lang_repro_gate` fails), the following procedure is invoked:

1.  **Isolation**: Identify the diverging inputs and the specific instruction index where trace $A$ differs from trace $B$.
2.  **Reproduction**: Create a minimal reproduction case (`repro.t81`).
3.  **Analysis**:
    *   Is it a compiler bug? (Check AST dump)
    *   Is it a VM bug? (Check `dmath` implementation)
    *   Is it a host library issue? (Check `libc` linkage)
4.  **Remediation**:
    *   Patch `dmath` to replace host fallback.
    *   Update `t81lang_repro_gate` with the new regression test.
5.  **Disclosure**: Publish a "Determinism Advisory" (if production systems affected).
