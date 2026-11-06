---
phase: 14-proven-frontend-states-and-responsive-play
plan: "07"
subsystem: testing
tags: [cmake, libretro, retroarch, github-actions, asan, windows]

requires:
  - phase: 14-proven-frontend-states-and-responsive-play
    provides: The bounded state consumer, exact-source host gate, and prior failure receipts
provides:
  - Three non-POSIX test paths consume self_path and pass hosted Windows x64 and ARM64 builds
  - One fresh hosted Ubuntu x64 ASan result and an exact audited outcome for the single authorized dispatch
  - Locally recorded partial failure with actual-host requirements still pending
affects: [FRONT-02, FRONT-03, FRONT-04, docs/libretro.md, docs/testing.md]

actuals:
  tokens: 4610.5
  tasks: 3
  commits: 3
  plan_head_before: 664f89ab54ed17e0717ceb64b67142acbbe54303
  plan_head_after: 496826e22afa5c04ba3a0a25d0995d84edb6ccd3

tech-stack:
  added: []
  patterns:
    - Host qualification requires a completed same-source host job and authenticated same-run receipt/artifacts
    - A single hosted timeout does not justify timeout, cache, or topology changes

key-files:
  created:
    - .planning/phases/14-proven-frontend-states-and-responsive-play/14-07-SUMMARY.md
  modified:
    - tests/retroarch/states.c
    - docs/libretro.md
    - docs/testing.md
    - .planning/phases/14-proven-frontend-states-and-responsive-play/14-RESOURCE-EVIDENCE.md
    - .planning/phases/14-proven-frontend-states-and-responsive-play/deferred-items.md
    - .planning/LESSONS.md

key-decisions:
  - "Keep FRONT-02, FRONT-03, and FRONT-04 pending: the single new run skipped the actual RetroArch job."
  - "Do not change CI timeouts, topology, or caching based on one macOS Intel baseline timeout."

requirements-completed: []
coverage:
  - id: D1
    description: The three non-POSIX state-control paths consume self_path; required Windows builds and hosted Ubuntu x64 ASan pass for the frozen source.
    requirement: FRONT-02
    verification:
      - kind: integration
        ref: "Current-source `cmake --workflow --preset ci`, ordinary/release inventories, hygiene, hygiene inventory, and focused controls: each exit 0; details in 14-RESOURCE-EVIDENCE.md."
        status: pass
      - kind: integration
        ref: "[Run 41629846188](https://github.com/szTheory/muhtari/actions/runs/41629846188), attempt 1: Windows x64, Windows ARM64, and hosted Ubuntu x64 ASan passed for source 27818674366f80e230b8fa44493e1046282c3a66."
        status: pass
    human_judgment: false
  - id: D2
    description: Actual pinned RetroArch save/load picture, progress, and committed-storage evidence for FRONT-02 was not produced.
    requirement: FRONT-02
    verification:
      - kind: integration
        ref: "Local `cmake --workflow --preset ci` exit 0 and focused state controls exit 0; these checks do not establish frontend behavior."
        status: pass
      - kind: integration
        ref: "[Run 41629846188](https://github.com/szTheory/muhtari/actions/runs/41629846188): macOS Intel independent-parent baseline failed on `fuzz.regress.run` timeout; artifact join and required aggregate failed."
        status: fail
      - kind: e2e
        ref: "The RetroArch job was skipped; no host receipt or `retroarch-frames` artifact exists in run 41629846188."
        status: unknown
    human_judgment: false
  - id: D3
    description: Actual pinned RetroArch rewind reversals and continuation evidence for FRONT-03 was not produced.
    requirement: FRONT-03
    verification:
      - kind: integration
        ref: "Local `cmake --workflow --preset ci` exit 0 and focused state controls exit 0; these checks do not establish frontend behavior."
        status: pass
      - kind: e2e
        ref: "The RetroArch job was skipped before host execution in [run 41629846188](https://github.com/szTheory/muhtari/actions/runs/41629846188); no receipt or `retroarch-frames` artifact exists."
        status: unknown
    human_judgment: false
  - id: D4
    description: Actual pinned RetroArch run-ahead, containment, persistent-storage safety, and completed-work evidence for FRONT-04 was not produced.
    requirement: FRONT-04
    verification:
      - kind: integration
        ref: "Local `cmake --workflow --preset ci` exit 0 and focused state controls exit 0; these checks do not establish frontend behavior."
        status: pass
      - kind: e2e
        ref: "The RetroArch job was skipped before host execution in [run 41629846188](https://github.com/szTheory/muhtari/actions/runs/41629846188); no receipt or `retroarch-frames` artifact exists."
        status: unknown
    human_judgment: false

duration: ">= 57 min (from first task commit; plan start preceded retained timing)"
completed: 2026-11-03
status: complete
---

# Phase 14 Plan 07: Proven frontend states and responsive play Summary

**The Windows warning repair and hosted ASan passed, but the sole new CI attempt failed its macOS Intel parent baseline before RetroArch could run.**

## Performance

- **Duration:** At least 57 minutes, measured from the first task commit; the plan start time was not captured.
- **Started:** 2026-11-03T11:33:43Z (first task commit)
- **Completed:** 2026-11-03T12:31:06Z (Task 3 commit)
- **Tasks:** 3
- **Files modified:** 6 plan files

## Accomplishments

- Added `(void)self_path;` in the existing three non-POSIX branches of `tests/retroarch/states.c`. The focused read-only review of this exact diff found zero issues and made zero fixes; it consumed no review-fix round. The hosted Windows x64 and ARM64 builds and fresh Ubuntu x64 ASan passed.
- Current-source CI (`cmake --workflow --preset ci`), ordinary and release inventories, hygiene, hygiene inventory, focused state controls, and whitespace checks passed locally. The independent exact-parent warm workflow and its inventories/hygiene/control checks passed. An initial cold exact-parent workflow exited 8 with 16 timeouts; a bounded serial diagnostic passed 19/19, then the warm full workflow passed without changing timeouts or CI policy. Local macOS arm64 ASan remained unavailable after pre-main hangs and was not retried.
- Pushed the frozen source once and verified the remote SHA, then issued exactly one `phase14_host=true` dispatch. [Run 41629846188](https://github.com/szTheory/muhtari/actions/runs/41629846188), attempt 1, failed because the macOS Intel independent-parent quality baseline timed out in `fuzz.regress.run`; the exact artifact join and required aggregate failed, and RetroArch was skipped. The same-run macOS arm64 archive and hash artifacts are authenticated, but no host receipt or `retroarch-frames` artifact exists.
- Updated the user and maintainer docs, evidence record, deferred items, and lesson ledger. FRONT-02/03/04 remain pending, and the 14-07 host allowance is spent.

## Evidence and Verification

| Command or evidence | Exit/result |
| --- | --- |
| `cmake --workflow --preset ci` on repaired current source | 0; 693 ordinary tests and 6 release tests passed |
| Current-source ordinary inventory and release inventory consumers | 0 each; 693 and 6 tests respectively, once each |
| `cmake --workflow --preset hygiene` and hygiene inventory consumer | 0 each; 30 checks once each |
| `ctest --test-dir build/ci -R '^retroarch\.states\.controls$' --no-tests=error --output-on-failure` | 0; mock-only controls passed |
| First exact-parent `cmake --workflow --preset ci` cold run | 8; 16 fixture-backed tests timed out; failed receipts preserved in `build/phase14-gap07-evidence/parent-ci-cold/` |
| Bounded serial exact-parent diagnostic | 0; 19/19; output retained in `parent-ci-serial-diagnostic.log` |
| Exact-parent warm `cmake --workflow --preset ci` | 0; captured `CAPTURED_EXIT=0` in `parent-ci-warm.log`, 693 ordinary and 6 release tests |
| Exact-parent hygiene, three inventory consumers, and focused controls | 0 for each; logs retained under ignored `build/phase14-gap07-evidence/` |
| `cmake -DINPUT=build/phase14-gap07-evidence/run.json -DEXPECTED_SHA=27818674366f80e230b8fa44493e1046282c3a66 -P tools/check_ci_run.cmake` | 1; required aggregate failed |
| `gh run watch 41629846188 --exit-status` | 1; the single run reached terminal failure |
| `ctest --preset ci -R '^docs\.(budgets|api_names(\.self_test)?)$' --no-tests=error --output-on-failure` | 0 on final run; 3/3 checks passed. Two earlier attempts exited 8 because the authored status text exceeded the existing `docs/testing.md` size budget; the note was shortened without changing the budget. |
| `git diff --check HEAD -- docs/libretro.md docs/testing.md .planning/phases/14-proven-frontend-states-and-responsive-play/14-RESOURCE-EVIDENCE.md .planning/phases/14-proven-frontend-states-and-responsive-play/deferred-items.md .planning/LESSONS.md` | 0 |
| `gh run download 41629846188 --name archives-macos-arm64 --dir build/phase14-gap07-evidence/downloads/archives-macos-arm64` and `gh run download 41629846188 --name hashes-macos-arm64 --dir build/phase14-gap07-evidence/downloads/hashes-macos-arm64` | 0 each; the archive's commit identity matched the frozen source; no host artifact was present |

The normal push command `git push origin phase/14-proven-frontend-states-and-responsive-play` exited 0, and `git ls-remote origin refs/heads/phase/14-proven-frontend-states-and-responsive-play` exited 0 with the frozen SHA. Exactly one `gh workflow run ci.yml --ref phase/14-proven-frontend-states-and-responsive-play -f phase14_host=true` exited 0 and resolved to the run linked above. The run audit and complete job/artifact/timing evidence are in [14-RESOURCE-EVIDENCE.md](14-RESOURCE-EVIDENCE.md). These actions were not repeated.

The run started at 11:43:40Z and finished at 12:21:02Z (37m22s). The GitHub API did not expose a separate per-job `queued_at`, so start offsets combine dependency wait and runner queue. Windows ARM64 was the critical path at 36m56s; no jobs or steps were cancelled. A single run supports no safe topology, deadline, or cache change. The six spec-less candidates keep their previous classifications, and both player-storage/transparency prohibitions remain flagged for evidence review.

## Task Commits

1. **Task 1: Consume mock child paths on non-POSIX builds** — `234c81a` (`fix(14-07): consume mock child paths on non-POSIX`)
2. **Task 2: Freeze exact-parent preflight evidence** — `2781867` (`docs(14-07): freeze exact-parent preflight evidence`)
3. **Task 3: Record the bounded host outcome** — `496826e` (`docs(14-07): record bounded host outcome`)

## Files Created/Modified

- `tests/retroarch/states.c` — consume `self_path` only in the three existing non-POSIX control paths.
- `docs/libretro.md`, `docs/testing.md` — report the latest compiler/ASan outcome and skipped host status.
- `14-RESOURCE-EVIDENCE.md` — record exact source, parent, push, dispatch, result, artifacts, timing, and remaining gaps.
- `deferred-items.md`, `LESSONS.md` — preserve the prior Windows failure and add the newly observed macOS Intel timeout without changing CI policy.
- `14-07-SUMMARY.md` — this plan outcome.

## Decisions Made

- Preserve all host-dependent requirements as pending because the actual RetroArch job did not run.
- Treat the single macOS Intel timeout as an observed failure, not evidence for a broader timeout or infrastructure change.
- Keep the original runs and receipts unchanged and leave all post-run remote actions stopped.

## Deviations from Plan

The final-plan `state.advance-plan` handler would write `Phase complete — ready for verification` before the orchestrator's final verifier. To honor the explicit no-phase-completion boundary, the plan position was set with `state.update` to 7 of 7 while the phase status remains Executing. `state.update-progress` and `roadmap.update-plan-progress 14` report all seven plan summaries present, but the roadmap remains In Progress with `complete: false`; no requirements were marked complete. The documentation budget failures were corrected within Task 3; final documentation checks passed without changing the budget or policy.

## Issues Encountered

- The initial cold exact-parent workflow had 16 timeouts. A targeted serial diagnostic passed 19/19 and the warmed exact-parent full workflow passed; no timeout was increased and no CI policy changed.
- The hosted macOS Intel independent-parent baseline timed out in `fuzz.regress.run` after 28.06 seconds (CTest child exit 8, quality step exit 1). The quality artifact join failed and the host job was skipped. The failed/skipped/absent outcome exhausts the only new dispatch allowance.
- Two initial Task 3 docs-budget checks failed at 41,452 bytes and 40,968 bytes against the unchanged 40,960-byte limit. A concise status note resolved the issue; the final check passed.

## Deferred Issues

- FRONT-02 requires authenticated actual RetroArch save/load picture, progress, and committed-storage observations. FRONT-03 requires actual rewind reversals and continuation. FRONT-04 requires actual run-ahead, containment, storage-safety, and completed-work observations. The host job skipped and produced no receipt or `retroarch-frames` artifact; any new hosted attempt requires separately authorized scope because the one 14-07 attempt is spent.
- FRONT-02 boundary, precision, idempotency and concurrency candidates remain unresolved; FRONT-03 and FRONT-04 remain unclassified. The two recalled prohibitions remain flagged evidence-review items and were not dismissed.
- Local macOS arm64 ASan remains unavailable after the already recorded pre-main hangs in unrelated instrumented tests; it was not retried. Fresh hosted Ubuntu x64 ASan passed for the frozen source.

## Next Phase Readiness

Plan 14-07's authorized work is recorded and locally complete. The Phase 14 goal remains unverified: the orchestrator must run the final verifier against this evidence and retain FRONT-02/03/04 as pending unless new qualifying host evidence exists. No further push, dispatch, rerun, local RetroArch launch, Phase 12 work, PR, merge, or release is authorized here; no owner UAT is needed.

## State Tracking

`STATE.md` now records plan 7 of 7, Executing status, the spent host allowance, the pending requirements, and the orchestrator's final verification as the next action. `ROADMAP.md` records 7/7 plan summaries and In Progress (`complete: false`). No requirement-completion command was run.

---
*Phase: 14-proven-frontend-states-and-responsive-play*
*Completed: 2026-11-03*

## Self-Check: PASSED

- `14-07-SUMMARY.md` exists at the required phase path.
- Task commits `234c81a`, `2781867`, and `496826e` are ancestors of current `HEAD`.
