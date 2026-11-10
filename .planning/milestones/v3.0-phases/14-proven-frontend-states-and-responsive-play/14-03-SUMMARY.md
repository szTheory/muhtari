---
phase: 14-proven-frontend-states-and-responsive-play
plan: "03"
subsystem: testing
tags: [retroarch, libretro, C, save-load, process-control]

requires:
  - phase: 14-proven-frontend-states-and-responsive-play
    provides: Independent progress, picture, and raw-save fixture oracles
  - phase: 13-portable-save-load-across-three-interfaces
    provides: Portable machine-state contract and libretro state behavior
provides:
  - Bounded owned-child RetroArch save/load sequence with fresh progress and picture observations
  - Portable protocol, identity, stale-evidence, path, and cleanup fault controls
  - Explicit opt-in workflow wiring for the later source-bound host invocation
affects: [14-04, 14-05]

actuals:
  tokens: 25379
  tasks: 2
  commits: 4
commits: 4
plan_head_before: 1837b5ce592918d86fcd57f0c728a5cd570dfd6c
plan_head_after: bfab8001fd3a3029a3c8813c275acebbf0ec07ad

tech-stack:
  added: []
  patterns:
    - Shared transport-injected control sequence for host and owned mock tests
    - Single-send mutations accepted only after bounded independent observations

key-files:
  created:
    - tests/retroarch/states.c
  modified:
    - .github/workflows/ci.yml
    - tests/diag/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/retroarch/run_retroarch.cmake
    - tests/retroarch/test.cfg.in
    - .planning/phases/14-proven-frontend-states-and-responsive-play/deferred-items.md

key-decisions:
  - "Require exact RetroArch version, core, fixture basename and CRC before issuing state commands."
  - "Treat ambiguous state changes as failed after one send; require observed progress, fresh pictures, and newly created files."
  - "Keep FRONT-02, FRONT-03, and FRONT-04 pending until the final source-bound host evidence in plan 14-05."

requirements-completed: []
coverage:
  - id: D1
    description: The owned mock exercises the bounded save, advance, load, fresh-picture, replay, and cleanup decisions locally; this is not pinned-host acceptance.
    requirement: FRONT-02
    verification:
      - kind: integration
        ref: "build/ci/tests/retroarch-states-control --controls"
        status: pass
    human_judgment: false
  - id: D2
    description: Named protocol, identity, path, stale-observation, and cleanup faults are rejected while the unrelated child and save sentinel survive.
    verification:
      - kind: integration
        ref: "ctest --preset ci -R '^(retroarch.states.controls|diag.frontend_states|libretro.host.state)$' --no-tests=error --output-on-failure"
        status: pass
    human_judgment: false
  - id: D3
    description: The Phase 14 host launch is structurally guarded by the explicit workflow_dispatch input and stays disabled until plan 14-05.
    verification:
      - kind: other
        ref: "actionlint -shellcheck='' -ignore 'label \"ubuntu-26\\.04' -ignore 'label \"ubuntu-26\\.04-arm' .github/workflows/ci.yml"
        status: pass
    human_judgment: false

duration: 29min
completed: 2026-11-03
status: complete
---

# Phase 14 Plan 03: Proven Frontend States and Responsive Play Summary

**A bounded owned-child RetroArch save/load path now checks exact identity, restored progress, fresh pictures, and replay, with portable fault controls behind a disabled host-launch gate.**

## Performance

- **Duration:** 29 minutes
- **Started:** 2026-11-03T04:38:12Z
- **Completed:** 2026-11-03T05:07:26Z
- **Tasks:** 2
- **Files modified:** 8

## Accomplishments

- Added the finite save/advance/load/replay path with a 45-second work deadline, five-second cleanup reserve, at most 64 explicit frame advances, exact frontend and fixture identity, progress-memory reads, fresh picture judgments, and owned-child cleanup.
- Added a states-only RetroArch configuration and isolated launcher mode with pre-cleanup path guards. The existing DMA and battery launches remain unchanged.
- Registered portable controls in the diagnostic CMake owner and CI/WASI inventories. Added a `workflow_dispatch` input that defaults false and gates only the new Phase 14 host step.
- Kept the live host case unlaunched and did not dispatch CI or use any Phase 12 input allowance.

## Task Commits

1. **Task 1 RED: cover the owned save/load path** — `c634b51` (`test`)
2. **Task 1 GREEN: add the owned RetroArch save/load path** — `9aab287` (`feat`)
3. **Task 2 RED: register save/load fault controls** — `7186a52` (`test`)
4. **Task 2 GREEN: add strict state fault controls** — `bfab800` (`feat`)

## TDD Gate Compliance

- Task 1's owned-mock valid-sequence assertion failed before the implementation, and the RED evidence classifier accepted the named failure. The implementation then passed the direct `--controls` run and strict C17 compilation.
- Task 2's registered control failed specifically because a wrong VERSION reply produced only a generic identity error; the classifier accepted that assertion-level RED. The fix added named identity causes and fault controls.
- After Task 2, `cmake --preset ci`, `cmake --build --preset ci`, and the targeted CTest command for `retroarch.states.controls`, `diag.frontend_states`, and `libretro.host.state` all passed.
- The scoped workflow syntax check passed. Full local actionlint also reported existing runner-label schema warnings and SC2017/SC2059 in the unchanged pixel negative-control shell; those are recorded in `deferred-items.md`.

## Files Created/Modified

- `tests/retroarch/states.c` — shared finite save/load engine, strict NCI reply parsing, macOS owned-process controls, and portable fault suite.
- `tests/retroarch/run_retroarch.cmake` — isolated states mode and symlink-aware pre-cleanup path rejection.
- `tests/retroarch/test.cfg.in` — states-only network, slot, rewind, and SRAM configuration.
- `.github/workflows/ci.yml` — optional, default-disabled Phase 14 host step.
- `tests/diag/tests.cmake`, `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt` — recurring portable control registration.
- `deferred-items.md` — records the pre-existing actionlint diagnostics and the planning Windows-ledger mismatch.

## Decisions Made

- Frontend identity requires the exact version, `atari_7800` core, fixture basename, and independently calculated fixture CRC before mutating commands are sent.
- Save/load and frame-advance commands are sent once. A timeout or ambiguous response fails closed; only independently observed file, progress, and picture changes count as completion.
- Local mock evidence does not complete the pinned-host requirements. FRONT-02/03/04 stay pending for plan 14-05.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Avoid a second wait on an already reaped command child**
- **Found during:** Task 2 cleanup review
- **Issue:** The macOS command-capture loop could call `waitpid` again after it had already reaped the child, turning `ECHILD` into a false failure.
- **Fix:** Skip the second wait once the child has been reaped.
- **Files modified:** `tests/retroarch/states.c`
- **Verification:** Strict native C17 build and the targeted controls passed.
- **Committed in:** `bfab800`

**2. [Rule 1 - Bug] Reap the owned child when QUIT cannot be sent**
- **Found during:** Task 2 cleanup review
- **Issue:** A failed QUIT send only waited for the child and could leave it running past cleanup.
- **Fix:** Route the failure through the existing ownership-checked wait, TERM, and KILL cleanup sequence; the command failure remains fatal.
- **Files modified:** `tests/retroarch/states.c`
- **Verification:** The interrupted/hung-child control reaped its owned child and preserved the unrelated process and save sentinel.
- **Committed in:** `bfab800`

**Total deviations:** 2 auto-fixed (Rule 1)
**Impact on plan:** Both fixes make the planned bounded capture and cleanup paths fail correctly; no new runtime scope or allowance was added.

## Issues Encountered

- The local sandbox helper preflight was denied by the execution environment (`sandbox_apply: Operation not permitted`). No RetroArch process was started. The final hosted prelaunch check must still prove loopback-only behavior before any host launch.
- A best-effort append to `.planning/WINDOWS.md` was rejected because its frontmatter counts do not match its entries. No ledger repair was made; the blocker is recorded in `deferred-items.md`.
- A broad web search briefly surfaced a GPL source snippet in a result. No GPL source page was opened, copied, or used; implementation reference material came from the official Libretro NCI documentation.

## User Setup Required

None.

## Next Phase Readiness

Plan 14-04 can add rewind and run-ahead on the shared local control framework. Plan 14-05 must prove the sandbox negative control and actual pinned-host round trip before enabling the one source-bound host invocation. FRONT-02/03/04 remain pending; Phase 12 remains halted and its hosted allowance stays spent.

## Self-Check: PASSED

The summary exists, all four task commits are ancestors of the plan head, and `git diff --check` passed.

---
*Phase: 14-proven-frontend-states-and-responsive-play*
*Completed: 2026-11-03*
