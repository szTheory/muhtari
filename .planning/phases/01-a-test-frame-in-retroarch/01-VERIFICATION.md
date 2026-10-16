---
phase: 01-a-test-frame-in-retroarch
verified: 2026-10-16T20:30:00Z
status: human_needed
score: 3/5 must-haves verified
covered_files:
  - .github/rulesets/main.json
  - .github/workflows/ci.yml
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
  - host/palette.c
  - include/muhtari.h
  - libretro/libretro.c
  - runner/bench.c
  - runner/main.c
  - runner/png.c
  - scripts/apply-repo-settings.sh
  - scripts/publish.sh
  - src/frame.c
  - src/instance.c
  - src/testcard.c
  - tests/cmake/install_line.cmake
  - tests/cmake/perf_compare.cmake
  - tests/libretro/host.c
covered_digest: "v3:sha256:9e6661e2c399abc001314065cd3197e6768a7c29002dbcbad1dbb4d6d178a0f9"
behavior_unverified: 2
overrides_applied: 0
overrides:
  - must_have: "Merging the phase's pull request publishes a GitHub release of the 18 archives, each holding LICENSE and THIRD-PARTY-NOTICES.md, with SHA256SUMS and verified attestations, through a release pull request the release App merges, after a rehearsal run exercised every publish step"
    reason: "Only the merge can show it, so the ship step confirms it from the push Release run on main (AGENTS rules 6 and 9; GSD-HANDOFF 3, step 8), not UAT. Before the merge the rehearsal ran every publish step on a prerelease (Release run 38957354117) and the real path is wired"
    accepted_by: "the agent, under the owner's standing instruction of 2026-10-16 (DEC.09)"
    accepted_at: "2026-10-16T21:45:03Z"
  - must_have: "The README's one-line install, run against the latest release's version-free archive, puts the core and its .info into RetroArch on an Apple Silicon Mac"
    reason: "releases/latest exists only after the first release, so the ship step confirms install-line-latest from the same Release run (AGENTS rules 6 and 9), not UAT. Before the merge the same line passed against the built zip on macOS arm64 (CI step 'README install line') and against the rehearsal tag (Release run 38957354117)"
    accepted_by: "the agent, under the owner's standing instruction of 2026-10-16 (DEC.09)"
    accepted_at: "2026-10-16T21:45:03Z"
behavior_unverified_items:
  - truth: "Merging the phase's pull request publishes a GitHub release of the 18 archives, each holding LICENSE and THIRD-PARTY-NOTICES.md, with SHA256SUMS and verified attestations, through a release pull request the release App merges, after a rehearsal run exercised every publish step"
    test: "After the phase PR merges, read the push Release run on main (gh run view <id> --json jobs): release-please opens the release PR with the App token and enables auto-merge; after that PR merges, release-ci, publish and install-line-latest succeed and v0.1.0 lists 19 assets (gh release view v0.1.0 --json assets)"
    expected: "v0.1.0 is published and marked latest with the 18 zips and SHA256SUMS, every attestation verifies with --signer-workflow szTheory/muhtari/.github/workflows/release.yml, and the release PR was merged by auto-merge with no manual step"
    why_human: "Not a human judgement: the App-token release-please job, the release PR auto-merge and the real publish run only on push to main, so no pre-merge run can show them. The ship step confirms it from the run URL (AGENTS rule 9). The rehearsal (run 38957354117) already ran every publish step on a prerelease"
  - truth: "The README's one-line install, run against the latest release's version-free archive, puts the core and its .info into RetroArch on an Apple Silicon Mac"
    test: "After v0.1.0 is published, read the install-line-latest job of the same Release run on main"
    expected: "The README line, fetched from releases/latest/download/muhtari-libretro-macos-arm64.zip, extracts cores/muhtari_libretro.dylib and info/muhtari_libretro.info"
    why_human: "releases/latest does not exist until the first real release is published (gh release list is empty today). The same line already passed against the built zip (CI run 38957354177, build (macos-arm64) step 'README install line') and against the rehearsal tag (Release run 38957354117, install-line job)"
human_verification:
  - test: "Read docs/runner.md's '## Test card' section next to the card (muhtari-run --frames 1 --dump-frame 1:card.png) and decide whether a user can tell what each element checks"
    expected: "Each element (corner block, border, overscan lines, 16x16 colour chart, the two checkers, red and green bands) is described clearly and correctly"
    why_human: "Whether prose reads clearly is a judgement no test can make (01-06 coverage, human_judgment: true)"
  - test: "Follow the README's 'Quick start: RetroArch' steps in RetroArch 1.22.2 on the Mac, and read README.md and docs/embedding.md as a new user and as an embedder"
    expected: "The menu names (Main Menu, Load Core, 'Atari - 7800 (muhtari)', Start Core) match what RetroArch shows, and the embedding guide is accurate and complete"
    why_human: "Whether menu text matches what a person sees, and whether docs read well, is a judgement (01-09 coverage, human_judgment: true). The hosted RetroArch job proves the core loads and shows the right frame, not that the written menu path matches the UI"
  - test: "Ship-step confirmation of the two post-merge items in behavior_unverified_items (release-please PR auto-merged, v0.1.0 published with 19 verified assets, install-line-latest green)"
    expected: "All jobs of the push Release run on main succeed; record the run URL"
    why_human: "Observable only after the merge; listed so the item is not lost. It is confirmed from a run, not by human judgement"
---

# Phase 1: A test frame in RetroArch Verification Report

**Phase Goal:** A downloadable release of the library, the runner and the libretro core shows a built-in test card in RetroArch, built and tested on every platform, and merging to `main` publishes the next release with no manual step
**Verified:** 2026-10-16T20:30:00Z
**Status:** human_needed
**Re-verification:** No (initial verification)

Everything this phase can prove before its pull request merges is proven by local commands and green GitHub runs on
65fdfda. Since that commit only `.planning/` has changed (`git diff --stat 65fdfda HEAD -- . ':!.planning'` is
empty), so those runs cover the code at HEAD bcd7bfc. Two parts of the goal are events that happen only after the
merge: the first real release, and the install line against `releases/latest`. They are recorded as
present-behaviour-unverified items for the ship step to confirm. They are not counted as gaps. Two coverage items
are real human judgements about the docs.

## Goal Achievement

### Observable Truths (ROADMAP success criteria)

| # | Truth | Status | Evidence |
|---|-------|--------|----------|
| 1 | `ci` preset builds library, runner and core and passes every test on all six platforms behind `CI required`; `install.consumer|embed.subdirectory` pass; `asan`, `nofp`, `hygiene` pass | ✓ VERIFIED | Local: `cmake --workflow --preset ci` exits 0 (63/63 tests, package, 4/4 release tests); `cmake --workflow --preset hygiene` exits 0 (17/17); `ctest -R 'install.consumer|embed.subdirectory'` exits 0 (4 tests). CI run 38957354177: `build` x6 (linux/macos/windows on x64 and arm64), `asan`, `nofp` x2, `hygiene`, `be32`, `wasm32`, `hash-equality`, `retroarch`, `perf`, `title` and `CI required` all succeeded. Each build job runs `cmake --workflow --preset ci` plus the JUnit completeness checks |
| 2 | No-cart runner writes the 320x242 card with all 256 colours and a platform-identical SHA-256; the libretro host sees 39375000/656711 fps, 320x242, aspect 960/847 and a frame equal to the runner's; the hosted RetroArch screenshot equals the runner's frame | ✓ VERIFIED | `build/ci/runner/muhtari-run --frames 1 --dump-frame 1:card.png --hash-frame 1` prints `frame 1 sha256 9eb011e2…05c4` and writes 232,645 bytes. `src/testcard.c` is a real pure function (cell `$rc` for every hue and luma). `tests/libretro/host.c:372-376` asserts 242, aspect `(float)(960.0/847.0)` and fps `39375000.0/656711.0`, and compares every pixel with the library frame through the palette (`libretro.host.xrgb8888` and `.rgb565` pass). `hash-equality` compares 7 hashes files (6 legs plus be32, `-DCOUNT=7`) and passed. `retroarch` job steps "The screenshot equals the runner's frame" and "The compare's negative control" succeeded |
| 3 | Merging the phase PR publishes the release of 18 archives (LICENSE and notices in each), SHA256SUMS and verified attestations, through a release PR the App merges, after a rehearsal exercised every publish step | ⚠️ PRESENT_BEHAVIOR_UNVERIFIED (post-merge) | Pre-merge proof is complete. Release run 38957354117: `rehearsal` prepared "18 archives from 65fdfda…", uploaded 18 zips and SHA256SUMS, attested them, checked that each attestation verifies and a copy with its last byte flipped fails, ran `gh release verify` and `publish.sh verify` (download plus `--signer-workflow`); `install-line` and `cleanup` succeeded. No releases or tags remain (`gh release list` is empty). `release.archive_contents` passes locally. The real path is wired: App token (`MUHTARI_APP_CLIENT_ID` var, `MUHTARI_APP_PRIVATE_KEY` secret exist), release-please, `gh pr merge --auto --squash`, `release-ci` at the release `sha`, `publish` and `install-line-latest`. These run only on push to main |
| 4 | README one-line install against the latest release puts core and `.info` into RetroArch on Apple Silicon; a CI test runs that line against the built archive on macOS arm64 | ⚠️ PRESENT_BEHAVIOR_UNVERIFIED (post-merge) | The CI half is verified: the `build (macos-arm64)` step "README install line" (`tests/cmake/install_line.cmake` with `BASE_URL=file://…/build/ci/`) succeeded in 38957354177, and the rehearsal's `install-line` job ran it against the rehearsal tag. The "latest release" half cannot run until v0.1.0 exists |
| 5 | `apply-repo-settings.sh` applies settings and the ruleset; PR titles checked as Conventional Commits; `policy.dependabot` and `ctest -L perf` pass; the Ir gate compares each PR with its merge base; the nightly `--bench` records trend data | ✓ VERIFIED | `sh scripts/apply-repo-settings.sh --check` prints `no drift on szTheory/muhtari` and exits 0, twice (idempotent). The ruleset has deletion, non_fast_forward, required_linear_history, pull_request and required_status_checks rules and no bypass actors. The `title` job passed. `policy.dependabot` passes in the hygiene lane. `ctest -L perf`: 8/8 pass (`core.no_alloc_after_load` and the seven `perf.gate.*` cases, including +1.00 % pass and +1.01 % fail). The `perf` job measured head and reported `NO_BASE`, because main has no tree yet, which is the designed report-only verdict. Nightly run 38957353962: bench on ubuntu-26.04, ubuntu-26.04-arm, macos-15 and wasm32, each kept as an unexpired artifact |

**Score:** 3/5 truths verified (2 present, behaviour-unverified because they happen only after the merge)

### Plan must-haves (21 plans)

The plan-level truths are covered by the registered tests. Each runs in the local `ci` and `hygiene` workflows
(both exit 0) and in the green CI, Release and Nightly runs above. Spot checks against the code:

- 01-01/01-02: the header is the five-function block with only `<stddef.h>`/`<stdint.h>` and `MUHTARI_API_VERSION 1`. The pinned hash prints. `version.txt` is 0.0.0. `src/frame.c` writes `audio_count` on every pitch or capacity error.
- 01-05: the PNG is 232,645 bytes, matching the plan.
- 01-07: `SET_SUPPORT_NO_GAME` and `SET_PIXEL_FORMAT` (XRGB8888, or RGB565 under `USE_RGB565`) are in `libretro/libretro.c`.
- 01-14/16: the job set, `CI required` needs (`changes, hygiene, title, build, asan, nofp, be32, hash-equality, retroarch, perf`) and the retroarch negative control are all present and green.
- 01-17/18: `publish.sh` enforces exactly 18 names, the flipped-byte check and the signer-workflow verify.
- 01-20/21: no drift. Measured timeouts, with the remaining ones marked `provisional` with sample counts (main-only release jobs, dependabot, nightly). `workflow.test_gate_timeout` is 3; today's local re-run took 0.75 s incremental. Probe PR #6 is closed and draft, and no PR is open.
- Prohibitions (judgement tier): no planning IDs in code, tests or user docs (git grep for `FRAME-0`, `D-NN` and `DEC.N` in src, runner, libretro, host, include, tests, docs and README finds nothing). No releases or tags created by the agent. Ruleset applied by the owner (`--check` clean). No ROM, BIOS or personal data (the hygiene lane passes). Each is recorded as checked, not as a test-tier enforcement.

### Required Artifacts

| Artifact | Status | Details |
|----------|--------|---------|
| `include/muhtari.h` | ✓ VERIFIED | 2,187 B, five functions, used by core, runner, libretro, consumer |
| `src/testcard.c`, `src/frame.c`, `src/instance.c` | ✓ VERIFIED | Substantive and wired through `muhtari_run_frame` |
| `runner/main.c`, `png.c`, `bench.c` | ✓ VERIFIED | Hash, dump and bench run |
| `libretro/libretro.c`, `.info.in`, vendored `libretro.h` | ✓ VERIFIED | Host tests and the RetroArch job pass |
| `.github/workflows/{ci,release,nightly,dependabot-automerge}.yml` | ✓ VERIFIED | Runs green on 65fdfda (dependabot skipped by design on a non-Dependabot PR) |
| `scripts/{publish,apply-repo-settings,check-title,classify-changes,fetch-pin,hygiene}.sh` | ✓ VERIFIED | Exercised by runs and tests |
| `.github/rulesets/main.json` | ✓ VERIFIED | Applied, no drift |
| README.md (2,565 B), docs/*.md | ✓ VERIFIED | Within budgets; prose quality goes to human verification |

### Key Link Verification

| From | To | Via | Status |
|------|----|-----|--------|
| runner/main.c | libmuhtari | `muhtari_run_frame`, then sha256/png | WIRED (hash prints) |
| libretro/libretro.c | libmuhtari + host/palette.c | `retro_run` → `video_cb` | WIRED (host tests, RetroArch job) |
| ci.yml build | `cmake --workflow --preset ci` | same command as local | WIRED |
| release.yml | ci.yml | `workflow_call` with PR head / release sha | WIRED (nested CI green in 38957354117) |
| release.yml publish | scripts/publish.sh | prepare/upload/attested/verify | WIRED (rehearsal) |
| CI required | all heavy jobs | `needs` plus jq roll-up | WIRED (`policy.ci_required`) |

### Data-Flow Trace (Level 4)

| Artifact | Data | Source | Real data | Status |
|----------|------|--------|-----------|--------|
| Runner hash and PNG | frame bytes | `muhtari__testcard_pixel` via `muhtari_run_frame` | Yes | ✓ FLOWING |
| libretro video_cb | XRGB8888/RGB565 pixels | library frame → `muhtari__palette_rgb*` | Yes | ✓ FLOWING |
| Nightly trend | fps_milli and percentiles | `muhtari-run --bench 300000` | Yes (artifacts 235-243 B) | ✓ FLOWING |

### Behavioral Spot-Checks

| Behavior | Command | Result | Status |
|----------|---------|--------|--------|
| ci workflow | `cmake --workflow --preset ci` | exit 0; 63 + 4 tests pass | ✓ PASS |
| hygiene workflow | `cmake --workflow --preset hygiene` | exit 0; 17 tests pass | ✓ PASS |
| Pinned hash and PNG | `muhtari-run --frames 1 --dump-frame 1:card.png --hash-frame 1` | `9eb011e2…05c4`, 232,645 B | ✓ PASS |
| Install/embed | `ctest -R 'install.consumer|embed.subdirectory'` | exit 0 | ✓ PASS |
| Perf label | `ctest -L perf` | 8/8 | ✓ PASS |
| Settings | `scripts/apply-repo-settings.sh --check` (x2) | `no drift`, exit 0 | ✓ PASS |
| GitHub runs | `gh run view 38957354177 / 38957354117 / 38957353962 --json jobs` | every job success; main-only release jobs skipped by design | ✓ PASS |

Run URLs: CI https://github.com/szTheory/muhtari/actions/runs/38957354177, Release
https://github.com/szTheory/muhtari/actions/runs/38957354117, Nightly
https://github.com/szTheory/muhtari/actions/runs/38957353962.

### Probe Execution

No `scripts/*/tests/probe-*.sh` exist and no plan declares one. "Probe PR" here means the draft pull request #6,
which is closed and draft as designed. Status: not applicable.

### Requirements Coverage

| Requirement | Source Plans | Status | Evidence |
|-------------|--------------|--------|----------|
| FRAME-01 | 01,02,03,13,14,15,21 | ✓ SATISFIED | SC1 |
| FRAME-02 | 08 | ✓ SATISFIED | install/embed tests pass locally and on six legs |
| FRAME-03 | 01,02,04,05,06,13,14 | ✓ SATISFIED | SC2, hash equality across 7 lanes |
| FRAME-04 | 04,05,07,16 | ✓ SATISFIED | libretro host tests and the hosted RetroArch pixel compare |
| FRAME-05 | 10,11,12,14 | ✓ SATISFIED | asan, nofp x2 and hygiene green; abi tests |
| FRAME-06 | 09,10,12,17,18 | ✓ SATISFIED pre-merge / ⚠️ publish on merge pending | 18 archives built on every PR and rehearsed end to end; the real publish happens after the merge (SC3) |
| FRAME-07 | 09,17,18 | ✓ SATISFIED pre-merge / ⚠️ latest pending | install line passes against the built zip and the rehearsal tag; the run against `releases/latest` happens after the merge (SC4) |
| FRAME-08 | 15,19,20,21 | ✓ SATISFIED | settings no drift, title check, dependabot policy, perf gate wired (arming is a post-merge todo by design) |
| FRAME-09 | 02,06,19 | ✓ SATISFIED | `core.no_alloc_after_load`, nightly bench artifacts |

All nine IDs appear in plan frontmatter and in SUMMARY `requirements-completed`. REQUIREMENTS.md maps no
additional ID to Phase 1, so there are no orphaned requirements.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|------|------|---------|----------|--------|
| (tracked files outside .planning) | - | TBD/FIXME/XXX | none found | Debt-marker gate clean |
| runner/main.c | 267-268 | `(size_t)opt->frames * sizeof *samples` with no overflow guard (review CR-01) | ⚠️ Warning | On 32-bit targets (wasm32, be32) `--bench N` with N >= 2^30 wraps the size and overflows the heap. It touches the FRAME-09 bench must-have only for huge N; `--bench 300000` works on every lane, wasm32 included. It belongs to the code-review fix step (AGENTS rule 7), not gap closure |
| src/instance.c | 13-29 | size tag has no upper bound (review WR-01) | ⚠️ Warning | Unbounded read on a garbage tag; a rule 4 concern; code-review fix step |
| docs/embedding.md | 108 | claims `audio_count` is always written (review WR-02) | ℹ️ Info | The doc overstates it; the plan truth (pitch and capacity errors write it) holds in `src/frame.c` |

The remaining review findings (WR-03 to WR-08, IN-01 to IN-06) are release and CI hardening items, all `open` in
01-REVIEW-DISPOSITION.md. They do not falsify a must-have.

### Human Verification Required

1. **Test-card docs clarity.** Read `docs/runner.md#test-card` against the dumped card and judge whether each element is described clearly.
2. **README and RetroArch menu path.** Follow the README quick start in RetroArch 1.22.2 and confirm the menu names match. Read README.md and docs/embedding.md for accuracy.
3. **Post-merge confirmation (ship step, from a run, not a judgement).** After the phase PR merges with a `feat:` squash title, confirm the push Release run on main: the release PR is opened and auto-merged, v0.1.0 is published with 19 verified assets, and `install-line-latest` is green. This closes SC3 and SC4.

### Gaps Summary

There are no gaps. Every criterion that can be shown before the merge is shown by a command with exit 0 and by green
runs. SC3 and SC4 are fully built and rehearsed (a prerelease with every publish step, then cleanup, and the install
line against the built zip and the rehearsal tag), but their final form, the first real release and
`releases/latest`, exists only after the merge. Other post-merge items, which are designed and do not count as
missing work, are tracked as todos: arming the Ir gate, the nightly rolling issue on main, and promoting wasm32 after
10 runs. The critical review finding CR-01 is a real defect in the runner's bench allocation on 32-bit targets. It
belongs to the code-review fix step and should be fixed before ship.

---

_Verified: 2026-10-16T20:30:00Z_
_Verifier: Claude (gsd-verifier)_
