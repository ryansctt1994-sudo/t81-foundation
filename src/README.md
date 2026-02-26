# src

Implementation tree for the T81 runtime, compiler/tooling pipeline, and supporting subsystems.

## Structure
- `c_api/`: C-language API bridge surface
- `canonfs/`: CanonFS runtime/storage integration
- `codec/`: canonical encoding/decoding utilities
- `crypto/`: cryptographic support utilities
- `io/`: runtime I/O helpers and adapters
- `python/`: Python binding module sources
- `setun/`: set/union support primitives
- `simd/`: SIMD helper implementations
- `tensor/`: tensor runtime operations and helpers
- `main.c`, `t81_core.h`: entry/header integration stubs used by specific build targets
- `c_api/`, `python/`: external language bindings

## Design constraints
- Preserve deterministic semantics across runtime paths.
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
