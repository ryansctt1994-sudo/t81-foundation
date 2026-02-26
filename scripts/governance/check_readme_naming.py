#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

EXPECTED = {
    "README.md",
    "README.zh-CN.md",
    "README.es.md",
    "README.pt-BR.md",
    "README.ru.md",
}


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    proc = subprocess.run(
        ["git", "ls-tree", "--name-only", "HEAD"],
        cwd=root,
        capture_output=True,
        text=True,
        check=False,
    )
    if proc.returncode != 0:
        print(proc.stdout)
        print(proc.stderr)
        return proc.returncode

    current = {line.strip() for line in proc.stdout.splitlines() if line.strip().startswith("README") and line.strip().endswith(".md")}
    missing = sorted(EXPECTED - current)
    extra = sorted(current - EXPECTED)

    if missing or extra:
        print("README naming check FAILED")
        if missing:
            print("- missing:")
            for m in missing:
                print(f"  - {m}")
        if extra:
            print("- unexpected:")
            for e in extra:
                print(f"  - {e}")
        return 1

    print("README naming check PASSED")
    return 0


if __name__ == "__main__":
    sys.exit(main())
