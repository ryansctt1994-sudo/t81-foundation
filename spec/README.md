# `spec` (Canonical)

This directory is the authoritative specification source for the T81 stack.

## Scope
- Normative architecture/specification documents (`t81lang`, `tisc`, `t81vm`, Axion, data types, etc.).
- RFC proposals and their lifecycle (`spec/rfcs`).
- Supplemental standards-track specs (for example CanonFS/Hanoi).

## Canonical Rule
- Edit normative text in `spec/` first.
- Generated or published documentation mirrors (for example `docs/spec`) must not diverge from this directory.

## Navigation
- Start with `spec/index.md`.
- RFC process and templates live in `spec/rfcs/README.md` and `spec/rfcs/template.md`.
- Historical long-form narrative lives in `spec/spec/t81-spec.md` and is non-normative unless explicitly promoted.

## Update Workflow
1. Update target spec or RFC in `spec/`.
2. Update indexes (`spec/index.md`, `spec/rfcs/index.md`) when status/links change.
3. Run docs generation/publish workflow if needed.
