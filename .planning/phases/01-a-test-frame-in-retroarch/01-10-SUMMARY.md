---
phase: 01-a-test-frame-in-retroarch
plan: 10
subsystem: testing
tags: [cmake, nm, otool, readelf, llvm-readobj, abi, release-floor]

requires:
  - phase: 01-a-test-frame-in-retroarch
    provides: "plan 09: release zips, the ci-release test step, docs/testing.md; plan 07: the libretro module with retro_* exports"
provides:
  - "tests/cmake/abi.cmake: one script, modes undefined, globals and symbols, on the core archive"
  - "tests/cmake/api_version.cmake: every header name in a versioned API test"
  - "tests/cmake/floor.cmake: per-leg OS and runtime floors read from the release binaries"
  - "Negative-control fixtures tests/abi/libc_fixture.c and tests/abi/global_fixture.c, built with the core's flags"
  - "Tests abi.undefined, abi.undefined.self_test, abi.globals, abi.globals.self_test, abi.symbols, abi.api_version, abi.api_version.self_test, release.floor, release.floor.self_test"
affects: [01-12 Linux and nofp legs, 01-14 Windows leg, any change to include/muhtari.h]

actuals:
  tokens: 6358
  tasks: 3
  commits: 3
plan_head_before: a3336559416a581a83c48eb1532837911d773407
plan_head_after: 5940e88fded4ea8d031b74b6e393c4644b3750b2

tech-stack:
  added: []
  patterns:
    - "Binary checks parse nm -P output (name type value size), the one format GNU nm, llvm-nm and Apple nm share"
    - "Per-system compiler helpers live in a table in abi.cmake, one source comment per row, keyed by CMAKE_SYSTEM_NAME/CMAKE_SYSTEM_PROCESSOR"
    - "release.floor extracts the zips itself into its own directory, so it never races release.archive_contents under parallel ctest"

key-files:
  created:
    - tests/cmake/abi.cmake
    - tests/cmake/api_version.cmake
    - tests/cmake/floor.cmake
    - tests/abi/libc_fixture.c
    - tests/abi/global_fixture.c
    - tests/abi/tests.cmake
  modified:
    - tests/tests.cmake
    - tests/inventory/ci.txt
    - tests/release/tests.cmake
    - tests/api/v1.c
    - docs/testing.md

key-decisions:
  - "Darwin's bzero is allowed in abi.undefined's per-system helper table rather than building the core with -fno-builtin: the plan's helper table names bzero for Darwin, and -fno-builtin would also stop memcpy and memset being inlined"
  - "The script parameter naming the system is SYSTEM, not TARGET: TARGET is an if() keyword in CMake, and if(TARGET MATCHES ...) fails with 'Unknown arguments specified'"
  - "abi.globals ignores no symbol names (the seed skipped ltmp/l_ labels): on Mach-O an ltmp label typed b or d only exists when that section holds data, so it is a true positive"
  - "tests/api/v1.c now names the muhtari_allocator type, which abi.api_version found missing; 0.1.0 is unreleased (version.txt 0.0.0, no tags), so v1.c is not yet frozen"
  - "MSVC sets no CMAKE_NM, so tests/abi/tests.cmake falls back to find_program(llvm-nm nm); the Windows rows of abi.cmake and floor.cmake are unverified until plan 14"

patterns-established:
  - "A self test for a binary check builds its fixture with the checked target's COMPILE_OPTIONS, so it compiles on every lane the check runs on"

requirements-completed: [FRAME-05, FRAME-06]

coverage:
  - id: D1
    description: "abi.undefined passes on the Release core, whose only undefined symbol outside its own members is bzero (Darwin helper); a library with no undefined symbols passes and a missing library fails"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^abi\\.undefined$'"
        status: pass
      - kind: other
        ref: "cmake -DMODE=undefined -DLIB=<scratch archive of one plain function> -DNM=/usr/bin/nm -DSYSTEM=Darwin/arm64 -P tests/cmake/abi.cmake exits 0; the same with a nonexistent LIB exits 1"
        status: pass
    human_judgment: false
  - id: D2
    description: "abi.undefined.self_test and abi.globals.self_test fail the check through expect_fail.cmake with exit 1 and a message naming abort and counter; both fixtures contain no float or double"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^abi\\.(undefined|globals)\\.self_test$'"
        status: pass
      - kind: other
        ref: "grep -cE '\\b(float|double)\\b' tests/abi/libc_fixture.c tests/abi/global_fixture.c prints 0 for each"
        status: pass
    human_judgment: false
  - id: D3
    description: "abi.globals passes on the core and fails when a static int counter written in a core function is added"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^abi\\.globals$'"
        status: pass
      - kind: other
        ref: "static counter injected into muhtari_get_versions: abi.globals failed naming 'b counter'; reverted, passed"
        status: pass
    human_judgment: false
  - id: D4
    description: "abi.symbols: exported muhtari_ names equal the header's five functions both ways, every other export starts muhtari__"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^abi\\.symbols$'"
        status: pass
      - kind: other
        ref: "header copy declaring muhtari_reset fails 'declared, not defined'; archive with an extra export z fails 'outside the muhtari_ namespace'"
        status: pass
    human_judgment: false
  - id: D5
    description: "abi.api_version requires tests/api/v<MUHTARI_API_VERSION>.c and every header identifier and struct field in some vK.c; its self test names an extra field"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^abi\\.api_version'"
        status: pass
      - kind: other
        ref: "API dir holding only v1_renamed.c: api_version.cmake exits 1 naming v1.c as missing"
        status: pass
    human_judgment: false
  - id: D6
    description: "release.floor passes on the macOS arm64 archives (minos 11.0, libSystem only, retro_* exports only) and its self test fails on the impossible floor 9.0"
    requirement: FRAME-06
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^release\\.floor'"
        status: pass
      - kind: other
        ref: "scratch build with CMAKE_OSX_DEPLOYMENT_TARGET=15.0, packaged: release.floor fails 'minimum OS 15.0 is above the floor 11.0'"
        status: pass
      - kind: other
        ref: "doctored libretro zip with a bundle exporting foo and linking libz: floor.cmake reports 'exports _foo' and 'links /usr/lib/libz.1.dylib'"
        status: pass
    human_judgment: false
  - id: D7
    description: "docs/testing.md documents the abi checks, the new self tests and a Supported systems section with each floor and the command that reads it"
    requirement: FRAME-06
    verification:
      - kind: other
        ref: "grep -c 'checked from the binaries, not run on these systems' docs/testing.md prints 1; docs.budgets passes"
        status: pass
    human_judgment: false
  - id: D8
    description: "The Linux and Windows branches of floor.cmake and abi.cmake (readelf, llvm-readobj, COFF nm output, ELF helper rows)"
    requirement: FRAME-06
    verification:
      - kind: integration
        ref: "plan 01-12: the Linux ci, asan and nofp lanes in ubuntu:26.04 containers run the abi tests and release.floor"
        status: unknown
      - kind: integration
        ref: "plan 01-14: the hosted ci.yml Windows legs run the abi tests and release.floor"
        status: unknown
    human_judgment: false
    rationale: "No Linux or Windows toolchain on this Mac; plans 12 and 14 run these branches on their CI legs and adjust them"

duration: 6min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 10: ABI and release-floor gates Summary

**nm-based gates on the core archive (undefined symbols, writable data, public names), a header-to-API-test version check, and a release floor check reading minimum OS, linked libraries and exports from the packaged binaries, each with a negative control**

## Performance

- **Duration:** about 6 min (measured from the plan start stamp)
- **Started:** 2026-10-16T15:48:40Z
- **Completed:** 2026-10-16T15:54:15Z
- **Tasks:** 3
- **Files modified:** 11

## Accomplishments

- `abi.undefined`, `abi.globals`, `abi.symbols` from one `tests/cmake/abi.cmake` (107 lines), with libc and global fixtures as negative controls built with the core's own flags.
- `abi.api_version` and its self test, which on its first run found that `tests/api/v1.c` never named `muhtari_allocator`.
- `release.floor` and its self test: the macOS arm64 floors proved from the packaged binaries; Linux and Windows branches written for plans 12 and 14 to prove.
- docs/testing.md: the symbol checks, three new self-test rows, and the "Supported systems" section.

## Measured facts

- Undefined symbols of the Mac core (`nm -P build/ci/libmuhtari.a`, Apple clang 21, Release): `bzero` from `frame.c.o`, plus `muhtari__check_size` and `muhtari__testcard_pixel`, which other members define. After subtracting what the archive defines, only `bzero` remains, and it is allowed by the Darwin row.
- Defined external names: `muhtari_create`, `muhtari_destroy`, `muhtari_get_info`, `muhtari_get_versions`, `muhtari_run_frame`, `muhtari__check_size`, `muhtari__testcard_pixel`.
- `otool -l build/ci/release/extract/runner/bin/muhtari-run`: `LC_BUILD_VERSION minos 11.0`. `release.floor` reads the same field.
- `abi.api_version` covers 43 header names: 18 identifiers and 25 distinct struct fields.

## Gate-code tally

| File | Lines |
|---|---|
| `tests/cmake/abi.cmake` | 107 (plan: about 90, at most 110) |
| `tests/cmake/api_version.cmake` | 48 (plan: about 25) |
| `tests/cmake/floor.cmake` | 125 (plan: about 80; it holds all three OS branches) |
| All `tests/cmake/*.cmake` plus `cmake/MuhtariTests.cmake` | 768 |
| `scripts/*.sh` | 283 |
| `src/` (`.c` and `.h`) | 315 |

Seed lines: nesturbator 9f9386c661f6 `tests/cmake/undefined_symbols.cmake` (86) and `global_symbols.cmake` (40), 126 lines in all, rewritten as the 107-line `abi.cmake`. The seed's malloc, free and fortify/stack-protector allowances are gone. The 1/10 ceiling applies from phase 3. The total is far above it today because `src/` is still the 315-line test card.

## Task Commits

1. **Task 1: ABI gate on the core library, with fixtures as negative controls** - `b03221f` (test)
2. **Task 2: abi.api_version with its self test** - `25b587e` (test)
3. **Task 3: release.floor with its self test and the Supported systems docs** - `5940e88` (test)

## Files Created/Modified

- `tests/cmake/abi.cmake` - nm -P checks in three modes, with the per-system helper table
- `tests/cmake/api_version.cmake` - header names must appear in `tests/api/vK.c`, K <= API version
- `tests/cmake/floor.cmake` - floors table; macOS, Linux and Windows loader checks; retro_* exports
- `tests/abi/libc_fixture.c`, `tests/abi/global_fixture.c` - negative controls, with no floating-point type
- `tests/abi/tests.cmake` - fixtures built with the core's COMPILE_OPTIONS; seven abi tests
- `tests/release/tests.cmake` - release.floor and its per-OS impossible-floor self test
- `tests/tests.cmake`, `tests/inventory/ci.txt` - include and the nine new inventory lines
- `tests/api/v1.c` - names `muhtari_allocator`
- `docs/testing.md` - symbol checks, self-test rows, Supported systems

## Decisions Made

See `key-decisions` in the frontmatter. The orchestrator raised the carry-forward question about `_bzero`; it was settled by allowing `bzero`/`__bzero` in the Darwin row of the helper table, with the measurement as its source comment.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] tests/api/v1.c did not name the muhtari_allocator type**
- **Found during:** Task 2
- **Issue:** The new check reported `muhtari_allocator` as present in the header but in no API test.
- **Fix:** v1.c now fills a `muhtari_allocator` and assigns it to `cfg.allocator`. The file is not yet frozen because nothing has been released.
- **Files modified:** tests/api/v1.c
- **Verification:** abi.api_v1 and abi.api_version pass
- **Committed in:** 25b587e

**2. [Rule 2 - Missing critical] docs/testing.md also documents the abi checks and the api_version rule**
- **Found during:** Tasks 1 and 2
- **Issue:** The plan named docs/testing.md only for task 3. AGENTS rule 5 requires each behaviour change to land with its docs.
- **Fix:** A section on the core library's symbols, the self-test rows, and the `abi` label description.
- **Committed in:** b03221f, 25b587e

**3. [Rule 3 - Blocking] release.floor extracts the zips itself**
- **Found during:** Task 3
- **Issue:** The ci-release step runs tests in parallel, so reading `release/extract` would depend on release.archive_contents finishing first.
- **Fix:** floor.cmake extracts into its own `-DWORK` directory: `release/floor`, and `release/floor_self_test` for the self test.
- **Committed in:** 5940e88

---

**Total deviations:** 3 auto-fixed (1 bug, 1 missing critical, 1 blocking)
**Impact on plan:** None widens scope. The scripts run over their line estimates (api_version 48 against about 25, floor 125 against about 80); see the tally.

## Issues Encountered

- `if(TARGET MATCHES ...)` parses as the TARGET keyword, so the parameter was renamed SYSTEM.
- `if(... MATCHES ...)` overwrites `CMAKE_MATCH_*`, so the helper row's names are saved first.
- A `;` inside struct bodies split CMake lists, so `api_version.cmake` turns `;` into `,` before parsing.
- CMake 4.4's CMP0219 warns about backslashes in macro arguments, so `within` in floor.cmake is a function.

## Evidence

- `cmake --workflow --preset ci` exit 0: 52/52 tests in the main step, 4/4 in the release step
- `ctest --test-dir build/ci -L abi --output-on-failure` exit 0, 10/10
- `ctest --test-dir build/ci -R '^release\.floor' --output-on-failure` exit 0, 2/2

## User Setup Required

None.

## Next Phase Readiness

- Plan 12 runs abi.* on the Linux and nofp legs and proves floor.cmake's Linux branch. If the `_GLOBAL_OFFSET_TABLE_` row's processor regex or the `nm -D` export parsing differ on ubuntu-26.04, adjust them there.
- Plan 14 proves the Windows rows: llvm-nm on COFF archives (section symbols could show as b/d), llvm-readobj imports and exports, and the /DEFAULTLIB scan.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED

All six created files exist; commits b03221f, 25b587e and 5940e88 are ancestors of HEAD.
