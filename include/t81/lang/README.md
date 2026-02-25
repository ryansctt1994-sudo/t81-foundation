# `include/t81/lang`

Legacy/minimal language front-end interfaces and canonical numeric formatting helpers.

## Key Headers
- `ast.hpp`: minimal AST model.
- `parser.hpp`: parser API (`parse_module`) and `ParseError` enum.
- `compiler.hpp`: module-to-`tisc::Program` compile API and `CompileError` enum.
- `types.hpp`: language type definitions.
- `numeric_format.hpp`: canonical textual formatting for T81 numeric types.

## Positioning
- This namespace provides lightweight language interfaces.
- Mainline C++ frontend pipeline lives under `include/t81/frontend` and `lang/frontend`.

## Notes
- Keep canonical format output stable for reproducibility gates.
- Document behavior differences if APIs in `t81::lang` and `t81::frontend` diverge.
