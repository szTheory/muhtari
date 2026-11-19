#!/bin/sh
# Decides whether a change needs CI's heavy jobs. Reads changed paths on
# standard input, one per line, and takes the pull request's changed-file count
# as its first argument. The optional second argument enables the narrow
# release-please metadata exception.
# Fault prevented: a code or release input change skipping CI. Prints
# heavy=false for planning-only paths, or for exactly the three generated
# release metadata files when the caller has independently verified the
# release-please branch and same-repository PR. All malformed, empty, oversized
# or unmatched input prints heavy=true.
# It reads all of its input, so the writer never sees a closed pipe.
set -u
paths=0
heavy=false
release_metadata=${2-}
release_manifest=false
release_changelog=false
release_version=false
if [ "$release_metadata" = true ]; then
  heavy=true
fi
case ${1-} in
  '' | *[!0-9]*) heavy=true ;;
  *) [ "$1" -le 3000 ] || heavy=true ;;
esac
while IFS= read -r path || [ -n "$path" ]; do
  [ -n "$path" ] || continue
  paths=$((paths + 1))
  if [ "$release_metadata" = true ]; then
    case "$path" in
      .release-please-manifest.json)
        [ "$release_manifest" = false ] || heavy=true
        release_manifest=true
        ;;
      CHANGELOG.md)
        [ "$release_changelog" = false ] || heavy=true
        release_changelog=true
        ;;
      version.txt)
        [ "$release_version" = false ] || heavy=true
        release_version=true
        ;;
      *) heavy=true ;;
    esac
  else
    case "$path" in
      .planning/*) ;;
      *) heavy=true ;;
    esac
  fi
done
[ "$paths" -gt 0 ] || heavy=true
if [ "$release_metadata" = true ] && [ "${1-}" = 3 ] && [ "$paths" -eq 3 ] \
  && [ "$release_manifest" = true ] && [ "$release_changelog" = true ] \
  && [ "$release_version" = true ]; then
  heavy=false
fi
echo "heavy=$heavy"
