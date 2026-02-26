#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

BLOCKLIST = [
    "gnu general public license",
    "gnu affero general public license",
    "agpl",
]


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
    candidates = []
    for rel in (line.strip() for line in proc.stdout.splitlines()):
        if not rel:
            continue
        name = Path(rel).name.upper()
        if name.startswith("LICENSE") or name.startswith("COPYING"):
            candidates.append(rel)

    for rel in candidates:
        text = (root / rel).read_text(encoding="utf-8", errors="ignore").lower()
        for token in BLOCKLIST:
            if token in text:
                issues.append(f"{rel}: blocked license token '{token}'")

    if issues:
        print("license policy check FAILED")
        for issue in issues:
            print(f"- {issue}")
        return 1

    print("license policy check PASSED")
    print(f"- scanned license files: {len(candidates)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
