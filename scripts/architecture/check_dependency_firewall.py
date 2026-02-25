#!/usr/bin/env python3
from __future__ import annotations

import argparse
import fnmatch
import re
import sys
from dataclasses import dataclass
from pathlib import Path

INCLUDE_RE = re.compile(r'^\s*#\s*include\s*[<\"]([^>\"]+)[>\"]')
SOURCE_EXTS = {".h", ".hh", ".hpp", ".hxx", ".c", ".cc", ".cpp", ".cxx", ".ipp"}

SCAN_ROOTS = [
    "core",
    "kernel",
    "runtime",
    "experimental",
    "tooling",
    "tools",
    "include/t81",
    "internal",
]


@dataclass
class Violation:
    code: str
    path: str
    line: int
    include: str
    message: str


def load_waivers(path: Path) -> list[tuple[str, str, str, str]]:
    waivers: list[tuple[str, str, str, str]] = []
    if not path.exists():
        return waivers

    for raw in path.read_text(encoding="utf-8").splitlines():
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        parts = line.split("\t")
        if len(parts) < 4:
            raise ValueError(f"Invalid waiver line (expected 4 tab-separated fields): {raw}")
        waivers.append((parts[0], parts[1], parts[2], parts[3]))
    return waivers


def is_waived(v: Violation, waivers: list[tuple[str, str, str, str]]) -> bool:
    for code_pat, path_pat, inc_pat, _reason in waivers:
        if not fnmatch.fnmatch(v.code, code_pat):
            continue
        if not fnmatch.fnmatch(v.path, path_pat):
            continue
        if not fnmatch.fnmatch(v.include, inc_pat):
            continue
        return True
    return False


def iter_files(repo_root: Path) -> list[Path]:
    files: list[Path] = []
    for root in SCAN_ROOTS:
        base = repo_root / root
        if not base.exists():
            continue
        for p in base.rglob("*"):
            if p.is_file() and p.suffix in SOURCE_EXTS:
                files.append(p)
    return files


def check_file(repo_root: Path, path: Path) -> list[Violation]:
    rel = path.relative_to(repo_root).as_posix()
    violations: list[Violation] = []

    for lineno, line in enumerate(path.read_text(encoding="utf-8", errors="ignore").splitlines(), start=1):
        m = INCLUDE_RE.match(line)
        if not m:
            continue
        inc = m.group(1)

        # Rule 1: core/* must not include experimental/*
        if rel.startswith("core/") and (
            inc.startswith("t81/experimental/") or inc.startswith("experimental/")
        ):
            violations.append(
                Violation(
                    code="CORE_DEPENDS_EXPERIMENTAL",
                    path=rel,
                    line=lineno,
                    include=inc,
                    message="core layer includes experimental header",
                )
            )

        # Rule 2: public headers must not include internal headers.
        if rel.startswith("include/t81/"):
            if inc.startswith("internal/") or "/internal/" in inc:
                violations.append(
                    Violation(
                        code="PUBLIC_INCLUDES_INTERNAL",
                        path=rel,
                        line=lineno,
                        include=inc,
                        message="public header includes internal header",
                    )
                )
            if ".." in inc and "internal" in inc:
                violations.append(
                    Violation(
                        code="PUBLIC_RELATIVE_INTERNAL",
                        path=rel,
                        line=lineno,
                        include=inc,
                        message="public header reaches internal via relative include",
                    )
                )

        # Rule 3: tools/* may only use public API headers.
        if rel.startswith("tools/"):
            forbidden_prefixes = (
                "internal/",
                "core/",
                "kernel/",
                "runtime/",
                "experimental/",
                "tooling/",
                "../",
                "../../",
            )
            if inc.startswith(forbidden_prefixes):
                violations.append(
                    Violation(
                        code="TOOLS_NON_PUBLIC_INCLUDE",
                        path=rel,
                        line=lineno,
                        include=inc,
                        message="tools layer includes non-public path",
                    )
                )

    return violations


def main() -> int:
    parser = argparse.ArgumentParser(description="Check structural dependency firewall invariants.")
    parser.add_argument(
        "--repo-root",
        default=str(Path(__file__).resolve().parents[2]),
        help="Repository root path",
    )
    parser.add_argument(
        "--waivers",
        default="scripts/architecture/dependency_firewall_waivers.tsv",
        help="Waiver file (tab-separated: code, path_glob, include_glob, reason)",
    )
    args = parser.parse_args()

    repo_root = Path(args.repo_root).resolve()
    waivers = load_waivers((repo_root / args.waivers).resolve())

    violations: list[Violation] = []
    waived: list[Violation] = []

    for f in iter_files(repo_root):
        for v in check_file(repo_root, f):
            if is_waived(v, waivers):
                waived.append(v)
            else:
                violations.append(v)

    print("Dependency firewall check")
    print(f"Scanned roots: {', '.join(SCAN_ROOTS)}")
    print(f"Waivers loaded: {len(waivers)}")

    if waived:
        print(f"Waived violations: {len(waived)}")
        for v in waived:
            print(f"  [WAIVED] {v.code} {v.path}:{v.line} -> {v.include}")

    if violations:
        print(f"Violations: {len(violations)}")
        for v in violations:
            print(f"  [FAIL] {v.code} {v.path}:{v.line} -> {v.include} ({v.message})")
        return 1

    print("No unwaived dependency firewall violations found.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
