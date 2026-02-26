# Full-System Architectural & Strategic Audit

Date: 2026-02-26  
Scope: `/src`, `/include`, `/spec`, `/docs`, `/book`, CI workflows, governance, contracts, ISA/opcodes, VM, Axion, determinism gates, benchmarks, tests, multilingual alignment, roadmap, release artifacts.

## Executive Summary
This repository is a substantial, working deterministic-runtime codebase with strong test/CI investment. Core pipeline elements are implemented (frontend, ISA encoding, VM interpreter, Axion engine, CanonFS, reproducibility gates), and representative determinism tests plus both repro gates were validated locally (`t81lang` and `t3k`) as passing.

The highest previously identified remediation gaps (spec/impl contradictions, documentation over-claims, workflow pinning drift, permissive stub opcode behavior, translation staleness machine-enforcement, trace policy granularity, and warning-only governance rows) were closed in this remediation cycle with linked test/governance evidence. Residual risk is now concentrated in long-horizon areas (non-DCP tensor/backend reproducibility and promotion governance for experimental surfaces).

Determinism is defensible only for explicitly bounded surfaces and only with caveats already present in some governance docs. The strategic position is best classified as a **Deterministic Runtime Candidate** (not pre-production infrastructure). If development stopped today, it would be remembered as a serious deterministic-systems research platform with unusually strong implementation depth, but inconsistent assurance posture.

## 1. Architectural Integrity

### 1.1 Drift Matrix (Spec vs Implementation)

| Area | Spec/Doc Claim | Implementation Evidence | Assessment |
|---|---|---|---|
| Register model | TISC defines mandatory `R0–R80` architectural window (`spec/tisc-spec.md`) | VM thread context stores a larger implementation-defined bank (`include/t81/vm/state.hpp`) | Reconciled to bounded extension model |
| Instruction encoding | Canonical TISC encoding is fixed-width 13-byte form (`spec/tisc-spec.md`) | Runtime uses 13-byte `{opcode,a,b,c}` (`core/isa/encoding.cpp`) | Aligned |
| JIT model | VM spec describes deterministic threaded trace mode (`spec/t81vm-spec.md`) | Capability contract describes threaded trace interpreter (not machine-code JIT) (`docs/reference/CAPABILITY_CONTRACT.md`) | Aligned |
| Axion policy invocation | Per-instruction policy expectation for governed execution | Interpreter does per-step checks; trace path now evaluates policy per instruction via JIT policy hook (`core/vm/vm.cpp`, `runtime/jit/jit_compiler.cpp`) | Aligned |
| Core maturity | Root status and reference status aligned to current Beta/Alpha posture (`README.md`, `docs/reference/STATUS.md`) | Current docs reflect bounded determinism and partial surfaces | Aligned |

### 1.2 Layer Violation Analysis
- Pipeline layering exists in code: frontend -> emitter -> VM -> Axion hooks.
- Authority layering is inconsistent across spec/status/reference artifacts.
- Experimental/stub opcode surfaces are present in core VM dispatch under the same ISA namespace.
- Legacy/archival naming persists in roadmap/reference documents.

### 1.3 Architectural Risk Score

| Metric | Score (1-10) | Basis |
|---|---:|---|
| Architectural Risk | 5.5 | Major norm/implementation contradictions remediated; residual risk now concentrated in bounded governance/semantic coverage gaps |

## 2. Determinism Validation

### 2.1 Determinism Confidence Rating
**Moderate** for scoped verified surfaces, **Low** for broad ecosystem claims.

### 2.2 Failure Surface Analysis
- VM float transcendental opcodes route through `T81Float` operations (deterministic dmath path when `T81_DETERMINISTIC` is enabled via build profile) (`core/vm/vm.cpp`, `CMakeLists.txt`).
- Determinism registry explicitly excludes/limits several surfaces (`docs/governance/DETERMINISM_SURFACE_REGISTRY.md`).
- Entropy/time APIs include non-deterministic sources unless overridden (`include/t81/entropy.hpp`, `include/t81/types/T81Time.hpp`).
- JIT trace execution now enforces policy per instruction through a policy callback and returns fail-closed on deny (`Trap::SecurityFault`) with regression coverage (`tests/cpp/vm_jit_per_instruction_policy_test.cpp`).

### 2.3 Determinism Threat Map

| Threat | Severity | Likelihood | Current Control |
|---|---|---|---|
| Float transcendental drift in non-strict builds | Medium | Low | Strict deterministic float profile is default in build configuration (`T81_DETERMINISTIC`) |
| Overclaim in active docs | Low | Low | CI overclaim guardrails + capability-boundary normalization in integration/status docs |
| JIT equivalence/policy drift | Medium | Low | Equivalence tests + per-instruction policy hook in trace execution |
| CanonFS read integrity trust | Medium | Medium | Write hashing enforced; read-time re-verify absent |
| Spec/encoding mismatch | High | Medium | No single harmonized canonical encoding narrative |

### 2.4 Validation Snapshot (Local)
- `ctest` selected tests passed: `t81_vm_determinism_property_test`, `jit_trace_equivalence_test`, `axion_policy_segment_event_test`.
- `scripts/ci/t81lang_repro_gate.py` passed (16 fixtures).
- `scripts/ci/t3k_repro_gate.py` passed.

## 3. Instruction Set Coherence (TISC)

### 3.1 Findings
- Opcode registry is extensive, mapped, and has matrix tests.
- Unimplemented extension/stub opcode surfaces are now largely fail-closed with explicit deny semantics and regression coverage.
- Encoding and register contract contradictions were reconciled in normative docs.

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
- JIT trace execution is real, tested, and now policy-checked per instruction; execution remains threaded trace (not native machine-code JIT).

### 4.2 Runtime Ratings

| Metric | Score | Interpretation |
|---|---:|---|
| Runtime Integrity | 6.8/10 | Strong implementation, bounded by model mismatches |
| Production Readiness | Not ready | Candidate-stage, not hardened infrastructure |

## 5. Axion Governance & Enforcement

### 5.1 Findings
- Axion policy engine and bytecode evaluator are implemented.
- Unknown policy clauses now hard-fail during parse (deterministic parse error).
- Embedded policy parse failures are now fail-closed at VM load (`SecurityFault` on execution path with deterministic deny-log event).

### 5.2 Governance Rating

| Metric | Rating |
|---|---|
| Governance Strength | Moderate |
| Risk Classification | Medium-High |

### 5.3 Missing Enforcement Surfaces
- No immediate cognitive-tier closure gaps are open in the 2026-02-26 compliance pass; remaining tier risk is governance posture and promotion evidence for still-experimental surfaces.
- Governance matrix and CI are synchronized for previously warning-level rows via machine-verifiable scripts; residual enforcement work is now mainly depth/coverage expansion, not missing baseline checks.

## 6. Documentation vs Reality

### 6.1 Overstatement Map
- Root and multilingual READMEs are aligned to bounded determinism language.
- Active docs now have overclaim guardrails enforced in CI to prevent stronger-than-implemented claim reintroduction.

### 6.2 Documentation Credibility Score

| Metric | Score (1-10) |
|---|---:|
| Documentation Credibility | 7.4 |

### 6.3 Required Corrections
No immediate high-priority corrections remain from this audit cycle. Ongoing governance work continues through scheduled month-close and promotion-gate processes.

## 7. Code Quality & Engineering Discipline

### 7.1 Findings
- Build/test matrix is broad and active; local CTest inventory shows 254 tests.
- Static analysis scope in CI has been expanded beyond `src/**` to include `core/**`, `kernel/**`, `runtime/**`, and `lang/**`; coverage is improved but still not full-repo exhaustive.
- Governance enforcement matrix now maps fail-closed runtime guarantees and promoted former warning rows to hard-fail checks (API lock, spec-code baseline, benchmark regression, overclaim/boundary guardrails).
- Workflow action pinning now passes strict thresholds (`tagged=0`, `unknown=0`) after SHA pinning remediation.

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
- Residual bounded-surface guarantees and experimental-surface promotion governance still block pre-production classification.

### 8.3 If Development Stopped Today
A technically serious deterministic-compute research/runtime candidate with strong artifacts and tests, remembered for both depth and unresolved assurance-coherence gaps.

## 9. Hard Truth

### 9.1 Most Serious Structural Risks (Top 5)
1. Tensor/backend hardware paths remain non-DCP and not cross-platform bit-exact by contract.
2. Experimental cognitive-tier surfaces are not yet promoted to verified/non-experimental status.
3. CanonFS read path integrity trust remains weaker than write-path guarantees.
4. Single-source normative convergence across all spec/reference/status artifacts is still incomplete.
5. Performance-gate scope currently focuses on SIMD kernel regression, not full workload families.

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

### 2026-02-26 (R10)
- Reconciled major monolithic-spec ISA contradictions in `spec/spec/t81-spec.md`:
  - replaced legacy `81-trit` instruction-word claims with canonical fixed-width `13-byte` encoding (`u8 opcode + 3x i32`)
  - replaced legacy `R0..R26` register model wording with `R0..R80` architectural window wording
  - updated decoder/fetch/alignment text to canonical record boundaries
- Preserved long-term native-ternary direction by explicitly framing ternary instruction-word layouts as a future profile that must deterministically transcode to canonical encoding.

### 2026-02-26 (R11)
- Added an explicit "Native Ternary Transition Profile (Planned)" subsection to `spec/tisc-spec.md`.
- Defined compatibility guardrails for eventual native ternary encoding:
  - deterministic/lossless transcoding to canonical 13-byte interchange
  - explicit profile identifiers at artifact boundaries
  - unchanged opcode semantics and `R0–R80` architectural contract across profiles

### 2026-02-26 (R12)
- Hardened privileged stub opcode behavior in `core/vm/vm.cpp`:
  - `AXSIGN`, `AXLINEAGE`, and `AXCANON` now fail closed with deterministic `SecurityFault` and explicit deny-log reasons.
- Added regression test coverage in `tests/cpp/vm_stubbed_privileged_opcode_fail_closed_test.cpp` and wired it into `CMakeLists.txt`.
- Updated ISA reference docs for consistency:
  - `spec/tisc/opcode-semantics.md`
  - `spec/tisc/opcode-registry.md`

### 2026-02-26 (R13)
- Updated `docs/governance/ENFORCEMENT_MATRIX.md` with a machine-verifiable hard-fail rule for unimplemented privileged opcode fail-closed behavior, mapped to `build-and-test` via `tests/cpp/vm_stubbed_privileged_opcode_fail_closed_test.cpp`.

### 2026-02-26 (R14)
- Expanded CI static-analysis (`clang-tidy`) compile-unit scope in `.github/workflows/ci.yml` from `src/**` only to include:
  - `core/**`
  - `kernel/**`
  - `runtime/**`
  - `lang/**`
- Expanded clang-tidy header filter accordingly (`include/t81|core|kernel|runtime|lang`) to improve analyzer coverage for non-`src` implementation surfaces.

### 2026-02-26 (R15)
- Hardened unimplemented async/network placeholder opcodes in `core/vm/vm.cpp`:
  - `NSEND`, `NRECV`, `VWAIT`, `VYIELD` now fail closed with deterministic `SecurityFault` and explicit deny-log reasons.
- Added regression coverage in `tests/cpp/vm_stubbed_async_network_opcode_fail_closed_test.cpp` and wired it into `CMakeLists.txt`.
- Updated ISA docs for implementation parity:
  - `spec/tisc/opcode-semantics.md`
  - `spec/tisc/opcode-registry.md`
- Added governance enforcement mapping in `docs/governance/ENFORCEMENT_MATRIX.md`.

### 2026-02-26 (R16)
- Updated `docs/reference/CAPABILITY_CONTRACT.md` to explicitly document fail-closed behavior for unimplemented tier/extension surfaces and to list async/network opcode semantics (`NSEND/NRECV/VWAIT/VYIELD`) as non-capabilities until implemented.

### 2026-02-26 (R17)
- Hardened unimplemented neural placeholder opcodes in `core/vm/vm.cpp`:
  - `TNEURALFWD`, `TNEURALBWD` now fail closed with deterministic `SecurityFault` and explicit deny-log reasons.
- Updated regression coverage by converting `tests/cpp/test_vm_neural_opcodes.cpp` to assert fail-closed behavior.
- Updated ISA/governance/reference docs for parity:
  - `spec/tisc/opcode-semantics.md`
  - `spec/tisc/opcode-registry.md`
  - `docs/governance/ENFORCEMENT_MATRIX.md`
  - `docs/reference/CAPABILITY_CONTRACT.md`

### 2026-02-26 (R18)
- Hardened Axion policy parser in `include/t81/axion/policy.hpp` to reject unknown policy clauses instead of skipping them.
- Extended `tests/cpp/vm_policy_parse_fail_closed_test.cpp` with an unknown-clause scenario to verify deterministic fail-closed VM behavior (`SecurityFault` with parse-failure log evidence).

### 2026-02-26 (R19)
- Hardened `AXCHECK` semantics in `core/vm/vm.cpp`: deny outcomes now fail closed with deterministic `SecurityFault` rather than continuing execution after a deny log.
- Added explicit Axion syscall reason handling for `AXCHECK`/`AXREPORT` (`include/t81/axion/reasons.hpp`) and policy-evaluated report handling in VM dispatch.
- Updated `tests/cpp/test_axion_opcodes.cpp` expectations to validate fail-closed behavior.
- Updated ISA docs for parity:
  - `spec/tisc/opcode-semantics.md`
  - `spec/tisc/opcode-registry.md`

### 2026-02-26 (R20)
- Updated `docs/governance/ENFORCEMENT_MATRIX.md` with explicit machine-verifiable hard-fail mapping for `AXCHECK` deny fail-closed semantics, backed by `tests/cpp/test_axion_opcodes.cpp`.

### 2026-02-26 (R21)
- Added explicit governance enforcement mapping for Axion policy parse fail-closed behavior (syntax and unknown-clause rejection), backed by `tests/cpp/vm_policy_parse_fail_closed_test.cpp`.

### 2026-02-26 (R22)
- Replaced stubbed system-register values in `core/vm/vm.cpp` with deterministic derived values:
  - `R76` lineage signature now derived from program instructions and policy/metadata text.
  - `R77` entropy signature now derived from deterministic runtime state tuple.
  - `R78` constitutional mask now derived from active policy capabilities/requirements.
- Added regression coverage in `tests/cpp/vm_system_registers_deterministic_test.cpp` and wired it into `CMakeLists.txt`.

### 2026-02-26 (R23)
- Added explicit regression coverage for `AXREPORT` policy-deny fail-closed semantics in `tests/cpp/vm_axreport_policy_deny_fail_closed_test.cpp`.
- Wired the new test into `CMakeLists.txt` and mapped it as a machine-verifiable hard-fail rule in `docs/governance/ENFORCEMENT_MATRIX.md`.

### 2026-02-26 (R24)
- Performed post-remediation audit refresh to reconcile top-level ratings/findings/open-risk lists with implemented closures (spec alignment, fail-closed opcode hardening, policy parser hard-fail, workflow pinning, and governance mapping evidence).

### 2026-02-26 (R25)
- Implemented new governance policy check scripts:
  - `scripts/governance/check_root_structure.py`
  - `scripts/governance/check_readme_naming.py`
  - `scripts/governance/check_translation_metadata.py`
  - `scripts/governance/check_docs_structure.py`
  - `scripts/governance/check_license_policy.py`
  - `scripts/governance/check_repo_artifact_hygiene.py`
- Integrated these checks into:
  - `scripts/governance/check_docs_governance_hygiene.py`
  - `.github/workflows/ci.yml` (`spec-and-docs` job)
- Updated `docs/governance/ENFORCEMENT_MATRIX.md` to promote root-structure, README naming, translation metadata, GPL/AGPL policy, and artifact-hygiene rows to machine-verifiable CI enforcement.

### 2026-02-26 (R26)
- Implemented explicit translation staleness threshold enforcement in `scripts/governance/check_translation_staleness.py`:
  - marks translation pairs stale when they are more than `30` days or `10` canonical commits behind English.
  - covers root README translations and book chapter pairs by canonical chapter prefix.
- Integrated the check into CI and governance aggregation:
  - `.github/workflows/ci.yml` (`spec-and-docs` job)
  - `scripts/governance/check_docs_governance_hygiene.py`
- Updated `docs/governance/ENFORCEMENT_MATRIX.md` to promote multilingual staleness threshold enforcement from warning/partial to machine-verifiable hard-fail.

### 2026-02-26 (R27)
- Normalized remaining over-strong sandbox/isolation language in integration guides:
  - `docs/how-to/system-integration.md`
  - `docs/how-to/systems-integration-agi.md`
- Added explicit capability-boundary note in AGI integration guidance to anchor all claims to `docs/reference/CAPABILITY_CONTRACT.md`.
- Corrected AGI integration register-window wording to the architectural `R0–R80` contract and removed present-tense distributed-network capability wording where implementation remains partial/fail-closed.

### 2026-02-26 (R28)
- Reconciled audit residual-risk language with current cognitive-tier compliance evidence (`docs/status/COGNITIVE_TIERS_SPEC_COMPLIANCE_2026-02-26.md`), replacing "open semantic closure gaps" wording with the current governance reality: experimental/non-DCP promotion discipline.

### 2026-02-26 (R29)
- Completed a focused post-remediation wording sweep across integration/status guidance to confirm no remaining high-risk overclaims of OS/hardware sandboxing or unconditional isolation guarantees in the audited documentation scope.
- Closed the audit-cycle "Required Corrections" section as complete, with remaining activity tracked as ongoing governance cadence rather than open remediation debt.

### 2026-02-26 (R30)
- Added per-instruction policy enforcement inside JIT trace execution:
  - extended JIT trace interface with policy callback support and explicit policy-deny exit kind.
  - wired VM trace execution to call `eval_axion_call(kStep, ...)` for each trace instruction with deterministic instruction-count progression.
  - fail-closed behavior on trace policy deny now returns `SecurityFault`.
- Added regression coverage: `tests/cpp/vm_jit_per_instruction_policy_test.cpp`.

### 2026-02-26 (R31)
- Reduced host-math exposure in VM float transcendental opcode dispatch by routing `FSin/FCos/FTan/FAsin/FAcos/FAtan/FSinh/FCosh/FTanh/FSqrt/FExp/FLog/FPow` through `T81Float` operations in `core/vm/vm.cpp`.
- Added strict deterministic float build profile option in `CMakeLists.txt`:
  - `T81_STRICT_DETERMINISTIC_FLOAT=ON` (default) -> compile definition `T81_DETERMINISTIC`.

### 2026-02-26 (R32)
- Promoted warning-only governance rows to machine checks by adding:
  - `scripts/governance/check_public_api_semver.py`
  - `scripts/governance/check_spec_code_alignment_baseline.py`
  - `docs/governance/PUBLIC_API_LOCK.json`
- Integrated checks into CI `spec-and-docs` and governance hygiene aggregation.

### 2026-02-26 (R33)
- Added documentation-governance guardrails:
  - `scripts/governance/check_overclaim_guardrails.py`
  - `scripts/governance/check_cognitive_tier_boundary.py`
- Integrated guardrails into CI/governance hygiene and normalized legacy wording in:
  - `docs/architecture/OVERVIEW.md`
  - `docs/reference/CAPABILITY_CONTRACT.md`

### 2026-02-26 (R34)
- Enabled benchmark regression hard-fail gating in CI benchmarks job by wiring:
  - `scripts/ci/check_simd_regression.py bench.json`
- Updated `docs/governance/ENFORCEMENT_MATRIX.md` to reflect hard-fail status for:
  - public API semver lock
  - spec-code alignment baseline coverage
  - benchmark performance regression gating
  - overclaim/cognitive-tier boundary documentation guardrails

## Audit Notes
- Ambiguous or weakly evidenced areas were treated conservatively; unresolved points should be considered **Indeterminate** until additional traceable evidence is added.
- This report is descriptive and evidence-based; it does not override normative `/spec` authority.
