---
phase: 27-headless-developer-trace
plan: "01"
subsystem: testing
tags: [cpu-trace, jsonl, CMake, CTest]
requires: []
provides:
  - Private selected-frame CPU instruction and bus tracing through muhtari-run
  - Generated numeric oracle and CPU timing edge coverage
affects: [27-02, 27-03, runner, CPU timing]
commits: 3
plan_head_before: b917df7d9126c6bf275be10569ad5a4a072332ad
plan_head_after: 99e5b695d4446f9945e1a6e386d53eb31d0076ed
actuals:
  tokens: 14357.5
  tasks: 2
  commits: 3
tech-stack:
  added: []
  patterns:
    - Select ordinary or traced CPU execution once at frame entry
    - Keep observer ownership outside canonical machine state
    - Verify JSONL numeric records against a build-local generated cartridge
key-files:
  created:
    - tests/runner/test_trace.c
  modified:
    - CMakeLists.txt
    - docs/runner.md
    - runner/main.c
    - src/bus.c
    - src/frame.c
    - src/internal.h
    - src/maria.c
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/runner/trace.cmake
key-decisions:
  - Keep trace callbacks private and per-instance, outside serialized state and public compatibility surfaces.
  - Select traced execution at frame entry; the measured hooks-off instruction-cost gate remains for plan 27-03.
requirements-completed: [TRACE-01, TRACE-02, TRACE-04, TRACE-05]
requirements-pending: [TRACE-06]
coverage:
  - id: D1
    description: The runner captures one selected invocation frame and independently filters instruction PCs and CPU bus addresses.
    requirement: TRACE-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.](oracle|filters)$' (exit 0; run URL: N/A-local)"
        status: pass
    human_judgment: false
  - id: D2
    description: Trace records pin numeric CPU fields, data-edge ticks, beam position, and instruction-before-opcode-bus order across timing edges.
    requirement: TRACE-02
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure -R '^(runner[.]trace[.]oracle|core[.]trace[.]edges)$' (exit 0; run URL: N/A-local)"
        status: pass
    human_judgment: false
  - id: D3
    description: A generated cartridge matches an independent numeric trace oracle and traced runs preserve ordinary output and frame/state hashes.
    requirement: TRACE-04
    verification:
      - kind: e2e
        ref: "ctest --test-dir build/ci --output-on-failure -R '^(runner[.]trace[.]smoke|runner[.]trace[.]oracle)$' (exit 0; run URL: N/A-local)"
        status: pass
    human_judgment: false
  - id: D4
    description: Trace observer state stays private and outside public headers and canonical state serialization.
    requirement: TRACE-05
    verification:
      - kind: unit
        ref: "core.trace.edges includes two instances, save/load, detach, and reattach checks (exit 0; run URL: N/A-local)"
        status: pass
    human_judgment: false
  - id: D5
    description: The trace-disabled build omits traced support and refuses trace requests; the instruction-cost requirement remains unverified until plan 27-03.
    requirement: TRACE-06
    verification:
      - kind: integration
        ref: "cmake -S . -B build/trace-off -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON -DMUHTARI_TRACE_LOOP=OFF -DMUHTARI_TESTS=OFF -DMUHTARI_BUILD_LIBRETRO=OFF && cmake --build build/trace-off --target muhtari-run (exit 0; run URL: N/A-local); trace request returned 77 with MUHTARI_TRACE_LOOP=OFF refusal (run URL: N/A-local)"
        status: unknown
    human_judgment: false
duration: 79min
completed: 2026-11-16
status: complete
---

# Phase 27 Plan 01: Headless developer trace Summary

**Private per-instance runner tracing now emits deterministic instruction and CPU bus timing records with a generated numeric oracle.**

## Performance

- **Duration:** 79 min
- **Started:** 2026-11-16T19:14:31Z
- **Completed:** 2026-11-16T20:33:45Z
- **Tasks:** 2
- **Files modified:** 11

## Accomplishments

- Added a private per-instance observer path for one selected runner frame, with separate instruction-PC and CPU bus-address filtering and bounded JSONL output.
- Added build-local generated diagnostics, exact record/timing checks, independent filter cases, and corruption controls for wrong ticks, same-tick order, CPU fields, and range inclusion.
- Verified the ordinary output and frame/state hashes stay identical with tracing enabled; the trace-disabled runner builds and returns its explicit capability refusal.

## Task Commits

1. **Task 1: Trace one real selected frame from CLI through CPU and bus to JSONL**
   - ad28080 — failing runner trace smoke test (valid RED evidence)
   - 9720d79 — capture selected CPU trace frame
2. **Task 2: Pin independent event fields, address semantics and beam transitions**
   - 99e5b69 — pin independent CPU trace contract

**Measured plan task commits:** 3 (b917df7d9126c6bf275be10569ad5a4a072332ad through 99e5b695d4446f9945e1a6e386d53eb31d0076ed).

## Files Created/Modified

- CMakeLists.txt — compiled trace selection and test registration.
- src/internal.h, src/frame.c, src/bus.c, src/maria.c — private observer contract, traced execution path, and data-edge/beam sampling.
- runner/main.c — trace CLI, filtering, capture, and JSONL publication.
- docs/runner.md — trace invocation and record documentation.
- tests/runner/trace.cmake, tests/runner/test_trace.c — runner smoke, generated numeric oracle, filters, timing edges, and corruption controls.
- tests/inventory/ci.txt, tests/inventory/wasm32.txt — focused trace checks in shared inventories.

## Decisions Made

- The observer remains private to each instance and outside serialized state; no public header or compatibility version changed.
- Ordinary and traced CPU/bus variants are chosen once per frame. This plan verifies the trace-disabled capability path but makes no hooks-off performance claim; plan 27-03 owns that measurement.

## Deviations from Plan

### Auto-fixed Test Expectations

**1. Corrected generated-fixture tail values and JAM event counts**
- **Found during:** Task 2
- **Issue:** Initial expected values assumed the wrong mapped byte at address 65535 and omitted the documented JAM dummy/held-address reads. The fixture and CPU behavior were correct.
- **Fix:** Updated expected register/value fields and callback counts to match the fixture image mapping and documented bus sequence.
- **Files modified:** tests/runner/test_trace.c
- **Verification:** The full trace oracle, filter and core edge suites passed; intentional corruption cases failed on the intended numeric/order/filter assertions.
- **Commit:** 99e5b69

**2. Task 2 had no valid RED/GREEN implementation pair**
- **Found during:** Task 2
- **Issue:** Task 1 had already delivered the production trace behavior that Task 2's acceptance tests exercise. The first Task 2 failures were invalid RED evidence caused by incorrect test expectations; the corrected contract tests passed without a Task 2 production change.
- **Fix:** Kept Task 2 test-only and recorded the limitation instead of inventing a producer change or claiming the invalid run as RED.
- **Files modified:** tests/runner/test_trace.c, tests/runner/trace.cmake, tests/inventory/ci.txt, tests/inventory/wasm32.txt
- **Verification:** Corrected focused trace CTests passed 4/4. Invalid early evidence remains build-local and is not counted as RED.
- **Commit:** 99e5b69

**Total deviations:** 2 (test expectation corrections and TDD evidence limitation).
**Impact on plan:** No production behavior remained missing after Task 1. Task 2 strengthened independent acceptance coverage without a justified additional production change.

## TDD Gate Compliance

- **Task 1 RED:** ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.]smoke$' exited 8 because the actual runner rejected the planned trace options. The named target failed for the intended missing behavior; the evidence classifier returned RED_EVIDENCE_OK. Task 1 GREEN is 9720d79.
- **Task 2:** The initial failed numeric runs were invalid because the test oracle expected incorrect fixture/JAM values. After correction, all Task 2 behavior already passed against Task 1's implementation, so no valid Task 2 RED or implementation GREEN commit exists. Corruption controls now demonstrate rejection of altered records.

## Verification

- cmake --preset ci — exit 0.
- cmake --build --preset ci --target muhtari-run muhtari-test-trace muhtari-test-cpu muhtari-test-bus — exit 0.
- ctest --test-dir build/ci --output-on-failure -R '^(runner[.]trace[.](oracle|filters)|core[.]trace[.]edges|runner[.]trace[.]smoke)$' — exit 0, 4/4 passed.
- ctest --test-dir build/ci --output-on-failure -R '^(cpu[.](nmi|jam)[.]|bus[.])' — exit 0, 26/26 passed.
- Trace-disabled configure/build with -DMUHTARI_TRACE_LOOP=OFF -DMUHTARI_TESTS=OFF -DMUHTARI_BUILD_LIBRETRO=OFF — exit 0; valid trace request returned expected exit 77 and MUHTARI_TRACE_LOOP=OFF refusal.
- Run URLs: N/A-local. No RetroArch, device, or external-host checks were run.

## Issues Encountered

The optional GSD WINDOWS deviation append did not write because the existing ledger frontmatter totals disagree with its entries. The mismatch is logged in deferred-items.md; the shared ledger was left unchanged.

## Threat Flags

| Flag | File | Description |
|------|------|-------------|
| threat_flag: user-controlled-file-write | runner/main.c | Adds --trace FILE output using an exclusive sibling temporary file and atomic replacement; dedicated output-path and I/O fault controls continue in plan 27-02. |

## Known Stubs

None found in the files created or modified by this plan.

## Next Phase Readiness

Plan 27-02 can add invalid path, output failure, and truncation fault controls on the established private trace path. Plan 27-03 still owns the measured hooks-off instruction-cost gate; this summary makes no performance claim for that gate.

---
*Phase: 27-headless-developer-trace*
*Completed: 2026-11-16*

## Self-Check: PASSED

- SUMMARY.md exists at the required phase path.
- The phase deferred-items.md records the existing WINDOWS ledger mismatch.
- TRACE-06 remains pending for the 27-03 performance measurement; no hooks-off cost claim is made.
- Task commits ad28080, 9720d79 and 99e5b69 are ancestors of the worktree HEAD.
