# Tests Primer

[`tests/`](.) is the regression proof layer mandated by [`AGENTS.md`](../AGENTS.md). Use this README to understand the high-level structure before adding new coverage.

## Overview
- [`tests/cpp/`](cpp/) contains the full suite of gtests validating deterministic semantics, Axion guards, frontend parsing, VM behavior, tensor ops, CLI helpers, and weights tooling. Every new public API or semantic change requires matching coverage here (property tests preferred when possible).
- [`tests/harness/`](harness/) provides the supporting harnesses, scripts, and fixtures used by the C++ suite and the CLI regression jobs. Inspect it before copying fixtures or telemetry snippets.

## Contribution reminders
- When expanding tests, keep Axion logs and canonical artifacts (like `docs/benchmarks.md` entries) updated so the CI trace readers remain reproducible.
- Consult [`AGENTS.md`](../AGENTS.md) for the required build/test commands and rerun `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`, `cmake --build build --parallel`, and `ctest --test-dir build --output-on-failure` after touching this directory.
