# Audit Artifacts Policy

This directory stores repository-audit snapshots and summaries used for operational housekeeping.

## What Is Versioned

- Snapshot reports (`YYYY-MM-*-summary.md`)
- Workflow/security/tooling audit outputs
- File inventory snapshots (`repo_tree*.txt`, `repo_inventory.tsv`) when intentionally refreshed

## Regeneration Commands

Run from repository root:

```bash
# Tree snapshots
find . -path ./.git -prune -o -print | sed 's#^\./##' | sort > docs/audits/repo_tree_expanded.txt
find . -path ./.git -prune -o -type d -print | sed 's#^\./##' | sort > docs/audits/repo_tree.txt

# Workflow action pinning audit
python3 scripts/ci/audit_workflow_actions.py --markdown-out docs/audits/2026-02-workflow-action-audit.md
```

## Update Cadence

- Monthly (recommended), or
- On major repository topology/tooling changes

## Review Rule

Audit snapshots are informational, but they must remain deterministic and reviewable:

- No machine-specific absolute paths
- No transient build output
- No secrets or tokens
