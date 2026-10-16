---
phase: 01-a-test-frame-in-retroarch
reviewed: 2026-10-16T21:54:07Z
depth: standard
files_reviewed: 41
files_reviewed_list:
  - .github/dependabot.yml
  - .github/workflows/ci.yml
  - .github/workflows/dependabot-automerge.yml
  - .github/workflows/release.yml
  - AGENTS.md
  - CMakeLists.txt
  - CMakePresets.json
  - README.md
  - cmake/packaging.cmake
  - docs/embedding.md
  - docs/maintaining.md
  - docs/runner.md
  - docs/testing.md
  - runner/bench.c
  - runner/bench.h
  - runner/clock.c
  - runner/clock.h
  - runner/exit_status.h
  - runner/main.c
  - runner/stop.c
  - runner/stop.h
  - scripts/classify-changes.sh
  - scripts/publish.sh
  - src/instance.c
  - tests/cmake/api_names.cmake
  - tests/cmake/archive_contents.cmake
  - tests/cmake/budgets.cmake
  - tests/cmake/dependabot.cmake
  - tests/cmake/exit_table.cmake
  - tests/cmake/install_line.cmake
  - tests/cmake/single_version.cmake
  - tests/core/test_core.c
  - tests/hygiene/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/hygiene.txt
  - tests/inventory/wasm32.txt
  - tests/policy/tests.cmake
  - tests/release/tests.cmake
  - tests/runner/test_runner.c
  - tests/runner/tests.cmake
  - tests/tests.cmake
findings:
  critical: 0
  warning: 4
  info: 5
  total: 9
status: issues_found
---

# Phase 1: Code Review Report

**Reviewed:** 2026-10-16T21:54:07Z
**Depth:** standard
**Files Reviewed:** 41
**Status:** issues_found

## Summary

This review follows the first fix round (CR-01 and WR-01 to WR-08 of the earlier review) and the commits after it,
from `0395e41` to `HEAD`. I read the whole diff for that range (25 files). I also read these files in full:
`scripts/publish.sh`, `scripts/classify-changes.sh`, `.github/workflows/release.yml`,
`.github/workflows/dependabot-automerge.yml`, `.github/dependabot.yml`, `src/instance.c`, `src/frame.c` (called
code), `runner/main.c`, `runner/bench.c`, `runner/bench.h`, `runner/stop.c`, `runner/clock.c`,
`runner/exit_status.h`, `tests/cmake/api_names.cmake`, `tests/cmake/install_line.cmake`,
`tests/cmake/exit_table.cmake`, `tests/cmake/archive_contents.cmake`, `tests/cmake/budgets.cmake`,
`cmake/packaging.cmake`, `README.md` and `.github/rulesets/main.json` (for context). Of `ci.yml` I read lines
1-140. The other listed files have not changed since the base commit. I only sampled them, so this review makes
no new claim about them.

I ran these new and changed tests in the existing build trees, and all passed: `ci.classify`,
`policy.publish_ref`, `docs.api_names`, `docs.api_names.self_test`, `runner.bench.bytes` and `api.size_tags`.
`docs.budgets` also passes.

The product fixes hold. `muhtari__bench_bytes` cannot wrap. Its `#if` gate is correct for 32-bit and 64-bit
`size_t`, and `main.c` treats a 0 result as out of memory. The size-tag cap is checked before any byte past the
tag is read. `muhtari_run_frame` now writes `audio_count` as zero on every error after the tag is accepted, as the
guide says. Pinning the source ref in `publish.sh` correctly separates rehearsal attestations from release
attestations. The classifier's count argument fails safe on an empty or non-numeric value.

Two fixes are incomplete:
- **WR-06 (Dependabot gate).** The gate checks `github.actor` at the moment the workflow runs. Auto-merge, once
  turned on, stays on, and a re-run keeps `github.actor`. So a person's commit can still merge unattended.
- **WR-08 (`rehearsal_delete`).** It still reports success when the lookup API call fails.

The earlier review's info items IN-01 to IN-06 are still open, as the disposition records, and are not repeated
here.

## Warnings

### WR-01: The Dependabot actor gate does not stop a person's commit from merging unattended

**File:** `.github/workflows/dependabot-automerge.yml:18`, `.github/workflows/dependabot-automerge.yml:45-49`,
`docs/maintaining.md:156-157`

**Issue:** The docs promise that "a person's push to a Dependabot branch never merges unattended". The workflow
does not keep that promise, for two reasons:
1. **Auto-merge stays on after a later push.** Dependabot's own run (actor `dependabot[bot]`) turns on auto-merge
   with `gh pr merge --auto --squash "$PR_URL"`. GitHub turns auto-merge off only when a user *without* write
   access pushes. A collaborator who pushes to the branch afterwards gets a run that is correctly skipped, but
   the auto-merge already armed stays armed. Their commit merges as soon as `CI required` passes. The ruleset
   requires 0 approvals, so nothing else stops it.
2. **A re-run keeps the original actor.** For a re-run, GitHub keeps `github.actor` as the actor of the first
   run (`github.triggering_actor` is the person who clicked re-run). A person can re-run an earlier
   Dependabot-triggered run after pushing their own commit. The run passes both gates and arms auto-merge.
   `gh pr merge` is not tied to the head commit the event saw, so it merges the PR as it is now.

`policy.dependabot` tests only the jq decision. It cannot see either path.

**Fix:** Tie the merge to the commit Dependabot pushed, gate on the person who started the run, and turn
auto-merge off when anyone else pushes:
```yaml
    if: >-
      github.actor == 'dependabot[bot]' && github.triggering_actor == 'dependabot[bot]' &&
      github.event.pull_request.user.login == 'dependabot[bot]'
    ...
      - if: steps.decide.outputs.decision == 'merge'
        env:
          GH_TOKEN: ${{ steps.token.outputs.token }}
          PR_URL: ${{ github.event.pull_request.html_url }}
          HEAD_SHA: ${{ github.event.pull_request.head.sha }}
        run: gh pr merge --auto --squash --match-head-commit "$HEAD_SHA" "$PR_URL"

  # A push by anyone else to a Dependabot branch disarms auto-merge.
  disarm:
    if: github.event.pull_request.user.login == 'dependabot[bot]' && github.triggering_actor != 'dependabot[bot]'
    permissions: { pull-requests: write, contents: write }
    runs-on: ubuntu-26.04
    steps:
      - env: { GH_TOKEN: "${{ github.token }}", PR_URL: "${{ github.event.pull_request.html_url }}" }
        run: gh pr merge --disable-auto "$PR_URL" || true
```
If that is too much, narrow the claim in `docs/maintaining.md` and the workflow header to what the gate does.
It stops a run triggered by a person from arming auto-merge. It does not stop a merge that was already armed.

### WR-02: `rehearsal_delete` still reports success when the lookup call fails

**File:** `scripts/publish.sh:154`, `scripts/publish.sh:157`, `scripts/publish.sh:159`

**Issue:** WR-08 added `--paginate` to `release_id` so that cleanup finds a release that is not on the first page.
The check that uses it still fails open: `if [ -n "$(release_id "$1")" ]; then fail ...; fi`. A command
substitution inside an `if` test is exempt from `set -e`. A rate limit, a 5xx or a network error makes
`release_id` print nothing, and the script then prints "publish: $1 and its release are gone". Lines 154 and 159
have the same problem: the script treats *any* failure of `gh api .../releases/$2` or `.../git/ref/tags/$1` as
"absent", including a failed request. So the fault the script names ("cleanup can report success while a release
remains") can still happen.

**Fix:** Tell "not found" apart from "request failed":
```sh
  ids=$(release_id "$1") || fail "cannot list releases to confirm $1 is gone"
  [ -z "$ids" ] || fail "$1 still has a release"
  ...
  if out=$(gh api "repos/$REPO/git/ref/tags/$1" 2>&1); then fail "tag $1 still exists"; fi
  case $out in *"HTTP 404"*) ;; *) fail "cannot confirm tag $1 is gone: $out" ;; esac
```
Apply the same 404 test to the release-by-ID check on line 154.

### WR-03: The pre-publish `attested` gate is weaker than the post-publish `verify` gate

**File:** `scripts/publish.sh:81`, `scripts/publish.sh:123-124`

**Issue:** `attested` exists so that a bad attestation is caught "before publishing too, because a published
release cannot be changed" (line 75). It pins `--source-ref` but not `--signer-workflow`. Without that flag,
`gh attestation verify --repo` accepts an attestation signed by *any* workflow in the repository on that ref.
`verify` then checks `--signer-workflow .../release.yml` only after the release is published and immutable. An
attestation for the same digest from another main-branch workflow would therefore pass the gate meant to protect
the immutable step, and fail only afterwards. Today only `release.yml` holds `attestations: write`. But nothing
enforces that, and a later workflow (nightly, for example) could add it.

**Fix:** Give the pre-publish check the same policy as the post-publish one:
```sh
    gh attestation verify "$1/$n" --repo "$REPO" \
      --signer-workflow "$REPO/.github/workflows/release.yml" --source-ref "$SOURCE_REF" > /dev/null ||
      fail "$n: its attestation does not verify as release.yml from $SOURCE_REF"
```

### WR-04: The documented 4096-byte size-tag limit has no boundary test

**File:** `tests/core/test_core.c:396-408`, `src/instance.c:22`

**Issue:** `docs/embedding.md:62` documents that a size above 4096 bytes returns `MUHTARI_STATUS_ERR_STRUCT_SIZE`.
The only new test uses a tag of `0xFFFFFFFF`. Changing the cap to 8192, to `0x7FFFFFFF` or to `UINT32_MAX - 1` would
still pass every test, and so would changing `>` to `>=`, which would refuse a valid 4096 tag. Rule 5 asks every
behaviour change to land with a test that pins it. (Under rule 7 this is a test-code finding, so it becomes a
seed.)

**Fix:** Add both sides of the boundary, using a buffer large enough to hold them:
```c
static union { muhtari_frame f; uint8_t bytes[4097]; } big;
memset(&big, 0, sizeof big);
uint32_t sz = 4096u; memcpy(big.bytes, &sz, sizeof sz);
CHECK(muhtari_run_frame(m, &big.f) == MUHTARI_STATUS_OK);         /* zero tail: accepted */
sz = 4097u; memcpy(big.bytes, &sz, sizeof sz);
CHECK(muhtari_run_frame(m, &big.f) == MUHTARI_STATUS_ERR_STRUCT_SIZE);
```
Set `video` and `audio` to NULL in `big.f`, or point them at valid buffers, so the accepted case writes nothing it
does not own.

## Info

### IN-01: The size-tag cap's comment claims more than the cap does

**File:** `src/instance.c:13-15`

**Issue:** The comment says the cap means "a garbage tag is refused without reading past the caller's object".
That is true only for a tag above 4096. A garbage tag between `sizeof` and 4096 is still accepted for scanning,
and the loop on lines 27-31 reads up to 4096 bytes past the start of a caller's struct of 24 to 40 bytes. The size
tag protocol is designed this way. The cap bounds the read; it does not prevent it.

**Fix:** "...so a garbage tag reads at most 4096 bytes from the caller's object and the scan stays bounded."

### IN-02: The new guide check and its self test are missing from `docs/testing.md`

**File:** `docs/testing.md:104-118`

**Issue:** The "Negative controls" table lists every repository check's self test. `docs.api_names.self_test`,
added in this range, is not in it, and `docs.api_names` is not described anywhere in `docs/` (rule 5:
documented together).

**Fix:** Add a row such as `` | `docs.api_names.self_test` | A header declaring a call its guide omits, beside a guide naming a call its header lacks. | ``
and one sentence near "Budgets" saying what `docs.api_names` enforces.

### IN-03: `api_names.cmake` relies on CMake 4's policy defaults for `IN_LIST`

**File:** `tests/cmake/api_names.cmake:40`, `tests/cmake/api_names.cmake:46`

**Issue:** The script has no `cmake_minimum_required`, unlike `dependabot.cmake`. In `-P` mode on CMake 3.25 to
3.31, the minimum `CMakeLists.txt` declares, CMP0057 is left unset, so `if(name IN_LIST written)` is a parse error.
It works today only because the presets require CMake 4.3. `info_claims.cmake` has the same latent problem.

**Fix:** Add `cmake_minimum_required(VERSION 3.25)` at the top of the script.

### IN-04: The `api_names` self test leaves the `_`-prefix branch untested, and header comments count as declarations

**File:** `tests/cmake/api_names.cmake:17`, `tests/cmake/api_names.cmake:48-55`, `tests/policy/tests.cmake:780-781`

**Issue:** The fixture never writes a name ending in `_`. A broken prefix branch (one that accepted any `_` name, or
none) would pass the self test. Also, `MATCHALL` over the whole header text counts a name that appears only in a
header comment as declared, so the guide could name a removed call that a stale comment still mentions.

**Fix:** Add `MUHTARI_STATUS_` (must pass) and `MUHTARI_NOPE_` (must fail) to the fixture guide. Strip `/* ... */`
comments from the header before matching.

### IN-05: One embedding-guide paragraph mixes `muhtari_get_info` and `muhtari_run_frame`

**File:** `docs/embedding.md:107-113`

**Issue:** The paragraph opens with `muhtari_get_info`. The next sentence, "A NULL `video` or `audio` skips that
output", and the pitch and capacity rules that follow are all about `muhtari_run_frame`. A reader can take them to
apply to `get_info`.

**Fix:** Split the paragraph: one sentence for `muhtari_get_info`, then a paragraph that starts "`muhtari_run_frame`:
a NULL `video` or `audio` skips that output; ...".

---

_Reviewed: 2026-10-16T21:54:07Z_
_Reviewer: Claude (gsd-code-reviewer)_
_Depth: standard_
