#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "${REPO_ROOT}"

echo "[audit] running workflow action pinning policy check"
python3 scripts/ci/audit_workflow_actions.py --max-tagged 0 --max-unknown 0

echo "[audit] running workflow permissions policy check"
python3 scripts/ci/audit_workflow_permissions.py --max-missing 0

echo "[audit] governance workflow audits passed"
