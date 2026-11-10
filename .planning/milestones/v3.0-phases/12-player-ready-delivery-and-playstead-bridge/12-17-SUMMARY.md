---
phase: 12-player-ready-delivery-and-playstead-bridge
plan: "17"
subsystem: testing
tags:
  - wasm32
  - wasi
  - test-inventory
requires: []
provides:
  - "Focused WASI execution of the actual preflight mock using a coherent, pinned SDK/runtime bundle."
affects:
  - PLAY-03
tech-stack:
  added: []
  patterns:
    - "Private, source-bound advisory toolchain evidence."
key-files:
  created:
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-17-LOCAL-EVIDENCE.md
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-17-SUMMARY.md
  modified: []
key-decisions:
  - "Use only the exact wasi-sdk-28 arm64 macOS bundle with its bundled sysroot and the existing pinned Wasmtime."
  - "Halt the full lane at the existing tmpfile() use; no source change, fallback or retry is within this plan."
requirements-completed: []
coverage:
  - id: focused-wasi-preflight
    description: "The actual preflight mock compiles, links and passes its two owning registrations as a WASI module."
    requirement: PLAY-03
    verification:
      - kind: integration
        ref: "cmake --build --preset wasm32 --target muhtari-test-preflight; focused CTest selector; tracer-wasi.xml"
        status: pass
    human_judgment: false
  - id: complete-local-wasi-lane
    description: "The complete wasm32 workflow and portable JUnit completeness audit run against the unchanged inventory."
    requirement: PLAY-03
    verification:
      - kind: integration
        ref: "cmake --workflow --preset wasm32; build stopped at tests/core/test_state.c:2022 because tmpfile() is deprecated under -Werror; no JUnit report"
        status: fail
    human_judgment: false
metrics:
  duration: "17 min"
  completed: 2026-11-06
  status: halted
duration: "17 min"
completed: 2026-11-06
status: halted
plan_head_before: 23d18aac72566081852f6a0b39651164fa212c2f
plan_head_after: c164b4ec04097846e0d33258c7d738c16464bed5
commits: 2
actuals:
  tokens: 2584
  tasks: 1
  commits: 2
---

# Phase 12 Plan 12-17: WASI Lane Summary

**The exact SDK runs the focused WASI mock, but the complete local lane halts on an existing unsupported `tmpfile()` call.**

## Performance

- **Duration:** 17 min
- **Started:** 2026-11-07T04:22:50Z
- **Completed:** 2026-11-07T04:40:00Z
- **Tasks:** 1 of 2 completed
- **Files modified:** 1 plan evidence ledger

## Accomplishments

- Downloaded and verified the exact SDK 28.0 asset once, retained its official notices, and recorded the compiler, linker, builtins and sysroot identities.
- Configured the unchanged wasm32 inventory and passed the actual preflight mock’s two owning tests under pinned Wasmtime.
- Ran the complete workflow once; it halted during compilation at `tests/core/test_state.c:2022` because `tmpfile()` is undefined on WASI and `-Werror` rejects its deprecation diagnostic.

## Task Commits

1. **Task 1: Run the actual preflight mock as WASI** — `7d505a0`.
2. **Task 2: Complete the portable lane** — `c164b4e` records the terminal build failure; no retry or source repair.

## Files Created/Modified

- [12-17-LOCAL-EVIDENCE.md](12-17-LOCAL-EVIDENCE.md) — exact SDK identity, focused test results and full-lane failure.
- `build/phase12-gap17-evidence/` — ignored archive, notices, build receipts, logs and JUnit report.

## Decisions Made

- Kept the SDK toolchain and sysroot coherent and private; local SDK evidence remains distinct from hosted CI’s WASI sysroot.
- Preserved the full-lane failure without changing source or repeating the workflow, as required by this plan’s bounded scope.

## Deviations from Plan

None. The full lane produced a terminal failure explicitly allowed by the plan’s stop condition.

## Recorded Results

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset wasm32` | 0 | Configure and exact inventory check passed. |
| `cmake --build --preset wasm32 --target muhtari-test-preflight` | 0 | Actual C mock built and linked as a WebAssembly module. |
| Focused CTest selector for `preflight_api` and `child_failure` | 0 | Both tests passed; JUnit reports two tests, no failures or skips. |
| `cmake --workflow --preset wasm32` | 1 | Build stopped at `tmpfile()` under `-Werror`; workflow never reached tests. |
| Full-lane `check_junit.cmake` | not run | No new full-lane JUnit file was produced. |

Raw output and the prior wasm32 cache/JUnit remain in ignored `build/phase12-gap17-evidence/`. No test, source, pin, preset, inventory or workflow file changed. The focused pass closes only the named mock execution check. PLAY-03 remains pending, Phase 12 remains accepted 0/4, and 12-16 remains blocked until all three successor plans pass.

## Issues Encountered

The existing `tmpfile()` use is incompatible with this SDK’s WASI headers under the project’s strict warning policy. This plan excludes a source repair or retry. The full portable lane and JUnit audit therefore remain unverified.

## Next Phase Readiness

Plan 12-18 remains independently runnable and can prepare its identity-bound owner decision. Plans 12-19 and 12-16 remain gated by their dependencies. This result does not clear the phase halt or any host/remote allowance.

## Self-Check: PASSED

- The local evidence ledger, focused JUnit report, full workflow log and SDK identity manifests exist.
- The focused report contains exactly two passing, non-skipped tests.
- The full workflow’s failed build result and the absence of a full JUnit report are stated accurately.
