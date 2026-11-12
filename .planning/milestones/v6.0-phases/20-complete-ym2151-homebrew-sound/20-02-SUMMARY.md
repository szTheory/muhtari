---
phase: 20-complete-ym2151-homebrew-sound
plan: "02"
subsystem: audio
tags: [ym2151, xm, timers, irq, pcm, observer, lifecycle]

requires:
  - phase: 20-01
    provides: source-qualified YM tracer, numeric model, and pinned OPM parity baseline
provides:
  - registered topology, timing, timer, IRQ, mixing, observer, lifecycle, and hostile-work checks
  - signed YM observation records and documented integrated mixer/carry bounds
  - deterministic power-cycle reset of YM clocked state and derived caches
affects: [20-03, YM-01, YM-03, YM-04, YM-08]

actuals:
  tokens: 15650
  tasks: 3
  commits: 3
  plan_head_before: 41322680de15b5efcad2ab0ccef25f0dd81745fc
  plan_head_after: 3d9b85dd36f46458dfcfdeaa559615a265dd29e0

tech-stack:
  added: []
  patterns: [registered C fixtures, signed observer bits, trace-derived hostile IRQ fixture]

key-files:
  created: []
  modified:
    - src/bus.c
    - src/internal.h
    - src/ym2151.c
    - tests/audio/test_ym2151.c
    - tests/audio/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/audio.md
    - docs/hardware/ym2151.md

key-decisions:
  - "Preserve the exact $0470 D7 register gate and the qualified timer/status/IRQ path; busy remains 68 φM periods, separately distinguished from the pinned seed's 64-clock convention."
  - "Keep signed YM samples in sources 10 and 11 and verify the actual integrated public C output at the supported 48 kHz rate for NTSC and PAL."
  - "Clear all clocked and derived YM runtime fields on power-cycle; retain the YM state-unavailable guard until Plan 20-03 installs format 2/behavior 7 continuation atomically."

patterns-established:
  - "CMake-registered YM fixtures exercise public frame boundaries and private counters without adding runtime allocation or scheduler quotas."
  - "Hostile traffic keeps IRQs masked across YM address/data pairs, then admits the pending IRQ into an inert cursor-derived spin loop."

requirements-completed: []
coverage:
  - id: D1
    description: "Declared topology, exact D7 gating, absolute clock phase, timer deadlines, and qualified YM IRQ routing remain covered."
    requirement: YM-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure -R '^(ym[.](profile|clock|irq)|cart[.].*)$'"
        status: pass
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure -R '^(sally[.]irq|cpu[.]irq[.]timing)$'"
        status: pass
    human_judgment: false
  - id: D2
    description: "Signed YM samples, the integrated mono mixer, observer ordering/carry, and legacy audio outputs have executable coverage."
    requirement: YM-03
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^(ym[.](mix|observer)|audio[.](frames|observer|stress))$'"
        status: pass
    human_judgment: false
  - id: D3
    description: "Power-cycle continuation matches a fresh machine; regions remain isolated and hostile frames stay bounded without late allocation or buffer writes."
    requirement: YM-04
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^ym[.](lifecycle|hostile)$'"
        status: pass
    human_judgment: false
  - id: D4
    description: "Existing hardware documentation records the supported signed sources, clock profile, lifecycle reset, and integrated carry proof."
    requirement: YM-08
    verification:
      - kind: other
        ref: "docs/hardware/ym2151.md; docs/hardware/audio.md; ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^ym[.]'"
        status: pass
    human_judgment: false

duration: "~84 min"
completed: 2026-11-12
status: complete
---

# Phase 20 Plan 02: YM bus, signed output, and lifecycle Summary

**The qualified YM profile now has registered timing/IRQ, signed-mix, observer, lifecycle, and hostile-work checks, with reset state matching a fresh instance.**

## Performance

- **Duration:** Approximately 84 minutes; this spans the recorded 20-01 session boundary and is an estimate.
- **Started:** Approximately 2026-11-12T06:01Z.
- **Completed:** 2026-11-12.
- **Tasks:** 3.
- **Files modified:** 9.

## Accomplishments

- Added native and Wasm inventory entries for `ym.profile`, `ym.clock`, `ym.irq`, `ym.mix`, `ym.observer`, `ym.lifecycle`, and `ym.hostile`. The tests retain the `$0470` D7 gate and qualified status/timer/IRQ behavior.
- Added signed YM source IDs 10 and 11 to the observer and proved signed endpoints, source ordering, long carry, and actual integrated public C output. Updated the audio and YM hardware notes with the source map and numerical carry/lifecycle behavior.
- Compared interleaved PAL and NTSC instances, reset a live YM machine against a fresh equivalent, checked accepted/refused transitions and no-post-load allocations, and bounded both legal register traffic and a jammed CPU through public frame calls.
- Found and fixed a real power-cycle bug: reset had preserved clock/divider and LFO/noise/cache fields, so the restarted chip could diverge from a fresh load. Reset now clears the complete YM runtime and rebuilds the same deterministic baseline.
- Kept the state-unavailable guard intact. Plan 20-03 still owns atomic installation of complete format-2/behavior-7 continuation.

## Verification

All commands were local. No seed was refetched or rebuilt.

- `cmake --preset ci` — exit 0.
- `cmake --build --preset ci --target muhtari-test-ym2151 muhtari-test-cart` — exit 0.
- `ctest --test-dir build/ci --output-on-failure -R '^(ym[.](profile|clock|irq)|cart[.].*)$'` — exit 0; 24/24 passed.
- `ctest --test-dir build/ci --output-on-failure -R '^(sally[.]irq|cpu[.]irq[.]timing)$'` — exit 0; 2/2 passed.
- `cmake --preset ci && cmake --build --preset ci --target muhtari-test-ym2151 muhtari-test-audio-contracts muhtari-test-audio-stress && ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^(ym[.](mix|observer)|audio[.](frames|observer|stress))$'` — exit 0; 5/5 passed.
- `cmake --preset ci && cmake --build --preset ci --target muhtari-test-ym2151 && ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^ym[.](lifecycle|hostile)$'` — exit 0; 2/2 passed.
- Final combined check `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^ym[.]'` — exit 0; all 11 YM tests passed.
- Focused hostile run `cmake --build --preset ci --target muhtari-test-ym2151 && build/ci/tests/muhtari-test-ym2151 hostile` — exit 0 after the final fixture correction.
- `git diff --check` — exit 0. Commit hooks, including repository hygiene, passed for all three task commits.

## Task Commits

1. **Task 1: Qualify declared topology, latch, clock and IRQ edge behavior** — `7eeb5cd` (`feat`).
2. **Task 2: Prove signed mixing, observation and carry bounds at every supported rate** — `f4d210b` (`feat`).
3. **Task 3: Exercise complete lifecycle, isolation and hostile bounded work** — `3d9b85d` (`feat`).

The final GSD metadata commit records this summary and planning progress separately.

## Files Modified

- `src/bus.c`, `src/internal.h`, and `src/ym2151.c` — due-event handling, signed output records, and complete YM reset behavior.
- `tests/audio/test_ym2151.c`, `tests/audio/tests.cmake`, and both test inventories — registered profile, clock, IRQ, mix, observation, lifecycle, and hostile contracts.
- `docs/hardware/audio.md` and `docs/hardware/ym2151.md` — signed source mapping, integrated carry arithmetic, clock profile, and reset contract.

## Decisions Made

- Retained the installed-qualified profile: whole-byte `$0470` D7 gating, the established timer/status/IRQ model, and Yamaha's 68 φM busy duration. The pinned seed's 64-clock convention remains a distinct parity difference.
- The public API currently exposes 48 kHz; the integrated C-output checks cover that supported rate in NTSC and PAL. No new public sample-rate capability was added.
- Kept state unavailable for YM until Plan 20-03 installs complete format-2/behavior-7 continuation. No phase requirements were closed here.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Clear stale chip runtime during power-cycle**

- **Found during:** Task 3, reset-vs-fresh comparison.
- **Issue:** `muhtari__ym2151_reset()` retained clocks, sample divider, LFO/noise runtime and derived tables from the active machine.
- **Fix:** Zero the complete YM structure, then establish the deterministic initial clock, noise LFSR, prescale, mask, and channel defaults.
- **Files modified:** `src/ym2151.c`, `tests/audio/test_ym2151.c`, `docs/hardware/ym2151.md`.
- **Verification:** `ym.lifecycle` compares chip/synth state and continuation audio against a fresh equivalent; the final 11-test YM suite passes.
- **Committed in:** `3d9b85d`.

**Total deviations:** 1 automatic production correctness fix, plus test-fixture corrections below. No phase scope was added.

## Issues Encountered

- **Task 1 fixture/build corrections:** Initial test compilation caught a `size_t` comparison and use of a nonexistent Maria event field; the comparison and event lookup were corrected. The first `ym.profile` run omitted D7 enable, so the fixture now enables the exact latch. An initial CTest regex matched zero cart tests; it was corrected to `cart[.].*` before the passing 24-test run.
- **Task 2 PAL buffer assumption:** The first mix/observer fixture run used the NTSC-sized 801-sample cap for a PAL frame. It now derives capacity from `AUDIO_CAPACITY` and the machine timing; the focused five-test command passes.
- **Task 3 IRQ fixture evidence:** The first trace showed the default CPU stack caused IRQ pushes to affect TIA-mapped addresses and the vector read as open bus. The ROM now initializes its stack before enabling timer traffic; the bus then read vector bytes `F0/CF` and fetched the handler at `CFF0`. The handler's original `INC $00` counter targeted TIA, so it now increments system RAM at `$1800` and the public frame test observes it.
- **Task 3 latch interleaving:** An instrumented hostile run showed IRQ entry between the mainline YM address store at `C923` and data store at `C928`; the handler's mode-register acknowledgement replaced the YM address latch, so pending data `$02` stopped Timer A. The fixture now keeps IRQs masked during the changing address/data traffic, then executes CLI and a cursor-derived JMP-only spin so the pending IRQ is tested without corrupting a partial register transaction. The status assertion was changed to check a future rearmed deadline because the handler correctly clears the flag. The final focused hostile run and registered lifecycle/hostile tests pass.

No verification remains unrun for this plan. The combined checks establish the digital profile and local lifecycle/mixer contracts; they do not establish physical XM accuracy or complete YM save-state support.

## User Setup Required

None. No owner action is needed for this plan.

## Next Phase Readiness

Plan 20-02 is complete and locally committed. Plan 20-03 may now atomically add the full format-2/behavior-7 YM continuation and replace the state-unavailable guard. Phase 20 remains in progress and unaccepted pending its remaining plans and gates.

---
*Phase: 20-complete-ym2151-homebrew-sound*
*Completed: 2026-11-12*

## Self-Check: PASSED

The summary exists and all three task commits are ancestors of the branch head. The later state bookkeeping records 2/7 completed plans, keeps Phase 20 in progress, and advances the next-plan pointer to 20-03.
