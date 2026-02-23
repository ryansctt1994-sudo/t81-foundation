# T81Lang Logical Operator Lowering Follow-up Plan (`&&`, `||`)

## 1. Current Status

*   **Parser support status:** Fully implemented. The parser correctly parses `&&` (logical and) and `||` (logical or) with correct precedence (binding tighter than `||` but looser than `|`).
*   **Spec status:** Defined in `spec/t81lang-spec.md`. The spec mandates short-circuiting left-to-right evaluation and canonical boolean results (`0t81`/`1t81`).
*   **IR/lowering status (gap):** **Not implemented.** The `IRGenerator` (in `include/t81/frontend/ir_generator.hpp`) does not handle `TokenType::AmpAmp` or `TokenType::PipePipe` in `visit(const BinaryExpr& expr)`. Attempting to use them will likely result in an "Unsupported binary operator" runtime error during compilation (or fallthrough to a crash).
*   **Runtime status (unsupported / partial):** The TISC VM has no single opcode for short-circuiting logical operators because they require control flow. They must be lowered to branches.

## 2. Why This Follow-up Exists

*   **Prevent parser/spec drift:** The parser accepts these operators, and the spec defines them, but the backend crashes or fails. This gap must be closed to avoid user confusion and compiler fragility.
*   **Define backend semantics:** Before implementing the lowering logic, we must explicitly agree on the control flow structure (short-circuiting) to ensure determinism and correct side-effect handling.

## 3. Semantics Decisions Required

*   **Short-circuit vs Eager:** The spec **requires** short-circuit evaluation. The lowering MUST NOT evaluate the right-hand side if the left-hand side determines the result.
    *   `lhs && rhs`: If `lhs` is false (0), return 0. Else evaluate `rhs`.
    *   `lhs || rhs`: If `lhs` is true (!= 0), return 1. Else evaluate `rhs`.
*   **Result Type:** The result MUST be a canonical boolean `T81Int` (0 or 1).
*   **Operand Type Requirements:** Operands must be booleans (or coercible to booleans?). Currently, `SemanticAnalyzer` enforces `Type::Kind::Bool` for logical operators.
*   **Interaction with Side Effects:** If the RHS has side effects (e.g., `print()`, assignment), they MUST NOT occur if short-circuited.
*   **Precedence:** Already handled by the parser. `&&` binds tighter than `||`.

## 4. Lowering Design Options

### Option A: Control-Flow Lowering (Recommended)

Lowering `a && b` to:

```tisc
    ...evaluate a -> regA...
    JZ regA, Label_False
    ...evaluate b -> regB...
    JZ regB, Label_False
    LOADI dest, 1
    JMP Label_End
Label_False:
    LOADI dest, 0
Label_End:
    ...
```

Or slightly optimized:

```tisc
    ...evaluate a -> regA...
    LOADI dest, 0
    JZ regA, Label_End
    ...evaluate b -> regB...
    JZ regB, Label_End
    LOADI dest, 1
Label_End:
```

### Option B: Eager Evaluation (Rejected)

Evaluating both sides and using bitwise ops (`BitAnd`/`BitOr`) violates the short-circuiting requirement and side-effect guarantees.

## 5. Test Plan Requirements

*   **Parser Precedence:** Verify `a || b && c` parses as `a || (b && c)`. (Already covered by `t81_parser_regression_audit_test`).
*   **IR Shape:** Verify that lowering produces `JZ` / `JNZ` / `JMP` instructions and labels, not just arithmetic/bitwise ops.
*   **Runtime Semantics (Short-circuit):**
    *   Test case: `false && { side_effect(); true }`. Ensure side effect does NOT happen.
    *   Test case: `true || { side_effect(); false }`. Ensure side effect does NOT happen.
*   **Type Diagnostics:** Ensure `1 && 2` (integers) is rejected by `SemanticAnalyzer` (unless we decide to allow it, but currently it requires Bools).

## 6. Acceptance Criteria

*   `IRGenerator::visit(const BinaryExpr&)` handles `TokenType::AmpAmp` and `TokenType::PipePipe`.
*   Lowering generates correct control flow (labels/jumps).
*   Short-circuiting behavior is verified by tests with side effects.
*   No "Unsupported binary operator" errors for valid logical expressions.

## 7. Non-Goals

*   Boolean algebra optimization (e.g., `!!a` -> `a`, De Morgan's laws) is not required for the initial implementation.
*   Changing parser precedence.

## 8. Recommended Next Steps

1.  Create a reproduction test case that fails with "Unsupported binary operator" for `&&` / `||`.
2.  Implement `TokenType::AmpAmp` handling in `IRGenerator`:
    *   Allocate result register.
    *   Emit evaluation of LHS.
    *   Emit `JZ` to short-circuit label (load 0/1 to dest).
    *   Emit evaluation of RHS.
    *   Emit logic to combine/select result.
    *   Emit labels.
3.  Implement `TokenType::PipePipe` similarly (using `JNZ` or inverted logic).
4.  Enable the reproduction test and verify passing.
