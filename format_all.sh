#!/bin/bash

FILES=$(find . -type d -name ".ccls-cache" -prune -o -type f \( -iname "*.c" -o -iname "*.h" \) -print)

if [ -z "$FILES" ]; then
  echo "Keine .c oder .h Dateien gefunden."
  exit 1
fi

for FILE in $FILES; do
  if [ -f "$FILE" ]; then
    clang-format -i -style="{BasedOnStyle: Google, IndentWidth: 4, TabWidth: 4, UseTab: Always, ColumnLimit: 0}" "$FILE"
    echo "formatted: $FILE"
  fi
done
