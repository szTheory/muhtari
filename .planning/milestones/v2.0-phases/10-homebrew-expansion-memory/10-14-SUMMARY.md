---
phase: 10-homebrew-expansion-memory
plan: "14"
subsystem: runner
tags: [runner, hsc, exit-status, gap-closure]
requires:
  - phase: 10-homebrew-expansion-memory
    provides: Plans 01-13 runner --hsc-rom and refusal contract
provides:
  - Refused images keep exit 77 and their canonical lines under --hsc-rom
  - HSC attachment decided by the library, not a runner copy of the rule
  - runner.hsc_rom.refused test with 24 anchored checks
affects: [phase-10-verification, BOARD-04, BOARD-05]
tech-stack:
  added: []
  patterns:
    - Ask the core for the firmware-free load decision before reading optional firmware
key-files:
  created: []
  modified:
    - runner/main.c
    - tests/runner/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/runner.md
key-decisions:
  - With --hsc-rom the runner loads once without firmware, reads HSC attachment from muhtari_get_battery, reads the firmware only for an accepted image with HSC attached, and loads again with it.
  - The new test writes its fixtures into its own directory so it never races runner.region.pal.
requirements-completed: [BOARD-04, BOARD-05]
coverage:
  - id: G1
    description: A 2600 image given --hsc-rom exits 77 with its canonical line, restoring the unsupported-board refusal contract and runner exit status 77 (no HSC or firmware message, firmware file not read)
    requirement: none
    verification:
      - kind: integration
        ref: runner.hsc_rom.refused
        status: pass
    human_judgment: false
  - id: G2
    description: A Souper image given --hsc-rom exits 77 with unsupported board souper, so Souper stays explicitly unsupported (phase success criterion 4)
    requirement: none
    verification:
      - kind: integration
        ref: runner.hsc_rom.refused
        status: pass
    human_judgment: false
  - id: G3
    description: A 52 KiB BankSet set with attached HSC given --hsc-rom exits 77 with unsupported board bankset before any firmware read, with missing, wrong-size and valid firmware
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: runner.hsc_rom.refused
        status: pass
    human_judgment: false
  - id: G4
    description: A declared YM2151, Covox or ADPCM image given --hsc-rom exits 77 with unsupported device and the named sound line
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: runner.hsc_rom.refused
        status: pass
    human_judgment: false
  - id: G5
    description: HSC firmware is read only after the library accepts the image and attaches HSC; detached HSC and bad-size firmware on a supported image still exit 1 and valid XM firmware still runs
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: runner.region.pal
        status: pass
      - kind: integration
        ref: runner.hsc_rom.refused
        status: pass
    human_judgment: false
  - id: G6
    description: The accepted-with-firmware double load gives the same frame, audio and count hashes as the library reference
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: diag.expansion
        status: pass
    human_judgment: false
  - id: G7
    description: docs/runner.md states exit 77 without a firmware read for refused images under --hsc-rom and the BankSet refusal as conditional on HSC attaching
    requirement: BOARD-05
    verification:
      - kind: other
        ref: docs/runner.md
        status: pass
    human_judgment: false
status: complete
duration: 20 min
completed: 2026-10-28
plan_head_before: 01bc0cb8a0ad1c99e50b418236c7be638f0d17d6
plan_head_after: 2c3a66fecc33bc644e23e8d32f0f631c97cb2864
actuals:
  tokens: 3200
  tasks: 2
  commits: 2
---

# Phase 10 Plan 14: Refused-image exit status under --hsc-rom Summary

**With --hsc-rom, muhtari-run now lets the library accept or refuse the image first, so 2600, unsupported-board, 52 KiB BankSet with HSC, and YM2151/Covox/ADPCM images exit 77 with only their canonical lines and never have the firmware file read.**

## Owner exception

Phase 10's single gap round was already spent. On 2026-10-28 the owner granted a one-time exception for a second gap round (owner, 2026-10-28, one-time, second gap round for CR-01). This plan is that round; no further fix or gap round was spent.

## Performance

- **Duration:** about 20 min
- **Tasks:** 2
- **Files modified:** 5

## Accomplishments

- runner/main.c load(): the runner-side copy of the attachment rule and the probe-status guard are gone. With --hsc-rom, `muhtari_load` runs first without firmware, `muhtari_get_battery` reports `hsc_attached`, and only an accepted image with HSC attached has its firmware read and is loaded again with it. A refusal goes straight to the existing print and exit mapping.
- The firmware load inherits the firmware-free attachment decision, because battery_prepare attaches HSC whenever hsc_rom is non-NULL and the runner passes firmware only when the first load reported hsc_attached.
- The double load's hash equivalence (diag.expansion) depends on src/boot.c's fast boot calling muhtari__cart_reset after the boot clock origin is set, so the second load starts the cart as a single load would.
- runner.hsc_rom.refused: 24 anchored exit-77 checks (five images x four firmware/--hsc configurations, plus four for the 52 KiB BankSet set), registered with inventory rows in ci.txt and wasm32.txt.
- docs/runner.md: the --hsc-rom row and the XM paragraph state the refused-image exit 77 without a firmware read, with the BankSet clause conditional on HSC attaching.

## Task Commits

1. **Task 1: refused image with --hsc-rom exits 77 (red test, then runner fix)** - e8a35a5 (fix)
2. **Task 2: docs and full workflow** - 2c3a66f (docs)

## Evidence

All local; run URL N/A-local.

- Red: `ctest --preset ci -R '^(runner[.](hsc_rom[.]refused|region[.]pal)|diag[.]expansion)$' --no-tests=error --output-on-failure` against the unfixed runner exited 8 (ctest: 1 failed); runner.hsc_rom.refused reported "got exit 1" with "HSC firmware requires attached HSC" for 2600-4k.
- Green: the same command after the fix exited 0, 4 run (including the probe_images fixture), 0 failed.
- `cmake --preset hygiene && ctest --preset hygiene -R '^hygiene[.]format$' --no-tests=error --output-on-failure` exited 0 (1 test passed).
- `ctest --preset ci -R '^(runner[.]|docs[.])' --no-tests=error --output-on-failure` exited 0 (82 passed, 0 failed).
- `cmake --workflow --preset ci` exited 0.
- Manual: `build/ci/runner/muhtari-run --frames 1 --hsc on --hsc-rom build/ci/runner-hsc-refused/missing.bin build/ci/runner-hsc-refused/ym2151.a78` exit 77 with the two sound lines; `build/ci/runner/muhtari-run --frames 1 --hsc-rom build/ci/runner-hsc-refused/hsc.bin build/ci/runner-hsc-refused/xm1.a78` exit 0.

## Deadline (provisional)

- runner.hsc_rom.refused, three local runs (macOS arm64): 0.08 s, 0.09 s, 0.08 s (slowest 0.09 s; ctest rounds to 0.01 s).
- runner.region.pal, local: 0.12 s (run alone), 0.14 s before the fix when run in parallel; unchanged after the fix (0.12 to 0.13 s). Its deadline and lines are untouched.
- Estimate = slowest local 0.09 s x 0.98 s / 0.12 s = 0.735 s, where 0.98 s is runner.region.pal's slowest complete Windows arm64 hosted figure (not the slowest hosted lane). Committed TIMEOUT = ceil(2 x 0.735) = 2.
- The deadline is provisional and is revisited at the first complete hosted run of runner.hsc_rom.refused.

## Deviations from Plan

None - plan executed as written.

Minor: the case chain's header comment gained a `-DWORK` usage line, and the 24 checks use a small local function over four configurations instead of 24 literal lines.

## Issues Encountered

None.

## Known Stubs

None.

## Threat Flags

None. No new endpoint, auth path or trust-boundary file access; the runner reads the same firmware file, later and only for an accepted image.

## Self-Check: PASSED

- runner/main.c, tests/runner/tests.cmake, tests/inventory/ci.txt, tests/inventory/wasm32.txt, docs/runner.md present and changed.
- Commits e8a35a5 and 2c3a66f are ancestors of HEAD.
