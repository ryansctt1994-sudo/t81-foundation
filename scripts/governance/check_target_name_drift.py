#!/usr/bin/env python3
"""Prevent legacy build target names from reappearing in active docs."""

from __future__ import annotations

import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]

# Legacy token -> canonical replacement.
LEGACY_TARGETS = {
    "t81_frontend": "t81_lang_frontend",
    "t81_cli_driver": "t81_tool_cli",
}

SCAN_ROOTS = [
    REPO_ROOT / "README.md",
    REPO_ROOT / "docs",
]

EXCLUDED_PREFIXES = (
    "docs/records/",
)


def should_scan(path: Path) -> bool:
    rel = path.relative_to(REPO_ROOT).as_posix()
    if path.name.endswith(".md") is False:
        return False
    return not rel.startswith(EXCLUDED_PREFIXES)


def main() -> int:
    violations: list[str] = []

    for root in SCAN_ROOTS:
        if root.is_file():
            files = [root]
        else:
            files = sorted(p for p in root.rglob("*.md") if p.is_file() and should_scan(p))
        for path in files:
            rel = path.relative_to(REPO_ROOT).as_posix()
            lines = path.read_text(encoding="utf-8").splitlines()
            for idx, line in enumerate(lines, 1):
                for legacy, replacement in LEGACY_TARGETS.items():
                    if re.search(rf"\b{re.escape(legacy)}\b", line):
                        violations.append(
                            f"{rel}:{idx}: found legacy target '{legacy}' (use '{replacement}')"
                        )

    if violations:
        print("target name drift check FAILED")
        for violation in violations:
            print(f"- {violation}")
        return 1

    print("target name drift check PASSED")
    print(f"- checked legacy target aliases: {', '.join(sorted(LEGACY_TARGETS))}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
