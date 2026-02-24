# Verified Surface Audit

**Status:** Active Audit
**Last Updated:** 2026-02-24
**Reference:** `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`

This document provides a rigorous trace of every "Verified" determinism surface to its specification, implementation, verification tests, and enforcement mechanism.

## Audit Matrix

| Surface | Spec Ref | Code Ref | Test Ref | CI Job | Freeze Scope | Status |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **TISC Opcode Semantics** | `spec/tisc-spec.md` | `core/vm/`, `core/isa/` | `tests/cpp/vm_determinism_property_test.cpp`<br>`tests/cpp/test_tritwise_backend_equivalence.cpp` | `ci.yml` | TISC ISA | **Verified** |
| **VM Interpreter Execution** | `spec/t81vm-spec.md` | `core/vm/` | `tests/cpp/vm_trace_test.cpp`<br>`tests/cpp/vm_determinism_property_test.cpp` | `ci.yml` | TISC ISA<br>Determinism Guarantees | **Verified** |
| **Data Type Canonical Encoding** | `spec/t81-data-types.md` | `src/data_types/` | `tests/cpp/v1_canonical_numeric_contract_test.cpp`<br>`tests/cpp/tisc_binary_io_determinism_test.cpp` | `ci.yml` | Data Types | **Verified** |
| **Soft-Float Deterministic Math** | `spec/t81-data-types.md` | `src/data_types/` | `tests/cpp/test_T81Float_arithmetic.cpp`<br>`tests/cpp/test_T81Float_rounding.cpp` | `ci.yml` | Determinism Guarantees | **Verified** |
| **Compiler Bytecode Emission** | `spec/t81lang-spec.md` | `src/lang/` | `scripts/ci/t81lang_repro_gate.py`<br>`tests/fixtures/t81lang_determinism/` | `repro-ledger.yml` | Determinism Guarantees | **Partial Traceability** |
| **T3K Quantization** | N/A | `src/codec/` | `scripts/ci/t3k_repro_gate.py` | `repro-ledger.yml` | Determinism Guarantees | **Partial Traceability** |

## Traceability Gaps

The following surfaces have gaps in their verification chain:

*   **Compiler Bytecode Emission**:
    *   **Spec Gap**: `spec/t81lang-spec.md` defines the language syntax but lacks a rigorous specification for deterministic bytecode emission rules (e.g., register allocation order, label resolution).
    *   **Action Required**: Expand `spec/t81lang-spec.md` to include a "Deterministic Compilation Profile" section.

*   **T3K Quantization**:
    *   **Spec Gap**: No formal specification document exists for the T3K quantization format or the `t81 weights quantize` command behavior, though implementation and tests are verified.
    *   **Action Required**: Draft `spec/t3k-quantization-spec.md` or add a normative section to `spec/t81-data-types.md`.
