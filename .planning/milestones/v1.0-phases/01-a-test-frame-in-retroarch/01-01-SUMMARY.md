---
phase: 01-a-test-frame-in-retroarch
plan: 01
subsystem: core
tags: [c17, cmake, presets, test-card, sha256, size-tags]

requires: []
provides:
  - "include/muhtari.h: the 0.1.0 public API (five functions, size-tagged structs, MUHTARI_API_VERSION 1)"
  - "Core library muhtari / muhtari::muhtari: test card, frame driver, instance, versions"
  - "muhtari-run --frames N --hash-frame N printing the pinned NTSC test-card hash"
  - "CMakePresets.json v11 with dev, ci, ci-msvc configure/build/test/workflow presets"
  - "version.txt as the single version source, generated private muhtari_version.h"
  - ".clang-format (LLVM base, indent 4, 100 columns)"
affects: [01-02, 01-04, 01-06, 01-07, 01-08, 01-09]

actuals:
  tokens: 8200
  tasks: 2
  commits: 2
plan_head_before: 22260c5fce3ffc272783eb6b1e7b55b8f957d7e3
plan_head_after: b7ed6c162fed37b5071ab6ff6f277b0767d3d44d

tech-stack:
  added: [CMake presets v11, Ninja]
  patterns:
    - "Size tag check: reject below first-release size, accept a zero tail beyond ours"
    - "Output writer copies min(caller size, sizeof) bytes of a fully built local"
    - "Validate everything, then apply: run_frame errors change no state"
    - "Allocator is required; each host defines its own six-line malloc wrapper"
    - "Private generated version header from version.txt; public header has no version macros"

key-files:
  created:
    - include/muhtari.h
    - src/internal.h
    - src/instance.c
    - src/frame.c
    - src/testcard.c
    - runner/main.c
    - runner/sha256.c
    - runner/sha256.h
    - CMakeLists.txt
    - CMakePresets.json
    - cmake/version.h.in
    - version.txt
    - .clang-format
  modified: []

key-decisions:
  - "Seeds came from published nesturbator main 9f9386c661f6: the pinned 3200aa868371 is not on GitHub, and its bytes are identical for every seeded file except CMakePresets.json"
  - "muhtari_run_frame writes audio_count = 0 on its error paths (no samples were produced); on success it is 800 or 801"
  - "muhtari_config keeps 4 alignment bytes after size on 64-bit targets (the header's field order fixes it); the static asserts pin allocator at offset sizeof(void *)"
  - "The runner target lives in the top-level CMakeLists.txt with RUNTIME_OUTPUT_DIRECTORY runner/, so no runner/CMakeLists.txt exists"
  - "ci-msvc inherits base, not ci, so the macOS deployment target never reaches Windows"

patterns-established:
  - "Hardware comments cite A7800-MARIA section 6 / A7800-AUDIO timing table"
  - "Commit bodies carry one 'Seeded from nesturbator <sha> <path> (same author, MIT).' line per seeded file"

requirements-completed: []

coverage:
  - id: D1
    description: "muhtari-run prints the pinned NTSC test-card hash through the ci preset"
    requirement: FRAME-03
    verification:
      - kind: e2e
        ref: "cmake --preset ci && cmake --build --preset ci && build/ci/runner/muhtari-run --frames 1 --hash-frame 1"
        status: pass
    human_judgment: false
  - id: D2
    description: "The test card is static: frame 1 and frame 600 hash the same"
    requirement: FRAME-03
    verification:
      - kind: e2e
        ref: "build/tracer/runner/muhtari-run --frames 600 --hash-frame 1 --hash-frame 600"
        status: pass
    human_judgment: false
  - id: D3
    description: "include/muhtari.h is the 0.1.0 contract: two includes, five functions, MUHTARI_API_VERSION 1"
    requirement: FRAME-01
    verification:
      - kind: other
        ref: "grep -cE '^(muhtari_status|void) +muhtari_' include/muhtari.h prints 5; grep -c '#include' include/muhtari.h prints 2"
        status: pass
    human_judgment: false
  - id: D4
    description: "Core requires the caller's allocator and validates size tags and run_frame arguments before changing state"
    requirement: FRAME-01
    verification:
      - kind: other
        ref: "scratch probe linked against build/tracer/libmuhtari.a: NULL alloc or free gives 1, size 4 gives 2, pitch 319 gives 1, capacity 799 gives 4, then counts 800 801 800 801"
        status: pass
    human_judgment: false
  - id: D5
    description: "ci preset builds warning-free and muhtari_get_versions reports 0.0.0, api 1, abi 1, behaviour revision 1"
    requirement: FRAME-01
    verification:
      - kind: other
        ref: "cmake --build --preset ci (warnings fatal) exit 0; scratch probe prints lib 0.0.0 api 1 abi 1 beh 1"
        status: pass
    human_judgment: false
  - id: D6
    description: "dev, ci and ci-msvc presets exist with version 11, macOS 10.13 target and the static MSVC runtime"
    requirement: FRAME-01
    verification:
      - kind: other
        ref: "cmake --list-presets=all; jq -r '[.workflowPresets[].name]|join(\",\")' CMakePresets.json prints dev,ci,ci-msvc"
        status: pass
    human_judgment: false

duration: 10min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 01: A test frame in RetroArch, tracer Summary

**The 0.1.0 C header, a pure integer test card and frame driver, and `muhtari-run --hash-frame` now produce the pinned NTSC hash `9eb011e2…` through a warning-free `ci` preset build with CMake presets v11 and a single `version.txt` version source.**

## Performance

- **Duration:** 10 min
- **Started:** 2026-10-16T14:29:28Z
- **Completed:** 2026-10-16T14:39:09Z
- **Tasks:** 2
- **Files modified:** 13 created

## Accomplishments
- `include/muhtari.h` holds exactly the agreed 0.1.0 block: `<stddef.h>` and `<stdint.h>` only, `extern "C"`, `MUHTARI_API_VERSION 1`, the `uint32_t` status typedef with five constants, five functions.
- Core: `muhtari__testcard_pixel` applies the eight card rules in order. `muhtari_run_frame` checks the size tag, pitch and capacity, then writes 242 rows at the caller's pitch and 800 or 801 zero samples, with no drift. Struct layouts are pinned by `_Static_assert` on every field.
- `muhtari-run` passes its own six-line malloc allocator, uses heap frame and audio buffers, and prints `frame 1 sha256 9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4`.
- The core builds with `-fno-stack-protector -U_FORTIFY_SOURCE -ffile-prefix-map=…` (MSVC `/GS- /Zl`), hidden visibility and PIC. The warning set applies only when the project is top-level.
- `CMakePresets.json` v11 has `dev`, `ci` (macOS 10.13 target) and `ci-msvc` (static CRT) presets. `version.txt` (0.0.0) feeds a private generated header read by `muhtari_get_versions`.

## Task Commits

1. **Task 1: End-to-end test-card hash: header → core → runner → CMake build** - `c9052d7` (feat)
2. **Task 2: Presets, the single version source and the formatter configuration** - `b7ed6c1` (feat)

## Files Created/Modified
- `include/muhtari.h` - the public 0.1.0 API
- `src/internal.h` - `struct muhtari`, frame and audio constants, first-release sizes, layout asserts
- `src/instance.c` - size-tag helper, output writer, get_versions, create, destroy, get_info
- `src/frame.c` - `muhtari_run_frame`: checks first, then video rows and silent audio
- `src/testcard.c` - `muhtari__testcard_pixel`, the pure test-card function
- `runner/main.c` - `muhtari-run --frames N --hash-frame N` with its own allocator
- `runner/sha256.c`, `runner/sha256.h` - FIPS 180-4 SHA-256 renamed to `muhtari__sha256_*`
- `CMakeLists.txt` - version parse, core target and flags, generated header, runner
- `CMakePresets.json` - dev, ci, ci-msvc presets and workflows
- `cmake/version.h.in` - template for `<build>/generated/muhtari_version.h`
- `version.txt` - `0.0.0`
- `.clang-format` - LLVM base, indent 4, 100 columns, Linux braces

## Seed lines used in commit bodies
- `c9052d7`: `Seeded from nesturbator 9f9386c661f6 src/testcard.c|src/instance.c|src/internal.h|runner/sha256.c|runner/sha256.h|CMakeLists.txt (same author, MIT).`, and `Seeded from nesturbator 5533aeb7e6e0 src/frame.c|runner/main.c (same author, MIT).` (one line per file)
- `b7ed6c1`: `Seeded from nesturbator 9f9386c661f6 CMakePresets.json (same author, MIT).` and `… .clang-format …`

## Gate-code line tally
- Gate code: `cmake/*.cmake` none, `tests/cmake/*.cmake` none, `scripts/*.sh` 283 (`apply-repo-settings.sh` 106, `hygiene.sh` 177).
- `src/*.c src/*.h`: 315 lines (frame.c 48, instance.c 122, testcard.c 50, internal.h 95).
- The tenth-of-`src/` ceiling applies from phase 3. It is recorded here for the trend.

## Evidence (command, exit status)
- `cmake -S . -B build/tracer -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build/tracer && build/tracer/runner/muhtari-run --frames 1 --hash-frame 1` exited 0 and printed the pinned line.
- `cmake --preset ci && cmake --build --preset ci && build/ci/runner/muhtari-run --frames 1 --hash-frame 1` exited 0 and printed the pinned line. Warnings were fatal and none appeared.
- `grep -rnE '\b[DR]-[0-9]{2}\b|FRAME-[0-9]{2}' include src runner cmake CMakeLists.txt CMakePresets.json .clang-format` exited 1, with no planning IDs.
- `otool -l build/ci/runner/muhtari-run` shows `LC_BUILD_VERSION minos 11.0`, the arm64 clamp of the 10.13 target.

## Decisions Made
- Seed source: see Deviations 1.
- `audio_count` is 0 on every error path after the size check. The caller already knows the maximum, 801, from `muhtari_get_info`.
- `muhtari_config` has 4 alignment bytes after `size` on 64-bit targets, because the header's field order puts a pointer-aligned struct after a `uint32_t`. The asserts pin `offsetof(allocator) == sizeof(void *)` and `sizeof == 4 * sizeof(void *)`. Every other struct has no inner padding.
- The runner is defined in the top-level `CMakeLists.txt`, with its output in `<build>/runner/`, so the plan's `build/ci/runner/muhtari-run` path holds without a `runner/CMakeLists.txt`.
- `ci-msvc` inherits `base` and sets the `cl` compilers, as the seed does, plus the static-runtime variable.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] The pinned seed commit is not fetchable from GitHub**
- **Found during:** Task 1 (seed fetch)
- **Issue:** `gh api …?ref=3200aa868371` returns 404 and `commits/3200aa868371` returns 422. The commit exists only on an unpushed branch of the owner's local nesturbator checkout. A read-only `git show` from that checkout was refused by the permission classifier, and I did not work around the refusal.
- **Fix:** Fetched with the plan's own `gh api` method at published `main` `9f9386c661f6`. `git diff --stat 9f9386c661f6 3200aa868371` lists only `CMakePresets.json` (2 lines) among the seeded files, so every other seed matches the pin byte for byte. The commit bodies cite `9f9386c661f6`, the commit actually fetched. `frame.c` and `runner/main.c` came from `5533aeb7e6e0`, as planned.
- **Files modified:** none beyond the plan's list
- **Verification:** the pinned hash reproduces
- **Committed in:** c9052d7, b7ed6c1

**2. [Rule 2 - Missing critical] `version.txt` is validated and tracked as a configure dependency**
- **Found during:** Task 2
- **Issue:** `file(STRINGS)` does not re-run configure when `version.txt` changes. A release bump would then leave a stale version header in an existing build tree.
- **Fix:** Added `CMAKE_CONFIGURE_DEPENDS version.txt` and a `MAJOR.MINOR.PATCH` regex check that fails configure.
- **Files modified:** CMakeLists.txt
- **Committed in:** b7ed6c1

---

**Total deviations:** 2 auto-fixed (1 blocking, 1 missing critical)
**Impact on plan:** No scope change. The provenance line names the fetched commit, not the pinned one. The owner may want to push `3200aa868371`, or repin PROVENANCE to a published commit.

## Issues Encountered
- Apple clang lowers the zeroing `memset` calls in the core to `_bzero`, which `nm -u build/tracer/libmuhtari.a` shows. The `abi.undefined` allowlist (`memcpy memmove memset memcmp` plus per-target helpers) must list `_bzero` for macOS, or the core must be built with `-fno-builtin-bzero` or an equivalent. This is for the plan that adds `abi.undefined`.
- `cmake --workflow --preset ci` currently stops at its test step with "No tests were found" (`noTestsAction: error`), as the plan intends. The workflows first run green in plan 02, when tests are registered.

## User Setup Required

None - no external service configuration required.

## Next Phase Readiness
- Ready for 01-02 (tests and the workflow presets). The core API, the runner hash and the presets are in place.
- The 0.1.0 header is a published-API commitment. Any change to it now goes through an API-version bump and a test.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED

- All 13 created files exist; commits c9052d7 and b7ed6c1 are ancestors of HEAD.
- Plan verification re-run: `cmake --preset ci && cmake --build --preset ci && build/ci/runner/muhtari-run --frames 1 --hash-frame 1` exit 0, pinned line printed.
