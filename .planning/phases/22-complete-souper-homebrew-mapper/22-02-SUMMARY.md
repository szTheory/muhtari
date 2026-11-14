---
phase: 22-complete-souper-homebrew-mapper
plan: "02"
subsystem: cartridge
tags: [souper, maria, libretro, diagnostics, deterministic-testing]
requires:
  - phase: 22-complete-souper-homebrew-mapper
    provides: Exact Souper image profile and reset/EXRAM policy from plan 22-01
provides:
  - Independent CPU and MARIA Souper maps, including character remapping and HALT-sensitive fetch trapping
  - Literal CPU/MARIA boundary matrix, real-fetch pixel oracle and wrong-view/page/selector controls
  - Stable-only Souper memory descriptors in the loaded libretro module
affects: [phase-22-03, cartridge, bus, maria, runner, libretro]
actuals:
  tokens: 11991
  tasks: 2
  commits: 4
commits: 4
plan_head_before: 4b6a94fcd60cf31e9a9d48e1c6728b40cec226a1
plan_head_after: db6feb0feb173fe5536e8129f33a48be546023a8
tech-stack:
  added: []
  patterns:
    - Existing tick-aware cart dispatch selects CPU and MARIA views independently
    - Literal mapper and fetch expectations are authored separately from production address arithmetic
key-files:
  created: []
  modified:
    - src/cart.c
    - src/internal.h
    - tests/diag/driver.c
    - tests/diag/expected.csv
    - tests/bus/test_bus.c
    - tests/cmake/hashes.cmake
    - libretro/libretro.c
    - tests/libretro/host.c
    - docs/hardware/cartridge.md
    - docs/libretro.md
key-decisions:
  - "Keep CPU bank, EXRAM page, character selectors and MARIA mode bits in independent bounded latches."
  - "Publish only the fixed $4000-$5FFF and $C000-$FFFF Souper CPU windows; dynamic windows remain holes."
  - "Keep the claim limited to the selected software model; physical or retail compatibility is not established."
patterns-established:
  - "Mapper tests use literal address/value rows and separate wrong-view/page/selector controls."
  - "Fetch-trap coverage exercises the existing HALT-selected reader immediately before and after the switch."
requirements-completed: [SOUPER-02, SOUPER-03, SOUPER-05, SOUPER-06, SOUPER-07]
coverage:
  - id: D1
    description: CPU bank selection, register aliases and flat/segmented EXRAM match independent boundary values.
    requirement: SOUPER-02
    verification:
      - kind: unit
        ref: "core.souper.cpu; diag.souper_tracer"
        status: pass
    human_judgment: false
  - id: D2
    description: MARIA character selection and trapped EXRAM view switch at the tested HALT reader boundary.
    requirement: SOUPER-03
    verification:
      - kind: unit
        ref: "bus.souper.halt_views"
        status: pass
      - kind: e2e
        ref: "diag.souper_tracer; libretro.observe.souper"
        status: pass
    human_judgment: false
  - id: D3
    description: Literal numeric and pixel controls reject wrong selectors, EXRAM pages, CPU-view graphics and malformed observations.
    requirement: SOUPER-05
    verification:
      - kind: e2e
        ref: "diag.souper_tracer; runner.hashes_file"
        status: pass
    human_judgment: false
  - id: D4
    description: The production libretro module exposes only stable Souper CPU windows across legal mode changes.
    requirement: SOUPER-06
    verification:
      - kind: integration
        ref: "libretro.host.souper; libretro.observe.souper"
        status: pass
    human_judgment: false
  - id: D5
    description: Existing documentation states the selected software mapping model and its confidence limits.
    requirement: SOUPER-07
    verification:
      - kind: other
        ref: "docs.budgets; policy.claims_wording"
        status: pass
    human_judgment: false
duration: 77min
completed: 2026-11-13
status: complete
---

# Phase 22 Plan 02: Complete Souper CPU/MARIA Maps Summary

**Souper CPU banking, EXRAM segmentation, MARIA character remapping and HALT-sensitive fetch trapping now have independent literal checks and stable libretro descriptors.**

## Performance

- **Duration:** 77 minutes
- **Started:** 2026-11-14T02:39:25Z
- **Completed:** 2026-11-14T03:56:21Z
- **Tasks:** 2
- **Files modified:** 24

## Accomplishments

- Completed the selected CPU map with 32 switchable 16 KiB banks, a fixed final bank, independent flat/segmented EXRAM controls, page selectors and documented write aliases. Undefined register/audio behavior remains inert.
- Completed MARIA's character and fetch-trap views through the existing tick-aware bus path. The diagnostic exercises real fetches and separately judges CPU mailbox values, native pixels, runner output and the production libretro module.
- Added a literal 26-row CPU/MARIA boundary matrix, distinct EXRAM/character tags, wrong-view/page/selector controls, stable-only module memory descriptors, explicit hash identity rows and documentation of the software-model limits.

## Task Commits

1. **Task 1: Complete CPU aliases and EXRAM observations** — `145ed14` (RED tests), `0a04646` (corrected literal expectation), `cf5e648` (implementation).
2. **Task 2: Complete MARIA views, HALT transition and descriptors** — `db6feb0`.

## Files Created/Modified

- `src/cart.c`, `src/internal.h` — independent CPU/MARIA register state and reader-specific mapping.
- `tests/diag/driver.c`, `tests/diag/souper.s`, `tests/diag/expected.csv`, `tests/diag/souper.a78` — owned stimulus, literal expectations and independent fault controls.
- `tests/bus/test_bus.c`, `tests/core/test_core.c` — CPU map and HALT-transition checks.
- `libretro/libretro.c`, `tests/libretro/host.c`, `tests/libretro/observe.c` — stable descriptors and loaded-module observations.
- `tests/cmake/hashes.cmake`, `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt`, `tests/runner/tests.cmake`, `tests/libretro/tests.cmake`, `tests/cmake/info_claims.cmake` — fixed hash identities and test registration/claims.
- `docs/hardware/cartridge.md`, `docs/libretro.md` — mapper model, reset and evidence limits.

## Decisions Made

The implementation follows the exact mapper software model selected in plan 22-01. CPU bank state, EXRAM pages, character selectors and MARIA mode bits remain independent. Libretro advertises the fixed CPU windows only; all windows that change with mapper controls stay unadvertised. This plan does not establish physical-cartridge or retail-software compatibility, and the state-unavailable guard remains for plan 22-03.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Test bug] Keep the pixel negative control from changing unrelated frame hashes**
- **Found during:** Task 2
- **Issue:** The test restored the injected CPU-view fault by writing the graphics color into all 12 snapshots, changing the first four frames that the oracle did not judge.
- **Fix:** Limit fault injection and restoration to frames 5–12, which the pixel oracle checks.
- **Files modified:** `tests/diag/driver.c`
- **Verification:** `diag.souper_tracer` and the full 720-test CI suite passed.
- **Committed in:** `db6feb0`

**2. [Rule 3 - Blocking test issue] Update the descriptor claim to the renamed host test**
- **Found during:** Overall CI
- **Issue:** Renaming the loaded-module memory-map test left `info_claims.cmake` pointing at the removed test name.
- **Fix:** Point the capability claim at `libretro.host.souper`, which exercises the same descriptors plus the Souper mode changes.
- **Files modified:** `tests/cmake/info_claims.cmake`
- **Verification:** `libretro.info_claims` and the full 720-test CI suite passed.
- **Committed in:** `db6feb0`

**Total deviations:** 2 auto-fixed issues (one test-oracle correction, one test-registration correction).
**Impact on plan:** Both fixes were required for trustworthy integration verification; the mapper model and scope stayed unchanged.

## Issues Encountered

The first test-only RED attempt used an incorrect literal address expectation; it was corrected before the implementation commit, and the corrected failing assertions were used as the TDD evidence. The initial high-concurrency full run hit short timeouts in unrelated tests and exposed the stale descriptor claim. The timed-out cases passed in isolation, and the complete suite passed with one worker.

## User Setup Required

None.

## Next Phase Readiness

Plan 22-03 remains. It owns complete Souper lifecycle and snapshot continuation; this plan intentionally leaves the state-unavailable guard intact. Phase 22 acceptance remains pending.

---
*Phase: 22-complete-souper-homebrew-mapper*
*Completed: 2026-11-13*

## Self-Check: PASSED

- Summary file exists and the four task commits are ancestors of the plan head.
- Measured task commit count is 4, matching `actuals.commits`.
- Coverage classifier reports all five coverage entries automatically verified, with no errors.
- The task commits contain no tracked file deletions.
