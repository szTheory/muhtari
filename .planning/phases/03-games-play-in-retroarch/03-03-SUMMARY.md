---
phase: 03-games-play-in-retroarch
plan: 03
subsystem: maria
tags: [maria, rendering, line-ram, dli, nmi, run-loop, 7800basic, c17, ctest]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "MARIA's line events, the c2 burst, the fast-boot hand-off and the bounded run loop (plan 03-02)"
provides:
  - "Span rendering from line RAM into the caller's rows: rasters 16-257 to rows 0-241, cells written twice, read modes 0-3, colour kill & $0F, first pixel t 135"
  - "A register write renders up to its data tick; t 0 renders the rest of the raster before the swap; NULL video writes nothing and leaves the same state"
  - "The DLI: NMI on the second phi1 after /HALT rises, raised through muhtari__bus_raise_nmi, its return kept as MUHTARI__LINE_DLI_QUEUED"
  - "The narrowed raise_nmi contract: 1 means queued for latching"
  - "frame.sample_renders, frame.dli_queued, frame.null_video_same_state and the dropped-edge sub-case of bus.nmi.one_edge_at_a_time"
affects: [03-04, 03-05, 03-08, 03-13]

actuals:
  tokens: 11553
  tasks: 3
  commits: 4
plan_head_before: f3010be98ac7cf374ce36294a8798efa5482b94a
plan_head_after: 1bbf63ff5d7330bec6273ff80bec17cb3d044c5c

tech-stack:
  added: []
  patterns:
    - "MARIA holds the caller's row pointer and pitch only for the duration of muhtari_run_frame; px tracks the next pixel of the current raster whether or not rows are set"
    - "The DLI's edge tick is measured on SALLY's 4-tick grid from c2, the cycle at which the bus runs the burst"
    - "Built-sample tests take the sample path as a second argument and require the built_samples fixture"

key-files:
  created: []
  modified:
    - src/maria.c
    - src/frame.c
    - src/internal.h
    - src/bus.c
    - tests/frame/test_frame.c
    - tests/frame/tests.cmake
    - tests/built/tests.cmake
    - tests/bus/test_bus.c
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/maria.md
    - docs/hardware/bus.md
    - docs/emulation-tests.md
    - .planning/preparation/ARCHITECTURE.md

key-decisions:
  - "A register write's data tick is the last pixel shown with the old value; the next pixel shows the new one"
  - "Rasters 16 and 17 show BACKGRND because no burst runs before line 17, not through a special case"
  - "Only the first frame after a load pre-writes $00, to every row; rendering then overwrites the rows the beam scans, so no rows_scanned bitmap is needed"
  - "The DLI's NMI is the second grid tick strictly after HALTRST on the 4-tick grid from c2"
  - "A DLI on zone 0's entry, read by VBE with no HALT window, raises no NMI yet; its timing waits for the diagnostic ROMs"

patterns-established:
  - "frame tests that take an NMI first lock INPTCTRL and set S to $FF, as a game does"

requirements-completed: []

coverage:
  - id: D1
    description: "The built simple sample renders through the internal load, the fast-boot hand-off and the run loop: frame 120 has two colours and is not the test card, and two instances give the same 120 frames"
    requirement: GAME-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^frame\\.sample_renders$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D2
    description: "MARIA renders spans from line RAM into the caller's rows, with read modes 0-3, colour kill as & 0x0F, a register write rendering up to its data tick, and frame 1's unscanned rows written $00"
    requirement: GAME-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^frame\\.(sample_renders|first_frame_partial|null_video_same_state)$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D3
    description: "With io->video NULL nothing is written and line RAM, statistics, CPU and RAM match a frame with video"
    requirement: GAME-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^frame\\.null_video_same_state$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D4
    description: "The DLI on a zone's last line raises NMI through muhtari__bus_raise_nmi, the CPU enters its handler on that line, and the line carries DLI_QUEUED"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^frame\\.dli_queued$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D5
    description: "raise_nmi's 1 means queued for latching, and a queued edge dropped by the latch under MEN = 0 is pinned by a sub-case"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^bus\\.nmi\\.one_edge_at_a_time$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D6
    description: "With no content loaded every frame and audio hash is unchanged and the behaviour revision stays 1"
    requirement: GAME-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^core\\.' (exit 0, 11 tests, core.testcard.pinned_hash included)"
        status: pass
    human_judgment: false
  - id: D7
    description: "maria.md, bus.md and emulation-tests.md describe line events, rendering, the DLI, the NMI return and frame 1 as built; ARCHITECTURE's cpu_phase row is derived from C10"
    requirement: GAME-02
    verification:
      - kind: other
        ref: "ctest --test-dir build/ci -R '^docs\\.' (exit 0, no over budget)"
        status: pass
    human_judgment: false

duration: 11min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 03: MARIA renders and raises the DLI Summary

**MARIA now writes each pixel into the caller's rows in spans broken at register writes and at t 0, so 7800basic's `simple` sample draws its face through the fast-boot hand-off. The DLI raises NMI on the second φ1 after /HALT rises, and MARIA keeps the bus's "queued" answer.**

## Performance

- **Duration:** about 11 min
- **Started:** 2026-10-20T00:22:16Z
- **Completed:** 2026-10-20T00:33:33Z
- **Tasks:** 3
- **Files modified:** 14

## Accomplishments

- `src/maria.c` plays raster n's pixel x at t 135 + x from the buffer line n - 1 filled. A register write first renders
  up to its data tick, and the t 0 event finishes the raster before line RAM swaps. Read modes 0-3 decode the cell,
  and colour kill masks with `0x0F`. With no rows set, `px` still advances, so state is identical.
- `src/frame.c` gives MARIA the caller's rows for the frame only. It writes `$00` only into frame 1 after a load,
  before the run, so the rows the beam scans overwrite it. The `$00` stub (ledger entry 3) is fixed.
- `muhtari__maria_burst` raises the DLI's edge at c2 + 4 × (⌊(HALTRST − c2) / 4⌋ + 2). A 1 from the bus sets the
  line's `DLI_QUEUED` flag and the feature bit.
- The `raise_nmi` contract in `src/internal.h`, `src/bus.c` and `docs/hardware/bus.md` now reads "queued for
  latching". No drop counter was added.

## Task Commits

1. **Task 1: spans into the caller's rows, frame.sample_renders (tracer)** - `ff819b5` (feat). Tracer gate
   (interactive, end-of-phase, automated-only verify): the verify re-run passed before the commit, with the workflow
   at exit 0, the named tests at exit 0 and hygiene at exit 0, so the expansion went ahead.
2. **Task 2: the DLI, the NMI contract, the null-video check** - `f06d45f` (test, RED) and `78c0c4d` (feat, GREEN).
   There was no refactor commit.
3. **Task 3: the MARIA page, bus.md's NMI, the frame tests page, ARCHITECTURE's cpu_phase row** - `1bbf63f` (docs)

## TDD Gate Compliance

- **RED** (`f06d45f`): `ctest --test-dir build/ci -R '^(frame\.(dli_queued|null_video_same_state)|bus\.nmi\.one_edge_at_a_time)$' --output-junit red-03-03.xml`
  exited 8. `gsd-tools check tdd-red-evidence` returned `RED_EVIDENCE_OK` for the target `frame.dli_queued`.
  - Semantic assessment: the target failed on its planned assertions. The handler was entered 0 times, not 1, and
    line 32's flags were `0x5` (LAST|DLI) where `0xd` was expected, without DLI_QUEUED. There was no build, load or
    crash fault.
  - `frame.null_video_same_state` and the bus sub-case passed at RED. They pin behaviour that Task 1 and the existing
    latch already provide, and the plan lists them as checks, not as new behaviour.
- **GREEN** (`78c0c4d`): all 31 `frame.*` and `bus.*` tests pass, and the full workflow is green (431 tests, then 6
  release tests).

## Verification

| Command | Exit |
|---------|------|
| `cmake --workflow --preset ci` | 0 (431 tests, then 6 release tests) |
| `ctest --test-dir build/ci -R '^(frame\.\|core\.testcard\.pinned_hash\|core\.no_alloc_after_load\|abi\.globals)' --output-on-failure` | 0 (15 tests, after Task 1) |
| `ctest --test-dir build/ci -R '^(frame\.\|bus\.)' --output-on-failure` | 0 (31 tests) |
| `ctest --test-dir build/ci -R '^core\.'` | 0 (11 tests, no-content hashes intact) |
| `ctest --test-dir build/ci -R '^docs\.' --output-on-failure` | 0 (3 tests, no "over budget") |
| `ctest --test-dir build/ci -N -R '^frame\.' \| grep -c 'frame\.'` | prints 12 |
| `cmake --workflow --preset hygiene` | 0 (24 tests, format and no_float included) |
| `scripts/hygiene.sh --tree` | 0 |
| `grep -nE '^frame\.sample_renders[[:space:]]built,portable$' tests/inventory/ci.txt` | one line |
| `grep -c 'a queued edge dropped by the latch' tests/bus/test_bus.c` | prints 1 |
| `grep -nE 'drop(ped)?_count\|nmi_drops' src/*.c src/*.h` | 1 (no match) |
| `grep -nE 'D-[0-9]{2}\|GAME-0' docs/hardware/*.md docs/emulation-tests.md` | 1 (no match) |

**Data point, not a pin** (plan 03-05 pins it): `muhtari-test-frame sample_renders build/ci/built/simple/simple.bas.a78`
prints `frame 120: 2 colours, sha256 07c13a656b348d010554f5760ea965799caab9b1220ef3e51c6599422aeec8df`. Running 240
frames takes 0.03 s. I viewed frame 120 as an image. It shows face1.png's 8-row face in colour 3 (P0C3 = `$FB`) at
rows 27-34 (rasters 43-50), with each 160-pixel cell doubled, on a `$00` background. The PNG's rows 9-17 are padding
and 7800basic's palette row. The picture is byte-identical before and after the DLI landed.

## Files Created/Modified

- `src/maria.c`: `pixels_by`, `pixel_reg`, `render`, `render_to`; `px` set in `muhtari__maria_start`; the t 0
  render; the render before a register store; the DLI in `muhtari__maria_burst`
- `src/frame.c`: the caller's rows handed to MARIA; `$00` only for the partial first frame; the test card path
  unchanged
- `src/internal.h`: `rows`, `pitch` and `px` in MARIA's state; the narrowed `raise_nmi` contract and the write
  comment
- `src/bus.c`: the `raise_nmi` comment says "queued for latching"
- `tests/frame/test_frame.c`, `tests/frame/tests.cmake` (sha256.c linked as a test helper),
  `tests/built/tests.cmake`, `tests/bus/test_bus.c`, both inventories
- `docs/hardware/maria.md`, `docs/hardware/bus.md`, `docs/emulation-tests.md`,
  `.planning/preparation/ARCHITECTURE.md`

## Decisions Made

See `key-decisions` above. Later plans depend on two of them. The DLI's edge is measured on the CPU grid from c2.
Zone 0's DLI is not raised yet.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Test bug] frame.dli_queued's program locks INPTCTRL and moves the stack**
- **Found during:** Task 2 GREEN
- **Issue:** The NMI was taken on line 32, but the entry jumped to `$2C2C`. The hand-off leaves S = `$16` and
  INPTCTRL unlocked, so the pushes to `$0114`-`$0116` fall in the TIA's range. They rewrote INPTCTRL to `$0C`, which
  cleared MEN, and the vector was read through the 2600 decode. The hardware model is faithful; the test program
  was not.
- **Fix:** The program writes `$07` to INPTCTRL and sets S to `$FF` first, as a game does. A comment in the case
  explains why.
- **Files modified:** tests/frame/test_frame.c
- **Commit:** 78c0c4d

**2. [Rule 2 - Docs accuracy] maria.md's stale cartridge-fetch sentence**
- **Found during:** Task 3
- **Issue:** The page still said that MARIA's cartridge fetch floats. Plan 03-02 had already routed that fetch to
  the board.
- **Fix:** The sentence now says the board answers MARIA as it answers the CPU.
- **Files modified:** docs/hardware/maria.md
- **Commit:** 1bbf63f

**3. [Rule 3 - Scope] No rows_scanned tracking**
- **Found during:** Task 1
- **Issue:** The plan says that rows not scanned since the load are still written `$00`.
- **Fix:** Only frame 1 after a load is partial. When `frame_deadline - frame_start` is not a full frame, the run
  writes `$00` to every row first, and rendering overwrites the rows the beam scans. That meets the convention with
  no bitmap.
- **Files modified:** src/frame.c
- **Commit:** ff819b5

---

**Total deviations:** 3 auto-fixed: 1 test bug, 1 docs accuracy fix, 1 scope trim.
**Impact on plan:** Every planned symbol and test exists, the no-content hashes are unchanged, and the behaviour
revision stays 1.

## Known Stubs

| File | Stub | Resolved by |
|------|------|-------------|
| src/maria.c | A DLI set on zone 0's DLL entry, which VBE reads with no HALT window, raises no NMI. MiSTer and A7800Hawk fire it after VBE, and its timing needs the diagnostic ROMs. | the diagnostic-ROM plans |

Recorded in `.planning/WINDOWS.md` as a `todo` entry. Entry 3, the `$00` rows, is marked fixed.

## Issues Encountered

`clang-format` is not on PATH on this host. The hygiene preset finds clang-format 21 at
`/opt/homebrew/opt/llvm@21/bin/clang-format`, and every changed C file was checked with it.

## User Setup Required

None. No external service configuration is required.

## Next Phase Readiness

Plan 03-04 can put the public API and the runner on top of this: `muhtari_run_frame` renders a cartridge's picture,
and a NULL video buffer leaves the machine's state unchanged.

---
*Phase: 03-games-play-in-retroarch*
*Completed: 2026-10-20*

## Self-Check: PASSED

The four task commits (ff819b5, f06d45f, 78c0c4d, 1bbf63f) are ancestors of HEAD, and every modified file named above exists.
