---
phase: 02-the-cpu-matches-the-public-vectors
plan: 04
subsystem: testing
tags: [decimal-mode, clark, dasm, wasmtime, manifest, fixtures, hygiene]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 2's decimal ADC/SBC/ARR and muhtari-test-cpu, plan 3's fixture rows
provides:
  - tests/cpu/decimal/clark.s, Appendix B as printed, with its public-domain line and URL
  - tests/cpu/decimal/clark-6502.bin and clark-65c02.bin with manifest rows pinned to ca244aa
  - muhtari-test-cpu decimal.clark FILE EXPECTED, cpu.decimal.clark and cpu.decimal.clark.self_test
  - tests/cmake/rom_hash_coverage.cmake, policy.rom_hash_coverage and its self test
affects: [the loader and fuzz plan's tests/fuzz regress rows, the docs plan's docs/testing.md, the D-31 timeout measurement, the phase 3 rebuild-and-compare of the Clark bytes]

actuals:
  tokens: 5637
  tasks: 2
  commits: 3
plan_head_before: 1ec0f0c364e45e35ffd088a29450c7225ab49cbc
plan_head_after: 49d223cf9d29391fa0ff9f76835cd134d3e40249

tech-stack:
  added: []
  patterns:
    - "muhtari-test-cpu cases take arguments: a case table entry names its argument count, and a wrong count is a usage error (exit 2)"
    - "A source-derived fixture is committed in two steps: the source first, then the bytes with a manifest row whose pin is that commit"

key-files:
  created:
    - tests/cpu/decimal/clark.s
    - tests/cpu/decimal/clark-6502.bin
    - tests/cpu/decimal/clark-65c02.bin
    - tests/cmake/rom_hash_coverage.cmake
  modified:
    - tests/cpu/test_cpu.c
    - tests/cpu/tests.cmake
    - tests/hygiene/tests.cmake
    - tests/roms/manifest.txt
    - .gitignore
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/inventory/hygiene.txt

key-decisions:
  - "The Clark harness returns through a sentinel RTS address of $FFF0 (pushed as $FFEF with S = $FD) and fails on a JAM or at 150,000,000 cycles"
  - "rom_hash_coverage globs the working tree under ROOT, not git's index, and exempts only tests/roms/manifest.txt; the fuzz cap counts every file under tests/fuzz"
  - "The coverage self test corrupts the sample row of a configure-time copy of the real manifest, so every other row is parsed and checked in the same run"

patterns-established:
  - "Manifest checks: cmake -DROOT=<root> -DMANIFEST=<file> -P tests/cmake/rom_hash_coverage.cmake"

requirements-completed: []

coverage:
  - id: D1
    description: "Clark's decimal test as printed runs on SALLY over all 262,144 cases, comparing A, C, N, V and Z, and leaves ERROR = 0 after 53,953,831 cycles"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.decimal\\.clark$'"
        status: pass
      - kind: other
        ref: "src/cpu.c decimal ADC Z taken from the decimal sum instead of the binary one, then muhtari-test-cpu decimal.clark clark-6502.bin 0 (exit 1, stopped at N1 $f9 N2 $00 C 1); src/cpu.c restored"
        status: pass
    human_judgment: false
  - id: D2
    description: "The 65C02 build of the same listing must fail on SALLY: ERROR = 1 after 64,940 cycles"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.decimal\\.clark\\.self_test$'"
        status: pass
      - kind: other
        ref: "muhtari-test-cpu decimal.clark clark-6502.bin 1 (exit 1, prints N1, N2 and C)"
        status: pass
    human_judgment: false
  - id: D3
    description: "Both Clark builds are committed, not ignored, each 343 bytes from the pinned dasm.wasm, with LicenseRef-PublicDomain manifest rows pinned to the source commit"
    requirement: CPU-01
    verification:
      - kind: other
        ref: "git check-ignore -q tests/cpu/decimal/clark-6502.bin (exit 1); grep -n '^tests/cpu/decimal/clark-' tests/roms/manifest.txt (two rows)"
        status: pass
      - kind: other
        ref: "scripts/hygiene.sh --tree (exit 0)"
        status: pass
    human_judgment: false
  - id: D4
    description: "policy.rom_hash_coverage passes on the six rows, and its self test fails naming the sample when the sample's digest is wrong"
    requirement: CPU-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/hygiene -R '^policy\\.rom_hash_coverage'"
        status: pass
      - kind: other
        ref: "cmake -P tests/cmake/rom_hash_coverage.cmake on a scratch ROOT with an unlisted tests/roms file and fuzz regress file, a missing listed file, a 524,289-byte fuzz tree and a two-field row (exit 1 each, naming the path)"
        status: pass
    human_judgment: false
  - id: D5
    description: "The probe PR's ci.yml run on the head is green on every leg with the two Clark tests"
    requirement: CPU-01
    verification:
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39182041117"
        status: pass
    human_judgment: false

duration: 9min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 4: Clark's decimal test and the manifest coverage gate Summary

**Bruce Clark's public-domain decimal test now runs on SALLY. It is transcribed from the 6502.org tutorial's Appendix B and assembled by the pinned 7800basic `dasm.wasm` into two 343-byte builds. The 6502 build leaves ERROR = 0 over all 262,144 cases, and its 65C02 control must leave ERROR = 1. `policy.rom_hash_coverage` now ties every manifest row to its file's SHA-256 and every fixture to a row. Probe PR #9 is green on every leg.**

## Performance

- **Duration:** 9 min
- **Started:** 2026-10-19T00:36:30Z
- **Completed:** 2026-10-19T00:45:27Z
- **Tasks:** 2
- **Files modified:** 12

## Accomplishments

- **Transcription:** `tests/cpu/decimal/clark.s` holds Appendix B exactly as printed, after a header with the page URL, a line on the 65C02 substitution (Appendix D), equates for the 17 bytes at $00-$10 (N2H takes two) and `ORG $2000`. It keeps the listing's "Written by Bruce Clark.  This code is public domain." line. It has no SPDX line and does not mention Dormann.
- **Bytes:** `clark-6502.bin` has SHA-256 1d4e8f0e...f083c and `clark-65c02.bin` has 8bc70bb4...448e1. Both match research F4, and the two differ from byte 43. Their manifest rows read `this-repo`, pin `ca244aa36b8f4e870bd3ee68935abfe474f274a7` (the source commit), `LicenseRef-PublicDomain`, notice `-`. `.gitignore` gains `!/tests/cpu/decimal/*.bin`.
- **Harness:** `muhtari-test-cpu decimal.clark FILE EXPECTED`:
  - loads FILE at $2000, pushes $FFEF so TEST's RTS lands on $FFF0, and steps with the cycle log reset every step;
  - fails on a JAM or at 150,000,000 cycles;
  - requires ERROR ($04) = EXPECTED, and prints N1, N2 and C (Y) on a mismatch.
- **Coverage gate:** `rom_hash_coverage.cmake` (CMake `-P`, 69 lines) fails, naming the path, on:
  - a malformed row;
  - a row listed twice;
  - a non-hex digest;
  - a missing file;
  - a wrong SHA-256;
  - a file under `tests/roms/` or a `tests/fuzz/**/regress/` directory with no row;
  - `tests/fuzz/` larger than 524,288 bytes.

## Recorded one-off checks (AGENTS rule 6)

- **Tutorial fetch:** `curl -sSL http://www.6502.org/tutorials/decimal_mode.html` gets a 404 page for curl's default user agent (this happens at the site root too). With a browser `-A` string, the page was fetched into scratch (HTTP 200, 48,913 bytes, SHA-256 83215024...703df). The listing is lines 880-1147 of the page, with HTML entities decoded.
- **Package:** `7800basic-0.41-wasm.tar.gz` into its own scratch directory; `shasum -a 256` gave 62ef0a562541bff3f813c038a4609abc89ab125ebda3a371f57de2821e7a7fe7, the digest the plan names. Only `dasm.wasm` was run; `7800basic.sh` was not opened.
- **wasmtime:** `scripts/fetch-pin.sh wasmtime-aarch64-macos build/pins/wasmtime` exit 0; `wasmtime --version` printed 48.0.6.
- **Assembly (in scratch copies of clark.s):**
  - `wasmtime run --dir=. dasm.wasm clark.s -f3 -oclark-6502.bin` printed "Complete. (0)", exit 0, 343 bytes.
  - `wasmtime run --dir=. dasm.wasm clark.s -f3 -oclark-65c02.bin`, on a copy with `JSR A65C02` and `JSR S65C02` substituted (`diff` shows exactly those two lines), printed "Complete. (0)", exit 0, 343 bytes.
- **Clark run time:** 0.09 s locally (53,953,831 cycles). On run 39182041117:
  - be32 3.78 s, macos-x64 0.95 s, wasm32 0.76 s, asan 0.61 s;
  - windows-x64 0.38 s, windows-arm64 0.33 s;
  - linux-x64 0.24 s, linux-arm64 0.22 s, macos-arm64 0.19 s.

  TIMEOUT 60 stays provisional until the D-31 measurement.
- **Negative controls:**
  - The Z mutation in src/cpu.c failed `decimal.clark` (exit 1), and src/cpu.c was then restored with `git checkout -- src/cpu.c`.
  - Expecting 1 from the 6502 build exited 1.
  - A missing argument exited 2.
  - The five coverage faults on a scratch ROOT each exited 1, naming the path.
- **Local lanes:**
  - `cmake --workflow --preset ci` exit 0 (332 tests).
  - `ctest --test-dir build/ci -R '^cpu\.decimal\.'` exit 0.
  - `cmake --workflow --preset hygiene` exit 0 (20 tests).
  - `scripts/hygiene.sh --tree` exit 0.
  - `clang-format --dry-run --Werror tests/cpu/test_cpu.c` (21) exit 0.

## Hosted evidence

- **Run on head `49d223c`:** https://github.com/szTheory/muhtari/actions/runs/39182041117. `gh run watch --exit-status` exited 0, and `CI required` concluded `success`. No job concluded other than success or skipped.

## Task Commits

1. **Task 1: Clark end to end:** `ca244aa` (test, the source), `9499ad8` (test, the bytes, rows, harness and registrations)
2. **Task 2: the manifest coverage gate:** `49d223c` (test)

## Files Created/Modified

- `tests/cpu/decimal/clark.s`: the transcription
- `tests/cpu/decimal/clark-{6502,65c02}.bin`: the assembled bytes
- `tests/cpu/test_cpu.c`: the `decimal.clark` case; cases now take arguments
- `tests/cpu/tests.cmake`: `cpu.decimal.clark` and `.self_test` (labels vectors, portable; TIMEOUT 60)
- `tests/roms/manifest.txt`: two rows
- `.gitignore`: the decimal bins negation and its comment
- `tests/cmake/rom_hash_coverage.cmake`: the coverage check
- `tests/hygiene/tests.cmake`: `policy.rom_hash_coverage` and `.self_test`
- `tests/inventory/{ci,wasm32}.txt`: two lines each; `tests/inventory/hygiene.txt`: two lines

## Decisions Made

- The sentinel return is $FFF0. The listing's code ends below $2160, so PC cannot reach $FFF0 by any other path.
- The coverage check reads the working tree under ROOT, so a stray local file under `tests/roms/` fails locally just as a committed one fails in CI.
- The self test's bad manifest is a configure-time copy of the real one, with `CMAKE_CONFIGURE_DEPENDS` on the manifest. A changed row then reconfigures, and every other row is still checked in the failing run.

## Deviations from Plan

None. The plan executed as written. `tests/cpu/vector_bus.c` is in the plan's file list, but it needed no change: the Clark case resets the log every step through the existing `vector_machine_reset_log`.

## Issues Encountered

- 6502.org answers 404 to curl's default user agent. A browser user agent fetched the page. The plan's URL redirects to `https://6502.org/tutorials/decimal_mode.html`.

## Known Stubs

None.

## User Setup Required

None.

## Next Phase Readiness

- The loader and fuzz plan's `tests/fuzz/loader/regress/*` files will fail `policy.rom_hash_coverage` until each has its row, as D-26 intends.
- CPU-01 and CPU-02 are not marked complete; later plans in this phase finish them.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files: tests/cpu/decimal/clark.s, clark-6502.bin, clark-65c02.bin, tests/cmake/rom_hash_coverage.cmake: all present
- Commits ca244aa, 9499ad8, 49d223c: all ancestors of HEAD; `git rev-list --count 1ec0f0c..HEAD` = 3

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
