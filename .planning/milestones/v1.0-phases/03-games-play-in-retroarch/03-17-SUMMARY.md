---
phase: 03-games-play-in-retroarch
plan: 17
subsystem: testing
tags: [property-tests, splitmix64, libfuzzer, fuzz, nightly, ci, run-loop]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "the built simple and eightysix samples (03-05), muhtari__peek and muhtari__bus_peek, the ports' pending input (03-10), the fast boot and the run loop"
provides:
  - "tests/prop.h: splitmix64, case count and seed from the environment, size-halving shrink, seed and case printed on failure"
  - "prop.split_run, prop.peek_pure, prop.instances_isolated, prop.input_at_vblank over a pool of run seeds, a random cart and two samples"
  - "tests/fuzz/run.c with its replay fuzz.regress.run and self test on every lane, and fuzz.run (libFuzzer) in the fuzz preset"
  - "tests/fuzz/run_cart.h: any bytes as a headerless 48 KiB linear cartridge whose footer passes; tests/fuzz/run_seeds.c: the run target's seeds"
  - "nightly.yml: a report-only prop job at 10,000 cases, the fuzz job running both targets, a pull_request path filter listing every input of its jobs"
affects: [03-18, 03-19]

actuals:
  tokens: 14704
  tasks: 3
  commits: 4
plan_head_before: aeb059e9ecf909bf5e2d8b53361258eec5d38b1f
plan_head_after: c2d3ce02062c78a0667127c312355f7234249d63

tech-stack:
  added: []
  patterns:
    - "A property is int fn(uint64_t *rng, uint32_t size); prop_run derives case c's stream from the seed and c alone, so a failure reruns from MUHTARI_PROP_SEED and MUHTARI_PROP_CASES=c+1"
    - "Runs compare by frame record (status, audio count, picture SHA-256) and a state digest: SHA-256 of struct muhtari with pointers cleared, then the cart's ROM and RAM"
    - "A second fuzz target reuses replay_main.h by linking its own implementation of seeds.h"

key-files:
  created:
    - tests/prop.h
    - tests/prop/test_prop.c
    - tests/prop/tests.cmake
    - tests/fuzz/run.c
    - tests/fuzz/run_cart.h
    - tests/fuzz/run_seeds.c
  modified:
    - tests/tests.cmake
    - tests/fuzz/tests.cmake
    - tests/cart/write_images.c
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/inventory/fuzz.txt
    - .github/workflows/nightly.yml
    - docs/emulation-tests.md
    - docs/testing.md
    - .planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md

key-decisions:
  - "The run target's cartridge passes the footer test: run_cart.h changes the last 8 bytes as little as needed, because a headerless 48 KiB image with a failing footer is refused as a 2600 image and no frame would run code"
  - "The prop pool makes its seed images and random cart through the same run_cart.h, so all 20 pool entries load and run"
  - "prop.input_at_vblank reads SWCHA, SWCHB and INPT0-5 through muhtari__bus_peek, since muhtari__peek returns open bus for every device register"
  - "prop.input_at_vblank starts at k >= 1: the first frame after a fast-boot load starts at the hand-off, which latches nothing"
  - "split_run's split is the host's queries and a wider pitch between frames k and k+1, since the API runs one frame per call"
  - "The run target bounds each frame at four lines past its deadline; the worst seen over 3,000 random cartridges of 60 frames was 794 ticks"
  - "The nightly prop job builds the ci preset with MUHTARI_TIMEOUT_SCALE=10 and runs ctest -L prop -j 4, rather than the full ci workflow, whose 60 s timeouts would kill 10,000 cases"
  - "The path filter names every src/ file one by one; a new source file also changes CMakeLists.txt, which is in the filter. ci.txt and tests/tests.cmake stay out, because ci.yml already fails on a break in them"
  - "No nightly be32 job: 03-05 kept basic on the per-pull-request be32 and asan lanes"

patterns-established:
  - "Property tests live in tests/prop/ and register under the prop label with portable, on the lanes that build the samples"
  - "A fuzz target's regressions go under tests/fuzz/<target>/regress/ with a manifest row"

requirements-completed: []

coverage:
  - id: D1
    description: "tests/prop.h (80 lines) runs a property over MUHTARI_PROP_CASES cases from MUHTARI_PROP_SEED, defaults 100 and a fixed seed, shrinks by halving the size and prints seed, case and smallest failing size; a non-decimal value exits 2"
    requirement: GAME-06
    verification:
      - kind: unit
        ref: "tests/prop/tests.cmake (prop.split_run)"
        status: pass
      - kind: other
        ref: "MUHTARI_PROP_CASES=x build/ci/tests/muhtari-test-prop split_run build/ci/built (exit 2)"
        status: pass
    human_judgment: false
  - id: D2
    description: "prop.split_run: frames 1..k at a wider pitch, the host's queries, then k+1..N equal one sequence of N by every frame and the final digest"
    requirement: GAME-06
    verification:
      - kind: integration
        ref: "tests/prop/tests.cmake (prop.split_run)"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39377950148"
        status: pass
    human_judgment: false
  - id: D3
    description: "prop.peek_pure: peeking all 65,536 addresses through both peeks changes neither the digest nor any later frame"
    requirement: GAME-06
    verification:
      - kind: integration
        ref: "tests/prop/tests.cmake (prop.peek_pure)"
        status: pass
    human_judgment: false
  - id: D4
    description: "prop.instances_isolated: two instances on different cartridges, given input and run alternately call by call, equal each run alone"
    requirement: GAME-06
    verification:
      - kind: integration
        ref: "tests/prop/tests.cmake (prop.instances_isolated)"
        status: pass
    human_judgment: false
  - id: D5
    description: "prop.input_at_vblank: set_input calls between frames leave the digest without the pending copy and the input registers unchanged; the next frame applies the last call and equals an instance given only it; a core that applies input at the call fails it"
    requirement: GAME-06
    verification:
      - kind: integration
        ref: "tests/prop/tests.cmake (prop.input_at_vblank)"
        status: pass
      - kind: other
        ref: "src/ports.c with muhtari__ports_latch called from muhtari_set_input, then muhtari-test-prop input_at_vblank (exit 1, reverted)"
        status: pass
    human_judgment: false
  - id: D6
    description: "tests/fuzz/run.c: input truncated or padded to 49,152 bytes, loaded headerless, two instances agree over 2 frames, each frame within four lines of its deadline; fuzz.regress.run replays 17 seeds, the empty input among them, and fuzz.regress.run.self_test fails with no corpus inputs"
    requirement: GAME-06
    verification:
      - kind: integration
        ref: "tests/fuzz/tests.cmake (fuzz.regress.run, fuzz.regress.run.self_test)"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39377950148"
        status: pass
    human_judgment: false
  - id: D7
    description: "fuzz.run runs libFuzzer for 60 s with -timeout=2 -max_len=49152 in the fuzz preset, from seeds fuzz.run.seeds writes"
    requirement: GAME-06
    verification:
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39377950042"
        status: pass
    human_judgment: false
  - id: D8
    description: "nightly.yml: the prop job ran 10,000 cases per property seeded with the run ID and passed the JUnit check; the fuzz job ran both targets; the pull_request paths list every input of the jobs"
    requirement: GAME-06
    verification:
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39377950042"
        status: pass
      - kind: other
        ref: "cmake -DDIR=.github/workflows -P tests/cmake/timeouts.cmake (exit 0)"
        status: pass
    human_judgment: false
  - id: D9
    description: "docs/emulation-tests.md has prop and fuzz sections, names prop.input_at_vblank in boot to play, and docs/testing.md describes the fuzz and prop lanes and the nightly jobs"
    requirement: GAME-06
    verification:
      - kind: unit
        ref: "tests/policy/tests.cmake (docs.budgets)"
        status: pass
    human_judgment: false

duration: 20min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 17: Properties and the run fuzz target Summary

**Four run-loop properties over a 20-cartridge pool (100 cases per pull request, 10,000 nightly), a run fuzz target that makes any bytes a bootable headerless 48 KiB cart and checks two instances agree within a four-line bound, and a nightly path filter covering every input**

## Performance

- **Duration:** 20 min
- **Started:** 2026-10-20T05:45:30Z
- **Completed:** 2026-10-20T06:05:29Z
- **Tasks:** 3
- **Files modified:** 16

## Accomplishments

- `tests/prop.h` (80 lines) with `prop.split_run`, `prop.peek_pure`, `prop.instances_isolated` and `prop.input_at_vblank`. They pass at 100 cases on all nine hosted lanes. At 10,000 cases on ubuntu x64 the slowest, `instances_isolated`, takes 188 s.
- `tests/fuzz/run.c` and its replay. libFuzzer ran it for 61 s alongside the loader in the nightly fuzz job.
- The nightly `prop` job is report-only. The fuzz job is renamed `fuzz` and runs both targets. The path filter is complete, which closes WR-01.

## Task Commits

1. **Task 1: End-to-end "split runs equal one run" (tracer)**: `21895d5` (test)
2. **Task 2: The other three properties and the run fuzz target**: `0323ffe` (test)
3. **Task 3: Nightly jobs, the full path filter, docs**: `5d02e56` (ci), plus `c2d3ce0` (ci: the hosted prop timing in the job comment)

**Plan metadata:** the SUMMARY commit follows.

## Tracer gate

`human_verify_mode` is end-of-phase and Task 1's `<verify>` is automated-only, so I re-ran it and synthesized no checkpoint. `cmake --workflow --preset ci` exited 0 (580 tests). `prop.split_run` passed at 100 and at 500 cases. `scripts/hygiene.sh --tree` exited 0. The tracer was verified end to end before expanding.

## Files Created/Modified

- `tests/prop.h`: the harness
- `tests/prop/test_prop.c`, `tests/prop/tests.cmake`: the four properties, the pool and the state digest
- `tests/fuzz/run.c`: the run target and its replay
- `tests/fuzz/run_cart.h`: input to a 48 KiB headerless cart with a passing footer
- `tests/fuzz/run_seeds.c`: all $00, all $02, all $FF, 13 splitmix64 images and an empty input, through `seeds.h`
- `tests/fuzz/tests.cmake`: `fuzz.regress.run` and its self test; in the fuzz preset `fuzz.run.seeds`, `fuzz.run` and `fuzz.regress.run`
- `tests/tests.cmake`: prop included on the ci and wasm32 inventories
- `tests/cart/write_images.c`: comment notes the run seeds build
- `tests/inventory/{ci,wasm32,fuzz}.txt`: the new tests
- `.github/workflows/nightly.yml`: prop job, fuzz job, path filter
- `docs/emulation-tests.md`, `docs/testing.md`: prop and fuzz sections, lanes and nightly
- the phase 2 review-seed todo: WR-01 folded

## Decisions Made

See `key-decisions` in the frontmatter. The two that matter most:

- **The run cartridge passes the footer test.** As planned, the 48 KiB headerless image would almost never run, so the target makes its footer pass.
- **The input registers are read through `muhtari__bus_peek`.** `muhtari__peek` returns open bus for device registers, which would have made that check vacuous.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] A headerless 48 KiB image of arbitrary bytes is refused as a 2600 image**
- **Found during:** Task 1, when I probed the pool's load statuses
- **Issue:** Before the fix, all 16 seed images and the random cart returned `ERR_UNSUPPORTED_2600` (status 8), and the empty input returned `ERR_INVALID_IMAGE`. src/cart.c applies the BIOS footer test to headerless images, and arbitrary bytes fail it. The run fuzz target would never have run a CPU instruction. Only `simple` and `eightysix` would have run in the property pool.
- **Fix:** `tests/fuzz/run_cart.h` changes $FFF8, $FFF9 and the reset vector only as far as the footer test requires. run.c and the property pool both use it. All 20 pool entries now load.
- **Files modified:** tests/fuzz/run_cart.h, tests/prop/test_prop.c, tests/fuzz/run.c
- **Commit:** 21895d5, 0323ffe

**2. [Rule 1 - Bug] `muhtari__peek` cannot see the input registers**
- **Found during:** Task 2
- **Issue:** The plan reads SWCHA, SWCHB and INPT0-5 through `muhtari__peek`, which returns the open-bus byte for every device register. That check could never fail.
- **Fix:** `prop.input_at_vblank` reads them through `muhtari__bus_peek`, which reads the pins with no side effect. `prop.peek_pure` peeks through both functions.
- **Files modified:** tests/prop/test_prop.c
- **Commit:** 0323ffe

**3. [Rule 1 - Bug] Input set before frame 1 is not applied at frame 1**
- **Found during:** Task 2 design
- **Issue:** After a fast-boot load, frame 1 starts at the hand-off, which latches nothing (src/frame.c). With k = 0 the property's "next frame applies the last call" is false by design.
- **Fix:** k is at least 1. The docs say why.
- **Commit:** 0323ffe

**4. [Rule 3 - Blocking] The run target needs its own seed set and seed files**
- **Found during:** Task 2
- **Issue:** `replay_main.h` and `write_images.c` are written against `seeds.h`, which held only the loader's seeds.
- **Fix:** `run_seeds.c` implements `seeds.h` for the run target. The replay and a `muhtari-test-run-images` build of `write_images.c` link it. The fuzz preset gains `fuzz.run.seeds`, a test the plan did not name, to write libFuzzer's seed directory. An empty input is the 17th seed and covers the empty edge.
- **Files modified:** tests/fuzz/run_seeds.c, tests/fuzz/tests.cmake, tests/inventory/fuzz.txt, tests/cart/write_images.c
- **Commit:** 0323ffe

**5. [Rule 3 - Blocking] 10,000 cases do not fit the 60 s test timeout**
- **Found during:** Task 3
- **Issue:** Locally, 10,000 cases take 33 to 68 s per property. Under the plain `cmake --workflow --preset ci`, the job would time out.
- **Fix:** The prop job configures `ci` with `MUHTARI_TIMEOUT_SCALE=10`, builds it and runs `ctest -L prop -j 4`. It does not run the whole ci workflow, then checks JUnit with `-DLABELS=prop`. Hosted, the slowest test took 188 s against a 600 s limit.
- **Files modified:** .github/workflows/nightly.yml
- **Commit:** 5d02e56, c2d3ce0

**6. [Rule 1 - Docs] The acceptance greps needed literal names**
- **Found during:** Tasks 1 and 2
- **Issue:** Two greps expected literal text: `grep 'MUHTARI_PROP_CASES\|MUHTARI_PROP_SEED' tests/prop.h` had to print exactly two lines, and `grep 49152 tests/fuzz/run.c` had to find the size.
- **Fix:** The env names are two macros. run.c's `_Static_assert` ties `RUN_CART_BYTES` to `-max_len=49152`.
- **Commit:** 21895d5, 0323ffe

**Total deviations:** 6 auto-fixed (3 bugs in the planned design found by running it, 2 blocking, 1 docs). **Impact:** every planned truth holds. Without deviation 1, the fuzz target and most of the pool would have tested nothing.

## Evidence

- **Local:** `cmake --workflow --preset ci` exited 0 (585 tests and 6 release tests). `ctest --test-dir build/ci -L '^(prop|fuzz)$' --output-on-failure` exited 0 (10 tests). `cmake --workflow --preset hygiene` exited 0 (24 tests). `scripts/hygiene.sh --tree` exited 0. `cmake -DDIR=.github/workflows -P tests/cmake/timeouts.cmake` exited 0 (25 timeouts).
- **Acceptance:**
  - `wc -l < tests/prop.h` printed 80.
  - The `MUHTARI_PROP_*` grep printed 2 lines, and `grep -rn getenv src/` printed nothing.
  - `ctest -N -L '^prop$' | grep -c 'prop\.'` printed 4.
  - `fuzz.run` is in fuzz.txt. `fuzz.regress.run` and its self test are in ci.txt with the labels `fuzz,portable`.
  - The nightly grep printed 4.
  - `WR-01 folded in phase 3` appears in the todo once.
- **Negative checks, run once and reverted:**
  - Calling `muhtari__ports_latch` from `muhtari_set_input` made `prop.input_at_vblank` fail (exit 1, seed 20261020 case 0 size 1).
  - With `RUN_BOUND` set to 1, the replay aborted (exit 134).
  - A forced failure at three or more frames shrank to size 4 and printed the rerun line.
  - `MUHTARI_PROP_CASES=x` exited 2.
- **Hosted:**
  - CI run https://github.com/szTheory/muhtari/actions/runs/39377950148 on `5d02e56` concluded success on every job. That covers all six build legs, asan, be32 and wasm32. The `prop.*` and `fuzz.regress.run*` tests passed on all of them; on be32 the props took 7 to 15 s.
  - Nightly run https://github.com/szTheory/muhtari/actions/runs/39377950042 on the same commit (pull_request, triggered by the new paths) concluded success. In the `fuzz` job, `fuzz.run` and `fuzz.loader` each ran 61 s, and check_junit found all 6 tests. In the `prop` job, 10,000 cases per property took 112 to 188 s, and check_junit found all 4.
  - `c2d3ce0` changes one comment in nightly.yml and goes out with the next push.

## Notes for 03-19

- The be32 job took 3 m 04 s on this run, up from 2 m 43 s in 03-15. The props add about 45 s under qemu.
- New provisional limits:
  - TIMEOUT 60 on the four `prop.*` tests and on `fuzz.regress.run`;
  - TIMEOUT 123 on `fuzz.run` and 120 on its fuzz-preset `fuzz.regress.run`;
  - `timeout-minutes` 8 on the nightly fuzz job and 20 on the prop job.

## Known Stubs

None.

## Issues Encountered

None beyond the deviations above.

## Next Phase Readiness

Ready for 03-18 (the library job). GAME-06 stays open until that job lands. The prop job joins the report job's needs after 10 green runs on main.

## Self-Check: PASSED

- FOUND: tests/prop.h, tests/prop/test_prop.c, tests/prop/tests.cmake, tests/fuzz/run.c, tests/fuzz/run_cart.h, tests/fuzz/run_seeds.c
- FOUND: 21895d5, 0323ffe, 5d02e56, c2d3ce0 (ancestors of HEAD)
