---
phase: 03-games-play-in-retroarch
plan: 02
subsystem: run-loop
tags: [run-loop, bus, maria, halt, rdy, cartridge, boot, c17, ctest]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "muhtari__maria_dma_line, muhtari__maria_vbe, the line statistics and the sticky feature mask (plan 03-01)"
provides:
  - "DEV_CART in the bus decode and muhtari__cart_read, _write and _peek with a switch on the board; a linear ROM ends at $FFFF"
  - "muhtari__boot_fast: the C10 hand-off at line 240, t 24, with cpu_phase 0 derived from that tick"
  - "muhtari_run_frame with content: the CPU runs from VBLANK start to VBLANK start, audio counted from elapsed ticks"
  - "MARIA's line events at t 0, 28, 42 and 43 from the tick count, fired before each cycle's HALT, RDY and NMI logic"
  - "holds split at every line event and HALT step; MARIA's burst runs at c2 with T0 = fall + 6"
  - "MARIA's registers on the bus: MSTAT's VBLANK bit, MARIA's WSYNC wire-ANDed with the TIA's, OFFSET writes ignored"
  - "nine frame.* tests in tests/frame/test_frame.c"
affects: [03-03, 03-04, 03-05, 03-08, 03-13]

actuals:
  tokens: 17983
  tasks: 3
  commits: 4
plan_head_before: 194891d54745a1b68a9ab8f651bcdfbbc3ce9954
plan_head_after: ef3ebbced82e2d4633830a9d61a5fb9b1dfe75e3

tech-stack:
  added: []
  patterns:
    - "Every cycle start, held or not, runs once through cycle_start: MARIA events up to the data tick, then the HALT step"
    - "A HALT request with until = MUHTARI__HALT_UNTIL_MARIA gets its end from the burst the bus runs at c2"
    - "Frame tests build a 48 KiB linear image in memory and drive the bus directly as the CPU, seeking to exact ticks"

key-files:
  created:
    - src/boot.c
    - tests/frame/tests.cmake
    - tests/frame/test_frame.c
  modified:
    - src/internal.h
    - src/bus.c
    - src/maria.c
    - src/frame.c
    - src/cart.c
    - src/tia.c
    - src/instance.c
    - CMakeLists.txt
    - tests/tests.cmake
    - tests/bus/test_bus.c
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/bus.md
    - docs/hardware/sally.md

key-decisions:
  - "The origin moves on by 119,402 at line 263's t 0, the next frame's line 0, not at the VBLANK-start crossing, so lines 258-262 keep a non-negative beam"
  - "A HALT window that falls during an RDY hold runs c0, c1 and c2 as held cycles; SALLY resumes when RDY and HALT have both released"
  - "VBE at line 16, t 43 runs only with DMA on and reads its DLL bytes at the event, taking no CPU cycle (a model)"
  - "The TIA's WSYNC also keeps a later pending release, so RDY is a true wire-AND"
  - "Every row of a frame with content is $00 until span rendering lands; no rows_scanned tracking is needed before then"

patterns-established:
  - "frame tests: muhtari-test-frame NAME, an in-memory linear cartridge through muhtari__cart_load and muhtari__boot_fast"
  - "Tests that write WSYNC lock INPTCTRL at $07 first, as a game does, so the write leaves MEN set"

requirements-completed: []

coverage:
  - id: D1
    description: "A linear cartridge answers on the bus through DEV_CART and muhtari__cart_read, and MARIA's DMA fetch reads cartridge space through the board"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^frame\\.(jam_ends|handoff_state)$' (exit 0)"
        status: pass
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^bus\\.' (exit 0, 18 tests)"
        status: pass
    human_judgment: false
  - id: D2
    description: "A jammed cartridge and a DMA-off cartridge end every frame within one instruction of the absolute VBLANK-start deadline"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^frame\\.(jam_ends|dma_off_ends)$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D3
    description: "Line events fire from the tick count, holds split at them, the burst runs at c2 with T0 = fall + 6, MARIA's WSYNC releases at the next t 0 and wire-ANDs with the TIA's, MSTAT moves at line 258 and line 16, t 42, and a refused request marks the line DMA blocked"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^frame\\.(hold_split_wsync_across_dma|halt_c1_write_visible|maria_wsync_releases_at_lrc|mstat_edges|dma_blocked_flag)$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D4
    description: "The fast-boot hand-off matches C10 at line 240, t 24 with cpu_phase 0; frame 1 is 8,190 ticks with 54 samples and rows 0-223 $00, frame 2 has 800 or 801"
    requirement: GAME-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^frame\\.(handoff_state|first_frame_partial)$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D5
    description: "With no content loaded every frame and audio hash is unchanged and the behaviour revision stays 1"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^core\\.' (exit 0, 11 tests, core.testcard.pinned_hash included)"
        status: pass
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^abi\\.(globals|undefined)$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D6
    description: "docs/hardware/bus.md describes the cartridge, line events, hold splitting, the c2 burst, both HALT refusal reasons and dma_blocked, the run loop and the hand-off; sally.md's JAM sentence says what frame.jam_ends shows"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^docs\\.' (exit 0, 3 tests)"
        status: pass
      - kind: other
        ref: "scripts/hygiene.sh --tree (exit 0)"
        status: pass
    human_judgment: false

duration: 13min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 02: The bounded run loop Summary

**A linear cartridge on the bus, the C10 fast-boot hand-off at line 240, t 24, and `muhtari_run_frame` stepping SALLY
from VBLANK start to VBLANK start. MARIA's line events fire from the tick count and split every hold, its burst runs
at c2 with T0 = fall + 6, and MSTAT and MARIA's WSYNC answer on the bus. Nine `frame.*` tests show that a jammed or
DMA-off ROM ends every frame on time.**

## Performance

- **Duration:** about 13 min
- **Started:** 2026-10-20T00:02:36Z
- **Completed:** 2026-10-20T00:15:58Z
- **Tasks:** 3
- **Files modified:** 17

## Accomplishments

- `src/bus.c` decodes cartridge space as `DEV_CART`, and `src/cart.c` answers it with one `switch` on the board.
  `src/maria.c`'s DMA fetch reads the same board.
- `src/boot.c` sets the C10 state. A static assert pins line 240, t 24 at 8,190 ticks before VBLANK start, and
  another derives `cpu_phase` 0 from that tick.
- `src/frame.c` steps the CPU to absolute deadlines and counts audio from the elapsed ticks. The no-content path is
  unchanged byte for byte.
- Each cycle start, real or held, runs once through `cycle_start`: MARIA's events up to its data tick, then the HALT
  step. So holds cannot skip a line event, and a burst can run inside an RDY hold.

## Task Commits

1. **Task 1: a jammed cartridge ends its frames (tracer)** - `b92ae0c` (feat). Tracer gate: the verify re-run passed
   (workflow exit 0, then the five named tests and hygiene), so the expansion went ahead.
2. **Task 2: MARIA on the loop** - `75c71a2` (test, RED) and `d81b630` (feat, GREEN). There was no refactor commit.
3. **Task 3: docs for the run loop, the hand-off and the HALT contract** - `ef3ebbc` (docs)

## TDD Gate Compliance

- **RED** (`75c71a2`): `ctest --test-dir build/ci -L '^frame$' --output-junit red-frame.xml` exited 8, with 6 of 9
  failing. `gsd-tools check tdd-red-evidence` returned `RED_EVIDENCE_OK` (target
  `frame.hold_split_wsync_across_dma`). Semantic assessment: each failure was a planned assertion on behaviour no
  line event provided yet. Examples: "stats cost 0 != 9", "MSTAT 0x0 != 0x80", "flags 0x0 != 0x20" and a read
  ending at 165274 instead of LRC's 165714. There was no build, load or crash fault. `handoff_state`,
  `first_frame_partial` and the TIA half of `maria_wsync_releases_at_lrc` already held after Task 1.
- **GREEN** (`d81b630`): all 9 frame tests pass, and the full workflow is green (428 tests, then 6 release tests).

## Verification

| Command | Exit |
|---------|------|
| `cmake --workflow --preset ci` | 0 (428 tests, then 6 release tests) |
| `ctest --test-dir build/ci -R '^(frame\.jam_ends\|core\.testcard\.pinned_hash\|core\.no_alloc_after_load\|abi\.globals\|abi\.undefined)$' --output-on-failure` | 0 |
| `ctest --test-dir build/ci -R '^(frame\.\|bus\.\|core\.)' --output-on-failure` | 0 (38 tests) |
| `ctest --test-dir build/ci -R '^docs\.' --output-on-failure` | 0 (3 tests, no "over budget") |
| `ctest --test-dir build/ci -N -R '^frame\.' \| grep -c 'frame\.'` | prints 9 |
| `cmake --workflow --preset hygiene` | 0 (24 tests) |
| `scripts/hygiene.sh --tree` | 0 |
| `grep -c 'the gate is shut' src/internal.h` | prints 1 |
| `grep -c 'c2\|fall + 6' docs/hardware/bus.md` | prints 6 |
| `grep -nE 'D-[0-9]{2}\|GAME-0' docs/hardware/bus.md docs/hardware/sally.md` | 1 (no match) |
| `cmake --workflow --preset nofp` | 1: this host has AppleClang and the preset needs GCC (CMakeLists.txt:219), so the lane is CI's |

## Files Created/Modified

- `src/boot.c`: the hand-off constants with their C10 derivation, and `muhtari__boot_fast`
- `src/bus.c`: `DEV_CART`, the MARIA register dispatch, `cycle_start`, `halt_cycle` with the c2 burst, the split
  `hold`, and MARIA's next event in `rearm`
- `src/maria.c`: the cartridge route in `fetch`, the line-event table, `muhtari__maria_start`, `_next_event`,
  `_event`, `_burst`, `_read` and `_write`
- `src/frame.c`: the run loop to the deadline, `$00` rows, and audio from the elapsed ticks
- `src/cart.c`: `muhtari__cart_read`, `_write` and `_peek`
- `src/tia.c`: WSYNC keeps a later pending release
- `src/instance.c`: power-on arms MARIA's events
- `src/internal.h`: the beam constants, `loaded`, the frame deadlines, the MARIA event and DMA fields, `halt_fall`,
  `MUHTARI__HALT_UNTIL_MARIA`, and the request_halt contract with both refusal reasons
- `tests/frame/*`, `tests/tests.cmake`, both inventories (9 `frame,portable` lines), and the MSTAT comment in
  `tests/bus/test_bus.c`
- `docs/hardware/bus.md`, `docs/hardware/sally.md`

## Decisions Made

See `key-decisions` above. Later plans depend on two of them: the origin moves at line 263's t 0, and a HALT window
inside an RDY hold counts held cycles as c0, c1 and c2.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug in the convention] The origin moves at line 263's t 0, not at the VBLANK-start crossing**
- **Found during:** Task 2
- **Issue:** The locked convention says the origin advances by 119,402 at each VBLANK-start crossing. Lines 258-262
  would then sit before the origin, and the unsigned beam would be negative.
- **Fix:** The origin advances when line 263's t 0 fires, which is the next frame's line 0. The frame boundary for
  `muhtari_run_frame` is still line 258, t 42.
- **Files modified:** src/maria.c, docs/hardware/bus.md
- **Commit:** d81b630, ef3ebbc

**2. [Rule 3 - Scope] No rows_scanned bitmap; `loaded` set by the hand-off**
- **Found during:** Task 1
- **Issue:** The plan asks for a `rows_scanned` bitmap or counter in instance.c, so that unscanned rows are written
  `$00`.
- **Fix:** Until plan 03-03 renders, every row of a frame with content is `$00`, which meets the convention, so a
  bitmap would have nothing to track. `loaded` is a field of the instance, cleared at create and set by
  `muhtari__boot_fast`. Plan 03-03 adds row tracking together with rendering.
- **Files modified:** src/frame.c, src/boot.c, src/internal.h
- **Commit:** b92ae0c

**3. [Rule 2 - Correctness] The TIA's WSYNC also takes the later release**
- **Found during:** Task 2
- **Issue:** The TIA write overwrote `rdy_release`, so a TIA WSYNC written after `STA $24` could release RDY before
  MARIA's t 0. RDY is a wire-AND.
- **Fix:** Both writers keep the later release. TIA-only behaviour is unchanged, because a later TIA write always
  releases later.
- **Files modified:** src/tia.c
- **Commit:** d81b630

**4. [Rule 3 - Blocking] hygiene.no_float failed on four citations**
- **Found during:** Task 2, from the hygiene workflow
- **Issue:** The float scan exempts only `section N.N` on one line. Two comments carried over from plan 03-01 read
  "sections 2.3, 4.2 and 4.4". Two new ones broke "section" and "4.7" or "2.1" across a line.
- **Fix:** Each citation now reads "section N.N" on one line.
- **Files modified:** src/maria.c, src/cart.c
- **Commit:** d81b630

**5. [Rule 3 - Blocking] Added `muhtari__maria_start`**
- **Found during:** Task 2
- **Issue:** The power-on and the hand-off place the beam directly. Nothing armed the first line event or set
  MSTAT to match.
- **Fix:** `muhtari__maria_start(m)` arms the first event after the beam and sets VBLANK. Both callers set
  `next_deadline` to 0, so the first cycle recomputes it.
- **Files modified:** src/maria.c, src/internal.h, src/instance.c, src/boot.c
- **Commit:** d81b630

**6. [Rule 1 - Test bug] WSYNC tests lock INPTCTRL first**
- **Found during:** Task 2 RED
- **Issue:** The hand-off leaves INPTCTRL unlocked, as the BIOS does. A test's `STA WSYNC` of `$00` therefore also
  wrote INPTCTRL, cleared MEN and slowed every later cycle.
- **Fix:** Those cases write `$07` to `$01` first, as a game does. In the same RED pass, the TIA release target was
  moved to within one 456-tick TIA line of the write.
- **Files modified:** tests/frame/test_frame.c
- **Commit:** 75c71a2

---

**Total deviations:** 6 auto-fixed: 1 convention fix, 1 scope trim, 1 correctness fix, 2 blocking, 1 test bug.
**Impact on plan:** Every planned symbol exists except `rows_scanned`. One internal function was added. No-content
hashes are unchanged, and the behaviour revision stays 1.

## Known Stubs

| File | Line | Stub | Resolved by |
|------|------|------|-------------|
| src/frame.c | 63 | Every row of a frame with content is written `$00`: MARIA renders nothing yet (the locked "rows until plan 03-03" convention) | plan 03-03, span rendering |

Recorded in `.planning/WINDOWS.md` as a `stub` entry. Entry 2, MARIA's cartridge fetch, is marked fixed.

## Issues Encountered

`cmake --workflow --preset nofp` cannot configure on this host (AppleClang). The CI lane runs it with GCC.

## User Setup Required

None.

## Next Phase Readiness

Plan 03-03 can render spans from line RAM at t 0 and before each `$20-$3F` write, and raise the DLI's NMI from the
burst. It will replace the `$00` rows without changing the loop.

---
*Phase: 03-games-play-in-retroarch*
*Completed: 2026-10-20*

## Self-Check: PASSED

All three created files exist, and the four task commits (b92ae0c, 75c71a2, d81b630, ef3ebbc) are ancestors of HEAD.
