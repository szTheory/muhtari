---
phase: 02-the-cpu-matches-the-public-vectors
plan: 13
subsystem: testing
tags: [7800basic, wasmtime, built-sample, probe, cartdb, ci, fetch-pin, windows]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 12's muhtari-run --probe with its eight hashes, and plan 10's test hash list in tests/cart/test_cartdb.c
provides:
  - pins for 7800basic-wasm 0.41 and wasmtime 48.0.6 on macOS x64, Windows x64 and Windows arm64
  - scripts/fetch-pin.sh reads a .zip with cmake -E tar
  - tests/built/build.cmake, our own six-stage 7800basic recipe, and tests/built/expected.txt
  - the built_samples fixture, built.samples and built.samples.self_test
  - muhtari-run-testdb, the runner linked with the test hash list
  - runner.probe.sample_a78, sample_headerless, sample_hash_match and cart.identify.test_list
  - the CMake option MUHTARI_TESTS (default ON); the bench preset, the perf gate and bench-wasm32 build with it OFF
affects: [plan 14's nightly fuzz job, plan 16's timeout measurement, phase 3's remaining 34 samples and their frame and audio hashes]

actuals:
  tokens: 9912
  tasks: 3
  commits: 4
plan_head_before: 9f9cc1db6a36cf8fc64ec5b396bb8b56671ae879
plan_head_after: cf79f5a84b5688688e29249713b10fb9cdce8910

tech-stack:
  added: [7800basic 0.41 WebAssembly package, run only as a tool]
  patterns:
    - "A built fixture: a CMake -P recipe builds it from pinned tools into build/<preset>/, checks its SHA-256s against a committed row, and is a FIXTURES_SETUP test that consumers require"
    - "Configure stops with the fetch commands when a test's tool is missing; MUHTARI_TESTS OFF is the only way to configure without it, and it registers no test"

key-files:
  created:
    - tests/built/build.cmake
    - tests/built/expected.txt
    - tests/built/tests.cmake
  modified:
    - pins.txt
    - scripts/fetch-pin.sh
    - CMakeLists.txt
    - CMakePresets.json
    - tests/tests.cmake
    - tests/runner/tests.cmake
    - tests/cart/tests.cmake
    - tests/cart/test_cart.c
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - .github/workflows/ci.yml
    - .github/workflows/nightly.yml
    - docs/testing.md

key-decisions:
  - "Hosted run 39193868744: the 7800basic sample builds byte-identical on all six build legs, asan, be32 and wasm32, both Windows legs included, so no leg moves to its own preset"
  - "be32 took 39 s with the sample, under the five-minute budget; no DEC.46 decision is needed for plan 16"
  - "tests/built/tests.cmake is included only when MUHTARI_INVENTORY is ci or wasm32, the two inventories that list the sample tests"
  - "fetch-pin.sh's zip case runs cmake -E tar on a relative path from inside the extraction directory, so a native Windows cmake never sees an MSYS path"

patterns-established:
  - "tests/built/build.cmake -DWASMTIME -DPKG -DOUT -DEXPECTED -DNAME: one execute_process per stage, a pipe as several COMMANDs, and the package mapped to /b"

requirements-completed: []

coverage:
  - id: D1
    description: "tests/built/build.cmake, our own recipe, builds 7800basic's simple sample in a fresh build/<preset>/built/simple/ with six wasmtime stages, the package mapped to /b, checks every stage's status and both SHA-256s against expected.txt, and fails with simple: .a78 SHA-256 drift on a wrong digest"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^built\\.samples' --output-on-failure (built.samples and built.samples.self_test, 2 of 2 passed)"
        status: pass
      - kind: other
        ref: "shasum -a 256 build/ci/built/simple/simple.bas.a78 prints d09da3ed27fe82e0a4dd2821ff2cb9bde26c9531c59890788acb1cfdc4004ecf"
        status: pass
      - kind: other
        ref: "cmake -DWASMTIME=<x86_64-macos wasmtime under Rosetta> ... -P tests/built/build.cmake (exit 0, both digests as expected)"
        status: pass
    human_judgment: false
  - id: D2
    description: "The sample's bytes are identical on every hosted leg that runs the ci or wasm32 inventory: linux x64 and arm64, macOS arm64 and x64, Windows x64 and arm64, asan, be32 and wasm32"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39193868744"
        status: pass
    human_judgment: false
  - id: D3
    description: "The shipped runner's --probe on the headered sample prints source header, board linear, region ntsc, ProLine on both ports, a78_version 0x04, title My Game, no warning line and all eight hashes"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#probe.sample_a78 (runner.probe.sample_a78)"
        status: pass
    human_judgment: false
  - id: D4
    description: "The shipped runner identifies the stripped sample by its footer ($FFF8 = $FF, $FFF9 = $F7, reset $F4AD) as source footer, board linear, with sha1 1aa9559b74eab35d4c5443b56bc4dfd7a445ac4a, md5 948e2565fefcbb635cd8b73ea9b29941 and crc32 55f0312b"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#probe.sample_headerless (runner.probe.sample_headerless)"
        status: pass
    human_judgment: false
  - id: D5
    description: "muhtari-run-testdb, the runner linked with tests/cart/test_cartdb.c in place of src/cartdb.c and never installed, prints source hash, board linear and region ntsc for the stripped sample; the test-list case cart.identify.test_list resolves the same bytes to linear, NTSC, source hash"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#probe.sample_hash_match (runner.probe.sample_hash_match)"
        status: pass
      - kind: unit
        ref: "tests/cart/test_cart.c#identify.test_list (cart.identify.test_list)"
        status: pass
      - kind: other
        ref: "grep -n 'install(' tests/runner/tests.cmake (no line)"
        status: pass
    human_judgment: false
  - id: D6
    description: "Configure stops naming both fetch commands when wasmtime or the 7800basic package is missing; every leg that lists the sample tests fetches both first, and the perf gate, the nightly bench and bench-wasm32 configure with MUHTARI_TESTS OFF, which registers no test"
    requirement: CPU-05
    verification:
      - kind: other
        ref: "rm -rf build/bench && cmake --workflow --preset bench && test ! -e build/bench/CTestTestfile.cmake (exit 0)"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39193868845"
        status: pass
    human_judgment: false
  - id: D7
    description: "fetch-pin.sh unpacks the Windows wasmtime zips with cmake -E tar, requiring one top-level directory"
    requirement: CPU-05
    verification:
      - kind: other
        ref: "scripts/fetch-pin.sh wasmtime-x86_64-windows <dir> and wasmtime-aarch64-windows <dir> (exit 0, wasmtime.exe at the top)"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39193868744 (both Windows build legs fetch and run built.samples)"
        status: pass
    human_judgment: false

duration: 10min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 13: The 7800basic sample through the probe Summary

**Our own CMake recipe builds 7800basic 0.41's `simple` sample from pinned tools on all nine hosted legs, and the
bytes match everywhere. `--probe` then reads the real image three ways: headered, stripped by its footer, and
stripped through the test hash list in `muhtari-run-testdb`.**

## Performance

- **Duration:** 10 min
- **Started:** 2026-10-19T03:08:53Z
- **Completed:** 2026-10-19T03:18:33Z
- **Tasks:** 3
- **Files modified:** 16

## Accomplishments

- **Pins and fetch.** `pins.txt` gains `7800basic-wasm`, `wasmtime-x86_64-macos`, `wasmtime-x86_64-windows` and
  `wasmtime-aarch64-windows`, with a note that the compiler is GPL-2 and runs only as a tool. `fetch-pin.sh` gets a
  `*.zip)` case. It extracts with `cmake -E tar xf` inside `$tmp/x` and finds the single top directory with `find`,
  since globbing is off.
- **Our own recipe (`tests/built/build.cmake`).** It recreates `OUT/NAME` and copies the sample from the package.
  Then it runs six stages, one `execute_process` each: preprocess, compile (`-i /b`), postprocess piped to optimize,
  dasm (`-I/b/includes -f3 -p20`) piped to 7800filter, 7800sign (`-w`) and 7800header (`-o -f a78info.cfg`). Each
  stage runs as `wasmtime run --dir=. --dir=<pkg>::/b`. Any non-zero status fails the build, naming the stage. Last,
  it compares both SHA-256s with `expected.txt`.
- **Tests.**
  - `built.samples` builds the sample and is the `built_samples` fixture.
  - `built.samples.self_test` changes one digit of the `.a78` digest and must fail with `simple: .a78 SHA-256 drift`.
  - Three `runner.probe.sample_*` cases read the built sample.
  - `cart.identify.test_list FILE` probes the `.bin` from the build tree through the test list.
- **`muhtari-run-testdb`.** It links the runner's sources (read from `muhtari-run`'s `SOURCES`), the core's
  `muhtari_objs` and `muhtari_cpu` objects, `tests/cart/test_cartdb.c` and `muhtari_host`. It is never installed.
- **`MUHTARI_TESTS`.** This option guards `enable_testing()` and both test includes. The `bench` preset sets it to
  OFF. ci.yml's `perf` helper and nightly `bench-wasm32` pass `-DMUHTARI_TESTS=OFF`.
- **CI.** Each build row has a `wasmtime:` key, and a "Fetch wasmtime and 7800basic" step runs before the workflow.
  asan and be32 fetch both tools, and wasm32 adds the 7800basic line. nofp and hygiene fetch nothing.

## Hosted evidence (AGENTS rules 6 and 10)

**ci.yml on head `cf79f5a`:** https://github.com/szTheory/muhtari/actions/runs/39193868744. `gh run watch
--exit-status` exited 0, and all 18 jobs passed. The plan's filter over the windows, macos-x64, be32, asan, wasm32
and perf jobs printed `["success"]`.

| Leg | `built.samples` | Fetch step | Job wall time |
|---|---|---|---|
| build (linux-x64) | passed, 0.59 s | 1 s | 17 s |
| build (linux-arm64) | passed, 0.55 s | 1 s | 18 s |
| build (macos-arm64) | passed, 0.52 s | 2 s | 22 s |
| build (macos-x64) | passed, 0.67 s | 2 s | 33 s |
| build (windows-x64) | passed, 0.80 s | 2 s | 32 s |
| build (windows-arm64) | passed, 0.74 s | 4 s | 56 s |
| asan | passed, 1.14 s | 1 s | 22 s |
| be32 | passed, 1.12 s | 1 s | **39 s** (budget 5 min) |
| wasm32 | passed, 1.12 s | 6 s (with the apt install) | 19 s |

On every leg, the five sample tests also passed: `built.samples.self_test`, the three `runner.probe.sample_*` cases
and `cart.identify.test_list`. Research risk R1 is settled: both Windows legs and macOS x64 reproduce the bytes, so
D-22's "would change if" does not apply. Risk R2 is settled: be32 takes 39 s, well under five minutes, so plan 16
needs no DEC.46 decision.

**nightly.yml for the same commit:** https://github.com/szTheory/muhtari/actions/runs/39193868845. The run concluded
`success`. All three `bench` jobs passed with the `bench` preset (OFF) and `bench-wasm32` passed with
`-DMUHTARI_TESTS=OFF`. `report` was skipped, as it is on every pull request.

## Recorded one-off checks

- `cmake --workflow --preset ci`: exit 0, 405 tests, then 4.
- `cmake --workflow --preset hygiene`: exit 0, 22 tests, `policy.pins` among them.
- `rm -rf build/bench && cmake --workflow --preset bench && test ! -e build/bench/CTestTestfile.cmake`: exit 0.
- `git status --porcelain --ignored build/ci/built` lists only `!! build/`, so the built bytes are ignored.
- The recipe under the x86_64-macos wasmtime (Rosetta, scratch directory): exit 0, both digests as expected, 3.1 s.
- `scripts/fetch-pin.sh` on each of the four new rows: exit 0. Both zips unpack to `wasmtime.exe` at the top.
- `actionlint` on ci.yml and nightly.yml finds only things that were already there: the `ubuntu-26.04` labels and
  two shellcheck infos in the retroarch job.

## Task Commits

1. **Task 1: pins, our own recipe, digests, built.samples** - `4f6b8e0` (feat)
2. **Task 2 RED: the sample probe tests, hash match failing on the shipped runner** - `ae01b10` (test)
3. **Task 2 GREEN: muhtari-run-testdb, docs** - `da0985f` (feat)
4. **Task 3: zip case, fetch steps, MUHTARI_TESTS OFF in perf and bench-wasm32** - `cf79f5a` (ci)

## Files Created/Modified

- `tests/built/{build.cmake,expected.txt,tests.cmake}`: the recipe, the row, the two tests (new)
- `pins.txt`: four rows
- `scripts/fetch-pin.sh`: the zip case
- `CMakeLists.txt`, `CMakePresets.json`: `MUHTARI_TESTS`, the bench preset
- `tests/tests.cmake`: includes `built/` for the ci and wasm32 inventories
- `tests/runner/tests.cmake`: `muhtari-run-testdb`, the three sample cases
- `tests/cart/{tests.cmake,test_cart.c}`: `identify.test_list FILE`
- `tests/inventory/{ci,wasm32}.txt`: six names each
- `.github/workflows/{ci,nightly}.yml`: the fetch steps, the option, provisional timeouts
- `docs/testing.md`: the fetch commands, `MUHTARI_TESTS`, the `built` label, its self test, the zip case and the job rows

## Decisions Made

See `key-decisions` above.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 2 - Missing critical] docs/testing.md**
- **Found during:** Tasks 2 and 3
- **Issue:** AGENTS rule 5 requires docs with each behaviour change, and D-22 names docs/testing.md for the built
  sample. No task listed the file.
- **Fix:** The doc now covers the two fetch commands and `MUHTARI_TESTS` under the entry point, the `built` label
  row, the `cart` row, `built.samples.self_test`, the zip case, and the container recipe's fetch line. The job rows
  for build, asan, be32, wasm32 and perf are updated too.
- **Committed in:** da0985f, cf79f5a

**2. [Rule 1 - Bug] The wasm module path**
- **Found during:** Task 1
- **Issue:** The first draft passed `/b/<tool>.wasm` as the module. wasmtime takes the module as a host path, and
  `/b` exists only inside the guest.
- **Fix:** The module is `<pkg>/<tool>.wasm`, and only the tools' arguments use `/b`.
- **Committed in:** 4f6b8e0

**3. [Rule 3 - Blocking] asan timeout**
- **Found during:** Task 3
- **Issue:** asan had 2 minutes, twice its slowest 55 s, and the fetch and the sample add to that.
- **Fix:** It now has 3 minutes, marked `# provisional` with the old measurement kept. The build, be32 and wasm32
  comments are marked provisional too. Plan 16 measures them.
- **Committed in:** cf79f5a

---

**Total deviations:** 3 auto-fixed (1 Rule 1, 1 Rule 2, 1 Rule 3). **Impact on plan:** none on scope.

- No code comment carries a planning ID. The footer comment in tests/runner/tests.cmake cites
  docs/hardware/cartridge.md, "The footer test", for $FFF8 and $FFF9, and gives the vectors $F000, $F4AD and $F05F.
- The recipe copies every file in `samples/simple/`, its Makefile included. The stages never read the Makefile.

## TDD Gate Compliance

Task 2 carries `tdd="true"`, and `workflow.tdd_mode` is false.

- **RED (`ae01b10`).** `runner.probe.sample_hash_match` was registered against the shipped `muhtari-run`. It failed
  on its planned assertion: `--probe simple.bas.bin: source is 'footer', not 'hash'`. The shipped runner holds no
  test-list entry, which is exactly why `muhtari-run-testdb` exists. There was no build, load or fixture fault.
- The other three sample tests passed at RED: `sample_a78`, `sample_headerless` and `cart.identify.test_list`. The
  probe (plan 12) and the test list's sample entry (plan 10) already existed. This plan adds the evidence on a real
  image, not new identification code.
- **Semantic assessment:** the target test ran and failed on the source line it exists to check.
- **GREEN (`da0985f`):** all four pass with `built.samples` as their fixture, and so does the whole ci workflow (405
  tests).
- The ctest console output is not a format the RED-evidence classifier reads, so the evidence is recorded here, as in
  plans 5 to 12.

## Issues Encountered

None.

## Known Stubs

None. The frame and audio columns of `expected.txt` are `-` by design (D-22): phase 3 fills them in.

## User Setup Required

None.

## Next Phase Readiness

- Plan 14 adds the nightly fuzz job that CPU-05 still needs. Built bytes stay out of fuzz seeds (DEC.19).
- Plan 16 measures the provisional timeouts, now with the sample in place.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files present: tests/built/build.cmake, tests/built/expected.txt and tests/built/tests.cmake.
- Commits 4f6b8e0, ae01b10, da0985f and cf79f5a are ancestors of HEAD, and `git rev-list --count 9f9cc1d..HEAD` is 4.

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
