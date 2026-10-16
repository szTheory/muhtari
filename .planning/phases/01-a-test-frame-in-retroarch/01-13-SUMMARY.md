---
phase: 01-a-test-frame-in-retroarch
plan: 13
subsystem: testing
tags: [cmake, presets, qemu, powerpc, wasm32, wasi, wasmtime, hashes]

requires:
  - phase: 01-12
    provides: pins.txt with wasmtime and CMake rows, fetch-pin.sh, the ubuntu:26.04 container recipe
provides:
  - tests/cmake/hashes.cmake (write hashes.txt; compare exactly N byte-identical files)
  - runner.hashes_file and two comparison self tests
  - be32 lane (32-bit big-endian PowerPC under qemu-ppc)
  - wasm32 lane (clang-21 wasm32-wasi under wasmtime 48.0.6)
  - emulator passed to every script that starts muhtari-run
  - docs/testing.md Lanes section
affects: [ci.yml hash-equality job, be32 and wasm32 CI jobs, phase close timeout scales]

actuals:
  tokens: 6200
  tasks: 3
  commits: 3
plan_head_before: 8f8b6b778242e87798decaa59fc6f559d9ab9afc
plan_head_after: 7c4c5ebbed46a5655d252d282d64f0852d7e0359

tech-stack:
  added: [gcc-powerpc-linux-gnu 15.2, qemu-user 10.2.1, clang-21 wasm32-wasi, wasi-libc, wasmtime 48.0.6]
  patterns:
    - "Runner-starting scripts take -DEMULATOR= from CMAKE_CROSSCOMPILING_EMULATOR, passed with $<SEMICOLON>"
    - "A list inside a nested command list is passed with an escaped semicolon (\\;)"
    - "Lane-specific inventories: a lane without a component registers none of its tests"

key-files:
  created:
    - tests/cmake/hashes.cmake
    - cmake/toolchains/ppc-linux-gnu.cmake
    - cmake/toolchains/wasm32-wasi.cmake
    - tests/inventory/wasm32.txt
  modified:
    - tests/runner/tests.cmake
    - tests/cmake/exit_table.cmake
    - tests/inventory/ci.txt
    - tests/tests.cmake
    - CMakePresets.json
    - CMakeLists.txt
    - docs/testing.md

key-decisions:
  - "hashes.txt is written at a fixed 600 frames on every lane, independent of MUHTARI_SHORT_RUNS, so all lanes write the same bytes"
  - "runner.stop stays portable on wasm32: raise(SIGINT) reaches the handler under wasmtime 48.0.6 with wasi-emulated-signal"
  - "The libretro tests register only when MUHTARI_BUILD_LIBRETRO is on, so wasm32.txt is the ci list minus libretro.*"
  - "The clang-21 wasm32-wasi triple worked without warnings; the wasi-sdk fallback was not needed"
  - "docs/testing.md's Presets table became the Lanes section, so the presets are described once"

patterns-established:
  - "Emulated lane: toolchain file sets CMAKE_CROSSCOMPILING_EMULATOR; test preset runs label portable"

requirements-completed: [FRAME-01, FRAME-03]

coverage:
  - id: D1
    description: "runner.hashes_file writes build/<preset>/hashes.txt with frame1, frame600 and png SHA-256 lines"
    requirement: FRAME-03
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#runner.hashes_file (cmake --workflow --preset ci, exit 0, 56/56)"
        status: pass
    human_judgment: false
  - id: D2
    description: "hashes.cmake compare passes only for exactly COUNT existing byte-identical files; a wrong count, one differing byte or a missing file fails"
    requirement: FRAME-03
    verification:
      - kind: unit
        ref: "tests/runner/tests.cmake#runner.hashes_compare.count.self_test"
        status: pass
      - kind: unit
        ref: "tests/runner/tests.cmake#runner.hashes_compare.byte.self_test"
        status: pass
      - kind: other
        ref: "cmake -DFILES=build/ci/hashes.txt;<missing> -DCOUNT=2 -DMODE=compare -P tests/cmake/hashes.cmake (exit 1)"
        status: pass
    human_judgment: false
  - id: D3
    description: "be32 workflow passes label portable under qemu-ppc in ubuntu:26.04, libretro host included, with the Mac's hashes"
    requirement: FRAME-01
    verification:
      - kind: e2e
        ref: "task 1 verify command (Mac ci, be32 container, hashes compare COUNT 2), exit 0, 30/30"
        status: pass
    human_judgment: false
  - id: D4
    description: "wasm32 workflow passes label portable under wasmtime 48.0.6 with clang-21 wasm32-wasi, -mcpu=lime1 and --stack-first"
    requirement: FRAME-03
    verification:
      - kind: e2e
        ref: "task 2 verify command (wasm32 container, hashes compare COUNT 2), exit 0, 28/28"
        status: pass
    human_judgment: false
  - id: D5
    description: "Mac ci, Linux arm64 ci, be32 and wasm32 hashes.txt are byte-identical"
    requirement: FRAME-03
    verification:
      - kind: e2e
        ref: "cmake -DFILES=<mac-ci>;<linux>;<be32>;<wasm32> -DCOUNT=4 -DMODE=compare -P tests/cmake/hashes.cmake (exit 0)"
        status: pass
    human_judgment: false
  - id: D6
    description: "Every runner-starting script gets CMAKE_CROSSCOMPILING_EMULATOR, so emulated lanes run the guest binary"
    requirement: FRAME-01
    verification:
      - kind: other
        ref: "ctest -N -V in build/be32 and build/wasm32 shows qemu-ppc and wasmtime run in the test commands and -DEMULATOR in the runner scripts; file reports ELF 32-bit MSB PowerPC and WebAssembly"
        status: pass
    human_judgment: false
  - id: D7
    description: "docs/testing.md Lanes section lists the eight presets, inventories, labels, the hashes comparison and the provisional timeout scales"
    verification:
      - kind: other
        ref: "grep -c 'be32\\|wasm32\\|hashes.txt' docs/testing.md (17); docs.budgets passes"
        status: pass
    human_judgment: false
  - id: D8
    description: "The hosted be32 and wasm32 jobs and the hash-equality job agree on GitHub runners"
    requirement: FRAME-03
    verification:
      - kind: e2e
        ref: "plan 01-14 CI run (hash-equality, be32, wasm32 jobs)"
        status: unknown
    human_judgment: false

duration: 10min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 13: be32 and wasm32 lanes with four-lane hash equality Summary

**The `be32` (qemu-ppc) and `wasm32` (wasmtime 48.0.6) lanes, plus a `hashes.txt` written by every lane and an exact-count comparison, prove that the test card's frame and PNG hashes match byte for byte on the Mac, Linux arm64, big-endian 32-bit PowerPC and wasm32.**

## Performance

- **Duration:** about 10 min
- **Started:** 2026-10-16T16:28:00Z
- **Completed:** 2026-10-16T16:38:29Z
- **Tasks:** 3
- **Files modified:** 11 (320 insertions, 20 deletions)

## Accomplishments

- `tests/cmake/hashes.cmake` (45 lines) has two modes:
  - `write` runs the runner through the lane's emulator and writes `frame1`, `frame600` and `png` lines.
  - `compare` fails unless it gets exactly `COUNT` existing, byte-identical files. It names the first file that differs.
- `runner.hashes_file` (labels `portable` and `runner`) is registered, along with two self tests for the comparison: a wrong count, and one differing byte.
- `tests/runner/tests.cmake` and `exit_table.cmake` take `-DEMULATOR=`. The registration passes `CMAKE_CROSSCOMPILING_EMULATOR`.
- The `be32` lane: `cmake/toolchains/ppc-linux-gnu.cmake` plus configure, build, test and workflow presets. All 30 `portable` tests pass under `qemu-ppc`, including the in-process libretro host, which loads the PowerPC `.so`. No endian, ILP32 or unsigned-char bug turned up.
- The `wasm32` lane: `cmake/toolchains/wasm32-wasi.cmake` and its presets. It uses one `--dir=${sourceDir}::${sourceDir}` preopen and builds no libretro core. Configure names the `fetch-pin.sh` command when the runtime is missing. All 28 `portable` tests pass under wasmtime 48.0.6.
- Four-lane equality, with the same values on all four lanes:
  - `frame1` and `frame600`: `9eb011e2…05c4`
  - `png`: `1c0b16cb…c956`
- `docs/testing.md` has a Lanes section covering the eight presets.

## Measured facts

| Lane | Container wall (whole `docker run`) | CTest test step | Tests |
|---|---|---|---|
| Linux arm64 `ci` | 23 s | 0.20 s, then 0.01 s release | 56, then 4 |
| `be32` (arm64 host, qemu-ppc 10.2.1) | 25–27 s | 1.02–1.08 s | 30 portable |
| `wasm32` (arm64 host, wasmtime 48.0.6) | 26 s | 0.28 s | 28 portable |

- **Timeout scales.** Both lanes finish far inside their provisional scales (10 and 5). They stay provisional until hosted x64 runs are measured.
- **runner.stop.** It stayed `portable` on wasm32: `raise(SIGINT)` reaches the handler under wasmtime with `-lwasi-emulated-signal`.
- **wasm32 build.** Clang 21.1.8 accepted `wasm32-wasi` with `-mcpu=lime1` and gave no warning, so the wasi-sdk fallback was not needed. `muhtari-run` builds as a WebAssembly module with no `.wasm` suffix.
- **Stack.** No stack-size trap: no local of 64 KiB or more is reached.

## Gate-code tally

- **New gate code:**
  - `tests/cmake/hashes.cmake`: 45 lines
  - registration in `tests/runner/tests.cmake`: about 25 lines
  - the two toolchain files: 34 lines (configuration)
- **All gate scripts:**
  - `tests/cmake/*.cmake` plus `cmake/MuhtariTests.cmake`: 974 lines
  - `scripts/*.sh`: 335 lines
- **Code under test:** `src/` is 315 lines. The tenth-of-`src/` ceiling applies from phase 3.

## Evidence (command, exit status)

- **Task 1.** The verify command, run verbatim: Mac `cmake --workflow --preset ci`, then the `be32` container workflow, then `hashes.cmake` compare with COUNT 2. Exit 0. Mac 56/56 then 4/4; `be32` 30/30; "2 identical files".
- **Task 2.** The verify command, run verbatim: the `wasm32` container workflow, then compare with COUNT 2. Exit 0. 28/28; "2 identical files".
- **Task 3, four-lane compare.** `cmake "-DFILES=<mac-ci>/hashes.txt;<linux>/hashes.txt;<be32>/hashes.txt;<wasm32>/hashes.txt" -DCOUNT=4 -DMODE=compare -P tests/cmake/hashes.cmake`, exit 0, "4 identical files". All four files were produced on HEAD `14d8a4e`.
- **Task 3, docs.** `grep -c 'be32\|wasm32\|hashes.txt' docs/testing.md` gave 17, exit 0.
- **Compare mode by hand.** Each failing case exits 1:
  - COUNT 3 with two files
  - one byte changed in a copy
  - a missing file
- **Hygiene.** `cmake --workflow --preset hygiene`: exit 0, 7/7.
- **Emulators in use.** `ctest -N -V` in the containers shows `/usr/bin/qemu-ppc -L /usr/powerpc-linux-gnu …` and `wasmtime run --dir=/src::/src …` in the test commands, and `-DEMULATOR=…` in the runner scripts.

## Task Commits

1. **Task 1: End-to-end be32: hashes.txt, ppc toolchain, qemu, compare with the Mac** - `632ff34` (ci)
2. **Task 2: The wasm32 lane: toolchain, preset, inventory, run under wasmtime** - `14d8a4e` (ci)
3. **Task 3: Four-lane local hash equality and the Lanes section of docs/testing.md** - `7c4c5eb` (docs)

## Files Created/Modified

- `tests/cmake/hashes.cmake`: writes `hashes.txt`; compares N files
- `cmake/toolchains/ppc-linux-gnu.cmake`: be32 cross toolchain with the qemu-ppc emulator
- `cmake/toolchains/wasm32-wasi.cmake`: the wasm32-wasi toolchain with the locked flags
- `tests/inventory/wasm32.txt`: the ci list without `libretro.*`
- `tests/runner/tests.cmake`: the emulator parameter, `runner.hashes_file` and the two compare self tests
- `tests/cmake/exit_table.cmake`: the emulator parameter
- `tests/inventory/ci.txt`: the three new tests
- `tests/tests.cmake`: libretro tests only with the libretro core
- `CMakePresets.json`: `be32` and `wasm32` configure, build, test and workflow presets
- `CMakeLists.txt`: the missing-runtime check on WASI
- `docs/testing.md`: the Lanes section, the comparison, the self-test rows and the container packages

## Decisions Made

See `key-decisions` in the frontmatter.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 2 - Missing critical] Self tests for the comparison**
- **Found during:** Task 1
- **Issue:** The compare mode is a gate, and the project rule is that each gate has a negative control that CI runs. The plan checked its failure cases only by hand.
- **Fix:** Added `runner.hashes_compare.count.self_test` and `runner.hashes_compare.byte.self_test` through `expect_fail.cmake`, with label `runner`, to `ci.txt` and `wasm32.txt`. The two-file list in the nested command is passed with an escaped `\;`. Both regexes tolerate CMake's message wrapping, which split "differs from" during the first run.
- **Files modified:** `tests/runner/tests.cmake`, `tests/inventory/ci.txt`
- **Committed in:** 632ff34

**2. [Rule 3 - Blocking] libretro tests on a build with no libretro core**
- **Found during:** Task 2
- **Issue:** `libretro.vendored` and the rest of `tests/libretro/tests.cmake` were registered even with `MUHTARI_BUILD_LIBRETRO=OFF`, so `wasm32.txt` could not leave out every `libretro.` name.
- **Fix:** `tests/tests.cmake` includes the libretro tests only when the core is built. The file was not in the plan's list.
- **Committed in:** 14d8a4e

**3. [Rule 3 - Blocking] exit_table.cmake also starts the runner**
- **Found during:** Task 1
- **Fix:** It takes `-DEMULATOR=` as well, as the plan's "every runner-starting script" requires. The file was not in the plan's list.
- **Committed in:** 632ff34

**4. [Docs] One Lanes section in place of the Presets table**
- The plan asked for a new section. The old Presets table would have duplicated it, so the two were merged into Lanes. `cmake/MuhtariTests.cmake` was in the plan's list but needed no change.

---

**Total deviations:** 3 auto-fixed (1 missing critical, 2 blocking) and 1 docs merge.
**Impact on plan:** No scope change.

## Issues Encountered

- On WASI, configure prints two CMake warnings from `cmake/packaging.cmake`, because `WASI` and `wasm32` are not release targets. The lane never packages, so they were left as they are. They are seeds for a cleanup if they turn out to be noisy in CI.
- The containers ran with the source mounted read-only (`:ro`) and the build in an anonymous volume. The verify commands also ran verbatim with the plan's read-write mount. Every container was started with `--rm`, and no background process is left running.

## Seeds

- None was used. `hashes.cmake` and the toolchain files were written here.

## User Setup Required

None.

## Next Phase Readiness

- The local part of the phase is complete. The tree is green on every lane this Mac can run: `ci`, `hygiene`, Linux arm64 `ci`, `asan`, `nofp`, `be32` and `wasm32`.
- Plan 14 (`ci.yml`) can call `cmake --workflow --preset be32` and `wasm32` as they stand.
  - The wasm32 job fetches `wasmtime-x86_64-linux` into `build/pins/wasmtime`.
  - `hash-equality` compares seven files with `-DCOUNT=7`.
  - wasm32 is compared report-only.
- The next session resumes with `/gsd-execute-phase 1` at plan 14.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED
