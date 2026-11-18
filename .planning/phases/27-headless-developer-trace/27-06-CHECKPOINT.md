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
- At this original checkpoint, no new push, publication, hosted run, PR update, dispatch, rerun, merge or release had occurred. No hosted allowance was implied by the checkpoint itself.
- At this original checkpoint, TRACE-06 remained pending until the exact reviewed source produced an authentic Linux PR Cachegrind receipt with repeated stable samples and absolute unrounded raw delta at most 9,600 over 300 frames.

## Proposal presented at the original checkpoint

The recommended next step, if the owner wants another receipt attempt, is a separate hosted continuation limited to one update push of reviewed commit `4630288226b705b4eb1f4177b3736d8ac0b072ca` to existing draft PR 77, followed by at most 75 minutes observing only its automatic checks. This is a new hosted attempt with a fresh count and observation window; it may fail and does not authorize another push or imply that the unrelated CI failures are fixed.

The choice is:

| Option | Effect |
|---|---|
| Authorize the bounded continuation | One push of the named commit to PR 77 and up to 75 minutes of automatic-check observation. No manual dispatch, rerun, second push, merge or release. |
| Keep hosted work stopped | No hosted action occurs; TRACE-06 and shipping remain blocked without the required receipt. |

An attributable owner decision had to name the exact source and scope before any remote action. If the observation cap expires, leave in-flight runs untouched. This plan contains no remote-producing task; any grant must be recorded and carried through a separately bounded hosted continuation. Silence, the prior attempt, or this checkpoint was not authority.

## Resolution: bounded continuation authorized

On 2026-11-17 the owner replied, “yes i authorize that,” to the proposal above. This authorized exactly one update push of reviewed commit `4630288226b705b4eb1f4177b3736d8ac0b072ca` to existing draft PR 77 and read-only observation of only the resulting automatic checks until `2026-11-17T17:29:13Z`. The separate 27-05 terminal attempt remains unchanged and its earlier reported push authorization remains unverified. No manual dispatch, rerun, second push, merge or release was authorized.

The update push succeeded. PR 77 remains open and draft at the authorized head, with base `main` at `89b6ff82812cd5be873e9f90d32ac55fded8694b`. The Linux `perf` job produced a source-qualified `quality-perf` receipt with stable repeated samples and a raw hooks-off delta of 900 against the 9,600 limit (3 Ir/frame). Receipt provenance and the command/exit record are in [27-CI-EVIDENCE.md](27-CI-EVIDENCE.md). This evidence satisfies the previously missing cost result; the phase acceptance record and normal verifier have not yet been refreshed.

All four automatic workflows were terminal by `2026-11-17T16:57:25Z`, before the cap, so observation ended early. CI and Release ended in failure; Nightly failed and Dependabot auto-merge was skipped. The CI and Release Windows x64/arm64 lanes passed. Linux/BE32 jobs failed on GCC `-Werror=mismatched-dealloc`; ASan and the Nightly fuzz build failed because Clang does not declare `realpath`. The full-CI quality joins and required roll-ups therefore failed, while the Linux perf receipt passed. The exact lane results and durations are in [27-CI-EVIDENCE.md](27-CI-EVIDENCE.md). No jobs were cancelled, and release publish/install steps remained skipped. No further owner input is required for this completed observation; no second push, dispatch, rerun, merge or release is authorized.
