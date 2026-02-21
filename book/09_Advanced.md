# Chapter 9: Advanced Topics

## 9.1 Cognitive Tiers

The **Cognitive Tier Model** (`spec/cognitive-tiers.md`) is a unique feature of T81. It categorizes code based on its structural complexity and risk.

### Tier 0: Ground State
*   **Status**: Loading & Validation.
*   **Activity**: Type checking, canonicalization. No code execution.

### Tier 1: Pure Deterministic Computation
*   **Characteristics**: Simple arithmetic, pure functions, flat control flow.
*   **Constraints**: No deep recursion (depth $\le$ 1). No tensor operations > rank 1.
*   **Use Case**: Financial calculations, simple logic.

### Tier 2: Structured Algorithms
*   **Characteristics**: Loops, basic branching, shallow recursion.
*   **Constraints**: Recursion depth $\le$ 10. Matrices $\le$ 81x81.
*   **Use Case**: Sorting, basic data processing.

### Tier 3: Recursive / Symbolic Reasoning
*   **Characteristics**: Deep recursion, graph traversal, symbolic manipulation.
*   **Constraints**: Recursion $\le$ 81. Tensor rank $\le$ 5.
*   **Use Case**: Compilers, pathfinding, complex parsing.

### Tier 4: Analytic Reasoning
*   **Characteristics**: Advanced symbolic reasoning, multi-layer recursive structures.
*   **Constraints**: Recursion $\le$ 243.
*   **Use Case**: Theorem provers, advanced AI.

### Tier 5: Cognitive Metareasoning
*   **Characteristics**: Self-reflection, modifying own code/policy.
*   **Constraints**: Strictly supervised by Axion.
*   **Use Case**: The Axion Kernel itself, AGI research.

Axion monitors execution. If a Tier 1 program attempts deep recursion, Axion triggers a **Tier Fault**. A program can request "promotion" to a higher tier, which Axion may grant or deny based on policy.

## 9.2 Trace-JIT (Experimental)

The T81 JIT (`src/vm/jit_compiler.cpp`) is not a standard JIT. Standard JITs optimize for speed, often reordering operations or changing floating-point precision. The T81 JIT is a **Trace JIT** that optimizes for speed *while proving behavioral equivalence*.

*   It records a linear trace of instructions.
*   It compiles that trace to native code.
*   It inserts "guard" checks. If execution diverges from the trace (e.g., a branch goes a different way), it falls back to the interpreter immediately.
*   This ensures that the JIT never produces a result different from the interpreter.

## 9.3 Distributed Tensors

The `src/cog/tier4/distributed.cpp` module implements experimental support for tensors that are split across multiple T81VM instances. This allows for computations larger than the memory of a single machine, coordinated via deterministic message passing.
