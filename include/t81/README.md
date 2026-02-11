# include/t81

Canonical C++ API surface for the T81 Foundation stack.

## Major modules
- `frontend/`: language frontend AST/parser/semantic contracts
- `tisc/` and `ir/`: IR and bytecode interfaces
- `vm/`, `hanoi/`, `axion/`: execution and policy interfaces
- `bigint/`, `tensor/`, `ternary/`, `simd/`: numeric/runtime primitives
- `canonfs/`, `codec/`, `hash/`, `crypto/`, `io/`: persistence and encoding
- `cli/`, `lang/`, `setun/`: tool and bridge-facing surfaces

## Include guidance
- Use narrow includes in implementation code where practical.
- Prefer stable namespace paths and avoid leaking `detail/` internals.
- Treat `core/` compatibility wrappers as migration/adapter lanes, not new feature targets.

## API hygiene
- New public symbols require corresponding tests.
- Keep semantics deterministic and trace-compatible with VM/Axion expectations.
- Document non-obvious behavior in `docs/guides/`.
