# T81 Foundation: Actionable Task List

**Last Updated:** December 4, 2025

This document lists the concrete, prioritized tasks for the next development cycle, aligned with the strategic priorities in `ROADMAP.md`.

______________________________________________________________________

## How to Contribute

1.  Read the updated [`ROADMAP.md`](./ROADMAP.md) to understand the high-level goals.
2.  Pick a task from the lists below, starting with **P0**.
3.  Follow the guidelines in [`CONTRIBUTING.md`](./CONTRIBUTING.md).

______________________________________________________________________

### [P0] T81Lang Compiler (Completed)

**Goal:** The C++20 compiler now matches the `t81lang-spec.md`, emits Axion-aligned match/loop metadata, and the CLI (including the REPL) produces deterministic Axion traces. This critical path is complete; the compiler introduces no open blockers toward v1.0.

- **[EPIC] Implement Semantic Analysis & Type System:**
    - **[DONE] [M] Task:** Create the foundational `SemanticAnalyzer` class that traverses the AST.
    - **[DONE] [L] Task:** Implement the core type-checking logic within the `SemanticAnalyzer`.
    - **[DONE] [M] Task:** Implement type checking for generic types, focusing on `Option[T]` and `Result[T, E]`.
    - **[DONE] [S] Task:** Add the Option/Result end-to-end regression that now accompanies the compiler pipeline.
    - **[DONE] [S] Task:** Fix `Result[T, E]` alias in `include/t81/core/Result.hpp` to remove unused `E`.

- **[EPIC] Expand Language Feature Support:**
    - **[DONE] [L] Task:** Extend the `Parser` for `loop`/`match`.
    - **[DONE] [L] Task:** Lower match/loop expressions via guard-aware IR with Axion metadata.
    - **[DONE] [M] Task:** Persist guard metadata (variants, payloads, guard expressions) through CLI/VM/Axion traces.

- **[EPIC] Improve Developer Experience:**
    - **[DONE] [M] Task:** Implement robust error reporting with location-aware diagnostics.
    - **[DONE] [M] Task:** Ship the `t81` CLI covering `compile`, `run`, `check`, and `repl`.

With P0 closed, work shifts to the runtime-focused priorities below.

- **[EPIC] T81Tensor & Transformer Kernels Optimization (Completed):**
    - **[DONE] [M] Task:** Implement AVX2 acceleration for `TMatMul` in `include/t81/tensor/matmul.hpp`.
    - **[DONE] [M] Task:** Implement SIMD horizontal sums and loop unrolling for `TMatMul` and `TRMSNorm`.
    - **[DONE] [M] Task:** Implement fast SIMD `exp` and optimize `TRMSNorm`, `TSiLU`, and `TSoftmax` in `include/t81/tensor/llama.hpp`.
    - **[DONE] [S] Task:** Ship the "Go Broad" killer demo: complete Llama-3.2 transformer block in `examples/llama32_demo.cpp`.

______________________________________________________________________

### [P1] HanoiVM & TISC Runtime (Completed)

**Goal:** The HanoiVM now implements the full deterministic memory model, strict segment containment, and the Axion trace engine required for v1.0.

- **[EPIC] Implement the T81VM Memory Model:**
    - **[DONE] [L] Task:** Design and implement the full stack and heap memory model as defined in `spec/t81vm-spec.md`.
    - **[DONE] [M] Task:** Add VM instructions for stack manipulation (push, pop, stack pointers).

- **[EPIC] Harden the VM:**
    - **[DONE] [M] Task:** Improve VM fault handling. Ensure all illegal operations (e.g., division by zero, out-of-bounds memory access) result in deterministic, spec-compliant faults.
    - **[DONE] [L] Task:** Add extensive "negative" tests for the VM that deliberately trigger faults and verify the correct behavior.
    - **[DONE] [S] Task:** Surface match metadata hints in the Axion/trace log to confirm CLI workloads can replay guard coverage during execution.
- **[DONE] [M] Task:** Document and verify the deterministic segment-trace strings (`stack frame allocated`, `tensor slot allocated`, `AxRead/AxSet guard …`) via `axion_policy_runner` and updated Axion trace guides so policy runners can replay RFC-0020/RFC-0009 expectations.

______________________________________________________________________

### [P2] Axion Kernel & CanonFS (Completed)

**Goal:** The Axion Kernel and CanonFS are now fully functional and integrated into the runtime stack.

- **[EPIC] Implement the Axion Kernel:**
    - **[DONE] [M] Task:** Formalize the API between the HanoiVM and the Axion Kernel.
    - **[DONE] [L] Task:** Implement the first set of safety policies in the Axion Kernel (e.g., Recursion Depth Limiter, Instruction Counter).
    - **[DONE] [M] Task:** Integrate the specified Axion hooks (`AXREAD`, `AXSET`, etc.) into the VM's main dispatch loop.

- **[EPIC] General Documentation & Good First Issues:**
    - **[DONE] [S] Task:** Add more unit tests for existing data types (`T81Float`, `Tensor`).
    - **[DONE] [S] Task:** Improve Doxygen comments on public headers in `/include/t81/`.
    - **[DONE] [M] Task:** Update the `docs/tensor-guide.md` to reflect the current C++ `Tensor` API.
- **[DONE] [S] Task:** Publish the `axion_policy_runner` trace output in release docs and CI artifacts to give auditors a reachable example of the required `verdict.reason` strings.
- **[DONE] [M] Task:** Implement the persistent CanonFS driver with Axion hooks so trace regressions exercise the disk-backed store before policy predicates run.
- **[DONE] [M] Task:** Document and implement CanonFS policy hooks that intercept `AXSET`/`AXREAD` calls, emit the canonical meta/trace strings for each write, and expose those strings to policy predicates like `(require-axion-event (reason "meta slot axion event segment=meta"))` so auditors can tie CanonFS persistence to Axion enforcement.

______________________________________________________________________

### [P3] High-Tier Cognition (Tier 4) (Completed)

**Goal:** Implement Tier 4 cognitive layers focusing on self-referential modeling and high-tier cognitive loops.

- **[EPIC] Self-Referential Modeling & Reflection:**
    - **[DONE] [M] Task:** Formalize mathematical proofs ensuring that Tier 4 agents correctly model their own state transitions and internal beliefs. (Owner: Jules)
    - **[DONE] [L] Task:** Implement multi-stage `reflect -> refine` cycles in `Tier4Loop` that allow for recursive self-optimization. (Owner: HanoiVM & Axion Core)
    - **[DONE] [M] Task:** Add native TISC opcodes for triggering reflection events and accessing the `META` segment's self-model. (Owner: TISC ISA & VM)
    - **[DONE] [S] Task:** Integrate Axion policy predicates that validate the integrity of self-models before promoting processes to Tier 5. (Owner: Axion Kernel)

______________________________________________________________________

### [P4] Performance & Scaling

**Goal:** Quantify and eliminate bottlenecks to move the system toward production scale.

- **[EPIC] Numeric & I/O Optimization:**
    - **[DONE] [M] Task:** Implement Balanced Base-81 representation and SIMD kernels (add/sub/neg) for Base-81 digits.
    - **[DONE] [S] Task:** Implement Packed Base-81 blocks (5 digits -> uint32) and canonical metadata headers.
    - **[DONE] [M] Task:** Implement Base-81 Assembler/Disassembler view for TISC bytecode.
    - **[M] Task:** Optimize multi-limb `T81BigInt` arithmetic using SIMD Karatsuba techniques.
    - **[L] Task:** Optimize CanonFS for high-throughput async persistence and retrieval.
    - **[L] Task:** Implement a HanoiVM JIT compiler prototype for compute-intensive workloads.
    - **[M] Task:** Implement distributed tensor sharding for high-rank ternary tensors.

______________________________________________________________________

### [P5] Formal Verification & Hardware

**Goal:** Provide formal guarantees of correctness and explore physical ternary execution.

- **[EPIC] Verification & DSL Design:**
    - **[L] Task:** Formally verify the core balanced ternary arithmetic primitives (BigInt, Fraction).
    - **[M] Task:** Prototype ternary hardware/FPGA backends for HanoiVM.
    - **[M] Task:** Design and implement the Axion Policy DSL for declarative safety definitions.
