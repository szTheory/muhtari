---
phase: 31-installed-c-library-quickstart
plan: "02"
subsystem: testing
tags:
  - c17
  - cmake
  - installed-package
  - fault-injection
requires: []
provides:
  - "Staged package checks for quickstart setup and frame failure cleanup"
  - "A fault observer that checks each acquired pointer is released exactly once"
affects:
  - embedding
  - installed-package-tests
actuals:
  tokens: 3147
  tasks: 2
  commits: 2
plan_head_before: 341a6425790bdee0ebf6f866142ebe438cef6013
plan_head_after: 2aa05586ea4761e56b10a209013c32aa46e463ff
commits: 2
tech-stack:
  added: []
  patterns:
    - "A test-only wrapper exercises the exact documented example against the installed library"
    - "Resource tracking retains freed blocks until assertions can detect duplicate releases"
key-files:
  created:
    - tests/consumer/quickstart_faults.c
  modified:
    - tests/consumer/CMakeLists.txt
    - tests/install/tests.cmake
key-decisions:
  - "Keep all fault injection in the staged test driver; leave the documented example and public API unchanged."
  - "Require setup and result fault checks before emitting the staged quickstart success marker."
patterns-established:
  - "Use scoped function-like substitutions around the included example to instrument allocation and frame calls."
requirements-completed:
  - EMBED-01
coverage:
  - id: FAULTS
    description: "Staged CRT consumers exercise create, partial buffer allocation, real frame rejection and wrong sample count, requiring the diagnostic, no example success output, and exactly one release of every acquired pointer."
    requirement: EMBED-01
    verification:
      - kind: integration
        ref: "ctest --preset ci --no-tests=error --output-on-failure -R '^(embed\\.subdirectory|install\\.consumer(\\.dll_crt)?(\\.ym)?)$'"
        status: pass
    human_judgment: false
duration: 12min
completed: 2026-11-20
status: complete
---

# Phase 31 Plan 02: Quickstart Failure Cleanup Summary

**The staged quickstart checks now prove error reporting and one-time cleanup across create, allocation, frame and result failures.**

## Performance

- **Duration:** 12 minutes
- **Started:** 2026-11-20T04:26:08Z
- **Completed:** 2026-11-20T04:38:00Z
- **Tasks:** 2
- **Files modified:** 3

## Accomplishments

- Added a test-only C driver that includes the exact quickstart source, injects four deterministic failures, and checks that every acquired pointer is released exactly once.
- Added mandatory fault runs to both staged CRT consumers. The quickstart success marker is emitted only after all error-path assertions pass.
- Proved the observer catches a deliberately omitted `free(audio)` in an isolated scratch copy while the original staged consumer still succeeds.

## Task Commits

1. **Task 1: Exercise a real frame rejection after all resources are acquired** - `af50b31` (test).
2. **Task 2: Check setup and result failures and prove a missing release is rejected** - `2aa0558` (test).

## Files Created/Modified

- `tests/consumer/quickstart_faults.c` - Test-only allocation tracker and setup/frame fault driver.
- `tests/consumer/CMakeLists.txt` - Optional staged-package fault executable.
- `tests/install/tests.cmake` - Exact exit, stdout, diagnostic and cleanup checks for both staged CRT configurations.

## Verification

| Check | Result |
|---|---|
| `cmake --preset ci` | Exit 0; existing CI preset configured with the staged fault target registered. |
| `git diff --check origin/main -- tests/consumer/CMakeLists.txt tests/install/tests.cmake tests/consumer/quickstart_faults.c` | Exit 0. |
| `ctest --preset ci --no-tests=error --output-on-failure -R '^(embed\\.subdirectory|install\\.consumer(\\.dll_crt)?(\\.ym)?)$'` | Exit 0; all 7 selected install and embedding tests passed, including both staged CRT fault checks. |
| `MUHTARI_RELEASE_QUICKSTART_PKG_CONFIG=1 ctest --preset ci-release --no-tests=error --verbose -R '^release\\.ym_consumers$'` | Exit 0; extracted archive CMake and pkg-config routes and existing package controls passed. |
| `cmake -DQUICKSTART_PROGRAM="$PWD/build/ci/consumer/quickstart" -DQUICKSTART_FAULTS_PROGRAM="$PWD/build/ci/quickstart-fault-control/build/quickstart_faults" -DEMULATOR= -P tests/install/tests.cmake` | Expected exit 1; the child exited 2 and reported `audio allocation was not released`. The staged success marker was not emitted. |
| `MUHTARI_RELEASE_QUICKSTART_PKG_CONFIG=1 cmake --workflow --preset ci` | Exit 0; all 736 CI tests and all 8 release tests passed, and the three CI ZIP packages were generated. Local run only; no hosted run URL. |

The scratch sensitivity control removed `free(audio)` only from its copied quickstart. Its diagnostic confirms the release tracker identifies the specific missing audio release rather than accepting a balanced aggregate count.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Limit allocator aliases to function calls**
- **Found during:** Task 1 (frame failure cleanup)
- **Issue:** An object-like `free` macro also replaced the public allocator member `.free`, preventing the test driver from compiling.
- **Fix:** Changed the `malloc` and `free` substitutions to function-like macros so only calls in the included quickstart are redirected.
- **Files modified:** `tests/consumer/quickstart_faults.c`
- **Verification:** The fault target compiled and the staged, archive and full local workflow checks passed.
- **Committed in:** `af50b31`.

**Total deviations:** 1 auto-fixed implementation issue. No public API, documented example, package format or CTest registration name changed.

## Issues Encountered

No unresolved issues. The required local test coverage is complete; fresh phase verification remains the acceptance gate.

## User Setup Required

None.

## Next Phase Readiness

The local cleanup evidence is ready for a fresh Phase 31 verifier run. The full workflow evidence is local and does not establish hosted CI or release status.

---
*Phase: 31-installed-c-library-quickstart*
*Completed: 2026-11-20*

## Self-Check: PASSED

The driver and both CMake test surfaces exist; task commits `af50b31` and `2aa0558` are ancestors of this plan's recorded head.
