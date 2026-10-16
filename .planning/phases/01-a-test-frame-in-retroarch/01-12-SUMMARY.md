---
phase: 01-a-test-frame-in-retroarch
plan: 12
subsystem: ci
tags: [docker, ubuntu-26.04, gcc-15, clang-21, asan, ubsan, general-regs-only, pins, sha256]

requires:
  - phase: 01-11
    provides: "hygiene lane and inventory; expect_fail.cmake"
  - phase: 01-10
    provides: "abi.cmake, floor.cmake and their self tests, whose Linux branches this plan proves"
provides:
  - "pins.txt: wasmtime 48.0.6 (x86_64-linux, aarch64-linux, aarch64-macos) and local-only CMake 4.4.3 (Linux aarch64, x86_64)"
  - "scripts/fetch-pin.sh NAME DEST, environment PINS: download, SHA-256 check, then unpack or copy"
  - "Presets and workflows asan and nofp; options MUHTARI_SHORT_RUNS and MUHTARI_NOFP"
  - "Tests abi.nofp_fixture (nofp inventory), policy.pins and policy.pins.self_test (hygiene inventory)"
  - "tests/inventory/nofp.txt; all abi-label tests now registered from tests/abi/tests.cmake"
  - "docs/testing.md: pinned downloads, running the Linux lanes locally, asan, nofp and hygiene presets"
affects: [01-13, 01-14, wasm32 lane, release workflow]

actuals:
  tokens: 7923
  tasks: 3
  commits: 3
plan_head_before: a1f68e0cfc504f21b23b7bf5480ddc704b5d6de5
plan_head_after: e6780947a8ce45aa38e6ad2d17dc0347ea7bc96d

tech-stack:
  added: [GCC 15.2 (ubuntu:26.04), clang-21 21.1.8 with libclang-rt-21-dev, CMake 4.4.3 Linux tarballs (local containers only), wasmtime 48.0.6 pin]
  patterns:
    - "Every download is a pins.txt row fetched through scripts/fetch-pin.sh, verified before it is unpacked"
    - "A lane option (MUHTARI_NOFP, like MUHTARI_HYGIENE) registers only its label's tests against its own inventory"
    - "Linux lanes run locally in ubuntu:26.04 with an anonymous volume over build/"

key-files:
  created:
    - pins.txt
    - scripts/fetch-pin.sh
    - tests/cmake/pins.cmake
    - tests/abi/nofp_fixture.c
    - tests/inventory/nofp.txt
  modified:
    - CMakeLists.txt
    - CMakePresets.json
    - tests/tests.cmake
    - tests/abi/tests.cmake
    - tests/core/tests.cmake
    - tests/core/test_core.c
    - tests/install/tests.cmake
    - tests/runner/tests.cmake
    - tests/hygiene/tests.cmake
    - tests/inventory/hygiene.txt
    - docs/testing.md
    - .planning/preparation/CONFORMANCE.md

key-decisions:
  - "The abi self-test regexes allow a line break anywhere in the finding's title: CMake wraps a FATAL_ERROR message at a column that depends on the build path, and the shorter container path moved the wrap"
  - "abi.api_v1 and abi.header.* moved into tests/abi/tests.cmake so the nofp lane registers every abi test by including one file; the ci inventory is unchanged"
  - "The asan lane needs libclang-rt-21-dev in a container: --no-install-recommends leaves out the sanitizer runtimes that clang-21 alone does not pull"
  - "Under Docker's x86_64 emulation on Apple Silicon GNU tar cannot create nested folders (ENOSYS); local amd64 runs substitute bsdtar, documented in docs/testing.md; fetch-pin.sh is unchanged"
  - "MUHTARI_SHORT_RUNS shortens the three 600-frame runs to 60 frames; every test still runs"

patterns-established:
  - "A download enters the build only as a pins.txt row with a digest from the release API"

requirements-completed: [FRAME-05, FRAME-06]

coverage:
  - id: D1
    description: "pins.txt holds the wasmtime 48.0.6 rows with the D-19 digests and the CMake 4.4.3 Linux rows with digests from the release API; scripts/fetch-pin.sh verifies before it unpacks, and a wrong digest exits 1 naming the row and leaves DEST absent"
    requirement: FRAME-06
    verification:
      - kind: other
        ref: "gh api repos/bytecodealliance/wasmtime/releases/tags/v48.0.6 and repos/Kitware/CMake/releases/tags/v4.4.3 asset digests equal the pins.txt rows"
        status: pass
      - kind: other
        ref: "scripts/fetch-pin.sh wasmtime-aarch64-macos <dir> exit 0; with PINS=<copy with one digit changed> exit 1 'wasmtime-aarch64-macos: sha256 mismatch' and no DEST; an unknown name exit 1"
        status: pass
      - kind: integration
        ref: "tests/hygiene/tests.cmake#policy.pins.self_test"
        status: pass
    human_judgment: false
  - id: D2
    description: "policy.pins checks every pins.txt row for four fields, a unique name, an https:// URL and a 64-digit hex digest"
    requirement: FRAME-05
    verification:
      - kind: unit
        ref: "tests/hygiene/tests.cmake#policy.pins"
        status: pass
      - kind: other
        ref: "cmake -DPINS=<file with an http URL, a 3-field row, a duplicate name and an upper-case digest> -P tests/cmake/pins.cmake exit 1 naming all five faults"
        status: pass
    human_judgment: false
  - id: D3
    description: "The Linux ci workflow passes in ubuntu:26.04 with GCC 15.2 on arm64 and x86_64, including release.floor on the linux-arm64 and linux-x64 archives"
    requirement: FRAME-06
    verification:
      - kind: integration
        ref: "docker run --rm --platform linux/arm64 -v \"$PWD:/src\" -v /src/build -w /src ubuntu:26.04 sh -c '<apt install> && scripts/fetch-pin.sh cmake-linux-aarch64 /opt/cmake && PATH=/opt/cmake/bin:$PATH cmake --workflow --preset ci' exit 0, 53 and 4 passed"
        status: pass
      - kind: integration
        ref: "the same with --platform linux/amd64, cmake-linux-x86_64 and bsdtar standing in for GNU tar: exit 0, 53 and 4 passed"
        status: pass
    human_judgment: false
  - id: D4
    description: "The asan lane (clang-21, ASan and UBSan, -fno-sanitize-recover=all, Debug -O1, short runs, timeouts x3) passes with no finding, and an injected overflow fails it"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "docker run ... ubuntu:26.04 (arm64, clang-21 lld-21 libclang-rt-21-dev added) cmake --workflow --preset asan exit 0, 43 passed, no 'ERROR: AddressSanitizer' or 'runtime error:' line"
        status: pass
      - kind: other
        ref: "scratch copy with a heap write one past a 4-byte malloc in core.info: AddressSanitizer heap-buffer-overflow, core.info failed, exit 8; with a stack array the same write failed through UBSan's bounds check; the copy was deleted"
        status: pass
    human_judgment: false
  - id: D5
    description: "The nofp lane builds the core with GCC 15 -mgeneral-regs-only and runs the 11 abi tests; abi.nofp_fixture shows a double fails to build under the flag"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "docker run ... ubuntu:26.04 cmake --workflow --preset nofp exit 0, 11 of 11 passed including abi.nofp_fixture, on arm64 and on x86_64"
        status: pass
      - kind: other
        ref: "a double division appended to src/frame.c in a scratch copy: the nofp build of frame.c fails with -mgeneral-regs-only on the command line; the copy was deleted"
        status: pass
    human_judgment: false
  - id: D6
    description: "The hygiene workflow passes on the Mac with the two pin tests added (7 tests)"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "cmake --workflow --preset hygiene exit 0, 7 of 7 passed"
        status: pass
    human_judgment: false
  - id: D7
    description: "The same Linux lanes pass on GitHub-hosted ubuntu-26.04 and ubuntu-26.04-arm runners, natively and with the image's own CMake"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "plan 01-14 ci.yml run on the probe pull request"
        status: unknown
    human_judgment: false

duration: 19min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 12: Linux lanes in containers, pins and the asan and nofp presets Summary

**Every Linux lane CI will run (`ci`, `asan`, `nofp`) now passes in `ubuntu:26.04` containers on the Mac, on arm64 and x86_64. Downloads go through `pins.txt` and `scripts/fetch-pin.sh`, which checks the SHA-256 before unpacking. The `asan` preset uses clang-21 ASan and UBSan, and the `nofp` preset builds with GCC 15 and `-mgeneral-regs-only`.**

## Performance

- **Duration:** about 19 min
- **Started:** 2026-10-16T16:05:58Z
- **Completed:** 2026-10-16T16:25:00Z
- **Tasks:** 3
- **Files modified:** 17 (357 insertions, 51 deletions)

## Accomplishments

- `pins.txt` has five rows. The digests were checked against the wasmtime and Kitware release APIs. `scripts/fetch-pin.sh` is 52 lines of POSIX sh.
- The Linux `ci` workflow is green on arm64 and x86_64, with the glibc floors read from the built binaries (table below). This proves plan 10's Linux branches of `abi.cmake` and `floor.cmake`. Plan 10's D8 entry is marked as passing.
- New `asan` lane: 43 tests, no sanitizer finding. Injected heap and stack overflows both fail it.
- New `nofp` lane: 11 abi tests, including `abi.nofp_fixture`. A double added to the core breaks the lane's build.
- New `policy.pins` and `policy.pins.self_test` tests in the hygiene lane.
- CONFORMANCE §2(a) and its CF.03 row now name the wasmtime v48.0.6 LTS pin. The 7800basic experiment ran on v49.0.1, so phase 3 repeats it on the pin.

## Measured facts

Read with `readelf -dW` and `readelf -VW` on the packaged binaries (`build/ci/release/floor/...`):

| Binary | Arch | NEEDED | GLIBC versions needed |
|---|---|---|---|
| `muhtari_libretro.so` | aarch64 | libc.so.6, ld-linux-aarch64.so.1 | 2.17 |
| `muhtari-run` | aarch64 | libc.so.6, ld-linux-aarch64.so.1 | 2.17, 2.34 |
| `muhtari_libretro.so` | x86_64 | libc.so.6 | 2.2.5, 2.4 |
| `muhtari-run` | x86_64 | libc.so.6 | 2.2.5, 2.3.4, 2.4, 2.14, 2.17, 2.34 |

- The core is at or below the 2.17 floor and the runner at or below 2.34 on both architectures. Every version need is a plain `GLIBC_n.n[.n]`.
- On x86_64 with GCC 15.2, the core archive's only undefined symbol outside its own members is `memset` (`nm -P`). `_GLOBAL_OFFSET_TABLE_` does not appear, so that helper row is allowed but unused.
- GCC 15.2 diagnostics for `abi.nofp_fixture`:
  - AArch64: `'-mgeneral-regs-only' is incompatible with the use of floating-point types`
  - x86-64: `SSE register return with SSE disabled`

## Container wall times

Each time covers the whole `docker run`: apt install, the CMake fetch, configure, build and the tests.

| Lane | arm64 (native) | x86_64 (emulated) |
|---|---|---|
| `ci` | 24–28 s | 65 s |
| `asan` | 29–33 s | not run |
| `nofp` | 25–26 s | 65 s |

## Gate-code tally

- New gate code: `scripts/fetch-pin.sh` 52 lines, `tests/cmake/pins.cmake` 49, and the pin-test registration in `tests/hygiene/tests.cmake` 19. `pins.txt` is data (18 lines).
- All gate scripts:
  - `tests/cmake/*.cmake` plus `cmake/MuhtariTests.cmake`: 929 lines.
  - `scripts/*.sh`: 335 lines.
- Code under test:
  - `src/`: 315 lines.
  - `src/` plus `include/`: 387 lines.
- The tenth-of-`src/` ceiling (AGENTS rule 7) applies from phase 3. It is recorded here so the trend stays visible.

## Evidence (command, exit status)

- Linux `ci`, arm64. The plan's task 1 verify command, run verbatim: exit 0, 53/53 then 4/4 (`release.floor` passed). Re-run on HEAD `e678094`: exit 0.
- Linux `ci`, x86_64: `--platform linux/amd64` with the `cmake-linux-x86_64` row, `libarchive-tools` and a bsdtar link. Exit 0, 53/53 then 4/4.
- `asan`, arm64. The task 2 command plus `libclang-rt-21-dev`: exit 0, 43/43, zero `ERROR: AddressSanitizer` or `runtime error:` lines. Re-run on HEAD: exit 0.
- `nofp` plus hygiene. The task 3 verify command, run verbatim: exit 0. nofp 11/11, then hygiene 7/7 on the Mac.
- `nofp`, x86_64: exit 0, 11/11.
- Mac `cmake --workflow --preset ci` on HEAD: exit 0, 53/53 then 4/4.

## Task Commits

1. **Task 1: End-to-end Linux leg: pins, fetch-pin, ci workflow, glibc floors** - `ea74349` (ci)
2. **Task 2: The asan preset, run in the container** - `588544e` (ci)
3. **Task 3: The nofp preset, its fixture, and the pins tests** - `e678094` (ci)

## Files Created/Modified

- `pins.txt`: pinned downloads, with the bump policy in its header
- `scripts/fetch-pin.sh`: download and verify by pin name
- `tests/cmake/pins.cmake`: checks the pins file format
- `tests/abi/nofp_fixture.c`: one double division, built only inside its expect-fail test
- `tests/inventory/nofp.txt`: the nofp lane's 11 tests
- `CMakeLists.txt`: options `MUHTARI_SHORT_RUNS` and `MUHTARI_NOFP`, and the nofp inventory default
- `CMakePresets.json`: `asan` and `nofp` configure, build, test and workflow presets
- `tests/tests.cmake`: the nofp lane includes only `tests/abi/tests.cmake`
- `tests/abi/tests.cmake`: wrap-tolerant self-test regexes, `abi.api_v1` and `abi.header.*` moved in, and `abi.nofp_fixture`
- `tests/core/tests.cmake`, `tests/core/test_core.c`, `tests/runner/tests.cmake`: long runs take their frame count from `MUHTARI_SHORT_RUNS`
- `tests/install/tests.cmake`: the header compile tests moved out
- `tests/hygiene/tests.cmake`, `tests/inventory/hygiene.txt`: `policy.pins` and its self test
- `docs/testing.md`: the new sections and preset rows
- `.planning/preparation/CONFORMANCE.md`: wasmtime pin

## Decisions Made

See `key-decisions` in the frontmatter.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] The abi self tests failed on Linux because a message wrap moved**
- **Found during:** Task 1
- **Issue:** CMake wraps a `FATAL_ERROR` message at a fixed column. With the shorter `/src/build` path, "writable data symbol:" and "unexpected undefined symbol:" were split across lines, so the expected regexes did not match. The checks themselves failed correctly.
- **Fix:** The regexes are now `unexpected[^:]*symbol:.*abort` and `writable[^:]*symbol:.*counter`.
- **Files modified:** tests/abi/tests.cmake
- **Verification:** Both self tests pass on the Mac, on arm64 and on x86_64.
- **Committed in:** ea74349

**2. [Rule 3 - Blocking] clang-21 could not link ASan in a container**
- **Found during:** Task 2
- **Issue:** `libclang_rt.asan.a` was missing. `--no-install-recommends` skips `libclang-rt-21-dev`.
- **Fix:** The container recipe for the asan lane adds `libclang-rt-21-dev`, and docs/testing.md says so. Nothing in the build changed.
- **Committed in:** 588544e (docs)

**3. [Rule 3 - Blocking] The other abi-label tests lived in core and install test files**
- **Found during:** Task 3
- **Issue:** The plan's nofp lane registers only abi-label tests, and `nofp.txt` should list every abi test. Three of them (`abi.api_v1`, `abi.header.c17`, `abi.header.cxx17`) were registered next to non-abi tests.
- **Fix:** Those registrations moved, unchanged, into `tests/abi/tests.cmake`. The ci inventory is the same.
- **Committed in:** e678094

**4. [Rule 2 - Missing critical] Documentation for the lanes and pins**
- docs/testing.md gained preset rows for `asan`, `nofp` and `hygiene`, rows for the two new self tests, and the pins checks (AGENTS rule 5). The plan named only the container recipe section.

**5. [Environment] Docker's x86_64 emulation and GNU tar**
- Under Rosetta, GNU tar 1.35 fails with ENOSYS when it creates a nested folder. The amd64 runs symlinked bsdtar as `tar`. Hosted x86_64 runners are native, so `fetch-pin.sh` stays as it is. docs/testing.md describes the workaround.

---

**Total deviations:** 4 auto-fixed (1 bug, 2 blocking, 1 missing critical) and 1 environment workaround.
**Impact on plan:** No scope change. `fetch-pin.sh` is 52 lines against the plan's estimate of about 35, which buys a misuse message and both digest tools.

## Issues Encountered

- A read-only source mount fails when `build/` does not yet exist, because Docker cannot create the anonymous volume's mount point. The docs recipe keeps the plan's read-write mount. Local runs here used `:ro` wherever `build/` existed.
- Pulling the amd64 `ubuntu:26.04` retagged the local image. The arm64 image was pulled again at the end. An untagged amd64 copy remains in the local Docker cache.

## Seeds

- None was used. nesturbator 9f9386c661f6 `tests/cmake/pins_check.cmake` checks a vector-manifest format that is unrelated to this one, so `pins.cmake` and `fetch-pin.sh` were written here.

## User Setup Required

None.

## Next Phase Readiness

- Plan 14's `ci.yml` can call `cmake --workflow --preset asan` and `nofp` as they stand. Under `asan`, the `check_junit` step must exclude the `abi` and `release` labels. The wasm32 job fetches `wasmtime-x86_64-linux` through `scripts/fetch-pin.sh`.
- Coverage entry D7, the hosted runs, stays `unknown` until plan 14's probe pull request.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED

- FOUND: pins.txt, scripts/fetch-pin.sh, tests/cmake/pins.cmake, tests/abi/nofp_fixture.c, tests/inventory/nofp.txt
- FOUND commits: ea74349, 588544e, e678094 (ancestors of HEAD)
