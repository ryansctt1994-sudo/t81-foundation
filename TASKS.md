# 12 Things to Further the T81 Project

Based on the analysis of `advanced_datatypes_showcase.t81` and the current compiler state, here are 12 key tasks to improve the project:

1.  **Implement If-Expressions:** [Done] The parser currently treats `if` only as a statement. Support `if` as an expression (e.g., `let x = if cond { a } else { b };`) to match the example usage and modern language features.
2.  **Implement Block Expressions:** [Done] Support code blocks `{ ... }` as expressions that return the value of their last statement. This is essential for `match` arms and functional-style programming.
3.  **Enhance Tensor/Vector Indexing:** [Done] Fix the semantic analysis for `IndexExpr` when applied to `Tensor` and `Vector` types. Currently, it often fails with "Type does not support indexing" or parser errors on nested literals.
4.  **Fix Generic Parameter Validation:** [Done] Relax the `SemanticAnalyzer` restriction that the first generic parameter must be a Type. Types like `T81Fixed[4, 4]` require integer expressions as parameters.
5.  **Implement Enum Namespaces and Scope Injection:** [Done] Enum variants are injected into scope, and `Enum.Variant` resolution is supported for both constructor-style and constant variants.
6.  **Fix Lexer Dot Consumption:** [Done] The Lexer consumes `.` in identifiers, treating `profile.active` as a single token. This prevents proper parsing of field access expressions. Ensure `.` is tokenized as a Dot operator when appropriate.
7.  **Implement Assignments to Array Indices:** [Done] The parser currently rejects assignments like `arr[i] = val` because assignment targets are restricted to variables. Extend assignment logic to support `IndexExpr` and `FieldAccessExpr`.
8.  **Resolve Type Aliases in Semantic Analysis:** [Done] `SimpleTypeExpr` resolves non-parameterized aliases so underlying type properties (including indexing behavior) are available during semantic checks.
9.  **Implement `T81Fixed` and `T81Complex` Support:** [Partial] Frontend semantic kinds, parsing, and IR lowering hooks now exist; `T81Fixed` has basic numeric/operator flow, while `T81Complex` still needs dedicated runtime representation and VM semantics.
10. **Implement `T81Qutrit` and `T81Uint` Support:** [Partial] These types are now first-class in T81Lang semantic typing and IR lowering with end-to-end coverage for arithmetic-style flows; unsigned semantics now enforce `-T81Uint` rejection and `T81Uint - x` promotion to `T81BigInt`, while deeper VM-native semantics and conversion ergonomics are still open.
11. **Relax Vector Literal Typing:** [Done] `VectorLiteralExpr` now accepts non-numeric element types when element types are consistent.
12. **Improve Error Recovery and Diagnostics:** [Partial] Panic-mode recovery and contextual diagnostics are in place; additional cascading-error recovery and precision improvements are still needed.
