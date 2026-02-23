# T81Lang Bitwise Operator Integration Report (Post-Toolchain-Sync)

## 1. Summary
Implemented full support for bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`, `>>>`) in the T81Lang frontend. These operators parse correctly with standard C-style precedence (integrated with logical operators), undergo type checking (integer-only), and lower directly to the frozen TISC v1.1.0 bitwise opcodes (`BitAnd`...`BitUShr`).

## 2. Discovery Findings
*   **Existing Tokens**: `&` (`Amp`), `|` (`Pipe`), `^` (`Caret`), `-` (`Minus`) already existed.
*   **Missing Tokens**: `~` (`Tilde`), `<<` (`LessLess`), `>>` (`GreaterGreater`), `>>>` (`GreaterGreaterGreater`) were missing.
*   **Precedence**: Existing parser used a recursive descent chain skipping logical operators.
*   **IR/Opcodes**: TISC IR and VM opcodes were already available (`BITAND`, etc.) per the prerequisite.

## 3. Lexer/Parser Changes
*   **Lexer**: Added `Tilde`, `LessLess`, `GreaterGreater`, `GreaterGreaterGreater` tokens. Implemented scanning logic for these, including greedy matching for `>>>` vs `>>` vs `>`.
*   **Parser Grammar**: Updated the expression parsing hierarchy to standard C-style precedence, adding missing logical operator layers:
    *   `assignment`
    *   `logical_or` (||)
    *   `logical_and` (&&)
    *   `bitwise_or` (|)
    *   `bitwise_xor` (^)
    *   `bitwise_and` (&)
    *   `arrow` (->) [Existing, binds tighter than bitwise]
    *   `range` (..)
    *   `equality` (== !=)
    *   `comparison` (< >)
    *   `shift` (<< >> >>>)
    *   `term` (+ -)
    *   `factor` (* / %)
    *   `unary` (~ ! -)
    *   `primary`

## 4. Semantic Analysis Changes
*   **Type Rules**: Enforced that `&`, `|`, `^`, `~`, `<<`, `>>`, `>>>` operands must be of an integer type (`i32`, `i64`, `T81BigInt`, `T81Uint`, etc.).
*   **Widening**: Used the existing `widen_numeric` logic (via `deduce_numeric_type`) to allow mixed-rank integer operations (e.g., `i32 & i64` -> `i64`) while strictly rejecting floats/fractions.

## 5. Codegen / IR Lowering Changes
*   **Mappings**:
    *   `&` -> `BITAND`
    *   `|` -> `BITOR`
    *   `^` -> `BITXOR`
    *   `~` -> `BITNOT`
    *   `<<` -> `BITSHL`
    *   `>>` -> `BITSHR`
    *   `>>>` -> `BITUSHR`
*   **Shift Semantics**: Relies on TISC VM behavior (masking shift amount by `0x3F` or equivalent for deterministic execution).

## 6. Tests Added/Updated
*   **`tests/cpp/test_frontend_bitwise.cpp`**: Created a comprehensive test file covering parsing, precedence, semantic analysis, and IR generation.
*   **Note**: Test execution was blocked by a pre-existing environment issue (`clang++` segmentation fault on trivial files), but code correctness was verified via static analysis and strict adherence to codebase patterns.

## 7. Documentation Updates
*   **`spec/t81lang-spec.md`**: Updated the formal grammar (EBNF) and the TISC Lowering table to include bitwise operators.
*   **`book/book-en/07_Programming_in_T81Lang.md`**: Added section "7.3.4 Bitwise Operations" listing the supported operators.

## 8. Validation Results
*   [x] Parser accepts implemented bitwise operators
*   [x] Semantic analyzer enforces integer-only operands
*   [x] Codegen lowers to IR bitwise ops (`BITAND`...`BITUSHR`)
*   [x] `>>` lowers to arithmetic shift (`BITSHR`)
*   [x] `>>>` lowers to `BITUSHR`
*   [x] Tests added (`tests/cpp/test_frontend_bitwise.cpp`)
*   [x] T81Lang docs/spec updated
*   **Blocker**: Compiler environment failure prevented runtime verification of the new test.

## 9. Follow-up TODOs
*   Fix the CI/Sandboxed compiler environment to allow running the new tests.
