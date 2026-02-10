# Contributing to T81 Foundation

Thanks for contributing to T81 Foundation.

This repository is **spec-first** and **determinism-first**. Changes must preserve canonical behavior and reproducible outputs.

______________________________________________________________________

## 1. Ground Rules

- Specs in `spec/` are normative.
- Do not change normative spec language (`MUST`, `SHOULD`, etc.) without an RFC.
- Keep behavior deterministic; no hidden non-determinism.
- Add tests for any behavior change.

______________________________________________________________________

## 2. Contribution Workflow

1. Open an issue (bug, clarification, or RFC proposal).
2. Discuss scope and acceptance criteria.
3. Open a PR linked to the issue.
4. Run required local ritual before requesting review.
5. Address review feedback; merge when checks are green.

______________________________________________________________________

## 3. Required Local Ritual

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Single-threaded safe mode:

```bash
cmake --build build --parallel 1
ctest --test-dir build --output-on-failure -j1
```

Optional extended suite:

```bash
ctest --test-dir build -R "fuzz|property|axion" --schedule-random
```

See `docs/ci.md` for full CI parity commands.

______________________________________________________________________

## 4. RFC Process (Spec Changes)

For spec/governance changes:

- Add/update RFC under `spec/rfcs/`.
- Link RFC in PR.
- Keep behavior and wording changes traceable.

______________________________________________________________________

## 5. Code and Docs Expectations

- Public API: `include/t81/`
- Implementation: `src/`
- Tests: `tests/cpp/`
- Docs: `docs/`

When changing code:
- update tests,
- update relevant docs,
- keep examples runnable.

______________________________________________________________________

## 6. Determinism and Safety

All contributions must preserve:

- encode/decode round-trip invariants,
- overflow trap behavior (Axion-visible),
- canonical serialization/replay behavior,
- reproducibility gates (T81Lang/T3_K, where applicable).

______________________________________________________________________

## 7. License

By contributing, you agree your changes are licensed under the repository license terms.
