# Chapter 1: Introduction

## 1.1 Scope and Definition

**Status: Current Narrative (Non-Normative)**

T81 is a deterministic, ternary-oriented compute stack built around explicit execution boundaries, policy mediation, and auditability.

This book is a narrative companion. Normative behavior is defined by `/spec` and governance/product artifacts.

### 1.1.1 Core Invariants

T81 is built around four enduring invariants:

1. **Determinism with explicit scope**: guarantees apply to verified surfaces, not universally.
2. **Canonical representation**: hash-critical data must have stable canonical form.
3. **Policy-gated execution**: safety and capability constraints are enforced at runtime.
4. **Auditable behavior**: execution evidence must be inspectable and reproducible.

### 1.1.2 Scope Snapshot (As of February 26, 2026)

| Classification | Meaning | Typical Examples |
| :--- | :--- | :--- |
| **DCP / Verified** | release-certified deterministic guarantees | core interpreter/ISA/data-type deterministic surfaces |
| **Governed non-DCP** | policy-governed surfaces without DCP deterministic certification | governed inference pathways, promotion-candidate surfaces |
| **Experimental** | active development or research, no deterministic release claim by default | distributed tiers, infinite-form research paths |

Authoritative scope references:

* `docs/product/DETERMINISTIC_CORE_PROFILE.md`
* `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`
* `docs/status/GOVERNED_AGI_PROMOTION_PIPELINE.md`

## 1.2 System Architecture

T81 can be viewed as layered controls rather than a single runtime binary:

1. language/tooling surfaces,
2. VM/ISA execution surfaces,
3. policy and trace governance surfaces,
4. storage/model artifact integrity surfaces,
5. experimental higher-tier research surfaces.

### 1.2.1 The TISC Virtual Machine (T81VM)

**Status: Implemented**

The VM executes TISC with deterministic intent on verified surfaces and explicit trap/policy behavior on constraint violations.

### 1.2.2 The Axion Safety Kernel

**Status: Implemented**

Axion mediates capability/resource boundaries and records policy-relevant events.

### 1.2.3 Canonical Filesystem (CanonFS)

**Status: Implemented Core + Ongoing Expansion**

CanonFS anchors artifact identity to content hash workflows and canonical data handling.

### 1.2.4 The Cognitive Tiers

**Status: Mixed Maturity**

Tiered capability framing is active, but deterministic guarantee level depends on registry status and promotion state.

## 1.3 Verifiable Compute Mission

T81's mission is not maximum throughput; it is accountable, reproducible computation where guarantees are explicit and testable.

Key use cases include:

1. reproducible high-assurance runtime behavior,
2. policy-governed execution workflows,
3. auditable artifact- and trace-based operations.

## 1.4 Terminology

| Term | Definition |
| :--- | :--- |
| **DCP** | Deterministic Core Profile; release-certified deterministic subset. |
| **Determinism Surface** | Subsystem boundary with scoped deterministic claim. |
| **Axion** | Runtime policy/audit mediation subsystem. |
| **CanonFS** | Content-addressed artifact storage and integrity workflow. |
| **Governed non-DCP** | Policy-governed but non-DCP deterministic boundary. |
| **Promotion** | Governance lifecycle for upgrading assurance state. |
| **Repro Gate** | Automation that checks deterministic/reproducibility expectations. |

## 1.5 Verification Checklist

* [ ] Claims in narrative docs match current registry and DCP documents.
* [ ] Repro and freeze checks pass for required release surfaces.
* [ ] Governed non-DCP surfaces are clearly labeled as such.
* [ ] Experimental content is not presented as release-certified deterministic behavior.

<!-- chapter-nav-start -->

---

**Navigation**

- [Book Index](./README.md)
- [Next: Chapter 2: Core Principles and Invariants](./02_Principles.md)

<!-- chapter-nav-end -->
