---
phase: 20-complete-ym2151-homebrew-sound
plan: "01"
subsystem: audio
tags: [ym2151, xm, audio, c17, ymfm]

requires:
  - phase: 19-measured-milestone-tune-up
    provides: canonical v6.0 baseline and local verification context
provides:
  - generated public-frame YM2151 tracer with state operations kept fail-closed
  - source-qualified busy and timer assertions plus receipt-bound OPM parity
  - fixed exact-pin synthesis vectors and registered native/Wasm C tests
affects: [20-02, 20-03, YM-01, YM-02, YM-03]

actuals:
  tokens: 30266
  tasks: 2
  commits: 2
  plan_head_before: 6977f436555421551f83ae7f875ce331d0fccb1a
  plan_head_after: 9f1675adabf7d1e9306b1d1cce054805112c231d

tech-stack:
  added: [test-only C++17 OPM oracle harness]
  patterns: [receipt-bound seed inputs, signed sample-by-sample comparison, source numeric tests before parity]

key-files:
  created:
    - tests/audio/ym_oracle.cpp
    - tests/audio/ym_oracle.cmake
    - tests/audio/ym_vectors.h
  modified:
    - src/ym2151.c
    - tests/audio/test_ym2151.c
    - tests/audio/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/ym2151.md
    - PROVENANCE.md

key-decisions:
  - "Use Yamaha's documented 68 φM busy duration; report the pinned seed's 64-clock convention as a separate four-clock parity difference."
  - "Keep timer IRQ masking fixed for both flags; mode enable bits control flag latching, and reset bits clear latched flags."
  - "Keep YM state unavailable until the complete format-2/behavior-7 continuation lands in Plan 20-03."

patterns-established:
  - "C-only numeric and synthesis tests consume frozen outputs, while the update-duty harness links the already-built receipt-matched object."
  - "YM translation evidence compares raw signed stereo DAC samples; public mono PCM is a separate integration check."

requirements-completed: []
coverage:
  - id: D1
    description: "The generated XM note runs through the public frame API and YM state save/load remains fail-closed."
    requirement: YM-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^ym[.](numeric|synthesis|controls|tracer)$'"
        status: pass
    human_judgment: false
  - id: D2
    description: "Busy, timer carry, flag reset, maximum periods, exact-edge ordering, and near-epoch catch-up have executable numeric checks."
    requirement: YM-01
    verification:
      - kind: unit
        ref: "tests/audio/test_ym2151.c#ym_numeric; Yamaha Application Manual §§2.1.1, 2.1.7"
        status: pass
      - kind: integration
        ref: "cmake -DROOT=. -DSEED=build/v6-ym-seed -DWORK=build/v6-ym-oracle -P tests/audio/ym_oracle.cmake"
        status: pass
    human_judgment: false
  - id: D3
    description: "The C17 OPM translation matches the prepared pinned object over ten bounded synthesis cases and both signed output channels."
    requirement: YM-02
    verification:
      - kind: integration
        ref: "receipt-bound tests/audio/ym_oracle.cmake; 9,728 synchronized steps and 10,240 chip sample pairs"
        status: pass
      - kind: unit
        ref: "ym.synthesis and ym.controls; fixed vectors and one-bit, swapped-channel, missing-case, wrong-seed faults"
        status: pass
    human_judgment: false
  - id: D4
    description: "Existing non-YM frame, observer, and runner audio outputs retain their recorded behavior."
    requirement: YM-03
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^audio[.](frames|observer|samples)$'"
        status: pass
    human_judgment: false

duration: "~80min; approximate because execution resumed mid-plan"
completed: 2026-11-12
status: complete
---

# Phase 20 Plan 01: Public YM tracer and exact-seed OPM parity Summary

**A generated XM note now reaches public mono PCM, while source-qualified timing and receipt-bound stereo OPM parity are checked independently.**

## Performance

- **Duration:** Approximately 80 minutes; the exact start time was not carried into the resumed execution context.
- **Started:** Before Task 1 commit `11ca736` on 2026-11-12.
- **Completed:** 2026-11-12.
- **Tasks:** 2.
- **Files modified:** 20 unique files.

## Accomplishments

- Completed the public XM/YM frame tracer, bounded mono output checks, no-YM state control, and fail-closed YM save/load guard.
- Added Yamaha-source numeric busy/timer expectations and a receipt-bound oracle comparing the C port to the already-built OPM object. The corpus covers all eight algorithms, four operator positions, key/envelope phases, feedback, noise, four LFO waveforms, panning, phase wrap, repeated writes, and two interleaved instances.
- Preserved legacy no-YM audio behavior in the registered frame, observer, and runner sample checks. The YM state guard remains in place for Plan 20-03; this plan does not complete YM-05 or phase-level acceptance.

## Task Commits

1. **Task 1: Run one generated XM note through the public C frame path** — `11ca736` (`feat(20-01): add XM YM2151 public sound tracer`).
2. **Task 2: Prove independent chip numbers and actual exact-seed translation parity** — `9f1675a` (`fix(20-01): qualify YM numeric and seed parity`).

The summary and planning state are recorded in the final metadata commit.

## Files Created/Modified

- `src/ym2151.c` — production OPM specialization; busy duration, timer IRQ semantics, and phase-derived raw DAC polarity now match their qualified contracts.
- `src/internal.h`, `src/instance.c`, `src/cart.c`, `src/bus.c`, `src/synth.c`, `src/state.c` — per-instance YM ownership, topology, timing, mixing, and fail-closed state integration.
- `tests/cart/images.c`, `tests/cart/images.h`, `tests/audio/test_ym2151.c` — generated public tracer, numeric boundaries, fixed vectors, and comparator fault controls.
- `tests/audio/ym_oracle.cpp`, `tests/audio/ym_oracle.cmake`, `tests/audio/ym_vectors.h` — exact-object comparator, receipt checks, named corpus, and frozen outputs.
- `tests/audio/tests.cmake`, `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt` — registered YM tracer, numeric, synthesis, and controls tests.
- `docs/hardware/ym2151.md`, `PROVENANCE.md`, `THIRD-PARTY-NOTICES.md` — selected digital profile, numeric bounds, corpus maintenance duty, source pin, and BSD-3 attribution.
- `CMakeLists.txt` — explicit production C source and test target wiring.

## Decisions Made

- Yamaha Application Manual §2.1.1 specifies a 68 `φM` busy duration. The pinned seed schedules 64 clocks from `32 * prescale(2)`. Production follows 68; the oracle checks busy separately and reports exactly four differing clocks. The two values are not treated as equivalent.
- Application Manual §2.1.7 timer periods are checked directly at shortest and maximum values. Timer flags remain latched when a carry-enable bit is cleared; the corresponding reset bit clears the flag and IRQ.
- The exact-seed comparator checks both signed 16-bit channels before the mono mixer. A separate raw-DAC assertion requires positive and negative samples on both channels; the public PCM tracer alone would not establish raw YM polarity.
- YM state sizing/save/load remains unavailable until Plan 20-03 installs complete continuation. No lossy legacy snapshot is exposed in this plan.

## Verification Evidence

- `cmake --preset ci && cmake --build --preset ci --target muhtari-test-ym2151 muhtari` — exit 0.
- `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^ym[.](numeric|synthesis|controls|tracer)$'` — exit 0; all four tests passed.
- `cmake -DROOT=. -DSEED=build/v6-ym-seed -DWORK=build/v6-ym-oracle -P tests/audio/ym_oracle.cmake` — exit 0. The script verified all six prepared input hashes and the existing object SHA-256 `7585c505b90b9a3fe3cd4917b819e56515a1baaa951484d07706353a9b06d5f8` from `build/v6-ym-seed/source-receipt.json`, then compared 9,728 synchronized steps (10,240 chip sample pairs including the two-chip case). It did not fetch or rebuild the seed.
- `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^audio[.](frames|observer|samples)$'` — exit 0; all 11 selected checks passed, including eight existing fixture builds and the eight runner audio comparisons.
- Run URLs: `N/A-local`.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Derived raw YM output polarity from the phase bit**
- **Found during:** Task 2.
- **Issue:** The C port derived sign after folding the sine table to absolute attenuation, which had already discarded negative-half polarity.
- **Fix:** Use phase bit 9 for signed raw DAC polarity and add an explicit positive/negative assertion on both channels.
- **Files modified:** `src/ym2151.c`, `tests/audio/test_ym2151.c`, `tests/audio/ym_vectors.h`.
- **Verification:** Exact per-sample parity against the prepared seed and passing `ym.synthesis`.
- **Committed in:** `9f1675a`.

**2. [Rule 1 - Bug] Kept the OPM IRQ mask independent of timer mode enables**
- **Found during:** Task 2.
- **Issue:** Replacing the seed's fixed two-flag IRQ mask from mode bits could suppress a previously latched flag when its carry-enable bit was cleared.
- **Fix:** Keep both internal flag bits masked-in; timer mode bits gate flag latching and reset bits clear latched status.
- **Files modified:** `src/ym2151.c`, `tests/audio/test_ym2151.c`, `docs/hardware/ym2151.md`.
- **Verification:** Numeric reset/enable checks and per-clock status, deadline, and IRQ parity against the exact object.
- **Committed in:** `9f1675a`.

**Total deviations:** 2 auto-fixed correctness issues. Both were found by the planned numeric/parity work; no scope was added.

## Issues Encountered

- The first harness compile omitted the public `include/` directory; the script now supplies it. A temporary diagnostic also attempted to link a non-exported template helper and was removed. The corpus initially duplicated an incorrect action count; it now derives each count from its static action array.
- The first synthesis comparison exposed the raw polarity defect recorded above. After correction, every named case and exact numeric boundary passed.

## User Setup Required

None. All work and verification were local.

## Next Phase Readiness

Plan 20-01 is complete and committed. Its report and planning bookkeeping are ready for the orchestrator to select the next sequential plan. YM-05 remains incomplete; the state refusal stays active until Plan 20-03. Phase 20 remains unaccepted pending its remaining plans, full verification, and review gates.

---
*Phase: 20-complete-ym2151-homebrew-sound*
*Completed: 2026-11-12*

## Self-Check: PASSED

The summary exists, and both task commits (`11ca736`, `9f1675a`) are ancestors of the current branch head.
