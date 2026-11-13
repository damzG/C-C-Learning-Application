#!/bin/sh
# Switches a source file from raylib's built in font to the project font.
# Usage: ./convert_text_calls.sh screen_map.c screen_lesson.c ...
# Safe to run more than once: it only matches the exact raylib names.
for f in "$@"; do
    sed -i -E 's/\bDrawText\(/UiDrawText(/g; s/\bMeasureText\(/UiMeasureText(/g' "$f"
    echo "converted $f"
done