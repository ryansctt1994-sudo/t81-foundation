#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build"
DEMO="${BUILD_DIR}/llama32_demo"

if [[ ! -x "${DEMO}" ]]; then
  echo "error: ${DEMO} not found; build the project first."
  exit 1
fi

echo "=== Running Llama-3.2 Demo (Run 1) ==="
OUT=$("${DEMO}")
echo "$OUT"

echo "=== Verifying Determinism (Run 2) ==="
OUT2=$("${DEMO}")

VALS1=$(echo "$OUT" | grep "First 3 elements:" | cut -d: -f2)
VALS2=$(echo "$OUT2" | grep "First 3 elements:" | cut -d: -f2)

if [[ "$VALS1" == "$VALS2" ]]; then
  echo "SUCCESS: Output values are deterministic."
else
  echo "FAILURE: Output values are NOT deterministic!"
  echo "Run 1: $VALS1"
  echo "Run 2: $VALS2"
  exit 1
fi

COUNT1=$(echo "$OUT" | grep "total" | awk '{print $3}')
COUNT2=$(echo "$OUT2" | grep "total" | awk '{print $3}')

if [[ "$COUNT1" == "$COUNT2" ]]; then
  echo "SUCCESS: Axion trace event count is deterministic ($COUNT1 events)."
else
  echo "FAILURE: Axion trace event count mismatch!"
  exit 1
fi

T1=$(mktemp)
T2=$(mktemp)
echo "$OUT" | grep -v "Inference time" > "$T1"
echo "$OUT2" | grep -v "Inference time" > "$T2"

if diff "$T1" "$T2"; then
    echo "SUCCESS: Full output (minus timing) is bit-identical."
else
    echo "FAILURE: Non-timing output differs!"
    rm "$T1" "$T2"
    exit 1
fi
rm "$T1" "$T2"
