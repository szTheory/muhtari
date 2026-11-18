---
phase: 29-windows-extended-path-runner-support
plan: "01"
subsystem: runner
tags: [windows, utf-16, extended-paths, smb, ctest]
requires:
  - phase: 28-measured-milestone-tune-up
    provides: source-qualified delivery and maintenance baseline
provides:
  - Unicode-preserving Windows runner argument and filesystem boundary for explicit drive and extended UNC paths
  - Real-process Windows drive and local SMB UNC path test wired to runner.trace.paths
affects: [29-02, 29-03]
actuals:
  tokens: 16277
  tasks: 2
  commits: 4
commits: 4
plan_head_before: 6c7e9c45d648aaeb054bd615d0402b4b80f32b5c
plan_head_after: 6136db1bda3df783d8fc842c9e2be7bf5fd50303
tech-stack:
  added: []
  patterns:
    - strict UTF-8 and UTF-16 conversion at the Windows process and filesystem boundary
    - owned local SMB share for actual-process UNC testing
key-files:
  created:
    - tests/runner/windows_paths.c
    - tests/runner/windows_paths.ps1
  modified:
    - runner/main.c
    - tests/runner/test_save.c
    - tests/runner/trace.cmake
key-decisions:
  - Use existing C and Win32 APIs without adding a dependency or changing public interfaces.
  - Keep Windows acceptance pending until the actual Windows CI path and SMB share test run.
patterns-established:
  - Windows path arguments retain their explicit UTF-16 namespace spelling through file operations.
  - Native process tests independently check output paths and state bytes under bounded child execution.
requirements-completed: []
coverage:
  - id: D1
    description: "Windows runner file roles preserve Unicode and explicit drive or extended UNC path names."
    requirement: WINPATH-01
    verification:
      - kind: integration
        ref: "Windows actual-process drive and UNC test; not run on Darwin, pending Phase 29-03."
        status: unknown
    human_judgment: false
  - id: D2
    description: "The recurring Windows path test provisions and removes an owned local SMB share and checks runner output through UNC."
    requirement: WINPATH-04
    verification:
      - kind: integration
        ref: "tests/runner/trace.cmake -> tests/runner/windows_paths.ps1; Windows execution pending Phase 29-03."
        status: unknown
    human_judgment: false
duration: 44m
completed: 2026-11-18
status: complete
---

# Phase 29 Plan 01: Windows extended-path runner support Summary

**Windows runner file operations now preserve explicit drive and UNC path spelling, with a real-process SMB acceptance test wired into the existing trace test.**

## Performance

- **Duration:** 44 minutes
- **Started:** 2026-11-18T18:36:42Z
- **Completed:** 2026-11-18T19:20:32Z
- **Tasks:** 2
- **Files modified:** 5

## Accomplishments

- Added strict UTF-8/UTF-16 conversion and wide Windows file operations across the runner's documented path roles, including state, movie, BIOS, HSC firmware, trace, frame output, save directory and battery paths.
- Added a bounded native process driver and a PowerShell helper that owns a temporary SMB share, checks drive and UNC paths, and reports both runner and cleanup failures.
- Preserved POSIX backslash semantics for derived battery filenames and added deep Windows path coverage beyond MAX_PATH.

## Verification

- `cmake --preset ci` — exit 0.
- `cmake --build --preset ci --target muhtari-run muhtari-test-save-runner muhtari-test-trace` — exit 0.
- Task 1 focused CTest selection for `runner.state.roundtrip` and `runner.trace.paths` — 2/2 passed on Darwin.
- Final CTest selection for `runner.trace.paths`, state roundtrip/options/files, and save roundtrip/faults/options — 7/7 passed on Darwin.
- `/opt/homebrew/opt/llvm@21/bin/clang-format --dry-run --Werror` on the changed C files and `git diff --check` — passed.
- The Windows process driver, PowerShell helper, Windows compiler lane and SMB share execution were not run on Darwin. No Windows or UNC acceptance is claimed; actual Windows evidence remains pending Phase 29-03.

## TDD Cycle

- **RED:** Added a `runner.trace.paths` assertion that a POSIX backslash in a cartridge basename remains literal. CTest exited 8 because the target test executed and failed on that assertion; its JUnit record is `build/ci/phase29-01-red-record.json`, and `gsd-tools check tdd-red-evidence` returned `RED_EVIDENCE_OK`. Semantic assessment: the path helper incorrectly treated a POSIX backslash as a directory separator.
- **GREEN:** Restricted backslash separator handling to Windows. The portable state/save/trace selection passed 7/7. The Windows driver also tests deep paths and the state-output frame index, but those cases still require Windows execution.
- **REFACTOR:** No separate refactor was needed.

## Task Commits

1. **Task 1: Save and reload a state through real Unicode drive and UNC arguments** — `fe25dda`.
2. **Task 2: Preserve namespaces through every documented runner file role** — `b78694c` (RED test), `ecbe607` (GREEN fix), `6136db1` (deep path and Windows driver corrections).

## Files Created/Modified

- `runner/main.c` — UTF-16 process entry and wide Windows path validation, file access, joins and replacement.
- `tests/runner/test_save.c` — wide filesystem interception and portable battery basename regression.
- `tests/runner/windows_paths.c` — bounded actual-process Windows tests for explicit drive and UNC paths, file roles, malformed forms, Unicode, literal suffixes and deep paths.
- `tests/runner/windows_paths.ps1` — temporary local SMB share lifecycle and drive/UNC test invocation.
- `tests/runner/trace.cmake` — Windows-only driver target and existing `runner.trace.paths` routing.

## Decisions Made

- Kept the implementation within existing C and Win32 APIs; no dependency or public API was added.
- Kept the Windows/UNC acceptance gate pending the source-qualified Windows run in Phase 29-03.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Preserved POSIX backslashes in battery basenames**
- **Found during:** Task 2.
- **Issue:** The new path helper interpreted a POSIX backslash as a directory separator, breaking the existing portable basename behavior.
- **Fix:** Treat backslash as a separator only on Windows and add a targeted portable regression.
- **Files modified:** `runner/main.c`, `tests/runner/test_save.c`.
- **Verification:** RED evidence was accepted by the GSD classifier; the planned seven-test portable selection passed after the fix.
- **Committed in:** `b78694c`, `ecbe607`.

**Total deviations:** 1 auto-fixed bug.
**Impact on plan:** The correction restores portable filename behavior without expanding scope or adding dependencies.

## Issues Encountered

- The best-effort `gsd-tools windows append` for the unrun Windows gate declined because the existing `.planning/WINDOWS.md` rendered table disagrees with its fenced JSON source for row 51. That shared ledger was left untouched; this summary records the pending Windows evidence directly.
- This Darwin host has no Windows compiler/runtime or PowerShell, so the real-process drive and SMB UNC tests remain unrun and cannot establish Windows acceptance.

## User Setup Required

None. No external configuration is needed for local portable checks.

## Next Plan Readiness

Plan 29-02 can proceed with alias and output-safety work. Plan 29-03 must run the registered Windows process/share test and record source-qualified evidence before any Windows path requirement is accepted.

---
*Phase: 29-windows-extended-path-runner-support*
*Completed: 2026-11-18*

## Self-Check: PASSED

- Summary file exists at the planned phase path.
- Task commits `fe25dda`, `b78694c`, `ecbe607` and `6136db1`, plus summary commit `d2ddb60`, are present in the current branch ancestry.
