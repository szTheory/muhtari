---
phase: 28-measured-milestone-tune-up
plan: "01"
subsystem: maintenance-evidence
tags: [github-actions, release-evidence, windows-ledger, ci]
requires: []
provides:
  - Source-qualified delivery and CI maintenance evidence
  - Byte-preserving Windows register count correction
affects: [phase-28-verification, phase-29-runner-paths]
actuals:
  tokens: 5709
  tasks: 2
  commits: 2
commits: 2
plan_head_before: 0be908edfb4c6bc582b382affb4293bc6064502e
plan_head_after: 98a266557fd79502b4fd1320826825011cceb292
tech-stack:
  added: []
  patterns:
    - Exact-source workflow receipts remain separate from local verification
    - Register aggregate counts are corrected without regenerating entries
key-files:
  created:
    - .planning/phases/28-measured-milestone-tune-up/28-TUNEUP.md
    - .planning/phases/28-measured-milestone-tune-up/28-01-SUMMARY.md
  modified:
    - .planning/WINDOWS.md
    - .planning/ROADMAP.md
    - .planning/STATE.md
key-decisions:
  - "Preserve five hosted receipts as source-specific historical evidence."
  - "Make no CI topology, timeout, cache, resource, or membership change without comparable completed measurements."
  - "Leave TUNE-15 and TUNE-16 pending phase verification."
duration: 39 min
completed: 2026-11-18
status: complete
requirements-completed:
  - TUNE-15
  - TUNE-16
coverage:
  - id: TUNE-15
    description: Source-qualified delivery evidence, six maintenance dispositions, and bounded no-change rationale are recorded.
    requirement: TUNE-15
    verification:
      - kind: other
        ref: ".planning/phases/28-measured-milestone-tune-up/28-TUNEUP.md#source-qualified-delivery-evidence; cmake --workflow --preset ci"
        status: pass
    human_judgment: false
  - id: TUNE-16
    description: The existing Windows register validator and exact-byte preservation checks validate the two-count repair.
    requirement: TUNE-16
    verification:
      - kind: other
        ref: ".planning/phases/28-measured-milestone-tune-up/28-TUNEUP.md#register-reconciliation; gsd_run windows status --raw"
        status: pass
    human_judgment: false
---

# Phase 28 Plan 01: Measured milestone tune-up Summary

**Source-qualified release and CI receipts, preserved-byte ledger count repair, and passing local configured CI**

## Performance

- **Duration:** 39 min
- **Started:** 2026-11-18T15:02:15Z
- **Completed:** 2026-11-18T15:41:37Z
- **Tasks:** 2
- **Files modified or created:** 5

## Accomplishments

- Recorded all five locked hosted receipts against their exact source revisions, including timing, queue precision, cancelled or skipped work, and evidence limits.
- Corrected only open_count from 17 to 16 and fixed_count from 35 to 36; normalized register bytes and all entries remain unchanged.
- Completed cheap preflight checks before configured local CI, which passed configure, build, all 736 tests, package generation, and all eight release checks.
- Recorded execution progress while leaving both TUNE requirements and Phase 28 acceptance pending verification.

## Task Commits

1. Task 1: Establish the evidence-to-register maintainer slice — d0c2f04 (docs)
2. Task 2: Verify the local snapshot and record execution progress — 98a2665 (docs)

## Files Created/Modified

- .planning/phases/28-measured-milestone-tune-up/28-TUNEUP.md — exact-source receipts, maintenance dispositions, preservation proof, and local command outcomes.
- .planning/WINDOWS.md — two aggregate count values only; baseline bytes and the existing id 51 Markdown/JSON mirror mismatch are preserved.
- .planning/ROADMAP.md and .planning/STATE.md — one plan recorded as executed; phase verification remains pending.
- .planning/phases/28-measured-milestone-tune-up/28-01-SUMMARY.md — execution summary and coverage metadata.

## Decisions Made

- Keep v0.17.0 publication, current-main CI, skipped current-main publication work, and report-only Nightly cancellation separate by source and evidence class.
- Preserve the existing pipeline, timeout, cache, parallelism, resource-limit, and required/advisory contracts. Revisit only with repeated comparable completed measurements and a per-step profile that identifies safe savings while retaining hash equality, artifact joins, and gate membership.
- Leave the id 51 mirror row unchanged because this plan permits only the two count-field edits; disclose the discrepancy for phase verification.
- Keep TUNE-15 and TUNE-16 unchecked until phase verification.

## Verification

- The exact baseline-blob comparison passed after the final report and progress edits, with normalized-byte equality and clean, stale-count, and unrelated-byte fault controls.
- gsd_run windows status --raw: exit 0; 52 canonical entries, 16 open, 0 waived, 36 fixed.
- Report evidence, newline, and whitespace checks: exit 0; all five receipt identities and six maintenance dimensions present.
- git diff --check HEAD -- .planning: exit 0.
- scripts/hygiene.sh --tree: exit 0.
- gsd_run query validate.consistency --raw: exit 0, passed.
- gsd_run query roadmap.analyze --raw: exit 0; Phase 29 remains dependent on Phase 28 and neither is marked complete.
- cmake --workflow --preset ci: exit 0; configure, build, 736/736 tests, library/runner/libretro packages, and 8/8 release checks ran. This local evidence does not verify Windows namespace behavior or acceptance for the historical hosted sources.
- Commands, initial failures, recoveries, source-qualified receipts, and limits are detailed in the report.

## Deviations from Plan

- The fresh worktree lacked four repository-pinned CI inputs. Two configure attempts exited 1 before build or tests; the missing Wasmtime, 7800basic, OpenBIOS, and rcheevos inputs were fetched with the repository's existing fetch-pin script using exact pins. No dependency manifest or package changed; configured CI then passed.
- An initial detached shell launch did not capture a CMake exit and was treated as incomplete. The managed-session run produced the recorded successful exit.
- The preserved register has an existing id 51 rendered-table mismatch: Markdown says open while canonical JSON says fixed. The required tally uses canonical JSON, the discrepancy is disclosed in the report, and the out-of-scope row remains unchanged.

## Issues Encountered

The existing Markdown view and canonical JSON differ for id 51. The GSD validator accepts the canonical entry counts; no row rewrite was allowed by the byte-preservation contract. No source, test, dependency, public API, workflow topology, host/device, remote dispatch, or Phase 29 change was made.

## Next Phase Readiness

The report and execution evidence are ready for configured review and Phase 28 verification. No owner action is needed for this local plan. Both requirements remain pending until that verification is complete.

## Self-Check: PASSED

The report and register artifacts exist; Task 1 commit d0c2f04 and Task 2 commit 98a2665 are ancestors of the current plan head. The exact-byte, report, planning, hygiene, and configured workflow checks passed as recorded above.

---
*Phase: 28-measured-milestone-tune-up*
*Completed: 2026-11-18*
