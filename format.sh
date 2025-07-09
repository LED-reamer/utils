#!/bin/bash

if [ -z "$1" ]; then
  echo "no file specified"
  exit 1
fi

FILE="$1"

if [ ! -f "$FILE" ]; then
  echo "file does not exist: $FILE"
  exit 1
fi

clang-format -i -style="{BasedOnStyle: Google, IndentWidth: 4, TabWidth: 4, UseTab: Always}" "$FILE"

echo "formatted: $FILE"
