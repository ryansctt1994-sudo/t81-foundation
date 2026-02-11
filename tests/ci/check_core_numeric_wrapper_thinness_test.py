#!/usr/bin/env python3
"""Regression tests for scripts/ci/check_core_numeric_wrapper_thinness.py."""

from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def run_checker(repo_root: Path, scan_root: Path) -> subprocess.CompletedProcess[str]:
    checker = repo_root / "scripts/ci/check_core_numeric_wrapper_thinness.py"
    return subprocess.run(
        ["python3", str(checker), "--root", str(scan_root)],
        text=True,
        capture_output=True,
        check=False,
    )


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]

    with tempfile.TemporaryDirectory(prefix="t81-core-wrapper-thinness-") as td:
        scan_root = Path(td)

        # Thin delegation-only wrappers should pass.
        write(
            scan_root / "src/core/bigint.cpp",
            '#include "t81/core/bigint.hpp"\n'
            "namespace t81::core { std::string BigInt::to_string() const { return impl_.to_string(); } }\n",
        )
        write(
            scan_root / "src/core/fraction.cpp",
            '#include "t81/core/fraction.hpp"\n'
            '#include "t81/fraction.hpp"\n'
            "namespace t81::core { std::string Fraction::to_string() const { return numerator.to_string() + \"/\" + denominator.to_string(); } }\n",
        )
        pass_run = run_checker(repo_root, scan_root)
        if pass_run.returncode != 0:
            print("expected pass but checker failed")
            print(pass_run.stdout)
            print(pass_run.stderr)
            return 1

        # Arithmetic implementation token in wrapper should fail.
        write(
            scan_root / "src/core/fraction.cpp",
            '#include "t81/core/fraction.hpp"\n'
            '#include "t81/fraction.hpp"\n'
            "namespace t81::core { std::string Fraction::to_string() const { auto x = t81::T81BigInt::mul(numerator.canonical(), denominator.canonical()); (void)x; return \"0/1\"; } }\n",
        )
        fail_run = run_checker(repo_root, scan_root)
        if fail_run.returncode == 0:
            print("expected failure but checker passed")
            print(fail_run.stdout)
            print(fail_run.stderr)
            return 1
        if "src/core/fraction.cpp:3" not in fail_run.stdout:
            print("checker failure output missing violating file path/line")
            print(fail_run.stdout)
            return 1

    print("check_core_numeric_wrapper_thinness_test: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
