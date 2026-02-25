# `docs/audits`

Audit artifacts and reproducibility evidence snapshots.

## Contents
- Workflow/security/tooling audit reports.
- Repository inventory snapshots used for governance checks.
- Historical snapshots may contain legacy paths as recorded at capture time.

## Regeneration
Run from repo root:

```bash
find . -path ./.git -prune -o -print | sed 's#^\./##' | sort > docs/audits/repo_tree_expanded.txt
find . -path ./.git -prune -o -type d -print | sed 's#^\./##' | sort > docs/audits/repo_tree.txt
python3 scripts/ci/audit_workflow_actions.py --markdown-out docs/audits/2026-02-workflow-action-audit.md
```

## Rules
- No secrets/tokens.
- No machine-specific absolute paths.
- Keep artifacts reproducible and reviewable.
