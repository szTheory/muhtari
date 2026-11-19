---
phase: 29-windows-extended-path-runner-support
plan: "02"
subsystem: runner
tags: [windows, file-identity, atomic-output, png]
requires:
  - phase: 29-01
    provides: Windows wide-path boundary and the shared sibling-temp writer
provides:
  - Invocation-wide target identity checks across runner inputs and outputs
  - Atomic streaming publication for frame PNGs
  - Portable output-failure controls and Windows actual-process alias/failure cases
affects: [29-03, runner, Windows acceptance]
actuals:
  tokens: 13035
  tasks: 2
  commits: 4
commits: 4
plan_head_before: ce9e380b109fe13922de506c19ff9a3ae8f45edc
plan_head_after: 980a1519bf34364357051c0994b30e661474cf51
tech-stack:
  added: []
  patterns:
    - Windows opened-file identity uses volume and file IDs; absent targets use parent identity plus namespace-aware leaf semantics.
    - Frame PNG rows stream through the existing checked sibling-temp publisher.
key-files:
  created: []
  modified:
    - runner/main.c
    - tests/runner/test_save.c
    - tests/runner/windows_paths.c
    - tests/runner/trace.cmake
key-decisions:
  - "Use the invocation-wide target identity preflight for every documented input and output role, including the derived battery path."
  - "Publish PNG output with the existing atomic writer, preserving its row-wise memory use and successful output bytes."
requirements-completed: []
requirements-addressed: [WINPATH-02, WINPATH-03, WINPATH-04]
coverage:
  - id: D1
    description: "Invocation-wide alias checks cover input/output and output/output pairs, including derived battery output."
    requirement: WINPATH-02
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.](io|paths)$|^runner[.]state[.]files$|^runner[.]save[.](faults|roundtrip)$|^runner[.](png|dump_png)$|^runner[.]state[.]options$|^runner[.]save[.]options$'"
        status: pass
      - kind: e2e
        ref: "Windows drive and SMB actual-process matrix in plan 29-03"
        status: unknown
    human_judgment: false
  - id: D2
    description: "State, PNG, trace and battery publication preserve prior files on injected write or replacement failures."
    requirement: WINPATH-03
    verification:
      - kind: unit
        ref: "ctest runner.save.faults, runner.state.files and runner.trace.io; exit 0"
        status: pass
      - kind: e2e
        ref: "Windows locked-destination output-failure matrix in plan 29-03"
        status: unknown
    human_judgment: false
  - id: D3
    description: "PNG output uses row-wise atomic streaming and retains byte-identical successful image output."
    requirement: WINPATH-03
    verification:
      - kind: unit
        ref: "ctest runner.dump_png and runner.png; exit 0"
        status: pass
    human_judgment: false
  - id: D4
    description: "Windows tests cover max-component PNG and derived .sav names, plus actual paths beyond MAX_PATH."
    requirement: WINPATH-04
    verification:
      - kind: e2e
        ref: "tests/runner/windows_paths.c actual-process drive/UNC lane in plan 29-03"
        status: unknown
    human_judgment: false
duration: 43min
completed: 2026-11-18
status: complete
---

# Phase 29 Plan 02: Windows path identity and atomic output Summary

**Invocation-wide target identity checks reject path aliases before mutation, and PNG frames now publish through the shared atomic writer.**

## Performance

- **Duration:** 43 min
- **Started:** 2026-11-18T19:44:13Z (first plan commit)
- **Completed:** 2026-11-18T20:27:00Z
- **Tasks:** 2
- **Files modified:** 4

## Accomplishments

- Routed input/output and output/output checks through one target-identity preflight, including state, PNG, trace and the derived battery path. Existing targets use handle identity; missing targets use the existing parent identity and namespace-aware leaf. Required query failures remain fail-closed.
- Routed frame PNG encoding through the checked sibling-temp writer. Serialization stays row-wise and the successful PNG bytes remain unchanged.
- Added real write-failure injection for PNG output and Windows actual-process coverage for aliases, blocked replacement, prior-byte preservation, owned-temp cleanup and maximum component names.

## Task Commits

1. **Task 1 RED:** `fbd6a7a` (test)
2. **Task 1 implementation:** `f8a1ac0` (feat)
3. **Task 2 RED:** `36e435e` (test)
4. **Task 2 implementation:** `980a151` (fix)

## Files Created/Modified

- `runner/main.c` - Shared identity checks and atomic PNG serialization.
- `tests/runner/test_save.c` - PNG filesystem-write fault and output-preservation controls.
- `tests/runner/windows_paths.c` - Actual-process alias, locked-output and component-boundary cases.
- `tests/runner/trace.cmake` - Actual executable check for ordinary/extended junction aliases.

## Decisions Made

- Use filesystem identity across every invocation path role so lexical differences cannot bypass collision protection.
- Reuse the existing sibling-temp publisher for PNGs without a new dependency or a second full-image buffer.

## Verification

- `/opt/homebrew/opt/llvm@21/bin/clang-format --dry-run --Werror runner/main.c tests/runner/test_save.c tests/runner/windows_paths.c` — exit 0.
- `git diff --check` — exit 0.
- `cmake --build --preset ci --target muhtari-run muhtari-test-save-runner muhtari-test-trace muhtari-test-png` — exit 0; Ninja reported no pending build work.
- `ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.](io|paths)$|^runner[.]state[.]files$|^runner[.]save[.](faults|roundtrip)$|^runner[.](png|dump_png)$|^runner[.]state[.]options$|^runner[.]save[.]options$'` — exit 0, 9/9 passed.
- Windows actual-process cases, Windows compilation, and SMB execution were not available on Darwin. No RetroArch or host run was performed. Plan 29-03 retains the source-qualified Windows/SMB gate; WINPATH-02/-03/-04 remain pending and are not marked complete here.

## Issues Encountered

The PNG encoder is compiled in a separate translation unit, so the test harness's `fwrite` wrapper could not inject a serializer write failure. The harness now closes the tracked temporary descriptor before encoding and makes the stream unbuffered, causing a real OS write error. `runner.save.faults` passes and confirms the existing destination remains intact.

## User Setup Required

None.

## Next Phase Readiness

Implementation and portable checks are complete. Plan 29-03 must qualify the real Windows drive and SMB paths, compile and run the new Windows process cases, then record exact-source evidence before phase requirements can be accepted.

## Self-Check: PASSED

The summary path was created, the four plan commits are ancestors of the recorded plan head, and the measured plan commit count is 4.

---
*Phase: 29-windows-extended-path-runner-support*
*Completed: 2026-11-18*
