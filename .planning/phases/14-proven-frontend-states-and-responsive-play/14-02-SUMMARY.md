---
phase: 14-proven-frontend-states-and-responsive-play
plan: "02"
subsystem: testing
tags: [atari-7800, c, diagnostics, libretro, save-state, rewind]

requires:
  - phase: 14-proven-frontend-states-and-responsive-play
    provides: Owned progress fixture and public-library diagnostic driver
  - phase: 13-portable-save-load-across-three-interfaces
    provides: Shared state API and loaded-module state contract
provides:
  - Independent progress-glyph, full-picture, and raw-save fixture judgments
  - Repeated loaded-module restore/replay and same-instance speculation checks
affects: [14-03, 14-04, 14-05]

actuals:
  tokens: 6170
  tasks: 2
  commits: 5
plan_head_before: ea3fc7881ddd9628d7c35cd13396b02b409854d5
plan_head_after: 8587ce054f00549a9049882cd4ca39900e528835

tech-stack:
  added: []
  patterns: [Independent fixture oracles, repeated loaded-module state replay]

key-files:
  created: []
  modified:
    - tests/diag/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/libretro/host.c
    - tests/retroarch/compare.c

key-decisions:
  - "Decode the visible hexadecimal progress glyph before judging full-picture equality."
  - "Register portable diagnostic tests when either the libretro target or a cross-compiling emulator consumes them."
  - "Keep FRONT-02, FRONT-03, and FRONT-04 pending until the later source-bound host receipt exists."

requirements-completed: []
coverage:
  - id: D1
    description: Local fixture checks independently reject wrong progress glyphs, picture pixels, and incomplete or extra save bytes; this does not establish pinned-host save/load.
    requirement: FRONT-02
    verification:
      - kind: integration
        ref: "ctest --preset ci -R '^diag.frontend_states$' --no-tests=error --output-on-failure"
        status: pass
    human_judgment: false
  - id: D2
    description: The loaded-module consumer checks three backward restores followed by replay across input, native video, PCM, sample counts, and exact NTSC timing.
    requirement: FRONT-03
    verification:
      - kind: integration
        ref: "ctest --preset ci -R '^libretro\\.host\\.state$' --no-tests=error --output-on-failure"
        status: pass
    human_judgment: false
  - id: D3
    description: The same-instance path restores canonical progress and raw battery state while preserving the SRAM span, instance isolation, and changed, equal, and refused generation behavior.
    requirement: FRONT-04
    verification:
      - kind: integration
        ref: "ctest --preset ci -R '^libretro\\.host\\.state$' --no-tests=error --output-on-failure"
        status: pass
    human_judgment: false

duration: 19min
completed: 2026-11-03
status: complete
---

# Phase 14 Plan 02: Proven Frontend States and Responsive Play Summary

**The owned fixture now has independent progress, picture, and complete HSC-save judgments, while the loaded-module test replays three restored branches and checks same-instance battery recovery.**

## Performance

- **Duration:** 19 minutes
- **Started:** 2026-11-03T04:19:05Z
- **Completed:** 2026-11-03T04:37:59Z
- **Tasks:** 2
- **Files modified:** 5

## Accomplishments

- Added a progress comparator that decodes each counter glyph before whole-image equality, with controls for an equal but wrong glyph, a changed expected counter, and an unrelated pixel. Corrected the fixture's B glyph to `{6,5,6,5,6}`.
- Registered `diag.frontend_states` in the native and WASM inventories. Its local test checks progress snapshots, full pictures, HSC values after save/restore/replay, and missing, truncated, trailing, and speculative media faults.
- Extended `libretro.host.state` with three successive backward restore/replay paths. It compares inputs, native frames, PCM, audio counts, and exact timing, while checking stable SRAM identity and changed, equal, and refused battery generations.
- Kept the serializer unchanged. No RetroArch launch or CI dispatch occurred; these checks establish local behavior only.

## Task Commits

1. **Task 1 RED: expose the progress comparator glyph defect** — `14e30e7` (`test`)
2. **Task 1 GREEN: add independent progress and picture judgment** — `ced6324` (`feat`)
3. **Task 2 RED: cover repeated loaded-state restores** — `7a343e1` (`test`)
4. **Task 2 GREEN: verify speculative state restoration** — `76eff3c` (`feat`)
5. **Task 2 replay coverage: replay after the third loaded-state rewind** — `8587ce0` (`test`)

## TDD Gate Compliance

- Task 1's named diagnostic test failed at the expected incorrect glyph/pixel judgment; the RED evidence classifier returned `RED_EVIDENCE_OK`. The corrected comparator and registered diagnostic passed.
- Task 2's named loaded-module test failed only at the deliberate comparison between restored module SRAM and the still-speculative independent reference. The RED evidence classifier returned `RED_EVIDENCE_OK`. Restoring both instances before comparison made the targeted checks pass.
- The task 2 RED record is retained under ignored build output at `build/ci/tdd-red/14-02-task2.json`; generated reports and fixtures are not committed.

## Files Created/Modified

- `tests/retroarch/compare.c` — independent counter decoding and falsifiable progress controls.
- `tests/diag/tests.cmake` — recurring fixture progress, image, and raw-media test registration and faults.
- `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt` — inventory entries for the portable diagnostic test.
- `tests/libretro/host.c` — loaded-module restore/replay, timing, SRAM, and battery-generation assertions.

## Decisions Made

- The fixture oracle judges progress glyphs before complete-picture equality so two identical but wrong counter images cannot pass.
- The WASM diagnostic registration follows the existing cross-compiling emulator path even though that preset disables the libretro target.
- Local callback evidence remains separate from the pinned RetroArch observations required to complete the phase requirements.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking issue] Register the diagnostic for the WASM consumer**
- **Found during:** Task 2 inventory review
- **Issue:** The new diagnostic registration was guarded only by `TARGET muhtari_libretro`, but the WASM preset disables that target while running diagnostics through `CMAKE_CROSSCOMPILING_EMULATOR`.
- **Fix:** Register when either the libretro target or cross-compiling emulator is available, and include the fixture test in the WASM inventory.
- **Files modified:** `tests/diag/tests.cmake`, `tests/inventory/wasm32.txt`
- **Verification:** Native CI preset registration and diagnostic test passed. Local WASM configuration was attempted but cannot configure because `clang-21` is absent.
- **Committed in:** `7a343e1`

**Total deviations:** 1 auto-fixed (Rule 3)
**Impact on plan:** The diagnostic is registered for both required consumers; no unrelated behavior or production code changed.

## Issues Encountered

- `cmake --preset wasm32` could not configure because this checkout environment has no `clang-21` compiler. The plan's native CI preset checks passed; no CI dispatch was attempted.
- The best-effort broken-windows entry for the documented deviation was rejected because existing ledger entry counts disagree with its frontmatter; the ledger was left unchanged.

## User Setup Required

None.

## Next Phase Readiness

Local fixture and loaded-module evidence is ready for the remaining Phase 14 plans. FRONT-02/03/04 remain pending until plan 14-05 records the source-bound pinned-host receipt; Phase 12 remains untouched.

## Self-Check: PASSED

The summary and all five task commits exist; every task commit is an ancestor of the current plan head. The changed files contain no unresolved placeholder or stub patterns.

---
*Phase: 14-proven-frontend-states-and-responsive-play*
*Completed: 2026-11-03*
