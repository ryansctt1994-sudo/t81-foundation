#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from pathlib import Path

LOCK_PATH = Path("docs/governance/PUBLIC_API_LOCK.json")
CONFIG_PATH = Path("include/t81/config.hpp")


def parse_version(config_text: str) -> str:
    major = re.search(r"#define\s+T81_VERSION_MAJOR\s+(\d+)", config_text)
    minor = re.search(r"#define\s+T81_VERSION_MINOR\s+(\d+)", config_text)
    patch = re.search(r"#define\s+T81_VERSION_PATCH\s+(\d+)", config_text)
    if not (major and minor and patch):
        raise RuntimeError("failed to parse T81 version macros from include/t81/config.hpp")
    return f"{major.group(1)}.{minor.group(1)}.{patch.group(1)}"


def surface_digest(repo_root: Path) -> str:
    include_root = repo_root / "include/t81"
    files = sorted(p for p in include_root.rglob("*.hpp"))
    h = hashlib.sha256()
    for p in files:
        rel = p.relative_to(repo_root).as_posix().encode("utf-8")
        h.update(rel)
        h.update(b"\x00")
        h.update(hashlib.sha256(p.read_bytes()).digest())
        h.update(b"\x00")
    return h.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Enforce public API lock against include/t81/** and version macros."
    )
    parser.add_argument(
        "--write-lock",
        action="store_true",
        help="write docs/governance/PUBLIC_API_LOCK.json from current workspace state",
    )
    args = parser.parse_args()

    repo_root = Path(__file__).resolve().parents[2]
    config_text = (repo_root / CONFIG_PATH).read_text(encoding="utf-8")
    version = parse_version(config_text)
    digest = surface_digest(repo_root)

    if args.write_lock:
        payload = {
            "version": version,
            "surface_digest_sha256": digest,
            "scope": "include/t81/**/*.hpp",
        }
        lock_path = repo_root / LOCK_PATH
        lock_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
        print(f"wrote {LOCK_PATH.as_posix()} for version {version}")
        return 0

    lock_path = repo_root / LOCK_PATH
    if not lock_path.exists():
        print("public API semver lock check FAILED")
        print(f"- missing lock file: {LOCK_PATH.as_posix()}")
        print("- run: python3 scripts/governance/check_public_api_semver.py --write-lock")
        return 1

    lock = json.loads(lock_path.read_text(encoding="utf-8"))
    lock_version = str(lock.get("version", ""))
    lock_digest = str(lock.get("surface_digest_sha256", ""))
    issues: list[str] = []

    if version != lock_version:
        issues.append(
            f"version mismatch: config={version}, lock={lock_version} "
            f"(update lock with --write-lock after approved semver bump)"
        )
    if digest != lock_digest:
        issues.append(
            "public API surface digest mismatch for include/t81/**/*.hpp "
            "(update lock with --write-lock after approved API change)"
        )

    if issues:
        print("public API semver lock check FAILED")
        for issue in issues:
            print(f"- {issue}")
        return 1

    print("public API semver lock check PASSED")
    print(f"- version: {version}")
    print(f"- digest: {digest}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
