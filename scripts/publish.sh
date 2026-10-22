#!/bin/sh
# Publish steps shared by the release and its rehearsal, each safe to re-run:
#   publish.sh prepare DIR                 exactly the 18 zips, one commit, no firmware; SHA256SUMS
#   publish.sh upload TAG RELEASE_ID DIR   upload to that release, replacing
#   publish.sh attested DIR                attestations verify; tampered copies do not
#   publish.sh verify TAG DIR              attested, then the published release
#   publish.sh rehearsal-create TAG        a draft prerelease at EXPECTED_SHA; prints its ID
#   publish.sh rehearsal-delete TAG [ID]   delete a rehearsal release and its tag
# Fault prevented: a release with a missing, extra, unverifiable or tampered
# asset, or with archives built from another commit or carrying firmware or a
# known ROM, or a release that accepts a pull request's attestation. prepare
# reads EXPECTED_SHA and runs cmake (CMAKE names another); the rest
# call gh with GH_TOKEN and GITHUB_REPOSITORY, and attested and verify accept
# only attestations made on SOURCE_REF, which for a release is refs/heads/main.
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
  # No member is firmware, a built sample or a known ROM (ASSET_POLICY.md 5).
  root=$(dirname "$0")/..
  zips=$(names | sed "s|^|$1/|" | tr '\n' ';')
  "${CMAKE:-cmake}" "-DZIPS=${zips%;}" -DFIRMWARE="$root/tests/release/firmware.txt" \
    -DEXPECTED="$root/tests/built/expected.txt" -DCARTDB="$root/src/cartdb.c" \
    -P "$root/tests/cmake/firmware_scan.cmake" || fail "the firmware scan failed"
  (cd "$1" && sha256 $(names)) > "$1/SHA256SUMS"
  echo "publish: 18 archives from $EXPECTED_SHA; SHA256SUMS written"
}

# By release ID, because a draft cannot be looked up by its tag; an asset
# already there is replaced.
upload() {
  found=$(gh api "repos/$REPO/releases/$2" --jq .tag_name)
  [ "$found" = "$1" ] || fail "release $2 is $found, not $1"
  old=$(gh api "repos/$REPO/releases/$2/assets?per_page=100" --jq '.[] | "\(.id) \(.name)"')
  for n in $(names) SHA256SUMS; do
    for id in $(printf '%s\n' "$old" | awk -v n="$n" '$2 == n { print $1 }'); do
      gh api -X DELETE "repos/$REPO/releases/assets/$id" > /dev/null
    done
    gh api -X POST "https://uploads.github.com/repos/$REPO/releases/$2/assets?name=$n" \
      -H 'Content-Type: application/octet-stream' --input "$3/$n" > /dev/null || fail "cannot upload $n"
    echo "publish: uploaded $n"
  done
}

require_gh() {
  version=$(gh --version) || fail "supported stable gh >=2.102.0 required"
  printf '%s\n' "$version" | sed -n '1p' | jq -eRs '
    capture("^gh version (?<major>[0-9]+)\\.(?<minor>[0-9]+)\\.(?<patch>[0-9]+) \\([0-9]{4}-[0-9]{2}-[0-9]{2}\\)\\n$") |
    [.major,.minor,.patch] | map(tonumber) | . >= [2,102,0]
  ' > /dev/null 2>&1 || fail "supported stable gh >=2.102.0 required"
}

# Run before publishing too, because a published release cannot be changed.
attested() {
  require_gh
  tmp=${RUNNER_TEMP:-${TMPDIR:-/tmp}}/publish-verify
  rm -rf "$tmp"
  mkdir -p "$tmp/download"
  for n in $(names); do
    gh attestation verify "$1/$n" --repo "$REPO" \
      --signer-workflow "$REPO/.github/workflows/release.yml" --source-ref "$SOURCE_REF" > /dev/null ||
      fail "$n: its attestation does not verify from $SOURCE_REF"
    cp "$1/$n" "$tmp/$n"
    size=$(wc -c < "$tmp/$n")
    last=$(tail -c 1 "$tmp/$n" | od -An -tu1 | tr -d ' ')
    printf "$(printf '\\%03o' $((last ^ 255)))" | dd of="$tmp/$n" bs=1 seek=$((size - 1)) conv=notrunc 2> /dev/null
    if cmp -s "$1/$n" "$tmp/$n"; then fail "$n: the copy was not changed"; fi
    if gh attestation verify "$tmp/$n" --repo "$REPO" > /dev/null 2>&1; then
      fail "$n: a copy with its last byte flipped verifies"
    fi
    echo "publish: $n verifies; a copy with its last byte flipped fails, as expected"
  done
}

# Only an attestation made on main is release provenance: a pull request's
# rehearsal signs with the same workflow file, but from its own ref.
verify() {
  case $1 in
    rehearsal-*) ;;
    *) [ "$SOURCE_REF" = refs/heads/main ] || fail "$1 verifies only from refs/heads/main, not $SOURCE_REF" ;;
  esac
  attested "$2"
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
  # The tag's own URL, never the latest alias: a re-run for an older release
  # must check that release. install-line-latest tests the alias.
  base=https://github.com/$REPO/releases/download/$1/
  for n in $(names) SHA256SUMS; do
    curl -fsSL --retry 3 -o "$tmp/download/$n" "$base$n" || fail "cannot download $base$n"
  done
  cmp -s "$2/SHA256SUMS" "$tmp/download/SHA256SUMS" || fail "the published SHA256SUMS differs"
  (cd "$tmp/download" && sha256 $(names)) | cmp -s - "$2/SHA256SUMS" || fail "a downloaded zip differs"
  for n in $(names); do
    gh attestation verify "$tmp/download/$n" --repo "$REPO" \
      --signer-workflow "$REPO/.github/workflows/release.yml" --source-ref "$SOURCE_REF" > /dev/null ||
      fail "downloaded $n does not verify from $SOURCE_REF"
  done
  echo "publish: $1 published with 19 assets; every download matches SHA256SUMS and verifies"
}

# Every page, so a release past the first 100 is still found; a draft has no
# tag to look up by.
release_id() {
  pages=$(gh api --paginate --slurp "repos/$REPO/releases?per_page=100") || fail "release listing failed"
  printf '%s\n' "$pages" | jq -er --arg tag "$1" '
    if type == "array" and length > 0 and all(.[]; type == "array" and all(.[];
      type == "object" and (.tag_name|type) == "string" and (.id|type) == "number" and .id > 0 and .id == (.id|floor)))
    then [ .[][] | select(.tag_name == $tag) | .id ] |
      if length == 0 then "" elif length == 1 then .[0] else error("duplicate release") end
    else error("invalid pages") end
  ' || fail "invalid release listing"
}

# Only actual response headers establish status; stderr remains visible.
api_response() {
  api_rc=0
  response=$(
    gh api --include -X "$@" && rc=0 || rc=$?
    printf '.'
    exit "$rc"
  ) || api_rc=$?
  response=${response%.}
  parsed=$(printf '%s' "$response" | tr -d '\r' | awk '
    NR == 1 { if ($0 !~ /^HTTP\/[0-9]+(\.[0-9]+)? [1-5][0-9][0-9] [^\r]*$/) exit 1; code=$2; next }
    !body && $0 == "" { print code; body=1; next }
    !body && $0 !~ /^[A-Za-z0-9-]+:/ { exit 1 }
    body { print }
    END { if (!body) exit 1 }
  ') || fail "invalid HTTP response"
  http_status=$(printf '%s\n' "$parsed" | sed -n '1p')
  body=$(printf '%s\n' "$parsed" | sed '1d')
  case "$api_rc:$http_status" in
    0:2?? | 0:3?? | [1-9]*:4?? | [1-9]*:5??) ;;
    *) fail "HTTP command/status disagreement: $api_rc/$http_status" ;;
  esac
}

# The ID comes from the create response: a new draft can be missing from the
# release list for a while.
rehearsal_create() {
  [ -n "${EXPECTED_SHA:-}" ] || fail "EXPECTED_SHA is not set"
  id=$(release_id "$1") || fail "release lookup failed"
  if [ -z "$id" ]; then
    api_response POST "repos/$REPO/releases" -f tag_name="$1" -f target_commitish="$EXPECTED_SHA" \
      -f name="$1" -f body="Rehearsal; deleted at the end of this run." -F draft=true -F prerelease=true \
      -f make_latest=false
    [ "$http_status" = 201 ] || fail "create status $http_status"
    id=$(printf '%s\n' "$body" | jq -er --arg tag "$1" 'select(type == "object" and .tag_name == $tag and
      .draft == true and .prerelease == true and (.id|type) == "number" and .id > 0 and .id == (.id|floor)) | .id') || fail "invalid create response"
  fi
  [ -n "$id" ] || fail "no release $1 after creating it"
  echo "$id"
}

# The release goes before its tag, which an immutable release protects.
rehearsal_delete() {
  listed=$(release_id "$1") || fail "release lookup failed"
  id=${2:-$listed}
  if [ -n "$id" ]; then
    case $id in 0* | *[!0-9]*) fail "invalid release ID" ;; esac
    [ -z "$listed" ] || [ "$listed" = "$id" ] || fail "release identity conflict"
    api_response GET "repos/$REPO/releases/$id"
    if [ "$http_status" = 200 ]; then
      printf '%s\n' "$body" | jq -e --arg tag "$1" --argjson id "$id" '.id == $id and .tag_name == $tag' > /dev/null || fail "release identity mismatch"
      api_response DELETE "repos/$REPO/releases/$id"
      case $http_status in 204 | 404) ;; *) fail "DELETE status $http_status" ;; esac
    elif [ "$http_status" != 404 ]; then fail "release GET status $http_status"
    elif [ -n "$listed" ]; then fail "listed release absent"
    fi
    api_response GET "repos/$REPO/releases/$id"
    [ "$http_status" = 404 ] || fail "release still exists or GET failed: $http_status"
  fi
  listed=$(release_id "$1") || fail "release lookup failed"
  [ -z "$listed" ] || fail "release still listed"
  api_response GET "repos/$REPO/git/ref/tags/$1"
  if [ "$http_status" = 200 ]; then
    printf '%s\n' "$body" | jq -e --arg ref "refs/tags/$1" '.ref == $ref' > /dev/null || fail "tag identity mismatch"
    api_response DELETE "repos/$REPO/git/refs/tags/$1"
    [ "$http_status" = 204 ] || fail "DELETE status $http_status"
    api_response GET "repos/$REPO/git/ref/tags/$1"
  fi
  [ "$http_status" = 404 ] || fail "tag still exists or GET failed: $http_status"
  listed=$(release_id "$1") || fail "release lookup failed"
  [ -z "$listed" ] || fail "release still listed"
  if [ -n "$id" ]; then
    api_response GET "repos/$REPO/releases/$id"
    [ "$http_status" = 404 ] || fail "release still exists or GET failed: $http_status"
  fi
  api_response GET "repos/$REPO/git/ref/tags/$1"
  [ "$http_status" = 404 ] || fail "tag still exists or GET failed: $http_status"
  echo "publish: $1 and its release are gone"
}

cmd=${1:-}
[ $# -eq 0 ] || shift
case $cmd in rehearsal-*) case ${1:-} in rehearsal-*) ;; *) fail "$cmd: '${1:-}' is not a rehearsal tag" ;; esac ;; esac
case $cmd in upload | attested | verify | rehearsal-*) REPO=${GITHUB_REPOSITORY:?publish: GITHUB_REPOSITORY is not set} ;; esac
case $cmd in attested | verify) SOURCE_REF=${SOURCE_REF:?publish: SOURCE_REF is not set} ;; esac
case "$cmd $#" in
  "prepare 1") prepare "$1" ;;
  "upload 3") upload "$1" "$2" "$3" ;;
  "attested 1") attested "$1" ;;
  "verify 2") verify "$1" "$2" ;;
  "rehearsal-create 1") rehearsal_create "$1" ;;
  "rehearsal-delete 1" | "rehearsal-delete 2") rehearsal_delete "$1" "${2:-}" ;;
  *) fail "usage: publish.sh prepare DIR | upload TAG RELEASE_ID DIR | attested DIR | verify TAG DIR | rehearsal-create TAG | rehearsal-delete TAG [RELEASE_ID]" ;;
esac
