# Specification Authority Model

**Effective Date:** 2024-10-27
**Status:** **Active**

This document establishes the canonical hierarchy of documentation, defines the "Frozen" state for core components, and mandates the protocols for modifying specifications and implementation.

---

## 1. Authority Levels

The T81 documentation ecosystem is stratified by authority. In the event of a contradiction, the higher-authority source prevails.

| Level | Path | Content Type | Authority |
| :--- | :--- | :--- | :--- |
| **1** | `/spec/**` | **Normative Specs** | **Absolute**. Defines the "law" of the system. |
| **2** | `/docs/architecture/OVERVIEW.md` | **Architecture** | **High**. Defines system boundaries and invariants. |
| **3** | `/docs/**` | **Descriptive Docs** | **Medium**. Explains the specs and architecture. |
| **4** | `/book/**` | **Narrative** | **Low**. Educational and illustrative. |
| **5** | `/notebooks/**`, `/examples/**` | **Experimental** | **None**. Use at your own risk. |
| **N/A** | `/artifacts/**`, `/benchmarks/results/**` | **Generated** | **None**. Ephemeral outputs. |

---

## 2. Conflict Resolution Rules

1.  **Spec vs. Docs**: If a statement in `/docs` conflicts with `/spec`, the **`/spec` definition is correct**. The documentation must be updated to match the spec.
2.  **README vs. Architecture**: If the root `README.md` conflicts with `/docs/architecture/OVERVIEW.md`, the **`OVERVIEW.md` is correct**.
3.  **Book vs. Specs/Docs**: The `/book` directory is a monograph and may lag behind the bleeding edge. If it conflicts with `/docs` or `/spec`, the **`/docs` or `/spec` prevails**.
4.  **Code vs. Spec**: If the implementation in `src/` behaves differently than defined in `/spec/`, **it is a bug in the code**, unless the spec is explicitly marked as "Draft".

---

## 3. Freeze Boundaries

Freeze boundaries are defined in `FREEZE_ENFORCEMENT.md`.

Certain subsystems are designated as **Frozen**. Changes to these areas are restricted to preserve stability and determinism.

### A. Data Types
*   **Scope**: `src/data_types/` and `spec/t81-data-types.md`.
*   **State**: **Frozen**.
*   **Guarantee**: Bit-exact binary representation for `Trit`, `Tryte`, and `T81Float`.

### B. TISC ISA
*   **Scope**: `src/tisc/`, `src/vm/` (dispatch loop), and `spec/tisc-spec.md`.
*   **State**: **Frozen**.
*   **Guarantee**: Opcode values and execution semantics are immutable for a given major version.

### C. Determinism Guarantees
*   **Scope**: Execution traces, floating-point math (`dmath`), and serialization.
*   **State**: **Frozen**.
*   **Guarantee**: Bit-exact reproducibility across x86-64 and ARM64 (Linux/macOS).

### D. Public API Surface
*   **Scope**: `include/t81/` headers.
*   **State**: **Versioning**.
*   **Guarantee**: Semantic Versioning (SemVer) compliance.

---

## 4. Change Protocols

All changes must adhere to the following protocols based on their nature.

### A. Spec-Only Change
*   **Allowed**: Only for clarification, typo fixes, or adding non-normative examples.
*   **Prohibited**: Changing normative behavior without code updates.
*   **Requirement**: Must not invalidate existing conformant implementations.

### B. Implementation-Only Change
*   **Allowed**: Refactoring, optimization, bug fixes that bring code *into* alignment with spec.
*   **Prohibited**: Changing observable behavior defined in a frozen spec.
*   **Requirement**: Must pass all existing regression tests and determinism gates.

### C. Spec + Impl Change (Feature)
*   **Allowed**: Adding new opcodes (if ISA version permits), new APIs, or new subsystems.
*   **Requirement**:
    1.  Update `/spec/`.
    2.  Update `src/`.
    3.  Add tests in `tests/`.
    4.  Verify no regression in determinism.

### D. Breaking Change
*   **Definition**: Any change that alters the meaning of existing TISC bytecode, breaks binary compatibility of data types, or changes the public API in a backward-incompatible way.
*   **Requirement**:
    1.  **Major Version Bump** (e.g., v1.x -> v2.0).
    2.  **RFC Approval**: Must be approved via the RFC process.
    3.  **Migration Guide**: A guide must be added to `docs/migration/`.
    4.  **Changelog**: Explicit entry in the changelog.

### E. Experimental Addition
*   **Allowed**: Adding files to `/notebooks/`, `/examples/`, or `src/experimental/`.
*   **Requirement**: Must not be linked from normative specs or core architecture docs as a dependency.

### F. New Root File
*   **Prohibited**: Adding new files to the repository root is **strictly prohibited** without unanimous maintainer approval.

---

## 5. Directory Ownership

Subsystems own specific directories. New files must be placed accordingly.

| Subsystem | Owner Directory | Implementation | Specification |
| :--- | :--- | :--- | :--- |
| **Core Types** | `src/data_types/` | `src/data_types/` | `spec/t81-data-types.md` |
| **TISC ISA** | `src/tisc/` | `src/tisc/` | `spec/tisc-spec.md` |
| **VM** | `src/vm/` | `src/vm/` | `spec/t81vm-spec.md` |
| **Language** | `src/lang/` | `src/lang/` | `spec/t81lang-spec.md` |
| **Axion** | `src/axion/` | `src/axion/` | `spec/axion-kernel.md` |
| **Docs** | `docs/` | N/A | `docs/spec/INDEX.md` |
| **Book** | `book/` | N/A | N/A |

---
