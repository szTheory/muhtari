---
phase: 12-player-ready-delivery-and-playstead-bridge
plan: "21"
subsystem: testing
tags: [cmake, wasi, wasmtime, portable-tests, hygiene]
requires: []
provides:
  - Native and WASI state-control scripts invoke drivers through their configured execution route
  - Complete retained-SDK local WASI portable lane passes after the route repair
  - Final native workflow, release/package checks, formatter, and tree hygiene pass
affects: [phase-12-player-ready-delivery, phase-12-plan-12-16-local-wasi-prerequisite]
actuals:
  tokens: 5427
  tasks: 3
  commits: 3
commits: 3
plan_head_before: 018de7728f746e7c41f86b8e21f7383be0ff636d
plan_head_after: 7f33dca732d9d80c985e2b819b49cdcb84a564a4
tech-stack:
  added: []
  patterns: [preserve configured CMake emulator lists in generated driver scripts]
key-files:
  created:
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-21-LOCAL-EVIDENCE.md
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-21-SUMMARY.md
  modified:
    - tests/diag/tests.cmake
    - .planning/LESSONS.md
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-21-LOCAL-EVIDENCE.md
key-decisions:
  - Keep runner test registrations unchanged because neither named timeout recurred in the complete corrected WASI run.
  - Treat the retained-SDK pass as local prerequisite evidence only; Phase 12 acceptance remains 0/4.
patterns-established:
  - Generated CMake scripts preserve CMAKE_CROSSCOMPILING_EMULATOR as a list for commands that launch cross-compiled drivers.
requirements-completed: []
coverage:
  - id: D1
    description: Both existing state-driver control modes pass through direct native execution and the configured WASI emulator.
    requirement: PLAY-03
    verification:
      - kind: integration
        ref: "native and WASI retroarch.states.controls CTest runs"
        status: pass
    human_judgment: false
  - id: D2
    description: The complete retained-SDK local WASI workflow passes the exact portable inventory once, without failures, errors, or skips.
    requirement: PLAY-03
    verification:
      - kind: integration
        ref: "cmake --workflow --preset wasm32; check_junit.cmake against build/wasm32/junit.xml and tests/inventory/wasm32.txt"
        status: pass
    human_judgment: false
  - id: D3
    description: The final native ordinary, package, release, formatter, and tree hygiene gates pass on the repaired source.
    requirement: PLAY-03
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; hygiene preset, hygiene.format, and scripts/hygiene.sh --tree"
        status: pass
    human_judgment: false
duration: 20min
completed: 2026-11-07
status: complete
---

# Phase 12 Plan 12-21: Repair WASI State-Control Routing Summary

**Both generated state-driver controls now use the configured WASI emulator, and the complete local WASI and native verification lanes pass.**

## Performance

- **Duration:** 20 min
- **Started:** 2026-11-07T16:39:24Z
- **Completed:** 2026-11-07T16:59:48Z
- **Tasks:** 3
- **Files touched:** 4

## Accomplishments

- Passed the existing `retroarch.states.controls` test on native and actual WASI. Both `--fault-controls` and `--prelaunch-controls` commands now preserve and expand the configured emulator list.
- Passed the complete local WASI workflow and exact portable JUnit audit: all 622 tests ran once with no failures, errors, or skips. Neither previously named runner timeout recurred, so no runner registration or deadline changed.
- Passed the full native workflow (700 ordinary tests, three ZIP packages, and six release tests), formatter, and final tree hygiene check. The bounded CMake review found no material issue.

## Task Commits

1. **Task 1: Run both state-driver control modes through the native and WASI execution routes** — `7e9c21f` (`fix`).
2. **Task 2: Complete the WASI lane and isolate only recurring runner scheduling failures** — `a33cc81` (`docs`).
3. **Task 3: Verify the final native snapshot and record the supported local prerequisite** — `7f33dca` (`docs`).

The plan metadata commit is recorded after this summary is verified.

## Files Created or Modified

- `tests/diag/tests.cmake` — Passes the configured emulator list to both generated driver-control commands.
- `.planning/LESSONS.md` — Records the reproduced generated-script emulator-list lesson.
- `12-21-LOCAL-EVIDENCE.md` — Records retained SDK/runtime identities, preserved historical failure, complete WASI/JUnit results, native/package/quality gates, resource bounds, and review.
- `12-21-SUMMARY.md` — This plan outcome and coverage record.

## Decisions Made

- No `RUN_SERIAL` change was needed: the two named cases passed under their existing limits in the corrected full WASI workflow.
- The successful WASI result is specific to the retained local advisory SDK. It is not hosted CI, released-core, actual RetroArch input, or Playstead acceptance evidence.

## Deviations from Plan

None. No package, SDK, or runtime was acquired. No frontend/content process, permission or event API, settings change, remote CI, or publication action occurred. The 12-18 denial evidence remains preserved; Phase 12 remains accepted 0/4. The orchestrator can reconcile this successful local-WASI prerequisite for Plan 12-16, while the separate causal and host-input gates remain open.

## Recorded Results

- The earlier Plan 12-20 report with the missing emulator route and two runner deadlines remains preserved as historical evidence; no cause is attributed to the timeouts.
- Complete local WASI and native results are documented in [12-21-LOCAL-EVIDENCE.md](12-21-LOCAL-EVIDENCE.md). Run URLs are N/A for local commands.
- This plan does not complete PLAY-03 or promote Phase 12 acceptance.

## Self-Check: PASSED

- The summary and evidence ledger exist, and all three task commits are ancestors of the frozen plan head.
- The measured plan commit count is 3; the 1,051 protected tracked-file identities match, and the local scratch peak remains below its cap.
- Required coverage and acceptance boundaries are present; the final native, WASI, formatter, hygiene, and audit results are recorded in the ledger.
