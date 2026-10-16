---
phase: 01-a-test-frame-in-retroarch
plan: 09
subsystem: release
tags: [cpack, zip, release-archives, notices, release-please, version-source, readme, install-line, docs, budgets]

requires:
  - phase: 01-08
    provides: "the library install component (header, archive, CMake package, pkg-config file)"
  - phase: 01-07
    provides: "the libretro install component (cores/ module, info/ file) and the generated display_version"
  - phase: 01-01
    provides: "the presets, version.txt, muhtari_add_test, the exact inventory and expect_fail/check_junit tools"
provides:
  - "cmake/packaging.cmake: CPack ZIPs muhtari-{library,runner,libretro}-<os>-<arch>.zip, version-free, no top-level folder, LICENSE and THIRD-PARTY-NOTICES.md in each"
  - "ci and ci-msvc workflows: configure, build, test (no release label), package, test ci-release / ci-msvc-release (junit-release.xml)"
  - "release.archive_contents and its self test"
  - "release.single_version_source, .next and .self_test"
  - "tests/cmake/install_line.cmake (-DREADME= -DBASE_URL= [-DHOME_DIR=]), run as a CI step on macOS arm64, not a CTest"
  - "User README, docs/embedding.md, docs/testing.md, docs.budgets"
affects: [ci-workflows, release-publish, macos-arm64-ci-leg, docs]

actuals:
  tokens: 9368
  tasks: 3
  commits: 4
plan_head_before: ac367c724aa7efb3fe98c8c2c790593efd789084
plan_head_after: fe055f741b6ef14785986b8b0787b00bea709973

tech-stack:
  added: [CPack ZIP generator, CMake package presets]
  patterns:
    - "Release tests carry label release and run in a second test step after the package step"
    - "Configure-time fixtures for negative controls (a zip without notices; a git repository repeating its version)"
    - "Multi-line FATAL_ERROR messages with indented lines so expect_fail regexes survive CMake's message wrapping"

key-files:
  created:
    - cmake/packaging.cmake
    - tests/cmake/archive_contents.cmake
    - tests/release/tests.cmake
    - tests/cmake/single_version.cmake
    - tests/cmake/install_line.cmake
    - tests/cmake/budgets.cmake
    - docs/embedding.md
    - docs/testing.md
  modified:
    - CMakeLists.txt
    - CMakePresets.json
    - tests/tests.cmake
    - tests/policy/tests.cmake
    - tests/inventory/ci.txt
    - README.md

key-decisions:
  - "Release zips land in the build directory itself (build/ci/muhtari-*.zip); the archive check unpacks them under build/ci/release/extract"
  - "The dev test preset also excludes label release, because the dev workflow has no package step"
  - "The single-version check scans tracked files only; a version followed by a dot (end of a sentence) is outside the bounded match, as the plan's no-digit-no-dot rule defines it"
  - "LICENSE and THIRD-PARTY-NOTICES.md are installed once per component from packaging.cmake, so install.stage's library prefix also holds them"

patterns-established:
  - "Package step between two test steps in a workflow preset; the release test preset writes junit-release.xml"
  - "Every repository check names its fault in its header and has a self test through expect_fail.cmake"

requirements-completed: [FRAME-06, FRAME-07]

coverage:
  - id: D1
    description: "The ci workflow packages exactly three zips named muhtari-{library,runner,libretro}-macos-arm64.zip, each with exactly its expected members at the root, LICENSE and THIRD-PARTY-NOTICES.md byte-identical to the tree's, and no file holding the source or build directory"
    requirement: FRAME-06
    verification:
      - kind: integration
        ref: "tests/release/tests.cmake#release.archive_contents"
        status: pass
      - kind: other
        ref: "cmake --workflow --preset ci (exit 0) then ls build/ci/muhtari-*-macos-arm64.zip lists three zips"
        status: pass
    human_judgment: false
  - id: D2
    description: "A libretro zip without THIRD-PARTY-NOTICES.md makes the archive check exit 1 naming that file, and a member holding the source path makes it exit 1 naming the member"
    requirement: FRAME-06
    verification:
      - kind: integration
        ref: "tests/release/tests.cmake#release.archive_contents.self_test"
        status: pass
    human_judgment: false
  - id: D3
    description: "The release test step runs both release tests once each, checked from its JUnit file"
    requirement: FRAME-06
    verification:
      - kind: other
        ref: "cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake (exit 0, 2 tests)"
        status: pass
    human_judgment: false
  - id: D4
    description: "The version in version.txt, and its next minor and next patch, appear in no in-scope tracked file; a fixture repository repeating its version in notes.md fails naming notes.md"
    requirement: FRAME-06
    verification:
      - kind: unit
        ref: "tests/policy/tests.cmake#release.single_version_source"
        status: pass
      - kind: unit
        ref: "tests/policy/tests.cmake#release.single_version_source.next"
        status: pass
      - kind: unit
        ref: "tests/policy/tests.cmake#release.single_version_source.self_test"
        status: pass
    human_judgment: false
  - id: D5
    description: "The README's one-line install, with its download base pointed at the locally built zip, puts cores/muhtari_libretro.dylib and info/muhtari_libretro.info under the RetroArch directory of a scratch HOME"
    requirement: FRAME-07
    verification:
      - kind: e2e
        ref: "cmake -DREADME=README.md -DBASE_URL=file://$PWD/build/ci/ -P tests/cmake/install_line.cmake (exit 0, both paths printed)"
        status: pass
    human_judgment: false
  - id: D6
    description: "README.md is at most 12 KB, include/muhtari.h at most 24 KB and each docs page at most 40 KB"
    requirement: FRAME-07
    verification:
      - kind: unit
        ref: "tests/policy/tests.cmake#docs.budgets"
        status: pass
    human_judgment: false
  - id: D7
    description: "README and docs read well for users and embedders: the RetroArch quick start matches the real menus, and the embedding guide is accurate and complete"
    requirement: FRAME-07
    verification: []
    human_judgment: true
    rationale: "Prose quality and whether the RetroArch menu steps match what a user sees are judgements no test makes; the install itself is covered by D5"

duration: 10 min
completed: 2026-10-16
status: complete
---

# Phase 01 Plan 09: Release archives, single version source and README install Summary

**CPack builds three version-free zips per leg, each carrying LICENSE and THIRD-PARTY-NOTICES.md and checked member by member after a package step; the version lives only in version.txt at the current and next release versions; and the README's one-line RetroArch install runs verbatim against the locally built zip.**

## Performance

- **Duration:** 10 min
- **Started:** 2026-10-16T15:32:54Z
- **Completed:** 2026-10-16T15:42:48Z
- **Tasks:** 3
- **Files modified:** 14

## Accomplishments

- `cmake --workflow --preset ci` now runs: configure, build, test (45 tests, label `release` excluded), package, and test `ci-release` (2 tests). The zips are `build/ci/muhtari-{library,runner,libretro}-macos-arm64.zip`.
- `release.archive_contents` requires the exact member list per component and OS, byte-identical notices, and no file holding the source or build directory. Its self test feeds it a libretro zip without `THIRD-PARTY-NOTICES.md`.
- `release.single_version_source` and `.next` keep 0.0.0, 0.1.0 and 0.0.1 (the current version and the next minor and next patch) out of every in-scope tracked file. The self test uses a git fixture repository.
- The README is rewritten for users (2,565 bytes): status, the one-line install, three Requirements lines, the quick starts, the docs ladder and the licence. `tests/cmake/install_line.cmake` runs that line verbatim.
- `docs/embedding.md` covers CMake, `add_subdirectory`, pkg-config, size tags, the six-line malloc wrapper, version numbers with zero meaning absent for `api`, `abi` and `behaviour_revision`, and the C++ conditional-expression warning (the research note R-17 carried over from plan 08). `docs/testing.md` and `docs.budgets` are new too.

## Task Commits

1. **Task 1: End-to-end archives** - `173b698` (feat)
2. **Task 2: Single version source**, RED `ac2b523` (test), GREEN `35f3701` (feat)
3. **Task 3: README, install line, docs, budgets** - `fe055f7` (docs)

## TDD Gate Compliance (Task 2)

- RED `ac2b523`. The tests were registered against a script that resolved the versions but scanned nothing. Command: `ctest --test-dir build/ci -R '^release\.single_version_source' --output-on-failure --output-junit red-single-version.xml`, exit 8. The target `release.single_version_source.self_test` failed on its own assertion: it expected exit 1 naming `notes.md` and got exit 0. Classifier: `gsd-tools check tdd-red-evidence` returned `RED_EVIDENCE_OK` (target_test_failed, junit). Semantic assessment: the target test executed and failed for the intended reason, not because of a load or fixture fault. The other two tests passed in RED because the tree is clean, which is expected.
- GREEN `35f3701`. The scan was added, and all three tests pass.
- REFACTOR: none needed.

## Seeds

- `cmake/packaging.cmake` from nesturbator 9f9386c661f6 `cmake/packaging.cmake` (53 lines). The version was dropped from the names, and the notices install and the build-directory package location were added.
- `tests/cmake/archive_contents.cmake` from nesturbator 5533aeb7e6e0 `tests/cmake/check_archives.cmake` (110 lines). It now uses exact per-OS member lists instead of regexes and extracts the zips. The notices byte check and the path scan are new.
- `tests/cmake/install_line.cmake` from nesturbator 9f9386c661f6 `tests/cmake/check_install_line.cmake` (131 lines). The rewrite runs the version-free line verbatim under `sh` with a scratch `HOME` instead of re-parsing its tar arguments.

## Gate-code tally

- `src/`: 315 lines.
- `tests/cmake/*.cmake`: 419 lines, of which this plan adds 209 (archive_contents 76, single_version 49, install_line 54, budgets 30).
- `scripts/*.sh`: 283 lines.
- The 10% cap applies from phase 3.

## Evidence (command, exit status)

- `cmake --workflow --preset ci` exits 0: 45 tests pass, then 2 release tests pass.
- `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` exits 0 (45 tests).
- `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` exits 0 (2 tests).
- `cmake -DREADME=README.md -DBASE_URL=file://$PWD/build/ci/ -P tests/cmake/install_line.cmake` exits 0 and prints both installed paths.
- `cmake -DVERSION=0.1.0 -P tests/cmake/single_version.cmake` exits 0.
- Negative probes, all reverted afterwards:
  - A zip member holding the source path makes `archive_contents.cmake` exit 1 and name the member.
  - A `scratch 0.0.0` line in tracked `docs/runner.md`, and in `docs/testing.md` once it was added to the index, fails `release.single_version_source` naming the file.
  - A 13,000-byte README makes `budgets.cmake` exit 1.
  - A README line without the `info` member makes `install_line.cmake` exit 1 naming the `.info` path.
  - `10.1.0` and `0.1.0.1` do not match 0.1.0, but `v0.1.0 x` does.
- `unzip -l build/ci/muhtari-libretro-macos-arm64.zip` lists `LICENSE`, `THIRD-PARTY-NOTICES.md`, `cores/muhtari_libretro.dylib` and `info/muhtari_libretro.info`, plus the directory entries `cores/` and `info/`.
- `grep -nE '\b[DR]-[0-9]{2}\b|FRAME-[0-9]{2}' README.md docs/*.md` prints nothing.

## Decisions Made

See key-decisions in the frontmatter.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] The runner had no install rule**
- **Found during:** Task 1
- **Issue:** `muhtari-run` was not installed in any component, so the runner zip would have held only the notices.
- **Fix:** `install(TARGETS muhtari-run RUNTIME DESTINATION bin COMPONENT runner)`.
- **Files modified:** CMakeLists.txt
- **Committed in:** 173b698

**2. [Rule 3 - Blocking] The dev test preset would run the release tests without zips**
- **Found during:** Task 1
- **Issue:** The `dev` workflow has no package step, so the `release` tests would fail there.
- **Fix:** The `dev` test preset excludes label `release`, as `ci` and `ci-msvc` do.
- **Files modified:** CMakePresets.json
- **Committed in:** 173b698

**3. [Rule 1 - Bug] The self-test regex broke on CMake's message wrapping**
- **Found during:** Task 1
- **Issue:** CMake wrapped the one-line "lacks" message between the zip name and the file, so the `expect_fail` regex missed it.
- **Fix:** The missing and extra lists now go on their own indented lines, which CMake does not reflow. The regex is `lacks: THIRD-PARTY-NOTICES.md`.
- **Files modified:** tests/cmake/archive_contents.cmake, tests/release/tests.cmake
- **Committed in:** 173b698

**4. [Rule 1 - Bug] The fixture text put a dot after the version**
- **Found during:** Task 2 GREEN
- **Issue:** The self-test fixture wrote "Released as 1.2.3." The trailing dot is outside the bounded match the plan defines (no digit or dot on either side), so the check passed.
- **Fix:** The fixture now reads "Released as 1.2.3 today."
- **Files modified:** tests/policy/tests.cmake
- **Committed in:** 35f3701

### Other notes

- The archive self test builds its fixture zip at configure time with `cmake -E tar --format=zip`, not at test time. This matches the existing policy fixtures. The negative control is the same.
- Task 2's `docs/testing.md` probe ran in Task 3, once that file existed. Untracked files are out of scope by design, so the file was added to the index with `git add -N` first.

**Total deviations:** 4 auto-fixed (2 blocking, 2 bugs)
**Impact on plan:** All were needed for the plan's own checks to work. No scope creep.

## Issues Encountered

None beyond the deviations above.

## Known Limitations

- The single-version check does not catch a version followed directly by a dot, such as the end of a sentence. The plan's bounded-match rule defines it that way. A tighter rule, such as a dot followed by a digit, is a possible follow-up.
- Running the install line over an existing RetroArch directory is untested; the check uses an empty scratch `HOME` (the plan's flagged assumption).
- The archive check covers the macOS arm64 leg locally. Linux, Windows and the other five legs run it in CI from plan 14 onward.

## User Setup Required

None.

## Next Phase Readiness

The ci and ci-msvc workflows package and check archives on every leg. Plan 14 runs `install_line.cmake` as a CI step on macOS arm64, and plans 17 and 18 run it after publishing with the release URL as `BASE_URL`.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*
