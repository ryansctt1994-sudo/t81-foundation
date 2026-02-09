#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CODE_ROOT="$(cd "${ROOT_DIR}/.." && pwd)"
EXAMPLES_DIR="${T81_EXAMPLES_DIR:-${CODE_ROOT}/t81-examples}"
RUNNER="${EXAMPLES_DIR}/scripts/run-runtime-v0.5-e2e.sh"

if [[ ! -x "${RUNNER}" ]]; then
  echo "missing canonical runner: ${RUNNER}" >&2
  echo "expected sibling repo checkout at: ${EXAMPLES_DIR}" >&2
  exit 1
fi

echo "forwarding to canonical runtime demo: ${RUNNER}"
exec "${RUNNER}" "$@"
