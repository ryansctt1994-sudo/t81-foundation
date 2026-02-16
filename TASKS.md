# 12 Things to Further the T81 Project

Based on the analysis of `advanced_datatypes_showcase.t81` and the current compiler state, here are 12 key tasks to improve the project:

1.  **Implement If-Expressions:** [Done] The parser currently treats `if` only as a statement. Support `if` as an expression (e.g., `let x = if cond { a } else { b };`) to match the example usage and modern language features.
2.  **Implement Block Expressions:** [Done] Support code blocks `{ ... }` as expressions that return the value of their last statement. This is essential for `match` arms and functional-style programming.
3.  **Enhance Tensor/Vector Indexing:** Fix the semantic analysis for `IndexExpr` when applied to `Tensor` and `Vector` types. Currently, it often fails with "Type does not support indexing" or parser errors on nested literals.
4.  **Fix Generic Parameter Validation:** [Done] Relax the `SemanticAnalyzer` restriction that the first generic parameter must be a Type. Types like `T81Fixed[4, 4]` require integer expressions as parameters.
5.  **Implement Enum Namespaces and Scope Injection:** Properly handle Enum scope resolution. Inject variants into the scope or support `Enum.Variant` syntax so that variants like `Off` or `Alert` can be found.
6.  **Fix Lexer Dot Consumption:** [Done] The Lexer consumes `.` in identifiers, treating `profile.active` as a single token. This prevents proper parsing of field access expressions. Ensure `.` is tokenized as a Dot operator when appropriate.
7.  **Implement Assignments to Array Indices:** [Done] The parser currently rejects assignments like `arr[i] = val` because assignment targets are restricted to variables. Extend assignment logic to support `IndexExpr` and `FieldAccessExpr`.
8.  **Resolve Type Aliases in Semantic Analysis:** Ensure `SimpleTypeExpr` correctly resolves type aliases (e.g., `type FixedWindow = Vector[...]`) so that the underlying type properties (like indexing) are available.
9.  **Implement `T81Fixed` and `T81Complex` Support:** Complete the compiler frontend and backend support for these types, including constructors, operators, and type checking.
10. **Implement `T81Qutrit` and `T81Uint` Support:** Add full support for these ternary-native types in the type system and VM.
11. **Relax Vector Literal Typing:** Modify `VectorLiteralExpr` validation to allow non-numeric types (e.g., `Vector[String]`) if the vector's type definition allows it.
12. **Improve Error Recovery and Diagnostics:** Enhance the compiler's error reporting to handle cascading errors better and provide more precise location information for semantic errors.
