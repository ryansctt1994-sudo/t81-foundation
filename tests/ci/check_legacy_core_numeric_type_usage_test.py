#!/usr/bin/env python3
"""Regression tests for scripts/ci/check_legacy_core_numeric_type_usage.py."""

from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def run_checker(repo_root: Path, scan_root: Path) -> subprocess.CompletedProcess[str]:
    checker = repo_root / "scripts/ci/check_legacy_core_numeric_type_usage.py"
    return subprocess.run(
        ["python3", str(checker), "--root", str(scan_root)],
        text=True,
        capture_output=True,
        check=False,
    )


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]

    with tempfile.TemporaryDirectory(prefix="t81-legacy-type-policy-") as td:
        scan_root = Path(td)

        # Allowlisted paths containing compatibility tokens should pass.
        write(
            scan_root / "tests/cpp/core_numeric_compat_test.cpp",
            "using t81::core::BigInt;\nusing t81::core::Fraction;\n",
        )
        write(
            scan_root / "tests/cpp/core_bigint_compat_properties_test.cpp",
            "using t81::core::BigInt;\n",
        )
        write(
            scan_root / "tests/cpp/core_fraction_compat_properties_test.cpp",
            "using t81::core::Fraction;\n",
        )
        # Canonical usage should pass.
        write(
            scan_root / "src/ok.cpp",
            "// core::BigInt in comment should be ignored\n"
            "const char* s = \"t81::core::Fraction in string should be ignored\";\n"
            "t81::T81BigInt x = t81::T81BigInt::from_i64(1);\n",
        )
        # Excluded dependency/tooling directories should be ignored.
        write(
            scan_root / "node_modules/pkg/ignored.cpp",
            "core::BigInt x;\n",
        )
        write(
            scan_root / "venv/lib/ignored.cpp",
            "t81::core::Fraction y;\n",
        )
        write(
            scan_root / "build-cxx20/tmp/ignored.cpp",
            "core::BigInt z;\n",
        )
        pass_run = run_checker(repo_root, scan_root)
        if pass_run.returncode != 0:
            print("expected pass but checker failed")
            print(pass_run.stdout)
            print(pass_run.stderr)
            return 1

        # Non-allowlisted legacy type usage should fail.
        write(
            scan_root / "src/violation.cpp",
            "core::BigInt x;\n",
        )
        fail_run = run_checker(repo_root, scan_root)
        if fail_run.returncode == 0:
            print("expected failure but checker passed")
            print(fail_run.stdout)
            print(fail_run.stderr)
            return 1
        if "src/violation.cpp" not in fail_run.stdout:
            print("checker failure output missing violating file path")
            print(fail_run.stdout)
            return 1
        if 'src/violation.cpp:1: disallowed token "core::BigInt"' not in fail_run.stdout:
            print("checker failure output missing expected line number for violation")
            print(fail_run.stdout)
            return 1

        # Multiline block comments should not skew violation line numbers.
        write(
            scan_root / "src/line_number_violation.cpp",
            "/* t81::core::BigInt in comment\n"
            "   still comment */\n"
            "\n"
            "core::BigInt y;\n",
        )
        line_run = run_checker(repo_root, scan_root)
        if line_run.returncode == 0:
            print("expected failure but checker passed for line number check")
            print(line_run.stdout)
            print(line_run.stderr)
            return 1
        if 'src/line_number_violation.cpp:4: disallowed token "core::BigInt"' not in line_run.stdout:
            print("checker reported incorrect line number after block comment stripping")
            print(line_run.stdout)
            return 1

        # Violation listing should be deterministic (repo-relative lexical order).
        write(
            scan_root / "src/a_first_violation.cpp",
            "core::BigInt a;\n",
        )
        write(
            scan_root / "src/z_last_violation.cpp",
            "t81::core::Fraction z;\n",
        )
        ordered_fail_run = run_checker(repo_root, scan_root)
        if ordered_fail_run.returncode == 0:
            print("expected failure but checker passed for ordering check")
            print(ordered_fail_run.stdout)
            print(ordered_fail_run.stderr)
            return 1
        lines = [
            line.strip()
            for line in ordered_fail_run.stdout.splitlines()
            if line.strip().startswith("- ")
        ]
        expected = [
            '- src/a_first_violation.cpp:1: disallowed token "core::BigInt"',
            '- src/line_number_violation.cpp:4: disallowed token "core::BigInt"',
            '- src/violation.cpp:1: disallowed token "core::BigInt"',
            '- src/z_last_violation.cpp:1: disallowed token "t81::core::Fraction"',
        ]
        if lines != expected:
            print("checker violation order mismatch")
            print("expected:")
            print("\n".join(expected))
            print("actual:")
            print("\n".join(lines))
            return 1

    print("check_legacy_core_numeric_type_usage_test: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
