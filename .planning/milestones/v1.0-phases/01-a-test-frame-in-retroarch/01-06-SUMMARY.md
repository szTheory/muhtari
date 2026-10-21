---
phase: 01-a-test-frame-in-retroarch
plan: 06
subsystem: runner
tags: [runner, bench, percentiles, signals, exit-status, docs]

requires:
  - phase: 01-05
    provides: "muhtari-run with --frames, --hash-frame and --dump-frame N:FILE"
provides:
  - "muhtari-run --bench N: nine-line report (frames, fps_milli, p50_us to p99_999_us, max_us) with nearest-rank percentiles in parts per million"
  - "runner/clock.c muhtari__clock_ns (CLOCK_MONOTONIC, QueryPerformanceCounter on Windows)"
  - "runner/stop.c muhtari__stop_install and muhtari__stop_requested: SIGINT or SIGTERM ends the run after a whole frame, exit 0"
  - "runner/exit_status.h: const table of statuses 0 done, 1 failure, 2 usage, printed by --help"
  - "--help and --version; hash frames sorted and de-duplicated before the run"
  - "docs/runner.md with options, output formats, exit table and the test-card section"
  - "12 new tests; cmake --workflow --preset ci passes 30"
affects: [01-07, 01-13, 01-19]

actuals:
  tokens: 9975
  tasks: 3
  commits: 4
plan_head_before: c1032d58c82c61f1959739b8c6f58d19ed5e0716
plan_head_after: 1ea117591c84b8cd5d2f137b903ff9a3a90bda56

tech-stack:
  added: []
  patterns:
    - "Runner command-line checks dispatch on -DCASE in tests/runner/tests.cmake; exit-status cases include tests/cmake/expect_fail.cmake from a CMake function"
    - "A doc table that must match a binary is checked by running the binary and finding each printed line, whole, in the doc"

key-files:
  created:
    - runner/bench.c
    - runner/bench.h
    - runner/clock.c
    - runner/clock.h
    - runner/stop.c
    - runner/stop.h
    - runner/exit_status.h
    - tests/runner/test_runner.c
    - tests/cmake/exit_table.cmake
    - docs/runner.md
  modified:
    - runner/main.c
    - CMakeLists.txt
    - tests/runner/tests.cmake
    - tests/inventory/ci.txt

key-decisions:
  - "--bench N is an alternative to --frames N (give one, once); only muhtari_run_frame is timed, and the report is nine key value lines"
  - "The exit table is a const struct array in runner/exit_status.h; runner.exit.table requires every --help table line verbatim in docs/runner.md"
  - "--version prints the library version from muhtari_get_versions, which CMake takes from version.txt"
  - "--help and --version must be the only argument; anything else with them is a usage error"

patterns-established:
  - "fps from integer nanoseconds by decimal long division, so no intermediate exceeds ten times the divisor"

requirements-completed: [FRAME-03, FRAME-09]

coverage:
  - id: D1
    description: "muhtari-run --bench N prints frames, fps_milli, p50_us, p90_us, p99_us, p99_9_us, p99_99_us, p99_999_us and max_us, in that order"
    requirement: FRAME-09
    verification:
      - kind: unit
        ref: "tests/runner/test_runner.c#runner.bench.format"
        status: pass
      - kind: other
        ref: "build/ci/runner/muhtari-run --bench 3000 exits 0 and prints the nine lines starting frames 3000"
        status: pass
    human_judgment: false
  - id: D2
    description: "Nearest-rank percentiles in integer parts per million with a ceiling: n = 1 gives rank 1 for every percentile, n = 300,000 gives p99.999 rank 299,997 and p50 rank 150,000, n = 7 gives p90 rank 7"
    requirement: FRAME-09
    verification:
      - kind: unit
        ref: "tests/runner/test_runner.c#runner.bench.percentile"
        status: pass
      - kind: other
        ref: "grep -cE '\\b(float|double)\\b' runner/bench.c prints 0; muhtari-run --bench 1 prints equal p50_us, p99_999_us and max_us"
        status: pass
    human_judgment: false
  - id: D3
    description: "SIGINT or SIGTERM sets a volatile sig_atomic_t flag; the loop stops after a whole frame, flushes and exits 0"
    requirement: FRAME-03
    verification:
      - kind: unit
        ref: "tests/runner/test_runner.c#runner.stop"
        status: pass
      - kind: other
        ref: "muhtari-run --frames 2000000000 --hash-frame 1 sent SIGTERM after 0.5 s exits 0 with the frame 1 line on stdout"
        status: pass
    human_judgment: false
  - id: D4
    description: "Repeated or unordered --hash-frame options print one line per distinct frame in ascending order"
    requirement: FRAME-03
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#runner.hash.repeat"
        status: pass
    human_judgment: false
  - id: D5
    description: "No arguments, an unknown option, a positional argument, --frames 0, --hash-frame 0, a --hash-frame beyond --frames and a malformed --dump-frame exit 2 with a usage line on stderr; an unwritable --dump-frame path exits 1"
    requirement: FRAME-03
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#runner.usage.no_args"
        status: pass
      - kind: integration
        ref: "tests/runner/tests.cmake#runner.usage.unknown"
        status: pass
      - kind: integration
        ref: "tests/runner/tests.cmake#runner.usage.positional"
        status: pass
      - kind: integration
        ref: "tests/runner/tests.cmake#runner.usage.frame_range"
        status: pass
      - kind: integration
        ref: "tests/runner/tests.cmake#runner.usage.dump_syntax"
        status: pass
      - kind: integration
        ref: "tests/runner/tests.cmake#runner.exit.failure"
        status: pass
    human_judgment: false
  - id: D6
    description: "muhtari-run --version prints muhtari-run followed by the contents of version.txt"
    requirement: FRAME-03
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#runner.version"
        status: pass
    human_judgment: false
  - id: D7
    description: "--help prints the exit-status table from a const struct array, and docs/runner.md holds the same lines; changing one word in the doc table fails the check"
    requirement: FRAME-03
    verification:
      - kind: integration
        ref: "tests/cmake/exit_table.cmake#runner.exit.table"
        status: pass
      - kind: other
        ref: "sed changed written to saved in the docs failure row, ctest --preset ci -R runner.exit.table failed naming the line, reverted and passed"
        status: pass
    human_judgment: false
  - id: D8
    description: "docs/runner.md explains for users what each test-card element reveals, that cell (r, c) is colour $rc, and that the PNG is unscaled 320x242 RGB with no aspect metadata"
    requirement: FRAME-03
    verification:
      - kind: other
        ref: "grep finds ## Test card and $rc in docs/runner.md; grep -nE for planning IDs prints nothing"
        status: pass
      - kind: other
        ref: "Agent check 2026-10-16 (01-UAT.md test 1): every position, size and colour the section states matches card.png from muhtari-run --frames 1 --dump-frame 1:card.png pixel by pixel (exit 0), and the agent read the section against the card"
        status: pass
    human_judgment: false

duration: 6min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 06: Runner bench, stop and exit table Summary

**`muhtari-run --bench N` reports fps and nearest-rank p50 to p99.999 and maximum microseconds per frame, using integer parts-per-million ranks. SIGINT or SIGTERM ends a run after a whole frame with exit 0. `--help` prints a `const` exit table that `docs/runner.md` must repeat line for line, and `--version` prints the version from `version.txt`.**

## Performance

- **Duration:** 6 min
- **Started:** 2026-10-16T15:09:01Z
- **Completed:** 2026-10-16T15:15:08Z
- **Tasks:** 3
- **Files modified:** 14

## Accomplishments

- `runner/bench.c` keeps one `uint32_t` microsecond sample per frame on the heap, so 300,000 frames take 1.2 MB. It sorts the samples with `qsort` and picks rank `ceil(n * ppm / 1,000,000)`, clamped to 1..n, all in integers. fps × 1000 is computed by decimal long division of `frames × 10^12` by the total nanoseconds, so no intermediate overflows.
- Only `muhtari_run_frame` is timed. Hashing and PNG writing are left out of the samples and the total.
- `runner/stop.c` is 34 lines. It uses ISO C `signal` for SIGINT and SIGTERM and keeps a signal that was inherited as ignored ignored. The loop checks the flag after the frame's hash and PNG are written. A bench run that is stopped reports the frames that ran.
- `runner/exit_status.h` holds an enum and a `const` struct array, with no X-macro. `main.c` returns the enum names.
- `docs/runner.md` covers the synopsis, options, the three output formats, stopping, the exit table, and a test-card section that explains each element and its colour numbers.

## Task Commits

1. **Task 1: End-to-end `--bench N` (tracer)** - `0b2ee82` (feat)
2. **Task 2: Stop flag, exit table, usage and version** - `3b3abfe` (test, RED), `751f93c` (feat, GREEN)
3. **Task 3: docs/runner.md and the exit-table equality test** - `1ea1175` (docs)

## Files Created/Modified

- `runner/clock.c`, `runner/clock.h` - monotonic nanoseconds (seed)
- `runner/bench.c`, `runner/bench.h` - ranks, microsecond samples and the report (seed, rewritten for ppm and nine keys)
- `runner/stop.c`, `runner/stop.h` - stop flag
- `runner/exit_status.h` - exit status table
- `runner/main.c` - `--bench`, `--help`, `--version`, sorted hash frames, stop check
- `CMakeLists.txt` - adds bench.c, clock.c and stop.c to muhtari-run
- `tests/runner/test_runner.c` - runner.bench.percentile, runner.bench.format, runner.stop
- `tests/runner/tests.cmake` - command-line checks dispatched on CASE, 12 new registrations
- `tests/cmake/exit_table.cmake` - --help table against docs/runner.md
- `tests/inventory/ci.txt` - the 12 new tests
- `docs/runner.md` - user documentation for the runner

## Decisions Made

- `--bench N` replaces `--frames N` rather than combining with it. Giving both, or either one twice, is a usage error, so the frame count has a single source.
- `--version` reuses `muhtari_get_versions`. The library version comes from `version.txt` through CMake, so the runner needs no second version macro.
- The usage row's meaning was reworded to avoid a semicolon ("the command line is invalid, so nothing ran"), because CMake lists split on semicolons.
- The plan said to sort a copy of the samples. They are sorted in place instead, because nothing reads them in run order after the report.

## Deviations from Plan

### Orchestrator-directed

**1. Seed pin 9f9386c661f6 instead of 3200aa868371**
- 3200aa868371 is not on GitHub. `runner/bench.c`, `runner/bench.h`, `runner/clock.c`, `runner/clock.h`, `runner/exit_status.h`, `runner/stop.c` and `runner/stop.h` were fetched from 9f9386c661f6 with `gh api` into a scratch directory outside the repository. The Task 1 commit body cites `Seeded from nesturbator 9f9386c661f6 runner/bench.c` and `runner/clock.c (same author, MIT).` `stop.c` and `exit_status.h` were rewritten as the plan asked: no Windows console handlers and no X-macro. Their seeds were read for shape only, so no seed line is cited for them.

### Auto-fixed Issues

None. Task 2's RED was valid for `runner.stop`, where the stub module had no handler and the raised SIGINT killed the process, and for `runner.version`, where `--version` was an unknown option and exited 2. The bench, usage, hash.repeat and exit.failure tests passed at RED because Task 1 and plan 05 already had that behaviour. They pin it as characterization tests.

**Total deviations:** 1, orchestrator-directed. **Impact:** none on behaviour.

## Issues Encountered

- CTest 4 prints "100% tests passed out of 30" when every test passes, not the "0 tests failed out of 30" in the plan's `fails_when`. The two mean the same thing.

## Gate-code tally

| Lines | File |
|---|---|
| 19 | tests/cmake/expect_fail.cmake |
| 38 | tests/cmake/no_skip.cmake |
| 58 | tests/cmake/check_junit.cmake |
| 22 | tests/cmake/exit_table.cmake (new) |
| 20 | tests/policy/tests.cmake |
| 69 | cmake/MuhtariTests.cmake |
| 136 | tests/runner/tests.cmake (was 60; it now holds 14 runner checks and registrations) |
| **362** | repository-check and registration code, against **315** lines in `src/` |

The tenth-of-`src/` budget applies from phase 3.

## Evidence

- `cmake --workflow --preset ci`: exit 0, "100% tests passed out of 30".
- `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake`: exit 0, "30 tests ran once each".
- `build/ci/runner/muhtari-run --bench 3000`: exit 0, printing `frames 3000`, `fps_milli 8688424`, `p50_us 114` … `p99_999_us 212`, `max_us 212`.
- `build/ci/runner/muhtari-run --bench 1`: exit 0. All the percentile lines and `max_us` are equal.
- `muhtari-run --frames 2000000000 --hash-frame 1`, sent SIGTERM after 0.5 s: exit 0, with the frame 1 line on stdout.
- `grep -cE '\b(float|double)\b' runner/bench.c` printed 0. `grep -c '#define X\|X(' runner/exit_status.h` printed 0.
- Changing one word in the docs table made `ctest --preset ci -R runner.exit.table` fail and name the line. After the revert it passed.
- `grep -nE '\b[DR]-[0-9]{2}\b|FRAME-[0-9]{2}' docs/runner.md`: exit 1, no matches. `scripts/hygiene.sh --tree`: exit 0.
- RED (`3b3abfe`): `cmake --workflow --preset ci` exited 8. `runner.stop` failed with "Exception: Interrupt" and `runner.version` with "unknown option".

## User Setup Required

None. No external service configuration is required.

## Next Phase Readiness

- The nightly bench (plan 19) can call `muhtari-run --bench 300000` and parse the nine `key value` lines.
- The wasm32 lane (plan 13) must check whether `raise(SIGINT)` calls the handler under wasmtime. If it does not, `runner.stop` leaves the portable label in `wasm32.txt` only.

## Self-Check: PASSED

- FOUND: runner/bench.c, runner/bench.h, runner/clock.c, runner/clock.h, runner/stop.c, runner/stop.h, runner/exit_status.h, tests/runner/test_runner.c, tests/cmake/exit_table.cmake, docs/runner.md
- FOUND: 0b2ee82, 3b3abfe, 751f93c, 1ea1175 (ancestors of HEAD)

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*
