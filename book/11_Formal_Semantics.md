# Chapter 11: Formal Semantics Layer

## 11.1 Overview

This chapter defines the formal semantics of the T81 Virtual Machine (T81VM) and the Ternary Instruction Set Computer (TISC). It distinguishes between the currently **Implemented Semantics** (C++ reference implementation), the **Derived Semantics** (theoretical properties guaranteed by the implementation), and the **Aspirational Semantics** (the long-term formal ideal).

## 11.2 Denotational Semantics of TISC

We define the T81VM state space $\Sigma$ as the Cartesian product of its components:

$$
\Sigma = R \times M \times K \times \Phi
$$

Where:
*   $R \in \mathbb{T}^{81}$: The register file (81 trits/words).
*   $M$: The memory map, partitioned into discrete segments (Stack, Heap, Tensor, Meta).
*   $K$: The control state (PC, SP, CallDepth, AxionLog).
*   $\Phi$: The flags register (Zero, Negative, Positive).

### 11.2.1 The Transition Function $\delta$

The execution of a single TISC instruction $i$ is modeled as a state transition function:

$$
\delta : \Sigma \times I \to \Sigma \cup \{\bot\}
$$

Where $I$ is the set of valid instructions and $\bot$ represents a trapped or halted state.

#### Implemented Semantics (C++)
In `src/vm/vm.cpp`, the function `Interpreter::step()` implements $\delta$.
*   **Atomicity**: Each `step()` corresponds to exactly one opcode execution (or one trace execution in JIT mode).
*   **Partial Function**: The function returns `std::expected<void, Trap>`, mapping $\bot$ to `Trap` variants (e.g., `Trap::DivisionFault`, `Trap::SecurityFault`).
*   **Side Effects**: The Axion Log (`state_.axion_log`) is append-only and strictly monotonic.

#### Derived Semantics
From the implementation of `Opcode::Add` and `T81Int`, we derive that for any state $S$:
$$
\delta(S, \text{ADD } r_a, r_b, r_c) \implies S'.R[r_a] = (S.R[r_b] + S.R[r_c]) \pmod{3^{64}}
$$
(assuming standard `int64_t` wrapping behavior, though `T81Int` effectively models infinite precision for small values).

### 11.2.2 Floating Point Semantics

The handling of `T81Float` represents the most significant divergence between implemented and aspirational semantics.

#### Implemented Semantics
The VM currently uses `std::vector<double>` for `ValueTag::FloatHandle`.
*   **Representation**: IEEE 754 binary64.
*   **Operations**: Host hardware instructions (`fadd`, `fmul`).
*   **Transcendental Functions**: `std::sin`, `std::exp` (Platform-dependent).
*   **Determinism**: **Weak**. Subject to compiler optimizations (`-ffast-math`) and libm variations.

> **Code Reference**: `src/vm/vm.cpp` uses `alloc_float(double)` and `float_ptr(handle) -> double*`.

#### Aspirational Semantics
The target semantics defined in `include/t81/core/T81Float.hpp`:
*   **Representation**: Balanced Ternary Floating Point (`T81Int<1 + E + M>`).
*   **Operations**: Software-defined arithmetic (`dmath`).
*   **Normalization**: Canonical rewriting $N(f) \to f'$ such that representation is unique.
*   **Determinism**: **Strong**. Bit-exact results on any architecture.

## 11.3 Canonicalization as a Rewriting System

Canonicalization is the process of mapping equivalent representations to a unique normal form.

### 11.3.1 Definition

Let $A$ be the set of all valid object encodings (e.g., non-normalized floats, redundant graph structures). We define a rewriting relation $\to_R$ such that:

1.  **Termination**: There are no infinite chains $a_1 \to_R a_2 \to_R \dots$
2.  **Confluence**: If $a \to_R^* b$ and $a \to_R^* c$, there exists $d$ such that $b \to_R^* d$ and $c \to_R^* d$.

The **Normal Form** of $a$ is the unique $n$ such that $a \to_R^* n$ and no rule applies to $n$.

### 11.3.2 Implemented Rewriting (Tier 1)
In `src/cog/tier1/symbolic.cpp` (conceptual), symbolic graphs are rewritten.
*   **Rules**: $x + 0 \to x$, $x \times 1 \to x$.
*   **Status**: `Opcode::SymCanon` is a placeholder in `vm.cpp` that invokes `graph->canonicalize()`.

### 11.3.3 Aspirational Rewriting (Float)
The `T81Float::normalize` function in `include/t81/core/T81Float.hpp` implements a rewriting step for ternary floats:
*   **Rule**: Shift mantissa until MSB is non-zero, adjust exponent.
*   **Invariant**: $v(f) = v(normalize(f))$.

## 11.4 Determinism Proof Surfaces

We analyze the surfaces where determinism must be proven.

### 11.4.1 The Integer Ring $\mathbb{Z}_{3^N}$
*   **Claim**: `T81Int` arithmetic is strictly deterministic.
*   **Proof Sketch**:
    1.  `T81Int` uses `std::vector<uint8_t>` or `uint64_t` limbs.
    2.  Operations are implemented via integer ALU instructions with well-defined 2's complement behavior.
    3.  No floating-point hardware is involved.
    4.  Therefore, output is function of input only.
*   **Status**: Verified by `tests/cpp/test_property_invariants.cpp`.

### 11.4.2 The Trace-JIT Equivalence
*   **Claim**: Execution via Interpreter is equivalent to execution via JIT Trace.
*   **Formal Statement**: $\forall P, I: \text{Eval}_{Interp}(P, I) \equiv \text{Eval}_{JIT}(P, I)$.
*   **Proof Sketch**:
    1.  `JitCompiler` records a sequence of `t81::tisc::Insn` $t = [i_1, \dots, i_k]$ from the interpreter stream.
    2.  `ThreadedJitTrace::execute(state)` iterates over $t$ applying the exact same logic block as `Interpreter::step()` for each opcode.
    3.  `vm.cpp` explicitly disables tracing for non-linear control flow (`Call`, `Ret`, `Jump`), ensuring JIT traces are pure basic blocks.
    4.  State mutations ($R$, $M$) are identical.
*   **Vulnerability**: If `ThreadedJitTrace` implementation diverges from `Interpreter` (copy-paste error), equivalence breaks.
*   **Mitigation**: `ctest` coverage should include mixed JIT/Interp runs.

## 11.5 Formal Limits

### 11.5.1 Recursion Boundedness
*   **Implemented**: `kHardRecursionCeiling` in `vm.cpp` (constant `T81_HARD_RECURSION_CEILING`).
*   **Semantics**: $\text{CallDepth}(S) > K \implies \delta(S) = \bot_{\text{SecurityFault}}$.
*   **Verification**: `tests/cpp/test_tier3_opcodes.cpp`.

### 11.5.2 Memory Boundedness
*   **Implemented**: Fixed segment limits (`kDefaultStackSize`, `kDefaultHeapSize`).
*   **Semantics**: $addr \notin [S_{start}, S_{limit}) \implies \delta(S) = \bot_{\text{BoundsFault}}$.

## 11.6 Conclusion

The T81VM formal semantics are currently in a **hybrid state**:
1.  **Control Flow & Integer Arithmetic**: Formally strict and deterministic (Tier A).
2.  **Floating Point**: Pragmatic and host-dependent (Tier C).
3.  **Symbolic/Cognitive**: Experimental placeholders (Tier B/D).

Future work must prioritize migrating `ValueTag::FloatHandle` to use `T81Float` storage to elevate Floating Point semantics to Tier A.
