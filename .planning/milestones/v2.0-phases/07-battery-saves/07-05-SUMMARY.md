---
phase: 07-battery-saves
plan: "05"
status: complete
subsystem: runner
tags: [battery, persistence, hsc, savekey, filesystem]
requires:
  - phase: 07-battery-saves
    provides: Stable load-time battery topology and tested HSC and EEPROM hardware
provides:
  - Opt-in clean raw save import and dirty normal-exit replacement through actual runner main
  - Load-time CLI and movie device provisioning and bounded copied HSC firmware
  - Exact fresh-process bytes and pre-replacement fault controls with public save documentation
affects: [07-06, 07-07, 07-08, 07-09]
tech-stack:
  added: []
  patterns:
    - Keep the file transaction in the existing runner owner and core storage in its authoritative span
    - Substitute narrow libc and frame calls in an actual-main test without production failure flags
key-files:
  created:
    - tests/runner/test_save.c
  modified:
    - runner/main.c
    - runner/exit_status.h
    - host/status_text.c
    - runner/movie.c
    - runner/movie.h
    - tests/runner/tests.cmake
    - tests/runner/test_movie.c
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/runner.md
    - .planning/preparation/HOSTS-AND-INTEGRATION.md
key-decisions:
  - Use exclusive sibling files and one native replacement commit point without truncating the destination or retrying failed flushes.
  - Provision explicit movie and CLI save devices before load and import while preserving the existing movie grammar.
requirements-completed: []
requirements-contributed: [SAVE-01, SAVE-04, SAVE-05]
coverage:
  - id: exact-clean-save-import
    description: All three battery lengths restore before frames with exact padding oversize refusal and no clean rewrite
    requirement: SAVE-04
    verification:
      - kind: integration
        ref: "runner.save.import and runner.save.nonregular; focused CTest exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: fresh-process-save-bytes
    description: An actual CPU increment is committed and a separately launched runner imports and increments the exact prior file
    requirement: SAVE-04
    verification:
      - kind: e2e
        ref: "runner.save.roundtrip; actual-main plus separate muhtari-run process and complete 2048-byte comparison; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: owned-save-transaction
    description: Every pre-replacement fault retains old bytes with bounded collision ownership no cleanup retry and original output failure precedence
    requirement: SAVE-05
    verification:
      - kind: integration
        ref: "runner.save.faults; import read/close and save write/flush/sync/close/replace faults, 32 collisions, BIOS and output failures; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: pre-load-devices-and-firmware
    description: Either EEPROM identity and port movie precedence firmware copy and visible identity warnings are resolved before import or frames
    requirement: SAVE-01
    verification:
      - kind: integration
        ref: "runner.save.options runner.movie_parse runner.usage.port and runner.exit.table; exit 0 in full CI; URL N/A-local"
        status: pass
    human_judgment: false
  - id: accurate-runner-docs-and-inventory
    description: Public docs describe raw format writer ownership replacement limits and pending controls while exact inventories include applicable cases
    requirement: SAVE-04
    verification:
      - kind: other
        ref: "Agent read docs against runner and actual-main assertions; docs budgets and exit equality pass; URL N/A-local"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset ci; 635 plus 6 tests; exact inventories and hygiene24 exit 0; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 14024
  tasks: 3
  commits: 5
plan_head_before: d9587cc731e9c1420eab4e4ec596dc42c8b82c4f
plan_head_after: 4dd51f7650f13b8c4c7f01dbfa9c96dce304428d
duration: 11min
completed: 2026-10-24
---

# Phase 7 Plan 5: Runner raw save files Summary

**The opted-in runner restores clean raw battery RAM before execution and commits changed bytes through an owned temporary and one native replacement operation.**

## Accomplishments

- Added `--save-dir`, `--hsc`, `--hsc-rom`, SaveKey/AtariVox port values and live exit 4. Save naming uses the final component and final extension, including dotfiles and extensionless names. Existing accessible directories are required even without storage. No opt-in means no save I/O; absent storage is a validated no-op.
- Import reads at most span plus one into FF staging and checks read and close before copying. Missing/empty/short/exact media are clean; oversize exits 4 and other errors exit 1 without frames or replacement. Actual-main tests inspect all 2048/32768/34816 bytes before the first frame. POSIX nonblocking descriptor open and fstat refuse FIFOs without blocking; native-only nonregular cases are honestly registered outside WASI.
- Final dirty flushing captures the generation, exclusively creates one sibling in at most 32 attempts, checks complete write, stdio flush, file sync and close, then uses POSIX rename or Windows MoveFileEx replace/write-through without copy fallback. Only its own temporary is removed. Failures retain the dirty token, print an error and never retry during cleanup. Unrelated output errors after valid frames still save while preserving exit 1; BIOS rejection suppresses it.
- CLI and movie storage declarations reach media before load and import. Movie save-device fields stay empty with the existing format version. Bounded exact-size supplied firmware is copied; the runner reports its SHA-1, unknown identity and missing signatures independently, or clearly warns about the limited signature-only stub. Firmware never enters save files.
- Added five recurring registrations: import, roundtrip, faults and options are portable; nonregular is native-only. A separate real runner launch increments the prior complete saved image. Documentation explains one writer, basename isolation, raw layouts, clean import and the replacement commit point without claiming universal crash durability or future cadence/on-demand support.

## Task Commits

1. Task 1 RED `22ddb7e`; GREEN `865b250` — actual-main HSC save roundtrip, then complete import and normal replacement.
2. Task 2 RED `65a49ec`; GREEN `b9ed1df` — missing SaveKey CLI value, then load-time device/movie/firmware provisioning and expanded edge coverage.
3. Task 3 `4dd51f7` — docs, exact consumer diagnostic update and checked output-error flush precedence.

The ledger records five task commits. Actual tokens are the realized task diff's 56,096 characters divided by four. Metadata commits are excluded. The elapsed estimate is 11 minutes; the first durable test timestamp is 2026-10-24T03:15:08Z, with the plan's earlier context-loading time not separately clocked.

## Commands, exits and run URLs

All results are local, with run URL N/A-local. No runtime bytes or logs are committed.

| Command | Exit | Result |
|---|---:|---|
| `ctest --preset ci -R '^runner[.]save[.]roundtrip$' --no-tests=error --output-on-failure --output-junit save-red.xml` | 8 expected | Target executed; missing save-dir produced exit 2 and the planned changed-byte assertion failed |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/save-red.json --raw` | 0 | RED_EVIDENCE_OK with the actual unchanged JUnit and semantic assessment |
| `ctest --preset ci -R '^runner[.]save[.]options$' --no-tests=error --output-on-failure --output-junit save-options-red.xml` | 8 expected | Target executed; missing SaveKey option produced exit 2 and zero frames |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/save-options-red.json --raw` | 0 | RED_EVIDENCE_OK; timestamps derive from contemporaneous CTest reports |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^runner[.](save[.]|movie_parse$|exit[.]table$)' --no-tests=error --output-on-failure` | 0 | All seven focused cases pass |
| `cmake --workflow --preset ci` | 0 | Final full run: 635/635 non-release and 6/6 release checks; all five workflow steps |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 635 tests ran once each |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 6 tests ran once each |
| `cmake --preset hygiene && cmake --build --preset hygiene && ctest --preset hygiene --output-on-failure` | 0 | 24/24 including formatter, analyzer and policy controls |
| `scripts/hygiene.sh --tree` and `git diff --check` | 0 each | Clean-tree and whitespace checks |
| `git diff --exit-code d9587cc -- tests/built/expected.txt tests/audio/expected.txt quality.json tests/roms/manifest.txt` | 0 | Old fixture bytes, pins and numeric policy unchanged |
| Three ZIP enumeration and byte sum with Python pathlib | 0 | Exactly 3 actual macOS arm64 archives, 225543 <=399937 |

## Decisions and Ownership Review

Followed the locked raw per-game format and existing owners. The runner controls path/import/transaction lifecycle; it only consumes public battery queries. Core code has no file I/O changes. The finite transaction has one armed/failed state, captured generation and one replacement point. The actual-main seam substitutes libc calls in the test translation unit, with no production failure flags, dependency, I/O framework or extra public interface. No demonstrated current coupling calls for a separate module.

## Deviations from Plan

- Incremental exact inventories and the exit-table documentation were updated alongside implementation because configure and the live equality consumer require them immediately.
- Added `runner.save.nonregular` separately from the four proposed portable registrations: FIFO/native filesystem refusal is inapplicable to WASI and does not self-skip.
- Full CI caught an old exact port-usage diagnostic expectation after new values became supported. Updated both exact consumer matchers, retaining their exit-2 and usage checks. Added stdout-error-before-destroy coverage alongside PNG failure while completing documentation.

No architectural change, new resource grant, review-fix round or gap round was needed. All ten phase assumption flags remain unresolved until the phase's evidence/review disposition; this summary does not silently resolve them.

## Limits and Next Readiness

Cadence, on-demand requests and native console controls remain plan 07-06. Hosted native Windows, WASM and final cross-platform acceptance remain plan 07-09; local tests do not substitute for those runs. No entire shared SAVE requirement is marked complete here. Plan 07-06 is ready after this summary. No next GSD step was invoked.

## Self-Check: PASSED

The new test file exists, all five task commits are reachable, full local CI and hygiene pass, exact inventories pass and there are no unexpected deletions or generated untracked files. No unplanned threat surface, placeholder or skipped applicable test remains.
