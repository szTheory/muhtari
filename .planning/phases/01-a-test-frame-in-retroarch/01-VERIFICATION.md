---
phase: 01-a-test-frame-in-retroarch
verified: 2026-10-16T21:59:35Z
status: passed
score: 5/5 must-haves verified (3 verified, 2 passed by override)
covered_files:
  - .github/dependabot.yml
  - .github/rulesets/main.json
  - .github/workflows/ci.yml
  - .github/workflows/dependabot-automerge.yml
  - .github/workflows/nightly.yml
  - .github/workflows/release.yml
  - .planning/phases/01-a-test-frame-in-retroarch/01-01-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-01-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-02-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-02-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-03-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-03-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-04-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-04-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-05-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-05-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-06-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-06-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-07-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-07-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-08-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-08-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-09-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-09-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-10-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-10-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-11-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-11-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-12-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-12-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-13-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-13-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-14-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-14-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-15-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-15-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-16-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-16-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-17-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-17-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-18-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-18-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-19-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-19-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-20-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-20-SUMMARY.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-21-PLAN.md
  - .planning/phases/01-a-test-frame-in-retroarch/01-21-SUMMARY.md
  - CMakePresets.json
  - README.md
  - docs/embedding.md
  - docs/maintaining.md
  - docs/runner.md
  - docs/testing.md
  - host/palette.c
  - include/muhtari.h
  - libretro/libretro.c
  - runner/bench.c
  - runner/bench.h
  - runner/main.c
  - runner/png.c
  - scripts/apply-repo-settings.sh
  - scripts/classify-changes.sh
  - scripts/publish.sh
  - src/frame.c
  - src/instance.c
  - src/testcard.c
  - tests/cmake/api_names.cmake
  - tests/cmake/dependabot.cmake
  - tests/cmake/install_line.cmake
  - tests/cmake/perf_compare.cmake
  - tests/core/test_core.c
  - tests/hygiene/tests.cmake
  - tests/libretro/host.c
  - tests/policy/tests.cmake
  - tests/runner/test_runner.c
covered_digest: "v3:sha256:74da9646e473380e285a412da0ef5e3192864509dbcd6fec763d7525a66057be"
behavior_unverified: 0
overrides_applied: 2
overrides:
  - must_have: "Merging the phase's pull request publishes a GitHub release of the 18 archives, each holding LICENSE and THIRD-PARTY-NOTICES.md, with SHA256SUMS and verified attestations, through a release pull request the release App merges, after a rehearsal run exercised every publish step"
    reason: "Only the merge can show it, so the ship step confirms it from the push Release run on main (AGENTS rules 6 and 9; GSD-HANDOFF 3, step 8), not UAT. Before the merge the rehearsal ran every publish step on a prerelease (Release run 38957354117) and the real path is wired"
    accepted_by: "the agent, under the owner's standing instruction of 2026-10-16 (DEC.09)"
    accepted_at: "2026-10-16T21:45:03Z"
  - must_have: "The README's one-line install, run against the latest release's version-free archive, puts the core and its .info into RetroArch on an Apple Silicon Mac"
    reason: "releases/latest exists only after the first release, so the ship step confirms install-line-latest from the same Release run (AGENTS rules 6 and 9), not UAT. Before the merge the same line passed against the built zip on macOS arm64 (CI step 'README install line') and against the rehearsal tag (Release run 38957354117)"
    accepted_by: "the agent, under the owner's standing instruction of 2026-10-16 (DEC.09)"
    accepted_at: "2026-10-16T21:45:03Z"
re_verification:
  previous_status: human_needed
  previous_score: 3/5
  gaps_closed: []
  gaps_remaining: []
  regressions: []
human_verification:
  - test: "When the phase pull request is opened at ship, wait for its pull_request Release run (it triggers because scripts/publish.sh and release.yml differ from main) and read it: gh run view <id> --json jobs. Merge only after it is green"
    expected: "ci, rehearsal, install-line and cleanup all succeed on the PR head commit; the rehearsal's 'publish.sh attested dist' and 'publish.sh verify' steps pass with --source-ref refs/pull/<N>/merge, and cleanup leaves no rehearsal release or tag (gh release list is empty)"
    why_human: "Not a judgement, a run to confirm (DEC.09). The only rehearsal on record (Release run 38957354117, on 65fdfda) predates the code-review fixes that changed publish.sh attested/verify (new --source-ref on both gh attestation verify calls, tag-URL downloads, --paginate in release_id) and release.yml (SOURCE_REF env). No Release run has exercised the current publish.sh, and the rehearsal is not a required check (the ruleset requires only 'CI required'), so auto-merge would not wait for it"
  - test: "After the merge, confirm the push Release run on main (the deferred UAT item 3 and both overrides): release-please opens the release PR with the App token and auto-merges it, then release-ci, publish and install-line-latest succeed"
    expected: "v0.1.0 is published as latest with 18 zips and SHA256SUMS; publish.sh verify passes with --signer-workflow .../release.yml and --source-ref refs/heads/main; install-line-latest extracts cores/muhtari_libretro.dylib and info/muhtari_libretro.info; record the run URL"
    why_human: "Observable only after the merge; confirmed from a run at ship, not by a person's judgement. The refs/heads/main source-ref path has never run anywhere"
---

# Phase 1: A test frame in RetroArch Verification Report

**Phase Goal:** A downloadable release of the library, the runner and the libretro core shows a built-in test card in RetroArch, built and tested on every platform, and merging to `main` publishes the next release with no manual step
**Verified:** 2026-10-16T21:59:35Z
**Status:** human_needed
**Re-verification:** Yes. The previous report (human_needed, 3/5, at bcd7bfc) had no gaps. Since then the code-review fix round changed 25 files outside `.planning/` (runner bench, size-tag cap, publish.sh, release.yml, dependabot workflow, classify-changes, docs, tests), UAT passed its two doc items, and two overrides were added. Every must-have was re-checked against the current code.

## Evidence base

- Local, at HEAD e36e413: `cmake --workflow --preset ci` exit 0 (66/66 tests, then 4/4 release tests, three macOS arm64 zips packaged); `cmake --workflow --preset hygiene` exit 0 (18/18); `ctest -L perf` 8/8; `ctest -R 'install.consumer|embed.subdirectory'` 4/4; `ctest -R 'policy.dependabot|policy.publish'` 5/5 in the hygiene tree; `sh scripts/apply-repo-settings.sh --check` prints `no drift on szTheory/muhtari`, exit 0; `muhtari-run --frames 1 --dump-frame 1:card.png --hash-frame 1` prints `frame 1 sha256 9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4`, exit 0, 232,645 bytes.
- CI run https://github.com/szTheory/muhtari/actions/runs/38971166983 (workflow_dispatch, success) on 24cf9fc, which is `origin/phase/01-a-test-frame-in-retroarch`. `git diff --stat 24cf9fc HEAD -- . ':!.planning'` lists only `AGENTS.md`, so this run covers all code at HEAD. Jobs: build x6, asan, nofp x2, hygiene, be32, wasm32, hash-equality, retroarch, perf and `CI required` succeeded; `title` skipped (no pull request on a dispatch).
- Earlier runs on 65fdfda: CI 38957354177 (title job success), Release 38957354117 (rehearsal), Nightly 38957353962.

## Goal Achievement

### Observable Truths (ROADMAP success criteria)

| # | Truth | Status | Evidence |
|---|-------|--------|----------|
| 1 | `ci` preset builds library, runner and core and passes every test on all six platforms behind `CI required`; `install.consumer|embed.subdirectory` pass; `asan`, `nofp`, `hygiene` pass | ✓ VERIFIED | Local ci and hygiene workflows exit 0. CI run 38971166983 on the current code: all six build legs, asan, nofp x2, hygiene and `CI required` succeeded |
| 2 | No-cart runner writes the 320x242 card with all 256 colours and a platform-identical SHA-256; libretro host sees 39375000/656711 fps, 320x242, aspect 960/847, frame equal to the runner's; hosted RetroArch screenshot equals the runner's frame | ✓ VERIFIED | Pinned hash printed locally (above). `hash-equality` (6 legs plus be32) and `retroarch` (RetroArch 1.22.2 on macos-15, screenshot compare and its negative control) succeeded in 38971166983. `libretro.host.*` tests pass in the local ci run. UAT test 1 checked the card pixel by pixel against docs/runner.md |
| 3 | Merging the phase PR publishes the release of 18 archives with LICENSE and notices, SHA256SUMS and verified attestations, through a release PR the App merges, after a rehearsal exercised every publish step | PASSED (override) | Override: post-merge only, confirmed at ship from the push Release run (accepted by the agent under DEC.09, 2026-10-16T21:45:03Z). The wiring is present: release-please with the App token, `gh pr merge --auto --squash`, `release-ci` at the release sha, `publish` (prepare, upload, attest, attested, publish as latest, verify) and `install-line-latest` in `.github/workflows/release.yml`. **Caveat (warning):** the override's pre-merge evidence, Release run 38957354117, ran the old `publish.sh`. The fix round added `--source-ref "$SOURCE_REF"` to both attestation checks, switched verify to the tag URL and paginated `release_id`. `policy.publish_ref` tests only the refuse-before-network branch. No Release run has exercised the new attested/verify path, so it is a ship-step item below |
| 4 | README one-line install against the latest release puts the core and `.info` into RetroArch on Apple Silicon; a CI test runs that line against the built archive on macOS arm64 | PASSED (override) | Override: `releases/latest` exists only after the first release. The CI half holds on current code: the `build (macos-arm64)` leg of 38971166983 succeeded, and that leg runs `tests/cmake/install_line.cmake` against the built zip. `gh release list` is empty, as expected before the merge |
| 5 | `apply-repo-settings.sh` applies settings and the ruleset; PR titles checked as Conventional Commits; `policy.dependabot` and `ctest -L perf` pass; the Ir gate compares each PR with its merge base; the nightly `--bench` records trend data | ✓ VERIFIED | `--check`: no drift, exit 0. The ruleset requires `CI required` (integration 15368) and has no bypass actors. `policy.dependabot` and its self test pass. `ctest -L perf` 8/8. The `perf` job succeeded in 38971166983. The `title` job succeeded in 38957354177, and `scripts/check-title.sh` has not changed since. Nightly 38957353962 recorded the bench artifacts. `runner/bench.c`'s CR-01 fix (`muhtari__bench_bytes` returns 0 on overflow, so allocation fails cleanly) passes on the wasm32 and be32 legs |

**Score:** 5/5 (3 verified, 2 passed by override, 0 present-but-behaviour-unverified)

### Plan must-haves (21 plans)

The plan-level truths are covered by registered tests in the ci and hygiene workflows. Both exit 0 locally and are green on all CI lanes for the current code. Spot checks of code changed since the last report:
- `src/instance.c`: the size tag is capped at 4096 (`MUHTARI__SIZE_TAG_MAX`) before any tail read.
- `runner/main.c:268`: `--bench` uses `muhtari__bench_bytes`, with no wrapped allocation on 32-bit targets.
- `scripts/classify-changes.sh`: a pull request with more than 3000 files, or no count, is classed heavy.
- `tests/cmake/api_names.cmake` (`docs.api_names` and its self test): every public name is mentioned in `docs/embedding.md`.
- Prohibitions (judgement tier, checked, not test-enforced): `git grep` finds no `FRAME-0`, `DEC.N` or `D-NN` IDs in src, runner, libretro, host, include, tests, docs or README. No TBD/FIXME/XXX outside `.planning/`. No release or tag exists. The hygiene lane passes (no personal data, no unlisted binaries, no GPL).

### Required Artifacts

| Artifact | Status | Details |
|----------|--------|---------|
| `include/muhtari.h`, `src/{testcard,frame,instance}.c` | ✓ VERIFIED | Substantive, wired through `muhtari_run_frame`, and the hash flows |
| `runner/{main,png,bench}.c` | ✓ VERIFIED | Hash, dump and bench run. The bench overflow is fixed |
| `libretro/libretro.c`, `host/palette.c` | ✓ VERIFIED | Host tests and the RetroArch job pass |
| `.github/workflows/{ci,release,nightly,dependabot-automerge}.yml` | ✓ VERIFIED (release.yml rehearsal not re-run on current code) | CI green on 24cf9fc. Release last ran on 65fdfda |
| `scripts/{publish,apply-repo-settings,check-title,classify-changes,fetch-pin,hygiene}.sh` | ✓ VERIFIED | Exercised by tests and runs. publish.sh attested/verify are current-code-unexercised (see caveat) |
| `.github/rulesets/main.json` | ✓ VERIFIED | Applied, no drift |

### Key Link Verification

| From | To | Via | Status |
|------|----|-----|--------|
| runner/main.c | libmuhtari | `muhtari_run_frame`, then sha256/png | WIRED (pinned hash prints) |
| libretro/libretro.c | libmuhtari + host/palette.c | `retro_run` → `video_cb` | WIRED (host tests, RetroArch job) |
| ci.yml build | `cmake --workflow --preset ci` | same command as local | WIRED |
| release.yml rehearsal/publish | scripts/publish.sh | prepare/upload/attested/verify with `SOURCE_REF` | WIRED. `gh attestation verify --source-ref` and `gh api --paginate` exist in gh 2.101.0. The values (`refs/pull/N/merge` for a rehearsal, `refs/heads/main` for a release) match GITHUB_REF for those events, but no run has confirmed them |
| Ruleset | `CI required` | required_status_checks | WIRED. The Release rehearsal is not required |

### Data-Flow Trace (Level 4)

| Artifact | Data | Source | Real data | Status |
|----------|------|--------|-----------|--------|
| Runner hash and PNG | frame bytes | `muhtari__testcard_pixel` via `muhtari_run_frame` | Yes | ✓ FLOWING |
| libretro video_cb | XRGB8888/RGB565 | library frame → palette | Yes | ✓ FLOWING |
| Nightly trend | fps and percentiles | `muhtari-run --bench 300000` | Yes (artifacts on 38957353962) | ✓ FLOWING |

### Behavioral Spot-Checks

| Behavior | Command | Result | Status |
|----------|---------|--------|--------|
| ci workflow | `cmake --workflow --preset ci` | exit 0, 66 + 4 pass | ✓ PASS |
| hygiene workflow | `cmake --workflow --preset hygiene` | exit 0, 18 pass | ✓ PASS |
| Pinned hash/PNG | `build/ci/runner/muhtari-run --frames 1 --dump-frame 1:card.png --hash-frame 1` | `9eb011e2…05c4`, 232,645 B | ✓ PASS |
| Perf | `ctest --test-dir build/ci -L perf` | 8/8 | ✓ PASS |
| Install/embed | `ctest --test-dir build/ci -R 'install.consumer|embed.subdirectory'` | 4/4 | ✓ PASS |
| Policy | `ctest --test-dir build/hygiene -R 'policy.dependabot|policy.publish'` | 5/5 | ✓ PASS |
| Settings | `sh scripts/apply-repo-settings.sh --check` | no drift, exit 0 | ✓ PASS |
| CI on current code | `gh run view 38971166983 --json jobs` | all success (title skipped on dispatch) | ✓ PASS |

### Probe Execution

No `scripts/*/tests/probe-*.sh` exists and no plan declares one. Not applicable.

### Requirements Coverage

| Requirement | Source Plans | Status | Evidence |
|-------------|--------------|--------|----------|
| FRAME-01 | 01,02,03,13,14,15,21 | ✓ SATISFIED | SC1 |
| FRAME-02 | 08 | ✓ SATISFIED | install/embed 4/4; header C++ compile in ci |
| FRAME-03 | 01,02,04,05,06,13,14 | ✓ SATISFIED | SC2 |
| FRAME-04 | 04,05,07,16 | ✓ SATISFIED | host tests, retroarch job on 24cf9fc |
| FRAME-05 | 10,11,12,14 | ✓ SATISFIED | asan, nofp x2, hygiene green |
| FRAME-06 | 09,10,12,17,18 | ✓ SATISFIED pre-merge (override for publish) | 18 archives built on every leg. Rehearsal on record on 65fdfda. Re-rehearsal of the current publish.sh at ship (human_verification 1) |
| FRAME-07 | 09,17,18 | ✓ SATISFIED pre-merge (override for latest) | install line against the built zip on macos-arm64 |
| FRAME-08 | 15,19,20,21 | ✓ SATISFIED | settings no drift, title check, policy.dependabot, perf gate wired |
| FRAME-09 | 02,06,19 | ✓ SATISFIED | `core.no_alloc_after_load`, nightly bench |

All nine IDs appear in plan frontmatter. REQUIREMENTS.md maps no other ID to Phase 1, so no requirement is orphaned.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|------|------|---------|----------|--------|
| tracked files outside .planning | - | TBD/FIXME/XXX | none | Debt-marker gate clean |
| `.github/workflows/dependabot-automerge.yml` | 18, 45-49 | Auto-merge armed by Dependabot stays armed after a person pushes. The gate uses `github.actor`, not `github.triggering_actor`, and `gh pr merge` has no `--match-head-commit` (review WR-01, open) | ⚠️ Warning | Security hardening. It does not falsify `policy.dependabot`, but `docs/maintaining.md` promises more than the workflow does. A seed or later fix under rule 7 |
| `scripts/publish.sh` | 154-159 | `rehearsal-delete` treats a failed lookup as "gone" (review WR-02, open) | ⚠️ Warning | A rehearsal cleanup can report success on an API error. It does not affect a real publish |
| `scripts/publish.sh` | 81 | `attested` has no `--signer-workflow` before the immutable publish (review WR-03, open) | ⚠️ Warning | Today only release.yml can attest, so the gate holds in practice |
| `src/instance.c` | 22 | the 4096 cap has no boundary test (review WR-04, open; test code, so a seed) | ℹ️ Info | The behaviour is present. Only its pin is missing |

### Human Verification Required

Both items are run confirmations for the ship step (DEC.09), not judgements by a person.

1. **Re-run the rehearsal on the current publish.sh before merging.** Open the phase PR, wait for its pull_request Release run and confirm ci, rehearsal, install-line and cleanup succeed on the head commit. Merge only after that. Why: the only rehearsal on record ran the pre-fix `publish.sh`. The new `--source-ref` checks have never run, and the rehearsal is not a required check.
2. **Post-merge Release run on main.** The release PR is opened and auto-merged by the App. v0.1.0 is published as latest with 19 verified assets (`--source-ref refs/heads/main`), and `install-line-latest` is green. Record the run URL. This closes the two overrides and the deferred UAT item 3.

### Gaps Summary

There are no gaps. Everything that can be shown before the merge is shown on the current code, by local commands that exit 0 and by CI run 38971166983. The two post-merge truths are carried as overrides. One thing has regressed since the last report. The code-review fix round changed the publish path's attestation checks, and no Release run has exercised them since. A wrong `--source-ref` would stop the first real publish at `attested`, before anything becomes immutable, and leave a draft that needs a manual step. That breaks the "no manual step" part of the goal. The phase PR's own Release rehearsal will exercise the change, but it is not a required check, so the ship step must wait for it. The open review warnings (WR-01 to WR-03) are hardening items and do not falsify a must-have.

---

_Verified: 2026-10-16T21:59:35Z_
_Verifier: Claude (gsd-verifier)_
