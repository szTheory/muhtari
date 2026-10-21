---
phase: 03-games-play-in-retroarch
plan: 12
subsystem: testing
tags: [openbios, bios, pins, dasm, wasmtime, ci, firmware-scan, fixtures]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "The pinned wasmtime and 7800basic package and build.cmake's stage() recipe (plan 03-05); the release firmware scan"
provides:
  - "pins.txt row 7800openbios-asm at b8bc745a8744 (raw CC0 source, SHA-256 checked by scripts/fetch-pin.sh)"
  - "tests/built/openbios.cmake: fills each incbin line exactly once with $FF or $00, assembles with the package's dasm, checks the image SHA-256"
  - "built.openbios (fixture built_openbios) writes build/<preset>/built/openbios/openbios.bin, 46b5bc2b... on every hosted leg"
  - "built.openbios.self_test: a doubled incbin line fails by name"
  - "Both fill digests in tests/release/firmware.txt, so release.firmware_scan refuses either image"
  - "03-CONTEXT.md D-16 defines K from INPTCTRL EXT going 0 to 1"
affects: [03-13, 03-18, BIOS path, title predicates]

actuals:
  tokens: 4474
  tasks: 2
  commits: 2
plan_head_before: 9ffb92d1136eaa314d3cfe2bf2eea1e2a405946d
plan_head_after: af2d6bc8b0d031e98fa6d4559c908bdcbb8a7b2a

tech-stack:
  added: ["7800 OpenBIOS source (CC0-1.0), fetched as a pin, never committed"]
  patterns:
    - "A fixture built from a pinned source file: fetch-pin row, a -P recipe in stage() shape, a digest check, and a negative control that fails by name"

key-files:
  created:
    - tests/built/openbios.cmake
    - .planning/phases/03-games-play-in-retroarch/03-12-SUMMARY.md
  modified:
    - pins.txt
    - tests/built/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - .github/workflows/ci.yml
    - docs/testing.md
    - tests/release/firmware.txt
    - docs/emulation-tests.md
    - .planning/phases/03-games-play-in-retroarch/03-CONTEXT.md

key-decisions:
  - "Both fill digests live in openbios.cmake, selected by -DFILL; built.openbios builds only the $FF image, and the $00 digest was reproduced once locally"
  - "Configure stops when build/pins/openbios/7800openbios.asm is missing, in the same check and message as wasmtime and 7800basic"
  - "Added built.openbios.self_test, the recipe's only edge (an incbin line not occurring exactly once), as a negative control"

patterns-established:
  - "A non-CC0 binary included by a CC0 source is replaced by a same-size fill, never fetched"

requirements-completed: []

coverage:
  - id: D1
    description: "built.openbios assembles the pinned OpenBIOS source with $FF fill to SHA-256 46b5bc2b716a20a1c40e2b920365c29cbab1594d01da94f4567b240181365cb2 and sets up built_openbios"
    requirement: GAME-03
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^built\\.openbios$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D2
    description: "A source with an incbin line that does not occur exactly once fails, naming the line"
    requirement: GAME-03
    verification:
      - kind: integration
        ref: "tests/built/tests.cmake#built.openbios.self_test (ctest -R '^built\\.openbios\\.self_test$', exit 0)"
        status: pass
    human_judgment: false
  - id: D3
    description: "Every ci.yml job that builds the samples fetches the pin, and built.openbios is green on all six build legs, asan, be32 and wasm32"
    requirement: GAME-03
    verification:
      - kind: other
        ref: "https://github.com/szTheory/muhtari/actions/runs/39364401534 (conclusion success)"
        status: pass
      - kind: other
        ref: "https://github.com/szTheory/muhtari/actions/runs/39364791699 (conclusion success, head commit af2d6bc)"
        status: pass
    human_judgment: false
  - id: D4
    description: "The $FF and $00 fill digests are firmware.txt rows, so the release scan refuses a zip holding either image"
    requirement: GAME-03
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^release\\.firmware_scan(\\.self_test)?$' (exit 0)"
        status: pass
      - kind: other
        ref: "cmake -DZIPS=<zip of openbios.bin, each fill> -DFIRMWARE=tests/release/firmware.txt -DEXPECTED=tests/built/expected.txt -DCARTDB=src/cartdb.c -P tests/cmake/firmware_scan.cmake (exit 1 for both)"
        status: pass
    human_judgment: false
  - id: D5
    description: "docs/emulation-tests.md documents the openbios build with the full $FF digest, and docs/testing.md lists the new fetch"
    requirement: GAME-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^docs\\.' (exit 0)"
        status: pass
    human_judgment: false

duration: 11min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 12: OpenBIOS built in CI Summary

**The CC0 7800 OpenBIOS is now a pinned source, built by `tests/built/openbios.cmake` with the package's dasm into a 16 KiB image of SHA-256 `46b5bc2b...` on every hosted leg. It is exposed as the `built_openbios` fixture. Both fill digests are firmware rows, so the image can never ship.**

## Performance

- **Duration:** 11 min
- **Started:** 2026-10-20T03:24:14Z
- **Completed:** 2026-10-20T03:35:02Z
- **Tasks:** 2
- **Files modified:** 10

## Accomplishments

- `pins.txt` has a `7800openbios-asm` row for the raw source at commit b8bc745a8744. KiloParsec is never fetched.
- `openbios.cmake` replaces `incbin "kiloparsec.bl1"` with `ds.b 8192,$FF` and `incbin "kiloparsec.bl2"` with `ds.b 896,$FF` (or `$00`). It fails unless each line occurs exactly once, then assembles under wasmtime in a fresh directory and checks the digest. The local results match the research: $FF gives `46b5bc2b...`, $00 gives `1ee4c780...`, and the vectors at $3FF8 are `ff f7 37 fb 00 f8 64 fb`.
- `built.openbios` (labels built and portable, TIMEOUT 60, provisional) and `built.openbios.self_test` are added to the ci and wasm32 inventories.
- The build matrix, asan, be32 and wasm32 now fetch the pin. No upload-artifact path touches `build/*/built`.
- `firmware.txt` has the two OpenBIOS SHA-256 rows. A zip holding either image exits 1, with the message `matches OpenBIOS (CI build, $FF fill)` for the $FF image.
- D-16's BIOS-path bullet now defines K by INPTCTRL EXT going from 0 to 1.

## Task Commits

1. **Task 1 (tracer): OpenBIOS builds on every leg** - `2d002dd` (feat). Tracer gate: interactive, end-of-phase, automated-only, so the `<verify>` was re-run. `cmake --workflow --preset ci` exited 0, `built.openbios` passed, `scripts/hygiene.sh --tree` exited 0, and hosted run 39364401534 succeeded on every job.
2. **Task 2: firmware rows, docs, D-16 wording** - `af2d6bc` (docs). Hosted run 39364791699 on this head succeeded on every job.

## Files Created/Modified

- `pins.txt`: the `7800openbios-asm` row with its comment.
- `tests/built/openbios.cmake`: the recipe.
- `tests/built/tests.cmake`: the pin check, `built.openbios`, and `built.openbios.self_test`.
- `tests/inventory/ci.txt` and `tests/inventory/wasm32.txt`: two lines each.
- `.github/workflows/ci.yml`: the fetch in four jobs.
- `docs/testing.md`: the fetch instructions.
- `tests/release/firmware.txt`: two rows.
- `docs/emulation-tests.md`: the `### openbios` section.
- `03-CONTEXT.md`: the D-16 wording.

## Decisions Made

See `key-decisions`.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 2 - Missing test] Negative control for the exactly-once replacement**
- **Found during:** Task 1
- **Issue:** The plan names the exactly-once `incbin` replacement as its only edge, but registers no test for it. Rule 5 requires a test with every behaviour.
- **Fix:** `built.openbios.self_test` writes a copy of the source with `incbin "kiloparsec.bl1"` doubled. Through `expect_fail.cmake` it expects exit 1 and `kiloparsec[.]bl1[^ ]* occurs 2 times`.
- **Files modified:** tests/built/tests.cmake, tests/inventory/ci.txt, tests/inventory/wasm32.txt
- **Commit:** 2d002dd

**2. [Rule 3 - Blocking] docs/testing.md fetch instructions**
- **Found during:** Task 1
- **Issue:** Configure now stops without the OpenBIOS source, so the documented local fetch commands were incomplete.
- **Fix:** Added `scripts/fetch-pin.sh 7800openbios-asm build/pins/openbios` to both places that list the fetches.
- **Commit:** 2d002dd

---

**Total deviations:** 2 auto-fixed (1 missing test, 1 blocking doc).
**Impact on plan:** One extra test and one doc file. The planned shape is otherwise unchanged.

## Issues Encountered

- The self-test regex failed on its first local run. CMake's error output wraps the message, and the `"'` between the file name and `occurs` did not match the literal `.` in the regex. The regex is now `kiloparsec[.]bl1[^ ]* occurs 2 times`, and the test passes.
- No frame or title pin moved. No C code changed.

## Known Stubs

None. The BIOS path that consumes `built_openbios` comes in plan 03-13, as the plan states.

## User Setup Required

None.

## Next Phase Readiness

- Plan 03-13 can boot `build/<preset>/built/openbios/openbios.bin` by requiring the `built_openbios` fixture. It measures K as the frame of the first opcode fetch from cartridge space after INPTCTRL EXT goes from 0 to 1.
- Still open from WINDOWS.md: a valid BIOS is copied, but the machine fast-boots until 03-13.
- Probe PR #12 is still a draft.

---
*Phase: 03-games-play-in-retroarch*
*Completed: 2026-10-20*

## Self-Check: PASSED

tests/built/openbios.cmake, tests/release/firmware.txt and docs/emulation-tests.md exist. Commits 2d002dd and af2d6bc are on HEAD.
