---
phase: 03-games-play-in-retroarch
plan: 09
subsystem: testing
tags: [coverage, feature-mask, diagnostic-rom, dasm, read-modes, kangaroo, supergame]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "the sticky feature mask (03-01), the sample fixtures (03-05), the diagnostic pipeline and dma ROM (03-06), the dli ROM (03-07), the board ROMs and SuperGame boards (03-08)"
provides:
  - "tests/built/coverage.txt: one line per board and MARIA feature, each naming the ROMs that show it"
  - "muhtari-test-coverage: probes and runs each named ROM once for 600 fast-boot frames, judges boards from the probe and MARIA features from the core's mask, and fails a feature no line claims"
  - "basic.coverage and basic.coverage.self_test"
  - "tests/diag/modes (.s, .cfg, .a78) and diag.modes: 160B, 320B, 320D, kangaroo mode and 2-byte characters on a plain SuperGame board"
  - "assemble.cmake accepts the repository-relative paths docs/emulation-tests.md writes"
affects: [03-14, 03-19]

actuals:
  tokens: 6472
  tasks: 3
  commits: 7
plan_head_before: 7aef4c0334404f6acf0e5447bdd6be39397e78ed
plan_head_after: 1990dbf9cec7aae19109fced979879333aa8cda6

tech-stack:
  added: []
  patterns:
    - "A coverage claim is judged only from muhtari_probe and m->maria.features; a ROM's source or name never counts"
    - "A per-zone mode change: a DLI on each drawn zone's entry writes CTRL from a table, with an 8-line empty zone after each drawn zone so its last raster plays in its own mode"

key-files:
  created:
    - tests/built/coverage.c
    - tests/built/coverage.txt
    - tests/diag/modes.s
    - tests/diag/modes.cfg
    - tests/diag/modes.a78
  modified:
    - tests/built/tests.cmake
    - tests/diag/tests.cmake
    - tests/diag/expected.csv
    - tests/diag/assemble.cmake
    - tests/roms/manifest.txt
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/emulation-tests.md
    - docs/hardware/maria.md
    - docs/hardware/cartridge.md

key-decisions:
  - "modes is a plain SuperGame board of 4 banks, not linear: no sample or diagnostic was a SuperGame board without RAM or options, so board-supergame had no honest ROM; its at_4000 record reads bank 2, the second-to-last"
  - "The holey8 and holey16 bits follow the DLL entry's H8 and H16, so samples whose zones set them (simple, adventurer) show them too; each claim names diag/dma, whose records hit holes, and one sample"
  - "All 17 features in the driver's table are required; a feature with no line, or a line with no ROM, fails"

patterns-established:
  - "Scan before claiming: run muhtari-test-coverage on a scratch file that names every ROM under one feature and read each mask"

requirements-completed: [GAME-01]

coverage:
  - id: D1
    description: "basic.coverage checks 31 claims over the 17 required features against the probe and the core's sticky feature mask after 600 fast-boot frames; all pass"
    requirement: GAME-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^basic\\.coverage$' --output-on-failure (exit 0)"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39359891337 (CI on 1990dbf, conclusion success)"
        status: pass
    human_judgment: false
  - id: D2
    description: "A claim of kangaroo mode for 7800basic/simple, which never uses it, fails by name; a feature with no line fails as no ROM claims it"
    requirement: GAME-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^basic\\.coverage\\.self_test$' (exit 0)"
        status: pass
      - kind: other
        ref: "muhtari-test-coverage on coverage.txt without its kangaroo line and with a bare rm2 line (exit 1, kangaroo FAIL no ROM claims it, rm2 FAIL no ROM on line 21)"
        status: pass
    human_judgment: false
  - id: D3
    description: "The modes ROM draws 160B, 320B, 320D, kangaroo mode and 2-byte characters on a plain SuperGame board, MIT with its source, cfg and manifest row; both records equal the rows derived before its first run (at_4000 2, zones 5)"
    requirement: GAME-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^(diag\\.modes|diag\\.rebuild)$' (exit 0)"
        status: pass
      - kind: other
        ref: "git merge-base --is-ancestor 4a2e6d2 a29d78b (exit 0): the rows precede the ROM bytes; frame 120 viewed as a PNG shows the five zones"
        status: pass
    human_judgment: false
  - id: D4
    description: "docs/emulation-tests.md documents coverage: the feature names, probe versus mask, how to add a claim and the negative control; maria.md derives the modes values"
    requirement: GAME-01
    verification:
      - kind: other
        ref: "ctest --test-dir build/ci -R '^docs\\.' (exit 0); grep -n 'coverage.txt' docs/emulation-tests.md (1 line)"
        status: pass
    human_judgment: false

duration: 14min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 09: Feature coverage Summary

**`basic.coverage` now checks GAME-01's whole list against what the probe and the core's feature mask report: all 17 board and MARIA features, 31 claims. The new `modes` ROM covers what no sample draws, 160B, 320B, 320D, kangaroo mode and the plain SuperGame board. Both of its values matched their derivations on the first run.**

## Performance

- **Duration:** 14 min
- **Started:** 2026-10-20T02:25:32Z
- **Completed:** 2026-10-20T02:39:57Z
- **Tasks:** 3
- **Files modified:** 15

## Accomplishments

- `muhtari-test-coverage COVERAGE BUILT_DIR DIAG_DIR` loads each ROM it names once, probes it and runs it for 600 fast-boot frames. It then judges board claims from `muhtari_probe` and MARIA claims from `m->maria.features`, printing one line per claim with the board, sizes and mask it saw.
- `coverage.txt` names one or two ROMs for each feature: samples where the masks showed one, and `diag/modes`, `dma`, `dli`, `linear16`, `bank6` and `sg144` where no sample does.
- The scan across all 34 samples confirmed the gaps D-14 predicted. No sample uses read mode 2 or kangaroo mode. No sample is a 16K linear, bank-6, 144K or plain SuperGame board.
- The `modes` ROM fills those gaps. Its mask is `071b`: RM0, RM2, write mode, kangaroo, 2-byte characters and DLI. The probe reports SuperGame, options 0, 64 KiB, no RAM.
- Hosted CI run 39359891337 passed on every leg on 1990dbf (be32 2 min, wasm32 under 1 min).

## Task Commits

1. **Task 1: a coverage claim is checked end to end** - `48ac5f2` (test)
2. **Task 2: the modes ROM and the full coverage file** - `4a2e6d2` (derivation), `b9d205e` (source), `a29d78b` (bytes and diag.modes), `bb17634` (coverage file, required check, self test)
3. **Task 3: document coverage** - `db63e36` (docs)

Deviation fix: `1990dbf` (assemble.cmake relative paths).

## Files Created/Modified

- `tests/built/coverage.c`: the coverage driver
- `tests/built/coverage.txt`: the 17 feature lines
- `tests/built/tests.cmake`: `basic.coverage` and its self test, with the claimed samples' fixtures
- `tests/diag/modes.s`, `modes.cfg`, `modes.a78`: the read-mode diagnostic
- `tests/diag/expected.csv`: `modes,at_4000,2` and `modes,zones,5`
- `tests/diag/tests.cmake`: `diag.modes`
- `tests/diag/assemble.cmake`: path arguments made absolute
- `tests/roms/manifest.txt`: the `modes.a78` row, pinned to source commit b9d205e
- `tests/inventory/ci.txt`, `wasm32.txt`: three new tests
- `docs/hardware/maria.md`: "The modes ROM" derivation and table row
- `docs/hardware/cartridge.md`: the modes board noted under The board ROMs
- `docs/emulation-tests.md`: the coverage subsection and the modes rows

## Decisions Made

- `modes` is a plain SuperGame board of 4 banks. Its `at_4000` record shows the second-to-last bank, so the plain board's own layout is exercised and not just reported by the probe.
- Every object in `modes` has a 5-byte header, so each one sets its own write mode. The write mode latch persists across zones.
- The required features are the driver's whole table, so adding a feature to the table requires a line for it.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 2 - Missing critical] modes is a plain SuperGame board, with a second record**
- **Found during:** Task 2 (the mask scan)
- **Issue:** The plan put `modes` on a linear board at $C000 and expected some sample to cover `board-supergame`. Every SuperGame sample has 16K of RAM, and the bank6 and sg144 ROMs have options, so no ROM was a plain SuperGame board.
- **Fix:** `modes` is a 4-bank, 64 KiB plain SuperGame board. It records `at_4000` (bank 2, derived in maria.md and cartridge.md) before `zones`. The diagnostic count stays 7, as the acceptance check requires.
- **Files modified:** tests/diag/modes.s, modes.cfg, expected.csv, docs/hardware/maria.md, docs/hardware/cartridge.md
- **Verification:** `diag.modes` passes with both records on the first run, and `board-supergame diag/modes` passes
- **Committed in:** 4a2e6d2, b9d205e, a29d78b

**2. [Rule 1 - Bug] coverage.c was not clang-formatted**
- **Found during:** Task 2 (the hygiene preset)
- **Issue:** Task 1's commit carried one line clang-format 21 wraps differently. `hygiene.format` had passed before that commit only because the file was still untracked.
- **Fix:** Ran clang-format 21 on the file.
- **Files modified:** tests/built/coverage.c
- **Verification:** `cmake --workflow --preset hygiene`, 24 of 24 tests pass
- **Committed in:** bb17634

**3. [Rule 1 - Bug] The documented rebuild command failed with relative paths**
- **Found during:** Task 2 (assembling modes with KEEP)
- **Issue:** `docs/emulation-tests.md` gives `-DSRC=tests/diag/<rom>.s` and the other paths relative to the repository. assemble.cmake's `file(RELATIVE_PATH)` and its tool stages, which run in the build directory, rejected them.
- **Fix:** The script makes WASMTIME, PKG, SRC, CFG, OUT, MANIFEST and KEEP absolute from the working directory.
- **Files modified:** tests/diag/assemble.cmake
- **Verification:** The documented command rebuilds modes.a78 to its manifest digest (exit 0), with and without KEEP. `diag.rebuild` and `diag.rebuild.self_test` still pass.
- **Committed in:** 1990dbf

---

**Total deviations:** 3 auto-fixed (1 missing critical, 2 bugs)
**Impact on plan:** The first fix closed a coverage hole the plan did not foresee. The other two were small tooling fixes. Nothing outside test code and docs changed, so no pin moved.

## Issues Encountered

- The holey8 and holey16 bits follow the zone's DLL bits, not whether a hole was hit. The plan said no sample uses holey DMA, but `simple` shows H8 and `adventurer` shows H16. Both lines therefore name `diag/dma`, whose hole records hit real holes, and one sample.

## User Setup Required

None. No external service configuration is required.

## Next Phase Readiness

- Plan 03-14 pins the modes picture through its ledger row. Frame 120 was viewed and shows the five zones.
- No frame or title pin moved, and the behaviour revision stays 1.

---
*Phase: 03-games-play-in-retroarch*
*Completed: 2026-10-20*

## Self-Check: PASSED
