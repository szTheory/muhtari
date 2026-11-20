---
phase: 30-user-jobs-and-player-runner-first-use
plan: "01"
subsystem: documentation
tags: [first-use, runner, libretro, release-guidance, persona-evidence]
requires: []
provides:
  - First-use paths from the README to a RetroArch test card or a lawful cartridge and to finite runner results.
  - Source-qualified runner refusal wording and corrected packaged libretro capability text.
  - Current first-use jobs mapped to dated evidence, confidence, limits and sibling ownership.
affects: [Phase 31, v12.0 milestone delivery]
actuals:
  tokens: 5076.75
  tasks: 3
  commits: 6
commits: 6
plan_head_before: 26b9c88d507b7a35668ee70f2d4003c96ba25ec1
plan_head_after: 1768e45a10e1f0c51a880f2547e8c1c000d56690
tech-stack:
  added: []
  patterns:
    - Version-free public release references link to exact historical receipts.
    - User feedback is distinguished from shipped capability and inferred demand.
key-files:
  created:
    - .planning/phases/30-user-jobs-and-player-runner-first-use/deferred-items.md
  modified:
    - README.md
    - docs/runner.md
    - host/status_text.c
    - tests/runner/tests.cmake
    - docs/libretro.md
    - docs/status.md
    - libretro/muhtari_libretro.info.in
    - .planning/preparation/A7800-ECOSYSTEM-AND-USERS.md
key-decisions:
  - "Keep Atari 2600 image identification while reporting a neutral unsupported refusal; Atari 2600 execution belongs to twentysixhundo."
  - "Keep public package references version-free under the single-version-source check and link the exact historical package receipt."
  - "Use a fix Conventional Commit title when the normal milestone delivery PR is squashed so release-please selects a patch."
patterns-established:
  - "README first-use steps point to detailed controls, BIOS, runner and compatibility references."
  - "The persona map separates one owner's observations from source-backed capability and inferred demand."
requirements-completed:
  - PERSONA-01
  - PLAYER-01
  - RUNNER-01
  - DOCS-01
coverage:
  - id: PERSONA-01
    description: "Player, runner/homebrew developer and C host-integrator jobs are mapped to deliverables and evidence limits."
    requirement: PERSONA-01
    verification:
      - kind: other
        ref: ".planning/preparation/A7800-ECOSYSTEM-AND-USERS.md#31-current-jobs-and-evidence; dated lesson, release receipt and source references checked"
        status: pass
    human_judgment: false
  - id: PLAYER-01
    description: "A player can follow release selection, RetroArch installation, test-card and lawful content-load steps with controls and support limits."
    requirement: PLAYER-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci (exit 0); local install_line consumer against packaged arm64 core (exit 0); simulated libretro tests and linked anchors pass"
        status: pass
    human_judgment: false
  - id: RUNNER-01
    description: "A runner user can identify an image, run a finite sample, interpret output and refusal codes, and find the CLI reference."
    requirement: RUNNER-01
    verification:
      - kind: integration
        ref: "runner process CTest selection (10/10); packaged runner test-card command exit 0 with the documented SHA-256; full CI workflow exit 0"
        status: pass
    human_judgment: false
  - id: DOCS-01
    description: "First-use, status, package metadata and user-job evidence agree with source, checks and historical release records."
    requirement: DOCS-01
    verification:
      - kind: other
        ref: "single_version, docs.budgets, install_line portable links, claims_wording and compat_doc_fresh pass; full CI has 736/736 tests and 8/8 release checks"
        status: pass
    human_judgment: false
duration: 34min
completed: 2026-11-20
status: complete
---

# Phase 30 Plan 01: User Jobs and Player/Runner First Use Summary

**The README now guides players from the release package to a test card or supported cartridge, while runner and package claims match tested behavior.**

## Performance

- **Duration:** 34 min
- **Started:** 2026-11-20T00:59:19Z
- **Completed:** 2026-11-20T01:33:08Z
- **Tasks:** 3
- **Files modified:** 9, including the deferred-items record

## Accomplishments

- Added a runner path for the test card and a lawful cartridge sample, clarified probe versus execution, and updated the shared Atari 2600 refusal while retaining exit 77 and the existing process assertions.
- Documented native RetroArch archive selection, configured install directories, Load Core/Start Core/Load Content, controls, optional BIOS boot and the 204-title evidence limits. The existing macOS command stayed byte-identical.
- Replaced stale release links, corrected Souper/BupChip metadata and the BankSet/HSC log example, and refreshed the three first-use jobs with evidence dates, confidence limits, owners and revisit triggers.

## Task Commits

1. **Task 1 red assertions:** `3a44193` — test(30-01): add failing Atari 2600 refusal assertions.
2. **Task 1 implementation:** `79947df` — fix(30-01): clarify runner first-use and Atari 2600 refusal.
3. **Task 1 budget correction:** `4611f3e` — fix(30-01): keep runner reference within budget.
4. **Task 2:** `75cd9fa` — fix(30-01): clarify RetroArch first-use and core metadata.
5. **Task 3:** `86b9158` — docs(30-01): refresh first-use job evidence.
6. **Release-policy correction:** `1768e45` — fix(30-01): keep public release references version agnostic.

## Files Created/Modified

- `README.md` — release and local-build paths, player setup, content loading, controls and compatibility limits.
- `docs/runner.md`, `host/status_text.c`, `tests/runner/tests.cmake` — probe/run distinction and tested refusal wording.
- `docs/libretro.md`, `docs/status.md`, `libretro/muhtari_libretro.info.in` — accurate player, package and release guidance.
- `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` — current job evidence and sibling ownership boundaries.
- `deferred-items.md` — pre-existing whitespace findings outside the plan's files.

## Decisions Made

- Kept Atari 2600 recognition and refusal intact; `twentysixhundo` owns Atari 2600 execution.
- Kept public release references version-free because `release.single_version_source` enforces `version.txt` as the release version source. The public docs link to the historical audit where the exact tag, source and receipt are recorded; the moving latest-release URL remains version-free.
- The normal v12.0 milestone delivery PR should use a `fix` squash title, then rely on the existing versioning, hosted CI, publication and latest-install checks to produce and verify a patch release.
- The two game reports remain observations from one owner's 2026-10-29 session. They do not change the compatibility ledger or establish representative demand.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking issue] Kept the runner reference within its document budget**
- **Found during:** Task 2 documentation-budget verification.
- **Issue:** Task 1 added enough text to move `docs/runner.md` above its 40,960-byte limit; its pre-task size was 40,947 bytes.
- **Fix:** Folded the probe/execution distinction and sibling boundary into the existing introduction and removed the duplicate command example. Existing CLI options, exit codes, tests and the source-qualified refusal row remain.
- **Files modified:** `docs/runner.md`.
- **Verification:** `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` passed; the focused runner CTest selection passed 10/10.
- **Committed in:** `4611f3e`.

**2. [Rule 3 - Blocking issue] Kept public release references version-free**
- **Found during:** First full `cmake --workflow --preset ci` run.
- **Issue:** `release.single_version_source` rejected the plan-requested literal release number in `docs/runner.md` and `docs/status.md`; hard-coding it would become stale when `version.txt` advances.
- **Fix:** Kept the exact release tag in the `.planning` audit and user-job evidence, described the public package by release date/status, and linked to the historical receipt for its exact identity. The runner row still distinguishes source wording from the previously published package wording.
- **Files modified:** `docs/runner.md`, `docs/status.md`.
- **Verification:** The focused `release.single_version_source`, `docs.budgets`, `policy.claims_wording` and `policy.compat_doc_fresh` tests passed; the full workflow then passed.
- **Committed in:** `1768e45`.

---

**Total deviations:** 2 auto-fixed (Rule 3: 2).
**Impact on plan:** Both fixes preserve the requested behavior and evidence while meeting the existing documentation budget and release-version contract.

## Verification Evidence

- Task 1 test-first RED: after updating only the two existing expectations, `runner.probe.2600` and `runner.hsc_rom.refused` failed specifically because the executable still printed `unsupported: 2600 (planned)` instead of `unsupported: Atari 2600`; both retained exit 77. The shared formatter change then passed the focused 10-test runner selection and the tracer recheck.
- Task 2: `git diff --check` passed; all 25 documentation budgets passed; the portable Emscripten URL and integrator link passed `install_line.cmake`; simulated libretro/input build succeeded; the selected metadata, host, wording and compatibility tests passed 6/6. The install command was compared byte-for-byte with the pre-edit README line.
- Latest release source: official GitHub CLI command `gh release view --repo szTheory/muhtari --json tagName,publishedAt,isDraft,isPrerelease,assets,url`, run at 2026-11-20T01:05:54Z. It returned stable `v0.18.0`, published 2026-11-19T21:00:36Z, 20 assets, and the native libretro archives for Linux, macOS and Windows on x64 and arm64. This is a moving latest-release observation plus the historical v11.0 receipt; it does not claim this phase's corrections are published.
- Final `cmake --workflow --preset ci`: exit 0; 736/736 CI tests and 8/8 release checks passed. The initial run exited 8 on the version-source policy only; after the documented correction, the targeted policy checks passed and the full workflow passed on its single bounded retry.
- Packaged runner command `build/ci/release/extract/runner/bin/muhtari-run --frames 1 --hash-frame 1`: exit 0, exact output `frame 1 sha256 9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4`.
- Apple Silicon package consumer `cmake -DREADME=README.md "-DBASE_URL=file://$PWD/build/ci/" -DHOME_DIR=/tmp/muhtari-install-line-30-01 -P tests/cmake/install_line.cmake`: exit 0; installed `muhtari_libretro.dylib` and `muhtari_libretro.info` under isolated scratch storage, then removed it.
- Changed local links and anchors for controls, BIOS files, test card, portable Emscripten guide, build instructions and the v11.0 receipt were checked. External RetroArch install and directory terms link to official guidance. No RetroArch host/device run or publication occurred.

## Deferred Issues

The [deferred-items record](deferred-items.md) lists trailing whitespace reported by a broad comparison against `origin/main` in `.planning/REQUIREMENTS.md` and `30-RESEARCH.md`. Those files predate this plan and are outside its scope; the plan-scoped whitespace check passes.

## Issues Encountered

The full workflow's first run exposed the single-version-source contract conflict described above. It was corrected before the final run. No authentication gates or unresolved task blockers remain.

## User Setup Required

None — this plan used local builds, simulated host tests and a scratch-directory package install.

## Next Phase Readiness

The player and runner first-use paths are ready. Phase 31 remains responsible for the complete installed C package one-frame example. At milestone delivery, use a `fix` squash title and retain the existing hosted release gates. No owner action is needed for this plan; no host/device or publication scope was opened.

---
*Phase: 30-user-jobs-and-player-runner-first-use*
*Completed: 2026-11-20*

## Self-Check: PASSED

The summary file, all six task commits, the packaged runner, and the local libretro archive exist. No task commit deleted tracked files; the plan-scoped whitespace check passed.
