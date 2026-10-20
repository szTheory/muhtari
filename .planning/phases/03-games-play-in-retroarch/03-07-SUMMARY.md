---
phase: 03-games-play-in-retroarch
plan: 07
subsystem: testing
tags: [diagnostic-rom, dasm, riot-timer, wsync, rdy, dli, nmi, mstat, slow-cycles]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "the diagnostic pipeline: lib.s, assemble.cmake, muhtari-test-diag, expected.csv and stats.csv (03-06); the DLI and line events (03-03)"
provides:
  - "tests/diag/cpu.s and cpu.a78: INTIM after 64 absolute loads from the TIA, RIOT RAM, RIOT and RAM, ended by MARIA's WSYNC"
  - "tests/diag/dli.s and dli.a78: the DLI's NMI on zones 0, 1 and 2 and parked in WSYNC, two WSYNC spans, a store right after a WSYNC, and both MSTAT edges by line and poll"
  - "a DLI on zone 0's DLL entry raises NMI on the second phi1 after VBE's end, t 50 of line 16"
  - "muhtari__bus_phi1_after and the bus's cycle_len: SALLY's phi1s from the cycle in flight"
  - "lib.s: LIB_TAIL takes a diagnostic's diag_nmi as its NMI vector"
  - "diag.cpu, diag.dli, frame.dli_first_zone"
affects: [03-08, 03-09, 03-13, 03-19]

actuals:
  tokens: 7695
  tasks: 3
  commits: 9
plan_head_before: 11d16532b07441f053db1a2283dff3c32e8ca7c1
plan_head_after: 9161753e7a680c122025b0e60456fb37de540f36

tech-stack:
  added: []
  patterns:
    - "A timed run starts the timer in the first instruction after a WSYNC releases; a later line event's cycle index is the same for every CPU phase when the slow cycles before it and the lines to it have the same parity"
    - "A slow access is invisible to INTIM alone, which counts phi2; ending the run with MARIA's WSYNC turns its extra ticks into fewer held cycles"
    - "A diagnostic that needs an NMI handler defines diag_nmi; the handler restores a saved stack pointer and returns from the test's subroutine"

key-files:
  created:
    - tests/diag/cpu.s
    - tests/diag/cpu.cfg
    - tests/diag/cpu.a78
    - tests/diag/dli.s
    - tests/diag/dli.cfg
    - tests/diag/dli.a78
  modified:
    - src/bus.c
    - src/internal.h
    - src/maria.c
    - tests/diag/lib.s
    - tests/diag/tests.cmake
    - tests/diag/expected.csv
    - tests/diag/stats.csv
    - tests/frame/test_frame.c
    - tests/frame/tests.cmake
    - tests/roms/manifest.txt
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/maria.md
    - docs/hardware/bus.md
    - docs/emulation-tests.md
    - docs/testing.md
    - .planning/WINDOWS.md

key-decisions:
  - "The cpu ROM ends each 64-load run with MARIA's WSYNC, so INTIM reads 205 after slow loads and 173 after RAM loads, 32 cycles apart, for every CPU phase"
  - "A DLI on zone 0's entry raises NMI on the second phi1 after VBE's end, t 50 of line 16, the burst's rule with VBE's end for HALTRST; the phi1s follow the cycle in flight, 4 or 6 ticks, then every 4"
  - "The dli ROM's NOP sled puts each opcode fetch on an odd cycle, which makes the first-zone DLI exact (233) and separates MAME's NMI at the burst's start (119) from the fitted rule (117)"
  - "parked's expected value is 16, not the first derivation's 20: the derivation left out line 18's burst, which freezes the NMI handler's opcode fetch 4 cycles"

patterns-established:
  - "Each fitted dli row names its test in docs/hardware/maria.md's fitted list, with the value hardware would have to show to flip it"
  - "A timed poll or loop branch is guarded at assembly time against a page crossing"

requirements-completed: [GAME-02]

coverage:
  - id: D1
    description: "The cpu ROM reports INTIM after 64 absolute loads from $0000, $0480, $0280 and $1800 with DMA off, ended by MARIA's WSYNC; each equals the value derived from 4- and 6-tick cycles before the ROM first ran: 205, 205, 205 and 173"
    requirement: GAME-02
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^diag\\.(cpu|rebuild)$' --output-on-failure (exit 0)"
        status: pass
      - kind: other
        ref: "git merge-base --is-ancestor 3eb7aa3 6ec5939 (exit 0): the cpu rows precede cpu.a78"
        status: pass
    human_judgment: false
  - id: D2
    description: "The dli ROM reports the DLI's NMI on zones 1 and 2 (117), on zone 0 (233) and with the CPU parked in WSYNC (16), WSYNC spans of one and three lines (142 and 171), a store right after a WSYNC (135) and MSTAT's set and clear edges by line (243, 21) and poll (2, 2); every record equals its row, and three DLI records also match stats.csv"
    requirement: GAME-02
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -L '^(diag|frame|maria)$' --output-on-failure (exit 0, 27 tests)"
        status: pass
      - kind: other
        ref: "git merge-base --is-ancestor 8b88408 d8751e5 (exit 0): the dli rows precede dli.a78"
        status: pass
    human_judgment: false
  - id: D3
    description: "A DLI on zone 0's DLL entry raises NMI after VBE on line 16; the handler runs once and the feature mask records DLI and DLI_QUEUED"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "tests/frame/test_frame.c#dli_first_zone (ctest -R '^frame\\.dli_first_zone$', exit 0)"
        status: pass
      - kind: integration
        ref: "diag.dli record zone0 = 233 (rtl)"
        status: pass
    human_judgment: false
  - id: D4
    description: "Eight dli rows are rtl or fit, and each names its test in docs/hardware/maria.md's fitted list with its flip value: DLI to NMI, first-zone DLI, RDY after a write and MSTAT's edges"
    requirement: GAME-02
    verification:
      - kind: other
        ref: "grep -cE '^dli,.*,(rtl|fit)$' tests/diag/expected.csv = 8, each test name found below '## Fitted values'"
        status: pass
    human_judgment: false
  - id: D5
    description: "cpu.a78 and dli.a78 are MIT, committed with sources and this-repo manifest rows; diag.rebuild rebuilds them, dma.a78 and Clark's builds byte for byte on every leg"
    requirement: GAME-02
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^diag\\.rebuild$' (exit 0); scripts/hygiene.sh --tree (exit 0)"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39356734460 (CI on 9161753, conclusion success)"
        status: pass
    human_judgment: false
  - id: D6
    description: "docs/hardware/maria.md indexes the three ROMs with their mailbox ids, records and derivations, and says what is not measured here; docs/emulation-tests.md lists the three ROMs and diag.cpu and diag.dli"
    requirement: GAME-02
    verification:
      - kind: other
        ref: "ctest --test-dir build/ci -R '^docs\\.' --output-on-failure (exit 0); grep -n 'Not measured here' docs/hardware/maria.md (1 line); grep -c 'cpu\\.a78\\|dli\\.a78\\|dma\\.a78' docs/emulation-tests.md (4)"
        status: pass
    human_judgment: false

duration: 23min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 07: The cpu and dli diagnostics Summary

**Two more MIT diagnostics, built by the pinned dasm, time a slow access, the DLI's NMI, MARIA's WSYNC, a store right after a WSYNC and both MSTAT edges through the RIOT timer. All 15 records equal values derived in `docs/hardware/maria.md`, 14 of them on the first run. A DLI on zone 0's DLL entry now raises an NMI after VBE.**

## Performance

- **Duration:** 23 min
- **Started:** 2026-10-20T01:39:19Z
- **Completed:** 2026-10-20T02:02:42Z
- **Tasks:** 3
- **Files modified:** 23

## Accomplishments

- **cpu.** INTIM alone cannot see a slow cycle: the timer counts φ2, and a 6-tick cycle counts once. So each run of 64 `lda.w` ends with MARIA's WSYNC, and the slow loads' extra 128 ticks come back as 32 fewer held cycles. The derived values were 205 for the TIA, RIOT RAM and RIOT and 173 for RAM, the same for every CPU phase, and the first run read exactly those.
- **The phase rule.** After a WSYNC release, a later line's t 0 or t 28 lands on the same cycle index for every phase φ when the slow cycles before it and the lines to it have the same parity. Every dli test is built to that rule. The one-line WSYNC span gives $8E (142), the figure the "Improving the emulators" thread measured.
- **dli.** zone1 117, zone2 117, zone0 233, parked 16, wsync_1 142, wsync_3 171, rdy_store 135, set_lines 243, clear_lines 21, set_polls 2, clear_polls 2. Ten matched the committed derivation on the first run. `parked` did not, and the derivation was wrong, not the core (see Deviations).
- **First-zone DLI.** VBE reads zone 0's entry with no HALT window. A DLI there now raises an NMI on the second φ1 after VBE's end, at t 50. `frame.dli_first_zone` checks it, and the broken-windows item for it is fixed.
- **Fitted rows.** Eight dli rows are `rtl` or `fit`. The fitted list now names each one's test and the hardware reading that would flip it.

## Commit order (expected values first, ROM run second)

| Step | Commit |
|---|---|
| cpu rows and derivation | `3eb7aa3` |
| cpu ROM source | `c35b8c1` |
| cpu ROM bytes, first run, `diag.cpu` | `6ec5939` |
| dli rows, stats rows and derivation | `8b88408` |
| first-zone DLI in the core | `11fc3e7` |
| dli ROM source, `diag_nmi` hook | `04a6e2d` |
| parked derivation corrected after the first run | `87005ba` |
| dli ROM bytes, `diag.dli` | `d8751e5` |

## Task Commits

1. **Task 1: the slow-access cost, derive, assemble and judge the cpu ROM:** `3eb7aa3`, `c35b8c1`, `6ec5939` (test)
2. **Task 2: the dli ROM:** `8b88408`, `11fc3e7` (fix), `04a6e2d`, `87005ba`, `d8751e5` (test)
3. **Task 3: what was not built, and the diag docs:** `9161753` (docs)

## Verification

| Command | Exit |
|---|---|
| `cmake --workflow --preset ci` (506 tests) | 0 |
| `ctest --test-dir build/ci -R '^diag\.(cpu\|rebuild)$' --output-on-failure` (Task 1) | 0 |
| `ctest --test-dir build/ci -L '^(diag\|frame\|maria)$' --output-on-failure` (Task 2, 27 tests) | 0 |
| `ctest --test-dir build/ci -R '^docs\.' --output-on-failure` (Task 3) | 0 |
| `scripts/hygiene.sh --tree` (each task) | 0 |
| `gh run view 39355059355` (CI on 6ec5939, the tracer push) | success |
| `gh run watch 39356734460 --exit-status` (CI on 9161753, every leg) | 0 |

Tracer gate (Task 1): `human_verify_mode` is end-of-phase, and the verify step is automated only. The gate re-ran the verify: ci 0, `diag.cpu` and `diag.rebuild` 0, hygiene 0. The tracer push's hosted run 39355059355 passed before Task 2 began.

## Files Created/Modified

- `tests/diag/cpu.s`, `cpu.cfg`, `cpu.a78` - the slow-access diagnostic
- `tests/diag/dli.s`, `dli.cfg`, `dli.a78` - the DLI, WSYNC, RDY and MSTAT diagnostic
- `tests/diag/lib.s` - `diag_nmi` as the NMI vector when a diagnostic defines it; dma.a78 and cpu.a78 rebuild unchanged
- `src/maria.c`, `src/bus.c`, `src/internal.h` - the first-zone DLI, `muhtari__bus_phi1_after`, `cycle_len`
- `tests/frame/test_frame.c`, `tests/frame/tests.cmake` - `frame.dli_first_zone`
- `tests/diag/expected.csv`, `stats.csv` - 4 cpu rows, 11 dli rows, 3 dli statistics rows
- `tests/diag/tests.cmake`, `tests/roms/manifest.txt`, `tests/inventory/ci.txt`, `wasm32.txt` - `diag.cpu`, `diag.dli`, two fixture rows
- `docs/hardware/maria.md`, `bus.md`, `docs/emulation-tests.md`, `docs/testing.md` - derivations, the first-zone DLI, the fitted list, the ROM index, "Not measured here"
- `.planning/WINDOWS.md` - item 4 fixed

## Decisions Made

See `key-decisions`. The stats rows cover only the three DLI tests that reach a burst. The zone0 handler turns DMA off on line 16, so that frame has no DMA line to check.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Derivation error] parked reads 16, not 20**
- **Found during:** Task 2, the dli ROM's first run
- **Issue:** The derivation released the parked CPU at line 18's t 0 and forgot that DMA is still on. Line 18 is zone 1's first line, and its empty burst freezes the NMI handler's opcode fetch for ceil(15 / 4) = 4 cycles.
- **Fix:** A trace of the core (scratch harness, not committed) put the handler at line 18, t 36, with `lda INTIM` taking 8 φ2 counts. `docs/hardware/maria.md` now derives 16 with the burst and states that the first derivation gave 20. The row changed only after the doc showed where the 4 cycles come from, as the plan allows. The core was not changed.
- **Committed in:** `87005ba`

**2. [Rule 2 - Missing functionality the plan requires] A DLI on zone 0's entry raised no NMI**
- **Found during:** Task 2 design. The `zone0` row is `rtl`, and the core dropped this DLI, which was open as broken-windows item 4.
- **Fix:** VBE's DLI raises NMI on the second φ1 after t 50. The bus now records the in-flight cycle's length, so a slow cycle at t 43 moves the φ1s correctly. The fix comes with `frame.dli_first_zone` and doc updates. No frame or title pin moved: ci passed with 506 tests and no re-pin.
- **Committed in:** `11fc3e7`

**3. [Plan detail] stats.csv: zone0 row dropped before the first run**
- **Issue:** `8b88408` added a zone0 statistics row. That frame turns DMA off on line 16, so it has no burst line to check.
- **Fix:** The row was removed in the source commit, before the ROM ran.
- **Committed in:** `04a6e2d`

**4. [Plan detail] A hook in lib.s for a diagnostic's NMI handler**
- **Fix:** `LIB_TAIL` points NMI at `diag_nmi` when it is defined. `diag.rebuild` shows dma.a78 and cpu.a78 unchanged.
- **Committed in:** `04a6e2d`

---

**Total deviations:** 4 (1 derivation error, 1 required core fix, 2 details).
**Impact on plan:** Every gating value was committed before its ROM ran. One `guide` row was corrected, with the correction shown in the doc. No released pin moved.

## Issues Encountered

- dasm will not expand a local label passed as a macro argument, so the page-crossing guards on the timed poll loops are inline `IF` blocks.
- The scratchpad is shared, so the trace harness lived in its own subdirectory.

## Known Stubs

None.

## Next Phase Readiness

- The three diagnostics share `lib.s`, the driver and `diag.rebuild`. The `modes` and board ROMs follow the same pattern: derive, commit the source, then the bytes.
- Still open: broken-windows item 5. A valid BIOS is copied, but the machine fast-boots until 03-13.

## Self-Check: PASSED
