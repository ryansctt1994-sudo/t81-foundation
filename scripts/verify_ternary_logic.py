#!/usr/bin/env python3
import subprocess
import sys
import os

def main():
    print("--- T81 Ternary Logic Verification ---")

    repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    t81_bin = os.path.join(repo_root, "build", "t81")
    test_file = os.path.join(repo_root, "examples", "ternary_verification.t81")

    if not os.path.exists(t81_bin):
        print(f"Error: {t81_bin} not found. Please build the project first.")
        sys.exit(1)

    print(f"Running {test_file}...")
    try:
        result = subprocess.run([t81_bin, "run", test_file], capture_output=True, text=True)

        if result.returncode == 0:
            print("SUCCESS: All ternary logic invariants verified.")
            sys.exit(0)
        else:
            print("FAILURE: Ternary logic verification failed.")
            print("STDOUT:", result.stdout)
            print("STDERR:", result.stderr)
            sys.exit(result.returncode)
    except Exception as e:
        print(f"Error executing t81: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
