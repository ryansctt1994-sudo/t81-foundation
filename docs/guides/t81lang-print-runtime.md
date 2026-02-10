# T81Lang `print(...)` Runtime Behavior

This guide documents the current deterministic runtime behavior of the T81Lang
`print(...)` builtin.

## Scope

- Frontend semantic contract: `src/frontend/semantic_analyzer.cpp`
- Frontend IR lowering: `include/t81/frontend/ir_generator.hpp`
- TISC opcode mapping: `src/tisc/binary_emitter.cpp`
- VM execution: `src/vm/vm.cpp`

## Contract

- `print` accepts exactly one argument.
- Accepted argument categories in semantic analysis:
  - scalar numeric
  - `bool`
  - `T81String`
- `print` returns `void`.

## Execution Model

The compiler lowers `print(x)` to a dedicated `PRINT` IR opcode and then to
`t81::tisc::Opcode::Print`.

At runtime, `Opcode::Print` does not write to stdout directly. Instead, it
appends a deterministic textual representation to:

- `t81::vm::State::printed_output`

This makes output verifiable in tests and reproducible across runs.

## Canonical Output Forms

Current VM formatting rules:

- Integer register value: decimal string (example: `42`)
- Boolean: `true` or `false`
- Float handle: locale-stable decimal with `t81` suffix (example: `1.25t81`)
- Fraction handle: `num/den` with `t81` suffix (example: `22/7t81`)
- Symbol/string handle: symbol text (example: `alpha`)
- Structured handles (tensor/shape/weights/reflection/enum): deterministic
  tagged placeholders (for debugging/traceability)

If a register/tag pair is invalid for formatting, the VM raises a deterministic
`TypeFault`.

## Test Coverage

- IR lowering check:
  - `tests/cpp/frontend_ir_generator_test.cpp`
- Binary emitter opcode mapping:
  - `tests/cpp/tisc_binary_emitter_test.cpp`
- VM print behavior and fault case:
  - `tests/cpp/vm_print_test.cpp`
- End-to-end frontend compile -> VM execution print capture:
  - `tests/cpp/e2e_print_runtime_test.cpp`
