---
phase: 03-games-play-in-retroarch
plan: 01
subsystem: maria
tags: [maria, dma, display-list, line-ram, c17, ctest]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: "the bus's HALT window and open-bus byte, system RAM and its page 0/1 mirrors, the counting allocator and check header"
provides:
  - "struct muhtari__maria in the instance: registers, DLL walk state, two line-RAM buffers, the write-mode latch, 263 line statistics and a sticky feature mask"
  - "muhtari__maria_dma_line: one DMA burst with per-fetch ticks, holes, the abort at LRC and line-RAM writes"
  - "muhtari__maria_vbe: the VBE fetch that loads the DLL pointer and zone 0's entry"
  - "eleven maria.dma_line.* unit tests with no CPU"
  - "docs/hardware/maria.md with every DMA constant, the closed form and the 16/24 handshake derivation"
  - "docs/emulation-tests.md, the page later plans add their label sections to"
affects: [03-02, 03-03, 03-05, 03-06, 03-07]

actuals:
  tokens: 15916
  tasks: 3
  commits: 4
plan_head_before: 95552817dc1a2aafdd83f592de6f9888deaab487
plan_head_after: b961c5a31735ddd047c21411801df110f8cb74a9

tech-stack:
  added: []
  patterns:
    - "One burst walks the DL with a cursor tick per fetch; a fetch at or after LRC is dropped and ends the walk"
    - "Expected ticks in tests are written from docs/hardware/maria.md's constants with the arithmetic in a comment"
    - "Line-RAM cells are palette x 4 + data pair; a sentinel prefill shows which cells a burst wrote"

key-files:
  created:
    - src/maria.c
    - tests/maria/tests.cmake
    - tests/maria/test_maria.c
    - docs/hardware/maria.md
    - docs/emulation-tests.md
  modified:
    - src/internal.h
    - CMakeLists.txt
    - tests/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/testing.md

key-decisions:
  - "Holes come from the DLL entry's H16 and H8 bits (A7800-MARIA section 2.1), not CTRL as the plan's wording said"
  - "muhtari__maria_vbe is added so a test can load zone 0's entry from DPPH/DPPL; plan 03-02's t 43 event calls it"
  - "dma_line stores its statistics in stats[line] as well as *st, and clamps LRC to T0 + 454 so every call is bounded"
  - "A feature bit sets when an object uses it (read mode, kangaroo, write mode 1, the zone's H8/H16, indirect width); a line with no object sets no mode bit"
  - "bytes counts graphics, character-map and character-graphics bytes fetched; holey slots are not counted; a holey object counts as completed"

patterns-established:
  - "maria tests: muhtari-test-maria NAME, one burst per case, instance from the public API with the counting allocator"
  - "Emulation label sections live in docs/emulation-tests.md, never in docs/testing.md"

requirements-completed: []

coverage:
  - id: D1
    description: "MARIA's state lives in the instance and src/maria.c is its own translation unit in muhtari_objs, with no mutable global and no undefined reference"
    requirement: GAME-02
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci (exit 0, 419 tests then 6 release tests)"
        status: pass
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^abi\\.(globals|undefined)$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D2
    description: "An empty display list costs T0 + 9 on an ordinary line and T0 + 15 on a zone's last line, and a zero-object line writes no line-RAM cell"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^maria\\.dma_line\\.empty_dl$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D3
    description: "Headers 8/10, graphics 3, indirect 6/9, holes one slot of the object's kind, the terminator rule (b1 & 0x5F) == 0 and the last line's +6 match the closed form T0 + 9 + D or T0 + 15 + D"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^maria\\.dma_line\\.(headers|widths|indirect|holes|terminators|last_line)$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D4
    description: "An overrun drops every fetch at or after LRC, ends with HALTRST at LRC + 3 or LRC + 9, and keeps the objects completed so far and the partial object's earlier cells"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^maria\\.dma_line\\.abort$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D5
    description: "Line-RAM cells for 160A, 160B, 320A, 320B, 320C and 320D, kangaroo mode, hpos wrap and write-mode inheritance, the per-line statistics and the sticky feature mask"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^maria\\.dma_line\\.(line_ram|stats|feature_mask)$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D6
    description: "docs/hardware/maria.md holds every constant with its source, the closed form, the terminator rule and the 16/24 handshake derivation; docs/emulation-tests.md exists and docs/testing.md points to it within budget"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^docs\\.' (exit 0, 3 tests)"
        status: pass
      - kind: other
        ref: "scripts/hygiene.sh --tree (exit 0)"
        status: pass
    human_judgment: false

duration: 10min
completed: 2026-10-19
status: complete
---

# Phase 3 Plan 01: MARIA's DMA burst Summary

**MARIA's display-list walk as one burst with a tick for every fetch: 4- and 5-byte headers, direct and indirect objects, H8/H16 holes, the terminator rule, the abort at LRC with +3/+9 tails, line RAM in both write modes, and per-line statistics with a sticky feature mask. Eleven CPU-free tests pin it to A7800-MARIA section 4.2's closed form.**

## Performance

- **Duration:** about 10 min
- **Started:** 2026-10-19T23:46:23Z
- **Completed:** 2026-10-19T23:56:03Z
- **Tasks:** 3
- **Files modified:** 11

## Accomplishments

- `src/maria.c` runs a line's DMA burst from T0 to HALTRST. Each fetch reads RAM, or the floating byte for TIA, MARIA,
  RIOT and cartridge space, and leaves it on the bus.
- The ten behaviour cases and `empty_dl` derive every expected tick from the cited constants, with the arithmetic in a
  comment; `grep -c 'section 4'` counts 35 citations in the test file.
- `docs/hardware/maria.md` derives the empty-DL 16/24 ticks from T0 = /HALT fall + 6 (fall + 15 rounds up to 4 cycles
  and fall + 21 to 6). It also lists each fitted value with the observation that would change it.

## Task Commits

1. **Task 1: End-to-end empty-DL burst** - `de6e5b2` (feat). Tracer gate: verify re-run passed, so the expansion went ahead.
2. **Task 2: The whole DL walk** - `e930ab8` (test, RED) and `aaa1984` (feat, GREEN). No refactor commit.
3. **Task 3: maria.md and the emulation-tests page** - `b961c5a` (docs)

## TDD Gate Compliance

- **RED** (`e930ab8`): `ctest --test-dir build/ci -L '^maria$' --output-junit` exited 8. Ten new cases failed and
  `empty_dl` passed. `gsd-tools check tdd-red-evidence` returned `RED_EVIDENCE_OK` (target
  `maria.dma_line.headers`). Semantic assessment: every failure was a planned assertion on HALTRST, statistics, cells
  or features against the terminator-only burst ("4-byte header: expected 1020 got 1009"). There was no build, load
  or crash fault.
- **GREEN** (`aaa1984`): all 11 maria tests pass; the full workflow is green.

## Verification

| Command | Exit |
|---------|------|
| `cmake --workflow --preset ci` | 0 (419 tests, then 6 release tests) |
| `ctest --test-dir build/ci -R '^maria\.dma_line\.empty_dl$' --output-on-failure` | 0 |
| `ctest --test-dir build/ci -L '^maria$' --output-on-failure` | 0 (11 tests) |
| `ctest --test-dir build/ci -R '^abi\.(globals\|undefined)$'` | 0 |
| `ctest --test-dir build/ci -R '^docs\.' --output-on-failure` | 0 (3 tests) |
| `scripts/hygiene.sh --tree` | 0 |
| `ctest --test-dir build/ci -N -L '^maria$' \| grep -c 'maria\.dma_line\.'` | prints 11 |
| `grep -nE '\b(426\|430)\b' src/maria.c` | 1 (no match) |
| `grep -nE 'D-[0-9]{2}\|GAME-0\|DEC\.' docs/hardware/maria.md docs/emulation-tests.md` | 1 (no match) |
| `wc -c < docs/testing.md` | prints 35700 (budget 40960) |

## Files Created/Modified

- `src/maria.c`: the DMA constants, the fetch helper, VBE, the DL walk, holes, the abort, line-RAM writes, statistics
  and features
- `src/internal.h`: `struct muhtari__line_stats`, `MUHTARI__LINE_*`, `MUHTARI__FEAT_*`, `MUHTARI__LINES_NTSC`,
  `struct muhtari__maria` (as member `maria`), and the burst and VBE declarations
- `CMakeLists.txt`: `src/maria.c` in `muhtari_objs`
- `tests/maria/tests.cmake`, `tests/maria/test_maria.c`: `muhtari-test-maria` and the 11 cases, with a provisional
  TIMEOUT of 10
- `tests/tests.cmake`: includes the maria tests
- `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt`: the 11 names, labelled `maria,portable`
- `docs/hardware/maria.md`, `docs/emulation-tests.md`, `docs/testing.md`

## Decisions Made

See `key-decisions` above. The two that matter for later plans are that holes come from the DLL entry, and that
`muhtari__maria_vbe` exists for plan 03-02's t 43 event to call.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] The holey bits come from the DLL entry, not CTRL**
- **Found during:** Task 2
- **Issue:** The plan says "holey 8 or 16 per CTRL". A7800-MARIA section 2.1 puts H16 and H8 in bits 6 and 5 of the
  DLL entry's byte 0. CTRL has no holey bits.
- **Fix:** The hole test reads the zone's entry byte, and the `holes` case sets H16 and H8 in the DLL entry.
- **Files modified:** src/maria.c, tests/maria/test_maria.c
- **Committed in:** e930ab8, aaa1984

**2. [Rule 3 - Blocking] Added `muhtari__maria_vbe` to load zone 0's entry**
- **Found during:** Task 1
- **Issue:** The test sets DPPH/DPPL and calls the burst. In hardware the next DLL entry is read only at the end of a
  zone's last line, and zone 0's entry at VBE (section 2.4), so nothing could load the first zone from DPP.
- **Fix:** Added a 7-tick VBE fetch, `muhtari__maria_vbe(m, t0)`. It reads DPPH/DPPL and zone 0's entry. The tests
  call it first, and plan 03-02's t 43 event can call it.
- **Files modified:** src/internal.h, src/maria.c, tests/maria/test_maria.c
- **Committed in:** de6e5b2

**3. [Rule 2 - Bounded call] dma_line clamps LRC to T0 + 454**
- **Found during:** Task 1
- **Issue:** A DL with no terminator would walk until LRC. With an unbounded LRC argument, the walk would never end
  (AGENTS rule 4).
- **Fix:** The walk's LRC is the earlier of the argument and T0 + 454. This is documented on the declaration.
- **Committed in:** de6e5b2

**4. [Rule 3 - Interpretation] The burst's `line` argument indexes stats**
- **Issue:** The planned signature passes both `line` and `*st`.
- **Fix:** The burst fills `*st` and also stores it in `m->maria.stats[line]` when line < 263.
- **Committed in:** de6e5b2

---

**Total deviations:** 4 auto-fixed (1 bug in the plan's wording, 2 blocking, 1 bounded-call)
**Impact on plan:** The planned signature and names are unchanged; one internal function was added. There is no scope creep.

## Known Stubs

| File | Line | Stub | Resolved by |
|------|------|------|-------------|
| src/maria.c | 68 | DMA fetches from cartridge space return the floating byte, because the board read is not routed yet (the plan calls this a functionality gap) | plan 03-02, which routes the fetch to `muhtari__cart_read(m, addr, 1)` |

Recorded in `.planning/WINDOWS.md` as a `stub` entry.

## Issues Encountered

None.

## User Setup Required

None.

## Next Phase Readiness

Plan 03-02 can wire the burst into the bus. It adds line events, the t 28 HALT request, the c2 call with
T0 = fall + 6, VBE at t 43 through `muhtari__maria_vbe`, and the cartridge route in `fetch`.

---
*Phase: 03-games-play-in-retroarch*
*Completed: 2026-10-19*

## Self-Check: PASSED

All five created files exist, and the four task commits (de6e5b2, e930ab8, aaa1984, b961c5a) are ancestors of HEAD.
