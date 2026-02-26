#!/usr/bin/env python3
from __future__ import annotations

import sys
from pathlib import Path

REQUIRED_MARKERS: dict[str, list[str]] = {
    "docs/status/SYSTEM_STATUS.md": [
        "Cognitive Tiers",
        "Experimental / non-DCP / non-verified unless promoted through governance",
    ],
    "docs/status/IMPLEMENTATION_MATRIX.md": [
        "Cognitive Tiers",
        "Experimental, non-DCP, non-verified unless promoted through governance",
    ],
    "docs/product/DETERMINISTIC_CORE_PROFILE.md": [
        "Experimental tiers",
        "non-DCP and non-verified unless promoted through governance",
    ],
}


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]
    issues: list[str] = []

    for rel, markers in sorted(REQUIRED_MARKERS.items()):
        path = repo_root / rel
        if not path.exists():
            issues.append(f"missing required status boundary file: {rel}")
            continue
        text = path.read_text(encoding="utf-8")
        for marker in markers:
            if marker not in text:
                issues.append(f"{rel}: missing required boundary marker: {marker}")

    if issues:
        print("cognitive-tier boundary check FAILED")
        for issue in issues:
            print(f"- {issue}")
        return 1

    print("cognitive-tier boundary check PASSED")
    print("- verified experimental/non-DCP boundary markers across status/product artifacts")
    return 0


if __name__ == "__main__":
    sys.exit(main())
