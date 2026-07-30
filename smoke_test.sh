#!/usr/bin/env bash

set -euo pipefail

INPUT_DIR="${1:-}"

if [[ -z "$INPUT_DIR" ]]; then
    echo "Usage: $0 INPUT_DIR" >&2
    exit 1
fi

if [[ ! -d "$INPUT_DIR" ]]; then
    echo "Error: directory does not exist: $INPUT_DIR" >&2
    exit 1
fi

echo "Checking for empty directories under: $INPUT_DIR"

empty_count=0

while IFS= read -r -d '' dir; do
    echo "$dir"
    ((empty_count += 1))
done < <(find "$INPUT_DIR" -type d -empty -print0)

if (( empty_count == 0 )); then
    echo "No empty directories found."
else
    echo "Found $empty_count empty directories."
fi