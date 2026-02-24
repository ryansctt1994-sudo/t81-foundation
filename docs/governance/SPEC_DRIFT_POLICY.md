# Spec Drift Policy

**Status:** Active
**Enforcement:** Soft-Fail (Scanner available, CI blocked on Critical Drift)

## 1. Definition of Drift

Spec Drift occurs when the implementation diverges from the authoritative specification. This includes:

1.  **Orphaned Implementation:** Public API surfaces (headers, opcodes) exist in code but are not documented in `/spec`.
2.  **Ghost Spec:** The specification defines behavior or symbols that do not exist in the implementation.
3.  **Semantic Mismatch:** The behavior of the code contradicts the normative language of the spec (e.g., fault conditions, binary formats).

## 2. Detection Mechanism

The project uses `scripts/governance/spec_impl_drift_check.py` to scan for:
*   Unreferenced public headers.
*   Opcode enum vs spec mismatches.
*   Missing implementation directories for spec files.

## 3. Remediation Policy

When drift is detected:

### A. If Implementation is Ahead (Orphaned Code)
*   **Action:** Update the relevant spec file to include the new feature.
*   **Deadline:** Before the feature is released in a stable version.

### B. If Spec is Ahead (Ghost Spec)
*   **Action:** Mark the spec section as "Planned" or "Experimental", or implement the missing feature.
*   **Constraint:** Stable specs MUST NOT reference nonexistent features.

### C. If Conflict (Semantic Mismatch)
*   **Authority:** The `/spec` is the Source of Truth.
*   **Action:** Fix the implementation to match the spec.
*   **Exception:** If the spec is demonstrably wrong (e.g., impossible to implement), follow the **Break Procedure** in `FREEZE_ENFORCEMENT.md` to update the spec.

## 4. Acceptable Drift

Drift is permissible **only** under these conditions:

1.  **Experimental Features:** Code in `src/experimental/` or headers marked `[[experimental]]` may lead the spec.
2.  **Internal Implementation Details:** Helper classes/functions not part of the public API do not need spec coverage.
3.  **Draft Specs:** Specs marked "Status: Draft" may describe future states not yet implemented.

## 5. Enforcement

*   **CI Gate:** Future integration will block PRs that introduce new drift in Frozen surfaces.
*   **Release Gate:** No Major/Minor release can occur with Critical Drift in Verified surfaces.
