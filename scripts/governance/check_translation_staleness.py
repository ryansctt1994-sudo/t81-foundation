#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

MAX_STALE_DAYS = 30
MAX_STALE_COMMITS = 10

ROOT_README_PAIRS = [
    ("README.md", "README.zh-CN.md"),
    ("README.md", "README.es.md"),
    ("README.md", "README.pt-BR.md"),
    ("README.md", "README.ru.md"),
]

BOOK_TRANSLATION_DIRS = [
    "book/book-cn",
    "book/book-es",
    "book/book-pt",
    "book/book-ru",
]


def run_git(args: list[str], repo_root: Path) -> str:
    result = subprocess.run(
        ["git", *args],
        cwd=repo_root,
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        raise RuntimeError(f"git {' '.join(args)} failed: {result.stderr.strip()}")
    return result.stdout


def last_commit_unix(path: str, repo_root: Path) -> int | None:
    out = run_git(["log", "-1", "--format=%ct", "--", path], repo_root).strip()
    if not out:
        return None
    return int(out)


def english_commits_after(path: str, after_unix: int, repo_root: Path) -> int:
    out = run_git(["log", "--format=%ct", "--", path], repo_root)
    count = 0
    for line in out.splitlines():
        if int(line) > after_unix:
            count += 1
    return count


def book_chapter_pairs(repo_root: Path) -> list[tuple[str, str]]:
    pairs: list[tuple[str, str]] = []
    en_dir = repo_root / "book/book-en"
    en_by_prefix = {
        p.name[:2]: p.relative_to(repo_root).as_posix()
        for p in en_dir.glob("[0-9][0-9]_*.md")
    }
    for rel_lang_dir in BOOK_TRANSLATION_DIRS:
        lang_dir = repo_root / rel_lang_dir
        trans_by_prefix = {
            p.name[:2]: p.relative_to(repo_root).as_posix()
            for p in lang_dir.glob("[0-9][0-9]_*.md")
        }
        for prefix, en_rel in sorted(en_by_prefix.items()):
            trans_rel = trans_by_prefix.get(prefix)
            if trans_rel:
                pairs.append((en_rel, trans_rel))
    return pairs


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]
    issues: list[str] = []
    scanned = 0

    pairs = [*ROOT_README_PAIRS, *book_chapter_pairs(repo_root)]
    for en_path, trans_path in pairs:
        scanned += 1
        en_ct = last_commit_unix(en_path, repo_root)
        tr_ct = last_commit_unix(trans_path, repo_root)
        if en_ct is None:
            issues.append(f"missing git history for canonical file: {en_path}")
            continue
        if tr_ct is None:
            issues.append(f"missing git history for translation file: {trans_path}")
            continue

        if tr_ct >= en_ct:
            continue

        stale_days = (en_ct - tr_ct) / 86400.0
        stale_commits = english_commits_after(en_path, tr_ct, repo_root)
        if stale_days > MAX_STALE_DAYS or stale_commits > MAX_STALE_COMMITS:
            issues.append(
                f"{trans_path} is stale vs {en_path}: "
                f"{stale_days:.1f} days, {stale_commits} canonical commits behind "
                f"(thresholds: >{MAX_STALE_DAYS} days or >{MAX_STALE_COMMITS} commits)"
            )

    if issues:
        print("translation staleness check FAILED")
        for issue in issues:
            print(f"- {issue}")
        return 1

    print("translation staleness check PASSED")
    print(f"- scanned translation pairs: {scanned}")
    print(
        f"- thresholds: >{MAX_STALE_DAYS} days or >{MAX_STALE_COMMITS} canonical commits behind"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
