---
phase: 01-a-test-frame-in-retroarch
plan: 04
subsystem: host
tags: [palette, bsd-3, a7800, rgb565, xrgb8888, notices, provenance]

requires:
  - phase: 01-03
    provides: "test harness, inventory check, JUnit completeness check, cmake --workflow --preset ci with 15 tests"
provides:
  - "host/palette.c and host/palette.h: warm NTSC palette (a7800 BSD-3 seed), muhtari__palette_rgb (0x00RRGGBB) and muhtari__palette_rgb565 (r>>3, g>>2, b>>3)"
  - "muhtari_host static library (top-level only, PIC, hidden visibility), linked by muhtari-run"
  - "host.palette test (labels host, portable); cmake --workflow --preset ci passes with 16 tests"
  - "THIRD-PARTY-NOTICES.md with the a7800 palette section"
affects: [01-05, 01-06, 01-07, 01-09]

actuals:
  tokens: 5030
  tasks: 2
  commits: 3
plan_head_before: 87211daa86f731952be7170a3b1bf224bc9f6dd9
plan_head_after: add157108a256cc1f28f36fc20d599d7fc12f7b2

tech-stack:
  added: []
  patterns:
    - "A seeded table is extracted mechanically by a script kept outside the repo and cross-checked by a second, independent pipeline over the same upstream file"
    - "Host helpers live in muhtari_host, outside the core and its hash, built PIC with hidden visibility so the libretro module can link them"

key-files:
  created:
    - host/palette.c
    - host/palette.h
    - tests/host/test_palette.c
    - tests/host/tests.cmake
    - THIRD-PARTY-NOTICES.md
  modified:
    - CMakeLists.txt
    - tests/tests.cmake
    - tests/inventory/ci.txt
    - PROVENANCE.md
    - ASSET_POLICY.md

key-decisions:
  - "D-03 holds on the fetched bytes: $46 = 0xAF5163 (red largest) and $C6 = 0x469A46 (green largest), so the test card and its pinned hash are unchanged"
  - "The BSD-3 text in THIRD-PARTY-NOTICES.md is SPDX's canonical BSD-3-Clause text, with the copyright line naming the upstream copyright-holders and no years; MAME's template was not fetched because the plan forbids opening any other MAME or a7800 file"
  - "nesturbator citations in PROVENANCE.md and ASSET_POLICY.md now pin published main 9f9386c661f6; PV.12's check_archives.cmake range is L32-50, the per-component allowlist"

patterns-established:
  - "Palette spot tests type expected colours as literals from the upstream rows, never read back from the table"

requirements-completed: []

coverage:
  - id: D1
    description: "Warm NTSC palette seeded from the pinned BSD-3 a7800 file: the 16 ntsc_hue*_warm rows, 256 entries indexed by the native byte"
    requirement: FRAME-03
    verification:
      - kind: unit
        ref: "tests/host/test_palette.c#host.palette"
        status: pass
      - kind: other
        ref: "awk extraction and an independent sed/grep pipeline over the upstream file give the same 256-value SHA-256 8a62ecac1215f04d38dd49896e924ed233475d8c4240e3a8c2326285612e01c6"
        status: pass
    human_judgment: false
  - id: D2
    description: "D-03: red is the largest channel of $46 and green the largest of $C6 in the warm table"
    requirement: FRAME-03
    verification:
      - kind: unit
        ref: "tests/host/test_palette.c#host.palette"
        status: pass
    human_judgment: false
  - id: D3
    description: "muhtari__palette_rgb565 applies r>>3, g>>2, b>>3 to the XRGB8888 entry"
    requirement: FRAME-04
    verification:
      - kind: unit
        ref: "tests/host/test_palette.c#host.palette"
        status: pass
    human_judgment: false
  - id: D4
    description: "Changing one byte of the $46 entry makes host.palette fail"
    requirement: FRAME-03
    verification:
      - kind: other
        ref: "sed s/0xaf5163/0xaf5164/ host/palette.c, then ctest --preset ci -R host.palette fails naming 0xaf5164 != 0xaf5163; reverted"
        status: pass
    human_judgment: false
  - id: D5
    description: "THIRD-PARTY-NOTICES.md carries the a7800 palette section with path, 40-hex commit, upstream file SHA-256 and the BSD-3 text"
    requirement: FRAME-04
    verification:
      - kind: other
        ref: "grep -c 'BSD-3-Clause|7a2afdc1ea08fc331b16b750d8c1f02d4ef62fc8|Redistribution and use in source and binary forms' THIRD-PARTY-NOTICES.md prints 3; scripts/hygiene.sh --tree exits 0"
        status: pass
    human_judgment: false

duration: 4min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 04: Warm NTSC palette seed Summary

**The warm NTSC palette (Trebor's Pro Palettes V7, 26.7 degrees) is extracted by script from a7800's BSD-3 `a7800.cpp` at 7a2afdc1ea08 into a new `muhtari_host` library with XRGB8888 and RGB565 lookups. `host.palette` checks D-03, and the BSD-3 notice is in `THIRD-PARTY-NOTICES.md`.**

## Performance

- **Duration:** 4 min
- **Started:** 2026-10-16T14:56:33Z
- **Completed:** 2026-10-16T15:00:21Z
- **Tasks:** 2
- **Files modified:** 10

## Accomplishments

- One BSD-3 file was fetched with `curl -fsSL` (exit 0) into a `mktemp -d` scratch directory outside the repository. Its line 1 is `// license:BSD-3-Clause` and its holders line names Dan Boris, Fabio Priuli, Mike Saarna and Robert Tuccitto. Its SHA-256 is `504b221d6dd1726cabd600b795a0243a6353d4642827f7d1bc438ea1d8ce31c2` (82,831 bytes).
- An awk script took the block "26.7 PHASE SHIFT - LUM3 from TREBORS PRO PALETTES V7" up to `#define pal_hue0_warm`. It checks that the hues are in order and that there are 256 entries. A separate `sed | grep -o` pipeline produced the same value list, with SHA-256 `8a62ecac…01c6` from both.
- D-03 holds, so nothing about the test card changes. `$46` = 0xAF5163 (R 175 > B 99 > G 81) and `$C6` = 0x469A46 (G 154 largest), as research F-02 predicted.
- `host.palette` checks the D-03 orderings, the spot values $00, $0F, $46 and $C6 typed as literals, and the RGB565 values of $0F, $46 and $C6 worked out from the truncation rule.
- `THIRD-PARTY-NOTICES.md` is new. The PROVENANCE §4 Palettes row now says that only the warm NTSC table is seeded so far.

## Task Commits

1. **Task 1: End-to-end palette (tracer)** - `c5b6b1a` (feat)
2. **Task 2: Notices for the palette seed** (plus the nesturbator pin move) - `5652561` (docs)
3. **Deviation: host library PIC and hidden visibility** - `add1571` (fix)

## Files Created/Modified

- `host/palette.c` - §5a header, 256-entry warm NTSC table, the two lookups
- `host/palette.h` - declarations of `muhtari__palette_rgb` and `muhtari__palette_rgb565`
- `tests/host/test_palette.c` - host.palette
- `tests/host/tests.cmake` - builds muhtari-test-palette, registers host.palette
- `CMakeLists.txt` - `muhtari_host` target (top-level only), linked by muhtari-run
- `tests/tests.cmake` - includes the host tests
- `tests/inventory/ci.txt` - `host.palette	host,portable`
- `THIRD-PARTY-NOTICES.md` - intro and the "a7800 palette" section
- `PROVENANCE.md` - Palettes row note; nesturbator pins moved to 9f9386c661f6; PV.12 range corrected
- `ASSET_POLICY.md` - AP.07 pin moved to 9f9386c661f6

## Decisions Made

- D-03 passes on the fetched bytes, so `$46`/`$C6` stay and neither `src/testcard.c` nor its hashes change.
- The licence text is SPDX's canonical BSD-3-Clause (fetched with `gh api` from spdx/license-list-data). The plan forbids opening any MAME file, MAME's `docs/legal` included. The copyright line names the holders without years (PROVENANCE §5b).
- `muhtari-run` links `muhtari_host` now, as the plan says. It does not call the palette yet; the image output comes in a later plan.

## Deviations from Plan

### Auto-fixed Issues

**1. [Orchestrator deviation] Moved nesturbator pins to published main 9f9386c661f6**
- **Found during:** Task 2, at the orchestrator's instruction. The commits API returns 422 for 3200aa868371, so it is not on GitHub.
- **Fix:** Four PROVENANCE.md pins (the §2 row, the two §4 seed rows and PV.12) and ASSET_POLICY.md AP.07 now read `9f9386c661f6`. `gh api` confirmed that every cited file exists at 9f9386c661f6: PROVENANCE.md, THIRD-PARTY-NOTICES.md, CMakeLists.txt, tests/cmake/check_archives.cmake, src/cpu.c, ASSET_POLICY.md, tests/roms/manifest.txt and scripts/hygiene.sh. CMakeLists.txt L115–138 is still the LICENSE and notices install block. `tests/cmake/check_archives.cmake` has only 112 lines, so the cited L400–420 could not exist; it now cites L32–50, the per-component allowlist that PROVENANCE §5c refers to. Line 36 says every commit is by the owner's account or its release bot. That still holds at the new pin: the authors are szTheory and nesturbator-release[bot], and the committers szTheory and web-flow. Planning docs keep their historical citations.
- **Files modified:** PROVENANCE.md, ASSET_POLICY.md
- **Committed in:** 5652561

**2. [Rule 2 - Missing critical] `muhtari_host` built PIC with hidden visibility**
- **Found during:** the Task 2 pin check. nesturbator's matching `nesturbator_host` target (CMakeLists.txt at 9f9386c661f6) sets both properties so the library can link into the libretro module.
- **Issue:** the plan says the libretro adapter will link `muhtari_host`. Without PIC, a static library fails to link into a shared module on Linux. Default visibility would also export `muhtari__palette_*` from the core.
- **Fix:** `C_VISIBILITY_PRESET hidden`, `POSITION_INDEPENDENT_CODE ON`
- **Verification:** `cmake --workflow --preset ci` exit 0, 16 tests
- **Committed in:** add1571

---

**Total deviations:** 2 (1 at the orchestrator's instruction, 1 Rule 2)
**Impact on plan:** Both are small. No scope creep.

## Issues Encountered

- The first awk run stopped with "hue out of order: 0" because a string was compared with an unset variable. Comparing numerically fixed it. The script's own check caught this, and no output was used.

## Gate-code tally

| Lines | File |
|---|---|
| 19 | tests/cmake/expect_fail.cmake |
| 38 | tests/cmake/no_skip.cmake |
| 58 | tests/cmake/check_junit.cmake |
| 20 | tests/policy/tests.cmake |
| 69 | cmake/MuhtariTests.cmake |
| 20 | tests/runner/tests.cmake |
| **224** | repository-check and registration code (unchanged by this plan), against **315** lines in `src/` |

This plan adds no gate code. `tests/host/tests.cmake` (8 lines) only registers a test, like `tests/core/tests.cmake`.

## Evidence

- `curl -fsSL https://raw.githubusercontent.com/7800-devtools/a7800/7a2afdc1ea08fc331b16b750d8c1f02d4ef62fc8/src/mame/drivers/a7800.cpp -o <mktemp -d>/a7800.cpp`: exit 0; `shasum -a 256` gives `504b221d6dd1726cabd600b795a0243a6353d4642827f7d1bc438ea1d8ce31c2`.
- `cmake --workflow --preset ci`: exit 0, "100% tests passed out of 16" (the wording CMake 4.4.3 uses).
- `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake`: exit 0, "16 tests ran once each".
- Mutation: changing $46 to 0xaf5164 makes `ctest --preset ci -R '^host\.palette$'` report "1 tests failed out of 1" (`c46 == 0xAF5163u failed: 0xaf5164 != 0xaf5163`). The file was then restored and `cmp` confirmed it matches.
- `scripts/hygiene.sh --tree`: exit 0. `git status --porcelain` lists no a7800 file.

## User Setup Required

None. No external service configuration is required.

## Next Phase Readiness

- Plans 05 and 06 (the runner's image) and plan 07 (the libretro adapter) can link `muhtari_host` and call `muhtari__palette_rgb` and `muhtari__palette_rgb565`.
- Plan 07 appends the libretro.h section to `THIRD-PARTY-NOTICES.md`. Plan 09 ships that file in every archive.

## Self-Check: PASSED

- FOUND: host/palette.c, host/palette.h, tests/host/test_palette.c, tests/host/tests.cmake, THIRD-PARTY-NOTICES.md
- FOUND: c5b6b1a, 5652561, add1571 (ancestors of HEAD)

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*
