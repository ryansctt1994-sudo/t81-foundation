# `include/t81/frontend`

Public frontend interfaces for T81Lang compilation stages.

## Key Headers
- `ast.hpp`: AST node definitions.
- `lexer.hpp`: lexer/token APIs.
- `parser.hpp`: parser entry points.
- `semantic_analyzer.hpp`: semantic/type analysis APIs.
- `ir_generator.hpp`: frontend-to-TISC IR lowering API.
- `symbol_table.hpp`: scoped symbol table APIs.

## Consumers
- Compiler driver/CLI paths
- Frontend unit/integration tests

## Compatibility
- Keep AST/semantic contracts explicit when evolving language features.
- Update docs/tests together for observable behavior changes.
