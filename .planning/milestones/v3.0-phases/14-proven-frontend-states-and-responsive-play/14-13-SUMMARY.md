---
phase: 14-proven-frontend-states-and-responsive-play
plan: "13"
subsystem: testing
tags:
  - ctest
  - bios
  - timeout
requires:
  - phase: 14-12
    provides:
      - Existing BIOS timeout evidence and the bounded five-test mitigation proposal
provides:
  - RUN_SERIAL metadata for exactly the five authorized BIOS tests with their one-second deadlines and other test contracts preserved
  - CTest metadata comparison, property validation, representative fault controls, and one successful focused BIOS batch
affects:
  - Phase 14 plan 14-11
  - FRONT-02
  - FRONT-03
  - FRONT-04
actuals:
  tokens: 8104
  tasks: 3
  commits: 1
  plan_head_before: d3d5ae2e7a44751643514bea422c6ce5989bdd98
  plan_head_after: 6ef384436912aca743ad72e94e9ef944350be76c
commits: 1
plan_head_before: d3d5ae2e7a44751643514bea422c6ce5989bdd98
plan_head_after: 6ef384436912aca743ad72e94e9ef944350be76c
tech-stack:
  added: []
  patterns:
    - Compare generated CTest metadata before and after a narrowly scoped property change
    - Validate the saved JUnit receipt independently from a single-use CTest batch
key-files:
  created:
    - .planning/phases/14-proven-frontend-states-and-responsive-play/14-13-SUMMARY.md
    - build/phase14-gap13-evidence/check-bios-isolation.cmake
    - build/phase14-gap13-evidence/inventory-bios.txt
    - build/phase14-gap13-evidence/ctest-before.json
    - build/phase14-gap13-evidence/ctest-after.json
    - build/phase14-gap13-evidence/junit-bios.xml
  modified:
    - tests/boot/tests.cmake
    - .planning/phases/14-proven-frontend-states-and-responsive-play/14-RESOURCE-EVIDENCE.md
    - .planning/LESSONS.md
key-decisions:
  - Apply RUN_SERIAL only to the five explicitly approved BIOS tests; retain every one-second deadline and make no claim about the historical timeout cause.
  - Keep FRONT-02, FRONT-03, and FRONT-04 pending; actual-host evidence and complete workflows remain with Plan 14-11.
patterns-established:
  - Use the before/after CTest JSON and copied metadata mutations to prove narrow test-registration changes.
requirements-completed: []
coverage:
  - id: bios-isolation-mitigation
    description: The approved five BIOS tests are serialized; all CTest metadata controls pass and the one focused batch passes with both fixture setup tests.
    requirement: FRONT-02, FRONT-03, FRONT-04
    verification:
      - kind: integration
        ref: "Commands and exits are recorded in 14-RESOURCE-EVIDENCE.md; compare/property/controls and the seven-test JUnit receipt pass; local run URL N/A-local."
        status: pass
    human_judgment: false
  - id: actual-host-state-evidence
    description: Actual RetroArch save/load, rewind, and run-ahead behavior remains unverified by this local mitigation.
    requirement: FRONT-02, FRONT-03, FRONT-04
    verification:
      - kind: e2e
        ref: "No host action was authorized in Plan 14-13; Plan 14-11 retains its separate host decision."
        status: unknown
    human_judgment: true
    rationale: Actual-host behavior requires a separately authorized host run and observation.
metrics:
  duration: 38min
  completed: 2026-11-03
status: complete
---

# Phase 14 Plan 13: Five BIOS tests serialized with unchanged deadlines

**The five affected BIOS tests now run serially with one-second deadlines, backed by exact metadata controls and one focused 7/7 CTest receipt.**

## Performance

- **Duration:** Approximately 38 minutes
- **Started:** 2026-11-03T21:01:09Z
- **Completed:** 2026-11-03T21:39:35Z
- **Tasks:** 3
- **Files modified:** 6 tracked files, including plan tracking.

## Accomplishments

- Recorded the owner's 2026-11-03 explicit approval for the exact five-test scheduling mitigation and one focused test batch.
- Added RUN_SERIAL TRUE only to bios.splash_skip, bios.rejects_cart, bios.k_window, bios.k_first_fetch, and bios.load_aliases. All commands, fixtures, labels, assertions, inventory rows, and one-second deadlines remain unchanged.
- Captured before/after CTest metadata and added an ignored CMake checker. Compare and property modes passed, and controls rejected a missing property, an unrelated new property, and a relaxed deadline.
- Ran the focused CTest closure once: both fixture builders and all five BIOS tests passed, 7/7. The saved JUnit inventory also confirms each ran once without skips or failures.
- Kept the historical timeout cause unknown and left all frontend requirements pending.

## Task Commits

- **Task 1:** Explicit decision checkpoint approved; no file change or commit was required.
- **Tasks 2–3:** Source property and execution evidence committed together as 6ef3844, as the plan directs.

## Files Created/Modified

- tests/boot/tests.cmake — adds the five approved RUN_SERIAL properties.
- build/phase14-gap13-evidence/check-bios-isolation.cmake — compares all generated test metadata and checks property and fault controls.
- build/phase14-gap13-evidence/inventory-bios.txt — exact five BIOS tests plus two fixture setup tests.
- build/phase14-gap13-evidence/ctest-before.json and ctest-after.json — generated metadata receipts.
- build/phase14-gap13-evidence/junit-bios.xml — JUnit receipt for the one focused test batch.
- 14-RESOURCE-EVIDENCE.md — records the decision, source identity, commands, exits, result, and remaining gates.
- .planning/LESSONS.md — records the reproduced relative JUnit output-path behavior.
- STATE.md and ROADMAP.md — plan position and roadmap progress closeout.

## Decisions Made

- Treat RUN_SERIAL as a bounded reliability mitigation, not evidence that CTest overlap caused the historical timeouts.
- Do not complete FRONT-02, FRONT-03, or FRONT-04 from this local test-only result. Plan 14-11 owns full local workflows and the separate actual-host decision.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] Made the metadata checker bounded**
- **Found during:** Task 2
- **Issue:** The first checker implementation repeatedly parsed the complete CTest JSON document and returned no result within 30 seconds.
- **Fix:** Reworked comparison to extract each test object once and compare the normalized object, preserving checks for every test and property.
- **Files modified:** build/phase14-gap13-evidence/check-bios-isolation.cmake
- **Verification:** Final compare, property, and all three representative fault controls exited 0.
- **Committed in:** 6ef3844.

**2. [Rule 3 - Blocking] Located and preserved the generated JUnit receipt**
- **Found during:** Task 2
- **Issue:** CTest resolved the relative JUnit output path under build/ci, so the first source-root receipt check exited 1 because its expected file was absent.
- **Fix:** Copied the exact JUnit file produced by the successful batch into the planned evidence directory and reran only the receipt checker.
- **Files modified:** build/phase14-gap13-evidence/junit-bios.xml, .planning/LESSONS.md
- **Verification:** The JUnit checker exited 0 and confirmed exactly seven tests ran once each. The focused CTest batch was not rerun.
- **Committed in:** 6ef3844.

**Total deviations:** 2 auto-fixed (Rule 3: 2).
**Impact on plan:** Both corrections repaired evidence-check tooling; the authorized source scope and one-batch limit were preserved.

## Issues Encountered

- The optional Windows ledger rejected both deviation append attempts because its stored open/fixed counts disagree with its entries (17/35 versus 16/36). The existing phase deferred-items file already records this bookkeeping issue; the unrelated ledger was not repaired.
- The initial metadata-only checker command returned no status within its 30-second window. A targeted stop attempt exited 3 because process-list access was unavailable. The optimized checker passed, and neither operation launched a project test.

## User Setup Required

None.

## Next Phase Readiness

Plan 14-13 is complete. Plan 14-11 remains the next work and owns complete ordinary/release workflows on the repaired candidate and its frozen evidence commit, followed by a separate actual-host decision. No full workflow or host action was run here. FRONT-02, FRONT-03, and FRONT-04 remain pending.

---
*Phase: 14-proven-frontend-states-and-responsive-play*
*Completed: 2026-11-03*

## Self-Check: PASSED

The summary, source evidence, lesson, checker, inventory, metadata receipts, and JUnit receipt exist. Task commit 6ef3844 is an ancestor of HEAD.
