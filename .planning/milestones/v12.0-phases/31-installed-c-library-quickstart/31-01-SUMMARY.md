---
phase: 31-installed-c-library-quickstart
plan: "01"
subsystem: api
tags:
  - c17
  - cmake
  - pkg-config
  - installed-package
  - consumer-tests
requires: []
provides:
  - "A ROM-free C17 host quickstart using the installed library package"
  - "Archive and staged-package checks for the documented C consumer"
  - "C host entry guidance linked from the README and full API guide"
affects:
  - embedding
  - release-package-verification
  - package-consumer-ci
actuals:
  tokens: 3981
  tasks: 3
  commits: 5
plan_head_before: e829d07f8ae9982b1acf486014208a7d990f3d00
plan_head_after: 27a324a3c23fb6002b3482d92e3365e01f3c3659
commits: 5
tech-stack:
  added: []
  patterns:
    - "One C source is built through both installed package routes and staged consumers"
    - "Release package controls validate the exact expected failure diagnostic"
key-files:
  created:
    - examples/embedding/quickstart.c
    - examples/embedding/CMakeLists.txt
    - docs/embedding-quickstart.md
  modified:
    - README.md
    - docs/embedding.md
    - tests/consumer/CMakeLists.txt
    - tests/install/tests.cmake
    - tests/release/ym_consumers.cmake
    - .github/workflows/ci.yml
key-decisions:
  - "Use the same ROM-free C17 source for documentation, extracted archive routes, and staged package checks."
  - "Require CMake archive consumption on every native lane and enable pkg-config only on Ubuntu 26.04 x64."
  - "Keep the missing-export negative control isolated from the good archive prefix and require its specific CMake diagnostic."
patterns-established:
  - "The host supplies both allocator callbacks and sizes caller-owned video and audio buffers from muhtari_info."
  - "Package verification runs the documented host source against an extracted release ZIP rather than the build tree."
requirements-completed:
  - EMBED-01
coverage:
  - id: D1
    description: "A clean CMake consumer builds the C17 quickstart against the extracted library ZIP and checks one NTSC frame with 800 audio samples."
    requirement: EMBED-01
    verification:
      - kind: e2e
        ref: "MUHTARI_RELEASE_QUICKSTART_PKG_CONFIG=1 ctest --preset ci-release --no-tests=error --verbose -R '^release\\.ym_consumers$'"
        status: pass
    human_judgment: false
  - id: D2
    description: "The same source compiles through pkg-config using only metadata from the extracted archive, with a missing-tool failure."
    requirement: EMBED-01
    verification:
      - kind: integration
        ref: "The release CTest above; restricted-PATH invocation of tests/release/ym_consumers.cmake"
        status: pass
    human_judgment: false
  - id: D3
    description: "Both staged no-content consumer checks build and run the quickstart alongside the original consumer."
    requirement: EMBED-01
    verification:
      - kind: integration
        ref: "ctest --preset ci --no-tests=error --output-on-failure -R '^(embed\\.subdirectory|install\\.consumer(\\.dll_crt)?(\\.ym)?)$'"
        status: pass
    human_judgment: false
  - id: D4
    description: "README and embedding documentation lead C hosts to the matching archive, example, package routes, and complete API guide."
    requirement: EMBED-01
    verification:
      - kind: other
        ref: "git diff --check origin/main plus tests/cmake/budgets.cmake and local link inspection"
        status: pass
    human_judgment: false
metrics:
  duration: 33min
  completed: 2026-11-19
status: complete
---

# Phase 31 Plan 01: Installed C Library Quickstart Summary

**A ROM-free C17 host quickstart now builds from the installed release ZIP through CMake and pkg-config and checks the 800-sample first frame.**

## Performance

- **Duration:** 33 minutes
- **Started:** 2026-11-20T03:06:41Z
- **Completed:** 2026-11-20T03:39:51Z
- **Tasks:** 3
- **Files modified:** 9

## Accomplishments

- Added a standalone C17 example with allocator callbacks, size-tagged structs, info-sized buffers, failure diagnostics, and cleanup. It prints the success line only after a successful frame with exactly 800 samples.
- Added a separate CMake consumer project and documented CMake and pkg-config recipes for the matching native library archive. The README links the example and the complete API guide.
- Extended the extracted archive check to build the same source through both routes, added a strict missing-export control, and ran the source through both existing staged consumer CRT checks.
- Enabled the pkg-config archive route only on the existing Ubuntu 26.04 x64 CI lane; no dependency or public API/ABI changes were made.

## Task Commits

1. **Task 1: Run the documented CMake example from the library archive** - `87adbd8` (test), `7c0cd8c` (feat).
2. **Task 2: Compile the same example via pkg-config on the extracted package** - `f6989b3` (feat).
3. **Task 3: Catch a broken installed target and exercise the example in staged consumers** - `836057d` (test), `27a324a` (feat).

The test-first archive assertion failed before implementation at the expected missing-project configure step (`release.ym_consumers`, exit 8); GSD classified the JUnit evidence as `RED_EVIDENCE_OK`. The staged quickstart output assertion also failed before wiring the example into the staged project (`install.consumer`, exit 8) and passed GSD's red-evidence check.

## Files Created/Modified

- `examples/embedding/quickstart.c` - Complete ROM-free C host example using the public API.
- `examples/embedding/CMakeLists.txt` - Standalone C17 consumer using `muhtari::muhtari`.
- `docs/embedding-quickstart.md` - Archive selection, tools, commands, expected output, and scope of the check.
- `README.md`, `docs/embedding.md` - C host entry point and link to the worked pkg-config route.
- `tests/consumer/CMakeLists.txt`, `tests/install/tests.cmake` - Same example built and run through the staged package checks.
- `tests/release/ym_consumers.cmake` - Extracted archive routes, missing-export control, and existing release consumer controls.
- `.github/workflows/ci.yml` - Ubuntu 26.04 x64 pkg-config opt-in.

## Decisions Made

- The quickstart supplies both allocator callbacks and sizes its video and signed 16-bit audio buffers from `muhtari_info`.
- Windows remains on the CMake route; the CI pkg-config route is limited to the existing Ubuntu 26.04 x64 lane.
- The damaged package is a copy under the release scratch directory, leaving the good extraction and archive bytes intact.

## Verification

| Check | Result |
|---|---|
| `git diff --check origin/main -- README.md docs/embedding-quickstart.md examples/embedding tests/release/ym_consumers.cmake` | Exit 0 |
| `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` | Exit 0; all 26 checked documents are within budget |
| `actionlint -ignore 'label "ubuntu-26\\.04(-arm)?" is unknown' -ignore 'SC(2017|2059|2129):' .github/workflows/ci.yml` | Exit 0 |
| `ctest --preset ci-release --no-tests=error --verbose -R '^release\\.ym_consumers$'` | Exit 0; extracted CMake route and existing archive/YM/source controls passed |
| `MUHTARI_RELEASE_QUICKSTART_PKG_CONFIG=1 ctest --preset ci-release --no-tests=error --verbose -R '^release\\.ym_consumers$'` | Exit 0; CMake, pkg-config, missing-export, and existing archive controls passed |
| `ctest --preset ci --no-tests=error --output-on-failure -R '^(embed\\.subdirectory|install\\.consumer(\\.dll_crt)?(\\.ym)?)$'` | Exit 0; all 7 selected tests passed |
| Clean scratch CMake and pkg-config recipes using only the two example files and `build/ci/muhtari-library-macos-arm64.zip` | Both exit 0 and print `quickstart: one frame, 800 samples` |
| Restricted-PATH `tests/release/ym_consumers.cmake` run with pkg-config opted in | Expected script exit 1; diagnostic names required `pkg-config` |
| `expect_fail.cmake` against a healthy package and an unrelated configure failure | Both controls rejected by the helper with exit 1 |
| `MUHTARI_RELEASE_QUICKSTART_PKG_CONFIG=1 cmake --workflow --preset ci` | Exit 0; all 736 non-release tests and all 8 release tests passed |

The missing-export control's expected inner configure exit is 1 and its diagnostic names the missing `muhtariTargets.cmake`; the release test passes only when the strict helper accepts that exact fault. The healthy and unrelated controls confirm that an ordinary success or a different configure error is not accepted. These were local runs; no hosted run URL exists for this plan.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Make the pkg-config recipe work in zsh as well as POSIX sh**
- **Found during:** Task 2 (clean archive recipe verification)
- **Issue:** `set -- $pkg_flags` did not split the flags under the local zsh defaults, so the compiler did not receive the extracted include path.
- **Fix:** Pass the separately checked, POSIX-escaped flags through `xargs`, with the source before the library flags.
- **Files modified:** `docs/embedding-quickstart.md`
- **Verification:** Recreated a clean scratch consumer from only the two example files and the current Apple Silicon ZIP; both documented routes exited 0 and printed the required success line.
- **Committed in:** `f6989b3`.

**Total deviations:** 1 auto-fixed issue (Rule 1).
**Impact on plan:** The change keeps the documented pkg-config route usable from the default macOS shell without changing the package or adding a dependency.

## Issues Encountered

No unresolved issues. The initial pkg-config documentation command failure is resolved as recorded above.

## User Setup Required

None - no external service configuration is required.

## Next Phase Readiness

EMBED-01 is implemented and all planned local checks pass. The exact-source hosted run remains with the normal delivery workflow; no host or owner-device check was performed or claimed.

---
*Phase: 31-installed-c-library-quickstart*
*Completed: 2026-11-19*

## Self-Check: PASSED

All ten created or modified plan files are present, and all five task commits are ancestors of the current HEAD.
