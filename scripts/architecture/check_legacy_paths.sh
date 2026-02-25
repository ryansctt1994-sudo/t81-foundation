#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$repo_root"

patterns=(
  'src/data_types'
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
)

search_roots=(README*.md docs scripts tools examples include core kernel lang runtime tooling tests spec pdf)

globs=(
  '!docs/records/audits/**'
  '!docs/records/inventories/**'
  '!book/**'
  '!legacy/**'
  '!notebooks/**'
  '!build/**'
  '!.git/**'
  '!docs/architecture/REPO_RESTRUCTURE_MASTERPLAN.md'
  '!docs/architecture/RESTRUCTURE_PHASE1_PREFLIGHT.md'
  '!scripts/restructure/phase1_scan.sh'
  '!scripts/restructure/verify_restructure_clean.sh'
  '!scripts/architecture/check_legacy_paths.sh'
)

rg_args=()
for glob in "${globs[@]}"; do
  rg_args+=(--glob "$glob")
done

status=0
for p in "${patterns[@]}"; do
  echo "[check] $p"
  if rg -n "$p" "${search_roots[@]}" "${rg_args[@]}"; then
    echo "[fail] deprecated path pattern found: $p"
    status=1
  else
    echo "[ok] no matches"
  fi
  echo
 done

if [[ $status -ne 0 ]]; then
  echo "legacy path scan: FAILED"
  exit 1
fi

echo "legacy path scan: PASSED"
