#!/usr/bin/env python3
import os
import sys
import re

# Mapping of old include paths to new include paths
# This should be updated for each phase
INCLUDE_MAPPING = {
    "t81/core/": "t81/types/",
    # Add more mappings as needed
}

def rewrite_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except UnicodeDecodeError:
        print(f"Skipping binary file: {filepath}")
        return

    original_content = content

    for old, new in INCLUDE_MAPPING.items():
        # Regex to match #include "t81/core/..." or <t81/core/...>
        # We need to be careful not to replace partial matches if not intended
        # But here t81/core/ is a directory prefix so it is safe.

        # Replace <t81/core/...>
        content = re.sub(r'#include\s+<t81/core/(.*?)>', f'#include <t81/types/\\1>', content)
        # Replace "t81/core/..."
        content = re.sub(r'#include\s+"t81/core/(.*?)"', f'#include "t81/types/\\1"', content)

    if content != original_content:
        print(f"Updating {filepath}")
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)

def main():
    root_dir = os.getcwd()
    # Extensions to check
    extensions = {'.c', '.cpp', '.h', '.hpp', '.cc', '.hh'}

    # Check if a target directory or file is provided
    if len(sys.argv) > 1:
        target = sys.argv[1]
        if os.path.isfile(target):
            rewrite_file(target)
            return
        elif os.path.isdir(target):
            root_dir = target

    for root, dirs, files in os.walk(root_dir):
        # Skip hidden directories and build artifacts
        if '/.' in root or '/build' in root or '/cmake-build' in root or 'node_modules' in root:
            continue

        for file in files:
            if any(file.endswith(ext) for ext in extensions):
                rewrite_file(os.path.join(root, file))

if __name__ == "__main__":
    main()
