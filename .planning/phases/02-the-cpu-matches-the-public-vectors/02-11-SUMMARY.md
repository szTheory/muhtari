---
phase: 02-the-cpu-matches-the-public-vectors
plan: 11
subsystem: cart
tags: [loader, allocator, libfuzzer, asan, ubsan, replay, presets, cmake]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 10's table-driven muhtari__cart_identify, tests/cart/images.{c,h} and the test list linked into muhtari-test-cart
provides:
  - src/cart.c muhtari__cart_load and struct muhtari__cart in src/internal.h; muhtari_destroy frees the ROM
  - tests/alloc.h, the counting allocator with a fail-from switch, used by test_core.c, test_cart.c and the fuzz target
  - cart.load.no_alloc_on_reject
  - tests/fuzz/{loader.c,seeds.c,seeds.h,replay_main.h,tests.cmake}; fuzz.regress.loader and its self test on the ci and wasm32 lanes
  - option MUHTARI_FUZZ, tests/inventory/fuzz.txt and the fuzz configure, build, test and workflow presets
  - muhtari-test-images DIR (tests/cart/write_images.c)
affects: [plan 12's runner tests on the written images, the nightly loader-fuzz job, phase 3's muhtari_load]

actuals:
  tokens: 11739
  tasks: 3
  commits: 3
plan_head_before: 353608b1e1168bc35ebf16d44b4e9f7ec1b2851f
plan_head_after: 3406fdf1dad883d545ad936118bca905ae7bf104

tech-stack:
  added: [libFuzzer (clang-21, apt, fuzz preset only)]
  patterns:
    - "A loader identifies before it allocates: the refusal path returns before the allocator is called"
    - "One configuration flag (MUHTARI_FUZZ) swaps the test set: the fuzz lane and the ci lanes register disjoint names from one tests.cmake"
    - "libFuzzer's writable corpus is a build directory listed first; tracked inputs come after it, read only"

key-files:
  created:
    - tests/alloc.h
    - tests/fuzz/loader.c
    - tests/fuzz/seeds.c
    - tests/fuzz/seeds.h
    - tests/fuzz/replay_main.h
    - tests/fuzz/tests.cmake
    - tests/cart/write_images.c
    - tests/inventory/fuzz.txt
  modified:
    - src/cart.c
    - src/internal.h
    - src/instance.c
    - CMakeLists.txt
    - CMakePresets.json
    - tests/tests.cmake
    - tests/core/test_core.c
    - tests/cart/test_cart.c
    - tests/cart/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/cartridge.md
    - docs/testing.md

key-decisions:
  - "struct muhtari__cart holds the ROM pointer and the loaded image's muhtari_cart_info; the ROM's size is info.rom_size, so there is no separate size field"
  - "The replay takes files, not directories (C17 has no directory listing); tests/fuzz/tests.cmake globs tests/fuzz/loader/regress at configure time with CONFIGURE_DEPENDS"
  - "The fuzz preset builds every object with -fsanitize=address,undefined,fuzzer-no-link, so the core's branches are coverage-instrumented; only muhtari-fuzz-loader adds -fsanitize=fuzzer. It builds no libretro core"
  - "In the fuzz configuration fuzz.regress.loader replays the in-memory seeds and the regressions under the same sanitizers; libFuzzer itself covers the generated corpus"
  - "The fuzz target also loads every input through an allocator that fails after create, and loads an accepted image twice, so the ERR_NO_MEMORY path and the replace path are fuzzed too"

patterns-established:
  - "tests/alloc.h: alloc_config(&counts) gives a zeroed config counting into counts; fail_from = allocs + 1 fails every later call"

requirements-completed: []

coverage:
  - id: D1
    description: "muhtari__cart_load identifies with the bundled tables first and returns muhtari_probe's status; a refusal touches nothing, OK copies the mapped bytes into one allocation of rom_size bytes and replaces any earlier ROM, ERR_NO_MEMORY leaves the instance unchanged, and muhtari_destroy frees the ROM"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^fuzz\\.regress\\.loader' --output-on-failure (2 passed; fuzz.regress: 21 inputs)"
        status: pass
      - kind: other
        ref: "mutant: the load writes the instance on refusal; muhtari-fuzz-loader-replay --seeds exits 134 (abort); cart.c restored, cmp exit 0"
        status: pass
      - kind: other
        ref: "grep -n 'muhtari__cart_load\\|muhtari_load' include/muhtari.h (exit 1, no line)"
        status: pass
    human_judgment: false
  - id: D2
    description: "cart.load.no_alloc_on_reject: for 9 images across the 2600, unsupported-board, malformed, truncated and oversized classes the status is probe's and the class's, no allocator call is made, the instance is byte-identical, and an allocator failing every call changes no status; an accepted linear image makes one allocation of rom_size bytes and destroy frees it, and with the failing allocator it gives ERR_NO_MEMORY with the instance unchanged"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "tests/cart/test_cart.c#load.no_alloc_on_reject"
        status: pass
      - kind: other
        ref: "mutant: one 16-byte allocation before identification; the case fails (c.allocs == created.allocs failed: 2 != 1; 2600, failing: expected status 8, got 3); cart.c restored, cmp exit 0"
        status: pass
      - kind: other
        ref: "mutant: identification writes m->cart.info; the case fails (2600: the instance changed); cart.c restored, cmp exit 0"
        status: pass
    human_judgment: false
  - id: D3
    description: "The counting allocator lives in tests/alloc.h as static inline functions with a fail-from switch; test_core.c's create.allocator and no_alloc_after_load use it unchanged, as do test_cart.c and tests/fuzz/loader.c"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^(cart\\.load\\.no_alloc_on_reject|core\\.)' --output-on-failure (12 passed)"
        status: pass
      - kind: other
        ref: "grep -n 'count_alloc' tests/core/test_core.c prints only lines 471 and 538, both call sites"
        status: pass
    human_judgment: false
  - id: D4
    description: "tests/fuzz/loader.c probes twice and compares, compares probe with the load, checks the counting allocator and the instance bytes, destroys, and calls abort() on any broken invariant; tests/fuzz/replay_main.h is seeded from nesturbator with the MUHTARI_ prefix, a --seeds option and the message no corpus inputs"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "fuzz.regress.loader and fuzz.regress.loader.self_test (expect_fail CODE 1, REGEX no corpus inputs) in cmake --workflow --preset ci, 388 tests passed"
        status: pass
    human_judgment: false
  - id: D5
    description: "The fuzz preset (clang-21, MUHTARI_FUZZ=ON, inventory fuzz.txt with fuzz.loader, fuzz.regress.loader, fuzz.seeds) builds the libFuzzer target with ASan and UBSan and -fno-sanitize-recover=all and runs it 60 s with -timeout=2 -max_len=1048705 -artifact_prefix=crash/, the build-tree corpus first; MUHTARI_FUZZ refuses a compiler other than Clang"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "docker run --rm --platform linux/arm64 -v \"$PWD:/src\" -v /src/build -w /src ubuntu:26.04 sh -c '<apt clang-21 lld-21 libclang-rt-21-dev> && scripts/fetch-pin.sh cmake-linux-aarch64 /opt/cmake && cmake --workflow --preset fuzz && cmake -DJUNIT=build/fuzz/junit.xml -DINVENTORY=tests/inventory/fuzz.txt -P tests/cmake/check_junit.cmake' exit 0; 3 of 3 passed; check_junit: 3 tests ran once each; build/fuzz/fuzz/crash empty"
        status: pass
      - kind: other
        ref: "cmake -S . -B <scratch> -G Ninja -DMUHTARI_FUZZ=ON with AppleClang: exit 1, fuzz: libFuzzer needs Clang, not AppleClang"
        status: pass
    human_judgment: false
  - id: D6
    description: "muhtari-test-images DIR writes the 21 seed images under stable names into a build directory; no image bytes are tracked, and every tracked path under tests/fuzz ends in .c, .h or .cmake"
    requirement: CPU-05
    verification:
      - kind: other
        ref: "build/ci/tests/muhtari-test-images <scratch dir> exit 0, 21 files; muhtari-fuzz-loader-replay over them exit 0, 21 inputs; no argument exit 2"
        status: pass
      - kind: other
        ref: "git ls-files tests/fuzz: loader.c, replay_main.h, seeds.c, seeds.h, tests.cmake"
        status: pass
    human_judgment: false
  - id: D7
    description: "The probe PR's head commit 3406fdf is green on every hosted leg, MSVC and wasm32 included"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39191834717"
        status: pass
    human_judgment: false

duration: 14min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 11: The loader validates before it allocates, fuzzed under sanitizers Summary

**`muhtari__cart_load` identifies an image exactly as `muhtari_probe` does before it calls the allocator, so a refused
file allocates nothing and leaves the instance byte-identical, shown for every refusal class. A libFuzzer target
holds probe and load to the same invariants under ASan and UBSan through a `fuzz` preset, and its replay runs on every
lane.**

## Performance

- **Duration:** 14 min
- **Started:** 2026-10-19T02:37:33Z
- **Completed:** 2026-10-19T02:52:23Z
- **Tasks:** 3
- **Files modified:** 21

## Accomplishments

- **Loader (`src/cart.c`).** `muhtari__cart_load(m, image, n)` runs identification with the bundled tables through
  `cart_identify_bundled`, which `muhtari_probe` now shares.
  - A refusal returns its status before the allocator is called.
  - On OK it allocates `rom_size` bytes, copies the mapped bytes (after the header when there is one), frees any ROM
    loaded before, and records the new one in `struct muhtari__cart`.
  - `muhtari_destroy` frees the ROM before the instance. `muhtari_run_frame` is unchanged. The public header is
    unchanged too, and docs/hardware/cartridge.md has a short Loading section.
- **Shared allocator (`tests/alloc.h`).** `count_alloc`, `count_free` and `alloc_config` are static inline, with a
  `fail_from` switch. `test_core.c` lost its own copy, and its tests are unchanged.
- **`cart.load.no_alloc_on_reject`.** It covers nine refused images:
  - 2600 by size and by test-list prefix;
  - Souper as the v4 mapper;
  - size field 0, bad magic, and a not-a-cartridge entry;
  - header only, and SuperGame over a short body;
  - the cap plus one byte.

  For each, it checks the status against probe and the class, the allocator counts, the instance bytes, and the
  status again under a failing allocator. A linear image is the positive control.
- **Fuzz target (`tests/fuzz/loader.c`).**
  - Two probes must agree, and the load must return probe's status.
  - Through the counting allocator, a refusal makes no call and leaves the instance byte-identical. An accepted image
    makes one allocation of `rom_size` bytes holding the mapped bytes, and a second load replaces it.
  - A load through an allocator that fails after create gives the refusal's status, or `ERR_NO_MEMORY` with the
    instance unchanged.
  - Destroy frees everything.
  - Any broken invariant calls `abort()`.
- **Replay.** `tests/fuzz/replay_main.h` is seeded from nesturbator. `--seeds` runs the 21 in-memory seeds from
  `tests/fuzz/seeds.c`, then each file named after it. With no input at all it prints `no corpus inputs` and exits 1.
  `fuzz.regress.loader` and its self test are in the ci and wasm32 inventories.
- **`fuzz` preset.**
  - `MUHTARI_FUZZ` requires Clang. The preset uses clang-21 and RelWithDebInfo, and builds no libretro core.
  - Every object is built with `-fsanitize=address,undefined,fuzzer-no-link -fno-sanitize-recover=all`, and only
    `muhtari-fuzz-loader` adds `-fsanitize=fuzzer`.
  - `tests/inventory/fuzz.txt` lists `fuzz.loader`, `fuzz.regress.loader` and `fuzz.seeds`.
  - `fuzz.seeds` is a fixture setup that runs `muhtari-test-images build/fuzz/fuzz/seeds`.
  - `fuzz.loader` runs in `build/fuzz/fuzz` over `corpus/` (writable, build tree), then `seeds/`, then
    `tests/fuzz/loader/regress` once that directory exists. Its timeout of 120 s is provisional.
  - docs/testing.md has the preset row and the `fuzz` label.

## Recorded one-off checks (AGENTS rule 6)

- **The fuzz run in a container, the plan's verify command**, with `ls build/fuzz/fuzz/crash` and a corpus count
  appended:
  `docker run --rm --platform linux/arm64 -v "$PWD:/src" -v /src/build -w /src ubuntu:26.04 sh -c 'apt-get update -qq && apt-get install -y -qq --no-install-recommends ca-certificates curl xz-utils git ninja-build gcc binutils clang-21 lld-21 libclang-rt-21-dev >/dev/null && scripts/fetch-pin.sh cmake-linux-aarch64 /opt/cmake && PATH=/opt/cmake/bin:$PATH cmake --workflow --preset fuzz && PATH=/opt/cmake/bin:$PATH cmake -DJUNIT=build/fuzz/junit.xml -DINVENTORY=tests/inventory/fuzz.txt -P tests/cmake/check_junit.cmake'`.
  - Exit 0. 3 of 3 tests passed, `fuzz.loader` in 61.04 s, and `check_junit: 3 tests ran once each`.
  - No crash file. 169 files in the build corpus.
- **The fuzz statistics, from a second identical run** (the build volume is anonymous, so a second run read
  `LastTest.log`). Exit 0.
  - 632 inline 8-bit counters, so the core objects are instrumented.
  - Seed corpus: 21 files, 128 B to 1,048,705 B.
  - `INITED cov: 173 ft: 231 corp: 18/3454Kb`, then `DONE cov: 245 ft: 360 corp: 85/3633Kb exec/s: 528 rss: 385Mb`.
    That is 32,253 runs in 61 s, the corpus growing from 18 to 85 units, with 173 files written to the build corpus.
  - No crash, so there is no regression input and no manifest row.
- **Mutation runs on `src/cart.c`**, each restored and checked with `cmp` (exit 0):
  - The load writes the instance on refusal: the replay aborts with exit 134, and `cart.load.no_alloc_on_reject` fails
    with `2600: the instance changed`.
  - One allocation before identification: `cart.load.no_alloc_on_reject` fails with
    `c.allocs == created.allocs failed: 2 != 1` and `2600, failing: expected status 8, got 3`.
- The `MUHTARI_FUZZ` guard: configure with AppleClang exits 1 with `fuzz: libFuzzer needs Clang, not AppleClang`.
- `muhtari-test-images` into a scratch directory: exit 0 and 21 files, which the replay runs as 21 inputs. With no
  argument it exits 2.
- `cmake --workflow --preset ci`: exit 0, 388 tests, then 4. `cmake --workflow --preset hygiene`: exit 0, 22 tests.
- **Hosted run on head `3406fdf`:** https://github.com/szTheory/muhtari/actions/runs/39191834717.
  - `gh run watch --exit-status` exited 0.
  - The conclusion was `success`, and so was every job, including build on both Windows legs, wasm32, be32, asan and
    nofp on both architectures.

## Task Commits

1. **Task 1: The loader, the replay and fuzz.regress.loader on the seeds** - `ee17c25` (feat)
2. **Task 2: The shared counting allocator and cart.load.no_alloc_on_reject** - `12bd18f` (feat)
3. **Task 3: The fuzz preset and libFuzzer target, muhtari-test-images, one local run** - `3406fdf` (feat)

## Files Created/Modified

- `src/cart.c`: `cart_identify_bundled`, `muhtari__cart_load`
- `src/internal.h`: `struct muhtari__cart`, the `cart` member, the loader's declaration
- `src/instance.c`: destroy frees the ROM
- `tests/alloc.h`: the counting allocator (new)
- `tests/fuzz/{loader.c,seeds.c,seeds.h,replay_main.h,tests.cmake}`: the fuzz target, seeds, replay and both
  configurations' tests (new)
- `tests/cart/write_images.c`: `muhtari-test-images` (new)
- `tests/inventory/fuzz.txt` (new), `tests/inventory/{ci,wasm32}.txt`: three names
- `tests/core/test_core.c`, `tests/cart/test_cart.c`, `tests/cart/tests.cmake`
- `CMakeLists.txt`: `MUHTARI_FUZZ` and its Clang check, inventory `fuzz`
- `CMakePresets.json`: `fuzz` configure, build, test and workflow presets
- `tests/tests.cmake`: the fuzz include and the fuzz lane's early return
- `docs/hardware/cartridge.md`: Loading; `docs/testing.md`: the preset row, the `fuzz` label and the inventory
  sentence

## Decisions Made

See `key-decisions` above.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] tests/alloc.h landed in Task 1**
- **Found during:** Task 1
- **Issue:** Task 1's `loader.c` loads through a counting allocator, which the plan creates in Task 2.
- **Fix:** `tests/alloc.h` was written in Task 1 for `loader.c`. Task 2 moved `test_core.c` onto it and added the cart
  case.
- **Committed in:** ee17c25

**2. [Rule 3 - Blocking] The replay gets the regression files, not their directory**
- **Found during:** Task 1
- **Issue:** The seeded replay main opens each argument as a file, and C17 has no directory listing.
- **Fix:** tests/fuzz/tests.cmake globs `tests/fuzz/loader/regress/*` with `CONFIGURE_DEPENDS` when the directory
  exists. libFuzzer still gets the directory.
- **Committed in:** ee17c25, 3406fdf

**3. [Rule 2 - Missing critical] Coverage instrumentation of the core and two more fuzz invariants**
- **Found during:** Task 1 and Task 3
- **Issue:** With `-fsanitize=fuzzer` on the target alone, libFuzzer sees only `loader.c`'s branches. The
  `ERR_NO_MEMORY` and replace paths of the load were also outside the five steps.
- **Fix:** The preset adds `fuzzer-no-link` to every object, and the loader run shows 632 counters. The target also
  loads through an allocator that fails after create, and loads an accepted image twice.
- **Committed in:** ee17c25, 3406fdf

**4. [Rule 2 - Missing critical] Docs for the loader and the preset**
- **Found during:** Task 1 and Task 3
- **Issue:** AGENTS rule 5 asks for behaviour and tests to be documented together, and neither file list named a
  doc.
- **Fix:** docs/hardware/cartridge.md has a Loading section. docs/testing.md has the `fuzz` preset row and label.
- **Committed in:** ee17c25, 3406fdf

---

**Total deviations:** 4 auto-fixed (2 Rule 3, 2 Rule 2). **Impact on plan:** none on scope.

## TDD Gate Compliance

Task 2 carries `tdd="true"`, and `workflow.tdd_mode` is false.

- **RED.** The loader is Task 1's tracer, so `cart.load.no_alloc_on_reject` passed when first run. RED was shown by
  two mutants of `muhtari__cart_load`, as listed under the one-off checks. The case failed on its planned assertions:
  the allocator counts, the failing-allocator status and the instance bytes.
- **Semantic assessment:** the target ran and failed on the counts, status and bytes it exists for, with no build,
  load or fixture fault.
- **GREEN:** with `cart.c` restored, all 388 tests pass.
- **Commits:** following plans 5 to 10, the test landed in one `feat(02-11)` commit, and there is no separate
  `test(02-11)` commit.

## Issues Encountered

None. The fuzz run found no crash.

## Known Stubs

None.

## User Setup Required

None.

## Next Phase Readiness

- Plan 12's runner tests can write their images with `muhtari-test-images DIR` (target in tests/fuzz/tests.cmake).
- The nightly `loader-fuzz` job can run `cmake --workflow --preset fuzz` as is, and upload `build/fuzz/fuzz/crash`.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files present: tests/alloc.h, tests/fuzz/loader.c, tests/fuzz/seeds.c, tests/fuzz/seeds.h,
  tests/fuzz/replay_main.h, tests/fuzz/tests.cmake, tests/cart/write_images.c and tests/inventory/fuzz.txt.
- Commits ee17c25, 12bd18f and 3406fdf are ancestors of HEAD, and `git rev-list --count 353608b..HEAD` is 3.

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
