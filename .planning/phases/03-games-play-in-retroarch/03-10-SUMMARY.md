---
phase: 03-games-play-in-retroarch
plan: 10
subsystem: input
tags: [input, controllers, riot, tia, movie, runner, lag, c17, ctest]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "Public input API, the latch at VBLANK start, device resolution and the CPU-only lag flag (plan 03-04)"
provides:
  - "src/ports.c pins: muhtari__ports_swcha, muhtari__ports_swchb, muhtari__ports_inpt, muhtari__ports_count_hazard and io_hazard in struct muhtari__ports"
  - "RIOT port A and B read (OR & DDR) | (pins & ~DDR) from the devices and switches; PA7's edge detector sees the pins, also at the latch"
  - "TIA INPT0-INPT5 D7 from the devices and the mode: ProLine two-button and one-button, 2600 stick, unplugged"
  - "runner/movie.{c,h}: muhtari__movie_parse, the whole muhtari-movie 1 grammar validated before frame 1, allocation-free"
  - "tests/input: an in-memory cart that reads every input register once a frame; eleven input.* tests driven by movie rows"
  - "runner.movie_parse: 33 table rows plus generated limit cases, each with its exact LINE:COL and message"
  - "docs/hardware/controllers.md"
affects: [03-11, 03-13, libretro adapter, movies, fuzz.movie]

actuals:
  tokens: 18900
  tasks: 3
  commits: 6
plan_head_before: efb4e02d2a4e0e7315647d84d22aec99ebad0f66
plan_head_after: 4017b2b649abab678d6587dcb1e863faeedb4e3c

tech-stack:
  added: []
  patterns:
    - "Device pins live in src/ports.c; riot.c and tia.c read them, so a device model changes one file"
    - "An in-memory test cart takes its settings from a ROM table the test patches, so one 6502 program serves every row"
    - "Movie rows in tests are muhtari-movie 1 text parsed by the runner's own reader"

key-files:
  created:
    - runner/movie.c
    - runner/movie.h
    - tests/input/test_input.c
    - tests/input/tests.cmake
    - tests/runner/test_movie.c
    - docs/hardware/controllers.md
  modified:
    - src/ports.c
    - src/riot.c
    - src/tia.c
    - src/frame.c
    - src/internal.h
    - tests/riot/test_riot.c
    - tests/runner/tests.cmake
    - tests/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/riot.md
    - docs/emulation-tests.md
    - docs/testing.md
    - docs/embedding.md

key-decisions:
  - "Two-button mode follows the levels at each read: PB2 [PB4] an output driven 0 and INPTCTRL TIA-EN clear; no write latches it"
  - "INPT0-INPT3 follow the ProLine's buttons in one-button mode too, as MAME, a7800 and MiSTer do; the console's own reading is unverified and documented so"
  - "io_hazard adds 1 per frame that ends with a 2600 stick's fire held on a port in two-button mode; it is internal, not in the public API"
  - "SWCHB's unconnected input pins D2, D4 and D5 read 1; with nothing held and both difficulties B, SWCHB reads $3F"
  - "A movie line is at most 64 bytes, except a rom-sha256 or bios-sha256 line, which holds its key, a space and 64 hex digits"
  - "The RIOT reset starts PA7's detector at the pin's level, so a direction held through a reset is no edge"

patterns-established:
  - "Movie refusals carry static what and expected strings and a position; the reader names the first fault in file order"

requirements-completed: []

coverage:
  - id: D1
    description: "Every direction on both ports reaches SWCHA from a movie row, and changes the picture of the frame that reads it"
    requirement: GAME-04
    verification:
      - kind: integration
        ref: "tests/input/test_input.c#directions (ctest -R '^input\\.directions$', exit 0)"
        status: pass
    human_judgment: false
  - id: D2
    description: "ProLine in two-button and one-button mode, a 2600 stick's fire in both with io_hazard, CTLSWB $14 with no SWCHB write, INPTCTRL TIA-EN, the console switches and unplugged ports read as the cartridge notes section 3.1 to 3.3 say"
    requirement: GAME-04
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -L '^input$' (11 tests, exit 0)"
        status: pass
    human_judgment: false
  - id: D3
    description: "A Right press on port 0 applied at VBLANK start sets PA7's flag at that tick; its release does not"
    requirement: GAME-04
    verification:
      - kind: integration
        ref: "tests/input/test_input.c#pa7_edge"
        status: pass
    human_judgment: false
  - id: D4
    description: "Lag: a program reading on even frames gives 30 lag frames in 60; DMA fetching from $0280 and a peek still count as lag"
    requirement: GAME-04
    verification:
      - kind: integration
        ref: "tests/input/test_input.c#lag_even_frames"
        status: pass
      - kind: integration
        ref: "tests/input/test_input.c#lag_dma_and_peek"
        status: pass
    human_judgment: false
  - id: D5
    description: "muhtari__movie_parse validates the whole muhtari-movie 1 file before frame 1, with every limit, and refuses each bad input at its exact line and column; a header with no frames gives 0 frames and a zero-byte file is refused at 1:1"
    requirement: GAME-04
    verification:
      - kind: unit
        ref: "tests/runner/test_movie.c (ctest -R '^runner\\.movie_parse$', exit 0)"
        status: pass
    human_judgment: false
  - id: D6
    description: "The pins, the reader and the tests hold on every hosted leg: six builds, asan, be32, nofp, wasm32, hash-equality and RetroArch"
    requirement: GAME-04
    verification:
      - kind: other
        ref: "https://github.com/szTheory/muhtari/actions/runs/39362291672 (conclusion success)"
        status: pass
    human_judgment: false

duration: 21min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 10: Controllers at pin level, driven by movie rows Summary

**ProLine (both modes), 2600 stick, console switches and unplugged ports reach SWCHA, SWCHB and INPT0-INPT5 from src/ports.c pins; a pure muhtari-movie 1 reader validates the whole file; an in-memory cart reads every input register once a frame and proves each value from movie syntax.**

## Performance

- **Duration:** 21 min
- **Started:** 2026-10-20T02:46:07Z
- **Completed:** 2026-10-20T03:07:05Z
- **Tasks:** 3
- **Files modified:** 20

## Accomplishments

- Port A and B read their pins from the applied devices and switches. PA7's edge detector sees the pins at the latch and at reset.
- INPT0-INPT5 follow the device and the two-button mode: PB2 [PB4] an output driven 0, with TIA-EN clear. A frame that ends with a stick's fire held in that mode counts `io_hazard`.
- `runner/movie.{c,h}`: the whole D-21 grammar with every limit, no allocation and no file access. Every refusal has an exact position.
- `tests/input/test_input.c`: a 48 KiB cart built from our own 6502 bytes. It reads SWCHA, SWCHB and INPT0-5 into a RAM ring and paints them as eight 28-line bands. I viewed one frame as an image: SWCHA $7F, SWCHB, then the six INPT bytes, each in its own band.
- `docs/hardware/controllers.md` plus updates to riot.md, emulation-tests.md, testing.md and embedding.md.
- Hosted run 39362291672 is green on every leg.

## Task Commits

1. **Task 1: tracer, Up on port 1 reaches SWCHA:** `63aefef` (feat). Tracer gate: interactive, end-of-phase and automated-only, so the `<verify>` was re-run. It passed: `cmake --workflow --preset ci` exit 0, `input.directions` passed, `scripts/hygiene.sh --tree` exit 0.
2. **Task 2: the whole grammar:** RED `8443c46` (test), GREEN `7e0150a` (feat), then `4017b2b` (fix: a 32-bit-safe cast).
3. **Task 3: every input row:** RED `57e6b5e` (test), GREEN `6237bd3` (feat).

## TDD evidence

- **Task 2 RED:** `ctest -R '^runner\.movie_parse$'` exited non-zero. The test built and ran, and failed on the planned assertions: the keys, the per-position messages, the byte checks and the limits were not yet implemented (for example `FAIL row 4: got rc 0 ... expected 3:1: duplicate key`). This is a semantically valid RED. The CTest output is not a format `tdd-red-evidence` reads, and `tdd_mode` is off.
- **Task 2 GREEN:** passed with all 33 table rows and the generated limit cases.
- **Task 3 RED:** 8 of the 11 `input.*` tests failed on their row assertions: SWCHB read $FF instead of $3F, the INPT D7 values were wrong, and `io_hazard` was 0. `input.pa7_edge`, `input.lag_even_frames` and `input.lag_dma_and_peek` already passed. Their behaviour comes from Task 1's latch hook and 03-04's CPU-path lag flag, so they are regression guards rather than new behaviour.
- **Task 3 GREEN:** all 11 passed, along with every riot and core test.

## Decisions Made

See `key-decisions`. Frame pins did not move: the full suite, including every `basic.*` chain and `diag.*`, passed unchanged after SWCHB began reading $3F. No unreleased pin was rewritten.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] 64-byte line limit vs hash lines**
- **Found during:** Task 2
- **Issue:** D-21 says lines are at most 64 bytes. A `rom-sha256` line is 75 bytes and a `bios-sha256` line is 76, so a strict limit would refuse every movie that names a hash.
- **Fix:** Every line is at most 64 bytes, except a hash line, which is exactly its key, a space and 64 digits. A longer hash line is refused at the next column with "too many hex digits".
- **Files modified:** runner/movie.c
- **Commit:** 7e0150a

**2. [Rule 3 - Blocking] RIOT tests assumed every SWCHB input pin reads high**
- **Found during:** Task 3
- **Issue:** `riot.swchb.ddr` and `riot.power_on.defaults` expected $FF or $EB. Difficulty B now reads 0 on D6 and D7.
- **Fix:** The RIOT test fixture sets both difficulty switches to A, so its input pins still read high. The tests' purpose, port B's DDR and ORB logic, is unchanged.
- **Files modified:** tests/riot/test_riot.c
- **Commit:** 6237bd3

**3. [Rule 1 - Bug] Tautological comparison on 32-bit size_t**
- **Found during:** pre-push review
- **Issue:** `len > UINT32_MAX` is always false where `size_t` is 32 bits (wasm32, be32).
- **Fix:** Cast directly. The 32 MiB file limit bounds the length.
- **Commit:** 4017b2b

**4. [Rule 2 - Docs] Small doc touches outside the file list**
- docs/testing.md's runner label row names `runner.movie_parse`.
- docs/embedding.md's Input section points to controllers.md.
- **Commit:** 6237bd3

**5. [Rule 3 - Blocking] io_hazard field in the RED commit**
- Task 3's RED commit added `io_hazard` to `struct muhtari__ports` so the rows compile and fail on assertions rather than on the build.
- **Commit:** 57e6b5e

---

**Total deviations:** 5 auto-fixed (2 bugs, 2 blocking, 1 docs).
**Impact on plan:** None changes the planned shape. The line-limit exception is the one grammar reading, and it is stated in the reader and its tests.

## Issues Encountered

- The tracer test's input rows put their input on movie line 2. 03-04 made the first frame after a load start at the hand-off, so it never latches line 1. Plan 03-11's runner must choose how a movie's line 1 maps to frames.
- `runner/movie.{c,h}` cite "docs/runner.md, Movies". Plan 03-11 Task 3 writes that section.

## Known Stubs

None.

## User Setup Required

None.

## Next Phase Readiness

- Plan 03-11 can link `runner/movie.c` into `muhtari-run` for `--movie`, write the header's devices into every frame's input, and print `lag_frames`.
- `io_hazard` is internal. Exposing it through counters would be an API change for a later plan.
- Still open from WINDOWS.md: a valid BIOS is copied but the machine fast-boots until 03-13.

---
*Phase: 03-games-play-in-retroarch*
*Completed: 2026-10-20*
