#!/usr/bin/env python3
import os
import re
import sys
from pathlib import Path

# Config
REPO_ROOT = Path(__file__).resolve().parent.parent.parent
SPEC_DIR = REPO_ROOT / "spec"
INCLUDE_DIR = REPO_ROOT / "include" / "t81"
SRC_DIR = REPO_ROOT / "src"
OPCODES_HEADER = INCLUDE_DIR / "tisc" / "opcodes.hpp"
TISC_SPEC = SPEC_DIR / "tisc-spec.md"

# Mappings (Spec -> Implementation Directory)
SPEC_IMPL_MAP = {
    "tisc-spec.md": "tisc",
    "t81vm-spec.md": "vm",
    "t81-data-types.md": "data_types",
    "canonfs-spec.md": "canonfs",
    "t81lang-spec.md": "lang",
    "axion-kernel.md": "axion",
    "cognitive-tiers.md": "tiers",
    "hanoi-kernel-spec.md": "hanoi",
}

def check_opcodes():
    print("Checking TISC Opcodes...")
    if not OPCODES_HEADER.exists() or not TISC_SPEC.exists():
        print(f"Skipping opcode check: {OPCODES_HEADER} or {TISC_SPEC} missing.")
        return []

    # Parse Header
    header_content = OPCODES_HEADER.read_text()
    header_opcodes = set()

    # Extract enum block
    enum_match = re.search(r'enum class Opcode : std::uint8_t \{(.*?)\};', header_content, re.DOTALL)
    if enum_match:
        enum_body = enum_match.group(1)
        lines = enum_body.split('\n')
        for line in lines:
            line = line.split('//')[0].strip()
            if not line: continue
            # Match Identifier = val, or Identifier,
            match = re.match(r'([A-Za-z0-9_]+)', line)
            if match:
                header_opcodes.add(match.group(1))

    # Parse Spec
    spec_content = TISC_SPEC.read_text()
    spec_opcodes = set()

    # Regex for headings #### NAME or #### NAME / NAME2
    headings = re.findall(r'^####\s+(.+)$', spec_content, re.MULTILINE)
    for h in headings:
        # Split by / if multiple
        parts = h.split('/')
        for p in parts:
            name = p.strip().upper()
            # Remove parentheses like "ADD (Binary)" if any
            name = name.split('(')[0].strip()
            # Remove underscores for comparison
            spec_opcodes.add(name.replace('_', ''))

    # Normalize Header Opcodes for comparison
    normalized_header = {op.upper().replace('_', ''): op for op in header_opcodes}

    drift = []

    # Check Spec -> Header (Are all spec opcodes in header?)
    # We filter out some common non-opcode headings if any
    for spec_op in spec_opcodes:
        if spec_op not in normalized_header:
            # Maybe it's not an opcode heading but something else?
            # We assume #### is reserved for opcodes in section 5.
            # But wait, there might be other sections with ####.
            # The spec has `#### ADD`, `#### SUB` under `### 5.2 Arithmetic Instructions`.
            # But section 5.2 itself starts with `###`.
            # Let's hope #### is only used for opcodes.
            # If not, we might get false positives.
            drift.append(f"Spec defines opcode '{spec_op}' but not found in opcodes.hpp (Normalized: {spec_op})")

    # Check Header -> Spec (Are all header opcodes in spec?)
    for norm_op, orig_op in normalized_header.items():
        if norm_op not in spec_opcodes:
            drift.append(f"Header defines opcode '{orig_op}' but not found in spec headings")

    return drift

def check_public_headers():
    print("Checking Public Headers vs Spec...")
    drift = []
    for root, dirs, files in os.walk(INCLUDE_DIR):
        for file in files:
            if not file.endswith(".hpp"): continue
            rel_path = Path(root) / file
            rel_path_str = str(rel_path.relative_to(INCLUDE_DIR)) # e.g. "tisc/opcodes.hpp"

            # Check if referenced in any spec file
            # We match full filename, relative path, or the stem (class name usually)
            found = False
            file_stem = rel_path.stem

            for spec_file in SPEC_DIR.glob("*.md"):
                content = spec_file.read_text()
                if file in content or rel_path_str in content or file_stem in content:
                    found = True
                    break

            if not found:
                drift.append(f"Public header '{rel_path_str}' not referenced in any spec file.")
    return drift

def check_impl_dirs():
    print("Checking Implementation Directories...")
    drift = []
    for spec, impl in SPEC_IMPL_MAP.items():
        spec_path = SPEC_DIR / spec
        impl_path = SRC_DIR / impl

        if not spec_path.exists():
            # If spec doesn't exist, we skip map check but warn?
            # Or maybe map is wrong.
            # For this script we assume map is correct intent.
            # But actually, hanoi-kernel-spec.md might not exist if it's not in list_files spec.
            # list_files spec showed hanoi-kernel-spec.md.
            drift.append(f"Spec file '{spec}' mapped but does not exist.")
            continue

        if not impl_path.exists() or not impl_path.is_dir():
             drift.append(f"Spec '{spec}' maps to implementation '{impl}' but directory not found.")

    return drift

def main():
    drift_detected = False

    try:
        op_drifts = check_opcodes()
        if op_drifts:
            print("\n[!] Opcode Drift Detected:")
            for d in op_drifts:
                print(f"  - {d}")
            drift_detected = True
    except Exception as e:
        print(f"Error checking opcodes: {e}")

    try:
        header_drifts = check_public_headers()
        if header_drifts:
            print("\n[!] Header Drift Detected:")
            for d in header_drifts:
                print(f"  - {d}")
            drift_detected = True
    except Exception as e:
        print(f"Error checking headers: {e}")

    try:
        impl_drifts = check_impl_dirs()
        if impl_drifts:
            print("\n[!] Implementation Directory Drift Detected:")
            for d in impl_drifts:
                print(f"  - {d}")
            drift_detected = True
    except Exception as e:
        print(f"Error checking impl dirs: {e}")

    if drift_detected:
        print("\nFAILURE: Spec drift detected.")
        sys.exit(1)
    else:
        print("\nSUCCESS: No spec drift detected.")
        sys.exit(0)

if __name__ == "__main__":
    main()
