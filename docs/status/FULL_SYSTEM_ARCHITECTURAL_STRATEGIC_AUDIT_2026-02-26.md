# Full-System Architectural & Strategic Audit

Date: 2026-02-26  
Scope: `/src`, `/include`, `/spec`, `/docs`, `/book`, CI workflows, governance, contracts, ISA/opcodes, VM, Axion, determinism gates, benchmarks, tests, multilingual alignment, roadmap, release artifacts.

## Executive Summary
This repository is a substantial, working deterministic-runtime codebase with strong test/CI investment, but it is not architecturally or governance-coherent enough to treat as production-grade deterministic infrastructure. Core pipeline elements are implemented (frontend, ISA encoding, VM interpreter, Axion engine, CanonFS, reproducibility gates), and representative determinism tests plus both repro gates were validated locally (`t81lang` and `t3k`) as passing.

The highest risks are specification/implementation divergence (including register model and encoding model conflicts), over-claiming in several public docs/translations, partial/stubbed ISA surfaces inside a frozen narrative, and governance enforcement drift (policy matrix references missing checks/scripts).

Determinism is defensible only for explicitly bounded surfaces and only with caveats already present in some governance docs. The strategic position is best classified as a **Deterministic Runtime Candidate** (not pre-production infrastructure). If development stopped today, it would be remembered as a serious deterministic-systems research platform with unusually strong implementation depth, but inconsistent assurance posture.

## 1. Architectural Integrity

### 1.1 Drift Matrix (Spec vs Implementation)

| Area | Spec/Doc Claim | Implementation Evidence | Assessment |
|---|---|---|---|
| Register model | TISC defines 81 registers (`spec/tisc-spec.md`) | VM thread context stores 243 registers (`include/t81/vm/state.hpp`) | High drift |
| Instruction encoding | TISC spec describes fixed 81-trit word (`spec/tisc-spec.md`) | Runtime encodes 13-byte `{opcode,a,b,c}` (`core/isa/encoding.cpp`); opcode registry also says 13-byte (`spec/tisc/opcode-registry.md`) | Spec internal inconsistency |
| JIT model | VM spec describes deterministic native-code JIT intent (`spec/t81vm-spec.md`) | Capability contract describes threaded trace interpreter (not machine-code JIT) (`docs/reference/CAPABILITY_CONTRACT.md`) | Medium-high drift |
| Axion policy invocation | Per-instruction policy expectation for governed execution | Interpreter does per-step checks; trace path checks entry/exit boundaries (`core/vm/vm.cpp`) | Known bounded divergence |
| Core maturity | Root status shows T81VM Beta / Axion Alpha (`README.md`) | Reference status shows Stable T81Lang/HanoiVM/Axion (`docs/reference/STATUS.md`) | Governance/document drift |

### 1.2 Layer Violation Analysis
- Pipeline layering exists in code: frontend -> emitter -> VM -> Axion hooks.
- Authority layering is inconsistent across spec/status/reference artifacts.
- Experimental/stub opcode surfaces are present in core VM dispatch under the same ISA namespace.
- Legacy/archival naming persists in roadmap/reference documents.

### 1.3 Architectural Risk Score

| Metric | Score (1-10) | Basis |
|---|---:|---|
| Architectural Risk | 8 | Normative/implemented model divergence plus governance inconsistency |

## 2. Determinism Validation

### 2.1 Determinism Confidence Rating
**Moderate** for scoped verified surfaces, **Low** for broad ecosystem claims.

### 2.2 Failure Surface Analysis
- VM float opcode path uses host `std::*` trig/log/exp/pow in execution dispatch (`core/vm/vm.cpp`).
- Determinism registry explicitly excludes/limits several surfaces (`docs/governance/DETERMINISM_SURFACE_REGISTRY.md`).
- Entropy/time APIs include non-deterministic sources unless overridden (`include/t81/entropy.hpp`, `include/t81/types/T81Time.hpp`).
- JIT policy checks are boundary-based (entry/exit), not per-instruction inside trace.

### 2.3 Determinism Threat Map

| Threat | Severity | Likelihood | Current Control |
|---|---|---|---|
| Host libm drift in float ops | High | Medium | Scoped as non-guaranteed in capability/registry docs |
| Overclaim in top-level docs | High | High | Some governance docs bound claims; messaging still inconsistent |
| JIT equivalence/policy drift | Medium | Medium | Equivalence tests exist; model remains experimental |
| CanonFS read integrity trust | Medium | Medium | Write hashing enforced; read-time re-verify absent |
| Spec/encoding mismatch | High | Medium | No single harmonized canonical encoding narrative |

### 2.4 Validation Snapshot (Local)
- `ctest` selected tests passed: `t81_vm_determinism_property_test`, `jit_trace_equivalence_test`, `axion_policy_segment_event_test`.
- `scripts/ci/t81lang_repro_gate.py` passed (16 fixtures).
- `scripts/ci/t3k_repro_gate.py` passed.

## 3. Instruction Set Coherence (TISC)

### 3.1 Findings
- Opcode registry is extensive, mapped, and has matrix tests.
- Freeze narrative is weakened by explicitly stub/placeholder opcode behavior in VM dispatch.
- Encoding contract inconsistency (81-trit narrative vs 13-byte implementation) remains unresolved.

### 3.2 ISA Maturity Stage
**Stabilizing**

### 3.3 Recommended Next Action
Split ISA posture into:
1. Frozen core profile
2. Extension/stub profile

Then reconcile encoding/register semantics into one normative source.

## 4. VM & Execution Engine

### 4.1 Findings
- Interpreter is substantial and policy-aware.
- Deterministic round-robin multi-context scheduling exists.
- JIT trace execution is real and tested, but not native machine-code JIT and not full per-instruction policy inside trace bodies.

### 4.2 Runtime Ratings

| Metric | Score | Interpretation |
|---|---:|---|
| Runtime Integrity | 6.8/10 | Strong implementation, bounded by model mismatches |
| Production Readiness | Not ready | Candidate-stage, not hardened infrastructure |

## 5. Axion Governance & Enforcement

### 5.1 Findings
- Axion policy engine and bytecode evaluator are implemented.
- Unknown policy clauses are skipped deterministically (not hard-failed) in parser.
- If embedded policy parse fails during load, VM path can continue without fail-closed semantics.

### 5.2 Governance Rating

| Metric | Rating |
|---|---|
| Governance Strength | Moderate |
| Risk Classification | Medium-High |

### 5.3 Missing Enforcement Surfaces
- Stronger gating around stub privileged/cognitive opcode surfaces.
- Full sync between governance matrix and actual CI-enforced checks.

## 6. Documentation vs Reality

### 6.1 Overstatement Map
- Root and multilingual READMEs are substantially aligned to bounded determinism language, but additional secondary docs still contain broad sandbox/determinism wording not yet fully normalized.
- Legacy documents still contain sandbox phrasing that can be read as stronger than current process-level enforcement model.

### 6.2 Documentation Credibility Score

| Metric | Score (1-10) |
|---|---:|
| Documentation Credibility | 6.1 |

### 6.3 Required Corrections
1. Resolve register and encoding contradictions in normative docs.
2. Normalize secondary docs (how-to/spec aggregations) to current bounded determinism and process-level isolation language.
3. Continue Axion enforcement hardening around stub privileged/cognitive opcode surfaces.

## 7. Code Quality & Engineering Discipline

### 7.1 Findings
- Build/test matrix is broad and active; local CTest inventory shows 254 tests.
- Static analysis scope in CI is narrowed to `src/**` compile units, omitting large implementation domains.
- Governance enforcement matrix references many missing scripts/checks.
- Workflow pinning policy fails under strict thresholds due to remaining tag-based action refs.

### 7.2 Maturity & Refactor Priority

| Dimension | Assessment |
|---|---|
| Engineering Maturity | Emerging System |

Refactor priority ranking:
1. Spec/runtime contract convergence (registers + encoding + JIT model).
2. Documentation governance unification (single claim model + multilingual sync).
3. Axion fail-closed behavior and policy versioning.
4. CI governance hardening (pinning + matrix-to-enforcement parity).
5. Determinism hardening/partitioning for host-dependent float/tensor behavior.

## 8. Strategic Position Assessment

### 8.1 Classification
**Deterministic Runtime Candidate**

### 8.2 Why
- Real implementation depth and strong automated validation.
- Determinism controls are operational on important slices.
- Unresolved spec contradictions, stubs, and claim drift block pre-production classification.

### 8.3 If Development Stopped Today
A technically serious deterministic-compute research/runtime candidate with strong artifacts and tests, remembered for both depth and unresolved assurance-coherence gaps.

## 9. Hard Truth

### 9.1 Most Serious Structural Risks (Top 5)
1. Register-model mismatch (81-reg spec vs 243-reg runtime).
2. Encoding-model contradiction (81-trit vs 13-byte).
3. Public overclaim risk (global determinism wording vs bounded guarantees).
4. Governance enforcement debt (policy matrix references missing checks/scripts).
5. Security/governance ambiguity from conflicting capability-contract statements.

### 9.2 Most Valuable Strengths (Top 5)
1. High-volume automated test infrastructure.
2. Working end-to-end toolchain and runtime (not paper architecture).
3. Operational reproducibility gates (`t81lang`, `t3k`).
4. Explicit determinism threat/registry framework.
5. Axion policy + trace substrate with meaningful audit hooks.

### 9.3 Single Most Important Next Move
Execute a **Normative Convergence Sprint** that produces one signed authoritative contract set for:
- ISA encoding
- register model
- determinism scope
- Axion enforcement semantics

Then make CI fail on any doc/status/translation drift from that contract.

## 10. Remediation Log

### 2026-02-26 (R1)
- Updated root determinism positioning text in `README.md` to registry-bounded guarantee language.
- Synchronized `docs/reference/STATUS.md` component maturity posture with active status/governance artifacts (`T81VM` Beta, `Axion` Alpha, `T81Lang` Beta implementation posture).
- Added this remediation log section to keep audit findings and closure evidence linked in one artifact.

### 2026-02-26 (R2)
- Synchronized multilingual README component rows (ES/PT/RU/ZH) so `T81VM` and `Axion` maturity no longer conflict with canonical posture.
- Updated `docs/governance/ENFORCEMENT_MATRIX.md` to reflect actually wired CI checks and to explicitly mark planned/non-implemented enforcement scripts as non-machine-verifiable.

### 2026-02-26 (R3)
- Reconciled `spec/tisc-spec.md` encoding language with canonical implemented interchange format (`u8 opcode + 3x i32 operands`, 13-byte fixed width).
- Added explicit register-contract clarification in `spec/tisc-spec.md`: `R0–R80` as the mandatory architectural window, with non-portable implementation-defined extension banks allowed only outside canonical deterministic-profile assumptions.

### 2026-02-26 (R4)
- Removed remaining tag-pinned workflow actions (`@v4`) and replaced them with SHA-pinned references in:
  - `.github/workflows/benchmark_packed_trit_vector.yml`
  - `.github/workflows/ci.yml` (`governance-metrics` job)
- This closes the workflow action pinning gap identified by `scripts/ci/audit_workflow_actions.py --max-tagged 0 --max-unknown 0`.

### 2026-02-26 (R5)
- Hardened VM Axion policy handling to fail closed on embedded policy parse errors in `core/vm/vm.cpp`.
- Added regression coverage in `tests/cpp/vm_policy_parse_fail_closed_test.cpp` and wired it into `CMakeLists.txt`.

### 2026-02-26 (R6)
- Updated deterministic-claim language in `README.pt-BR.md`, `README.ru.md`, and `README.zh-CN.md` intros and deterministic-execution feature rows to match bounded verified-surface positioning.
- Re-ran governance and integrity checks (`status label coherence`, `docs governance hygiene`, `TISC freeze integrity`, and strict workflow action pinning) with all checks passing.

### 2026-02-26 (R7)
- Reconciled capability-contract sandbox language by updating `docs/governance/CAPABILITY_CONTRACT.md` to match process-level enforcement limits and explicit non-guarantee of OS/hardware sandboxing.
- Added explicit contract-authority pointer from governance summary to `docs/reference/CAPABILITY_CONTRACT.md`.

### 2026-02-26 (R8)
- Normalized over-strong determinism/sandbox/JIT wording in high-visibility integration guides:
  - `docs/how-to/system-integration.md`
  - `docs/how-to/systems-integration-agi.md`
- Replaced global replay/native-code claims with bounded determinism and current threaded-trace execution wording.

### 2026-02-26 (R9)
- Removed stale register-model wording in `spec/tisc-spec.md` section 1 (`27 general-purpose registers`) and aligned it to the mandatory `R0–R80` architectural window with optional implementation-defined extensions.
- Replaced stale native-code JIT wording in `spec/t81vm-spec.md` with deterministic threaded trace mode terminology to match current runtime capability contracts.

## Audit Notes
- Ambiguous or weakly evidenced areas were treated conservatively; unresolved points should be considered **Indeterminate** until additional traceable evidence is added.
- This report is descriptive and evidence-based; it does not override normative `/spec` authority.
