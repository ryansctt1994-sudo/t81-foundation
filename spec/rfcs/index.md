# T81 RFC Index

This index tracks RFC status and intended disposition.

## Status Legend
- `draft`: active authoring
- `proposed`: under formal review
- `accepted`: design accepted, pending/ongoing integration
- `integrated`: merged into normative specs
- `superseded`: replaced by a newer RFC
- `rejected`: closed without adoption

## RFC Catalog
| RFC | Title | Status | Notes |
|---|---|---|---|
| RFC-0000 | T81 Base-81 Ternary Computing Stack | draft | Foundational umbrella document |
| RFC-0001 | Architecture Principles | draft | Candidate for partial integration into `t81-overview.md` |
| RFC-0002 | Deterministic Execution Contract | draft | Cross-layer invariants |
| RFC-0003 | Axion Safety Model | draft | Axion threat/safety model |
| RFC-0004 | Canonical Tensor Semantics | draft | Tensor semantics harmonization |
| RFC-0005 | TISC v0.4 Extensions | draft | ISA extension planning |
| RFC-0006 | Deterministic GC | draft | VM reclaim semantics |
| RFC-0007 | T81Lang Standard Library | draft | Stdlib contract definition |
| RFC-0008 | Formal Verification Harness | draft | Proof/replay harness design |
| RFC-0009 | Axion Policy Language (APL) | draft | Superseded path by RFC-0022 once accepted |
| RFC-0010 | TISC Float/Fraction Ops | draft | Non-integer opcode plan |
| RFC-0011 | T81Lang Grammar Modernization | draft | Grammar evolution proposal |
| RFC-0012 | Ternary Tensor Quantization | draft | AI-native data representation |
| RFC-0013 | Ternary Matmul | draft | Trit-packed matmul path |
| RFC-0014 | Neural Primitives | draft | `train`/`infer` language constructs |
| RFC-0015 | Agentic Constructs | draft | First-class agent model |
| RFC-0016 | SIMD Limb | proposed | SIMD arithmetic proposal |
| RFC-0017 | Introduce T81 Native | proposed | Register-native type proposal |
| RFC-0018 | T81 Native SIMD Arithmetic | proposed | Follow-on SIMD arithmetic details |
| RFC-0019 | Axion Match Logging | draft | Guard/loop metadata auditing |
| RFC-0020 | Axion Segment Trace | draft | Segment trace semantics |
| RFC-0021 | Tier4 Cognition | draft | Tier-4 reflection/cognition proposal |
| RFC-0022 | Axion Policy Language Evolution | draft | Preferred evolution track for APL |
| RFC-0023 | T81Lang Print Canonical Runtime | draft | Deterministic print/runtime surface |
| RFC-0024 | C++23 Wording Alignment | draft | Documentation/process wording alignment |

## Active Consolidations
- APL track: RFC-0022 is the forward path; RFC-0009 is retained for provenance until formal supersession.

## Authoring
- Start from `spec/rfcs/template.md`.
- Update this index whenever RFC status changes.
