#!/bin/sh
# Publish steps shared by the release and its rehearsal, each safe to re-run:
#   publish.sh prepare DIR                 exactly the 18 zips, one commit; SHA256SUMS
#   publish.sh upload TAG RELEASE_ID DIR   upload to that release, overwriting
#   publish.sh verify TAG DIR              attestations, tampered copies, the published release
#   publish.sh rehearsal-create TAG        a draft prerelease at EXPECTED_SHA; prints its ID
#   publish.sh rehearsal-delete TAG        delete a rehearsal release and its tag
# Fault prevented: a release with a missing, extra, unverifiable or tampered
# asset, or with archives built from another commit. prepare reads
# EXPECTED_SHA; the rest call gh with GH_TOKEN and GITHUB_REPOSITORY.
set -eu

fail() {
  echo "publish: $*" >&2
  exit 1
}

names() {
  for c in library runner libretro; do
    for o in linux macos windows; do
      for a in x64 arm64; do echo "muhtari-$c-$o-$a.zip"; done
    done
  done | LC_ALL=C sort
}

sha256() {
  if command -v sha256sum > /dev/null 2>&1; then sha256sum -- "$@"; else shasum -a 256 -- "$@"; fi
}

# DIR holds the zips at its top and each build leg's commit.txt below it.
prepare() {
  [ -n "${EXPECTED_SHA:-}" ] || fail "EXPECTED_SHA is not set"
  bad=0
  for n in $(names); do
    [ -f "$1/$n" ] || { echo "publish: missing $n" >&2; bad=1; }
  done
  for f in "$1"/* "$1"/.[!.]*; do
    n=${f##*/}
    if [ -f "$f" ] && [ "$n" != SHA256SUMS ] && ! names | grep -qx "$n"; then
      echo "publish: unexpected $n" >&2
      bad=1
    fi
  done
  [ "$bad" -eq 0 ] || fail "$1 does not hold exactly the 18 archives"
  commits=$(find "$1" -name commit.txt)
  count=$(printf '%s\n' "$commits" | grep -c . || true)
  [ "$count" -eq 6 ] || fail "$count commit.txt files, expected 6, one per build leg"
  printf '%s\n' "$commits" | while IFS= read -r f; do
    got=$(tr -d '\r\n' < "$f")
    [ "$got" = "$EXPECTED_SHA" ] || fail "$f records $got, not $EXPECTED_SHA"
  done
  (cd "$1" && sha256 $(names)) > "$1/SHA256SUMS"
  echo "publish: 18 archives from $EXPECTED_SHA; SHA256SUMS written"
}

upload() {
  found=$(gh api "repos/$REPO/releases/$2" --jq .tag_name)
  [ "$found" = "$1" ] || fail "release $2 is $found, not $1"
  (cd "$3" && gh release upload "$1" $(names) SHA256SUMS --clobber --repo "$REPO")
}

verify() {
  tmp=${RUNNER_TEMP:-${TMPDIR:-/tmp}}/publish-verify
  rm -rf "$tmp"
  mkdir -p "$tmp/download"
  for n in $(names); do
    gh attestation verify "$2/$n" --repo "$REPO" > /dev/null || fail "$n: its attestation does not verify"
    cp "$2/$n" "$tmp/$n"
    size=$(wc -c < "$tmp/$n")
    last=$(tail -c 1 "$tmp/$n" | od -An -tu1 | tr -d ' ')
    printf "$(printf '\\%03o' $((last ^ 255)))" | dd of="$tmp/$n" bs=1 seek=$((size - 1)) conv=notrunc 2> /dev/null
    if cmp -s "$2/$n" "$tmp/$n"; then fail "$n: the copy was not changed"; fi
    if gh attestation verify "$tmp/$n" --repo "$REPO" > /dev/null 2>&1; then
      fail "$n: a copy with its last byte flipped verifies"
    fi
    echo "publish: $n verifies; a copy with its last byte flipped fails, as expected"
  done
  draft=$(gh release view "$1" --repo "$REPO" --json isDraft --jq .isDraft)
  [ "$draft" = false ] || fail "$1 is not published"
  gh release verify "$1" --repo "$REPO"
  want=$({ names; echo SHA256SUMS; } | LC_ALL=C sort)
  i=0
  until [ "$(gh release view "$1" --repo "$REPO" --json assets \
    --jq '.assets[] | select(.size > 0) | .name' | LC_ALL=C sort)" = "$want" ]; do
    i=$((i + 1))
    [ "$i" -lt 6 ] || fail "$1 does not list the 18 archives and SHA256SUMS, all non-empty"
    sleep 10
  done
  case $1 in
    rehearsal-*) base=https://github.com/$REPO/releases/download/$1/ ;;
    *) base=https://github.com/$REPO/releases/latest/download/ ;;
  esac
  for n in $(names) SHA256SUMS; do
    curl -fsSL --retry 3 -o "$tmp/download/$n" "$base$n" || fail "cannot download $base$n"
  done
  cmp -s "$2/SHA256SUMS" "$tmp/download/SHA256SUMS" || fail "the published SHA256SUMS differs"
  (cd "$tmp/download" && sha256 $(names)) | cmp -s - "$2/SHA256SUMS" || fail "a downloaded zip differs"
  for n in $(names); do
    gh attestation verify "$tmp/download/$n" --repo "$REPO" \
      --signer-workflow "$REPO/.github/workflows/release.yml" > /dev/null || fail "downloaded $n does not verify"
  done
  echo "publish: $1 published with 19 assets; every download matches SHA256SUMS and verifies"
}

release_id() {
  gh api "repos/$REPO/releases?per_page=100" --jq ".[] | select(.tag_name == \"$1\") | .id"
}

rehearsal_create() {
  [ -n "${EXPECTED_SHA:-}" ] || fail "EXPECTED_SHA is not set"
  if [ -z "$(release_id "$1")" ]; then
    gh release create "$1" --repo "$REPO" --draft --prerelease --latest=false --target "$EXPECTED_SHA" \
      --title "$1" --notes "Rehearsal; deleted at the end of this run." > /dev/null
  fi
  id=$(release_id "$1")
  [ -n "$id" ] || fail "no release $1 after creating it"
  echo "$id"
}

rehearsal_delete() {
  gh release delete "$1" --repo "$REPO" --cleanup-tag --yes > /dev/null 2>&1 || true
  gh api -X DELETE "repos/$REPO/git/refs/tags/$1" > /dev/null 2>&1 || true
  if [ -n "$(release_id "$1")" ]; then fail "$1 still has a release"; fi
  if gh api "repos/$REPO/git/ref/tags/$1" > /dev/null 2>&1; then fail "tag $1 still exists"; fi
  echo "publish: $1 and its release are gone"
}

cmd=${1:-}
[ $# -eq 0 ] || shift
case $cmd in rehearsal-*) case ${1:-} in rehearsal-*) ;; *) fail "$cmd: '${1:-}' is not a rehearsal tag" ;; esac ;; esac
case $cmd in upload | verify | rehearsal-*) REPO=${GITHUB_REPOSITORY:?publish: GITHUB_REPOSITORY is not set} ;; esac
case "$cmd $#" in
  "prepare 1") prepare "$1" ;;
  "upload 3") upload "$1" "$2" "$3" ;;
  "verify 2") verify "$1" "$2" ;;
  "rehearsal-create 1") rehearsal_create "$1" ;;
  "rehearsal-delete 1") rehearsal_delete "$1" ;;
  *) fail "usage: publish.sh prepare DIR | upload TAG RELEASE_ID DIR | verify TAG DIR | rehearsal-create TAG | rehearsal-delete TAG" ;;
esac
