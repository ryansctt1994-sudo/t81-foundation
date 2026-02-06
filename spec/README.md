# Documentation-spec Mirror

This folder duplicates the canonical specification for delivery to the static site. The authoritative texts live under `/spec/`, so treat this subtree as an `npm run docs` artifact that should stay in sync with the constitution (see [`spec/index.md`](../spec/index.md)).

## Contents
- [`axion-kernel.md`](axion-kernel.md), [`tisc-spec.md`](tisc-spec.md), [`t81vm-spec.md`](t81vm-spec.md), [`t81lang-spec.md`](t81lang-spec.md), [`t81-data-types.md`](t81-data-types.md), and [`cognitive-tiers.md`](cognitive-tiers.md) match the corresponding files in `/spec/`. Use them for quick browsing on the rendered site.
- [`constitution.md`](constitution.md), [`cpp-mapping.md`](cpp-mapping.md), [`canonfs-spec.md`](canonfs-spec.md), [`hanoi-kernel-spec.md`](hanoi-kernel-spec.md), and [`t81-overview.md`](t81-overview.md) provide supplemental context; defer to the upstream `/spec/` version for normative decisions.
- [`rfcs/`](rfcs/) mirrors `spec/rfcs/` for publications but should not be edited here directly. Always edit `spec/rfcs/*` and regenerate `docs` as part of the build.
- The [`spec/`](spec/) subdirectory inside this folder currently mirrors the same `spec/` tree — treat it as a navigation convenience for documentation consumers.

## Keeping it aligned
- When a spec file changes, rerun `cmake --build build --target docs` to regenerate the `docs/spec/*` set or update it manually while confirming parity.
- Do not introduce new normative text here without corresponding `/spec/` updates and, if necessary, an RFC (see `spec/rfcs/template.md`).
