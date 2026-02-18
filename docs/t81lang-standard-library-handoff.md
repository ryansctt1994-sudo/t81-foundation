# T81Lang Standard Library Handoff

This document is the execution handoff for continuing the standard library plan with another AI agent.

## 1. Current State

Implemented and validated end-to-end (semantic + IR + VM + CLI coverage):
- `std.core`: `assert`, `debug`, `unwrap_or`
- `std.math`: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `sinh`, `cosh`, `tanh`, `sqrt`, `exp`, `log`, `pow`, `clamp`
- `std.io`: `println`, `print_int`, `print_float`, `stream`, `net` (`stream/net` currently lower to stable symbolic tokens)
- `std.text`: `str_len`, `str_is_empty`, `concat`, `starts_with`, `ends_with`, `contains`, `index_of`, `replace`, `to_string`, `from_bytes`, `split`, `join`
- `std.bytes`: `len`, `is_empty`, `concat`, `starts_with`, `ends_with`, `contains`, `index_of`, `replace`, `to_string`, `from_string`, `split`, `join`, `T81Bytes(...)`
- `std.collections`: `len`, `is_empty`, `first`, `last`, `push`, `pop`, `list`, `map`, `set`, `tree`, `graph` (all five constructors currently return deterministic empty runtime-backed vector values)
- `std.symbol`: `intern`, `to_string`, `eq`, `ne`
- `std.sys`: `exit`, `time`, `entropy`, `proof` (`proof` currently lowers to stable symbolic token)
- `std.async`: `yield`, `sleep`, `thread`, `promise` (`thread/promise` currently lower to stable symbolic tokens)
- `std.tensor`: `load`, `from_list`, `matmul`, `vec_add`
- `std.agent`: `self_reflect`
- `std.sys` / `std.io` / `std.async`: fixture-driven CLI goldens for runtime observable behavior are now present under `tests/fixtures/t81lang_std_runtime/*` and `tests/cpp/cli_std_runtime_fixtures_test.cpp`

Generic function work now supported:
- Generic declarations: `fn id[T](x: T) -> T`
- Inferred calls: `id(7)`
- Explicit calls: `id[i32](7)`
- Partial explicit calls with inference fallback: `first[i32](7, "tail")`
- Deterministic unresolved inference diagnostics for unbound generics:
  - Example: `fn none_of[T]() -> Option[T] { return None; }` then `none_of()`
  - Diagnostic: `Cannot infer generic parameter 'T' for function 'none_of'.`
  - Example: `fn none_pair[T, U]() -> Option[Result[T, U]] { return None; }` then `none_pair()`
  - Diagnostic: `Cannot infer generic parameters 'T', 'U' for function 'none_pair'.`

## 2. Files Most Recently Touched

Core implementation:
- `src/frontend/semantic_analyzer.cpp`
- `include/t81/frontend/ir_generator.hpp`

Coverage:
- `tests/cpp/t81lang_conformance_baseline_test.cpp`
- `tests/cpp/semantic_analyzer_generic_test.cpp`
- `tests/cpp/frontend_parser_generics_test.cpp`
- `tests/cpp/frontend_ir_generator_test.cpp`
- `tests/cpp/cli_check_test.cpp`
- `tests/cpp/cli_std_runtime_fixtures_test.cpp`
- `tests/fixtures/t81lang_std_collections/02_generic_inference.t81`
- `tests/fixtures/t81lang_std_collections/02_generic_inference.out`
- `tests/fixtures/t81lang_std_collections/README.md`
- `tests/fixtures/t81lang_std_runtime/01_tokens.t81`
- `tests/fixtures/t81lang_std_runtime/01_tokens.out`
- `tests/fixtures/t81lang_std_runtime/02_time_entropy_async.t81`
- `tests/fixtures/t81lang_std_runtime/02_time_entropy_async.out`
- `tests/fixtures/t81lang_std_runtime/README.md`

Tracking/docs:
- `docs/standard-library.md`
- `docs/t81lang-standard-library-plan.md`
- `TASKS.md`

## 3. Required Validation Gate

Always run before proposing commit/PR:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Current known good baseline: full suite passing (`211/211`).

## 4. What Is Next (Priority Order)

1. Replace vector-placeholder constructor semantics with full runtime semantics for:
   - `std.collections.list/map/set/tree/graph`
   - keyed/tree/graph-specific deterministic data models and operations
2. Keep fixture-driven CLI goldens for each new module before marking complete.
3. Replace symbolic-token aliases with full typed runtime objects for:
   - `std.sys.proof`
   - `std.io.stream/net`
   - `std.async.thread/promise`

## 5. Determinism/Quality Rules To Preserve

- No hidden nondeterminism.
- Deterministic diagnostics for type/arity/semantic failures.
- Do not weaken existing tests; add coverage for every behavior change.
- Keep frontend/IR/VM behavior aligned (no frontend-only aliases without runtime story).

## 6. Suggested Commit Chunking

For cleaner handoff and review, keep future changes in small chunks:
1. Semantics change
2. IR/runtime lowering
3. CLI fixture + golden
4. Docs + TASKS status update

## 7. Fast Resume Checklist For New AI

1. Read:
   - `docs/t81lang-standard-library-plan.md`
   - `docs/standard-library.md`
   - `TASKS.md`
2. Run full tests once to confirm baseline.
3. Pick next unimplemented module and define:
   - semantic contract
   - IR opcode mapping
   - VM behavior
   - conformance + fixture tests
4. Implement in vertical slice and re-run full gate.
