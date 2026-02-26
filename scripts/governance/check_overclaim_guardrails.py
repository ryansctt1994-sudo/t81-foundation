#!/usr/bin/env python3
from __future__ import annotations

import re
import sys
from pathlib import Path

SCAN_ROOTS = [
    "README.md",
    "README.es.md",
    "README.pt-BR.md",
    "README.ru.md",
    "README.zh-CN.md",
    "docs",
    "book",
]

EXCLUDE_PREFIXES = [
    "docs/records/archive/",
    "docs/records/inventories/",
    "docs/site/",
]

BANNED_PATTERNS = [
    re.compile(r"\bfully hardened sandbox(?:ing)?\b", re.IGNORECASE),
    re.compile(r"\bhardware-backed containment\b", re.IGNORECASE),
    re.compile(r"\bos-level sandbox(?:ing)? (?:is|are) (?:provided|guaranteed)\b", re.IGNORECASE),
    re.compile(r"\bguaranteed cross-platform bit-exact(?:ness)? for all\b", re.IGNORECASE),
]


def should_scan(path: Path, repo_root: Path) -> bool:
    rel = path.relative_to(repo_root).as_posix()
    if not rel.endswith(".md"):
        return False
    return not any(rel.startswith(prefix) for prefix in EXCLUDE_PREFIXES)


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]
    issues: list[str] = []

    for root in SCAN_ROOTS:
        p = repo_root / root
        if p.is_file():
            files = [p]
        elif p.is_dir():
            files = list(p.rglob("*.md"))
        else:
            continue

        for md in files:
            if not should_scan(md, repo_root):
                continue
            rel = md.relative_to(repo_root).as_posix()
            text = md.read_text(encoding="utf-8")
            lines = text.splitlines()
            for i, line in enumerate(lines, 1):
                for pat in BANNED_PATTERNS:
                    if pat.search(line):
                        issues.append(f"{rel}:{i}: banned overclaim phrase matched /{pat.pattern}/")

    if issues:
        print("overclaim guardrails check FAILED")
        for issue in issues:
            print(f"- {issue}")
        return 1

    print("overclaim guardrails check PASSED")
    print("- scanned active README/docs/book markdown surfaces for banned overclaim phrases")
    return 0


if __name__ == "__main__":
    sys.exit(main())
