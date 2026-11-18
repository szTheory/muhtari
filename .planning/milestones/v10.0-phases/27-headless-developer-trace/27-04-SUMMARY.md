---
phase: 27-headless-developer-trace
plan: "04"
subsystem: testing
tags: [CI, Windows, Cachegrind, evidence]

requires:
  - phase: 27-headless-developer-trace
    provides: Private trace implementation, Windows path controls, and the Linux trace-cost producer from plans 27-01 through 27-03.
provides:
  - Exact-source Windows x64 and arm64 path-test receipts for the initial authorized PR.
  - A source-specific failed Linux Cachegrind receipt disposition with phase verification still pending.
affects: [phase-27-verification, TRACE-03, TRACE-06]

actuals:
  tokens: 2539
  tasks: 2
  commits: 3
  plan_head_before: 65219ecf5169553f803bcf67a3ec02cfd02724b0
  plan_head_after: a28404fd18692a2a68db13c94813811da1df7a02

tech-stack:
  added: []
  patterns:
    - Source-qualified CI evidence keeps the branch head separate from the synthetic merge checkout.
key-files:
  created:
    - .planning/phases/27-headless-developer-trace/27-CI-EVIDENCE.md
    - .planning/phases/27-headless-developer-trace/27-04-SUMMARY.md
  modified:
    - .planning/phases/27-headless-developer-trace/27-CI-EVIDENCE.md
key-decisions:
  - "The Windows x64 and arm64 receipts passed individually; TRACE-03 is evidenced."
  - "TRACE-06 remains pending because Linux failed before the measurement and emitted no trace_cost artifact."
  - "The authorized initial run did not justify a workflow topology, timeout, or cache change."
patterns-established:
  - "Record the checked-out synthetic merge SHA from artifacts instead of treating the pull-request branch SHA as the built source."

requirements-completed: [TRACE-03]
coverage:
  - id: D1
    description: "Both exact-source Windows architectures execute and pass runner.trace.paths."
    requirement: TRACE-03
    verification:
      - kind: integration
        ref: "CI run 44001040567 attempt 1, jobs 133764925186 and 133764925225; validate.py receipts"
        status: pass
    human_judgment: false
  - id: D2
    description: "The Linux Cachegrind job produces a source-qualified trace_cost receipt."
    requirement: TRACE-06
    verification:
      - kind: integration
        ref: "CI run 44001040567 attempt 1, perf job 133764925116; validate.py receipts"
        status: fail
    human_judgment: false

duration: 107min
completed: 2026-11-17
status: complete
---

# Phase 27 Plan 04: Headless developer trace CI evidence summary

**The initial authorized PR proved the Windows path tests on x64 and arm64; Linux Cachegrind stopped at a compile warning, leaving TRACE-06 pending.**

## Performance

- **Duration:** 107 minutes
- **Started:** 2026-11-17T01:31:20Z
- **Completed:** 2026-11-17T03:18:33Z
- **Tasks:** 2
- **Files modified:** 1

## Accomplishments

- Created a source-qualified evidence report for the sole authorized draft validation PR.
- Verified both Windows architecture jobs ran `runner.trace.paths` against built checkout `070f65f10d1cc2f62aba63ad3e2f931d2e5987c7`; both path-test suites passed.
- Preserved the Linux perf failure, all required CI dispositions, and the unchanged pending verification gate without a retry or merge.

## Task Commits

1. **Task 1: Trace existing source-qualified CI results into an evidence disposition** — `6cf6830` (docs)
2. **Task 2: Record publication authority and initial CI evidence** — `fbf1668` (authority) and `a28404f` (evidence)

## Files Created/Modified

- `.planning/phases/27-headless-developer-trace/27-CI-EVIDENCE.md` — actual run, job, checkout, artifact, and failure evidence.
- `.planning/phases/27-headless-developer-trace/27-04-SUMMARY.md` — execution outcome and requirement coverage.

## Decisions Made

- The two Windows receipts establish the Windows path evidence for TRACE-03. The plan's acceptance matrix remains unchanged until all three required receipts pass.
- The Linux Cachegrind job did not reach its ON/OFF build or measurement step, so TRACE-06 remains unproven.
- One run does not support a safe CI topology, timeout, or cache change.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] Allow the local receipt validator to read ANSI-bearing GitHub logs**

- **Found during:** Task 2 receipt validation
- **Issue:** The installed GitHub CLI returned HTTP 200 for the Windows job log but rejected its ANSI escape sequences, preventing the one-off validator from reading it.
- **Fix:** Added the CLI-supported `--allow-escape-sequences` option to the ignored validator's API transport; all receipt requirements and assertions stayed unchanged.
- **Files modified:** Ignored `build/phase27-evidence/validate.py`.
- **Verification:** The next `receipts` invocation passed both Windows rows, then failed closed on the Linux perf job.
- **Commit:** Not committed; helper and runtime downloads remain ignored.

**Total deviations:** 1 auto-fixed blocking tool issue.  
**Impact on plan:** Validation continued without changing acceptance criteria, source, workflow, or remote scope.

## Issues Encountered

- Linux perf, Linux x64/arm64, both Ubuntu nofp jobs, and the library job failed compilation at `runner/main.c:1257:36` because GCC `-Werror=type-limits` rejects an always-false size comparison.
- ASan failed compilation at `runner/main.c:841` because Clang did not declare `realpath`; BE32 failed at `tests/runner/test_save.c:308` on GCC `-Werror=mismatched-dealloc`.
- The Linux perf failure prevented creation of `quality-perf`; the downstream quality job then rejected the incomplete artifact set. No source change or hosted retry was within this plan.

## User Setup Required

None.

## Next Phase Readiness

Phase 27 remains open. The Windows path gap has exact-source passing evidence; the normal verifier has not re-evaluated its previous Windows finding. TRACE-06 and the phase gate remain pending until a separately reviewed source correction and permitted evidence path establish the Linux receipt. PR #77 remains draft and unmerged. No further hosted attempt is authorized by this plan.

## Self-Check: PASSED

- Evidence report exists at `.planning/phases/27-headless-developer-trace/27-CI-EVIDENCE.md`.
- Task commits `6cf6830`, `fbf1668`, and `a28404f` are ancestors of the summary commit.
- Non-planning files remain identical to reviewed baseline `c60c192b9088b5f21fc199d574de17e89e5b4c94`.

---
*Phase: 27-headless-developer-trace*  
*Completed: 2026-11-17*
