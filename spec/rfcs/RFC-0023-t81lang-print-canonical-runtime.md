______________________________________________________________________

# RFC-0023 — T81Lang `print` Canonical Runtime Surface

Version 0.1 — Draft\
Status: Draft\
Author: T81 Foundation\
Applies to: T81Lang, TISC, T81VM

vote: +1

______________________________________________________________________

# 0. Summary

This RFC proposes a formal runtime contract for the T81Lang `print(...)`
builtin:

1. Lower `print(expr)` to a dedicated opcode path.
2. Capture output in deterministic VM state (`printed_output`), not host stdout.
3. Define canonical string formatting for scalar values and supported handles.

______________________________________________________________________

# 1. Motivation

T81Lang already type-checks `print(...)`, but reproducibility requires runtime
observability to be deterministic and testable. Host-side IO is environment
dependent and cannot be a verification surface.

By making `print` part of the VM state transition, tests and auditors can
compare exact output sequences across platforms.

______________________________________________________________________

# 2. Design / Specification

### 2.1 Lowering

- Frontend IR introduces `PRINT`.
- Binary emitter maps `PRINT -> t81::tisc::Opcode::Print`.

### 2.2 Runtime Sink

- VM appends rendered strings to `State::printed_output`.
- `Opcode::Print` MUST NOT require host stdout side effects.

### 2.3 Rendering Rules (Initial Surface)

- `Int` -> decimal string (`42`)
- `Bool` -> `true` / `false`
- `FloatHandle` -> canonical decimal with `t81` suffix
- `FractionHandle` -> `num/den` with `t81` suffix
- `SymbolHandle` -> symbol text

Unsupported/invalid handle combinations trap deterministically.

______________________________________________________________________

# 3. Rationale

- Keeps runtime deterministic and replayable.
- Enables e2e tests without process-level stdout capture.
- Preserves a strict boundary between VM semantics and host integration.

______________________________________________________________________

# 4. Backwards Compatibility

- Existing programs remain valid.
- `print` now has a stable runtime target for conformance testing.

______________________________________________________________________

# 5. Security Considerations

- Avoids host IO channels in core execution paths.
- Output remains part of canonical VM state, simplifying audit/replay.

______________________________________________________________________

# 6. Open Questions

1. Should enum/option/result text rendering become normative in this RFC or a
   follow-on RFC?
2. Should CLI expose `printed_output` directly in deterministic trace mode?
