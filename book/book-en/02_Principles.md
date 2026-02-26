# Chapter 2: Core Principles and Invariants

## 2.1 The Determinism Invariant

**Status: Governance-Enforced (Scope-Bounded)**

T81 treats determinism as a constrained guarantee, not a blanket slogan.

For verified surfaces, identical inputs and configuration must yield identical outputs and trace outcomes across supported release platforms.

Formal intent:

$$
\text{Exec}(S_0, I, C) \to S_n \quad\text{is invariant for verified surfaces}
$$

Where `C` includes policy, build boundary, and runtime mode.

### 2.1.1 Determinism Surfaces and Attack Vectors

The authoritative scope is tracked in:

* `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`
* `docs/product/DETERMINISTIC_CORE_PROFILE.md`

Common risks and controls:

| Surface | Risk | Control |
| :--- | :--- | :--- |
| Compiler emission | nondeterministic ordering or lowering drift | canonical frontend/IR plus repro gate |
| VM interpreter | host-dependent behavior | deterministic interpreter tests and trace checks |
| Numeric behavior | host math library variance | deterministic float behavior on verified path |
| Serialization | semantically equal objects hashing differently | canonical encoding rules |
| Governance boundary | overclaiming guarantees on non-verified surfaces | registry + release discipline |

### 2.1.2 The "Libm Gap" and `dmath`

Host `libm` behavior can vary by platform and toolchain. T81 addresses this through deterministic numeric controls on verified surfaces and explicit boundary statements for non-verified paths.

Determinism claims must always be interpreted through registry status, not through narrative wording alone.

## 2.2 Ternary Logic (Base-3)

**Status: Foundational Design Principle**

T81 is ternary-native in type and representation design. Balanced ternary remains central to the architecture identity and canonical type system.

### 2.2.1 Why Ternary?

1. Symmetric signed representation.
2. Clear radix identity and encoding discipline.
3. Architectural continuity across types, ISA, and canonicalization.

### 2.2.2 Implementation

Implementation details evolve, but public behavior is constrained by normative spec and compatibility controls:

* `spec/t81-data-types.md`
* `spec/tisc-spec.md`
* `spec/t81vm-spec.md`

## 2.3 Auditability and The Axion Trace

**Status: Implemented (scope-specific guarantees)**

Auditability is a first-class requirement. Axion events and traces provide post-hoc verification evidence for execution behavior.

### 2.3.1 The Trace Structure

Conceptually, each trace step links operation, verdict, and state progression under policy control.

### 2.3.2 Example Trace

For deterministic surfaces, repeated runs under identical inputs and controls should produce stable trace outcomes according to registry-verified expectations.

## 2.4 The Nine Principles (Ethics Enforcement)

**Status: Policy and Governance Framing**

The project philosophy emphasizes:

* safety over opportunistic optimization,
* explicit policy gates,
* auditable transitions,
* bounded authority for high-risk capabilities.

Axion is the enforcement mechanism; governance docs define institutional constraints.

## 2.5 Verification Checklist

* [ ] Determinism claims match current registry status.
* [ ] DCP boundaries are not overextended by narrative language.
* [ ] Repro gates pass for required surfaces.
* [ ] Freeze-integrity checks pass for release candidates.
* [ ] Incident response path is defined for regressions.

## 2.6 Formal Audit Matrix

| Principle | Normative Source | Validation Surface |
| :--- | :--- | :--- |
| Scope-bounded determinism | `docs/governance/DETERMINISM_SURFACE_REGISTRY.md` | repro gates + CI |
| DCP release boundary | `docs/product/DETERMINISTIC_CORE_PROFILE.md` | release readiness packet |
| Freeze discipline | `docs/governance/FREEZE_ENFORCEMENT.md` | governance checks |
| Threat-oriented hardening | `docs/governance/DETERMINISM_THREAT_MODEL.md` | security/governance review |
