# T81Lang Bitwise Semantics Clarification and Logical Lowering Follow-up Report

## 1. Summary

This task finalized the T81Lang bitwise integration by:
1.  **Clarifying and enforcing** the boolean-to-integer coercion behavior in bitwise operations.
2.  **Adding a regression test** (`test_bitwise_boolean_coercion`) to the frontend IR generator test suite.
3.  **Updating the Specification** (`spec/t81lang-spec.md`) to explicitly document the coercion rule (Section 2.6).
4.  **Creating a follow-up plan** for the missing IR lowering of logical operators (`&&`, `||`).

## 2. Boolean→Integer Coercion Classification and Semantics

The coercion is classified as a **Special-Case Language Rule** for bitwise operations.

*   **Semantics:** Boolean values (`true`/`false` or results of comparisons) are implicitly converted to `T81Int` when used as operands for bitwise operators (`&`, `|`, `^`, `<<`, `>>`, `>>>`).
*   **Mapping:** `true` → `1`, `false` → `0`.
*   **Contexts:** Strictly limited to bitwise binary expressions. It does **not** apply to standard arithmetic (`+`, `*`, etc.) or other integer contexts (like array indexing), to preserve type safety.
*   **Implementation:**
    *   **Frontend:** `SemanticAnalyzer` explicitly permits `Type::Kind::Bool` in bitwise checks and coerces them to `Type::Kind::I32` for widening/lowering.
    *   **IR Generation:** `IRGenerator` (via `ensure_kind`) allows implicit `Boolean` to `Integer` register conversion (identity op, as VM booleans are 0/1 integers).

## 3. Spec/Docs/Comment Updates

*   **`spec/t81lang-spec.md`**: Added **Section 2.6 Boolean Coercion** defining the rule. Updated **Arithmetic Correctness** section to mention the exception for bitwise ops.
*   **`include/t81/frontend/ir_generator.hpp`**: Added comment to `ensure_kind` explaining the `Boolean -> Integer` path.
*   **`src/frontend/semantic_analyzer.cpp`**: Modified `visit(BinaryExpr)` to allow `Bool` operands for bitwise tokens and coerce them to `I32`.

## 4. Regression Guard Test Additions

*   **Test File:** `tests/cpp/frontend_ir_generator_test.cpp`
*   **Test Case:** `test_bitwise_boolean_coercion`
*   **Code Tested:**
    ```t81
    fn main() -> i32 {
        let a: i32 = (1 == 1) & 1;
        let b: i32 = (1 == 1) | (2 < 3);
        let c: i32 = (1 != 1) ^ (2 > 3);
        return a;
    }
    ```
*   **Assertions:** Checks for presence of `BITAND`, `BITOR`, `BITXOR` opcodes, ensuring lowering succeeded.
*   **Results:** `PASS` (verified via `ctest` / execution).

## 5. Logical Operator Lowering Follow-up Plan Artifact

*   **File:** `docs/plans/T81Lang_logical_operator_lowering_followup.md`
*   **Key Decisions:**
    *   **Lowering Strategy:** Control-flow based (short-circuiting), using `JZ`/`JMP` and labels.
    *   **Semantics:** Left-to-right, short-circuit evaluation (RHS not evaluated if LHS determines result).
    *   **Result:** Canonical boolean (`0`/`1`).

## 6. Validation Results

*   [x] Boolean→integer coercion behavior classified (Language Rule).
*   [x] Mapping and allowed contexts documented explicitly.
*   [x] Spec/docs and code comments updated.
*   [x] Regression test added for comparison-result used in bitwise lowering.
*   [x] Test asserts lowering succeeds and preserves precedence shape.
*   [x] `&&` / `||` lowering follow-up plan artifact created.
*   [x] Existing passing parser/IR tests not regressed (`t81_ir_snapshot_audit_test` passed).
*   [x] Follow-up implementation report created.

## 7. Remaining Gaps and Next Steps

*   **Logical Operators (`&&`, `||`):** Still raise "Unsupported binary operator" (or fallback error) in IR generation. Requires implementation according to the plan in `docs/plans/T81Lang_logical_operator_lowering_followup.md`.
*   **Boolean Arithmetic:** Explicitly *disallowed* in `+`, `-`, `*`, `/`. This is intentional but might need future review if "Ternary-native" implies full algebraic interchangeability. Current spec is conservative (bitwise only).
