# T81Lang Determinism Gates

This guide defines the practical determinism checks used for T81Lang compile
and runtime behavior.

## Why these gates exist

A deterministic language runtime requires both of these to hold:

- identical source -> identical bytecode
- identical bytecode + inputs -> identical runtime-visible output

The current gates enforce both properties directly in tests.

## Current gate set

### 1) Compile-twice bytecode identity

Test: `tests/cpp/e2e_compile_determinism_test.cpp`

The gate compiles one source program twice using:

- `t81::cli::build_program_from_source(...)`

Then it verifies:

- `t81::tisc::encode(program_a) == t81::tisc::encode(program_b)`

### 2) Compile-twice hash identity

In the same test, bytecode from each compile pass is hashed with:

- `t81::crypto::sha3_512_hex(...)`

The two digests must be exactly equal.

### 3) Runtime printed output identity

The same program is executed twice in the interpreter VM, and both runs must
produce identical:

- `t81::vm::State::printed_output`

The fixture also asserts expected canonical output values.

### 4) Golden fixture pack

The compile/runtime gate is exercised over a fixture set in:

- `tests/fixtures/t81lang_determinism/*.t81`
- `tests/fixtures/t81lang_determinism/*.out`

Each fixture enforces:

- compile pass A == compile pass B (bytecode)
- SHA3-512 hash(pass A) == hash(pass B)
- run output(pass A) == run output(pass B) == golden `.out`

## Local verification commands

Run the full repository ritual:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Run only determinism-focused tests:

```bash
ctest --test-dir build -R "e2e_compile_determinism_test|e2e_print_runtime_test|t81_vm_print_test" --output-on-failure
```

Run the CLI reproducibility script used in CI:

```bash
python3 scripts/ci/t81lang_repro_gate.py \
  --t81-bin build/t81 \
  --fixtures-dir tests/fixtures/t81lang_determinism \
  --workdir build/t81lang-repro \
  --hash-out build/t81lang-repro/hash.txt
```

## Interpretation

- Pass: compile + runtime determinism contract holds for this fixture.
- Fail at byte equality: compiler pipeline emitted non-canonical program data.
- Fail at hash equality with byte mismatch: same root cause as above.
- Fail at printed output equality only: runtime formatting or value-tag handling
  diverged while bytecode remained stable.

## Related docs

- `docs/guides/t81lang-print-runtime.md`
- `docs/proposals/t81lang-implementation-plan.md`
- `spec/t81lang-spec.md`
