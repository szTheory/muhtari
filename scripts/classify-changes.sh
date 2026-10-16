#!/bin/sh
# Decides whether a change needs CI's heavy jobs. Reads the changed paths on
# standard input, one per line, with both the old and the new name of a rename.
# Fault prevented: a code change skipping CI as "planning only". Prints
# heavy=false only when there is at least one path and every path is under
# .planning/; anything else, including empty or unreadable input, prints
# heavy=true. It reads all of its input, so the writer never sees a closed pipe.
set -u
paths=0
heavy=false
while IFS= read -r path || [ -n "$path" ]; do
  [ -n "$path" ] || continue
  paths=$((paths + 1))
  case "$path" in
    .planning/*) ;;
    *) heavy=true ;;
  esac
done
[ "$paths" -gt 0 ] || heavy=true
echo "heavy=$heavy"
