# T81 Foundation: Long-Horizon TODOs

This document tracks long-term strategic goals and ecosystem enhancements that extend beyond the v1.0 milestone. For immediate, actionable tasks, see [`TASKS.md`](./TASKS.md).

## Ecosystem & Tooling [P0]

- [DONE] **Python Bindings:** Initial `pybind11` wrappers for `T81Tensor`, `T729IntTensor`, `HanoiVM`, `BigInt`, `Float`, and `Fraction` implemented; expansion for full cognitive kernel support ongoing.
- [DONE] **IDE Support:** Initial VS Code extension with syntax highlighting developed in `tools/vscode-t81`.
- [DONE] **Package Management:** Basic `t81 pkg` command for project initialization and manifest management implemented.
- [DONE] **Integrated Debugger:** HanoiVM debugger (`t81 debug`) with stepping, breakpoints, and state inspection implemented.
- [DONE] **CLI Assists:** Expanded the `t81` CLI with project scaffolding (`init`), linting (`check`), `repl`, and `--benchmark`. (Code formatting tool still planned).

## High-Tier Cognition [P1]

- [DONE] **Tier 3 Reasoning:** Tier 3, 4, and 5 cognitive layers defined and integrated into the promotion engine.
- [ ] **Tier 4 Consciousness:** Research and implement Tier 4 layers focusing on self-referential modeling and high-tier cognitive loops.
    - [ ] **Deliverable: Self-Referential Modeling Proofs** - Formalize mathematical proofs ensuring that Tier 4 agents correctly model their own state transitions and internal beliefs (Owner: Jules).
    - [ ] **Deliverable: High-Tier Cognitive Loops** - Implement multi-stage `reflect -> refine` cycles in `Tier4Loop` that allow for recursive self-optimization (Owner: HanoiVM & Axion Core).
    - [ ] **Deliverable: TISC Reflection Primitives** - Add native TISC opcodes for triggering reflection events and accessing the `META` segment's self-model (Owner: TISC ISA & VM).
    - [ ] **Deliverable: Alignment Clause Verification** - Integrate Axion policy predicates that validate the integrity of self-models before promoting processes to Tier 5 (Owner: Axion Kernel).
- [DONE] **T81-AGI Alignment:** Formalized and implemented `require-alignment` policies within Axion.

## Performance Optimization [P2]

- [ ] **Numeric Bottlenecks:** Optimize multi-limb `T81BigInt` arithmetic (karatsuba, SIMD acceleration).
- [ ] **I/O Scaling:** Optimize CanonFS for high-throughput ternary data persistence and retrieval.
- [ ] **JIT Compilation:** Explore Just-In-Time compilation for HanoiVM to improve execution speed for compute-intensive workloads.
- [ ] **Large-Scale Tensors:** Implement distributed tensor operations for high-rank ternary tensors.

## Core System & Verification

- [ ] **Formal Verification:** Formally verify the core balanced ternary arithmetic primitives (BigInt, Fraction).
- [ ] **Hardware Backends:** Research and prototype physical ternary hardware backends or FPGA implementations of HanoiVM.
- [ ] **Axion Policy Language:** Develop a domain-specific language for defining Axion safety policies.
- [DONE] **Advanced CI Matrix:** Expanded CI to include static analysis, sanitizers, and formal logic verification scripts.
- [DONE] **T81Lang Determinism Gates:** Added compile-twice bytecode identity and runtime `printed_output` determinism tests with golden fixtures.
- [ ] **Cross-Arch Determinism Verification Runs:** Keep Linux `x86_64` + `arm64` T81Lang hash gate green in CI and investigate any drift immediately.

## Documentation & Community

- [DONE] **Comprehensive Tutorials:** Created end-to-end tutorials for building secure, Axion-verified applications on the T81 stack.
- [DONE] **Researcher's Guide:** Documented the mathematical foundations of the T81 cognitive tiers.
- [DONE] **Spec Alignment:** Ensured 100% bidirectional alignment between the implementation and the NewBook specifications for v1.0 milestone.
- [DONE] **Killer Demo:** Shipped deterministic Llama-3.2-1B block demo (`examples/llama32_demo.cpp`) with bit-identical output across platforms.
