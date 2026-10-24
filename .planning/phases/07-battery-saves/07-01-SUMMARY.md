---
phase: 07-battery-saves
plan: "01"
status: complete
subsystem: testing
tags: [cmake, evidence, fixtures, compatibility]
requires:
  - phase: 06-retail-cartridge-boards
    provides: Authenticated evidence producer recursion and exact unchanged quality join
provides:
  - Narrow battery fixture comparison permission with raw trusted manifest identity
  - Exact behavior annotation relations with unchanged sample content authority
  - Actual producer and join pass/fault controls and matching quality documentation
affects: [07-02, 07-08, 07-09]
tech-stack:
  added: []
  patterns:
    - Extend existing producer controls without replacing authenticated consumers
key-files:
  created: []
  modified:
    - tests/cmake/quality.cmake
    - tests/policy/quality.cmake
    - docs/quality.md
key-decisions:
  - Keep fixture legality and production behavior independent from evidence comparability.
  - Permit only equality and exact 2-to-3 or 3-to-4 annotation migrations with raw trusted identity.
requirements-completed: []
requirements-contributed: [SAVE-01, SAVE-02, SAVE-03, SAVE-04, SAVE-05, SAVE-06]
coverage:
  - id: battery-fixture-relation
    description: Only the named battery addition extends the three existing names while all trusted old bytes remain protected
    requirement: SAVE-06
    verification:
      - kind: integration
        ref: "policy.quality.controls through actual producer recursion and unchanged join; focused CTest exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: exact-annotation-transition
    description: Equality and exact 2-to-3 or 3-to-4 annotations preserve raw trusted identity and reject content changes
    requirement: SAVE-03
    verification:
      - kind: integration
        ref: "policy.quality.controls revision equality, forward, reverse, skipped, arbitrary, missing, duplicate, changed and forged sample cases; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: authenticated-consumer-controls
    description: Produced observations retain event base, compiler, platform, commit and run authority through the real consumer
    requirement: SAVE-06
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; exit 0; 612 non-release plus 6 release tests; URL N/A-local"
        status: pass
      - kind: other
        ref: "git diff --exit-code bbaa3fe -- tests/cmake/quality_join.cmake tests/cmake/quality_evidence.cmake tests/roms/manifest.txt tests/built/expected.txt quality.json; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 4154
  tasks: 2
  commits: 2
plan_head_before: bbaa3fe6ef3167403b91547541ab09cd50874dfc
plan_head_after: 1779f92670f2de1650f7c521723413600a1df15a
duration: 3min
started: 2026-10-24T02:24:36Z
completed: 2026-10-24
---

# Phase 7 Plan 1: Battery evidence prerequisite Summary

**Battery fixture additions and exact behavior annotation migrations retain independent event-base identity through the actual evidence producer and unchanged join.**

## Accomplishments

- Added only `battery.a78` to the existing three-name relation. All four additions pass; a partial later base already containing battery and Activision rows protects both as old bytes. Changed, deleted, duplicate, unrelated, malformed, wrong-source, empty-pin, false-license/digest and comment faults remain rejected.
- Extended the existing temporary Git/event controls rather than manufacturing passing environment JSON. Actual producer recursion exercises PR, push and dispatch authority; emitted observations reach the real join. New consumer faults change the produced run, commit identity or recorded base SHA in one lane and must fail. Existing candidate-supplied trust, compiler and platform faults remain effective.
- Added the exact 3-to-4 sample annotation relation alongside 2-to-3. Real producer controls prove revision-4 equality, forward migration, both reverse directions, skipped and arbitrary revisions, missing/duplicate annotations, changed rows/pins/trailers and forged trust on both migrations. Documentation states these boundaries and the separate actual-fixture admission requirement.

## Task Commits

1. Task 1: `a215495` — prove battery fixture evidence comparability.
2. Task 2: `1779f92` — constrain battery behavior annotation transition and document it.

The plan ledger measures two task commits. Actual tokens are the realized task diff's 16,616 characters divided by four; metadata is excluded.

## Commands, exits and run URLs

All evidence is local; every run URL below is N/A-local. No log is committed.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci && ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure` | 0 | Task 1: 1/1, 4.02 s; Task 2: 1/1, 4.84 s |
| `cmake --workflow --preset ci` | 0 | 612/612 non-release tests, three component packages, 6/6 release checks; final quality control 6.38 s |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 612 tests ran once each |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 6 release tests ran once each |
| `scripts/hygiene.sh --tree` | 0 | No findings |
| `git diff --check` | 0 | No whitespace errors |
| `git diff --exit-code bbaa3fe -- tests/cmake/quality_join.cmake tests/cmake/quality_evidence.cmake tests/roms/manifest.txt tests/built/expected.txt quality.json` | 0 | Join, producer authority, product fixture/sample bytes, pins and numeric predicates unchanged |
| `git merge-base --is-ancestor a215495 HEAD` and `git merge-base --is-ancestor 1779f92 HEAD` | 0 each | Both task commits reachable |

## Decisions Made

Followed the existing owner and consumer boundaries. The allowlist grows by exactly one name; sample normalization accepts only two literal revision pairs. No dependency, package install, bound, registry, maintenance quota, Nyquist file, ASVS machinery or approval gate was introduced. The live core, expected sample annotation and diagnostic report judge remain unchanged for the coordinated update in 07-02; wrong-live-revision acceptance remains assigned to 07-02/09.

## Deviations from Plan

One tooling adjustment: the existing STATE had no labeled numeric plan position, so `state.advance-plan` refused it. Added `Current Plan` and `Total Plans in Phase` labels and reran the same helper successfully. No product or evidence-contract deviation.

## Issues Encountered

Git metadata writes required sandbox escalation; both task commits ran normal hooks under the configured noreply identity. No authentication or owner verification was needed. No review-fix or gap round was consumed.

## Limits and Next Readiness

This prerequisite completes no SAVE requirement. No battery fixture bytes, hardware implementation, firmware, API version change, hosted acceptance or release-size feasibility is claimed. Plan 07-02 owns the production HSC tracer and early real archive measurements. Its dependency is now satisfied; later execution and phase gates remain with the parent orchestrator.

All ten plan flags remain unresolved and visible in the unchanged plan: SAVE-01 unclassified; SAVE-02 unclassified; SAVE-03 unclassified; SAVE-04 idempotency and concurrency; SAVE-05 concurrency; SAVE-06 boundary, precision, idempotency and concurrency. Scheduled evidence is not resolution. Descriptor-less prohibition/intent flags retain their status and later review assignments.

No known stub, skipped test, unrun plan verification or new unplanned security surface remains. No codebase structural change requires map updates.

## Self-Check: PASSED

All three task files exist, both task commits are reachable, focused controls and full local CI pass, exact inventories and hygiene pass, and protected consumer/product inputs are unchanged. No unexpected deletion or generated untracked file remains. The next incomplete plan is 07-02; phase completion is pending.
