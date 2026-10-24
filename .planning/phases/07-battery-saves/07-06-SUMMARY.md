---
phase: 07-battery-saves
plan: "06"
status: complete
subsystem: runner
tags: [battery, persistence, signals, console, cadence]
requires:
  - phase: 07-battery-saves
    provides: 07-05 exact raw import and checked exclusive temporary replacement
provides:
  - Bounded dirty-save cadence and consume-before-I/O platform requests
  - Actual child-process POSIX signal and native Windows console test paths
  - Expanded actual-consumer fault and replacement-commit-point controls
affects: [07-07, 07-08, 07-09]
tech-stack:
  added: []
  patterns:
    - Keep host request synchronization in stop.c and all persistence in the existing runner transaction
    - Use ordinary PNG output for bounded child readiness without another control protocol
key-files:
  created:
    - tests/runner/test_control.c
  modified:
    - runner/main.c
    - runner/stop.c
    - runner/stop.h
    - tests/runner/test_save.c
    - tests/runner/test_runner.c
    - tests/runner/test_audio.c
    - tests/runner/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/runner.md
    - .planning/preparation/HOSTS-AND-INTEGRATION.md
key-decisions:
  - Consume coalesced flush requests before file I/O with blocked POSIX read-clear or Windows Interlocked exchange.
  - Preserve inherited ignored POSIX signals and the Windows Ctrl+C ignore attribute.
  - Keep simulated frame-boundary scheduling separate from actual OS child-delivery evidence.
requirements-completed: []
requirements-contributed: [SAVE-05]
coverage:
  - id: bounded-dirty-cadence
    description: Positive bounded cadence default 60 clean requests stop and requests during writes use the actual runner transaction
    requirement: SAVE-05
    verification:
      - kind: integration
        ref: "runner.save.cadence; F1 default 60 UINT32_MAX invalid values change-after-boundary clean coalescing during-write and stop cases; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: actual-consumer-faults
    description: Create fdopen partial write flush sync close and replace failures preserve complete prior bytes foreign temporaries and no-retry behavior
    requirement: SAVE-05
    verification:
      - kind: integration
        ref: "runner.save.faults; complete old-file checks owned-temp absence early-cadence failure and later-output-error commit point; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: real-posix-controls
    description: Actual runner child persists complete bytes on repeated USR1 while alive and exits cleanly on INT or TERM
    requirement: SAVE-05
    verification:
      - kind: e2e
        ref: "runner.save.controls on macOS arm64; native fork exec kill bounded waits and all 2048 bytes; exit 0; URL N/A-local"
        status: pass
      - kind: integration
        ref: "runner.stop and runner.audio.stop_failure retain repeated persistent handlers inherited-ignore and frame-boundary behavior; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: windows-native-controls
    description: Native callback uses Interlocked flags and recurring tests generate BREAK and C in a private console
    requirement: SAVE-05
    verification:
      - kind: integration
        ref: "Windows-specific runner.stop runner.save.cadence runner.save.controls and runner.audio.stop_failure paths implemented; actual Windows x64 and arm64 execution remains assigned to 07-09"
        status: unknown
    human_judgment: false
  - id: local-acceptance-and-docs
    description: Current docs match runner ownership and guarantees and full local inventory remains within the unchanged archive bound
    requirement: SAVE-05
    verification:
      - kind: other
        ref: "Agent read docs against actual control transaction and tests plus ENGINEERING section 11 ownership review; no universal durability or multiple-writer claim"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset ci; 637 plus 6 tests and exact inventories; hygiene 24; three ZIPs 226048 <=399937; exits0; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 10092
  tasks: 3
  commits: 5
plan_head_before: 6cff408e04b15bace6a784c8916db5892b7d2b46
plan_head_after: a707137013c89077a563c3201f14b15cc505f902
duration: 10min
started: 2026-10-24T03:29:12Z
completed: 2026-10-24
---

# Phase 7 Plan 6: Runner save cadence and controls Summary

**Dirty runner saves now flush at bounded completed-frame cadences, on native platform requests, and before orderly destruction through one checked replacement transaction.**

## Accomplishments

- Added positive uint32 `--save-interval F`, requiring `--save-dir`, default 60. Only successfully completed frames advance cadence. Dirty progress waits at most F subsequent completed frames; clean cadence/request/finalization never rewrites.
- POSIX persistent `sigaction` handlers preserve inherited ignored USR1/INT/TERM. USR1 read-clear blocks that signal, consumes before I/O and restores the prior mask. Windows BREAK and C callbacks perform only synchronized Interlocked flag stores; flush exchange consumes, stop remains sticky. Existing audio and stop tests use the native callback test seam on Windows and POSIX signals otherwise.
- Actual-main tests cover repeated/coalesced requests, request during write with a second dirty frame and a third-frame final flush, stop plus flush, F1/default 60/UINT32_MAX, invalid input and changes after a cadence boundary. Windows during-write injection runs its callback on a separate native thread. Real OS delivery is a distinct child test, using ordinary frame-one PNG readiness and bounded cleanup.
- Native macOS child tests send repeated USR1, verify all 2048 saved bytes while the runner is alive, then stop with INT. A separate child stops with TERM and verifies final bytes and exit 0. Windows test code owns a private console, generates actual BREAK/C broadcasts only there, and checks the same complete media and exit. Configured emulator prefixes propagate to child execution for the existing full PowerPC lane.
- Extended the actual consumer seam for create, fdopen and partial-write failure in addition to write/flush/sync/close/replace faults. Complete prior bytes and a recognizable foreign temporary survive; owned temporaries disappear. An early cadence error stops after one frame and never cleanup-retries. A later stdout failure after successful replacement retains the new bytes and failure exit, matching the replacement commit point.

## Task Commits

1. Task1 RED `996fe91`; GREEN `6a3532b` — cadence option, requests, synchronized platform implementation and adapted audio/stop consumers.
2. Task2 `4d7557e` — actual OS child controls and expanded transaction failures. `a707137` corrects the child emulator prefix needed by the full BE inventory.
3. Task3 `c5b7a83` — public cadence/control/durability semantics and graduated preparation pointer.

The ledger measures five task commits from the recorded base through the final task head. Actual tokens are the realized diff's 40,368 characters divided by 4; summary and tracking are excluded.

## Commands, exits and run URLs

All current execution evidence is local; run URL N/A-local. No committed log or runtime save/ROM data is introduced.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^runner[.]save[.]cadence$' --no-tests=error --output-on-failure --output-junit save-cadence-red.xml` | 8 expected | Target actually executed and asserted supported interval returns 0; absent option returned usage 2 |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/save-cadence-red.json --raw` | 0 | RED_EVIDENCE_OK; unchanged JUnit and semantic assessment in ignored build output |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^runner[.](save[.]\|stop$\|audio[.]stop_failure$)' --no-tests=error --output-on-failure` | 0 | Final 9/9 focused tests |
| `ctest --preset ci -R '^runner[.]save[.]' --no-tests=error --output-on-failure` | 0 | Final 7/7 save checks, including actual native child controls |
| `build/ci/tests/muhtari-test-control build/ci/tests/save-controls /usr/bin/env build/ci/runner/muhtari-run` | 0 | Real child controls also pass through a separate executable prefix |
| `cmake --workflow --preset ci` | 0 | Final 637/637 non-release plus 6/6 release; all five steps |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 637 registrations ran once |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 6 release registrations ran once |
| `cmake --workflow --preset hygiene` | 0 | Final 24/24 including formatting, analyzer, tree and no-float |
| `git diff --check` | 0 | No whitespace errors |
| `python3 -c 'from pathlib import Path; p=list(Path("build/ci").glob("muhtari-*-macos-arm64.zip")); print(len(p),sum(x.stat().st_size for x in p))'` | 0 | Exactly 3 archives, 226048 bytes <=399937 |

## TDD Gate Compliance

Task1's actual CTest JUnit failure was machine-classified and inspected before GREEN: the intended option was rejected by the real main, with no build/setup/discovery error. RED then GREEN commits are reachable. Task2 is test-only expansion over the implemented consumer/platform paths: it passed against the current product, so no artificial production defect or fake RED was introduced. Global TDD enforcement is false; no implementation gate was bypassed. Native Windows execution is explicitly pending 07-09, not a local passing result.

## Deviations from Plan

- Updated the exact WASI cadence inventory as soon as registration changed because configure enforces it. Native controls remain excluded from WASI and have no portable label.
- Moved existing audio/stop test adaptation into Task1 alongside the production platform change so Windows did not temporarily lose that coverage.
- Final inspection found full PowerPC child execution needs the same configured emulator prefix as other subprocess consumers. Added the argument path and verified its pass behavior with a separate executable prefix; final BE execution remains07-09.

## Ownership and limits

Requests remain in the existing runner-only stop owner; the current runner owns all file transactions. No core state, dependency, general I/O framework, IPC endpoint, production fault flag, new lane or changed policy/pin is introduced. Native tests generate legal owned runtime bytes only under ignored build directories. The Windows API contracts were checked against Microsoft HandlerRoutine, GenerateConsoleCtrlEvent and console-process-group documentation; this is implementation guidance, not Windows runtime evidence.

The descriptor-less transparency prohibition remains flagged-unverified for phase-level agent disposition. Current docs state one writer, replacement commit point, no directory-sync framework and no universal power-loss guarantee. No owner UAT or hardware request is needed. No whole SAVE requirement is marked complete before final native acceptance. Existing review-fix and gap allowances remain unused.

## Self-Check: PASSED

Created native control source exists; all five task commits are ancestors of HEAD. Final focused checks, full CI, exact inventories and hygiene pass. No unexpected deletion, known stub, skipped local plan test, generated untracked file or unplanned threat surface remains. Windows and BE evidence remains explicitly assigned to the later final acceptance plan. Next incomplete plan 07-07 owns libretro provisioning and publication.
