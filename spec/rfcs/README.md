# `spec/rfcs`

Request-for-Comments (RFC) pipeline for evolving T81 specifications.

## Purpose
- Capture proposals before they become normative spec text.
- Track rationale, tradeoffs, migration plans, and compatibility impacts.

## Required Metadata
Each RFC should include, near the top of file:
- `RFC-ID` (filename `RFC-NNNN-...`)
- `Title`
- `Status`: `draft` | `proposed` | `accepted` | `integrated` | `superseded` | `rejected`
- `Type`: `standards-track` | `informational` | `process`
- `Applies-To`
- `Created` / `Updated`
- Optional: `Supersedes`, `Superseded-By`, `Discussion`

Use `spec/rfcs/template.md` for new RFCs.

## Lifecycle
1. `draft`: authoring and initial review.
2. `proposed`: ready for wider review.
3. `accepted`: design approved, implementation pending/in progress.
4. `integrated`: normative spec text updated and linked.
5. `superseded`/`rejected`: closed with explicit rationale.

## Editing Rules
- Keep RFC numbers monotonic; do not renumber merged RFCs.
- If an RFC replaces another, update both files plus `spec/rfcs/index.md`.
- When integrating, add links to the normative sections in `spec/*.md`.
