---
phase: 01-a-test-frame-in-retroarch
plan: 16
subsystem: ci
tags: [retroarch, libretro, github-actions, macos, sips, bmp, e2e, negative-control]

requires:
  - phase: 01-07
    provides: muhtari_libretro core with supports_no_game and the generated .info
  - phase: 01-14
    provides: ci.yml with the build matrix, the archives-macos-arm64 artifact and the CI required roll-up
  - phase: 01-15
    provides: policy.ci_required and hygiene.timeout_comments, which the new job satisfies
provides:
  - "pins.txt row retroarch-dmg: RetroArch 1.22.2 Metal DMG, SHA-256 81b79121...db434"
  - "tests/retroarch/: test.cfg.in, run_retroarch.cmake, bmp.c, compare.c (241 lines)"
  - "retroarch job on macos-15 in CI required's needs, with a one-pixel negative control"
  - "docs/testing.md Hosted RetroArch section and CI table row"
affects: [01-17 release workflow_call, 01-21 measured timeouts and probe close, phase 3 own diagnostic ROM case]

actuals:
  tokens: 5100
  tasks: 2
  commits: 2
plan_head_before: 9967dd46e987f11e77482dadc4795eb73b85ff5c
plan_head_after: f9db90b8c859799aec79ba967ee93ed334f11e62

tech-stack:
  added: ["RetroArch 1.22.2 Metal (hosted CI only, pinned DMG)"]
  patterns:
    - "A hosted end-to-end check is a CI step, not a CTest, so it never self-skips off its platform"
    - "Both images go through the same decoder (sips to BMP), so the compare reads one format"
    - "A negative control in the same job feeds the compare a one-byte fault at an offset read from the file header"

key-files:
  created:
    - tests/retroarch/test.cfg.in
    - tests/retroarch/run_retroarch.cmake
    - tests/retroarch/bmp.c
    - tests/retroarch/compare.c
  modified:
    - pins.txt
    - .github/workflows/ci.yml
    - tests/cmake/ci_required.cmake
    - docs/testing.md

key-decisions:
  - "Hosted RetroArch 1.22.2 starts the no-game core with -L and no content: research assumption A5 holds on macos-15 (run 38939482093), so the blocker rule did not trigger"
  - "The compare reads two BMPs, both produced by sips, instead of the seed's PPM plus BMP; the PPM reader is dropped"
  - "The BMP reader accepts 24 and 32-bit uncompressed (and 32-bit standard bitfields) in either row order; sips wrote a 24-bit top-down BMP for the runner PNG"
  - "The job checks the app bundle's CFBundleShortVersionString is 1.22.2 (HOSTS section 3: a version check, not another case)"
  - "The job timeout is 15 minutes, marked provisional; measured wall time is 17 to 22 seconds"

patterns-established:
  - "Seeded nesturbator files carry no header; the commit body names the seed commit and paths"

requirements-completed: [FRAME-04]

coverage:
  - id: D1
    description: "Hosted RetroArch 1.22.2 Metal, from the DMG pinned by SHA-256 and version-checked, runs the built core unattended with -L and no content for 5 frames inside an isolated HOME and CFFIXED_USER_HOME, and writes a screenshot"
    requirement: FRAME-04
    verification:
      - kind: e2e
        ref: "gh run watch 38939482093 --exit-status (exit 0), https://github.com/szTheory/muhtari/actions/runs/38939482093/job/117103780328, step RetroArch runs the core with no content"
        status: pass
      - kind: e2e
        ref: "gh run watch 38939999680 --exit-status (exit 0), https://github.com/szTheory/muhtari/actions/runs/38939999680/job/117105438069"
        status: pass
    human_judgment: false
  - id: D2
    description: "RetroArch's screenshot, decoded by sips and bmp.c, equals the runner's --dump-frame 5 PNG decoded the same way, pixel for pixel at 320x242"
    requirement: FRAME-04
    verification:
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/38939999680/job/117105438069, step The screenshot equals the runner's frame, grep for 320x242, every pixel equal (exit 0)"
        status: pass
    human_judgment: false
  - id: D3
    description: "The compare fails on a one-pixel fault: inverting one byte of pixel (7,3) makes it exit 1 naming 7,3 first"
    requirement: FRAME-04
    verification:
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/38939999680/job/117105438069, step The compare's negative control (success)"
        status: pass
      - kind: other
        ref: "the same step run locally with a compare that always prints equal exits 1"
        status: pass
    human_judgment: false
  - id: D4
    description: "The retroarch job is in CI required's needs and in the roll-up test's synthetic cases from its first run"
    requirement: FRAME-04
    verification:
      - kind: unit
        ref: "cmake --workflow --preset hygiene (exit 0, 13 tests), policy.ci_required"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/38939482093, CI required success with retroarch in needs"
        status: pass
    human_judgment: false
  - id: D5
    description: "pins.txt has the retroarch-dmg row in the checked format, and docs/testing.md documents the job, the pin, rerunning it and the no-content blocker rule"
    requirement: FRAME-04
    verification:
      - kind: unit
        ref: "cmake --workflow --preset hygiene (exit 0), policy.pins"
        status: pass
      - kind: unit
        ref: "cmake -DROOT=. -P tests/cmake/budgets.cmake (exit 0)"
        status: pass
    human_judgment: false

duration: 11min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 16: Hosted RetroArch Summary

**A required `retroarch` job on macos-15 runs RetroArch 1.22.2 Metal from the SHA-256-pinned DMG. RetroArch loads the built core with `-L` and no content, and its 5-frame screenshot, decoded by sips, equals the runner's frame pixel for pixel at 320x242. A one-byte negative control proves the compare can fail.**

## Performance

- **Duration:** 11 min
- **Started:** 2026-10-16T17:19:48Z
- **Completed:** 2026-10-16T17:30:53Z
- **Tasks:** 2
- **Files modified:** 8

## Accomplishments

- The no-content launch works on the hosted runner, which settles research assumption A5. RetroArch ran 5 frames and wrote a 2,032-byte screenshot about 4 to 6 seconds after launch.
- The compare passed on the first hosted run (38939482093) and again with the negative control (38939999680). The `retroarch` job took 22 s and 17 s of wall time.
- `tests/retroarch/` is 241 lines in total, within the 300-line budget: bmp.c 87, compare.c 54, run_retroarch.cmake 54, test.cfg.in 46.
- The job is in `CI required`'s needs and in the roll-up test's synthetic job lists.

## Evidence

- `gh run watch 38939482093 --exit-status` exit 0, https://github.com/szTheory/muhtari/actions/runs/38939482093 (head 9b8282e, every job success, `retroarch` success)
- `gh run watch 38939999680 --exit-status` exit 0, https://github.com/szTheory/muhtari/actions/runs/38939999680 (head f9db90b, `retroarch` and `CI required` success, negative-control step success)
- `cmake --workflow --preset hygiene` exit 0 (13 tests)
- `cat tests/retroarch/* | wc -l` gives 241

## Task Commits

1. **Task 1: hosted RetroArch, pinned DMG, no-content core, screenshot compare** - `9b8282e` (ci)
2. **Task 2: negative control and the docs section** - `f9db90b` (ci)

## Files Created/Modified

- `pins.txt` - the `retroarch-dmg` row (buildbot stable 1.22.2 universal `RetroArch_Metal.dmg`)
- `tests/retroarch/test.cfg.in` - GPU screenshot off, on-screen text off, null audio, no pause when unfocused, bundled asset unpacking off, every directory under the run directory
- `tests/retroarch/run_retroarch.cmake` - isolated home, generated config, a `--verbose` launch with no content bounded at 50 s, and a failure with the log on a non-zero exit or a missing shot
- `tests/retroarch/bmp.c` - bounds-checked uncompressed BMP reader to RGB24
- `tests/retroarch/compare.c` - prints the size when the images are equal; exits 1 naming a size mismatch or the first differing pixel
- `.github/workflows/ci.yml` - `retroarch` job (needs changes and build; heavy only; 15 min provisional), added to `CI required`'s needs
- `tests/cmake/ci_required.cmake` - `retroarch` added to the synthetic `all` and `light` job lists
- `docs/testing.md` - Hosted RetroArch section, CI table row

## Decisions Made

See key-decisions in the frontmatter. The seeds are nesturbator 5533aeb7e6e0 (`tests/retroarch/test.cfg.in`, `run_retroarch.cmake`, `bmp_ppm.c`, `compare_frame.c`) and nesturbator 9f9386c661f6 (the isolated home in `run_retroarch.cmake` and the DMG steps in `.github/workflows/ci.yml`). They were fetched with `gh api` into the scratch directory, and the commit body cites them. The research cited the e2e job at 3200aa868371, but that commit is not on GitHub, so the published 9f9386c661f6 was used instead.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 2 - Missing critical] The roll-up test's synthetic cases include `retroarch`**
- **Found during:** Task 1
- **Issue:** `policy.ci_required` passes once the job is in the needs. Its `all` and `light` lists still lacked `retroarch`, so the "light pull request" case did not exercise a skipped `retroarch`.
- **Fix:** Added `retroarch` to both lists in `tests/cmake/ci_required.cmake`.
- **Verification:** `cmake --workflow --preset hygiene` exit 0
- **Committed in:** 9b8282e

**2. [Rule 2 - Missing critical] Bundle version check and a failure-only log step**
- **Found during:** Task 1
- **Issue:** A DMG with the right digest could in principle hold another version, and a failure in the compare step would otherwise hide RetroArch's log.
- **Fix:** The job checks that `CFBundleShortVersionString` is 1.22.2, as in the nesturbator precedent. A `RetroArch log` step runs only on failure.
- **Committed in:** 9b8282e

---

**Total deviations:** 2 auto-fixed (2 missing critical). **Impact on plan:** small additions within the planned files, plus one line pair in the roll-up test. No scope creep.

## Issues Encountered

None. Both pushes went green on their first run.

## User Setup Required

None. No external service configuration is required.

## Next Phase Readiness

- FRAME-04 is complete: in-process (plan 07) and in hosted RetroArch (this plan).
- The `retroarch` job's 15-minute timeout is provisional. Plan 21 fixes it from measured runs (17 s and 22 s so far).
- Probe PR 6 is still a draft with every run green. Nothing was merged, retitled or marked ready.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED

Files: tests/retroarch/test.cfg.in, run_retroarch.cmake, bmp.c, compare.c found. Commits 9b8282e and f9db90b are ancestors of HEAD. Acceptance: pins.txt has the digest; tests/retroarch is 241 lines; CI required needs retroarch; the negative-control step exists; docs/testing.md has the Hosted RetroArch heading.
