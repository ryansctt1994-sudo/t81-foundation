# Manifesto Guide Map

The guides here are the portal annexes referenced by the Manifesto & Portal README: each article expands a ledgered workflow (CLI ops, Axion observability, language semantics, weights, release hygiene) so readers can execute deterministic tasks with full traceability.

# Guide Directory Map

This directory contains the living set of `[docs/guides/*](.)` articles that describe how to work with the T81 CLI, Axion policies, vector literals, and related release tooling. Treat this README as the table of contents before opening individual markdown files.

## Active categories
- **CLI & automation** – `cli-user-manual.md` and `cli-toolkit.md` cover day-to-day commands plus reusable scripts.
- **Axion observability** – `[axion-trace.md](axion-trace.md)`, `[axion-policy-manual.md](axion-policy-manual.md)`, `[axion-tracing-manual.md](axion-tracing-manual.md)`, and `[runtime-observability-manual.md](runtime-observability-manual.md)` show how to capture policies, interpret verdicts, and keep logs paired with research artifacts.
- **Language & semantics** – `[data-types-overview.md](data-types-overview.md)`, `[match-patterns.md](match-patterns.md)`, `[match-example.md](match-example.md)`, `[vector-literals.md](vector-literals.md)`, `[vm-opcodes.md](vm-opcodes.md)`, and `[adding-a-language-feature.md](adding-a-language-feature.md)` set expectations for deterministic semantics and DSL evolution.
- **Language determinism gates** – `[t81lang-print-runtime.md](t81lang-print-runtime.md)` and `[t81lang-determinism-gates.md](t81lang-determinism-gates.md)` document canonical print formatting, compile-twice bytecode identity checks, and runtime output reproducibility gates.
- **Weights & public APIs** – `[weights-integration.md](weights-integration.md)`, `[public-api-overview.md](public-api-overview.md)`, and `[record-enum.md](record-enum.md)` explain tensor bundles, canonical hashes, and header-level conventions.
- **Research & release hygiene** – `[demo-gallery.md](demo-gallery.md)` and `[release-audit-manual.md](release-audit-manual.md)` keep demonstrations and governance notes centralized.

## Staying oriented
- Use this README with `[docs/navigation.md](../navigation.md)` for a broader site map and quick pointers to specs, onboarding instructions, and directories that Axion audits treat as mandatory.
- When working with Axion content, open `docs/guides/axion/README.md` first so you understand which trace/policy guide to pick and how enforcement vs. observability modes differ.
- When you archive or replace guides, put the legacy version in `docs/` with an `.archived` suffix and mention it here so the next reader immediately knows why it is no longer active.

## Need more context?
- Search the site index at `docs/search/` or open `docs/api/html/index.html` after running `cmake --build build --target docs`.
- For big changes that touch semantics/automations, reference `AGENTS.md`, `spec/index.md`, and the RFC process in `spec/rfcs/template.md` before modifying any guide content.
