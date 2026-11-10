---
phase: 14-proven-frontend-states-and-responsive-play
plan: "08"
subsystem: testing
tags: [cmake, ctest, github-actions, retroarch, frontend-states]

requires:
  - phase: 14-proven-frontend-states-and-responsive-play
    provides: The bounded RetroArch state consumer, fixture judges, and preserved host-failure evidence
provides:
  - The normal portable replay registration is serial without changing its corpus, command, labels, or timeout
  - Exact-source and immediate-parent local preflight receipts plus one fully audited hosted outcome
  - A precise blocker record for the skipped actual-host state control; frontend requirements remain pending
affects: [FRONT-02, FRONT-03, FRONT-04, CI]

actuals:
  tokens: 16211
  tasks: 3
  commits: 4
  plan_head_before: 228621bb435b7d56de52e42c5bafc74cbfc3f8b1
  plan_head_after: b23d6bd47a7e6ad7c50116b8688a2ed4cbb15588

tech-stack:
  added: []
  patterns:
    - Reuse CTest RUN_SERIAL for the regular portable replay and verify its complete registration metadata
    - Treat host acceptance as absent when the source-bound host state step and receipt are skipped

key-files:
  created:
    - .planning/phases/14-proven-frontend-states-and-responsive-play/14-08-SUMMARY.md
  modified:
    - tests/fuzz/tests.cmake
    - .planning/phases/14-proven-frontend-states-and-responsive-play/14-RESOURCE-EVIDENCE.md
    - .planning/phases/14-proven-frontend-states-and-responsive-play/deferred-items.md
    - .planning/LESSONS.md

key-decisions:
  - "Keep FRONT-02, FRONT-03, and FRONT-04 pending because the one host-enabled run stopped before state-mode control."
  - "Record one-run CTest serial ordering without claiming it explains the earlier hosted timeout generally."
  - "Do not change CI timeouts, topology, or cache based on this single hosted run."

requirements-completed: []
coverage:
  - id: D1
    description: The normal portable replay uses CTest's serial policy with its original command, corpus, labels, assertions, and timeout.
    verification:
      - kind: unit
        ref: "cmake -P build/phase14-gap08-evidence/check-portable-scheduling.cmake; exit 0 on the final source metadata, 693 tests."
        status: pass
    human_judgment: false
  - id: D2
    description: The frozen dispatch source and its immediate parent passed their required local CI, inventories, hygiene, scheduling, and focused gates.
    verification:
      - kind: integration
        ref: "Exact source 2e32dc51558dceb5c536c704459f9d4765f959d7: full CI, ordinary/release inventories, hygiene/inventory, property metadata and six focused tests all exited 0; details in 14-RESOURCE-EVIDENCE.md."
        status: pass
      - kind: integration
        ref: "Immediate parent 32b3e3eea111f43858f601774071dc068ccd29af: one warm full CI, inventories, hygiene, property metadata and six focused tests all exited 0; cold timeout and bounded diagnosis are preserved separately."
        status: pass
    human_judgment: false
  - id: D3
    description: Actual pinned-host save/load picture, progress, and committed-storage evidence for FRONT-02 was not produced.
    requirement: FRONT-02
    verification:
      - kind: e2e
        ref: "Run 41656505025, attempt 1: the battery pre-step passed but the 300-line guard failed; the owned state control and state receipt were skipped. No state screenshot or raw save exists."
        status: unknown
    human_judgment: false
  - id: D4
    description: Actual pinned-host rewind reversal and continuation evidence for FRONT-03 was not produced.
    requirement: FRONT-03
    verification:
      - kind: e2e
        ref: "Run 41656505025, attempt 1: actual RetroArch state-mode control did not run; no host rewind observation or pass receipt exists."
        status: unknown
    human_judgment: false
  - id: D5
    description: Actual pinned-host run-ahead, isolation, storage, and completed-work evidence for FRONT-04 was not produced.
    requirement: FRONT-04
    verification:
      - kind: e2e
        ref: "Run 41656505025, attempt 1: actual RetroArch state-mode control did not run; no containment, run-ahead, durability, or complete-work observation exists."
        status: unknown
    human_judgment: false
  - id: D6
    description: The only newly authorized hosted attempt is source-bound and fully audited, with its failed and skipped steps preserved.
    verification:
      - kind: integration
        ref: "[Run 41656505025](https://github.com/szTheory/muhtari/actions/runs/41656505025), workflow_dispatch, frozen SHA 2e32dc51558dceb5c536c704459f9d4765f959d7, attempt 1; required aggregate audit exited 1."
        status: fail
    human_judgment: false

duration: 65min
completed: 2026-11-03
status: complete
---

# Phase 14 Plan 08: Isolated replay but host state observations blocked

**The regular portable replay is serial and passed in one current/base hosted comparison; the single host attempt stopped at an existing line-count guard before state-mode testing.**

## Performance

- **Duration:** 65 minutes recorded from the first source commit; earlier preparation time was not captured.
- **Started:** 2026-11-03T14:25:26Z (first source commit; preparation began earlier)
- **Completed:** 2026-11-03
- **Tasks:** 3/3
- **Files modified:** 8 (including frozen plan/state/roadmap bookkeeping and this summary)

## Accomplishments

- Applied `RUN_SERIAL TRUE` only to the normal portable `fuzz.regress.run`; its command, full 123-input corpus, assertions, labels and timeout scaling are unchanged.
- Passed the source and exact-parent local gates. On hosted macOS Intel, CTest completed the other 692 tests before the normal replay in both source and parent runs; each replay passed (5.48s and 5.90s).
- Pushed the frozen source once and issued one `phase14_host=true` dispatch. The run passed Windows x64/ARM64, Ubuntu x64 ASan, macOS current/base quality and hash checks, but the required aggregate failed because the RetroArch job stopped at its existing 300-line guard.
- Authenticated same-run macOS ARM64 package/source identity and recorded package digests. The actual state control, receipt, state screenshot, raw save, containment, rewind, run-ahead, and completed-work observations are absent.

## Local and Hosted Gates

| Command or observation | Exit/result |
| --- | --- |
| `cmake --workflow --preset ci` on frozen source | 0; 693 ordinary and six release tests passed |
| Ordinary and release `tests/cmake/check_junit.cmake` inventories | 0; 693 and six tests ran once |
| `cmake --workflow --preset hygiene` and its exact inventory | 0; all 30 checks ran once |
| `cmake -P build/phase14-gap08-evidence/check-portable-scheduling.cmake` | 0; exact source metadata and serial property passed |
| Six-test focused CTest command from 14-RESOURCE-EVIDENCE.md | 0; replay, loader, empty-corpus, install, and mock controls passed |
| Immediate-parent cold full CI | 8; only `bios.splash_skip` and `bios.rejects_cart` timed out at 1s |
| Bounded parent diagnostic and one authorized warm full CI | 0; diagnostic 4/4 in 0.21s, then 693 ordinary and six release tests passed |
| `gh run watch 41656505025 --interval 15 --compact` | 0 as a watcher command without `--exit-status`; run conclusion was failure |
| `cmake -DINPUT=build/phase14-gap08-evidence/run.json -DEXPECTED_SHA=2e32dc51558dceb5c536c704459f9d4765f959d7 -P tools/check_ci_run.cmake` | 1; required aggregate did not succeed |

The local parent timeout and warm diagnostic are separate retained receipts. The hosted WASI job also failed because `tmpfile` is unavailable on WASI and the deprecation warning is fatal under `-Werror`; this job is advisory and is not the reason the required aggregate failed.

## Task Commits

1. **Task 1: isolate portable replay scheduling** — `6de1300` (`fix(14-08): isolate portable replay scheduling`).
2. **Task 2: record exact-parent preflight** — `32b3e3e` (`docs(14-08): record exact-parent preflight evidence`).
3. **Pre-freeze plan bookkeeping** — `2e32dc5` (`docs(14-08): commit validated execution plan and state`).
4. **Task 3: record the single hosted outcome** — `b23d6bd` (`docs(14-08): record single hosted attempt outcome`).

The summary metadata commit is local and follows this task commit; no outcome commit was pushed.

## Files Created/Modified

- `tests/fuzz/tests.cmake` — serializes the regular portable replay registration.
- `14-RESOURCE-EVIDENCE.md` — records exact-source gates, run identity, logs, artifact authentication, failures, and job timing.
- `deferred-items.md` and `LESSONS.md` — preserve the current host blocker and qualify what the one-run scheduling observation proves.
- `14-08-PLAN.md`, `STATE.md`, and `ROADMAP.md` — validated scope and pre-freeze bookkeeping are in the frozen source commit.

## Decisions Made

- Keep FRONT-02, FRONT-03 and FRONT-04 pending. The actual state control and receipt step were skipped.
- Treat the 5.48s/5.90s current/base runs as evidence that CTest honored `RUN_SERIAL` in this attempt, not proof that overlap caused the earlier timeout.
- Keep the WASI `tmpfile` issue and the pre-existing host line-count guard as separate follow-up blockers; no out-of-scope workflow or test change was made.
- Do not repeat, rerun, cancel, push, dispatch, launch RetroArch locally, or proceed to Phase 12 under this exhausted allowance.

## Deviations from Plan

None. The exact-parent cold failure was diagnosed and the one bounded warm retest was explicitly authorized. All remaining evidence and bookkeeping stayed within Task 3.

## Issues Encountered

The host job's battery controls passed: both 120-frame runs matched all 34,816 save bytes and exact 320x242 pixels. The same step then failed at `.github/workflows/ci.yml:535`, which asserts that `cat tests/retroarch/*` has at most 300 lines. The current files total 3,845 lines, including `states.c` at 3,145. The owned state-mode control and state receipt checks were skipped. This independent guard must be addressed in a separately scoped change before any new host attempt.

The later phase-level regression gate ran the configured `cmake --workflow --preset ci` with a 600-second bound. Its one warm full-workflow retest also exited 8 on the same five one-second BIOS timeouts; the focused serial test plus fixtures passed 7/7 in 0.52 seconds. The cause remains unresolved, no timeout or source change was made, and there was no further full local run. Exact commands and receipts are in `14-RESOURCE-EVIDENCE.md`.

The best-effort `gsd_run windows append --kind unrun-verify ...` was rejected because existing `.planning/WINDOWS.md` counts disagree with its entries (17 open/35 fixed in frontmatter versus 16 open/36 fixed in the entries). The known ledger inconsistency remains deferred; no ledger repair was attempted.

## User Setup Required

None.

## Next Phase Readiness

The 14-08 execution is recorded, but Phase 14 acceptance is still blocked on actual pinned-host observations for FRONT-02/03/04. The single new push/dispatch allowance is spent. The orchestrator must perform the phase-level verification and decide whether to plan a separate cause-addressed repair for the existing line-count guard before requesting any new hosted allowance. No owner UAT is needed for the evidence currently available.

---
*Phase: 14-proven-frontend-states-and-responsive-play*
*Completed: 2026-11-03*

## Self-Check: PASSED

The summary file exists and the Task 1, Task 2, pre-freeze bookkeeping, and Task 3 commits are ancestors of the current plan outcome.
