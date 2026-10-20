---
phase: 03-games-play-in-retroarch
plan: 16
subsystem: testing
tags: [runner, movie, boot-to-play, spacephysics, png, negative-controls]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "muhtari-run --movie, --hash-frame and --dump-frame (plan 03-11); the built spacephysics sample and its title@120 ledger row (03-08, 03-14)"
provides:
  - "runner.boot_to_play: spacephysics played from the fast boot through the real muhtari-run, judged by muhtari-test-boot-to-play"
  - "runner.boot_to_play.{no_input,port2,mirrored,static_mutant,late_mutant}, each failing with its exact mask or message"
  - "tests/runner/png_read.{c,h}: the PNG reader test_png.c used inline, now shared"
  - "docs/emulation-tests.md, boot to play"
affects: [03-17, 03-19, phase 4 audio check on spacephysics]

actuals:
  tokens: 8746
  tasks: 2
  commits: 2
plan_head_before: b15f18383a74b6139f37be0d78dfa4bdb3062140
plan_head_after: 2491e4366a20ad038ab040f8450071bf956c76c8

tech-stack:
  added: []
  patterns:
    - "A judge prints one mask line, mask <bits>: <details>, so expect_fail can require an exact mask with ^mask N:"
    - "Negative controls judge a setup test's recorded outputs through a CTest fixture, and the mutants transform those outputs, never the emulator"

key-files:
  created:
    - tests/runner/boot_to_play.cmake
    - tests/runner/boot_to_play.c
    - tests/runner/png_read.c
    - tests/runner/png_read.h
  modified:
    - tests/runner/test_png.c
    - tests/runner/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/emulation-tests.md

key-decisions:
  - "The held input is Up+Left on port 1 from frame 121 for 60 frames; its mirror is Up+Right. Up alone thrusts, but Down does nothing in this game, so Up has no mirror; Left and Right turn on a global 8-frame cadence, too late for (a)"
  - "(a) uses the measured bound [F, F+1], inside the planned [F, F+2]: spacephysics draws thrust one frame after the input from any start frame, so a one-frame-late latch lands on F+2 and would pass a two-frame bound"
  - "(b) and (c) are judged only when some frame from F on differs from the no-input run, so a run with no response fails (a) alone, giving the planned mask 1 for no_input and port2"
  - "The mirror check also requires the input run to move up, so swapped left and right bits fail, not only merged ones"
  - "The late mutant delays only the hashes from frame F; delaying the whole list moved the boot frames and put the first difference at frame 7"

patterns-established:
  - "Frame-dump tests decode the runner's PNG with tests/runner/png_read.c"

requirements-completed: [GAME-05]

coverage:
  - id: D1
    description: "runner.boot_to_play plays spacephysics from the fast boot through the real muhtari-run: four movies, a hash for every frame to 180 and dumps at 120, 140, 160 and 180; the input run gives mask 0 (first difference at frame 122, the difference moving 17 px, the picture changing after frame 160)"
    requirement: GAME-05
    verification:
      - kind: e2e
        ref: "tests/runner/boot_to_play.cmake (runner.boot_to_play)"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39375738182"
        status: pass
    human_judgment: false
  - id: D2
    description: "Up+Left moves the ship up 17 px across the hold window and its mirror, Up+Right, down 17 px; the input judged as its own mirror prints sign did not flip"
    requirement: GAME-05
    verification:
      - kind: e2e
        ref: "tests/runner/boot_to_play.cmake (runner.boot_to_play)"
        status: pass
      - kind: integration
        ref: "tests/runner/tests.cmake (runner.boot_to_play.mirrored)"
        status: pass
    human_judgment: false
  - id: D3
    description: "The no-input run and the port 2 run judged as the input each fail with exactly mask 1"
    requirement: GAME-05
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake (runner.boot_to_play.no_input, runner.boot_to_play.port2)"
        status: pass
    human_judgment: false
  - id: D4
    description: "The static mutant (frame 120 dump recoloured and repeated) fails with exactly mask 2, and the late mutant (hashes from frame 121 one frame later) with exactly mask 1"
    requirement: GAME-05
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake (runner.boot_to_play.static_mutant, runner.boot_to_play.late_mutant)"
        status: pass
    human_judgment: false
  - id: D5
    description: "test_png.c reads the PNG through the shared png_read.c and keeps its file size, four-block and pixel checks"
    requirement: GAME-05
    verification:
      - kind: unit
        ref: "tests/runner/test_png.c (runner.png)"
        status: pass
    human_judgment: false
  - id: D6
    description: "docs/emulation-tests.md has a boot to play section: the runs, the predicates and mask bits, how the controls were found, and the negative controls"
    requirement: GAME-05
    verification:
      - kind: other
        ref: "ctest --test-dir build/ci -R '^docs\\.' (exit 0); section read against boot_to_play.c by the agent"
        status: pass
    human_judgment: false

duration: 22min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 16: Boot to play Summary

**spacephysics, played from the fast boot through the real runner, reacts to Up+Left on port 1 one frame after the input and moves its ship 17 px up the screen. Up+Right moves it down by the same amount. Five negative controls each fail with their exact mask or message, on every hosted leg including Windows and wasm32.**

## Performance

- **Duration:** about 22 min
- **Started:** about 2026-10-20T05:17Z (exploration); ledger recorded at 05:23:05Z
- **Completed:** 2026-10-20T05:39:38Z
- **Tasks:** 2
- **Files modified:** 9

## How the controls were found

The sample's source was never opened (PROVENANCE section 3 lists the samples as unknown; the build directory holds `spacephysics.bas`, and it stayed unread). Everything below comes from runner output and dumped frames, which I viewed as images:

1. No input: frames 120 and 240 show one ship at about (50, 122) pointing right, and nothing else. Every hash from frame 7 to 240 is the same, so the no-input picture is static.
2. Each input held alone on port 1 from frame 121 for 60 frames, hashing frames 118 to 180, with frames 150 and 180 dumped and viewed:
   - **Up:** the first change is at frame 122 (F+1), and the ship moves right, faster each frame (diff centroid x 51, 53, 57, 63, 70, 78 at frames 130 to 180). With the start frame anywhere from 121 to 150, the first change always came one frame after the input.
   - **Left and Right:** the ship turns in 8-frame steps. The first change is at 125 for any start frame from 121 to 124, and at 133 for starts from 125 to 128, so it falls on a global 8-frame cadence, too late for (a).
   - **Button 2:** fires a shot (frame 180 shows a bullet).
   - **Down and button 1:** no change.
3. Up held on port 2: no frame differs from the no-input run.
4. Up+Left and Up+Right, frames 121 to 180 dumped (and later the test's own dumps at 140 and 180 viewed): the ship thrusts while turning, climbing under Left and diving under Right. The diff centroid's y shift was -18 and +18 in exploration, and is 17 px each way in the test's window.

The input chosen is **Up+Left on port 1**, with **Up+Right** as its mirror. It is the only pair of inputs that responds within the bound and moves in mirrored directions.

## Accomplishments

- `tests/runner/boot_to_play.cmake` writes the `none`, `input`, `port2` and `mirror` movies and runs the built `muhtari-run` on each. Each run gets `--hash-frame` 1 to 180 and `--dump-frame` at 120, 140, 160 and 180. The script then requires `mask 0:` from `play` and `sign flipped:` from `mirrored`.
- `muhtari-test-boot-to-play` (`play`, `static`, `late`, `mirrored`) judges (a), (b) and (c) and prints `mask <bits>: <details>`. It exits 0, 1, or 2 on a usage or file error.
- Five `expect_fail` controls with exact regexes: `^mask 1:` (no_input, port2, late_mutant), `^mask 2:` (static_mutant), `^sign did not flip:` (mirrored). They depend on the main test through the `boot_to_play` fixture.
- `png_read.c` holds the reader that was inline in `test_png.c`. `runner.png` still checks the file size, the four stored blocks and every pixel.

## Task Commits

1. **Task 1: End-to-end "a held direction moves the ship" (tracer)**: `4965f68` (feat)
2. **Task 2: The five negative controls, the mirror check and the docs**: `2491e43` (test)

**Plan metadata:** the SUMMARY commit follows.

Task 1's commit already holds the judge's `static`, `late` and `mirrored` modes. Task 2 registers the controls, adds the mirror check to the main test and writes the docs.

## Tracer gate

`human_verify_mode` is end-of-phase and Task 1's `<verify>` is automated-only, so I re-ran it with no checkpoint: `cmake --workflow --preset ci` exited 0 (574 tests), `runner.boot_to_play` passed, and `scripts/hygiene.sh --tree` exited 0. The tracer was verified end to end before expanding.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] The planned (a) window [F, F+2] cannot catch a one-frame-late latch on this game**
- **Found during:** Task 1 exploration and Task 2
- **Issue:** spacephysics responds at F+1, so a latch one frame late gives F+2, which a two-frame bound accepts. The planned truth "a latch one frame late fails (a)" would not hold.
- **Fix:** (a) takes the bound L as an argument. The test passes L = 1, the measured response, which stays inside D-24's [F, F+2], so every passing run still meets the planned predicate.
- **Files modified:** tests/runner/boot_to_play.c, tests/runner/tests.cmake
- **Commit:** 4965f68

**2. [Rule 1 - Bug] The "mirrored direction" of Up does not exist in this game**
- **Found during:** Task 1 exploration
- **Issue:** Down changes nothing, so the centroid of Up's mirror cannot flip.
- **Fix:** the held direction is Up+Left and its mirror Up+Right (mirrored about the vertical). The mirror check also pins the input run's direction (up), so swapped left and right bits fail too.
- **Files modified:** tests/runner/boot_to_play.cmake, tests/runner/boot_to_play.c
- **Commit:** 4965f68, 2491e43

**3. [Rule 1 - Bug] The late mutant first shifted the whole hash list**
- **Found during:** Task 2
- **Issue:** shifting frames 1 to 6 (the blank boot frames) put the first difference at frame 7.
- **Fix:** only the hashes from frame F are delayed, as a late latch would delay them.
- **Files modified:** tests/runner/boot_to_play.c
- **Commit:** 4965f68

**4. [Rule 3 - Blocking] test_png.c's reader was inline in a test with its own main**
- **Found during:** Task 1
- **Issue:** the plan's "judges with test_png.c's reader" needs a reader that both programs can link.
- **Fix:** the parse moved into tests/runner/png_read.{c,h}. test_png.c calls it and keeps its writer-specific checks.
- **Files modified:** tests/runner/png_read.c, tests/runner/png_read.h, tests/runner/test_png.c, tests/runner/tests.cmake
- **Commit:** 4965f68

**5. [Rule 1 - Docs] The docs do not name `prop.input_at_vblank`**
- **Found during:** Task 2
- **Issue:** that test is created by plan 03-17, so naming it now would point at a test that does not exist yet.
- **Fix:** the docs say input timing between frames is left to the property tests. Plan 03-17 can name the test when it lands.
- **Files modified:** docs/emulation-tests.md
- **Commit:** 2491e43

**Total deviations:** 5 auto-fixed (3 bugs in the planned design found by measurement, 1 blocking, 1 docs). **Impact:** every planned truth holds, and the masks are as planned. (a) is tighter than D-24's bound, never looser.

## Evidence

- **Local:** `cmake --workflow --preset ci` exited 0 (579 tests and 6 release tests). `ctest --test-dir build/ci -R '^runner\.boot_to_play' --output-on-failure` exited 0 (6 tests). `ctest --test-dir build/ci -R '^docs\.'` exited 0. `scripts/hygiene.sh --tree` exited 0. `cmake --workflow --preset hygiene` exited 0 (24 tests).
- **Acceptance:** `ctest --test-dir build/ci -N -R '^runner\.boot_to_play' | grep -c 'runner\.boot_to_play'` printed 6. `grep -c 'mask 1\|mask 2' tests/runner/tests.cmake` printed 4. Both inventories list `runner.boot_to_play` with `portable,runner`.
- **Sanitizers:** the judge, built with `cc -fsanitize=address,undefined -fno-sanitize-recover=all`, ran every mode and the usage and missing-file paths with no report. The local asan preset needs `clang-21` by its Linux name, so the hosted asan job is the preset run.
- **Hosted:** CI run https://github.com/szTheory/muhtari/actions/runs/39375738182 on `2491e43` concluded success on every job. All six `runner.boot_to_play*` tests passed on asan, be32, wasm32, and linux, macos and windows on x64 and arm64.

## Known Stubs

None.

## Issues Encountered

None beyond the deviations above.

## Next Phase Readiness

Ready for 03-17 (property tests and fuzzing). Phase 4's "not silent" check can attach to the same spacephysics runs: button 2 fires, and that is where its sound effects should be.

## Self-Check: PASSED

- FOUND: tests/runner/boot_to_play.cmake, tests/runner/boot_to_play.c, tests/runner/png_read.c, tests/runner/png_read.h
- FOUND: 4965f68, 2491e43 (ancestors of HEAD)
