#!/usr/bin/env python3
from __future__ import annotations

import sys
from pathlib import Path

README_FILES = [
    "README.md",
    "README.zh-CN.md",
    "README.es.md",
    "README.pt-BR.md",
    "README.ru.md",
]


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    issues: list[str] = []
    for rel in README_FILES:
        path = root / rel
        if not path.exists():
            issues.append(f"missing file: {rel}")
            continue
        txt = path.read_text(encoding="utf-8")
        for target in README_FILES:
            if target not in txt:
                issues.append(f"{rel}: missing language link token '{target}'")

    if issues:
        print("translation metadata check FAILED")
        for issue in issues:
            print(f"- {issue}")
        return 1

    print("translation metadata check PASSED")
    print(f"- files checked: {len(README_FILES)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
