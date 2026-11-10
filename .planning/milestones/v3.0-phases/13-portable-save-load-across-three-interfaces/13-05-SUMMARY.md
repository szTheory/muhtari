---
phase: 13-portable-save-load-across-three-interfaces
plan: "05"
subsystem: runner-state
tags: [c, runner, portable-state, file-io]
requires:
  - phase: 13-portable-save-load-across-three-interfaces
    provides: "The core canonical state API and battery rollback contract from plan 13-04."
provides:
  - "Runner state save, load, and hash operations at invocation-local completed-frame points."
  - "Generated-media process continuation and actual state-file boundary coverage."
  - "Runner state CLI documentation and inventory registration."
affects: [runner, state-verification, phase-13-portable-save-load-across-three-interfaces]
actuals:
  tokens: 12341.25
  tasks: 3
  commits: 7
plan_head_before: 1d2940c4d1cc6083016386ec8c5ab958361c0bf8
plan_head_after: fa04dabddea25168cd3ca28aa9659d2c64d163fa
commits: 7
tech-stack:
  added: []
  patterns:
    - "One runner-owned exact-size state buffer is reused for load, save, and hashing."
    - "State reads are bounded to the expected size plus one byte; writes use checked sibling replacement."
key-files:
  created: []
  modified:
    - runner/main.c
    - tests/runner/test_save.c
    - tests/runner/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/runner.md
key-decisions:
  - "State points count successful frames in the current invocation; continued movie input must be supplied as an explicit suffix."
  - "Keep STATE-02 automatic shape classification unresolved and preserve the exact-source native/big-endian evidence boundary for plan 13-08."
patterns-established:
  - "Process-level tests use generated, build-local cartridge and movie fixtures, then compare canonical state and frame digests."
  - "State file faults are injected through the actual runner consumer's OS and stdio seams."
requirements-completed: []
coverage:
  - id: D1
    description: "Fresh-process state restore and explicit movie-suffix continuation match uninterrupted canonical state and frame hashes."
    requirement: STATE-06
    verification:
      kind: integration
      ref: "ctest --test-dir /private/tmp/muhtari-state-05-red/build -R '^runner\\.state\\.(roundtrip|options|files)$' --no-tests=error --output-on-failure; exit 0, 3/3. Local JUnit: /private/tmp/muhtari-state-05-red/junit-final-13-05.xml."
      status: pass
    human_judgment: false
  - id: D2
    description: "Runner rejects malformed, unreadable, nonregular, and failed state-file operations without running a frame or overwriting existing state or battery files."
    requirement: STATE-06
    verification:
      kind: integration
      ref: "runner.state.files CTest plus direct runner.save.faults and runner.save.nonregular consumers; all exit 0. Local JUnit: /private/tmp/muhtari-state-05-red/junit-review-task2.xml."
      status: pass
    human_judgment: false
  - id: D3
    description: "CLI point, path, overflow, mode, and duplicate rules are covered; canonical state bytes and hashes do not depend on output path or host battery generation."
    requirement: STATE-06
    verification:
      kind: integration
      ref: "runner.state.options CTest, including distinct output paths and imported-battery-then-state-restore comparison; exit 0."
      status: pass
    human_judgment: false
  - id: D4
    description: "A zero-frame accepted state rollback does not write restored battery bytes to the durable save file."
    requirement: STATE-05
    verification:
      kind: integration
      ref: "runner.state.files asserts zero save temporaries/replacements and byte-identical existing battery data; CTest exit 0."
      status: pass
    human_judgment: false
  - id: D5
    description: "Runner guide explains frame-point timing, exact state bytes, zero-frame constraints, and a fresh-process suffix example."
    requirement: STATE-06
    verification:
      kind: other
      ref: "Updated docs option/section search exit 0; matching generated-image/movie process example passes in runner.state.roundtrip. Run URL: N/A-local."
      status: pass
    human_judgment: false
duration: 20m
completed: 2026-11-01
status: complete
---

# Phase 13 Plan 05: Runner State Continuation Summary

**The runner now saves, restores, and hashes canonical state at invocation-local frame points, with bounded file I/O and a tested explicit movie-suffix workflow.**

## Performance

- **Duration:** 20 minutes, from the first plan task commit to summary preparation.
- **Started:** 2026-11-01T17:01:00-05:00.
- **Completed:** 2026-11-01T17:21:12-05:00.
- **Tasks:** 3.
- **Files modified:** 6 plan-owned source, test, inventory, and guide files.

## Accomplishments

- Added `--load-state`, repeatable `--state-at-frame N:FILE`, repeatable `--hash-state N`, and explicit `--frames0` state-only operation. The runner loads the canonical state after content, configuration, and battery setup; point zero precedes input, and later points follow successful frames.
- Used a single exact-size host buffer for state load/save/hash. State reads stop at the installed size plus one probe byte; state writes use the checked atomic sibling-file path, preserving prior data on failure.
- Added actual-consumer tests for roundtrip and explicit suffix continuation, ordered/duplicate/parser cases, exact-length and failed reads, nonregular files, write failures, and battery rollback persistence. Added corresponding CI/WASM inventory rows and documented the observable CLI contract.

## Task Commits

1. **Task 1: Wire runner state points and continuation** — `911454c` (test), `b7e2f9c` (test), `593f2d2` (feat).
2. **Task 2: Prove state-file boundaries through runner I/O** — `813c0f7` (test), `21cf4db` (test), `d006ca1` (fix).
3. **Task 3: Document and verify runner progress semantics** — `fa04dab` (docs).

The summary metadata is committed separately; its commit hash is reported in the executor completion message.

## Files Created/Modified

- `runner/main.c` — runner state options, bounded reads, state point capture, and checked sibling replacement.
- `tests/runner/test_save.c` — generated-media state file tests and actual descriptor/stdio fault seams.
- `tests/runner/tests.cmake` — process continuation, canonical hash/path checks, and CLI refusal cases.
- `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt` — registered runner state checks.
- `docs/runner.md` — state CLI, frame-point semantics, failure statuses, battery rollback note, and suffix example.

## Decisions Made

- State point numbers remain local to each invocation. A new process must receive the remaining movie input explicitly; the state file does not carry a movie cursor.
- Canonical hashes cover machine bytes, including emulated battery contents, but exclude output filenames and runner-side persistence-generation bookkeeping.
- `requirements-completed` remains empty: plan 13-05 completes its runner deliverables, while phase-level STATE requirements and exact-source big-endian evidence remain pending until their full acceptance work, including plan 13-08.

## TDD Gate Compliance

- **Task 1 RED:** `ctest --test-dir /private/tmp/muhtari-state-05-red/build -R '^runner\.state\.(roundtrip|options)$' --no-tests=error --output-on-failure --output-junit /private/tmp/muhtari-state-05-red/junit-red-task1.xml` exited 8 with two target failures; the named roundtrip test failed on the unknown, not-yet-implemented state options. `ctest --test-dir /private/tmp/muhtari-state-05-red/build -R '^runner\.state\.options$' --no-tests=error --output-on-failure --output-junit /private/tmp/muhtari-state-05-red/junit-red-task1-frames0.xml` separately exited 8 because `--frames0` was not implemented. These were intended missing-behavior failures, not setup errors. The focused tests then passed after `593f2d2`; tracer verification reran the two-case selector successfully.
- **Task 2 RED:** `ctest --test-dir /private/tmp/muhtari-state-05-red/build -R '^runner\.state\.files$' --no-tests=error --output-on-failure --output-junit /private/tmp/muhtari-state-05-red/junit-red-task2.xml` exited 8 on the expected actual-read seam (`state_probe_reads` was 0 instead of 1 and requested bytes were 0 instead of 13021), before the hooks were added. The focused file test passed after `21cf4db` and the descriptor cleanup review fix.
- **Task 3:** The parser behavior had already been implemented in Task 1, so the expanded negative matrix passed against that implementation and required no parser code change. The pre-documentation guide check at `21cf4db` exited 1 for missing state documentation; the updated guide check exited 0. No RED result is claimed for the already-working parser rules.

## Verification

All evidence below was generated locally; run URLs are N/A-local.

- `ctest --test-dir /private/tmp/muhtari-state-05-red/build -R '^runner\.state\.(roundtrip|options|files)$' --no-tests=error --output-on-failure --output-junit /private/tmp/muhtari-state-05-red/junit-final-13-05.xml` — exit 0, 3/3.
- Strict C17 helper build — exit 0:

  ```sh
  cc -std=c17 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Wmissing-prototypes -Wvla -Wundef -Wcast-qual -Wimplicit-fallthrough -Werror -Isrc -Iinclude -Ihost tests/runner/test_save.c runner/bench.c runner/clock.c runner/crc32.c runner/md5.c runner/movie.c runner/png.c runner/sha256.c build/ci/libmuhtari_host.a build/ci/libmuhtari.a -o /private/tmp/muhtari-test-save-runner-13-05
  ```

- Each direct generated-media consumer exited 0:

  ```sh
  /private/tmp/muhtari-test-save-runner-13-05 /private/tmp/muhtari-save-import-13-05 import
  /private/tmp/muhtari-test-save-runner-13-05 /private/tmp/muhtari-save-faults-13-05 faults
  /private/tmp/muhtari-test-save-runner-13-05 /private/tmp/muhtari-save-options-13-05 options
  /private/tmp/muhtari-test-save-runner-13-05 /private/tmp/muhtari-save-cadence-13-05 cadence
  /private/tmp/muhtari-test-save-runner-13-05 /private/tmp/muhtari-save-nonregular-13-05 nonregular
  ```

- Native `test_control` compile and process run covering save/stop signals and broken pipes — exit 0:

  ```sh
  cc -std=c17 -D_POSIX_C_SOURCE=200809L -DMUHTARI_TIMEOUT_SCALE=1 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Wmissing-prototypes -Wvla -Wundef -Wcast-qual -Wimplicit-fallthrough -Werror tests/runner/test_control.c -o /private/tmp/muhtari-test-control-13-05
  /private/tmp/muhtari-test-control-13-05 /private/tmp/muhtari-save-controls-13-05 tests/diag/battery.a78 build/ci/runner/muhtari-run
  ```

- `cmake --build build/ci --target muhtari-run` — exit 0, no work required. `sh scripts/hygiene.sh` and `git diff --check` — exit 0.
- `cmake --build build/ci --target muhtari-test-save-runner` — exit 1 because this existing configured tree has no such target; the exact-source strict compile above supplied the focused test binary.
- Documentation baseline search at `21cf4db` — exit 1 as expected; updated `rg -n -- '--load-state|--state-at-frame|--hash-state|## State files' docs/runner.md` — exit 0.
- **Full CI limitation:** `build/pins/wasmtime`, `build/pins/7800basic`, and `build/pins/openbios` are absent. No pin or media was fetched, and full `cmake --preset ci`, rcheevos integration, hosted jobs, RetroArch, Phase 12, and identity-bound actions were not run. The focused checks used the exact current runner/test sources and generated temporary fixtures. Run URL: N/A-local.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Cleared stale descriptor tracking after rejecting a nonregular state input.**
- **Found during:** Task 2 review.
- **Issue:** The runner closes a rejected directory descriptor before `fdopen`, so the test seam could retain its number and misclassify a later reused descriptor.
- **Fix:** Reset the captured descriptor immediately after the nonregular-input case.
- **Files modified:** `tests/runner/test_save.c`.
- **Verification:** Strict C17 warning compile and `runner.state.files` CTest passed.
- **Committed in:** `d006ca1`.

**Total deviations:** 1 auto-fixed (Rule 1). **Impact:** Test seam now remains accurate after descriptor reuse; no production scope expansion.

## Issues Encountered

- The pre-existing `build/ci` configuration did not have a `muhtari-test-save-runner` build target. The helper was compiled directly from the exact source with project warnings and `-Werror`, then run through a small local CTest harness. The runner target itself built successfully.
- Full CI and exact-source big-endian verification remain owned by phase plan 13-08. The missing local pins above were not fetched. `.planning/WINDOWS.md` was left untouched because the orchestrator identified its known mismatch and explicitly prohibited appending or retrying it.
- STATE-02 automatic shape classification remains unresolved/unclassified as recorded by the phase design. This plan's state-file refusal matrix does not claim to resolve that assumption.

## User Setup Required

None - no external service configuration required.

## Next Phase Readiness

Plan 13-05 is complete and its runner artifacts are ready for the root orchestrator's next sequential plan. The phase acceptance boundary is still pending: plan 13-08 must provide executed native and big-endian results against the exact source. No user action is needed for this plan.

## Self-Check: PASSED

The summary and all six plan files exist, and all seven plan task commits are ancestors of the current phase branch head.

---
*Phase: 13-portable-save-load-across-three-interfaces*
*Completed: 2026-11-01*
