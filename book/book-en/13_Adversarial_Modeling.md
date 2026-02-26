# Chapter 13: Adversarial Modeling and Determinism Attacks

## 13.1 Threat Model

**Status: Active Governance Artifact**

T81 assumes adversarial pressure at compiler, runtime, dependency, and governance layers.

Authoritative threat tracking lives in:

* `docs/governance/DETERMINISM_THREAT_MODEL.md`
* `docs/governance/INCIDENT_RESPONSE.md`

## 13.2 Compiler-Level Attacks

Primary concerns:

* source ambiguity and parser confusion,
* nondeterministic lowering paths,
* toolchain drift affecting emitted bytecode.

Mitigations include deterministic compilation checks and fixture-based repro gates.

## 13.3 VM and GC Attack Vectors

Primary concerns:

* host/environment influence on state progression,
* resource-exhaustion behavior,
* policy bypass attempts.

Mitigations include policy-gated execution, deterministic test coverage, and explicit trap behavior.

## 13.4 CanonFS and Hash Attacks

Primary concerns:

* hash misuse or weak artifact verification,
* non-canonical serialization causing identity drift,
* model/artifact substitution.

Mitigations include canonical serialization discipline and hash-bound loading controls.

## 13.5 Distributed Tier Time-Travel Attack

Distributed and higher-tier surfaces are treated as governed non-DCP/experimental unless promoted. Replay, withholding, and ordering attacks remain first-class threat categories.

## 13.6 Determinism Breach Postmortem Template

**Status: Operational Process**

Minimum response flow:

1. isolate divergence surface and reproduction case,
2. classify boundary impact (DCP / governed non-DCP / experimental),
3. apply mitigation and add regression evidence,
4. update registry/threat/governance records,
5. issue release-impact decision.

<!-- chapter-nav-start -->

---

**Navigation**

- [Book Index](./README.md)
- [Previous: Chapter 12: Formal Semantics of TISC and T81VM](./12_Formal_Semantics.md)
- [Next: Chapter 14: Continuity and Resilience](./14_Continuity_Resilience.md)

<!-- chapter-nav-end -->
