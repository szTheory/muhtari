#!/bin/sh
# Download a pinned file by name and verify it before anything uses it.
#
#   scripts/fetch-pin.sh NAME DEST
#
# Fault prevented: an unpinned or tampered download reaching a build. NAME is a
# row of pins.txt ("name version url sha256"; $PINS names another file). The
# download lands in a temporary directory; only when its SHA-256 equals the
# row's is it unpacked into DEST (.tar.xz, .tar.gz or .zip, top directory
# stripped) or copied there. A zip is read by CMake, because Git Bash's tar
# cannot read one, and must hold exactly one top-level directory. Prints DEST.
# Exit status 1 on any failure, 2 on misuse.
set -eu
set -f

if [ $# -ne 2 ]; then
  echo "usage: fetch-pin.sh NAME DEST" >&2
  exit 2
fi
name=$1
dest=$2
pins=${PINS:-$(dirname "$0")/../pins.txt}

row=$(awk -v n="$name" '$1 == n { print; exit }' "$pins")
if [ -z "$row" ]; then
  echo "fetch-pin: no row $name in $pins" >&2
  exit 1
fi
# shellcheck disable=SC2086 # split the row into its four fields
set -- $row
url=$3
want=$4

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
file=$tmp/${url##*/}
curl -fsSL --retry 3 -o "$file" "$url"
if command -v sha256sum >/dev/null 2>&1; then
  got=$(sha256sum "$file")
else
  got=$(shasum -a 256 "$file")
fi
got=${got%% *}
if [ "$got" != "$want" ]; then
  echo "fetch-pin: $name: sha256 mismatch: pins.txt has $want, the download is $got" >&2
  exit 1
fi

mkdir -p "$dest"
case $file in
  *.tar.xz | *.tar.gz) tar -xf "$file" -C "$dest" --strip-components=1 ;;
  *.zip)
    # Globbing is off, so find names the top directory. The relative path
    # keeps an MSYS path away from a native cmake on Windows.
    mkdir "$tmp/x"
    (cd "$tmp/x" && ${CMAKE:-cmake} -E tar xf "../${file##*/}")
    tops=$(find "$tmp/x" -mindepth 1 -maxdepth 1)
    top=$(find "$tmp/x" -mindepth 1 -maxdepth 1 -type d)
    if [ -z "$top" ] || [ "$tops" != "$top" ] || [ "$(printf '%s\n' "$top" | wc -l)" -ne 1 ]; then
      echo "fetch-pin: $name: the zip must hold exactly one top-level directory" >&2
      exit 1
    fi
    cp -R "$top/." "$dest/"
    ;;
  *) cp "$file" "$dest/" ;;
esac
echo "$dest"
