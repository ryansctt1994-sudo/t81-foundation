# docs

Documentation hub for T81 Foundation architecture, operations, and user/developer guides.

## What lives here
- Core references: `ci.md`, `system-status.md`, `release.md`, `navigation.md`
- Guides: `guides/` (CLI, Axion, weights, runtime observability, feature workflows)
- Research and design: `research-guide.md`, `jit-research.md`, `hardware-roadmap.md`
- Audits and governance artifacts: `audits/`, `proposals/`, `archive/`
- Site/Jekyll scaffolding: `_layouts/`, `_includes/`, `assets/`, `index.md`

## High-signal entry points
- `docs/ai-quickstart.md`
- `docs/ci.md`
- `docs/runtime-semantics-boundary.md`
- `docs/guides/cli-user-manual.md`
- `docs/guides/weights-integration.md`

## Update rules
- Keep docs aligned with actual command surfaces and CI behavior.
- When changing public behavior, update relevant guide(s) in the same PR.
- Prefer linking to authoritative files (`ARCHITECTURE.md`, `TASKS.md`, workflow files) rather than duplicating details.

## Validation
Useful checks before merging doc-heavy changes:
```bash
python3 scripts/ci/check_architecture_targets.py
python3 scripts/check-runtime-contract-sync.py
```
