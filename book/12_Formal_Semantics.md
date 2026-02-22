# Chapter 12: Formal Semantics of TISC and T81VM

## 12.1 Operational Semantics

**Status: Formalizing**

The T81 Virtual Machine (T81VM) is formally defined as a **Deterministic Finite Automaton with Infinite Memory** (DFA-IM). Its semantics are specified using **Small-Step Operational Semantics** (SSOS).

### 12.1.1 The Transition Function $\delta$

The core of the VM is the transition function $\delta$, which maps a state $S$ to a successor state $S'$:
$$
\delta: S \to S'
$$
where $S = (\mathbf{R}, \mathbf{M}, \mathbf{K}, \mathbf{\Phi}, PC)$ as defined in Chapter 3.

The execution of a program is the repeated application of $\delta$ until a terminal state is reached:
$$
S_0 \xrightarrow{\delta} S_1 \xrightarrow{\delta} \dots \xrightarrow{\delta} S_n
$$
where $S_n$ is a **Halt State** (either `STOP` or `TRAP`).

### 12.1.2 Inference Rules

We use inference rules of the form:
$$
\frac{\text{Premise}}{\text{Conclusion}}
$$

#### **Rule: Fetch-Decode**
$$
\frac{Code[PC] = \text{Op} \quad \text{Arity}(\text{Op}) = k}{S \to (PC \leftarrow PC + 1 + k, \dots)}
$$

#### **Rule: Arithmetic (Add)**
$$
\frac{Code[PC] = \text{Add } r_d, r_a, r_b \quad v_a = R[r_a] \quad v_b = R[r_b] \quad v' = v_a + v_b}{S \to S[R[r_d] \leftarrow v', \Phi \leftarrow \text{Flags}(v')]}
$$
*Invariant*: The addition $+$ is the balanced ternary addition defined in `dmath`.

#### **Rule: Policy Check (Axion)**
Before any transition $\delta$ is applied, the Axion function $\alpha$ must approve:
$$
\frac{\alpha(S, \text{Op}) = \text{Deny}}{S \to S_{\text{Trap}}(\text{SecurityFault})}
$$
$$
\frac{\alpha(S, \text{Op}) = \text{Allow}}{S \to \delta_{\text{Op}}(S)}
$$

## 12.2 Algebraic Transition Function

**Status: Theoretical**

We can also view the VM state as an element of an algebraic structure (a Ring of States). This allows us to prove properties about the execution.

Let $\mathcal{S}$ be the set of all valid machine states.
Let $\mathcal{O}$ be the set of all valid opcode sequences.
We define a monoid action $\cdot : \mathcal{O} \times \mathcal{S} \to \mathcal{S}$.

**Property: Associativity of Execution**
$$
(Op_1 \cdot Op_2) \cdot S \equiv Op_1 \cdot (Op_2 \cdot S)
$$

**Property: Determinism**
$$
\forall S \in \mathcal{S}, Op \in \mathcal{O}: \exists! S' \text{ such that } S \xrightarrow{Op} S'
$$

## 12.3 Canonicalization Rewriting System

**Status: Implemented**

The T81 compiler and runtime employ a **Rewriting System** to ensure canonical forms. This is critical for `CanonHash81` stability.

### 12.3.1 Normal Forms
A value $v$ is in **Normal Form** if it cannot be rewritten further by the reduction rules.

**Rule: Float Normalization**
$$
\text{Float}(s, m, e) \xrightarrow{} \text{Float}(s, m', e')
$$
where $m'$ has no leading zero trits (unless $m'=0$).

**Rule: Graph Topological Sort**
Given a graph $G=(V, E)$, we rewrite the node indices $V \to \{0 \dots |V|-1\}$ such that:
$$
i < j \implies \text{TopoRank}(v_i) < \text{TopoRank}(v_j)
$$
This ensures that isomorphic graphs serialize to identical byte streams.

## 12.4 Determinism Proof Sketches

### Theorem 1: Hardware Independence of `dmath`

**Claim**: For any `T81Float` inputs $x, y$ and operation $\odot \in \{+, -, *, /\}$, the result $z = x \odot y$ is bit-identical on architectures $A_1$ (x86) and $A_2$ (ARM).

**Proof Sketch**:
1.  `T81Float` is composed of `T81Int` mantissa and exponent.
2.  `T81Int` operations use only integer arithmetic (add, sub, mul, div, mod).
3.  Integer arithmetic is standardized by C++ standard (Two's Complement) and is isomorphic to the abstract integer ring $\mathbb{Z}_{2^{64}}$.
4.  Since $A_1$ and $A_2$ both implement $\mathbb{Z}_{2^{64}}$ correctly (as verified by conformance tests), and `dmath` logic is purely a composition of these integer operations, the result must be identical. $\blacksquare$

### Theorem 2: Axion Safety

**Claim**: If Policy $P$ forbids opcodes in set $\mathcal{F}$, then no state reachable from $S_0$ will have been produced by an opcode $op \in \mathcal{F}$.

**Proof Sketch**:
1.  The transition function $\delta$ is guarded: $\delta(S) = \text{if } \alpha(S, Op) \text{ then } \dots \text{ else } \text{Trap}$.
2.  The function $\alpha(S, Op)$ returns `Deny` if $Op \in \mathcal{F}$ (by definition of Policy enforcement).
3.  Therefore, the "else" branch is taken, leading to a Trap state, not a state produced by execution of $Op$.
4.  By induction, no valid state transition uses $Op$. $\blacksquare$

## 12.5 Interpreter vs Trace-JIT Equivalence

**Status: In Progress**

The Trace-JIT optimization is a semantics-preserving transformation.

Let $I(S)$ be the interpreter function.
Let $J(S)$ be the JIT-compiled function for a trace $T$.

**Requirement**:
$$
\forall S: J(S) \cong I(S)
$$
where $\cong$ denotes observational equivalence (same register values, same memory effects, same side effects).

**Mechanism**:
The JIT inserts **Guards** $G_1, \dots, G_k$.
$$
J(S) = \begin{cases}
\text{OptimizedExec}(S) & \text{if } G_1(S) \wedge \dots \wedge G_k(S) \\
\text{Deoptimize}(S) \to I(S) & \text{otherwise}
\end{cases}
$$
Since the fallback is the interpreter itself, equivalence is maintained even if optimizations fail assumptions.

> **Verification**: See `tests/cpp/jit_trace_equivalence_test.cpp`.
