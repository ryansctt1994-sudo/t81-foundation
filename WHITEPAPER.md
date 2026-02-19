# **T81 Foundation Whitepaper**

**Deterministic, Auditable Computing with Explicit Semantic Governance**

## Document Status

* **Version:** 1.2 (CI/determinism hardening + security fixes)
* **Status:** Working whitepaper – reflects current verified system behavior and recent CI/determinism hardening
* **Scope:** `t81-foundation` repository and governed runtime boundary
* **Date:** 2026-02-19

---

## Abstract

T81 Foundation is a deterministic, ternary-native computing stack designed to make compilation, execution, and runtime behavior **auditable, replayable, and governance-visible across environments**. The stack integrates a typed frontend, canonical intermediate representation, deterministic virtual machine execution, and explicit policy tracing through the Axion governance layer.

Unlike conventional systems that treat reproducibility as a best-effort property, T81 treats **determinism, traceability, and compatibility contracts as first-class engineering constraints**. Performance improvements from ternary-native kernels are treated as workload-local optimizations rather than universal claims. The central contribution of T81 is not raw throughput, but a reproducible execution discipline that survives host variation, toolchain drift, and opaque lowering stages.

**Audience.**
This document is intended for system architects, reviewers, and contributors evaluating the determinism, auditability, and verification posture of the T81 stack.

---

## 1. Problem Statement

Modern software and machine-learning pipelines routinely diverge under nominally identical inputs due to:

* host and architecture variation,
* toolchain and library drift,
* implicit runtime behavior,
* opaque or unstable lowering stages.

In high-assurance, long-horizon, or cross-environment contexts, this undermines trust in produced artifacts. Debugging, verification, and governance become post-hoc and probabilistic rather than systematic.

Most existing approaches treat reproducibility as an emergent property of tooling discipline. T81 instead treats nondeterminism as an **engineering attack surface**.

---

## 2. Design Goals and Non-Goals

### 2.1 Primary Design Goals

T81 enforces:

* deterministic semantics at language, IR, and runtime boundaries,
* canonical serialization of executable artifacts,
* policy-visible runtime traces,
* explicit compatibility contracts across repositories and versions.

### 2.2 Explicit Non-Goals

T81 does **not** claim:

* universal performance superiority over binary systems,
* automatic formal verification of all components,
* immunity to malicious hosts or compromised toolchains.

These exclusions are deliberate and documented.

---

## 3. Why Ternary Is Architecturally Relevant

Ternary representation in T81 is not adopted for novelty or speculative advantage. It is used where it **reduces semantic ambiguity and instruction-level nondeterminism** under explicit constraints.

Key motivations:

1. **Semantic Compression Without Implicit Heuristics**
   Balanced ternary `{-1, 0, +1}` enables arithmetic and control primitives (negation, sparsity, gating) that would otherwise rely on implicit binary heuristics or floating-point behavior.

2. **Deterministic Sparse Semantics**
   Zero-states are first-class values rather than emergent patterns, enabling explicit zero-skip and gating behavior that is observable and traceable at runtime.

3. **Canonical Numeric Behavior**
   Ternary-native kernels avoid several sources of floating-point nondeterminism (rounding modes, fused operations, platform-specific lowering) by design.

Binary representations remain supported and dominant for many workloads. Ternary is introduced **selectively**, where it improves determinism or auditability under measurement.

---

## 4. Architecture Overview

T81 is organized as a vertical, explicitly governed stack:

1. **Core Numerics**
   (`include/t81/core`, `src/`)
   Deterministic numeric primitives, canonical formatting, and serialization.

2. **Frontend (T81Lang)**
   (`include/t81/frontend`, `src/frontend`)
   Lexer, parser, semantic analysis, and IR generation with deterministic lowering rules.

3. **TISC + Virtual Machine**
   (`include/t81/tisc`, `src/tisc`, `src/vm`)
   Canonical program format and deterministic execution model.

4. **Axion Governance + CanonFS**
   (`src/axion`, `src/canonfs`, `include/t81/canonfs`)
   Policy enforcement, trace visibility, and canonical persistence surfaces.

5. **CLI and Tooling**
   (`src/cli`, `scripts/ci`, `docs/guides`)
   Compile/run/debug workflows and reproducibility enforcement.

Each layer exposes explicit boundaries and contracts; no layer assumes implicit behavior from the next.

---

## 5. Determinism and Security Model

### 5.1 Determinism Contract

For supported source, build, and runtime matrices, T81 targets:

* bit-stable compilation outputs,
* deterministic runtime execution for identical inputs and initial state,
* reproducible trace surfaces suitable for replay and audit.

Mechanisms include:

* canonical IR and binary emission,
* deterministic VM instruction semantics,
* reproducibility gates in CI (AST/IR hashes),
* cross-architecture hash comparison,
* explicit semantic/runtime contract synchronization.

### 5.2 Security Posture & Recent Hardening

**Threats addressed directly:**
* **Environmental drift:** Toolchain, host, and library variations are mitigated via strict reproducible builds and CI gates.
* **Silent semantic drift:** Opaque lowering stages are verified against canonical hashes (`tests/fixtures/t81lang_determinism/`).
* **Input validation:** Package initialization now strictly sanitizes names to prevent S-expression injection vulnerabilities (fixed in `src/cli/driver.cpp`).
* **Crypto correctness:** SHA3-512 implementation verified against test vectors, correcting previous Chi step logic.

**Threats explicitly out of scope:**
* Malicious hosts or compilers.
* Adversarial runtime tampering (though traces aid detection).
* Trust bootstrapping (T81 assumes a trusted initial compile).

T81 guarantees apply to *cooperative but fallible environments* where reproducibility and auditability are required.

---

## 6. Verification and Reproducibility Methodology

### 6.1 Local Baseline Ritual

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### 6.2 Test Inventory Snapshot

At the time of this document:

* **Total tests:** 214
  (`ctest --test-dir build -N`)

### 6.3 CI Enforcement Gates

Representative gates include:

* **Reproducibility Gates:** `scripts/ci/t81lang_repro_gate.py` ensures AST and IR structures match canonical hashes.
* **Format & Linting:** Strict `clang-format-18` and static analysis enforcement preventing style drift.
* **Runtime Contract Sync:** `.github/workflows/runtime-contract.yml` monitors drift between specification and runtime.
* **Security Checks:** Automated dependency auditing and permission checks for workflows.

These gates are normative; failure blocks merge.

---

## 7. Formalism Boundary

T81 distinguishes between **formal specification** and **engineering enforcement**:

* **Formally specified:**
  Language semantics, IR encoding rules, runtime instruction behavior, and contract schemas.

* **Engineering-enforced:**
  Compiler implementation, VM execution engine, CI workflows, and trace infrastructure.

Formal methods are applied where tractable and valuable. Elsewhere, reproducibility gates and trace replay provide practical verification without overclaiming formal proof.

---

## 8. Runtime Boundary Contract

Semantic ownership and runtime compatibility are separated:

* **`t81-foundation`:**
  Semantic source of truth and specification.

* **`t81-vm`:**
  Runtime compatibility artifacts and host ABI lanes.

Drift detection is enforced via:

* contract marker: `contracts/runtime-contract.json`,
* CI validation: `.github/workflows/runtime-contract.yml`.

This separation prevents silent divergence while allowing independent hardening.

---

## 9. Performance Posture

Performance is treated as **workload-dependent** and subordinate to deterministic correctness.

Current optimization focus includes:
* BigInt hot paths,
* Ternary tensor kernels,
* CanonFS throughput (persistent vs in-memory),
* Deterministic runtime hardening.

All performance claims are accompanied by benchmark artifacts and commit-pinned snapshots.

### 9.1 Benchmark Snapshot

Source: `docs/benchmarks.md`

* Last updated: `2026-02-17 16:05:09 UTC`
* Snapshot commit: `92d6280`

| Benchmark        |           T81 |   T81 Native |        Binary |  Ratio | Interpretation            |
| ---------------- | ------------: | -----------: | ------------: | -----: | ------------------------- |
| BM_NegationSpeed |   1.74 Gops/s |  1.92 Gops/s |   4.80 Gops/s |  0.36× | Binary scalar win         |
| BM_Llama_RMSNorm | 567.56 Mops/s |          n/a | 602.17 Mops/s |  0.94× | Near parity               |
| BM_Llama_SiLU    | 322.14 Mops/s |          n/a | 329.33 Mops/s |  0.98× | Near parity               |
| BM_Llama_Softmax | 186.09 Mops/s |          n/a | 181.44 Mops/s |  1.03× | T81 slight lead           |
| BM_Llama_Block   |   6.94 Mops/s |          n/a | 486.97 Kops/s | 14.26× | **Major T81 advantage**   |
| BM_CanonFS_Read_P|    27.70 GB/s |          n/a |    3.32 GB/s  |  8.35× | **Major T81 advantage**   |
| BM_Add_4096_bit  |  79.76 Kops/s |          n/a |   5.79 Mops/s |  0.01× | Binary dominates (SIMD)   |
| BM_Overflow_Check|   1.95 Gops/s |          n/a | 646.95 Mops/s |  3.01× | **T81 safety advantage**  |

*Note: Ratios are T81/Binary. Values > 1.0 indicate T81 advantage.*

---

## 10. Known Limitations

* Performance is uneven across workloads (e.g., binary SIMD still dominates large integer add).
* Several advanced features remain spec-driven and require further hardening.
* Cross-repo contracts introduce process overhead by design.

These limitations are tracked and intentionally visible.

---

## 11. Roadmap (Near-Term)

1. **Broaden Reproducibility:** Expand coverage across more host/architecture matrices and tighten `t81lang` AST/IR determinism checks.
2. **Deepen Verification:** Increase property-based testing and fuzzing at compiler/runtime boundaries.
3. **Optimize Kernels:** Continue kernel optimization (especially tensor ops) without weakening determinism guarantees.
4. **Governance UI:** Improve tooling for visualizing Axion policies and traces.

---

## 12. Claims vs Evidence Matrix

| Claim                                        | Evidence                                | Strength | Notes                      |
| -------------------------------------------- | --------------------------------------- | -------- | -------------------------- |
| Deterministic compile/runtime is operational | CI workflows, repro gates, hash checks  | Strong   | Continuously enforced      |
| Runtime boundary is governed                 | Contracts + sync workflows              | Strong   | Drift detection active     |
| Security hardening active                    | Package init sanitization, SHA3 fixes   | Strong   | Recent patches verified    |
| Performance is workload-dependent            | `docs/benchmarks.md`                    | Strong   | Both wins and losses shown |
| Universal production readiness               | N/A                                     | Weak     | Not claimed                |

---

## 13. Conclusion

T81 Foundation applies a governance-driven determinism model end-to-end: language semantics, executable artifacts, runtime traces, and cross-repository compatibility are treated as auditable, enforceable surfaces. The system is operational and actively hardened, prioritizing replayability and verification discipline over speculative performance claims.

Its contribution is not a faster machine, but a **more accountable one**.

---
