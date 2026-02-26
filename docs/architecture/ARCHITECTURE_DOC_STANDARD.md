# Architecture Documentation Standard

Status: Active  
Last Updated: 2026-02-26

## Purpose

Define mandatory structure and quality gates for files under `docs/architecture/`.

## Required Header Fields

Each architecture file should include:

- `Status`
- `Last Verified (UTC)` or `Last Updated`
- `Maturity` using: `Frozen`, `Stable`, `Experimental`, `Stubbed`

## Required Sections

1. Purpose / scope
2. Implemented vs experimental boundary
3. Key invariants
4. Failure modes (or risk/fault surfaces)
5. `Indeterminate` section for unknown or not-yet-verified areas
6. `Evidence` section (code/spec/governance paths)
7. `Acceptance Criteria` checklist

## Evidence Rules

- Every non-trivial claim must map to at least one source in `Evidence`.
- If evidence is insufficient, mark the claim `Indeterminate` instead of inferring.
- Prefer stable path references over line-specific references unless actively verified in-session.

## Diagram Rules

- Mermaid only (GitHub-renderable).
- Use one primary system view plus optional detailed flow.
- Mark optional/experimental paths with dashed edges where relevant.

## Authority Reminder

All architecture docs must preserve authority order:

`/spec` > `docs/architecture/OVERVIEW.md` > `/docs` > `/book`
