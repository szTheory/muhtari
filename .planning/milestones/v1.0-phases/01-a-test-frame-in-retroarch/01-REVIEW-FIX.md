---
phase: 01-a-test-frame-in-retroarch
fixed_at: 2026-10-16T20:58:41Z
review_path: .planning/phases/01-a-test-frame-in-retroarch/01-REVIEW.md
iteration: 1
findings_in_scope: 9
fixed: 9
skipped: 0
status: all_fixed
---

# Phase 1: Code Review Fix Report

**Fixed at:** 2026-10-16T20:58:41Z
**Source review:** .planning/phases/01-a-test-frame-in-retroarch/01-REVIEW.md
**Iteration:** 1

**Summary:**
- Findings in scope: 9 (CR-01, WR-01 to WR-08; the IN-* findings are out of scope)
- Fixed: 9
- Skipped: 0

`workflow.use_worktrees` is false, so every edit, commit and local gate ran in the main checkout on branch
`phase/01-a-test-frame-in-retroarch`. The Linux lanes ran in an `ubuntu:26.04` arm64 container over that checkout.
Nothing was pushed.

## Fixed Issues

### CR-01: `--bench N` overflows the sample buffer on 32-bit targets [product]

**Files modified:** `runner/bench.c`, `runner/bench.h`, `runner/main.c`, `docs/runner.md`, `tests/runner/test_runner.c`, `tests/runner/tests.cmake`, `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt`
**Commit:** 772a4f5
**Applied fix:** The new `muhtari__bench_bytes(n)` returns the exact byte count, or 0 when it does not fit in
`size_t`. The guard is a preprocessor `#if SIZE_MAX / 4u < 0xffffffffu`, because a plain C comparison is a constant
on 64-bit, and `-Werror` with `-Wtautological-constant-out-of-range-compare` rejected it. `run()` allocates through
the helper, and the existing `samples == NULL` path exits 1 with "out of memory". The unit test is
`runner.bench.bytes` (labels bench, portable, runner), registered in `ci.txt` and `wasm32.txt`. It checks exact
sizes, and checks 2^30 and 2^32-1 frames: 0 where `size_t` has 32 bits, the exact product otherwise. It allocates
nothing and runs no frames, so it is cheap on 64-bit and meaningful on be32 and wasm32. `docs/runner.md` states the
32-bit limit.

### WR-01: The size tag has no upper bound, so a garbage tag reads out of bounds and loops up to 4 GiB [product]

**Files modified:** `src/instance.c`, `docs/embedding.md`, `tests/core/test_core.c`
**Commit:** c59122f
**Applied fix:** `MUHTARI__SIZE_TAG_MAX` (4096) is rejected before the zero-tail scan. `size_cases` now also runs a
`0xFFFFFFFF` tag through every size-tagged call. It expects `ERR_STRUCT_SIZE` with no byte changed. An ASan build of
the pre-fix `instance.c` fails `core.api.size_tags`, and passes with the fix. `docs/embedding.md` documents the
4096-byte cap.

### WR-02: The docs say `muhtari_run_frame` always writes `audio_count`, but it does not [product]

**Files modified:** `docs/embedding.md`, `tests/core/test_core.c`
**Commit:** adc4001
**Applied fix:** The doc and the test comment now say that `audio_count` is written on every return once `m` and
`io` are not NULL and the size tag is accepted, and that an error writes zero. This is a doc and comment change only.

### WR-03: Pull-request rehearsals produce attestations the release checks cannot tell apart from release attestations [release/CI]

**Files modified:** `scripts/publish.sh`, `.github/workflows/release.yml`, `tests/hygiene/tests.cmake`, `tests/inventory/hygiene.txt`, `docs/maintaining.md`, `docs/testing.md`
**Commit:** f98312f, bd222f6 (follow-up: the fixture tag `v0.1.0` tripped `release.single_version_source.next`, so it was renamed `fixture-release`), 932a7ac (orchestrator follow-up: the provenance note had split the release steps' numbered list, so it now follows step 7)
**Applied fix:**
- `attested` and `verify` now require `SOURCE_REF` and pass `--source-ref "$SOURCE_REF"` to the positive
  `gh attestation verify` calls. The flipped-byte negative check stays `--repo`-only, which makes it the stricter
  check.
- `verify` refuses a non-rehearsal tag unless `SOURCE_REF` is `refs/heads/main`.
- `release.yml` sets `SOURCE_REF: refs/pull/<number>/merge` in the rehearsal job and `refs/heads/main` in the
  publish job.
- The new hygiene check `policy.publish_ref` runs `verify` on a release tag with a pull-request ref, and expects
  exit 1 before any network call.
- `docs/maintaining.md` now states that only `refs/heads/main` attestations are release provenance, and gives the
  pinned `gh attestation verify` command.

The optional separate signer workflow file was not adopted.

### WR-04: Dependabot action updates auto-merge with no cooldown into workflows that hold the App key and the attestation identity [release/CI]

**Files modified:** `.github/dependabot.yml`, `docs/maintaining.md`
**Commit:** 58e7d57
**Applied fix:** Added `cooldown: default-days: 7`. The github-actions ecosystem supports only `default-days`. The
cooldown applies to version updates, not to security updates. The optional exclusion of release actions from
auto-merge was not adopted.

### WR-05: The "planning only" classification can skip CI for a pull request with more than 3,000 files [release/CI]

**Files modified:** `scripts/classify-changes.sh`, `.github/workflows/ci.yml`, `tests/hygiene/tests.cmake`, `docs/testing.md`
**Commit:** b3d801e
**Applied fix:** The classifier now takes the pull request's changed-file count as its argument. It prints
`heavy=true` when the count is above 3000, empty or not a number. `ci.yml` passes
`github.event.pull_request.changed_files` from the event payload, so no extra API call is needed. `ci.classify` gains
four cases: 3000, 3001, empty and non-numeric.

### WR-06: Dependabot auto-merge is gated on the pull request's author, not on who triggered the run [release/CI]

**Files modified:** `.github/workflows/dependabot-automerge.yml`, `tests/cmake/dependabot.cmake`, `docs/maintaining.md`
**Commit:** 693dfd2, 6efcf9a (orchestrator follow-up: rewrapped the header comment)
**Applied fix:**
- The job `if:` now requires both `github.actor` and the pull request's author to be `dependabot[bot]`.
- The `decide` jq program now takes `--arg actor`, and skips unless both the author and the actor are Dependabot.
- `policy.dependabot` now requires the three arguments, and adds author-only, actor-only and neither cases.

A mutated copy without the actor test fails the check (`dependabot[bot] a-person ...: want skip, got 'merge'`).

### WR-07: `publish.sh verify` downloads from `releases/latest/` for real tags, so a re-run of an older release fails or checks the wrong release [release/CI]

**Files modified:** `scripts/publish.sh`, `docs/maintaining.md`
**Commit:** 32a3bd3
**Applied fix:** `verify` now always downloads from `releases/download/<tag>/`. The `latest` alias is left to
`install-line-latest`.

### WR-08: `release_id` reads only the first 100 releases, so rehearsal cleanup can report success while a release remains [release/CI]

**Files modified:** `scripts/publish.sh`
**Commit:** 7ae28e1
**Applied fix:** `release_id` now calls `gh api --paginate`, so lookups by tag in `rehearsal_create` and
`rehearsal_delete` see every release.

## Verification

All commands ran after the last fix, at HEAD bd222f6. Every commit passed the `.githooks` hygiene hook.

On the main checkout (macOS arm64, with brew `llvm@21` found by configure):

| Command | Exit status |
|---|---|
| `cmake --workflow --preset ci` | 0 |
| `cmake --workflow --preset hygiene` | 0 |

In a container, using the recipe in docs/testing.md "Running the Linux lanes locally". The command was
`docker run --rm --platform linux/arm64 -v "$PWD:/src" -v /src/build -w /src ubuntu:26.04`, with the asan, be32 and
wasm32 packages and the pinned CMake and wasmtime:

| Command | Exit status |
|---|---|
| `cmake --workflow --preset asan` | 0 |
| `cmake --workflow --preset nofp` | 0 |
| `cmake --workflow --preset be32` | 0 |
| `cmake --workflow --preset wasm32` | 0 |

`asan` and `nofp` cannot run on this Mac: `asan` needs a `clang-21` binary by that name, and `nofp` needs GCC's
`-mgeneral-regs-only`. Both ran in the container instead.

The orchestrator read every diff, then re-ran the Mac lanes at HEAD 6efcf9a, after its two follow-ups. Those
follow-ups touch only `docs/maintaining.md` and a YAML comment:

| Command | Exit status |
|---|---|
| `cmake --workflow --preset ci` | 0 (64 tests, then the 4 `release` tests) |
| `cmake --workflow --preset hygiene` | 0 (18 tests) |

**Verified locally:**
- CR-01 on 32-bit: be32 and wasm32 run `runner.bench.bytes`, because it carries the `portable` label.
- WR-01 and WR-02.
- The script logic for WR-03 (`policy.publish_ref`), WR-05 (`ci.classify`) and WR-06 (`policy.dependabot`).
- `sh -n scripts/publish.sh`.

**Waits for the pull request's hosted CI or for GitHub:**
- WR-03: whether the rehearsal's attestations verify with `--source-ref refs/pull/<n>/merge`. The release path,
  `refs/heads/main`, runs only on the first release after merge.
- WR-04: Dependabot accepting the `cooldown` key. GitHub validates `dependabot.yml` on the default branch.
- WR-05: `changed_files` reaching the `changes` job from the event payload.
- WR-06: the job-level `github.actor` gate on a real Dependabot pull request.
- WR-07: the tag-URL download, which the rehearsal already used for rehearsal tags. The real-release path runs on
  the next release.
- WR-08: `--paginate` on the releases list, which the rehearsal's `rehearsal-create` and `rehearsal-delete`
  exercise.

---

_Fixed: 2026-10-16T00:00:00Z_
_Fixer: Claude (gsd-code-fixer)_
_Iteration: 1_
