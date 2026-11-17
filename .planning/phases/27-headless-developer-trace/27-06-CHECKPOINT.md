---
phase: 27-headless-developer-trace
plan: "06"
status: partial
requirements-completed: []
---

# Phase 27 plan 27-06 checkpoint

## Requirement coverage

coverage:
  - id: TRACE-06
    description: The hooks-off instruction-cost gate passes on an authentic Linux Cachegrind receipt.
    requirement: With tracing disabled, the core preserves the existing hooks-off instruction-cost gate; instrumentation is selected only for calls that have tracing installed.
    verification:
      kind: other
      ref: 27-CI-EVIDENCE.md
      status: pending
    human_judgment: false

## Completed local work

- Task 1 committed the four existing pin fetches in `4630288226b705b4eb1f4177b3736d8ac0b072ca`. Only `.github/workflows/ci.yml` changed in that commit; the workflow blob is `95163d421b8ae3ac244bd2ecb52402d41ec143c3`.
- A fresh native CI Release configure, the `muhtari-run` and `muhtari-test-trace` targets, and the generated `trace-cost.a78` diagnostic marker all passed on Darwin arm64. The existing hygiene workflow passed all 30 checks. Commands, exits, tool/pin identities and the local-only limitation are recorded in `27-CI-EVIDENCE.md`.
- The local result establishes the fixture prerequisite path and CPU marker only. It is not Linux execution, Cachegrind measurement, or TRACE-06 acceptance.
- The correction changes no emulator product source and preserves D-01 through D-09, the unrounded 9,600-instruction ceiling over 300 frames, repeated sample requirements, the five-minute perf job timeout, and the existing required artifact contract.
- Generated cartridge and runner output remain under ignored `build/phase27-perf-preflight`; no ROM, BIOS, state, trace, pin payload or log was added to the tracked tree.

## Preserved history and blockers

- Plan 27-05 remains terminal and superseded. Its reported one-push count is treated as spent; its historical push authorization remains unverified. The read-only observation of run `44092872751`, attempt 1, perf job `134072368332`, established a failure before measurement and no `quality-perf` artifact. No part of this attempt was resumed or repeated.
- The unrelated recorded GCC `-Werror=mismatched-dealloc` in `tests/runner/test_save.c:308`, Clang `realpath` declaration failure in `runner/main.c:841`, and macOS `fuzz.regress.run` timeout after 735/736 tests remain separate shipping blockers where required. The local workflow correction does not resolve or promise a green full CI run.
- No new push, publication, hosted run, PR update, dispatch, rerun, merge or release occurred. No fresh hosted allowance is granted by this checkpoint.
- TRACE-06 remains pending until the exact reviewed source produces an authentic Linux PR Cachegrind receipt with repeated stable samples and absolute unrounded raw delta at most 9,600 over 300 frames.

## Next boundary: fresh hosted authority

The recommended next step, if the owner wants another receipt attempt, is a separate hosted continuation limited to one update push of reviewed commit `4630288226b705b4eb1f4177b3736d8ac0b072ca` to existing draft PR 77, followed by at most 75 minutes observing only its automatic checks. This is a new hosted attempt with a fresh count and observation window; it may fail and does not authorize another push or imply that the unrelated CI failures are fixed.

The choice is:

| Option | Effect |
|---|---|
| Authorize the bounded continuation | One push of the named commit to PR 77 and up to 75 minutes of automatic-check observation. No manual dispatch, rerun, second push, merge or release. |
| Keep hosted work stopped | No hosted action occurs; TRACE-06 and shipping remain blocked without the required receipt. |

An attributable owner decision must name the exact source and scope before any remote action. If the observation cap expires, leave in-flight runs untouched. This plan contains no remote-producing task; any grant must be recorded and carried through a separately bounded hosted continuation. Silence, the prior attempt, or this checkpoint is not authority.
