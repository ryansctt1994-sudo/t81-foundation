# Chapter 12: Formal Semantics of TISC and T81VM

## 12.1 Operational Semantics

**Status: Theoretical**

The behavior of the T81VM is defined using **Small-Step Operational Semantics (SSOS)**.

Let $S$ be the set of all possible machine states. The machine is a tuple $\mathcal{M} = (S, \to)$, where $\to \subseteq S \times S$ is the transition relation.

A state $s \in S$ is a tuple $(PC, SP, \rho, \sigma, \mu)$, where:
*   $PC \in \mathbb{N}$: Program Counter.
*   $SP \in \mathbb{N}$: Stack Pointer.
*   $\rho: \text{RegID} \to \text{Value}$: Register File.
*   $\sigma: \mathbb{N} \to \text{Value}$: Operand Stack.
*   $\mu: \text{Addr} \to \text{Value}$: Memory.

### 12.1.1 Transition Rules
The transition rules are defined for each opcode. For example, the `ADD` instruction:

$$
\frac{Code[PC] = \text{ADD} \quad v_1 = \sigma(SP) \quad v_2 = \sigma(SP-1)}{
(PC, SP, \rho, \sigma, \mu) \to (PC+1, SP-1, \rho, \sigma[SP-1 \mapsto v_1 + v_2], \mu)
}
$$

This rule states: If the current instruction is `ADD`, pop two values from the stack, add them, push the result, and increment PC.

## 12.2 Algebraic Transition Function

**Status: Implemented**

The implementation in `src/vm/vm.cpp` corresponds to the function $\delta: S \times \text{Op} \to S$.

$$
S_{t+1} = \delta(S_t, \text{Fetch}(S_t.PC))
$$

This function is:
1.  **Total**: Defined for all valid states and opcodes.
2.  **Deterministic**: Maps a single input state to a single output state.
3.  **Computable**: Requires finite resources (bounded by Axion).

## 12.3 Canonicalization Rewrite System

**Status: Implemented**

T81 employs a rewrite system to ensure that semantically equivalent objects have identical binary representations. This is crucial for CanonFS.

Let $\mathcal{O}$ be the set of objects. We define a canonicalization function $C: \mathcal{O} \to \mathcal{O}$.
For any two objects $x, y$:
$$
x \equiv y \iff C(x) = C(y)
$$

Examples:
*   **Floats**: $C(\text{NaN}) = \text{0x7ff8...}$ (Canonical NaN).
*   **Maps**: $C(\{b:1, a:2\}) = \{a:2, b:1\}$ (Key Sorting).
*   **Zero**: $C(-0.0) = +0.0$ (Signed Zero Normalization).

## 12.4 Determinism Proof Sketches

### 12.4.1 Floating Point Independence
**Theorem**: For any sequence of floating point operations $F$, $Val(F)$ is independent of the host FPU.
**Proof Strategy**: By induction on the length of $F$. Base cases (literals) are fixed. Inductive step: `dmath` operations rely only on integer arithmetic, which is consistent across Two's Complement machines (proven by C++ standard).

### 12.4.2 Trace Integrity
**Theorem**: If $S_n$ is reached, then $H_n$ (Trace Hash) uniquely identifies the path $S_0 \dots S_n$.
**Proof Strategy**: Follows from the collision resistance of SHA3-256. Since $H_t = Hash(H_{t-1} || Op_t)$, finding a different path to the same hash implies finding a collision.

## 12.5 Interpreter vs. Trace-JIT Equivalence

**Status: Verification**

The JIT compiler is an optimization function $Opt: \text{Trace} \to \text{MachineCode}$.
We require that:
$$
\forall s \in S: \text{Exec}_{\text{Nat}}(Opt(T), s) \equiv \text{Exec}_{\text{Int}}(T, s)
$$

This is enforced by **Guards**. A compiled trace $T'$ consists of a sequence of check instructions $G_1, G_2 \dots$ followed by operations.
$$
\text{Exec}(T', S) = \begin{cases}
\text{OptimizedExec}(S) & \text{if } \forall k, G_k(S) \\
\text{Deoptimize}(S) \to I(S) & \text{otherwise}
\end{cases}
$$
Since the fallback is the interpreter itself, equivalence is maintained even if optimizations fail assumptions.

> **Verification**: See `tests/cpp/jit_trace_equivalence_test.cpp`.
