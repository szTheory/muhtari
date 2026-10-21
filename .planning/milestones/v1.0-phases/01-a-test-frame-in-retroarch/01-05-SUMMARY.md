---
phase: 01-a-test-frame-in-retroarch
plan: 05
subsystem: runner
tags: [png, stored-deflate, crc32, adler32, palette, runner, dump-frame]

requires:
  - phase: 01-04
    provides: "muhtari_host with muhtari__palette_rgb (warm NTSC, 0x00RRGGBB), linked by muhtari-run"
provides:
  - "runner/png.c and runner/png.h: stored-deflate PNG writer (muhtari__png_begin, muhtari__png_row, muhtari__png_end) and muhtari__adler32"
  - "runner/crc32.c and runner/crc32.h: muhtari__crc32, seeded from nesturbator src/crc32.c"
  - "muhtari-run --dump-frame N:FILE (repeatable): frame N as a 320x242 RGB PNG through the warm palette"
  - "runner.png and runner.dump_png tests; cmake --workflow --preset ci passes with 18 tests"
  - "Pinned dump PNG SHA-256 1c0b16cb9c92d70869d3aee5cd581627f8902cb528e90f1f1d4252d9288ac956 (232,645 bytes)"
affects: [01-06, 01-07, 01-08, 01-09]

actuals:
  tokens: 8042
  tasks: 2
  commits: 2
plan_head_before: 41be8a418a1b7a1c91c61b5cf038c7e8efdcf719
plan_head_after: 45ee8cd8bca90b481a97f4874516ddfa189c755f

tech-stack:
  added: []
  patterns:
    - "Image output is streamed: begin, one call per RGB row from a heap row buffer, end; the writer knows the IDAT length up front, so nothing is buffered whole"
    - "A file-producing runner test pins size and SHA-256 of the file in the same cmake -P script that registers it"

key-files:
  created:
    - runner/png.c
    - runner/png.h
    - runner/crc32.c
    - runner/crc32.h
    - tests/runner/test_png.c
  modified:
    - runner/main.c
    - CMakeLists.txt
    - tests/runner/tests.cmake
    - tests/inventory/ci.txt

key-decisions:
  - "The PNG writer takes RGB rows; the runner converts native bytes through muhtari__palette_rgb one row at a time into a 960-byte heap row buffer, so the writer has no palette dependency"
  - "runner/crc32.h is new: the seed declared its CRC in src/internal.h, which has no runner counterpart"
  - "runner.dump_png pins the PNG file SHA-256 1c0b16cb9c92d70869d3aee5cd581627f8902cb528e90f1f1d4252d9288ac956 and requires empty stdout when only --dump-frame is given"

patterns-established:
  - "Mutation checks must change output: an NLEN mask mutation that left both block lengths unchanged was discarded and replaced by an Adler-32 modulus mutation"

requirements-completed: []

coverage:
  - id: D1
    description: "Stored-deflate PNG writer: signature, IHDR 320x242 depth 8 colour type 2, one IDAT with zlib header 78 01 and four stored blocks of at most 65,535 bytes, Adler-32, IEND, and no other chunks"
    requirement: FRAME-03
    verification:
      - kind: unit
        ref: "tests/runner/test_png.c#runner.png"
        status: pass
      - kind: other
        ref: "python3 zlib.decompress and zlib.crc32 over build/ci/card.png report chunks IHDR, IDAT, IEND, valid CRCs and 232562 raw bytes"
        status: pass
    human_judgment: false
  - id: D2
    description: "CRC-32 and Adler-32 give the published check values CBF43926 for 123456789 and 11E60398 for Wikipedia"
    requirement: FRAME-03
    verification:
      - kind: unit
        ref: "tests/runner/test_png.c#runner.png"
        status: pass
    human_judgment: false
  - id: D3
    description: "Every PNG pixel equals the warm NTSC palette entry of the card's native byte at that position"
    requirement: FRAME-04
    verification:
      - kind: unit
        ref: "tests/runner/test_png.c#runner.png"
        status: pass
    human_judgment: false
  - id: D4
    description: "muhtari-run --frames 1 --dump-frame 1:FILE writes a 232,645-byte PNG with the pinned SHA-256, and macOS sips decodes it as 320x242"
    requirement: FRAME-03
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#runner.dump_png"
        status: pass
      - kind: other
        ref: "sips -g pixelWidth -g pixelHeight build/ci/card.png prints pixelWidth 320 and pixelHeight 242, exit 0"
        status: pass
    human_judgment: false
  - id: D5
    description: "Changing one byte of the $46 palette entry makes runner.dump_png fail, and changing the writer's Adler-32 modulus makes runner.png and runner.dump_png fail"
    requirement: FRAME-03
    verification:
      - kind: other
        ref: "sed s/0xaf5163/0xaf5164/ host/palette.c then ctest --preset ci -R runner.dump_png fails; ADLER_BASE 65519 then runner.png and runner.dump_png fail; both reverted and git diff is clean"
        status: pass
    human_judgment: false
  - id: D6
    description: "An unwritable --dump-frame path exits 1; N beyond --frames or an empty FILE exits 2 with usage"
    requirement: FRAME-03
    verification:
      - kind: other
        ref: "muhtari-run --frames 1 --dump-frame 1:/nonexistent/dir/x.png exits 1; --dump-frame 2:x.png and --dump-frame 1: exit 2"
        status: pass
    human_judgment: false

duration: 4min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 05: Runner PNG output Summary

**`muhtari-run --dump-frame N:FILE` writes the frame as a 320x242 RGB PNG through the warm NTSC palette. The writer uses stored deflate blocks with its own Adler-32 and the CRC-32 seeded from nesturbator, so it needs no compression library. The 232,645-byte card PNG is pinned at SHA-256 `1c0b16cb…c956`.**

## Performance

- **Duration:** 4 min
- **Started:** 2026-10-16T15:03:37Z
- **Completed:** 2026-10-16T15:07:30Z
- **Tasks:** 2
- **Files modified:** 9

## Accomplishments

- `runner/png.c` writes Pattern 3 exactly. The raw stream is 242 rows of a filter byte 0 plus 960 RGB bytes, 232,562 bytes in all. It is split across four stored blocks, and only the last has BFINAL set. Each multi-byte field is written a byte at a time, so the output does not depend on the host's byte order. The IDAT length (232,588) is computed up front, so the image streams row by row.
- `runner/crc32.c` is the nesturbator table and loop with the function renamed. A `diff` of the two tables found no difference.
- `--dump-frame` repeats like `--hash-frame` and is checked the same way: N must be at least 1 and no more than `--frames`, and FILE must not be empty. The runner converts each row with `muhtari__palette_rgb`, writing R, G and B explicitly. If a file cannot be opened or written, the runner prints a message and exits 1.
- `runner.png` reads the PNG back without using any of the writer's code: a bitwise CRC, a stored-block decoder and Adler-32 computed from its definition. It also checks that a row after the last is refused and that an empty image is refused.
- The PNG that `runner.png` writes has the same SHA-256 as the runner's dump. Two separate conversion loops produced the same bytes.

## Task Commits

1. **Task 1: End-to-end dump (tracer)** - `ab1363a` (feat)
2. **Task 2: PNG tests and the pinned dump digest** - `45ee8cd` (test)

## Files Created/Modified

- `runner/png.c`, `runner/png.h` - stored-deflate PNG writer and Adler-32
- `runner/crc32.c`, `runner/crc32.h` - CRC-32 (seed)
- `runner/main.c` - `--dump-frame N:FILE`, palette conversion, heap row buffer
- `CMakeLists.txt` - adds png.c and crc32.c to muhtari-run
- `tests/runner/test_png.c` - runner.png
- `tests/runner/tests.cmake` - runner.dump_png script mode and pin; registers runner.png
- `tests/inventory/ci.txt` - `runner.dump_png` and `runner.png`, sorted

## Decisions Made

- The writer takes RGB rows rather than native bytes and a palette, so it stays independent of the host palette. The runner does the conversion, as the plan's key link requires.
- nesturbator has no `src/crc32.h` (it returned 404 at 9f9386c661f6), because the seed declares its CRC in `src/internal.h`. The runner gets a small header of its own.
- When only `--dump-frame` is given, `runner.dump_png` requires empty stdout, so a stray print would also fail it.

## Deviations from Plan

### Orchestrator-directed

**1. Seed pin 9f9386c661f6 instead of 3200aa868371**
- The plan's `project_rules` name 3200aa868371, which is not on GitHub. As the orchestrator instructed, the seed was fetched from published main 9f9386c661f6 with `gh api` into a scratch directory outside the repository. The commit body says `Seeded from nesturbator 9f9386c661f6 src/crc32.c (same author, MIT).`

No other deviations. The writer mutation was swapped for one that actually changes the output; see Issues Encountered.

## Issues Encountered

- The first writer mutation (NLEN mask `0xffff` to `0xfffe`) changed no output bytes, because bit 0 of the complement is already 0 for both block lengths (65,535 and 35,957). It was replaced with a change to the Adler-32 modulus. That made `runner.png` fail at the trailer check and `runner.dump_png` fail at the digest.

## Gate-code tally

| Lines | File |
|---|---|
| 19 | tests/cmake/expect_fail.cmake |
| 38 | tests/cmake/no_skip.cmake |
| 58 | tests/cmake/check_junit.cmake |
| 20 | tests/policy/tests.cmake |
| 69 | cmake/MuhtariTests.cmake |
| 60 | tests/runner/tests.cmake (was 20; it now also holds runner.dump_png and the runner.png registration) |
| **264** | repository-check and registration code, against **315** lines in `src/` |

The tenth-of-`src/` budget applies from phase 3.

## Evidence

- `cmake --workflow --preset ci`: exit 0, "100% tests passed out of 18".
- `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake`: exit 0, "18 tests ran once each".
- `build/ci/runner/muhtari-run --frames 1 --dump-frame 1:build/ci/card.png --hash-frame 1`: exit 0 and printed `frame 1 sha256 9eb011e2…05c4`. Then `test "$(wc -c < build/ci/card.png | tr -d ' ')" = 232645` exited 0, and `sips -g pixelWidth -g pixelHeight build/ci/card.png` exited 0 with 320 and 242.
- `shasum -a 256` gives `1c0b16cb9c92d70869d3aee5cd581627f8902cb528e90f1f1d4252d9288ac956` for `build/ci/card.png`, `build/ci/tests/dump.png` and `build/ci/tests/png_test.png`.
- RED: with a placeholder pin of zeros, `runner.dump_png` failed and printed the actual digest. That digest is now pinned.
- Mutations (described above) each made the tests fail. Each was reverted, and `git diff --quiet` confirmed the file was restored.
- `scripts/hygiene.sh --tree`: exit 0.

## User Setup Required

None. No external service configuration is required.

## Next Phase Readiness

- Plan 06 (exit table, `--version`, `--help`, bench) can put the unwritable-file failure under exit status 1 in its table.
- The libretro host test and the RetroArch screenshot comparison can compare against `--dump-frame` output, pinned at `1c0b16cb…c956`.

## Self-Check: PASSED

- FOUND: runner/png.c, runner/png.h, runner/crc32.c, runner/crc32.h, tests/runner/test_png.c
- FOUND: ab1363a, 45ee8cd (ancestors of HEAD)

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*
