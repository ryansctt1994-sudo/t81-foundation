# Project Control Center

Status: Active Program Management
Last Updated: 2026-02-25
Owner: Project Management / Governance
Version: 2.0.0

## 1. Purpose

Provide the operational control view for delivery status, governance posture,
active risks, and near-term execution priorities.

## 2. Scope

This control document tracks program state across:

- architecture and docs governance
- determinism and release discipline
- implementation alignment and delivery risks

Authority remains:
`/spec` > `docs/architecture/OVERVIEW.md` > `/docs` > `/book`.

## 3. Current Program Snapshot

### 3.1 Structural and Governance Posture

- Documentation IA separation completed:
  - canonical docs in `docs/`
  - records in `docs/records/`
  - site machinery in `docs/site/`
- Institutional governance controls are in place:
  - module ownership charter
  - ADR system
  - release discipline manifest
  - incident response plan
  - deterministic corpus manifest
- Root-level artifact hygiene updated:
  - historical test logs moved to `docs/records/archive/`

### 3.2 Verification Snapshot

- Full configure/build and full test sweep completed in current cycle.
- Last local gate snapshot: `ctest` passed (`247` tests, `0` failures).
- A1 T81Lang drift decomposition execution track is closed (M1-M4, 2026-02-25)
  with synchronized matrix and governance evidence artifacts.

## 4. Active Workstreams

1. Governance maintenance and enforcement consistency
   - keep ADR and release discipline references synchronized
2. Determinism assurance continuity
   - maintain verified-surface evidence and corpus hash discipline
3. Spec-implementation alignment
   - A1 closure complete; continue high-drift reduction beyond current
     decomposition baseline
   - A1B closure complete; A1C next-cycle queue opened for section 3/6 and
     section 7 evidence-index hardening
   - A1C closure complete with matrix/governance synchronization; next queue
     definition remains active
   - A1D queue seeded for residual section 5 IR/lowering and section 8 stdlib
     evidence-index closure
   - A1D closure complete with matrix/governance synchronization; next queue
     definition remains active
   - A1E queue seeded for residual section 4 and section 3/6 evidence-addendum
     closure
4. Documentation lifecycle control
   - prevent root-level doc sprawl and stale governance artifacts

## 5. Risk Register

### 5.1 High

- Spec/implementation drift remains non-trivial in draft/experimental surfaces.
- Determinism overclaim risk persists if external summaries omit registry scope.

### 5.2 Medium

- Multilingual synchronization lag against English canonical content.
- Single-owner concentration for critical governance and release decisions.

### 5.3 Monitoring

- Benchmark noise due to environment variability.
- Experimental surface growth without promotion/retirement discipline.

## 6. 30/60/90 Day Plan

### 30 Days (Control Tightening)

- Enforce root-document hygiene and records archival discipline.
- Standardize status document refresh cadence.
- Ensure release template usage is consistent with release discipline manifest.

### 60 Days (Alignment)

- Reduce documented spec drift in highest-risk subsystems.
- Validate deterministic corpus manifest against release evidence workflow.
- Tighten cross-link integrity between governance, product, and status docs.

### 90 Days (Institutionalization)

- Establish repeatable monthly governance review checkpoint.
- Produce release-readiness evidence packet from existing governance artifacts.
- Confirm backlog decomposition aligns with DCP and freeze boundaries.

## 7. Governance Directives

1. Spec-first policy remains mandatory for normative behavior changes.
2. Freeze boundaries remain unchanged and non-negotiable.
3. Determinism claims remain limited to Verified registry surfaces.
4. Public API contract remains `include/t81/**` only.
5. New root-level documentation files require explicit governance approval.

## 8. Cross-References

- `docs/governance/SPEC_AUTHORITY_MODEL.md`
- `docs/governance/FREEZE_ENFORCEMENT.md`
- `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`
- `docs/governance/INCIDENT_RESPONSE.md`
- `docs/product/DETERMINISTIC_CORE_PROFILE.md`
- `docs/product/RELEASE_DISCIPLINE.md`
- `docs/status/IMPLEMENTATION_MATRIX.md`
- `docs/status/SYSTEM_STATUS.md`
- `docs/status/EXECUTION_PLAN_2026-03.md`

## 9. Versioning Statement

This control document is operational governance. Updates must preserve authority
hierarchy and may not weaken freeze or determinism boundaries.
