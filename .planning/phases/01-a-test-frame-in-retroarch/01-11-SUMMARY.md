---
phase: 01-a-test-frame-in-retroarch
plan: 11
subsystem: testing
tags: [cmake, ctest, clang-format, clang-tidy, hygiene, notices, spdx]

requires:
  - phase: 01-10
    provides: "test tools (expect_fail.cmake, inventories), policy label, release gates"
provides:
  - "Preset and workflow `hygiene` with option MUHTARI_HYGIENE and inventory tests/inventory/hygiene.txt"
  - "Configure-time search for clang-format and clang-tidy 21 (Debian -21 names, then brew --prefix llvm@21, then PATH)"
  - "Tests hygiene.tree, hygiene.format, hygiene.no_float, hygiene.no_float.self_test, hygiene.analyzer"
  - "Test policy.notices in the ci inventory"
affects: [01-12, 01-14, 01-15, 01-17]

actuals:
  tokens: 5281
  tasks: 3
  commits: 4
plan_head_before: 34959e6f5105e834245137571d80c23a1fbee1d0
plan_head_after: 10f6a942962556cc6254fe429a394767a0d04b8d

tech-stack:
  added: [clang-format 21.1.8, clang-tidy 21.1.8]
  patterns:
    - "A lane option (MUHTARI_HYGIENE) that registers only its label's tests and picks its own inventory"
    - "Tools found at configure time through a package manager's prefix query, never a written-out path"

key-files:
  created:
    - tests/hygiene/tests.cmake
    - tests/inventory/hygiene.txt
    - tests/cmake/no_float.cmake
    - tests/cmake/notices.cmake
  modified:
    - CMakeLists.txt
    - CMakePresets.json
    - tests/tests.cmake
    - tests/policy/tests.cmake
    - tests/inventory/ci.txt
    - host/palette.c
    - runner/exit_status.h
    - runner/main.c
    - tests/core/test_core.c
    - tests/libretro/host.c
    - tests/runner/test_png.c

key-decisions:
  - "The hygiene preset sets CMAKE_OSX_SYSROOT=macosx: CMake 4 leaves the sysroot empty, so Homebrew clang-tidy found no SDK headers; Linux ignores the variable"
  - "The palette table sits inside clang-format off/on so it keeps four values per line and sixteen per hue row; bin-packing split each hue 9+7"
  - "no_float removes cited section numbers (section 6.2) before matching decimal literals, so hardware citations in src/ stay as written"

patterns-established:
  - "Gate scripts open with a `Fault prevented:` comment and run as cmake -P"

requirements-completed: [FRAME-05]

coverage:
  - id: D1
    description: "The hygiene preset runs from one command with no PATH setup, and configure finds clang-format and clang-tidy 21 by itself"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "cmake --workflow --preset hygiene (exit 0, 5 of 5 passed)"
        status: pass
      - kind: other
        ref: "env PATH=/usr/bin:/bin cmake -S . -B <scratch> -G Ninja -DCMAKE_MAKE_PROGRAM=<ninja> -DCMAKE_IGNORE_PREFIX_PATH=/opt/homebrew;/usr/local -DMUHTARI_HYGIENE=ON (exit 1, message names clang-format 21 and how to install it)"
        status: pass
      - kind: other
        ref: "cmake -S . -B <scratch> -DMUHTARI_HYGIENE=ON -DMUHTARI_CLANG_FORMAT=<stub printing version 18.1.8> (exit 1, message names version 18.1.8)"
        status: pass
    human_judgment: false
  - id: D2
    description: "hygiene.tree runs scripts/hygiene.sh --tree, which scans in the C locale so bytes that are not valid UTF-8 are scanned"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "tests/hygiene/tests.cmake#hygiene.tree"
        status: pass
    human_judgment: false
  - id: D3
    description: "hygiene.format checks every tracked C file except the vendored libretro.h with clang-format 21; the tree was formatted once in its own commit"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "tests/hygiene/tests.cmake#hygiene.format"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset ci after the reformat (exit 0, 52 and 4 passed, core.testcard.pinned_hash unchanged)"
        status: pass
    human_judgment: false
  - id: D4
    description: "hygiene.no_float scans src/ and include/ for float, double, math.h, float.h and float literals; its self test proves six float lines are named and two look-alikes are not"
    requirement: FRAME-05
    verification:
      - kind: unit
        ref: "tests/hygiene/tests.cmake#hygiene.no_float"
        status: pass
      - kind: unit
        ref: "tests/hygiene/tests.cmake#hygiene.no_float.self_test"
        status: pass
      - kind: other
        ref: "append `double x;` to src/frame.c, ctest -R hygiene.no_float (failed naming src/frame.c:49), then git checkout"
        status: pass
    human_judgment: false
  - id: D5
    description: "hygiene.analyzer runs clang-tidy clang-analyzer-* over src/*.c with warnings as errors and finds nothing"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "tests/hygiene/tests.cmake#hygiene.analyzer"
        status: pass
      - kind: other
        ref: "append a null dereference to src/frame.c, ctest -R hygiene.analyzer (failed with clang-analyzer-core.NullDereference), then git checkout"
        status: pass
    human_judgment: false
  - id: D6
    description: "policy.notices: SPDX lines, THIRD-PARTY-NOTICES.md sections and the libretro .info licence field agree"
    requirement: FRAME-05
    verification:
      - kind: unit
        ref: "tests/policy/tests.cmake#policy.notices"
        status: pass
      - kind: other
        ref: "scratch copy with the palette section renamed to host/colours.c: notices.cmake exit 1 naming host/palette.c and host/colours.c; with the .info field changed: exit 1 naming both licence lists"
        status: pass
    human_judgment: false
  - id: D7
    description: "The hygiene lane passes in CI on ubuntu-26.04 with the preinstalled clang-format-21 and clang-tidy-21"
    requirement: FRAME-05
    verification:
      - kind: integration
        ref: "plan 01-14 ci.yml hygiene job run on the probe pull request"
        status: unknown
    human_judgment: false

duration: 6min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 11: Hygiene lane and notices policy Summary

**One `cmake --workflow --preset hygiene` finds clang-format and clang-tidy 21 on its own (Homebrew `llvm@21` through `brew --prefix`) and runs five gates: the tree scan, formatting, integer-only text, the clang analyzer, and a float-scan self test. `policy.notices` in the ci lane keeps SPDX lines, notices and the .info licence in agreement.**

## Performance

- **Duration:** about 6 min
- **Started:** 2026-10-16T15:57:29Z
- **Completed:** 2026-10-16T16:03:23Z
- **Tasks:** 3
- **Files modified:** 15

## Accomplishments
- Preset and workflow `hygiene`. Option `MUHTARI_HYGIENE` registers only `hygiene`-label tests, against `tests/inventory/hygiene.txt`.
- Configure-time tool search with cache variables `MUHTARI_CLANG_FORMAT` and `MUHTARI_CLANG_TIDY`. When a tool is missing, configure fails, names the tool and says how to install it. A clang-format other than 21 also fails configure, naming the version found.
- Tests `hygiene.tree`, `hygiene.format`, `hygiene.no_float`, `hygiene.no_float.self_test`, `hygiene.analyzer`, and `policy.notices` (ci inventory, now 53 tests).
- The tree was formatted with clang-format 21 for the first time, in its own commit. It changed no behaviour: the pinned hashes held.

## Tools the configure found (Mac)
- clang-format: `$(brew --prefix llvm@21)/bin/clang-format`, Homebrew clang-format 21.1.8
- clang-tidy: `$(brew --prefix llvm@21)/bin/clang-tidy`, 21.1.8
- jq `/usr/bin/jq`, sh `/bin/sh`, git through `find_package(Git)`

## Gate-code tally
- `src/` + `include/`: 387 lines.
- New gate code this plan: `tests/cmake/no_float.cmake` 55, `tests/cmake/notices.cmake` 57, `tests/hygiene/tests.cmake` 40, the hygiene block in `CMakeLists.txt` 30, the `policy.notices` registration 6.
- All gate scripts (`tests/cmake/*.cmake` + `scripts/hygiene.sh`): 988 lines, about 2.6 times `src/`. The tenth-of-`src/` budget (AGENTS rule 7) starts at phase 3. This is recorded now so the tune-up can see the trend.
- Seed: nesturbator `tests/cmake/float_scan.cmake` at `9f9386c661f6` (83 lines), rewritten as 55 lines. The planned seed commit `3200aa868371` is not on GitHub; the orchestrator verified this, so the published main was used.

## Evidence
- `cmake --workflow --preset hygiene`: exit 0, 5 of 5 passed.
- `cmake --workflow --preset ci`: exit 0. Test step 53 of 53 passed (`policy.notices` included); release step 4 of 4.
- Configure with stripped PATH and Homebrew unreachable (`env PATH=/usr/bin:/bin`, `-DCMAKE_IGNORE_PREFIX_PATH=/opt/homebrew;/usr/local`): exit 1 with `hygiene: clang-format 21 not found (macOS: brew install llvm@21; Ubuntu: apt install clang-format-21 clang-tidy-21)`. Stripping PATH alone was not enough, because CMake's own system prefix still reaches `/opt/homebrew/bin/brew`.
- Configure with a stub clang-format that reports 18.1.8: exit 1 with `... is version 18.1.8, not 21`.
- `grep -c '/opt/homebrew\|/usr/local/opt' CMakeLists.txt`: 0.

## Task Commits

1. **Task 1: End-to-end hygiene lane** - `4fe0b29` (test)
2. **Task 2 (formatting pass, own commit)** - `1531356` (style)
3. **Task 2: Format, float scan and analyzer tests** - `a5a80e0` (test)
4. **Task 3: The notices policy** - `10f6a94` (test)

## Files Created/Modified
- `CMakeLists.txt`: option `MUHTARI_HYGIENE`, tool search, inventory default
- `CMakePresets.json`: configure, build, test and workflow presets `hygiene`
- `tests/tests.cmake`: the hygiene lane includes only `tests/hygiene/tests.cmake`
- `tests/hygiene/tests.cmake`: the five hygiene tests and the float fixture
- `tests/inventory/hygiene.txt`: the hygiene inventory
- `tests/cmake/no_float.cmake`: text scan for floating point
- `tests/cmake/notices.cmake`: checks that SPDX lines, notices and the .info licence agree
- `tests/policy/tests.cmake`, `tests/inventory/ci.txt`: `policy.notices`
- `host/palette.c`, `runner/*`, `tests/{core,libretro,runner}/*.c`: clang-format pass

## Decisions Made
- `CMAKE_OSX_SYSROOT=macosx` in the hygiene preset (see key-decisions). Alternative considered: an `xcrun --show-sdk-path` extra argument to clang-tidy. That needs more code for the same effect.
- The palette table is fenced with `clang-format off/on` rather than reflowed.
- `no_float` exempts `section N.N` citations, so the A7800-MARIA citations in `src/instance.c` stay as written.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] clang-tidy could not find the SDK headers**
- **Found during:** Task 2
- **Issue:** `hygiene.analyzer` failed with `'string.h' file not found`. CMake 4 leaves `CMAKE_OSX_SYSROOT` empty, so the compile commands carry no `-isysroot`.
- **Fix:** the hygiene preset sets `CMAKE_OSX_SYSROOT=macosx`.
- **Files modified:** CMakePresets.json
- **Verification:** `hygiene.analyzer` passes, and an injected null dereference fails it.
- **Committed in:** a5a80e0

**2. [Rule 1 - Bug] The float scan flagged section citations in src/**
- **Found during:** Task 2
- **Issue:** `(A7800-MARIA section 6.2)` in `src/instance.c` matched the decimal-literal rule.
- **Fix:** `section N.N` is removed before matching.
- **Files modified:** tests/cmake/no_float.cmake
- **Committed in:** a5a80e0

**3. [Rule 1 - Bug] The reformat broke a comment**
- **Found during:** Task 2 (formatting pass)
- **Issue:** clang-format turned a comment line that began with `*out` into a star continuation.
- **Fix:** reworded to "the out pointer".
- **Files modified:** tests/core/test_core.c
- **Committed in:** 1531356

**4. [Seed source] Seed fetched from 9f9386c661f6, not 3200aa868371**
- The planned commit is not on GitHub. The commit body cites the commit actually fetched.

---

**Total deviations:** 3 auto-fixed (1 blocking, 2 bugs) and 1 seed-source substitution.
**Impact on plan:** no scope change. `notices.cmake` is 57 lines against the plan's estimate of about 30. The extra lines cover the four checks PROVENANCE section 5 requires, plus readable findings.

## Issues Encountered
- The plan's stripped-PATH acceptance check needed `CMAKE_IGNORE_PREFIX_PATH` to make brew unreachable. The Evidence section records how it was shown.

## User Setup Required
None. On a Mac without the tools, `brew install llvm@21`; configure prints this.

## Next Phase Readiness
- Plan 12 adds `policy.pins` tests to `hygiene.txt`. Plan 14 runs the `hygiene` job in `ci.yml` on ubuntu-26.04, which proves coverage entry D7.

## Self-Check: PASSED
- FOUND: tests/hygiene/tests.cmake, tests/inventory/hygiene.txt, tests/cmake/no_float.cmake, tests/cmake/notices.cmake
- FOUND commits: 4fe0b29, 1531356, a5a80e0, 10f6a94

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*
