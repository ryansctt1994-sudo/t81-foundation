# `src/frontend`

Implementation of the T81Lang frontend pipeline.

## Scope
- Lexing and parsing
- Semantic analysis
- IR generation for downstream TISC tooling

## Key Files
- `lexer.cpp`: tokenization.
- `parser.cpp`: AST construction.
- `semantic_analyzer.cpp`: typing/semantic checks.
- `ir_generator.cpp`: AST to TISC IR lowering.
- `symbol_table.cpp`: scoped symbol management.

## Related Interfaces
- `include/t81/frontend/ast.hpp`
- `include/t81/frontend/lexer.hpp`
- `include/t81/frontend/parser.hpp`
- `include/t81/frontend/semantic_analyzer.hpp`

## Notes
- Keep diagnostics stable and actionable.
- Preserve canonical behavior for equivalent source inputs.
