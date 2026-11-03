---
phase: 14-proven-frontend-states-and-responsive-play
plan: "09"
subsystem: testing
tags: [cmake, github-actions, retroarch-controls, policy-tests]
requires:
  - phase: 14-08
    provides: Existing hosted battery and state controls whose source report must remain reachable.
provides:
  - Side-effect-free support source reporting for the existing hosted battery step.
  - Recurring policy controls for source growth, missing inputs, and restored workflow ceilings.
affects: [phase-14-hosted-validation, ci-policy]
tech-stack:
  added: []
  patterns: [CMake report-only mode, generated policy mutation controls]
key-files:
  created:
    - .planning/phases/14-proven-frontend-states-and-responsive-play/14-09-SUMMARY.md
  modified:
    - .github/workflows/ci.yml
    - tests/retroarch/run_retroarch.cmake
    - tests/policy/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - .planning/phases/14-proven-frontend-states-and-responsive-play/14-RESOURCE-EVIDENCE.md
key-decisions:
  - "Replace only the retired hosted support source ceiling with a report-only command; do not add a new quota."
  - "Keep FRONT-02, FRONT-03, and FRONT-04 pending until separately authorized real-host evidence exists."
patterns-established:
  - "Support reports return before launch configuration, cleanup, or frontend process access."
  - "Policy controls mutate the actual bounded workflow region in isolated build-tree copies."
requirements-completed: []
coverage:
  - id: D1
    description: "The hosted battery step reports per-file and aggregate support lines without a source-growth ceiling; recurring controls guard reporting, missing inputs, and stale numeric gates."
    requirement: "FRONT-02, FRONT-03, FRONT-04 (supporting CI path only; acceptance remains pending)"
    verification:
      - kind: integration
        ref: "ctest --preset ci -R '^policy\\.host_support\\.controls$' --no-tests=error --output-on-failure --output-junit build/ci/junit-gap09-policy.xml"
        status: pass
      - kind: other
        ref: "cmake -DMODE=support-report -DROOT=\"$PWD\" -P tests/retroarch/run_retroarch.cmake"
        status: pass
      - kind: integration
        ref: "ctest --preset ci -R '^retroarch\\.states\\.controls$' --no-tests=error --output-on-failure --output-junit build/ci/junit-gap09-host-controls.xml"
        status: pass
    human_judgment: false
# Measured before the final metadata commit from the persisted plan ledger.
commits: 2
plan_head_before: 7a89411bb5e1a3a00aeeff6b89490fc9d2b7f957
plan_head_after: 89404dc454fb8ab63b15d3014c2d5ef33b4763e5
actuals:
  tokens: 8104
  tasks: 2
  commits: 2
duration: 13min
completed: 2026-11-03
status: complete
---

# Phase 14 Plan 09: Hosted Support Reporting Summary

**The hosted battery step now reports support-source totals and has recurring local controls that reject the retired quota without launching RetroArch.**

## Performance

- **Duration:** 13 minutes
- **Started:** 2026-11-03T17:13:24Z
- **Completed:** 2026-11-03T17:26:16Z
- **Tasks:** 2
- **Files modified:** 7, including this summary

## Accomplishments

- Replaced only the stale source-line acceptance gate with `MODE=support-report`; the report lists each admitted support file and the aggregate without a quota.
- Added recurring controls that independently verify the report, accept known source growth, reject missing inputs and restored numeric ceilings, and confirm that report mode avoids frontend launch and cleanup.
- Preserved existing state controls and recorded local command, status, and non-host evidence in the phase resource evidence.

## Task Commits

1. **Task 1: Make the existing host job report source totals through a locally exercised consumer** — `1447e4d` (`fix`).
2. **Task 2: Review the narrow boundary repair and preserve its evidence** — `89404dc` (`fix`).

**Plan metadata:** `a7dc8c2` (`docs`).

## Files Created/Modified

- `.github/workflows/ci.yml` — replaced the retired source ceiling with the report-only command.
- `tests/retroarch/run_retroarch.cmake` — added report mode with required-input checks and no launch or cleanup side effects.
- `tests/policy/tests.cmake` — added generated pass/fault and workflow-mutation controls, including bounded scratch paths.
- `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt` — registered the policy control in both inventories.
- `.planning/phases/14-proven-frontend-states-and-responsive-play/14-RESOURCE-EVIDENCE.md` — recorded evidence and boundary review.
- `.planning/phases/14-proven-frontend-states-and-responsive-play/14-09-SUMMARY.md` — this plan result.

## Decisions Made

- Kept the change in the existing hosted job and CMake policy harness; no framework or dependency was added.
- Treated the report as observability only. Mock controls and local evidence do not qualify the actual frontend requirements.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Made policy assertions robust to CMake-wrapped diagnostics.**
- **Found during:** Task 1.
- **Issue:** The first focused policy runs did not match diagnostics when CMake wrapped error text across lines.
- **Fix:** Match the missing path and normalize whitespace before checking the rejection message.
- **Files modified:** `tests/policy/tests.cmake`.
- **Verification:** The focused policy control passed and exercised the expected missing-input and restored-gate failures.
- **Committed in:** `1447e4d`.

**2. [Rule 2 - Critical functionality] Confined destructive policy scratch cleanup beneath the build tree.**
- **Found during:** Task 2 boundary review.
- **Issue:** A malformed scratch path could otherwise make recursive cleanup target the build directory itself.
- **Fix:** Validate normalized root/build/work relationships and require the scratch path to be a strict descendant; tested self-target rejection while preserving a sentinel.
- **Files modified:** `tests/policy/tests.cmake`.
- **Verification:** Focused policy control, explicit self-target fault check, and `git diff --check` passed.
- **Committed in:** `89404dc`.

**Total deviations:** 2 auto-fixed (one Rule 1, one Rule 2). Both were directly related to the new policy control; no scope expansion occurred.

## Verification Evidence

- `cmake --preset ci` — exit 0.
- `cmake --build --preset ci` — exit 0.
- `ctest --preset ci -R '^policy\.host_support\.controls$' --no-tests=error --output-on-failure --output-junit build/ci/junit-gap09-policy.xml` — exit 0; growth, missing-input, and workflow gate faults were exercised.
- `cmake -DMODE=support-report -DROOT="$PWD" -P tests/retroarch/run_retroarch.cmake` — exit 0; aggregate 3,897 lines. The historical 3,845 count remains evidence only, not a limit.
- `ctest --preset ci -R '^retroarch\.states\.controls$' --no-tests=error --output-on-failure --output-junit build/ci/junit-gap09-host-controls.xml` — exit 0 using mocks.
- `actionlint -shellcheck='' -ignore 'label "ubuntu-26\.04' -ignore 'label "ubuntu-26\.04-arm' .github/workflows/ci.yml` — exit 0.
- `git diff --check` — exit 0.
- The pre-repair workflow snapshot failed specifically on the historical 300-line ceiling. The self-target scratch fault check exited nonzero as intended and preserved its sentinel.

No full workflow, real RetroArch run, hosted CI dispatch, push, or remote action was performed. FRONT-02/03/04 remain pending real-host evidence; the previously spent allowance remains spent and Phase 12 remains halted. The prior local regression blocker of five one-second BIOS timeouts was not rerun or changed by this plan.

## Issues Encountered

The initial policy assertion mismatch and scratch-cleanup boundary issue were fixed and verified above. No remaining issue blocks this local plan.

## User Setup Required

None.

## Next Phase Readiness

The stale source ceiling is guarded by normal local policy tests, so the later coordinated preflight can reach the existing opted-in state step. This plan supplies no host qualification or publication authority. No user action is needed for this local completion.

---
*Phase: 14-proven-frontend-states-and-responsive-play*
*Completed: 2026-11-03*

## Self-Check: PASSED

- Summary file exists at the required path.
- Task commits `1447e4d` and `89404dc` are ancestors of the recorded plan head.
