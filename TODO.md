# T81 Foundation: Long-Horizon TODOs

**Last Updated:** February 10, 2026

This file tracks strategic items beyond the near-term execution queue in `TASKS.md`.

## 1. Formal Methods and Proofs

- [ ] Formal verification for core balanced ternary arithmetic primitives (`T81Int`, `T81BigInt`, `T81Fraction`).
- [ ] Proof-oriented validation for key Axion policy invariants.
- [ ] Expand deterministic replay proofs across compiler + VM boundaries.

## 2. Performance Architecture

- [ ] Deep optimization of `T81BigInt` (algorithmic + SIMD layers).
- [ ] Expand tensor backend strategy (portable scalar parity + optimized backends).
- [ ] CanonFS scalability for larger persistent workloads with deterministic observability.

## 3. Runtime and Execution

- [ ] Incremental deterministic trace-JIT hardening for numeric/tensor hot paths.
- [ ] Evaluate distributed tensor orchestration patterns that preserve replay guarantees.

## 4. Ecosystem and Interop

- [ ] Expand language bindings and integration surfaces where determinism guarantees can be preserved.
- [ ] Strengthen downstream runtime boundary tooling (`t81-foundation` <-> `t81-vm` <-> examples).

## 5. Hardware and Future Targets

- [ ] Investigate FPGA/hardware-backed ternary execution pathways.
- [ ] Define pragmatic hardware abstraction boundaries without weakening canonical semantics.
