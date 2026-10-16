---
phase: 01-a-test-frame-in-retroarch
reviewed: 2026-10-16T00:00:00Z
depth: standard
files_reviewed: 47
files_reviewed_list:
  - include/muhtari.h
  - src/internal.h
  - src/instance.c
  - src/frame.c
  - src/testcard.c
  - host/palette.h
  - host/palette.c
  - runner/main.c
  - runner/bench.c
  - runner/bench.h
  - runner/clock.c
  - runner/clock.h
  - runner/crc32.c
  - runner/crc32.h
  - runner/exit_status.h
  - runner/png.c
  - runner/png.h
  - runner/sha256.c
  - runner/sha256.h
  - runner/stop.c
  - runner/stop.h
  - libretro/libretro.c
  - libretro/CMakeLists.txt
  - libretro/muhtari_libretro.info.in
  - CMakeLists.txt
  - CMakePresets.json
  - cmake/MuhtariTests.cmake
  - cmake/muhtari.pc.in
  - cmake/muhtariConfig.cmake.in
  - cmake/packaging.cmake
  - cmake/toolchains/ppc-linux-gnu.cmake
  - cmake/toolchains/wasm32-wasi.cmake
  - cmake/version.h.in
  - .github/workflows/ci.yml
  - .github/workflows/release.yml
  - .github/workflows/nightly.yml
  - .github/workflows/dependabot-automerge.yml
  - .github/dependabot.yml
  - .github/rulesets/main.json
  - scripts/publish.sh
  - scripts/fetch-pin.sh
  - scripts/classify-changes.sh
  - scripts/check-title.sh
  - scripts/apply-repo-settings.sh
  - pins.txt
  - release-please-config.json
  - .release-please-manifest.json
findings:
  critical: 1
  warning: 8
  info: 6
  total: 15
status: issues_found
---

# Phase 1: Code Review Report

**Reviewed:** 2026-10-16T00:00:00Z
**Depth:** standard
**Files Reviewed:** 47 Tier 1 files, read in full
**Status:** issues_found

## Summary

I read every Tier 1 file in full. These Tier 2 files I also read in full: `docs/embedding.md`,
`tests/cmake/check_junit.cmake`, `tests/cmake/hashes.cmake`, `README.md` and `version.txt`. I sampled
`tests/core/test_core.c` (lines 230-399, the validation and size-tag tests), `docs/runner.md` (lines 1-60) and
`docs/maintaining.md` (lines 1-80). I did not open the other Tier 2 files, so this review claims nothing about
them. The scope note excluded `libretro/libretro.h`, which I did not read.

The core library (`src/`) is small and correct for what it does. I checked the timing constants
(39375000/656711 Hz, 88 samples per 13,125 ticks giving exactly 48 kHz), the pitch bound, the ABI static asserts,
the test card geometry and the documented PNG size (232,645 bytes) by hand, and they hold. SHA-256, CRC-32 and
Adler-32 follow their specifications. No workflow puts `${{ }}` expressions into `run:` text that an attacker
controls. Every action is pinned by SHA, every checkout uses `persist-credentials: false`, and the token
permissions are narrow.

Findings:
- One memory-safety bug in the runner on 32-bit targets. The project builds two such lanes itself (wasm32, be32).
- A size-tag check with no upper bound.
- A documented API guarantee that the code and its own test contradict.
- Several weaknesses in the release and CI path. The largest is that attestations made by pull requests cannot be
  told apart from release attestations by the checks the pipeline uses.

Each finding is tagged **[product]**, **[release/CI]** or **[test/tooling]**. Under AGENTS rule 7, test and
tooling findings are seeds, not requirements.

## Narrative Findings (AI reviewer)

## Critical Issues

### CR-01: `--bench N` overflows the sample buffer on 32-bit targets [product]

**File:** `runner/main.c:267-268`, `runner/main.c:312`
**Issue:** The runner sizes the bench sample buffer as `(size_t)opt->frames * sizeof *samples`. `parse_count`
accepts any N up to 4294967295, and `docs/runner.md:21` documents that range. Where `size_t` is 32 bits (the
wasm32 and be32 lanes this phase builds, and any 32-bit embedder build of the runner), the multiplication wraps
once N ≥ 2^30. For example, `--bench 1073741824` asks for `malloc(0)`, and `--bench 1073741825` asks for 4 bytes.
wasi-libc and glibc return a non-NULL pointer for these sizes, so the `samples == NULL` check passes. The frame
loop then writes `samples[timed++]` far past the allocation on every frame: a heap buffer overflow reached from
the command line. On 64-bit targets the same request is merely huge (16 GiB). It either fails cleanly or, under
overcommit, gets committed lazily.
**Fix:**
```c
if (opt->bench && opt->frames > SIZE_MAX / sizeof(uint32_t)) {
    fprintf(stderr, "muhtari-run: out of memory\n");
    return MUHTARI__EXIT_FAILURE;
}
uint32_t *samples =
    opt->bench ? (uint32_t *)malloc((size_t)opt->frames * sizeof *samples) : NULL;
```
(`SIZE_MAX` needs `<stdint.h>`, which `muhtari.h` already includes.) Add a runner test that runs
`--bench 1073741824` on the wasm32 and be32 lanes and expects exit 1 rather than a crash.

## Warnings

### WR-01: The size tag has no upper bound, so a garbage tag reads out of bounds and loops up to 4 GiB [product]

**File:** `src/instance.c:13-29`
**Issue:** `muhtari__check_size` reads bytes from `ours` up to the caller's `size`, and `size` can be anything up
to `UINT32_MAX`. If a caller forgets the `memset`/`size` step that `docs/embedding.md:52` asks for, or passes a
stack struct whose tag holds garbage, the call does not return `ERR_STRUCT_SIZE`. It reads past the caller's
object, which is undefined behaviour and usually a crash once it crosses a page, and it may spin through billions
of bytes. That breaks rule 4's "every call returns within a bounded amount" for the cheapest possible caller
mistake, and the mistake is the one the size tag exists to catch.
**Fix:** Reject any tag far above what any future layout could need, before the loop:
```c
#define MUHTARI__SIZE_MAX 4096u /* far above any planned struct */
if (size < first || size > MUHTARI__SIZE_MAX) {
    return MUHTARI_STATUS_ERR_STRUCT_SIZE;
}
```
Document the cap in `docs/embedding.md` and add a size-tag case for `0xFFFFFFFF`.

### WR-02: The docs say `muhtari_run_frame` always writes `audio_count`, but it does not [product]

**File:** `docs/embedding.md:108`, `src/frame.c:9-16`, `tests/core/test_core.c:236`
**Issue:** `docs/embedding.md` says `muhtari_run_frame` "checks its arguments before it changes anything, and
always writes `audio_count`". The code does not write `audio_count` when `io == NULL`, which is unavoidable, or
when the size tag is rejected (`frame.c:12-15` returns before the "From here on" comment). The size-tag test
(`test_core.c:380-393`) asserts that nothing is written on `ERR_STRUCT_SIZE`, so code and test agree with each
other and disagree with the user docs. The comment on `test_validate_first` (`test_core.c:236`, "Each refused call
... writes audio_count") makes the same false claim. A host that trusts the docs and reads `audio_count` after a
size-tag error reads a stale or uninitialised value.
**Fix:** Correct the doc: "Once the size tag is accepted, `muhtari_run_frame` writes `audio_count` on every
return: zero on an error." Fix the test comment to match.

### WR-03: Pull-request rehearsals produce attestations the release checks cannot tell apart from release attestations [release/CI]

**File:** `.github/workflows/release.yml:51-97`, `scripts/publish.sh:79`, `scripts/publish.sh:85`, `scripts/publish.sh:115-116`
**Issue:** The rehearsal job runs on `pull_request` with `id-token: write` and `attestations: write`. It runs the
pull request's own `release.yml` and `publish.sh` and calls `actions/attest` over whatever is in `dist/`, so any
same-repository pull request (any branch, including Dependabot's) can mint a build-provenance attestation, signed
under this repository's identity, for arbitrary bytes. The pipeline's own checks accept those attestations as
release attestations:
- `attested` uses only `--repo`.
- `verify` adds `--signer-workflow "$REPO/.github/workflows/release.yml"`, which a rehearsal satisfies, because
  the rehearsal is that same workflow file and the pull request can rewrite it.

Neither check pins the source ref to `refs/heads/main`. So "verifies as signed by `release.yml`"
(`docs/maintaining.md` step 6) does not establish that the bytes came from reviewed `main`. A user or downstream
packager who copies this check gets the same false assurance.
**Fix:** For the real release, pin the ref in both checks:
```sh
gh attestation verify "$f" --repo "$REPO" \
  --signer-workflow "$REPO/.github/workflows/release.yml" --source-ref refs/heads/main
```
Have the rehearsal check `--source-ref "refs/pull/$PR/merge"` instead, so both paths stay exercised. State in
`docs/maintaining.md` that only attestations with a `main` source ref are release provenance. Also consider giving
the rehearsal a separate signer workflow file, so a `--signer-workflow` check alone excludes rehearsals.

### WR-04: Dependabot action updates auto-merge with no cooldown into workflows that hold the App key and the attestation identity [release/CI]

**File:** `.github/dependabot.yml:6-15`, `.github/workflows/dependabot-automerge.yml:22-45`
**Issue:** Minor and patch updates of third-party actions merge automatically. The ruleset requires zero approvals
and the merge token carries `workflows: write`. The updated actions then run in `release.yml`, which mints the
App token (`release.yml:145-152`) and holds `id-token`/`attestations: write`. A compromised upstream action that
publishes a malicious minor or patch tag is pulled in, merged and running with release credentials within one
Dependabot cycle, and no person looks at it. SHA pinning does not help here, because Dependabot moves the pin.
**Fix:** Add a cooldown so a new version must age before Dependabot proposes it, and keep auto-merge for
already-aged versions only:
```yaml
    cooldown:
      default-days: 7
```
Optionally, exclude the actions used in `release.yml` (`create-github-app-token`, `attest`,
`release-please-action`) from auto-merge.

### WR-05: The "planning only" classification can skip CI for a pull request with more than 3,000 files [release/CI]

**File:** `.github/workflows/ci.yml:60-71`, `scripts/classify-changes.sh:9-20`
**Issue:** `GET /repos/{owner}/{repo}/pulls/{n}/files` returns at most 3,000 files, even with `--paginate`. Files
come back in path order, and `.planning/` sorts before every source directory (`.` is 0x2E, below `C`, `s`, `t`
and the rest). A pull request that touches 3,000 or more `.planning/` files plus any code change therefore lists
only planning paths, and the job prints `heavy=false`. Every heavy job is skipped, and `CI required` passes. The
push run on `main` would catch the problem only after the merge. The script's stated fault ("a code change
skipping CI as 'planning only'") is exactly what happens.
**Fix:** Compare the number of listed paths with the pull request's `changed_files` and treat any shortfall as
heavy:
```sh
total=$(gh api "repos/$GITHUB_REPOSITORY/pulls/$NUMBER" --jq .changed_files)
[ "$total" -le 3000 ] || { echo heavy=true | tee -a "$GITHUB_OUTPUT"; exit 0; }
```

### WR-06: Dependabot auto-merge is gated on the pull request's author, not on who triggered the run [release/CI]

**File:** `.github/workflows/dependabot-automerge.yml:15`, `.github/workflows/dependabot-automerge.yml:24-27`
**Issue:** Both the job `if:` and the `decide` program test `github.event.pull_request.user.login`. That is the
author of the pull request, and it stays `dependabot[bot]` after anyone with write access pushes more commits to
the Dependabot branch. The `synchronize` run that follows comes from a human actor, so it gets Actions secrets (the
App key is stored there too, per the comment at lines 29-31), mints a `workflows: write` App token and enables
auto-merge on code Dependabot did not write. The only thing between that and a merge is
`dependabot/fetch-metadata`'s own commit verification. Nothing in this repository asserts that the verification
is on or tests it (`tests/cmake/dependabot.cmake` exercises only the jq program, per the comment at line 21).
**Fix:** Gate on the triggering actor, as GitHub's Dependabot docs recommend:
```yaml
if: github.actor == 'dependabot[bot]' && github.event.pull_request.user.login == 'dependabot[bot]'
```
Pass `github.actor` to the `decide` program as well, so the tested logic covers it.

### WR-07: `publish.sh verify` downloads from `releases/latest/` for real tags, so a re-run of an older release fails or checks the wrong release [release/CI]

**File:** `scripts/publish.sh:105-113`, `docs/maintaining.md` ("Re-running a failed release")
**Issue:** For any tag that is not a rehearsal, `verify` downloads from `releases/latest/download/` instead of
`releases/download/$TAG/`. If a newer release has become latest before someone re-runs `publish` for an older one
(the docs call that safe), the script downloads the newer release's assets and fails with "the published
SHA256SUMS differs". If the `latest` redirect lags right after `make_latest=true` (`release.yml:224`), the first
run can fail in the same way, because the retry loop at lines 99-104 waits for the asset list, not for the alias.
**Fix:** Verify the tag's own URL in every case. Leave the `latest` alias to `install-line-latest`, which already
tests it:
```sh
base=https://github.com/$REPO/releases/download/$1/
```

### WR-08: `release_id` reads only the first 100 releases, so rehearsal cleanup can report success while a release remains [release/CI]

**File:** `scripts/publish.sh:121-123`, `scripts/publish.sh:146`
**Issue:** `release_id` lists `releases?per_page=100` without `--paginate`. After 100 releases (rehearsals count
while they exist, and every rehearsal publishes one), a rehearsal outside the first page is invisible:
- `rehearsal_create` creates a duplicate instead of reusing the existing release.
- `rehearsal_delete`'s check at line 146 ("still has a release") passes even when the delete at line 145 failed,
  so leftover public prereleases go unreported, against the post-rehearsal invariant in `docs/maintaining.md`.

**Fix:** Look the release up by tag with `gh api --paginate "repos/$REPO/releases" --jq ...`, or use
`gh release view "$1" --json databaseId`. That works for published releases, and in `rehearsal_delete` it runs
after the ID-based delete.

## Info

### IN-01: Wrong window size in the zlib header comment [product]

**File:** `runner/png.c:130-132`
**Issue:** The comment says 0x78 is "CM 8 with a 256-byte window". 0x78 has CINFO = 7, which is a 32 KiB window
(RFC 1950 §2.2). A 256-byte window would be 0x08. The bytes are valid, so only the comment is wrong. That matters
because the comment cites the RFC as its source.
**Fix:** "CM 8 with CINFO 7, a 32 KiB window (0x78)".

### IN-02: The bench silently reports nothing, or saturated times, when its inputs go bad [product]

**File:** `runner/main.c:308-313`, `runner/main.c:339-343`, `runner/clock.c:37-39`
**Issue:**
- `muhtari__clock_ns` returns 0 when the clock cannot be read. A failure on the second reading of a frame makes
  `ns` wrap to about 2^64, which saturates that sample to `UINT32_MAX` and corrupts `total_ns`, with no message.
- When `muhtari__bench_format` returns 0 (the report did not fit), `fwrite` writes nothing and the runner still
  exits 0.

**Fix:** Treat a zero clock reading, or a zero report length, as `MUHTARI__EXIT_FAILURE` with a message.

### IN-03: `argc == 0` is reported as out of memory [product]

**File:** `runner/main.c:182-188`
**Issue:** If the runner is executed with an empty `argv`, the code calls `malloc(0)`, which may return NULL, and
then prints "out of memory" with exit 1 instead of a usage error.
**Fix:** Allocate `(size_t)argc + 1u` entries, or return `usage(...)` when `argc < 2` before allocating.

### IN-04: The App client ID is defined twice [release/CI]

**File:** `.github/workflows/dependabot-automerge.yml:36`, `.github/workflows/release.yml:148`
**Issue:** `release.yml` reads `vars.MUHTARI_APP_CLIENT_ID`, and `dependabot-automerge.yml` hard-codes
`Iv23liRlBEtMHhK6V7Yk`. If the App is ever rotated, one of the two silently stays on the old ID.
**Fix:** Have `apply-repo-settings.sh --check` compare the variable with the literal, or use the literal in both
places with one comment.

### IN-05: `check_junit.cmake` matches labels as an unanchored regex [test/tooling]

**File:** `tests/cmake/check_junit.cmake:38-39`
**Issue:** `labels MATCHES "${LABELS}"` with `-DLABELS=portable` would also select a test whose labels contain,
say, `nonportable`, and `EXCLUDE=release` would also drop `prerelease`. No such label exists today. This is a
seed only.
**Fix:** Match whole comma-separated labels: `labels MATCHES "(^|,)(${LABELS})(,|$)"`.

### IN-06: The presets require CMake 4.3 but `CMakeLists.txt` declares 3.25 [release/CI]

**File:** `CMakePresets.json:3`, `CMakeLists.txt:1`
**Issue:** The two minimums differ. An embedder using `add_subdirectory` is told 3.25 works. A contributor with
CMake 3.25 to 4.2 who follows the docs' preset workflow gets a preset-version error rather than a clear minimum.
This is harmless, but the real floor for the documented workflow is stated only in the presets file.
**Fix:** State the preset floor (CMake 4.3) in `docs/testing.md` next to the workflow command, or add a comment
in `CMakeLists.txt` explaining why the two differ.

---

_Reviewed: 2026-10-16T00:00:00Z_
_Reviewer: Claude (gsd-code-reviewer)_
_Depth: standard_
