#!/usr/bin/env python3
"""Lightweight docs/governance hygiene checks (non-CI mandatory by default)."""

from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
QUEUE_FILE = REPO_ROOT / "docs/status/T81LANG_IMPLEMENTATION_TASK_QUEUE_2026-03.md"

REQUIRED_READMES = [
    "core/README.md",
    "kernel/README.md",
    "runtime/README.md",
    "runtime/jit/README.md",
    "lang/README.md",
    "experimental/README.md",
    "experimental/distributed/README.md",
    "internal/README.md",
    "internal/axion/README.md",
    "internal/tooling/README.md",
    "tooling/README.md",
    "scripts/architecture/README.md",
    "scripts/governance/README.md",
    "scripts/restructure/README.md",
    "spec/tisc/README.md",
    "examples/consumer_cmake/README.md",
    "examples/system-integration/README.md",
    "docs/architecture/README.md",
    "docs/benchmarks/README.md",
    "docs/explanation/README.md",
    "docs/governance/README.md",
    "docs/how-to/README.md",
    "docs/migration/README.md",
    "docs/policies/README.md",
    "docs/product/README.md",
    "docs/records/README.md",
    "docs/records/inventories/README.md",
    "docs/reference/README.md",
    "docs/releases/README.md",
    "docs/research/README.md",
    "docs/rfcs/README.md",
    "docs/spec/README.md",
    "docs/spec/vm/README.md",
    "docs/standards/README.md",
    "docs/status/README.md",
    "docs/tutorials/README.md",
]


def parse_queue_statuses(queue_text: str) -> dict[str, set[str]]:
    statuses: dict[str, set[str]] = {}
    for line in queue_text.splitlines():
        if not line.startswith("|"):
            continue
        parts = [p.strip() for p in line.split("|")]
        if len(parts) < 9:
            continue
        task_id = parts[1]
        status = parts[7]
        if not re.match(r"^[A-Z0-9-]+$", task_id):
            continue
        statuses.setdefault(task_id, set()).add(status)
    return statuses


def main() -> int:
    issues: list[str] = []

    # 1) Required README presence check.
    for rel in REQUIRED_READMES:
        path = REPO_ROOT / rel
        if not path.exists():
            issues.append(f"missing required README: {rel}")

    # 2) Queue consistency check.
    if not QUEUE_FILE.exists():
        issues.append(f"missing queue file: {QUEUE_FILE.relative_to(REPO_ROOT)}")
        queue_statuses: dict[str, set[str]] = {}
    else:
        queue_text = QUEUE_FILE.read_text(encoding="utf-8")
        queue_statuses = parse_queue_statuses(queue_text)
        for task_id, vals in sorted(queue_statuses.items()):
            has_planned = any("Planned" in v for v in vals)
            has_completed = any("Completed" in v for v in vals)
            if has_planned and has_completed:
                issues.append(
                    f"task has both Planned and Completed statuses in queue: {task_id}"
                )

    # 3) Stale planned markers for completed tasks in status/audit artifacts.
    scan_dirs = [REPO_ROOT / "docs/status", REPO_ROOT / "docs/records/audits"]
    for task_id, vals in sorted(queue_statuses.items()):
        if not any("Completed" in v for v in vals):
            continue
        pattern = re.compile(rf"\b{re.escape(task_id)}\b.*\bPlanned\b")
        for scan_dir in scan_dirs:
            for md_file in scan_dir.rglob("*.md"):
                if not md_file.is_file():
                    continue
                rel = md_file.relative_to(REPO_ROOT)
                for idx, line in enumerate(md_file.read_text(encoding="utf-8").splitlines(), 1):
                    if pattern.search(line):
                        issues.append(
                            f"stale planned marker for completed task {task_id}: {rel}:{idx}"
                        )

    # 4) Cross-document status label coherence check.
    coherence_check = REPO_ROOT / "scripts/governance/check_status_label_coherence.py"
    if not coherence_check.exists():
      issues.append("missing status coherence check script: scripts/governance/check_status_label_coherence.py")
    else:
      result = subprocess.run(
          [sys.executable, str(coherence_check)],
          cwd=REPO_ROOT,
          capture_output=True,
          text=True,
          check=False,
      )
      if result.returncode != 0:
          issues.append("status label coherence check failed")
          output = (result.stdout + "\n" + result.stderr).strip()
          if output:
              for line in output.splitlines():
                  issues.append(f"coherence: {line}")

    if issues:
        print("governance hygiene check FAILED:")
        for issue in issues:
            print(f"- {issue}")
        return 1

    print("governance hygiene check PASSED")
    print(f"- required README coverage checked: {len(REQUIRED_READMES)} paths")
    print("- task-queue status consistency checked")
    print("- stale planned markers checked for completed tasks")
    print("- status label coherence checked")
    return 0


if __name__ == "__main__":
    sys.exit(main())
