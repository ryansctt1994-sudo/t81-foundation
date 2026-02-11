# src

Implementation tree for the T81 runtime, compiler/tooling pipeline, and supporting subsystems.

## Structure
- `frontend/`: lexer, parser, semantic analyzer
- `tisc/`: IR representation, binary encoding/IO, disassembly plumbing
- `vm/`: HanoiVM interpreter, trace-JIT, execution state/traps
- `axion/`: policy engine, verdicts, runtime safety hooks
- `tensor/`, `simd/`, `bigint/`: numeric and kernel implementations
- `canonfs/`, `codec/`, `hash/`, `crypto/`, `io/`: storage/encoding/support layers
- `cli/`: command orchestration and diagnostics surfaces
- `c_api/`, `python/`: external language bindings

## Design constraints
- Preserve deterministic semantics across interpreter and JIT paths.
- Keep policy and trace behavior equivalent across optimization paths.
- Route public contracts through headers in `include/t81/`.

## When editing here
- Add/adjust tests in `tests/cpp/` for behavior changes.
- Re-run build/test ritual:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```
