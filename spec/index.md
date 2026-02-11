# T81 Specification Index

This is the canonical index for top-level specs in `spec/`.

## Document Status Model
- `normative-draft`: intended to govern implementations; still under active revision.
- `supplemental-draft`: useful and referenced, but not yet fully integrated into the core layer set.
- `historical-companion`: background/context only, non-normative.

## Core Layer Specs
| Document | Role | Status |
|---|---|---|
| `t81-overview.md` | Architectural overview and stack framing | normative-draft |
| `t81-data-types.md` | Canonical type semantics | normative-draft |
| `tisc-spec.md` | ISA semantics and opcode contracts | normative-draft |
| `t81vm-spec.md` | Runtime, memory model, execution contracts | normative-draft |
| `t81lang-spec.md` | Language semantics and lowering model | normative-draft |
| `axion-kernel.md` | Supervisory/policy enforcement model | normative-draft |
| `cognitive-tiers.md` | Tier model and constraints | normative-draft |

## Supplemental Specs
| Document | Role | Status |
|---|---|---|
| `canonfs-spec.md` | CanonFS storage model | supplemental-draft |
| `hanoi-kernel-spec.md` | Hanoi kernel reference spec | supplemental-draft |
| `axion-policy-grammar.md` | APL grammar and verification targets | supplemental-draft |
| `cpp-mapping.md` | Legacy-to-current API mapping notes | supplemental-draft |
| `constitution.md` | Governance principles | supplemental-draft |

## Historical Companion
| Document | Role | Status |
|---|---|---|
| `spec/t81-spec.md` | Long-form narrative/spec companion | historical-companion |

## RFCs
- RFC catalog and statuses: `spec/rfcs/index.md`
- RFC process/template: `spec/rfcs/README.md`, `spec/rfcs/template.md`

## Conformance Note
When conflicts exist between docs:
1. Top-level normative-draft specs in `spec/*.md` take precedence.
2. Accepted/integrated RFC text informs upcoming edits.
3. Historical companion text is non-normative unless explicitly promoted.
