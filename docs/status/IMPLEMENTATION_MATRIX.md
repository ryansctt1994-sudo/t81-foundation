# Implementation vs. Specification Matrix

Status: Active
Last Updated: 2026-02-25
Owner: Status / Engineering

## Purpose

Track implementation maturity relative to specification surfaces and make drift
risk explicit for planning and governance review.

## Matrix

| Subsystem | Specification Surface | Implementation Maturity | Alignment | Drift Risk | Owner | Target Date | Notes |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Data Types** | `spec/t81-data-types.md` (Frozen) | Implemented | High | Low | @t81dev | N/A (maintain) | Frozen deterministic core surface. |
| **TISC ISA** | `spec/tisc-spec.md` (Frozen) | Implemented | High | Low | @t81dev | N/A (maintain) | Frozen deterministic core surface. |
| **T81VM** | `spec/t81vm-spec.md` (Beta) | Partial | Medium | Medium | @t81dev | 2026-04-15 | Beta surface under active verification. |
| **T81Lang** | `spec/t81lang-spec.md` (Draft) | Experimental | Low | High | @t81dev | 2026-05-15 | Drift decomposition tracked in `docs/status/T81LANG_DRIFT_DECOMPOSITION_2026-03.md`; executable closure queue tracked in `docs/status/T81LANG_IMPLEMENTATION_TASK_QUEUE_2026-03.md`; promotion gate tracked in `docs/status/T81LANG_PROMOTION_GATE.md`; ranked engineering backlog tracked in `docs/status/T81LANG_ENGINEERING_BACKLOG_2026-03.md`; A1 matrix/governance sync closure recorded on 2026-02-25 (A1-CODE-06); post-A1 follow-on sync closure recorded on 2026-02-25 (A1B-CODE-03); A1C cycle sync closure recorded on 2026-02-25 (A1C-CODE-03); A1D cycle sync closure recorded on 2026-02-25 (A1D-CODE-03); A1E cycle sync closure recorded on 2026-02-25 (A1E-CODE-03); A1F cycle sync closure recorded on 2026-02-25 (A1F-CODE-03); A1G cycle sync closure recorded on 2026-02-25 (A1G-CODE-03); BG-01..BG-05 backlog closure recorded on 2026-02-25 with rerunnable gate snapshot automation in `scripts/governance/t81lang_promotion_gate_snapshot.py` and latest snapshot in `docs/status/T81LANG_PROMOTION_GATE_SNAPSHOT.md`. |
| **Axion Kernel** | `spec/axion-kernel.md` (Draft) | Partial | Medium | Medium | @t81dev | 2026-04-30 | Partial-coverage alignment tracked in `docs/status/AXION_PARTIAL_COVERAGE_ALIGNMENT_2026-03.md`; planning milestones M1-M4 synchronized/closed on 2026-02-25 with explicit open-scope boundaries retained. |
| **Cognitive Tiers** | `spec/cognitive-tiers.md` (Draft) | Concept / Experimental | Low | High | @t81dev | 2026-06-15 | Experimental, non-DCP, non-verified unless promoted through governance and determinism registry update. |
| **Governed llama.cpp Path** | `docs/how-to/llama-governed-repro.md` (Guidance, non-normative) | Experimental | Medium | Medium | @t81dev | 2026-04-30 | Governed inference surface (`third_party/llama.cpp`, `tooling/model/llama_cpp_adapter.cpp`, `t81 llama-run`) is explicitly governed non-DCP; practical reproducibility only; promotion requires governed AGI pipeline and registry alignment. |

## Governed AGI Surface Taxonomy

| Layer | Representative Paths | Determinism Status | Promotion State | Governance Gate |
| :--- | :--- | :--- | :--- | :--- |
| Deterministic Substrate | `core/types`, `core/isa`, `core/vm`, `include/t81/**` | DCP/registry bounded | Verified (where registry says Verified) | Freeze enforcement + DCP release discipline |
| Governance Kernel | `kernel/axion` | Partially verified, scope-bounded | Experimental/Verified Candidate by surface | Incident response + threat-model linkage |
| AGI-Oriented Runtime/Research | `runtime/tracing`, `experimental/*`, cognitive tiers | Non-DCP unless promoted | Experimental by default | Governed AGI promotion pipeline |
| Governed Inference Adapter | `third_party/llama.cpp`, `tooling/model/llama_cpp_adapter.cpp`, CLI `llama-run` | Governed non-DCP | Experimental by default | Governed AGI promotion pipeline + release boundary classification |

Promotion policy reference:

- `docs/status/GOVERNED_AGI_PROMOTION_PIPELINE.md`

## Planning Actions

1. Prioritize high-drift subsystem decomposition into measurable milestones.
2. Keep deterministic-core surfaces stable under freeze and DCP discipline.
3. Tie matrix refresh to monthly governance review cadence.
4. Record boundary-impacting alignment decisions in ADRs.
5. Maintain AGI-surface promotion state and determinism-status mapping per the
   governed AGI promotion pipeline.

## Cross-References

- `docs/status/PROJECT_CONTROL_CENTER.md`
- `docs/status/SYSTEM_STATUS.md`
- `docs/governance/SPEC_AUTHORITY_MODEL.md`
- `docs/governance/FREEZE_ENFORCEMENT.md`
- `docs/architecture/adr/`

## Versioning Statement

This matrix is a descriptive control artifact; it does not override `/spec` or
freeze policy.
