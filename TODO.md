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
- [ ] **Advanced CI Matrix:** Expand CI to include formal proof checking and extensive fuzzing of the entire stack.

## Documentation & Community

- [ ] **Comprehensive Tutorials:** Create end-to-end tutorials for building applications on the T81 stack.
- [ ] **Researcher's Guide:** Document the mathematical foundations of the T81 cognitive layers.
- [DONE] **Spec Alignment:** Ensured 100% bidirectional alignment between the implementation and the NewBook specifications for v1.0 milestone.
- [DONE] **Killer Demo:** Shipped deterministic Llama-3.2-1B block demo (`examples/llama32_demo.cpp`) with bit-identical output across platforms.
