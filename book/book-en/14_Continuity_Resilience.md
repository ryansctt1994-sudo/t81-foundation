# Chapter 14: Continuity and Resilience

## 14.1 The Cleanroom Protocol

**Status: Defined Direction**

Continuity planning assumes long-term reconstruction and verification needs.

### 14.1.1 Reconstruction Steps

1. recover normative specs and governance authority chain,
2. recover reference implementation and tests,
3. reproduce deterministic evidence for required release surfaces,
4. re-establish governance controls and incident workflows.

## 14.2 Single Points of Failure

**Status: Managed Risk**

Key concentration risks include maintainer concentration, infrastructure dependence, and undocumented operational knowledge.

Mitigation is process-heavy: mirrored artifacts, reproducible builds, and governance checklists.

## 14.3 Continuity Manifest

**Status: Documented**

Continuity-critical artifacts:

* `/spec` (normative behavior),
* governance docs (`docs/governance/`),
* deterministic release boundary definitions (`docs/product/`),
* status evidence (`docs/status/`),
* implementation and tests.

## 14.4 Immutable Formal Invariants

**Status: Project Identity Constraint**

The project identity requires:

1. explicit determinism boundary discipline,
2. canonical representations for hash-critical behavior,
3. policy-mediated safety controls,
4. auditable execution/governance evidence.
