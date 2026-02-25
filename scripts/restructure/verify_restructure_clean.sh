#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$repo_root"

# Legacy path patterns that should not appear in active files.
patterns=(
  'src/data_types'
  'src/core'
  'include/t81/core'
  'src/tisc'
  'src/vm'
  'src/axion'
  'src/lang'
  'src/frontend'
  'src/cli'
  'src/tools'
  'src/hash'
  'src/jit'
  'src/trace'
  'src/tiers'
  'src/hanoi'
  'src/cog'
  '`/policy/'
)

# Historical artifacts intentionally preserve pre-restructure paths.
exclude_globs=(
  '!docs/records/audits/**'
  '!book/**'
  '!legacy/**'
  '!notebooks/**'
  '!build/**'
  '!.git/**'
  '!docs/architecture/REPO_RESTRUCTURE_MASTERPLAN.md'
  '!docs/architecture/RESTRUCTURE_PHASE1_PREFLIGHT.md'
  '!docs/governance/MIGRATION_MAP.md'
  '!scripts/restructure/phase1_scan.sh'
  '!scripts/restructure/verify_restructure_clean.sh'
  '!scripts/architecture/check_legacy_paths.sh'
)

search_roots=(README*.md docs scripts tools examples include core kernel lang runtime tooling tests spec pdf)

status=0

for pattern in "${patterns[@]}"; do
  echo "[check] pattern: $pattern"
  rg_args=()
  for glob in "${exclude_globs[@]}"; do
    rg_args+=(--glob "$glob")
  done
  if rg -n "$pattern" "${search_roots[@]}" "${rg_args[@]}"; then
    echo "[fail] legacy path references found for pattern: $pattern"
    status=1
  else
    echo "[ok] no matches"
  fi
  echo
 done

if [[ $status -ne 0 ]]; then
  echo "restructure verification: FAILED"
  exit 1
fi

echo "restructure verification: PASSED"
