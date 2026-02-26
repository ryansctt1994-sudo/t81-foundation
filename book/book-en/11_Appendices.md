# Chapter 11: Appendices

## 11.1 Boundary and Maturity Snapshot

**Status: Current as of this revision**

The practical question is not whether a feature exists, but whether it is inside verified deterministic release scope.

### 11.1.1 DCP-Certified Direction

DCP (Deterministic Core Profile) is the release-certified subset. Verified surfaces are tracked in:

* `docs/product/DETERMINISTIC_CORE_PROFILE.md`
* `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`

### 11.1.2 Governed Non-DCP Surfaces

The following may be implemented but are not DCP deterministic guarantees by default:

* Trace-JIT runtime paths
* Experimental distributed/cognitive-tier surfaces
* Governed `llama.cpp` inference path (`t81 llama-run`)

### 11.1.3 Open Promotion Work

Promotion of non-DCP surfaces requires governance gates, deterministic evidence, and registry updates:

* `docs/status/GOVERNED_AGI_PROMOTION_PIPELINE.md`
* `docs/governance/FREEZE_ENFORCEMENT.md`
* `docs/governance/DETERMINISM_THREAT_MODEL.md`

## 11.2 Glossary

| Term | Definition |
| :--- | :--- |
| **DCP** | Deterministic Core Profile; the release-certified deterministic subset of T81. |
| **Determinism Surface Registry** | Governance registry that marks surfaces as Verified/Partial/Unverified for determinism guarantees. |
| **Governed non-DCP** | Surface under policy/governance controls but outside DCP guarantee scope. |
| **Axion** | Policy and audit kernel integrated with runtime execution controls. |
| **CanonRef** | SHA3-256 content reference for immutable CanonFS objects. |
| **Promotion Pipeline** | Governance lifecycle used to move experimental surfaces toward verified status. |
| **Repro Gate** | Automated reproducibility checks used to detect determinism drift. |

## 11.3 Useful Links

* **Repository**: [github.com/t81dev/t81-foundation](https://github.com/t81dev/t81-foundation)
* **Normative Specs**: `spec/`
* **Book (this narrative)**: `book/book-en/`
* **Governance docs**: `docs/governance/`
* **Status/Promotion tracking**: `docs/status/`
