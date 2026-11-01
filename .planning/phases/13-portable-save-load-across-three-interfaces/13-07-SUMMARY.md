---
phase: 13-portable-save-load-across-three-interfaces
plan: "07"
subsystem: testing
tags: [c, cmake, savestate, sha256, portability]
requires:
  - phase: 13-06
    provides: libretro state delegation through the core state API
provides:
  - Thirteen in-memory continuation scenarios emit six canonical state and continuation digest rows each.
  - The existing hashes writer appends and validates the 78 state rows for its current lane consumers.
affects: [13-08, native-and-big-endian-hash-consumers]
actuals:
  tokens: 13874
  tasks: 2
  commits: 4
plan_head_before: 75eefc28742d8d080c1d5190d05c0b0f364ece26
plan_head_after: 3c7eb30eee506e389723db970783dafa99e06b29
tech-stack:
  added: []
  patterns:
    - Reuse the existing SHA-256 implementation for canonical report rows.
    - Validate ordered scenario and field keys before appending rows to the existing lane hashes file.
key-files:
  created: []
  modified:
    - tests/core/test_state.c
    - tests/core/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - .planning/phases/13-portable-save-load-across-three-interfaces/13-CONTINUATION-MATRIX.md
    - tests/cmake/hashes.cmake
    - tests/runner/tests.cmake
key-decisions:
  - Keep each scenario tied to the existing owned continuation fixture and independent semantic checks.
  - Hash numeric observations with explicit widths and little-endian encoding; do not hash native structure layouts.
  - Apply the same test-owned volume-only POKEY register write to both post-restore paths so enabled sound scenarios produce actual transition coverage.
patterns-established:
  - State reports contain exactly six rows in a fixed field order and 64 lowercase hexadecimal digest format.
  - The existing hashes file remains the only lane equality artifact; its prior rows and comparison behavior are retained.
requirements-completed: []
coverage:
  - id: D1
    description: Thirteen owned continuation scenarios produce deterministic state-byte, frame, audio-transition, PCM, counter, and semantic digests.
    requirement: STATE-03
    verification:
      - kind: unit
        ref: "core.state.equality_report; direct muhtari-test-state --report scenario runs"
        status: pass
    human_judgment: false
  - id: D2
    description: The existing lane hash producer and validator accept the ordered 78-row state report and reject incomplete or malformed evidence.
    requirement: STATE-03
    verification:
      - kind: integration
        ref: "runner.hashes_state_control in a focused CTest harness against tests/runner/tests.cmake and tests/cmake/hashes.cmake"
        status: pass
    human_judgment: false
  - id: D3
    description: The complete hashes.txt artifact is compared across all required native lanes and the executed big-endian consumer.
    requirement: STATE-03
    verification:
      - kind: integration
        ref: "runner.hashes_file; full producer invocation unavailable in this checkout"
        status: unknown
    human_judgment: false
duration: 45min
completed: 2026-11-01
status: complete
---

# Phase 13 Plan 07: Carry State Rows Into Lane Hashes Summary

**Thirteen independently checked continuation scenarios now add 78 canonical SHA-256 rows to the existing cross-lane hashes file.**

## Performance

- **Duration:** 45 min
- **Started:** 2026-11-01T17:20:00-05:00
- **Completed:** 2026-11-01T18:05:49-05:00
- **Tasks:** 2
- **Files modified:** 7

## Accomplishments

- Added `--report SCENARIO` for the 13 named in-memory state continuation scenarios, with exactly six lowercase SHA-256 rows per scenario.
- Registered `core.state.equality_report`, which checks all 78 rows, repeated-report identity, nonempty frame/audio coverage, and refusal after a deliberately corrupted independent mailbox observation.
- Appended the 78 rows after the retained observations in the current hashes writer; the existing validator now checks exact ordered keys and 64-character lowercase digests, and the runner hash controls cover state-row faults.
- Documented scenario-to-topology mappings and the sound-transition test stimulus in the continuation matrix.

## Task Commits

1. **Task 1 RED:** `3650f5a` — assert the canonical state equality report row.
2. **Task 1 GREEN:** `10f03e8` — emit canonical state continuation reports.
3. **Task 2 RED:** `57f7f76` — assert state rows through the hash consumer.
4. **Task 2 GREEN:** `3c7eb30` — join state reports to lane hashes.

The plan metadata commit is recorded separately after this summary is self-checked.

## Files Created/Modified

- `tests/core/test_state.c` — report generation, explicit canonical hashing, scenario fixtures and oracle controls.
- `tests/core/tests.cmake` — state report source registration and `core.state.equality_report` registration.
- `tests/inventory/ci.txt` and `tests/inventory/wasm32.txt` — portable test inventory entry.
- `.planning/phases/13-portable-save-load-across-three-interfaces/13-CONTINUATION-MATRIX.md` — report scenario mapping and evidence description.
- `tests/cmake/hashes.cmake` — report producer, required-key list and state-row validation in the existing hashes consumer.
- `tests/runner/tests.cmake` — 78-row count, state-specific fault controls and the actual state report executable forwarding.

## Decisions Made

- Reports reuse the existing continuation fixtures and semantic observations; no ROM, BIOS or firmware files were added.
- The saved state digest is captured before continuation. Both source and restored machines then receive the same test-owned volume-only POKEY register write; this guarantees an observed raw transition even when generated ROM contents do not program sound.
- The generated SuperGame BIOS maps over `$8000` during the fixture setup, so the test selects its banked RAM state directly in the private fixture rather than sending a mapper write the BIOS would consume.

## TDD Evidence

- **Task 1 RED:** `build/state07-red/red-evidence.json` was accepted by `gsd_run check tdd-red-evidence` as `RED_EVIDENCE_OK`; the target failed because the registered report row was absent. GREEN evidence: `core.state.equality_report` passed 1/1 in the focused CTest harness and emitted exactly 78 rows.
- **Task 2 RED:** `build/state07-task2-red/red-evidence.json` was accepted as `RED_EVIDENCE_OK`; the target failed its explicit assertion that the hashes producer expose the 13 scenario reports. GREEN evidence: the focused `runner.hashes_state_control` CTest passed 1/1 against the exact CMake consumer and state executable.

## Verification Evidence

- Strict direct C build of `tests/core/test_state.c`, `tests/cart/images.c`, and `runner/sha256.c` against `build/ci/libmuhtari.a`: exit 0, no warnings.
- Direct state cases `aliases`, `battery`, `fields`, `lifecycle`, `payload`, `topologies`, and `equality_report`: all exit 0.
- Direct `--report` invocations for all 13 named scenarios: each exits 0 and emits six rows. `--report unknown` exits 2 with empty stdout.
- Focused CTest `core.state.equality_report`: 1/1 passed; generated 78 rows and exercised the repeated-report and wrong-oracle controls.
- Focused CTest `runner.hashes_state_control`: 1/1 passed; valid exact-source reports were accepted, missing scenario/field, duplicate field and malformed digest were refused, a changed well-formed digest differed under the existing byte comparison, and the independent-oracle control passed.
- The `RUN`-relative state executable fallback used by the existing policy writer was exercised with a temporary build-layout harness: exit 0.
- `git diff --check` and `scripts/hygiene.sh`: exit 0.

The full `runner.hashes_file` producer was not run locally. The existing `build/ci` cache has `MUHTARI_TESTS=OFF`; `muhtari-test-diag`, generated soundtest/POKEY inputs and pinned `build/pins/wasmtime`, `build/pins/7800basic`, and `build/pins/openbios` are unavailable. Generating or fetching those inputs is outside this plan's constraints. The source adds the 78 rows after the existing producer rows, but the complete retained artifact and lane consumers still need their scheduled integration evidence. No hosted run or big-endian execution is claimed.

## Deviations from Plan

None. The full asset-dependent producer check remains unrun for the stated local environment constraint; the scoped report and consumer harnesses ran against the exact source.

## Issues Encountered

- The SuperGame BIOS overlay consumes writes to the mapper range before handoff. The private fixture now seeds the selected bank state directly, then validates that both bank registers survive state load.
- Some generated cartridge images do not produce POKEY transitions on their own. The report path sends the same real POKEY channel write to both paths after restore, then compares observed transitions and PCM output.

## User Setup Required

None. No external service or user configuration is required.

## Next Phase Readiness

Plan 13-08 can consume the complete report producer and hash validator. The full hashes artifact was not produced in this local checkout and no executed big-endian receipt is claimed. STATE-02's classification assumption and Phase 12's existing halt remain unchanged.

---
*Phase: 13-portable-save-load-across-three-interfaces*
*Completed: 2026-11-01*

## Self-Check: PASSED

- This summary exists at the required plan path.
- Task commits `3650f5a`, `10f03e8`, `57f7f76`, and `3c7eb30` are ancestors of the current plan head.
- The persisted plan ledger measures four task commits before this summary commit.
