#!/usr/bin/env python3
"""Regression tests for scripts/ci/check_v1_canonical_numeric_alias_usage.py."""

from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def run_checker(repo_root: Path, scan_root: Path) -> subprocess.CompletedProcess[str]:
    checker = repo_root / "scripts/ci/check_v1_canonical_numeric_alias_usage.py"
    return subprocess.run(
        ["python3", str(checker), "--root", str(scan_root)],
        text=True,
        capture_output=True,
        check=False,
    )


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]

    with tempfile.TemporaryDirectory(prefix="t81-v1-canonical-alias-policy-") as td:
        scan_root = Path(td)

        # Passing v1 migration test: uses canonical aliases only.
        write(
            scan_root / "tests/cpp/v1_pass_case.cpp",
            "\n".join(
                [
                    '#include "t81/bigint.hpp"',
                    '#include "t81/fraction.hpp"',
                    "int main() {",
                    "  t81::v1::CanonicalBigInt a = t81::v1::CanonicalBigInt::from_i64(1);",
                    "  t81::v1::CanonicalFraction f(a, t81::v1::CanonicalBigInt::from_i64(2));",
                    "  return (void)f, 0;",
                    "}",
                ]
            )
            + "\n",
        )

        pass_run = run_checker(repo_root, scan_root)
        if pass_run.returncode != 0:
            print("expected pass but checker failed")
            print(pass_run.stdout)
            print(pass_run.stderr)
            return 1

        # Missing alias usage in v1 migration file should fail.
        write(
            scan_root / "tests/cpp/v1_missing_alias.cpp",
            "\n".join(
                [
                    '#include "t81/bigint.hpp"',
                    "int main() {",
                    "  return 0;",
                    "}",
                ]
            )
            + "\n",
        )
        missing_alias_run = run_checker(repo_root, scan_root)
        if missing_alias_run.returncode == 0:
            print("expected failure for missing alias usage but checker passed")
            print(missing_alias_run.stdout)
            print(missing_alias_run.stderr)
            return 1
        if "v1_missing_alias.cpp" not in missing_alias_run.stdout:
            print("checker failure output missing missing-alias file path")
            print(missing_alias_run.stdout)
            return 1

        # Direct canonical type usage in v1 migration file should fail.
        write(
            scan_root / "tests/cpp/v1_direct_type_violation.cpp",
            "\n".join(
                [
                    '#include "t81/bigint.hpp"',
                    "int main() {",
                    "  t81::T81BigInt x = t81::T81BigInt::from_i64(7);",
                    "  return x.to_int64() == 7 ? 0 : 1;",
                    "}",
                ]
            )
            + "\n",
        )
        direct_violation_run = run_checker(repo_root, scan_root)
        if direct_violation_run.returncode == 0:
            print("expected failure for direct canonical type usage but checker passed")
            print(direct_violation_run.stdout)
            print(direct_violation_run.stderr)
            return 1
        if "v1_direct_type_violation.cpp" not in direct_violation_run.stdout:
            print("checker failure output missing direct-usage file path")
            print(direct_violation_run.stdout)
            return 1

    print("check_v1_canonical_numeric_alias_usage_test: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
