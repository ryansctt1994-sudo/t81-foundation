# `tests/fixtures`

Static test inputs used across C++ and Python tests.

## Contents
- Source snippets (`.t81`, `.t81lang`) for parser/semantic/compiler coverage.
- Binary/IR fixtures (`.tisc`, related artifacts) for loader/disasm/runtime checks.

## Rules
- Fixtures must be deterministic and portable.
- Prefer small, focused inputs over large bundled scenarios.
- Name fixtures by behavior under test, not by implementation detail.
