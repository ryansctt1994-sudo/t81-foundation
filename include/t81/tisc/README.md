# `include/t81/tisc`

Public interfaces for TISC IR and binary program handling.

## Key Headers
- `ir.hpp`: IR model (instructions, operands, blocks/program).
- `opcodes.hpp`: opcode enumeration and opcode helpers.
- `program.hpp`: loaded/runtime program structure.
- `binary_emitter.hpp`: IR-to-binary emission interface.
- `binary_io.hpp`: binary read/write API.
- `pretty_printer.hpp`: human-readable rendering API.
- `encoding.hpp`: encoding-related constants/helpers.

## Consumers
- Frontend IR generation (`src/frontend`)
- Emitter/IO implementation (`src/tisc`)
- VM loading/execution (`src/vm`)

## Compatibility
- Preserve deterministic serialization guarantees.
- Keep on-disk format updates versioned and explicit.
