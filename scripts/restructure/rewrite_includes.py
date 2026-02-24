#!/usr/bin/env python3
import os
import re
import sys

# Define the root of the repo
REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../'))

# Target directories to scan for include updates
SCAN_DIRS = [
    'core',
    'src',
    'include',
    'tests',
    'examples',
    'tools',
]

def main():
    print(f"Scanning for include updates in {REPO_ROOT}...")

    # Regex to find t81/core includes
    # Matches: #include "t81/core/foo.hpp" or <t81/core/foo.hpp>
    include_pattern = re.compile(r'#include\s+["<]t81/core/(.*?)(\.hpp)?([">])')

    modified_files = 0
    total_replacements = 0

    for directory in SCAN_DIRS:
        dir_path = os.path.join(REPO_ROOT, directory)
        if not os.path.exists(dir_path):
            print(f"Skipping {directory} (does not exist)")
            continue

        for root, _, files in os.walk(dir_path):
            for file in files:
                if not (file.endswith('.hpp') or file.endswith('.cpp') or file.endswith('.h') or file.endswith('.c')):
                    continue

                filepath = os.path.join(root, file)

                try:
                    with open(filepath, 'r', encoding='utf-8') as f:
                        content = f.read()
                except UnicodeDecodeError:
                    print(f"Skipping binary file: {filepath}")
                    continue

                new_content = content.replace('t81/core/', 't81/types/')

                if new_content != content:
                    replacements = content.count('t81/core/')
                    # print(f"Updating {filepath} ({replacements} replacements)")

                    with open(filepath, 'w', encoding='utf-8') as f:
                        f.write(new_content)

                    modified_files += 1
                    total_replacements += replacements

    print("\nRewrite Complete.")
    print(f"Files modified: {modified_files}")
    print(f"Total replacements: {total_replacements}")

if __name__ == '__main__':
    main()
