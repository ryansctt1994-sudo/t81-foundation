#!/bin/bash
set -e

# T81 'Go Broad' Killer Demo Reproduction Script
# This script runs the Llama-3.2-1B block demo and verifies the Axion trace.

BUILD_DIR="build"
DEMO_BIN="./${BUILD_DIR}/llama32_demo"

if [ ! -f "$DEMO_BIN" ]; then
    echo "Error: Demo binary not found. Please build the project first."
    exit 1
fi

echo "--- Step 1: Running Llama-3.2-1B Demo and capturing Axion Trace ---"
$DEMO_BIN > demo_output_run1.txt

echo "--- Step 2: Extracting Axion trace for comparison ---"
grep "\[Axion\]" demo_output_run1.txt > trace_run1.txt

echo "--- Step 3: Running second pass to verify determinism ---"
$DEMO_BIN > demo_output_run2.txt
grep "\[Axion\]" demo_output_run2.txt > trace_run2.txt

echo "--- Step 4: Comparing traces ---"
if diff trace_run1.txt trace_run2.txt; then
    echo "SUCCESS: Axion traces are bit-identical and reproducible!"
else
    echo "FAILURE: Axion traces differ between runs!"
    exit 1
fi

echo "--- Step 5: Verifying policy enforcement ---"
if grep -q "SUCCESS: Llama-3.2-1B block inference complete" demo_output_run1.txt; then
    echo "SUCCESS: Policy enforced and inference succeeded."
else
    echo "FAILURE: Inference did not complete successfully."
    exit 1
fi

# Cleanup
rm demo_output_run1.txt demo_output_run2.txt trace_run1.txt trace_run2.txt

echo "--- Llama-3.2-1B Deterministic Story is REPRODUCIBLE and SHAREABLE ---"
