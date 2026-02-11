# **T81 Foundation Whitepaper**

**Deterministic, Auditable Computing with Explicit Semantic Governance**

## Document Status

* **Version:** 1.1 (architectural + governance revision)
* **Status:** Working whitepaper; reflects current verified system behavior
* **Scope:** `t81-foundation` repository and governed runtime boundary
* **Date:** 2026-02-11

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
* reproducibility gates in CI,
* cross-architecture hash comparison,
* explicit semantic/runtime contract synchronization.

### 5.2 Threat Model and Scope

**Threats addressed directly:**

* environmental drift (toolchain, host, library),
* silent semantic drift during lowering,
* opaque runtime behavior without traceability.

**Threats partially mitigated but not eliminated:**

* accidental misconfiguration,
* unintended runtime divergence.

**Threats explicitly out of scope:**

* malicious hosts or compilers,
* adversarial runtime tampering,
* cryptographic integrity or trust bootstrapping.

T81 does not claim adversarial security. Its guarantees apply to *cooperative but fallible environments* where reproducibility and auditability are required.

---

## 6. Verification and Reproducibility Methodology

### 6.1 Local Baseline Ritual

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### 6.2 Test Inventory Snapshot

At the time of this document:

* **Total tests:** 156
  (`ctest --test-dir build -N`)

### 6.3 CI Enforcement Gates

Representative gates include:

* T81Lang reproducibility gate: `scripts/ci/t81lang_repro_gate.py`
* T3_K reproducibility gate: `scripts/ci/t3k_repro_gate.py`
* Runtime contract sync: `.github/workflows/runtime-contract.yml`
* Local reproduction guide: `docs/ci.md`

These gates are treated as normative, not advisory.

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
* ternary tensor kernels,
* CanonFS throughput,
* deterministic runtime hardening.

All performance claims are accompanied by benchmark artifacts and commit-pinned snapshots.

### 9.1 Benchmark Snapshot

Source: `docs/benchmarks.md`

* Last updated: `2026-02-09 20:06:23 UTC`
* Snapshot commit: `d3dc39b`

| Benchmark        |           T81 |   T81 Native |        Binary |  Ratio | Interpretation            |
| ---------------- | ------------: | -----------: | ------------: | -----: | ------------------------- |
| BM_NegationSpeed |  24.96 Gops/s | 40.93 Gops/s |   1.40 Gops/s | 29.18× | Strong ternary-native win |
| BM_Llama_RMSNorm |   1.82 Gops/s |          n/a |  57.56 Mops/s | 31.63× | Workload-local win        |
| BM_Llama_SiLU    |   1.23 Gops/s |          n/a | 169.45 Mops/s |  7.27× | Notable improvement       |
| BM_Llama_Softmax | 899.52 Mops/s |          n/a | 127.57 Mops/s |  7.05× | Notable improvement       |
| BM_Add_4096_bit  |  95.82 Kops/s |          n/a | 997.68 Mops/s |  0.00× | Binary dominates          |
| BM_CanonFS_Write |    13.17 MB/s |          n/a |   467.79 MB/s |  0.03× | Known hotspot             |

---

## 10. Known Limitations

* Performance is uneven across workloads.
* Several advanced features remain spec-driven and require further hardening.
* Cross-repo contracts introduce process overhead by design.

These limitations are tracked and intentionally visible.

---

## 11. Roadmap (Near-Term)

1. Expand reproducibility coverage across broader host and architecture matrices.
2. Increase property-based and fuzz testing at compiler/runtime boundaries.
3. Continue kernel optimization without weakening determinism guarantees.
4. Tighten spec-to-implementation traceability across documentation and CI.

---

## 12. Claims vs Evidence Matrix

| Claim                                        | Evidence                                | Strength | Notes                      |
| -------------------------------------------- | --------------------------------------- | -------- | -------------------------- |
| Deterministic compile/runtime is operational | CI workflows, repro gates, `docs/ci.md` | Strong   | Continuously enforced      |
| Runtime boundary is governed                 | Contracts + sync workflows              | Strong   | Drift detection active     |
| Performance is workload-dependent            | `docs/benchmarks.md`                    | Strong   | Both wins and losses shown |
| Universal production readiness               | N/A                                     | Weak     | Not claimed                |

---

## 13. Conclusion

T81 Foundation applies a governance-driven determinism model end-to-end: language semantics, executable artifacts, runtime traces, and cross-repository compatibility are treated as auditable, enforceable surfaces. The system is operational and actively hardened, prioritizing replayability and verification discipline over speculative performance claims.

Its contribution is not a faster machine, but a **more accountable one**.

---
