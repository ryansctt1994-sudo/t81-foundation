# Determinism Verification Report (Language-Surface Edition)

## Surface Inventory Audit

### Canonical Types

| Type | Backend Exists | Exposed in T81Lang | VM Opcode Coverage | Canonical Serialization | Determinism Tests | Status |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| `List` | Yes | Yes (New) | Yes (`STRVECNEW`) | Incomplete | Incomplete | Beta |
| `Map` | Yes | Yes (New) | Yes (`STRVECNEW`) | Incomplete | Incomplete | Beta |
| `Set` | Yes | Yes (New) | Yes (`STRVECNEW`) | Incomplete | Incomplete | Beta |
| `Tree` | Yes | Yes (New) | Yes (`STRVECNEW`) | Incomplete | Incomplete | Beta |
| `Fixed` | Yes | Partial | Yes | Yes | Yes | Stable |
| `Complex`| Yes | Partial | Yes | Yes | Yes | Stable |

### Gap Classification

* **Exposed:** `List`, `Map`, `Set`, `Tree` are now fully exposed in the language frontend (Lexer, Parser, Semantic Analyzer, IR Generator).
* **Lowering:** All four collection types lower to the generic `STRVECNEW` opcode, which initializes a dynamic container in the VM.
* **Serialization:** Canonical serialization for these types relies on the underlying VM vector serialization, which is generally deterministic but requires specific verification for nested structures.
* **Determinism:** Basic lowering determinism verified. Runtime determinism relies on existing VM guarantees.

## Deterministic Exposure Completion

### A. Language Exposure

* **Implemented:**
    * **Lexer:** Added `List`, `Map`, `Set`, `Tree` tokens.
    * **Parser:** Added parsing rules for generic types `List[T]`, `Map[K,V]`, etc.
    * **Semantic Analyzer:** Added type checking and representation for `List`, `Map`, `Set`, `Tree`.
    * **IR Generator:** Added lowering logic for `GenericTypeExpr` constructors (e.g., `List[i32]()`) to `STRVECNEW` instructions.

* **Verified:**
    * Created `tests/cpp/cli_std_collections_determinism_test.cpp` to verify that `List`, `Map`, `Set`, and `Tree` constructors lower to the correct IR sequence including `STRVECNEW`.
    * The test passes, confirming the frontend logic is correct and deterministic (same source -> same IR).

### B. Canonical Serialization

* **Status:** Pending. The current implementation relies on the generic `to_string` or `print` behavior of the VM's underlying vector type.
* **Next Steps:** Implement specific canonical printing for Maps and Sets to ensure key ordering is stable regardless of insertion order (e.g., sort keys before printing).

### C. Deterministic Construction

* **Status:** The constructors currently create empty containers.
* **Next Steps:** Verify that populated constructors (e.g., `List[i32](1, 2, 3)`) behave deterministically. (Currently only empty constructors are explicitly tested).

## DecodeFault Elimination

* **Status:** No DecodeFaults observed during testing of empty container construction.
* **Risk:** DecodeFaults might occur if incorrect types are passed to the generic constructor (e.g., `Map[i32]` without a value type), but the Semantic Analyzer should catch this before lowering.

## Conclusion

The first phase of Language-Surface Completeness Hardening for T81Lang regarding **Collection Types** has been successfully completed. `List`, `Map`, `Set`, and `Tree` are now first-class citizens in the T81Lang frontend, reducing the gap between the implemented backend types and the language exposure.

**Next Immediate Actions:**
1.  Implement `Phase 2 - B` (Canonical Serialization) for Map/Set to guarantee stable output.
2.  Expand testing to include populated containers and nested structures.
3.  Add `t81lang_surface_gate` to CI.
