---
phase: 03-games-play-in-retroarch
plan: 11
subsystem: runner
tags: [runner, movie, input, lag, cli, ctest, docs]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "runner/movie.{c,h} muhtari__movie_parse, the pin-level controllers and the in-memory input cart (plan 03-10); the input latch and lag counter (plan 03-04)"
provides:
  - "muhtari-run --movie FILE: the whole movie read (at most 32 MiB plus one byte) and checked before frame 1, line f set before frame f, lag_frames N after the hash lines"
  - "--port1/--port2 auto|proline|joystick|none and --region auto|ntsc, with --region pal refused at parse time"
  - "Header precedence: a conflicting --port1, --port2 or --region, or a header with boot bios, exits 2; --frames defaults to the movie's frame count"
  - "Past the last line every button and console switch is released and the last difficulty positions are kept"
  - "rom-sha256 checked against the headerless SHA-256 (exit 1 naming both); another behaviour revision warns once"
  - "muhtari-test-input --write-cart FILE [even]: the input cart and its even-frame variant as files (fixture input_carts)"
  - "docs/runner.md Movies section: the muhtari-movie 1 grammar, timing, length, precedence, limits and the error line"
affects: [03-13, libretro adapter, movies, fuzz.movie]

actuals:
  tokens: 11705
  tasks: 3
  commits: 4
plan_head_before: ae1d3981120457feb8faa880d3e68e3b52808326
plan_head_after: 7e8a7b54663dcf034174b364b9bc952e055f2be0

tech-stack:
  added: []
  patterns:
    - "Runner checks run in two steps: parse_options checks spelling, check_options checks the options against each other and the movie header once the movie is read"
    - "Movie tests in tests/runner/tests.cmake write their movies at run time and compare frame hashes of the input cart, each run twice"

key-files:
  created:
    - .planning/phases/03-games-play-in-retroarch/03-11-SUMMARY.md
  modified:
    - runner/main.c
    - runner/exit_status.h
    - CMakeLists.txt
    - tests/input/test_input.c
    - tests/input/tests.cmake
    - tests/runner/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/runner.md

key-decisions:
  - "Movie line k is the input set before the runner's frame k (the k-th muhtari_run_frame), latched at the VBLANK start that opens that frame. Frame 1 opens at the fast-boot hand-off, so line 1 reaches no game; a movie is then a record of what a host passes to muhtari_set_input before each frame"
  - "--region ntsc sets muhtari_media.region, so a PAL cartridge runs as NTSC under it; auto still refuses a PAL cartridge with 77"
  - "Without a movie the runner sets input only when --port1 or --port2 names a device, so existing frame pins are untouched"
  - "A movie with rom-sha256 and no cartridge exits 1"
  - "The usage row now reads 'the command line is invalid or conflicts with the movie's header, so nothing ran'; the failure row names a malformed movie and a movie naming another cartridge"

patterns-established:
  - "A header option conflict is a usage error, printed as '--port1 joystick conflicts with the movie's port1 proline'"

requirements-completed: []

coverage:
  - id: D1
    description: "muhtari-run --movie replays a movie on the input cart and prints lag_frames 30 over 60 frames on the even-frame variant"
    requirement: GAME-04
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#movie.lag (ctest -R '^runner\\.movie\\.lag$', exit 0)"
        status: pass
      - kind: integration
        ref: "tests/input/tests.cmake#input.cart_files (ctest -R '^input\\.cart_files$', exit 0)"
        status: pass
    human_judgment: false
  - id: D2
    description: "Held input changes the picture of the frames that read it and matches the empty movie once released, repeatably; ProLine and joystick on both ports and Reset, Select, Pause and both difficulty switches each give a distinct frame"
    requirement: GAME-04
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^runner\\.movie\\.(replay|ports|switches)$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D3
    description: "Past the last movie line buttons are released and the difficulty kept: 20 frames of a 10-line movie equal the 20-line movie that spells it out"
    requirement: GAME-04
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#movie.past_end (exit 0)"
        status: pass
    human_judgment: false
  - id: D4
    description: "Refusals: a malformed movie exits 1 with FILE:LINE:COL, a rom-sha256 mismatch exits 1 naming both digests, a header conflict or boot bios exits 2, another behaviour revision warns once and exits 0, --port1 paddle and --region pal exit 2"
    requirement: GAME-04
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^runner\\.(movie\\.(malformed|rom_mismatch|conflict|behaviour_warning)|usage\\.(port|region_pal))$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D5
    description: "docs/runner.md documents --movie, the ports, the region, the muhtari-movie 1 grammar and lag_frames; the --help exit table matches it and the page is within budget"
    requirement: GAME-04
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^(runner\\.exit\\.table|docs\\.)' (4 tests, exit 0)"
        status: pass
    human_judgment: false
  - id: D6
    description: "The movie path holds on every hosted leg: six builds, asan, be32, nofp, wasm32, hash-equality and RetroArch"
    requirement: GAME-04
    verification:
      - kind: other
        ref: "https://github.com/szTheory/muhtari/actions/runs/39363678798 (conclusion success)"
        status: pass
    human_judgment: false

duration: 13min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 11: The runner replays movies Summary

**`muhtari-run --movie FILE` checks the whole muhtari-movie 1 file before frame 1, sets line f before frame f with the header's devices, applies the header-wins and past-the-end rules, checks `rom-sha256` and `behaviour`, and prints `lag_frames N`. `--port1/--port2` and `--region` are added, and 11 new runner tests drive the input cart through the CLI.**

## Performance

- **Duration:** 13 min
- **Started:** 2026-10-20T03:08:55Z
- **Completed:** 2026-10-20T03:22:07Z
- **Tasks:** 3
- **Files modified:** 9

## Accomplishments

- `--movie FILE` reads at most 32 MiB plus one byte with sequential reads. Two parser passes count the frames and then fill them, and a refusal prints `muhtari-run: FILE:LINE:COL: <what>, expected <what>` and exits 1. Every frame gets its line's input with the header's devices.
- `check_options` applies the header after the movie is read. A conflicting `--port1`, `--port2` or `--region` exits 2, and so does `boot bios`, which waits for the BIOS path in plan 03-13. `--frames` defaults to the movie length.
- Past the end of the movie, every button and console switch is released and the last difficulty positions are kept.
- `rom-sha256` is compared with the headerless SHA-256, which is the same rule as `--probe`'s `sha256_headerless`. A different `behaviour` prints one warning.
- `muhtari-test-input --write-cart FILE [even]` writes the generated cart into the build tree, so no ROM bytes are tracked.
- docs/runner.md gains a Movies section and the new options, and its exit table matches `--help`.
- Hosted run 39363678798 is green on every leg.

## Task Commits

1. **Task 1: tracer, a movie drives the runner:** `f9228f0` (feat). For the tracer gate, the run was interactive, end-of-phase and automated-only, so the `<verify>` was re-run: `cmake --workflow --preset ci` exited 0, `runner.movie.lag` and `input.cart_files` passed, and `scripts/hygiene.sh --tree` exited 0.
2. **Task 2: ports, region, precedence, past the end and the checks:** RED `c6605b6` (test), GREEN `dbc09bf` (feat).
3. **Task 3: docs/runner.md:** `7e8a7b5` (docs).

## TDD evidence

- **Task 2 RED:** `ctest -R '^runner\.(movie|usage\.(port|region_pal))'` built and ran all 13 tests. The 10 new ones failed on their own assertions, all for missing behaviour:
  - movies without `--frames` exited 2 with `--frames N or --bench N is required`;
  - `--port1`, `--port2` and `--region` exited 2 as unknown options, not with their own messages;
  - `past_end`'s explicit `--frames 20` run passed, and the run that relies on the default failed.
  `runner.movie.lag` and `runner.movie_parse` still passed. This is a semantically valid RED. CTest output is not a format `tdd-red-evidence` reads, and `tdd_mode` is off.
- **Task 2 GREEN:** all 49 runner tests pass.

## Decisions Made

See `key-decisions`. The main one is how lines map to frames, which the previous plan left open. Movie line k is the input set before the runner's frame k, so a movie records what a host passes to `muhtari_set_input` before each `muhtari_run_frame`. A libretro recording would replay the same way. Under fast boot the cost is that line 1 reaches no game. docs/runner.md, Movies, says so and tells users to put their first input on line 2.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] The plan's replay expectation at frame 12 contradicts the input cart**
- **Found during:** Task 2
- **Issue:** The behaviour list says Up held on lines 5-10 gives a frame-12 hash that differs from an empty movie's. The cart paints only the entry it read last, and frame 12 reads released input, so its picture equals the empty movie's.
- **Fix:** `runner.movie.replay` requires frame 8, inside the held lines, to differ and frame 12 to match the empty movie, which also proves the release. `ports` and `switches` compare frame 8.
- **Files modified:** tests/runner/tests.cmake
- **Commit:** c6605b6

**2. [Rule 1 - Bug] The help line for --movie was missing after Task 1**
- **Found during:** Task 2
- **Issue:** A string replacement in Task 1 did not match, so `--help` did not list `--movie`.
- **Fix:** Added in Task 2 together with the other option lines.
- **Commit:** dbc09bf

**3. [Rule 1 - Bug] The held_hash test helper padded joystick movies with a ProLine-shaped line**
- **Found during:** Task 2 GREEN
- **Fix:** The helper takes the released line that matches the header.
- **Commit:** dbc09bf

**4. [Rule 3 - Blocking] The exit-table doc line changed in Task 2**
- **Issue:** Rewording the failure and usage rows in `exit_status.h` breaks `runner.exit.table` unless docs/runner.md changes in the same commit.
- **Fix:** Task 2 changed both rows in docs/runner.md, and Task 3 wrote the rest of the doc.
- **Commit:** dbc09bf

**5. [Scope] One fixture invocation writes both carts**
- `--write-cart FILE [even]` may repeat, so the single `input.cart_files` test writes `input.a78` and `input-even.a78`.
- **Commit:** f9228f0

---

**Total deviations:** 5 (3 bugs, 1 blocking, 1 scope note).
**Impact on plan:** The replay test asserts more than the plan asked: a difference while the input is held and a match after release. Nothing else changes the planned shape.

## Issues Encountered

None. No frame or title pin moved, because a run without a movie or a port option sets no input.

## Known Stubs

None. `boot bios` exits 2 by design until plan 03-13, as the plan says.

## User Setup Required

None.

## Next Phase Readiness

- Plan 03-13 adds `--boot`, `--bios`, and the `boot` and `bios-sha256` keys. It replaces the `boot bios` refusal in `check_options` and should decide whether a BIOS boot's first frame starts at a VBLANK start, which would change what movie line 1 does.
- GAME-04 stays open until 03-13 applies the movie's boot key.
- Still open from WINDOWS.md: a valid BIOS is copied, but the machine fast-boots until 03-13.

---
*Phase: 03-games-play-in-retroarch*
*Completed: 2026-10-20*

## Self-Check: PASSED

docs/runner.md, runner/main.c, runner/exit_status.h and the SUMMARY exist. Commits f9228f0, c6605b6, dbc09bf and 7e8a7b5 are on HEAD.
