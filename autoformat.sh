#!/bin/bash
# Uses clang-format to format source files.

echo "Formatting with clang-format..."
find . -name "*.c" -o -name "*.h" | xargs clang-format -i
echo "Done."
