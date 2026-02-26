#!/usr/bin/env python3
"""Run C2 month-close preflight checks and emit a consolidated markdown report."""

from __future__ import annotations

import argparse
import datetime as dt
import pathlib
import subprocess
import sys
from dataclasses import dataclass

REPO_ROOT = pathlib.Path(__file__).resolve().parents[2]
DEFAULT_OUTPUT = REPO_ROOT / "docs/status/C2_MONTH_CLOSE_PREFLIGHT_2026-03-31.md"
MAX_OUTPUT_LINES = 120


@dataclass
class CheckResult:
    name: str
    command: list[str]
    returncode: int
    output: str

    @property
    def status(self) -> str:
        return "PASS" if self.returncode == 0 else "FAIL"


def run_command(name: str, command: list[str]) -> CheckResult:
    proc = subprocess.run(command, cwd=REPO_ROOT, capture_output=True, text=True)
    out = (proc.stdout or "") + (proc.stderr or "")
    return CheckResult(name=name, command=command, returncode=proc.returncode, output=out)


def clipped_output(text: str, max_lines: int = MAX_OUTPUT_LINES) -> str:
    lines = text.strip().splitlines()
    if not lines:
        return "(no output)"
    if len(lines) <= max_lines:
        return "\n".join(lines)
    kept = "\n".join(lines[:max_lines])
    return (
        f"{kept}\n"
        f"... [truncated {len(lines) - max_lines} lines; see local command logs for full output]"
    )


def format_report(results: list[CheckResult]) -> str:
    generated = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%d %H:%M:%SZ")
    overall_ok = all(r.returncode == 0 for r in results)
    overall = "PASS" if overall_ok else "FAIL"

    lines = [
        "# C2 Month-Close Preflight Report (2026-03-31)",
        "",
        f"Generated (UTC): {generated}",
        f"Generator: `scripts/governance/c2_month_close_preflight.py`",
        f"Overall: {overall}",
        "",
        "## Summary",
        "",
        "| Check | Status |",
        "| :--- | :--- |",
    ]

    for result in results:
        lines.append(f"| {result.name} | {result.status} |")

    lines.extend(["", "## Command Outputs", ""])
    for result in results:
        lines.append(f"### {result.name}")
        lines.append("")
        lines.append(f"- Status: {result.status}")
        lines.append(f"- Command: `{' '.join(result.command)}`")
        lines.append("")
        lines.append("```text")
        lines.append(clipped_output(result.output))
        lines.append("```")
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output",
        default=str(DEFAULT_OUTPUT.relative_to(REPO_ROOT)),
        help="Markdown report path (repo-relative or absolute).",
    )
    args = parser.parse_args()

    out_path = pathlib.Path(args.output)
    if not out_path.is_absolute():
        out_path = (REPO_ROOT / out_path).resolve()

    checks = [
        run_command(
            "C2 runbook consolidated check",
            [
                sys.executable,
                "scripts/governance/c2_month_close_check.py",
                "--output",
                "docs/status/C2_MONTH_CLOSE_CHECK_2026-03-31.md",
            ],
        ),
        run_command(
            "Full local CTest sweep",
            ["ctest", "--test-dir", "build", "--output-on-failure"],
        ),
        run_command(
            "Determinism slice",
            ["scripts/ci/run_determinism_slice.sh", "build"],
        ),
        run_command(
            "Stdlib surface baseline",
            [sys.executable, "scripts/governance/check_stdlib_surface_baseline.py"],
        ),
        run_command(
            "Stdlib promotion snapshot",
            [sys.executable, "scripts/governance/check_stdlib_promotion_snapshot.py"],
        ),
    ]

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(format_report(checks), encoding="utf-8")
    print(f"Wrote report: {out_path.relative_to(REPO_ROOT)}")
    for c in checks:
        print(f"{c.name}: {c.status}")
    return 0 if all(c.returncode == 0 for c in checks) else 1


if __name__ == "__main__":
    raise SystemExit(main())
