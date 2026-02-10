#!/usr/bin/env python3
"""Validate t81-foundation runtime marker against canonical t81-vm contract."""

from __future__ import annotations

import json
import os
import subprocess
from pathlib import Path


def read_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def git_output(repo: Path, *args: str) -> str:
    return subprocess.check_output(["git", "-C", str(repo), *args], text=True).strip()


def git_show_json(repo: Path, ref: str, rel_path: str) -> dict | None:
    try:
        raw = subprocess.check_output(
            ["git", "-C", str(repo), "show", f"{ref}:{rel_path}"],
            text=True,
            stderr=subprocess.DEVNULL,
        )
    except subprocess.CalledProcessError:
        return None
    return json.loads(raw)


def main() -> None:
    root = Path(__file__).resolve().parent.parent
    vm_dir = Path(os.environ.get("T81_VM_DIR", str((root / "../t81-vm").resolve())))
    enforce_approval = os.environ.get("T81_ENFORCE_CONTRACT_CHANGE_APPROVAL", "0") == "1"
    allow_change = os.environ.get("T81_ALLOW_RUNTIME_CONTRACT_CHANGE", "0") == "1"
    base_ref = os.environ.get("T81_RUNTIME_CONTRACT_BASE_REF", "").strip()
    if not base_ref:
        github_base_ref = os.environ.get("GITHUB_BASE_REF", "").strip()
        base_ref = f"origin/{github_base_ref}" if github_base_ref else "origin/main"

    marker_path = root / "contracts/runtime-contract.json"
    boundary_doc_path = root / "docs/runtime-semantics-boundary.md"
    vm_contract_path = vm_dir / "docs/contracts/vm-compatibility.json"

    if not marker_path.exists():
        raise SystemExit(f"Missing marker: {marker_path}")
    if not vm_contract_path.exists():
        raise SystemExit(f"Missing VM contract: {vm_contract_path}")
    if not boundary_doc_path.exists():
        raise SystemExit(f"Missing boundary doc: {boundary_doc_path}")

    marker = read_json(marker_path)
    vm_contract = read_json(vm_contract_path)
    boundary_doc = boundary_doc_path.read_text(encoding="utf-8")

    contract_version = str(vm_contract.get("contract_version", "")).strip()
    if not contract_version:
        raise SystemExit("contract_version missing in VM contract")
    if marker.get("contract_version") != contract_version:
        raise SystemExit(
            "contract_version mismatch: "
            f"marker={marker.get('contract_version')!r} vm={contract_version!r}"
        )

    tag_lines = git_output(
        vm_dir, "tag", "--list", "runtime-contract-v*", "--sort=version:refname"
    ).splitlines()
    latest_tag = tag_lines[-1] if tag_lines else ""
    if not latest_tag:
        raise SystemExit("No runtime-contract-v* tags found in t81-vm")
    if marker.get("runtime_tag") != latest_tag:
        raise SystemExit(
            f"runtime_tag mismatch: marker={marker.get('runtime_tag')!r} vm={latest_tag!r}"
        )

    baseline_pin = git_output(vm_dir, "rev-parse", f"{latest_tag}^{{}}")
    marker_pin = str(marker.get("vm_main_pin", "")).strip()
    if not marker_pin:
        raise SystemExit("vm_main_pin missing in contracts/runtime-contract.json")
    if marker_pin != baseline_pin:
        raise SystemExit(f"vm_main_pin mismatch: marker={marker_pin!r} vm={baseline_pin!r}")

    previous_marker = git_show_json(root, base_ref, "contracts/runtime-contract.json")
    if previous_marker is not None:
        changed_fields = [
            key
            for key in ("runtime_tag", "contract_version", "vm_main_pin")
            if str(previous_marker.get(key, "")) != str(marker.get(key, ""))
        ]
        if changed_fields:
            msg = (
                "runtime contract marker changed vs "
                f"{base_ref}: {', '.join(changed_fields)}"
            )
            if enforce_approval and not allow_change:
                raise SystemExit(
                    f"{msg}. Set T81_ALLOW_RUNTIME_CONTRACT_CHANGE=1 for approved updates."
                )
            print(f"{msg} (approved={allow_change})")
    else:
        print(f"warning: unable to read contracts/runtime-contract.json at {base_ref}; skipping drift check")

    required_doc_snippets = [
        f"Active tagged runtime baseline: `{latest_tag}`",
        f"VM contract version: `{contract_version}`",
        f"VM contract commit pin (`t81-vm/main`): `{baseline_pin}`",
        "Contract marker in this repo: `contracts/runtime-contract.json`",
    ]
    missing = [snippet for snippet in required_doc_snippets if snippet not in boundary_doc]
    if missing:
        raise SystemExit(f"docs/runtime-semantics-boundary.md missing entries: {missing}")

    print(f"runtime boundary sync: ok ({latest_tag}, {contract_version})")


if __name__ == "__main__":
    main()
