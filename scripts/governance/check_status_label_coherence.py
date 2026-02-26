#!/usr/bin/env python3
"""Check component maturity label coherence across key status artifacts."""

from __future__ import annotations

import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]

README = REPO_ROOT / "README.md"
MATRIX = REPO_ROOT / "docs/status/IMPLEMENTATION_MATRIX.md"
SYSTEM_STATUS = REPO_ROOT / "docs/status/SYSTEM_STATUS.md"
PROMOTION_GATE = REPO_ROOT / "docs/status/T81LANG_PROMOTION_GATE.md"


def _normalize(label: str) -> str:
    cleaned = label.lower()
    cleaned = re.sub(r"[*_`]", "", cleaned)
    cleaned = re.sub(r":[^:]+:", "", cleaned)
    cleaned = cleaned.replace("⚠️", "").replace("🚧", "").replace("✅", "").replace("🧪", "")
    cleaned = " ".join(cleaned.split())
    if "beta" in cleaned:
        return "beta"
    if "alpha" in cleaned:
        return "alpha"
    if "stable" in cleaned:
        return "stable"
    if "experimental" in cleaned:
        return "experimental"
    if "concept" in cleaned:
        return "concept"
    if "partial" in cleaned:
        return "partial"
    return cleaned


def _parse_markdown_table(path: Path) -> list[list[str]]:
    rows: list[list[str]] = []
    for line in path.read_text(encoding="utf-8").splitlines():
        if not line.startswith("|"):
            continue
        if set(line.replace("|", "").strip()) <= {":", "-", " "}:
            continue
        parts = [p.strip() for p in line.split("|")[1:-1]]
        if parts:
            rows.append(parts)
    return rows


def _component_labels_from_readme(path: Path) -> dict[str, str]:
    labels: dict[str, str] = {}
    for row in _parse_markdown_table(path):
        if len(row) < 2:
            continue
        comp = re.sub(r"[*`]", "", row[0]).strip().lower()
        if comp in {"t81vm", "axion", "t81lang"}:
            labels[comp] = _normalize(row[1])
    return labels


def _component_labels_from_system_status(path: Path) -> dict[str, str]:
    labels: dict[str, str] = {}
    for row in _parse_markdown_table(path):
        if len(row) < 2:
            continue
        comp = re.sub(r"[*`]", "", row[0]).strip().lower()
        if comp in {"t81vm", "axion kernel", "t81lang"}:
            key = "axion" if comp == "axion kernel" else comp
            labels[key] = _normalize(row[1])
    return labels


def _component_labels_from_matrix(path: Path) -> dict[str, str]:
    labels: dict[str, str] = {}
    for row in _parse_markdown_table(path):
        if len(row) < 3:
            continue
        comp = re.sub(r"[*`]", "", row[0]).strip().lower()
        if comp in {"t81vm", "axion kernel", "t81lang"}:
            key = "axion" if comp == "axion kernel" else comp
            labels[key] = _normalize(row[2])
    return labels


def main() -> int:
    issues: list[str] = []

    for p in (README, MATRIX, SYSTEM_STATUS, PROMOTION_GATE):
        if not p.exists():
            issues.append(f"missing required file: {p.relative_to(REPO_ROOT)}")
    if issues:
        for issue in issues:
            print(f"- {issue}")
        return 1

    readme = _component_labels_from_readme(README)
    matrix = _component_labels_from_matrix(MATRIX)
    system = _component_labels_from_system_status(SYSTEM_STATUS)

    required_components = ("t81vm", "axion", "t81lang")
    for comp in required_components:
        if comp not in readme:
            issues.append(f"README missing component status row for: {comp}")
        if comp not in matrix:
            issues.append(f"implementation matrix missing component row for: {comp}")
        if comp not in system:
            issues.append(f"system status missing component row for: {comp}")

    for comp in required_components:
        if comp not in readme or comp not in matrix or comp not in system:
            continue
        if not (readme[comp] == matrix[comp] == system[comp]):
            issues.append(
                "label mismatch for "
                f"{comp}: README={readme[comp]} matrix={matrix[comp]} system={system[comp]}"
            )

    matrix_text = MATRIX.read_text(encoding="utf-8")
    if "T81Lang" in matrix_text and "(Draft)" not in matrix_text:
        issues.append("implementation matrix must retain T81Lang spec authority as Draft")
    if "spec authority remains Draft" not in matrix_text:
        issues.append("implementation matrix missing explicit Draft-spec/Beta-implementation note")

    gate_text = PROMOTION_GATE.read_text(encoding="utf-8")
    if "Current Classification: Draft Spec / Beta Implementation" not in gate_text:
        issues.append("promotion gate missing Draft Spec / Beta Implementation classification")

    if issues:
        print("status label coherence check FAILED:")
        for issue in issues:
            print(f"- {issue}")
        return 1

    print("status label coherence check PASSED")
    print("- checked README, IMPLEMENTATION_MATRIX, and SYSTEM_STATUS component maturity labels")
    print("- checked T81Lang Draft-spec/Beta-implementation posture consistency")
    return 0


if __name__ == "__main__":
    sys.exit(main())
