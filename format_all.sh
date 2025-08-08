#!/bin/bash

# Überprüfen, ob .c oder .h Dateien im aktuellen Verzeichnis oder Unterverzeichnissen existieren, aber ohne den .ccls-cache Ordner
FILES=$(find . -type d -name ".ccls-cache" -prune -o -type f \( -iname "*.c" -o -iname "*.h" \) -print)

# Wenn keine Dateien gefunden werden
if [ -z "$FILES" ]; then
  echo "Keine .c oder .h Dateien gefunden."
  exit 1
fi

# Durchlaufen der gefundenen Dateien und formatieren
for FILE in $FILES; do
  if [ -f "$FILE" ]; then
    ./format.sh "$FILE"  # Aufruf von format.sh mit der jeweiligen Datei
  fi
done
