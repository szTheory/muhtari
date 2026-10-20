---
phase: 03-games-play-in-retroarch
plan: 06
subsystem: testing
tags: [diagnostic-rom, dasm, wasmtime, mailbox, crc16, maria-dma, riot-timer, ctest]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "MARIA's burst, line events and per-line statistics (03-01 to 03-04); the pinned 7800basic package and wasmtime under tests/built (03-05)"
provides:
  - "muhtari__peek: RAM, RIOT RAM and cartridge bytes as the CPU reads them, open bus elsewhere, with no side effect"
  - "tests/diag/lib.s: the mailbox at $2200, its CRC-16, a results screen in our own 16 hex glyphs, and the vectors"
  - "tests/diag/dma.s and the committed dma.a78: 27 DMA-cost records measured as INTIM on one line per frame"
  - "muhtari-test-diag ROM EXPECTED [STATS]: the mailbox reader and judge, with the per-line statistics cross-check"
  - "tests/diag/assemble.cmake: dasm -f3, 7800sign -w and 7800header under wasmtime, compared with the manifest"
  - "diag.dma, diag.rebuild (dma and Clark's two builds), diag.rebuild.self_test, diag.expected.self_test"
affects: [03-07, 03-08, 03-09, 03-13, 03-19]

actuals:
  tokens: 15800
  tasks: 3
  commits: 7
plan_head_before: 70b6461339451f8d2a35ffee86aac4695bc83c50
plan_head_after: fd4c35ef0cb6eca1a5cd2284a7cfa84f92b37229

tech-stack:
  added: []
  patterns:
    - "A diagnostic measures one DMA line per frame: DMA on only for VBE and for line 100, then INTIM read 10 cycles after a TIM1T write at line 100's t 0"
    - "Derive the value in docs/hardware/maria.md and commit expected.csv, then commit the source, then the .a78 with its manifest row pinned to the source commit"
    - "A record's test id is its row's position among the ROM's rows in expected.csv"

key-files:
  created:
    - tests/diag/lib.s
    - tests/diag/dma.s
    - tests/diag/dma.cfg
    - tests/diag/dma.a78
    - tests/diag/driver.c
    - tests/diag/assemble.cmake
    - tests/diag/tests.cmake
    - tests/diag/expected.csv
    - tests/diag/stats.csv
  modified:
    - src/bus.c
    - src/internal.h
    - tests/tests.cmake
    - tests/roms/manifest.txt
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/maria.md
    - docs/emulation-tests.md
    - docs/testing.md
    - .gitignore

key-decisions:
  - "The dma ROM times one burst per frame, on line 100, with DMA off on every other line after VBE; INTIM = 245 - ceil((15 + D) / 4) on an ordinary line and 245 - ceil((21 + D) / 4) on a last line, independent of the CPU's phase"
  - "Overrun rows are ranges (INTIM 137-138, cost 420-423) because the abort's lost cycles depend on where t 28 falls in the CPU's cycle; the first run gave 138, 137 and 138"
  - "stats.csv is rom,test,ordinary,last,objects; the driver checks the frame's first DMA line, and the objects column checks the overrun's 13 completed objects"
  - "H8 and H16 come from zone 0's DLL entry, as the core already models, not from CTRL"

patterns-established:
  - "Every diagnostic .s begins with an SPDX MIT line, includes lib.s after ORG $C000 and ends with LIB_TAIL"
  - "A gating expected value is committed before the ROM that reports it is assembled or run; git log shows the order"

requirements-completed: []

coverage:
  - id: D1
    description: "muhtari__peek returns RAM, RIOT RAM and cartridge bytes under the current decode and the open-bus byte elsewhere, changing no state; the driver reads the mailbox only through it"
    requirement: GAME-02
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^diag\\.dma$' --output-on-failure (exit 0)"
        status: pass
      - kind: other
        ref: "grep -n 'muhtari__peek' src/internal.h src/bus.c tests/diag/driver.c (17 lines)"
        status: pass
    human_judgment: false
  - id: D2
    description: "The dma ROM reports 27 records (empty DL, 4- and 5-byte headers, widths 1, 8 and 32, direct against indirect 1- and 2-byte, last line against ordinary, six terminator values, holes under H8 and H16 with 4- and 5-byte headers, and overrun at 11 to 16 objects), each equal to the value derived in docs/hardware/maria.md before the ROM first ran"
    requirement: GAME-02
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -L '^diag$' --output-on-failure (exit 0; 27 records PASS)"
        status: pass
      - kind: other
        ref: "E=037a578 (expected.csv added) is an ancestor of R=6283d3e (dma.a78 added): git merge-base --is-ancestor exit 0; b3e7013 (Task 2 values) precedes 6a4e8d5 and 1ae7f27"
        status: pass
    human_judgment: false
  - id: D3
    description: "Each DMA record is checked a second time against the core's per-line statistics on the frame that measured it: cost by line kind and objects completed, 26 rows of stats.csv"
    requirement: GAME-02
    verification:
      - kind: integration
        ref: "build/ci/tests/muhtari-test-diag tests/diag/dma.a78 tests/diag/expected.csv tests/diag/stats.csv (26 'stats agree', exit 0)"
        status: pass
    human_judgment: false
  - id: D4
    description: "The driver stops at status $00, fails as hang after 600 frames, checks the version and the CRC-16, and fails a missing, extra or wrong-valued record; a CSV with one changed value fails on that record"
    requirement: GAME-02
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^diag\\.expected\\.self_test$' (exit 0, matching 'dma null: 241 expected 242 .hw. id 1 flags 01 FAIL.*exited 1')"
        status: pass
    human_judgment: false
  - id: D5
    description: "diag.rebuild assembles dma.s and Bruce Clark's clark.s (as printed and as the 65C02 control) with the pinned dasm, 7800sign and 7800header under wasmtime and matches every manifest digest; one changed digest fails by name"
    requirement: GAME-02
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^diag\\.rebuild(\\.self_test)?$' --output-on-failure (exit 0)"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39354194917 (conclusion success on every leg, wasm32, be32 and both Windows legs included)"
        status: pass
    human_judgment: false
  - id: D6
    description: "dma.s and lib.s are our own MIT code with an SPDX first line, draw our own 16 hex glyphs, sit linear at $C000, and dma.a78 has a this-repo, MIT manifest row"
    requirement: GAME-02
    verification:
      - kind: other
        ref: "head -n 1 tests/diag/dma.s tests/diag/lib.s | grep -c 'SPDX-License-Identifier: MIT' = 2; grep -cE '^tests/diag/dma\\.a78[[:space:]]this-repo[[:space:]]' tests/roms/manifest.txt = 1; scripts/hygiene.sh --tree (exit 0)"
        status: pass
      - kind: other
        ref: "build/ci/runner/muhtari-run --frames 40 --dump-frame 40:dma40.png tests/diag/dma.a78 (exit 0); the image, viewed, shows rows 01-1B with their values and the CRC BE8F"
        status: pass
    human_judgment: false
  - id: D7
    description: "docs/emulation-tests.md has a diag section (mailbox, driver, CSV columns and source classes, derive-before-run, stats.csv, rebuilding a ROM); docs/testing.md lists the diag label and both self tests"
    requirement: GAME-02
    verification:
      - kind: other
        ref: "ctest --test-dir build/ci -R '^docs\\.' (exit 0)"
        status: pass
    human_judgment: false

duration: 26min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 06: The diagnostic pipeline and the dma ROM Summary

**A MIT 6502 diagnostic, built by the pinned dasm under wasmtime and committed with its source, measures 27 MARIA DMA costs as RIOT timer values through a RAM mailbox. Every one equals the value derived in `docs/hardware/maria.md` before the ROM first ran, and the core's per-line statistics agree with each.**

## Performance

- **Duration:** 26 min
- **Started:** 2026-10-20T01:10:26Z
- **Completed:** 2026-10-20T01:36:39Z
- **Tasks:** 3
- **Files modified:** 19

## Accomplishments

- **How dma measures.** In VBLANK the ROM points DPPH and DPPL at a test's DLL in ROM and turns DMA on for VBE. When MSTAT clears, it turns DMA off, counts lines with MARIA's WSYNC to line 99, then writes the test's CTRL and WSYNC once more. At line 100's t 0 it writes 255 to TIM1T, runs three NOPs and reads INTIM 10 cycles later.
- **The derivation.** The only burst in the frame freezes c2 for h = ceil((HALTRST − fall) / 4) cycles, so INTIM = 245 − h. That holds for every CPU phase, which the doc shows by listing the NOP cycle starts. All 27 values and 26 statistics rows are derived in the doc's "Diagnostic expected values" section.
- **First runs matched.** Task 1's single record and Task 2's 27 records all equalled their derivations on the first run. The overrun records came back as 138, 137 and 138, with statistics costs of 421, 423 and 421. That confirms the abort's lost cycles depend on the CPU's phase, as derived.
- **The results screen.** Frame 40, viewed, shows rows `01 00F1` to `1B 008A` in our own glyphs and the CRC `BE8F`.
- **Rebuild check.** `diag.rebuild` rebuilds `dma.a78` (`a89e6388...`) and both Clark builds byte for byte. This closes phase 2's pending "rebuild-and-compare joins the diagnostics lane".

## Commit order (expected values first, ROM run second)

| Step | Commit |
|---|---|
| Task 1 expected value (`dma,null,241,hw`) and its derivation | `037a578` |
| Task 1 ROM source | `1bc0c65` |
| Task 1 ROM bytes added, first run (`diag.dma`) | `6283d3e` |
| Task 2 expected values (27 rows) and stats.csv | `b3e7013` |
| Task 2 ROM source | `6a4e8d5` |
| Task 2 ROM bytes, first full run | `1ae7f27` |

## Task Commits

1. **Task 1: peek, mailbox library, one-record dma ROM, assemble, driver, one expected row:** `037a578`, `1bc0c65`, `6283d3e` (test)
2. **Task 2: the full dma matrix, on-screen hex, statistics cross-check:** `b3e7013`, `6a4e8d5`, `1ae7f27` (test)
3. **Task 3: diag.rebuild over the diagnostics and clark.s, both negative controls, docs:** `fd4c35e` (test)

## Verification

| Command | Exit |
|---|---|
| `cmake --workflow --preset ci` (503 tests) | 0 |
| `ctest --test-dir build/ci -R '^diag\.dma$' --output-on-failure` (Task 1) | 0 |
| `ctest --test-dir build/ci -L '^diag$' --output-on-failure` (Task 2) | 0 |
| `ctest --test-dir build/ci -R '^(diag\.\|docs\.)' --output-on-failure` (Task 3, 7 tests) | 0 |
| `scripts/hygiene.sh --tree` | 0 |
| `PATH=<llvm 21>:$PATH cmake --workflow --preset hygiene` (24 tests) | 0 |
| `gh run watch 39353599149`'s CI sibling, run 39353599146 on 6283d3e (tracer) | success |
| `gh run watch 39354194917 --exit-status` on fd4c35e | 0 |

Tracer gate (Task 1): `human_verify_mode` is end-of-phase and the verify step is automated only. The gate re-ran the verify: ci 0, `diag.dma` 0, hygiene 0. The tracer push's hosted run 39353599146 then passed on every leg before Task 2's results were committed.

## Files Created/Modified

- `src/bus.c`, `src/internal.h` - `muhtari__peek`
- `tests/diag/lib.s` - registers, mailbox, CRC-16, results screen, font, vectors
- `tests/diag/dma.s`, `dma.cfg`, `dma.a78` - the DMA cost diagnostic, its header answers and its bytes
- `tests/diag/driver.c` - muhtari-test-diag
- `tests/diag/assemble.cmake` - the rebuild-and-compare recipe, with `-DVARIANT=65c02` and `-DKEEP`
- `tests/diag/tests.cmake`, `tests/tests.cmake` - the four diag tests, included with the built tests
- `tests/diag/expected.csv`, `tests/diag/stats.csv` - 27 expected values and 26 statistics rows
- `tests/roms/manifest.txt`, `.gitignore` - dma.a78's row; tests/diag/*.a78 allowed
- `tests/inventory/ci.txt`, `wasm32.txt` - four diag lines
- `docs/hardware/maria.md`, `docs/emulation-tests.md`, `docs/testing.md` - the derivations, the diag section, the label and self-test rows

## Decisions Made

See `key-decisions`. One frame per test: the measured line's record is written in the frame that measured it, so the driver's statistics check needs no two-frame hold.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug in the plan's text] Holes come from the DLL entry, not CTRL**
- **Found during:** Task 2
- **Issue:** The plan said to set "CTRL's holey bits". H8 and H16 are bits 5 and 6 of a DLL entry's byte 0 (A7800-MARIA section 2.1), and the core already reads them there (03-01).
- **Fix:** The hole tests set H8 or H16 in zone 0's entry. The test names `hole8_*` and `hole16_*` are as planned, and both feature bits set.
- **Committed in:** `6a4e8d5`

**2. [Rule 3 - Blocking] `.gitignore` ignored every `.a78`**
- **Found during:** Task 1
- **Fix:** added `!/tests/diag/*.a78`, with the comment updated.
- **Committed in:** `6283d3e`

**3. [Rule 2 - Correctness] One timed line per frame, not a multi-line window or a two-frame hold**
- **Found during:** Task 1 design
- **Issue:** With DMA on every line, an overrun's burst runs past the next line's t 0. A WSYNC line count then skips lines, and a multi-line timer window holds a phase-dependent number of bursts.
- **Fix:** DMA is on only for VBE and line 100. The ROM writes each record in the frame that measured it, so the frame the driver checks is that frame.
- **Committed in:** `1bc0c65`, `6a4e8d5`

**4. [Rule 2 - Correctness] stats.csv has an objects column, and both CSVs accept ranges**
- **Issue:** #9 is "objects completed as objects are added". Only the statistics see objects, and the abort's cost depends on the CPU's phase.
- **Fix:** `rom,test,ordinary,last,objects`. A field may be `N`, `lo-hi` or `-`. The overrun rows require 13 objects and a cost of 420-423.
- **Committed in:** `b3e7013`

**5. [Plan detail] Test names and positional ids; no record macro**
- **Fix:** The plan's example row was `dma,1,...`, but its own acceptance greps `^dma,hole8`. So `expected.csv`'s test column holds names, and record n's id must be n + 1. dma computes its ids at run time, so `lib.s` has a `mb_record` subroutine rather than a macro.

**6. [Plan detail] KEEP writes without comparing**
- **Fix:** KEEP also updates an existing fixture, not only a new one. Each `.a78` change is committed after its source commit, with the manifest pin set to that source commit. Clark's rows follow the same convention.

**7. [Rule 2 - Docs] docs/testing.md**
- **Fix:** added the `diag` label row, the two self tests and the dma fixture. The planned `docs/emulation-tests.md` carries the full text.
- **Committed in:** `fd4c35e`

---

**Total deviations:** 7 (1 plan-text bug, 1 blocking, 3 correctness, 2 detail).
**Impact on plan:** None of the truths changed. Every gating value was still derived before its first run.

## Issues Encountered

- dasm's `>sym + n` binds as `(>sym) + n`. All four macros now parenthesise their arguments, and this was found in the listing before the first run.
- dasm treats `END` as a directive, so the terminator macro is `TERM`.
- At commit `b3e7013`, `diag.dma` fails: the 27 new rows are committed before the ROM that reports them. This is deliberate, and `1ae7f27` restores it.

## Known Stubs

None.

## Next Phase Readiness

- Later diagnostics (`cpu`, `dli`, `modes`, the board ROMs) reuse `lib.s`, `assemble.cmake` and the driver. Each adds a `.s` and `.cfg` pair, which `diag.rebuild` picks up by glob, plus its rows and a `diag.<rom>` test.
- Still open from 03-03: a DLI on zone 0's entry raises no NMI. A valid BIOS is copied but fast-boots until 03-13.

## Self-Check: PASSED
