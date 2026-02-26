#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

FORBIDDEN_SUBSTRINGS = [
    "__pycache__/",
    ".DS_Store",
    "Thumbs.db",
]
FORBIDDEN_SUFFIXES = [".pyc", ".pyo"]


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    proc = subprocess.run(
        ["git", "ls-files"],
        cwd=root,
        capture_output=True,
        text=True,
        check=False,
    )
    if proc.returncode != 0:
        print(proc.stdout)
        print(proc.stderr)
        return proc.returncode

    issues: list[str] = []
    for rel in (line.strip() for line in proc.stdout.splitlines()):
        if not rel:
            continue
        if any(token in rel for token in FORBIDDEN_SUBSTRINGS):
            issues.append(rel)
            continue
        if any(rel.endswith(sfx) for sfx in FORBIDDEN_SUFFIXES):
            issues.append(rel)

    if issues:
        print("repo artifact hygiene check FAILED")
        for rel in sorted(issues):
            print(f"- forbidden tracked artifact: {rel}")
        return 1

    print("repo artifact hygiene check PASSED")
    return 0


if __name__ == "__main__":
    sys.exit(main())
