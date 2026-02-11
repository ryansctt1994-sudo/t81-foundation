# T81 Foundation Whitepaper

## Document Status
- Version: 1.0 (working)
- Status: Current architecture/verification snapshot
- Scope: `t81-foundation` repository
- Date: 2026-02-11

## Abstract
T81 Foundation is a deterministic, ternary-native computing stack designed to make compilation and execution auditable across environments. The stack combines a typed frontend, canonical intermediate representation, deterministic virtual machine behavior, and Axion policy visibility. The central claim is not only performance from ternary-native kernels, but reproducibility and traceability as first-class engineering constraints.

## 1. Problem Statement
Modern software and ML pipelines routinely drift under nominally identical inputs due to host variation, toolchain versioning, implicit runtime behavior, and opaque lowering stages. In high-assurance domains, this undermines trust in artifacts and complicates verification.

T81 addresses this by enforcing:
- deterministic semantics at language and runtime boundaries,
- canonical serialization for executable artifacts,
- policy-visible runtime traces,
- explicit runtime compatibility contracts across repositories.

## 2. Architecture Overview
T81 is organized as a vertical stack:
1. **Core Numerics** (`include/t81/core`, `src/`): deterministic numeric primitives and canonical formatting surfaces.
2. **Frontend** (`include/t81/frontend`, `src/frontend`): lexer, parser, semantic analyzer, IR generator for T81Lang.
3. **TISC + VM** (`include/t81/tisc`, `src/tisc`, `src/vm`): canonical program format + deterministic VM execution model.
4. **Axion + CanonFS** (`src/axion`, `src/canonfs`, `include/t81/canonfs`): policy/tracing surfaces + canonical persistence abstractions.
5. **CLI + Tooling** (`src/cli`, `scripts/ci`, `docs/guides`): compile/run/disasm/debug/repro workflows and CI enforcement gates.

## 3. Determinism and Security Model
### 3.1 Determinism Contract
T81 targets bit-stable outputs for supported source/build matrices and deterministic runtime behavior for identical inputs and initial state.

Key mechanisms:
- canonical IR/binary emission,
- deterministic runtime trace surfaces,
- CI reproducibility gates and cross-arch hash comparison,
- explicit contract synchronization between semantic and runtime repos.

### 3.2 Threat Model
Primary threats considered:
- environmental drift (toolchain/host/library changes),
- silent semantic drift during lowering,
- opaque runtime behavior that cannot be audited post hoc.

T81 treats these as engineering attack surfaces and uses policy gates, deterministic traces, and compatibility contracts to reduce ambiguity.

## 4. Verification and Reproducibility Methodology
### 4.1 Local Baseline Ritual
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### 4.2 Current Test Inventory Snapshot
At the time of this document, the local configured test inventory reports:
- **Total tests: 156** (`ctest --test-dir build -N`)

### 4.3 CI Gates (Representative)
- T81Lang reproducibility gate: `scripts/ci/t81lang_repro_gate.py`
- T3_K reproducibility gate: `scripts/ci/t3k_repro_gate.py`
- Runtime contract sync workflow: `.github/workflows/runtime-contract.yml`
- Local CI reproduction reference: `docs/ci.md`

## 5. Runtime Boundary Contract
T81 separates semantic ownership from runtime compatibility ownership:
- `t81-foundation`: semantic and specification source-of-truth behavior.
- `t81-vm`: runtime compatibility artifacts and host ABI lane.

Boundary pinning and drift detection:
- contract marker: `contracts/runtime-contract.json`
- workflow enforcement: `.github/workflows/runtime-contract.yml`

## 6. Performance Posture
T81 prioritizes deterministic correctness first, with targeted performance work on hot kernels.

Current emphasis areas:
- BigInt hot paths,
- tensor and SIMD kernels,
- CanonFS throughput paths,
- deterministic JIT/runtime hardening.

Performance claims should always be interpreted with workload context and accompanied by benchmark artifacts (`docs/benchmarks.md` and CI outputs).

### 6.1 Benchmark Snapshot (Repository Artifact)
Source: `docs/benchmarks.md`  
Snapshot metadata in that file:
- Last Updated: `2026-02-09 20:06:23 UTC`
- Snapshot Commit: `d3dc39b`

Representative rows:

| Benchmark | T81 | T81 Native | Binary | Ratio (T81/Binary) | Interpretation |
|---|---:|---:|---:|---:|---|
| `BM_NegationSpeed` | 24.96 Gops/s | 40.93 Gops/s | 1.40 Gops/s | 29.18x | Strong ternary-native win on negation path |
| `BM_Llama_RMSNorm` | 1.82 Gops/s | n/a | 57.56 Mops/s | 31.63x | Strong workload-local win |
| `BM_Llama_SiLU` | 1.23 Gops/s | n/a | 169.45 Mops/s | 7.27x | Notable win |
| `BM_Llama_Softmax` | 899.52 Mops/s | n/a | 127.57 Mops/s | 7.05x | Notable win |
| `BM_Add_4096_bit` | 95.82 Kops/s | n/a | 997.68 Mops/s | 0.00x | Binary baseline still dominates this path |
| `BM_CanonFS_WriteThroughput_Persistent` | 13.17 MB/s | n/a | 467.79 MB/s | 0.03x | CanonFS persistent write path remains a hotspot |

## 7. Known Limitations
- Workload performance is uneven; some ternary-native kernels are strong while others remain optimization targets.
- Several advanced features remain draft/spec-driven and require further implementation hardening.
- Cross-repo contract synchronization adds process overhead, but is retained to preserve compatibility clarity.

## 8. Roadmap (Near-Term)
1. Increase reproducibility depth across wider architecture/build matrices.
2. Expand property/fuzz testing for compiler/runtime boundaries.
3. Continue kernel-level throughput improvements without weakening determinism guarantees.
4. Tighten spec-to-implementation traceability in docs and CI artifacts.

## 9. Claims vs Evidence Matrix
| Claim | Evidence Source | Current Strength | Notes |
|---|---|---|---|
| Deterministic compile/runtime posture is operational | `docs/ci.md`, `.github/workflows/ci.yml`, repro gates (`scripts/ci/t81lang_repro_gate.py`, `scripts/ci/t3k_repro_gate.py`) | Strong | Enforced continuously in CI |
| Runtime boundary is explicitly governed | `contracts/runtime-contract.json`, `.github/workflows/runtime-contract.yml`, `docs/runtime-semantics-boundary.md` | Strong | Cross-repo drift checks in place |
| Performance is workload-dependent, not universally superior | `docs/benchmarks.md` | Strong | Contains both large wins and lagging paths |
| Stack is production-hardened for all workloads | N/A | Weak | Not claimed; ongoing hardening phase |

## 10. Workflow Evidence Appendix
Primary workflow evidence references:
- `.github/workflows/ci.yml`  
  - build + `ctest` execution  
  - T3_K reproducibility gate  
  - T81Lang reproducibility gate  
  - cross-arch hash comparisons
- `.github/workflows/repro-ledger.yml`  
  - periodic reproducibility bundle generation  
  - benchmark + trace artifacts
- `.github/workflows/runtime-contract.yml`  
  - runtime boundary marker and compatibility sync validation
- `.github/workflows/bench.yml`  
  - benchmark runner execution and benchmark artifact updates

Supporting local/author docs:
- `docs/ci.md`
- `docs/guides/repro-ledger.md`
- `docs/runtime-semantics-boundary.md`

## 11. Conclusion
T81’s differentiator is a deterministic governance model applied end-to-end: language semantics, executable artifacts, runtime traces, and inter-repo compatibility contracts are all treated as auditable surfaces. The stack is operational and in active hardening for performance and scale while preserving replayability and verification discipline.
