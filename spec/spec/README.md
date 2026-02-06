# Spec Mirror: `/docs/spec/spec`

This mini subtree hosts the [`t81-spec.md`](t81-spec.md) narrative that supplements the main `/spec/` constitution. It exists primarily to keep the published docs site aligned with the deeper specification.

## What lives here
- [`t81-spec.md`](t81-spec.md)—a single, high-level companion piece that ties together the layered spec documents (`spec/t81-data-types.md`, `spec/tisc-spec.md`, etc.) and highlights user-facing summaries.

## How to use it
- Treat this README as your entry point: read [`t81-spec.md`](t81-spec.md) when you want a condensed, web-friendly synopsis after consulting the canonical `/spec/` sources.
- Do **not** edit this file directly unless you are refreshing the rendered site; all normative updates must land in `/spec/` (and, if needed, follow the RFC process in [`spec/rfcs`](../rfcs)).

When rebuilding docs, rerun `cmake --build build --target docs` so `docs/spec/spec/t81-spec.md` stays in sync with the authoritative spec text.
