#!/bin/bash
set -e

OUTPUT_DIR="artifacts/restructure"
mkdir -p "$OUTPUT_DIR"
OUTPUT_FILE="$OUTPUT_DIR/phase1_inventory.txt"

echo "=== Phase 1 Preflight Scan ===" > "$OUTPUT_FILE"
echo "Date: $(date)" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "--- 1. File System Inventory ---" >> "$OUTPUT_FILE"
for dir in "src/data_types" "src/core" "include/t81/core" "core/types" "include/t81/types"; do
    if [ -d "$dir" ]; then
        echo "Directory exists: $dir" >> "$OUTPUT_FILE"
        ls -F "$dir" >> "$OUTPUT_FILE"
    else
        echo "Directory MISSING: $dir" >> "$OUTPUT_FILE"
    fi
    echo "" >> "$OUTPUT_FILE"
done

echo "--- 2. Include References (t81/core) ---" >> "$OUTPUT_FILE"
grep -r "t81/core" . --include="*.h" --include="*.hpp" --include="*.cpp" --include="*.c" | grep -v "$OUTPUT_FILE" >> "$OUTPUT_FILE" || echo "No references found." >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "--- 3. Include References (src/data_types) ---" >> "$OUTPUT_FILE"
grep -r "src/data_types" . --include="*.h" --include="*.hpp" --include="*.cpp" --include="*.c" | grep -v "$OUTPUT_FILE" >> "$OUTPUT_FILE" || echo "No references found." >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "--- 4. Include References (data_types/) ---" >> "$OUTPUT_FILE"
grep -r "\"data_types/" . --include="*.h" --include="*.hpp" --include="*.cpp" --include="*.c" | grep -v "$OUTPUT_FILE" >> "$OUTPUT_FILE" || echo "No references found." >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "--- 5. Build System References (CMakeLists.txt) ---" >> "$OUTPUT_FILE"
grep -n "src/core" CMakeLists.txt >> "$OUTPUT_FILE" || echo "No src/core in CMakeLists.txt" >> "$OUTPUT_FILE"
grep -n "core/types" CMakeLists.txt >> "$OUTPUT_FILE" || echo "No core/types in CMakeLists.txt" >> "$OUTPUT_FILE"
grep -n "src/data_types" CMakeLists.txt >> "$OUTPUT_FILE" || echo "No src/data_types in CMakeLists.txt" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "--- 6. Recursive Search for Missing Files (divmod.cpp, gcd.cpp) ---" >> "$OUTPUT_FILE"
find . -name "divmod.cpp" >> "$OUTPUT_FILE"
find . -name "gcd.cpp" >> "$OUTPUT_FILE"
find . -name "t81_bigint.h" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Scan complete. Results in $OUTPUT_FILE"
cat "$OUTPUT_FILE"
