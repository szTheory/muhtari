---
phase: 08-controllers-at-the-pins
plan: "01"
status: complete
subsystem: testing
tags: [cmake, evidence, controllers, fixtures]
requires:
  - phase: 07-battery-saves
    provides: Authenticated recursive evidence production and narrow fixture/sample relations
provides:
  - Exact controller-light, controller-analog and controller-motion comparison permission
  - Exact annotation-only 4-to-5 transition with raw event-base sample identity
  - Real producer/join pass and fault controls preserving archive and source authority
affects: [08-02, 08-06, 08-14]
tech-stack:
  added: []
  patterns:
    - Extend existing controlled Git repositories and unchanged authenticated consumers
key-files:
  created:
    - .planning/phases/08-controllers-at-the-pins/08-01-SUMMARY.md
  modified:
    - tests/cmake/quality.cmake
    - tests/policy/quality.cmake
    - docs/quality.md
key-decisions:
  - Preserve separate fixture admission, device behavior and evidence comparison authorities.
  - Admit only the exact 4-to-5 annotation transition alongside existing equality and historical adjacent pairs.
requirements-completed: []
requirements-contributed: [INPUT-01, INPUT-02, INPUT-03, INPUT-04, INPUT-05]
coverage:
  - id: named-controller-fixture-relation
    description: Three exact controller additions preserve every trusted old manifest byte and raw base identity
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: "policy.quality.controls actual recursive producer and unchanged join; focused CTest exit 0; run URL N/A-local"
        status: pass
    human_judgment: false
  - id: exact-annotation-transition
    description: Equal revisions 4 and 5 and annotation-only 4-to-5 preserve trusted samples while malformed or changed content fails
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: "policy.quality.controls equality, forward, reverse, skipped, annotation, row, pin, source, digest, numeric, trailer and forged-trust cases; exit 0; run URL N/A-local"
        status: pass
    human_judgment: false
  - id: authenticated-evidence-authority
    description: Actual producer observations retain base, commit, run, compiler, platform and required native archive authority
    requirement: INPUT-05
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; exit 0; 641 native and 6 release tests; run URL N/A-local"
        status: pass
      - kind: other
        ref: "git diff --exit-code a05337b -- quality.json tests/cmake/quality_join.cmake tests/cmake/quality_evidence.cmake tests/roms/manifest.txt tests/built/expected.txt src/instance.c; exit 0; run URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 6224
  tasks: 2
  commits: 2
plan_head_before: a05337b48740f3a09ca64d4d04e98a17b4edaa75
plan_head_after: 4210dd55f70316829aad88ae5175ea253a13a102
duration: 6min
started: 2026-10-24T19:44:50Z
completed: 2026-10-24
---

# Phase 8 Plan 1: Controller evidence prerequisite Summary

**Three named controller fixture additions and exact annotation-only 4-to-5 migration preserve independent event-base identity through real evidence production and the unchanged authenticated join.**

## Accomplishments

- Extended the anchored fixture relation by exactly `controller-light.a78`, `controller-analog.a78` and `controller-motion.a78`. All historical admitted names and manifest predicates remain. The temporary repository includes every current real manifest row; controlled additions rename existing owned diagnostic observations while retaining actual source pins, MIT fields and byte digests. These are identity controls, not newly admitted product ROMs or controller diagnostics.
- Proved additions against an actual base Git object and a later base already containing the light row. Changed/deleted old or newly trusted rows, duplicate old/new/trusted rows, a suffix near-match, malformed/source/pin/license/digest faults and forged caller trust fail at the producer. Real observations reach the join, where altered run/commit/base and compiler/platform identity fail. Existing PR, push, dispatch, board, battery, sample and policy controls remain; no control was removed.
- Added only the exact 4-to-5 annotation pair, retaining equality, 2-to-3 and 3-to-4. Both equal endpoints and the forward transition pass through recursive production and the join. Reverse/skipped/arbitrary revisions, missing/duplicate/forged/unterminated annotations and altered rows, tool/source pins, digest, numeric field or trailer fail. Caller-supplied trusted sample inputs remain ineffective.
- Kept `quality.json`, actual manifest/sample bytes, production revision, evidence producer and join unchanged. The controlled producer also judges its real archive observation using the current required/native archive metric, whose independent ceiling remains 399937. Documentation explains comparison permission and the TIA D6/D7 correction's separate implementation obligations.

## Task Commits

1. Task 1: `20ae7ad` — prove narrow controller fixture comparability.
2. Task 2: `4210dd5` — constrain sample annotation migration to revision five.

The persisted per-plan ledger measures two task commits between the recorded heads. Actual tokens are the realized task diff's 24,895 characters divided by four, rounded upward; metadata is excluded.

## Commands, exits and run URLs

Every run URL below is **N/A-local**. No hosted candidate/base or six-native acceptance is claimed, and no log is committed.

| Command | Exit | Observed result |
|---|---:|---|
| `cmake --preset ci && ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure` before fixture expansion | 8 | New pass case rejected as `quality: unrelated fixture tests/diag/controller-light.a78`, establishing the intended failure direction |
| Same focused command after Task 1 | 0 | 1/1 pass, 6.67 s; tracer feedback rerun also passes, 6.98 s |
| Same focused command before annotation expansion | 8 | New 4-to-5 pass case rejected as `quality: invalid sample revision transition` |
| Same focused command after Task 2 | 0 | 1/1 pass, 7.13 s, including all retained and new negative controls |
| `cmake --workflow --preset ci` | 0 | 641/641 native tests, all three packages, 6/6 release checks; full-run quality control passes in 9.52 s |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 641 tests ran once each |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly six release tests ran once each |
| `scripts/hygiene.sh --tree` | 0 | No findings; commits also ran normal hooks with the configured noreply identity |
| `git diff --check` | 0 | No whitespace errors |
| `git diff --exit-code a05337b -- quality.json tests/cmake/quality_join.cmake tests/cmake/quality_evidence.cmake tests/roms/manifest.txt tests/built/expected.txt src/instance.c` | 0 | Policy, authoritative consumers, actual fixture/sample bytes and production behavior revision unchanged |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 08-01 --commits-only --raw` | 0 | Resolves both task commits on this branch |

The local package observation used `python3 -c 'import pathlib; a=list(pathlib.Path("build/ci").glob("muhtari-*.zip")); assert len(a)==3; print(sum(p.stat().st_size for p in a))'` (exit 0, run URL N/A-local): the three macOS arm64 archives total 228523 bytes. This is one local lane only, without independent comparable-base authority; it does not establish hosted or final controller archive fit.

## Decisions Made

Followed the existing owner and consumer seams. No dependency, new lane, registry, policy key, numeric threshold, maintenance quota, advisory promotion or alternative consumer was introduced. The primary-source TIA D6/D7 behavior correction and actual version transition remain with 08-06; comparison support alone changes neither hardware nor pinned product output.

## Deviations from Plan

**[Rule 2 - Contract] Document fixture behavior in its owning task.** AGENTS rule 5 requires behavior and documentation to land together. The fixture paragraphs of `docs/quality.md` therefore landed in Task 1 (`20ae7ad`) instead of waiting for Task 2; Task 2 owns the annotation documentation. Verification and implementation scope are unchanged.

## Issues Encountered

Git metadata writes required sandbox escalation. Both task commits then completed with hooks enabled; no authentication gate, owner verification, review-fix round or gap round was needed.

## Limits and Next Readiness

This prerequisite completes no INPUT requirement. Plan 08-02 can use the narrow comparison relation for its actual tracer fixture and early attributable package measurements. Later plans still own all controller implementation, legal fixture admission, numeric device judgments, production revision changes, cross-host equality and six-native acceptance.

The plan's descriptor-less provenance prohibition remains a flagged-unverified intent constraint, unchanged in the plan. This work introduces no asset bytes or external expression, and no GPL/LGPL/unlicensed emulator source was opened; future actual controller fixtures still require normal provenance/asset review and admission. Controlled schema observations are not that admission.

No known stub, skipped test, unrun plan verification or new unplanned trust-boundary surface remains. All state and policy authority remain with their existing owners.

## Self-Check: PASSED

All four claimed files exist, both task commits are reachable from this branch, and committed task changes match the tested source. Focused controls, full CI, exact native/release inventories, hygiene and whitespace checks pass. No unexpected deletion or generated untracked file remains. The next incomplete plan is 08-02; phase completion is pending.
