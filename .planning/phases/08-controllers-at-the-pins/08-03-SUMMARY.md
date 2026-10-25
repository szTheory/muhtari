---
phase: 08-controllers-at-the-pins
plan: "03"
subsystem: input
tags: [controllers, timing, driving, diagnostics, tdd]
requires:
  - phase: 08-02
    provides: Transactional extended input and load selection lifecycle
provides:
  - Bounded timed driving pins through actual CPU and held bus execution
  - Owned motion executable with 19 independently derived driving observations
  - Actual admitted motion manifest row in authenticated producer and join controls
affects: [08-04, 08-05, 08-09, 08-10, 08-14]
tech-stack:
  added: []
  patterns: [Fixed-size residual state, merged chronological deadlines, source-pinned numeric executable]
key-files:
  created:
    - tests/diag/controller-motion.s
    - tests/diag/controller-motion.cfg
    - tests/diag/controller-motion.a78
  modified:
    - src/internal.h
    - src/ports.c
    - src/bus.c
    - tests/input/test_input.c
    - docs/hardware/controllers.md
    - docs/embedding.md
    - tests/roms/manifest.txt
    - tests/diag/expected.csv
    - tests/policy/quality.cmake
key-decisions:
  - Keep motion timing in the existing bus owner with cached idle deadlines and fixed instance state.
  - Decode the diagnostic selector before mailbox initialization can consume the short handoff frame.
  - Exercise actual admitted controller rows through independent new-row and later-base producer controls.
requirements-completed: []
requirements-contributed: [INPUT-01, INPUT-04]
coverage:
  - id: driving-pins
    description: Both ports expose documented clockwise and reverse tuples and pin-6 fire through actual cartridge reads.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: input.pa7_edge driving_timing and exact-edge fresh executions
        status: pass
    human_judgment: false
  - id: bounded-motion-time
    description: One-shot replacement, saturation, cancellation, reversal and idle phase survive JAM, HALT, RDY and reset.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: input.pa7_edge; full ci workflow 643 ordinary and 6 release tests
        status: pass
    human_judgment: false
  - id: driving-selection
    description: Extended Auto and explicit load overrides activate driving while invalid payloads retain the accepted packet.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: tests/input/test_input.c driving_timing full-frame CPU observations
        status: pass
    human_judgment: false
  - id: numeric-owned-executable
    description: Fresh motion0 execution records 19 predeclared observations with scenario, version, count and CRC judgment.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: One-time motion-proof library execution reusing tests/diag/driver.c judge; exit 0; N/A-local
        status: pass
      - kind: integration
        ref: diag.rebuild and policy.quality.controls actual motion-row producer and join controls
        status: pass
    human_judgment: false
duration: 10min
started: 2026-10-24T20:08:03Z
completed: 2026-10-24
status: complete
actuals:
  tokens: 9923
  tasks: 2
  commits: 5
plan_head_before: d5a9c1d55b5b88db775783200457989b31dc3824
plan_head_after: d888c3128996653d20743bbef8ec595c9311048a
---

# Phase 8 Plan 3: Timed Driving Pins Summary

**Driving input now produces bounded 454-tick connector transitions through the CPU bus, with an owned executable recording 19 independently derived numeric observations.**

## Accomplishments

The extended setter and explicit load choice accept driving on either port. Positive X walks connector (1,2) tuples 11,10,00,01,11; negative reverses them, and button 1 grounds pin 6. Other payload fields refuse transactionally. The old setter and automatic legacy fallback remain unchanged. Complete public frame execution proves initial selection, held phase, no delta replay and button changes at CPU-visible registers.

Each instance owns two fixed motion states and one cached next-event tick. Both incoming signed int32 values and their summed residual clamp independently to [-255,255], with discarded excess and no negation of INT32_MIN. An idle axis starts at latch+454; an in-flight deadline survives added, canceled or reversed residual. Same-device phase persists; device change and reset cancel work. The existing bus merges controller and MARIA deadlines chronologically, including normal access data ticks, the final tick of a slow cycle, held-cycle batches and JAM reads. Events do not mark a lag frame as polled. EEPROM timing remains in its established owner and all prior tests pass.

The real cartridge LDA test checks data at start+17 and completion at start+18, exact first/following edge ticks, both signs, all four positive tuples, replacement, independent saturation/cancellation, fire, no-submission frames, peeks, JAM, RDY and HALT. Driving uses pins 1/2, so its motion cannot itself produce PA7/pin-4 edges; the unchanged digital PA7 case remains, and pin-4 motion history belongs to the later mouse/CX22 slice.

The owned motion executable uses ROM ID14, mailbox version1, the existing count/CRC protocol and finite completion. It decodes the held startup selector at the first full-frame VBLANK start, before clearing the mailbox. Only scenario motion0 is implemented. Comments and controller documentation declare the frame schedule and derive sample-data windows 429..432 ticks after VBLANK plus whole 454-tick lines, strictly before each next edge. The 19 expected.csv values were declared before observations, then all passed unchanged. Later families reuse this executable in fresh fixed-device scenarios.

The admitted binary SHA-256 is `7290b28e0c4e2851e82c684e29f3f683ac040cfb9daaae4f11ea96225e89c4f1`; its MIT/this-repo manifest row pins exact source commit b474a2bb0d3efebe01816f7cbfbe512093b6a473. Normal rebuild reproduces it. Existing producer controls now use the actual motion row, synthesize only absent controller families, authenticate the base without that row, and protect the real row once it appears in the later base. Existing changed/deleted/duplicate/source/license/digest/forged-identity faults remain exercised through the recursive producer and join.

## Task Commits

1. Task 1 RED: `a0ed99c` — exercise timed driving input through CPU reads.
2. Task 1 GREEN: `4f222d3` — clock bounded driving movement through bus deadlines.
3. Task 2 RED: `7db89c3` — derive numeric driving fixture observations.
4. Task 2 GREEN source: `b474a2b` — record timed driving pins in owned executable.
5. Task 2 admission: `d888c31` — admit motion fixture through authenticated controls.

The persisted ledger measures five task commits. Actual tokens are realized diff characters divided by four, rounded up; summary/state commits are separate.

## Command Evidence

Every run URL here is `N/A-local`. No hosted, six-native or cross-host acceptance is claimed.

| Command | Exit | Observed result |
|---|---|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^input[.]pa7_edge$' --no-tests=error --output-on-failure` | 0 | Timed CPU driving assertions and original PA7 case pass; tracer re-run after Task 1 commit also passes. |
| `ctest --preset ci -R '^(input\|bus\|riot\|core)[.]' --no-tests=error --output-on-failure` | 0 | 75 focused cases pass. |
| `cc -std=c17 -Wall -Wextra -Wpedantic -Werror -Isrc -Iinclude -Ihost build/ci/motion-proof.c build/ci/libmuhtari.a build/ci/libmuhtari_host.a -o build/ci/motion-proof` | 0 | One-time local stimulus harness compiles, reusing the existing diagnostic driver and judge. |
| `build/ci/motion-proof build/ci/motion-draft.a78` | 0 | All 19 predeclared records pass with actual library execution, ROM identity, version and CRC checks. |
| `ctest --test-dir build/ci/motion-tdd -R '^motion[.]numeric$' --no-tests=error --output-on-failure` | 0 | Actual executable and reused numeric judge pass after implementation. |
| `cmake -DWASMTIME=build/pins/wasmtime/wasmtime -DPKG=build/pins/7800basic -DSRC=tests/diag/controller-motion.s -DCFG=tests/diag/controller-motion.cfg -DOUT=build/ci/motion-admit -DMANIFEST=tests/roms/manifest.txt -DKEEP=tests/diag/controller-motion.a78 -P tests/diag/assemble.cmake` | 0 | Initial admitted binary generated with pinned fetched tools. KEEP is only initial generation; normal rebuild below verifies admission. |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(diag[.]rebuild\|policy[.]quality[.]controls)$' --no-tests=error --output-on-failure` | 0 | Byte-identical fixture rebuild and actual manifest producer/join controls pass. |
| `cmake --workflow --preset ci` | 0 | 643 ordinary tests and six release tests pass, including hygiene, fixture provenance, exact inventory, ABI, previous frame/audio pins and packages. |
| `ctest --test-dir build/ci --show-only=json-v1` with independent name/label comparison to `tests/inventory/ci.txt` | 0 | All 649 registered names and labels match exactly. ci/nofp/wasm32 inventory files are unchanged from the plan base. |
| `sh scripts/hygiene.sh --tree` and `git diff --check` | 0 | Clean hygiene and whitespace; enabled hooks and noreply identity used for every task commit. |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 08-03 --commits-only --raw` | 0 | Resolves all five task commits on this branch. |

An independent Python comparison against d5a9c1d confirms every old manifest row remains and expected.csv retains its exact old prefix. The three actual local macOS-arm64 ZIP sizes sum to235948 bytes. That is a local observation, not the later limiting-lane or six-native package gate.

Supplementary toolchain probes: `cmake --preset nofp` exits1 because local gcc resolves to AppleClang, which the existing GCC-only nofp gate refuses; `cmake --preset wasm32` exits1 because clang-21 is absent on PATH. No lane was weakened or inventory changed. These extra probes are not unrun plan verification; platform execution remains with the existing later acceptance gates.

## TDD Evidence

Both tasks have RED then GREEN commits. Task1's actual JUnit `red-driving.xml` records executed input.pa7_edge assertions: the unsupported setter returns13 instead of0, cartridge SWCHA reads FF instead of DE/CE, and fire remains high. Task2's actual JUnit `red-motion.xml` records a valid owned scaffold execution with scenario63 instead of0 and17 missing planned records; its idle observation passes, and the existing numeric judge rejects the missing behavior. Neither RED is a missing-file, configure or compile failure.

`node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/red-driving.json --raw` and the corresponding `build/ci/red-motion.json` command exit0 with RED_EVIDENCE_OK. Semantic inspection confirms the intended target assertions executed and failed for absent motion behavior. Reports, harness and temporary CTest registration remain ignored local build output; no logs are committed. Green commands above pass without changing expected numeric values. No refactor commit was needed.

## Deviations from Plan

**[Rule 2 - Documentation correctness] Updated docs/embedding.md with the implemented driving choice and applicable fields.** Its digital-only claim would otherwise contradict the new public behavior. Included in4f222d3.

**[Rule 3 - Required admission controls] Updated tests/policy/quality.cmake to consume the now-real motion row.** Keeping its synthetic motion rename alongside the actual manifest would create a duplicate and fail the required control. The existing new-row and later-base fault matrix now targets the actual row, without changing production admission policy or the join. Included ind888c31.

Task2 used a one-time local library harness around the existing diagnostic driver to establish behavioral RED and numeric GREEN before the permanent three-host driver slice. It is supplementary evidence; no public CLI/protocol or host mapping was added early. Source and byte admission are separate commits so the manifest can pin a real exact-source commit.

## Issues Encountered

The first full assembly exceeded a relative branch's range; replaced it with a nearby branch and absolute jump. The first complete numeric execution showed the mailbox clear had consumed the short handoff frame, moving selector decode and later reads one frame late. Moved decode before initialization; the originally derived schedule and all19 expected values then pass unchanged. These were two Task2 correction attempts, not phase review-fix or gap rounds. No owner confirmation, authentication gate, unexpected deletion or untracked generated output remains.

## Limits and Next Readiness

INPUT-01 and INPUT-04 remain incomplete: replay, frontend mappings, three-host diagnostic acceptance and the other required motion devices belong to following plans. All five INPUT requirements remain open. The 454/255 model is an explicitly documented operating-cost compatibility policy, not hardware calibration; comparable full-phase cost and six-native package acceptance remain pending as planned. No known stub prevents this plan's driving/executable goal, and no required plan verification was skipped.

Next separate command: `$gsd-execute-phase 8`. No shipping or later GSD step was invoked.

The SDK advanced Current Plan to4 and counted31/42 runtime summaries. Its roadmap helper updated the detail and checkbox but retained the old progress-table cell; reconciled that cell to3/14. Preserved3/7 completed milestone phases (43%) and30/41 active slots, excluding the historical halted06-01 outcome. No requirement was marked complete.

## Self-Check: PASSED

All three created fixture paths exist. Every listed task commit is an ancestor of HEAD, and evaluation-scope resolves all five on this branch. Fresh full CI and required task commands pass; the registered native names/labels match the exact inventory. No requirement is prematurely marked complete.
