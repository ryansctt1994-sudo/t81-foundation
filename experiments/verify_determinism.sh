#!/bin/bash
# experiments/verify_determinism.sh

T81_BIN="./build/t81"
OUT_DIR="experiments/output"
mkdir -p "$OUT_DIR"

echo "=== T81Lang Determinism Verification Run ==="

verify_experiment() {
    local src="$1"
    local name=$(basename "$src" .t81)
    local tisc="experiments/${name}.tisc"
    local out1="$OUT_DIR/${name}_run1.txt"
    local out2="$OUT_DIR/${name}_run2.txt"

    echo "---------------------------------------------------"
    echo "Experiment: $name"

    # Compile (once)
    echo "  Compiling..."
    $T81_BIN compile "$src" -o "$tisc" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "  Compilation FAILED."
        return 1
    fi

    # Run 1
    echo "  Run 1..."
    $T81_BIN run "$tisc" > "$out1" 2>&1

    # Run 2
    echo "  Run 2..."
    $T81_BIN run "$tisc" > "$out2" 2>&1

    # Compare
    if diff -q "$out1" "$out2" > /dev/null; then
        echo "  [PASS] Deterministic Output Verified."
        # Calculate hash of output
        local h=$(sha256sum "$out1" | awk '{print $1}')
        echo "  Output SHA256: $h"
    else
        echo "  [FAIL] Output Mismatch!"
        diff "$out1" "$out2"
        return 1
    fi
}

verify_experiment "experiments/primitive_arithmetic.t81"
verify_experiment "experiments/symbolic_algebraic.t81"
verify_experiment "experiments/container_structural.t81"
verify_experiment "experiments/numerical_tensor.t81"
verify_experiment "experiments/monadic_flow.t81"
verify_experiment "experiments/reflective_layer.t81"
verify_experiment "experiments/cross_layer.t81"

echo "---------------------------------------------------"
echo "Verification Complete."
