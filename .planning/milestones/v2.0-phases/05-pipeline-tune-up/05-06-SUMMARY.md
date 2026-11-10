---
phase: 05-pipeline-tune-up
plan: "06"
status: halted
subsystem: infra
tags: [cmake, quality, allocation, performance]
requires:
  - phase: 05-05
    provides: Typed quality reasons and fixed quality allocation
provides:
  - Committed failing control for central performance reasons
  - Measured uncommitted migration draft and explicit allocation halt
affects: [05-06, 05-07, 05-08, 05-09]
tech-stack:
  added: []
  patterns: [existing CTest dispatcher and exact-failure controls]
key-files:
  created:
    - .planning/phases/05-pipeline-tune-up/05-06-HANDOFF.md
  modified:
    - tests/perf/tests.cmake
    - tests/cmake/quality.cmake
    - tests/cmake/perf_compare.cmake
    - quality.json
key-decisions:
  - Halt the affected cutover at measured quality967 versus allocation920; no cap increase or use of BE12 is authorized.
requirements-completed: []
coverage:
  - id: perf-red
    description: Central current-reason control fails against the original semantic comparator for the intended verdict assertion
    requirement: TUNE-03
    verification:
      - kind: unit
        ref: ctest --preset ci -R '^perf[.]gate[.]allowed$' --output-junit build/plan-evidence/perf-red.xml --output-on-failure; exit8 before implementation; RED_EVIDENCE_OK
        status: pass
    human_judgment: false
  - id: draft-controls
    description: Uncommitted reader draft preserves seven perf cases and the existing quality controls; this is partial evidence only
    requirement: TUNE-03
    verification:
      - kind: unit
        ref: ctest --preset ci -R '^(perf[.]gate[.]|policy[.]quality[.]controls$)' --output-on-failure; eight tests; exit0
        status: pass
    human_judgment: false
actuals:
  tokens: 2703
  tasks: 0
  commits: 1
plan_head_before: 5535b57168ef31a8993c28646920a8b88ed0e98a
plan_head_after: df6b0959dfbb31f843ada7fe3cbfc7e1b1f24c25
duration: 5min execution plus initial context loading
completed: 2026-10-22
---

# Phase 5 Plan 6: Central domain policy migration halt Summary

**The central-reason RED test is committed, but the passing perf migration draft exceeds the fixed allocation and the production cutover is halted.**

No task is complete. Task1 is partial, Tasks2–3 are unstarted, and shared TUNE-03/TUNE-04
remain pending. This summary records a durable stop, not completed migration or hosted acceptance.
The recorded execution interval is 2026-10-22T02:29:01Z–02:34:04Z, after initial context loading.

## Exact committed and working-tree state

Only `df6b095` (`test(05-06): exercise central performance reasons`) is a task commit.
It changes `tests/perf/tests.cmake` to exercise generated JSON policies. The original
comparator ignores POLICY and rejects the intended valid +2.00% temporary allowance.
The target assertion expected PASS and observed FAIL; exit8 is an assertion failure,
not a parser, fixture, build or missing-tool failure. The unchanged JUnit evidence passed
`node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/plan-evidence/perf-red.json --raw`
with exit0 and RED_EVIDENCE_OK. Evidence remains ignored build output, never a committed log.

The following implementation is **uncommitted** and retained for recovery:

- `quality.json`: proposed domains/metrics test-card ceiling100.
- `tests/cmake/quality.cmake`: narrow domain reader plus shared reason-pair and loosening checks.
- `tests/cmake/perf_compare.cmake`: central-policy consumption in place of the allow-file parser;
  integer cross-multiplication, positive measurements, duplicate-base equality and ARMED/NO_BASE remain.
- `tests/perf/tests.cmake`: remove the temporary RED-only ALLOW fixture after the draft consumes POLICY.

There is no GREEN commit, production caller switch, compatibility migration or retired-file deletion.
Both old allow files and their inventory entries remain. The committed RED alone adds17 physical
lines to14841, yielding14858; it deliberately cannot be treated as a releasable green tree.
Do not discard the draft or mistake its passing controls for a committed implementation.

## Verification and limits

| Command | Exit | Meaning |
|---|---:|---|
| `cmake --preset ci` |0| Draft configures |
| `ctest --preset ci -R '^perf[.]gate[.]' --output-on-failure` |0| All seven original names pass on the draft, including exact +1.00/+1.01 behavior and new inherited-expiry/lifetime/unrelated-reason cases |
| `ctest --preset ci -R '^(perf[.]gate[.]\|policy[.]quality[.]controls$)' --output-on-failure` |0| Eight existing-dispatcher tests pass after common reason extraction |
| `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/count-final-halt.json -P tests/cmake/gate_count.cmake` |0| Observes14888 physical lines; the observation command does not claim budget compliance |
| `git diff --check` |0| Draft whitespace check |
| `git merge-base --is-ancestor df6b095 HEAD` |0| RED commit is retained |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" query requirements.ready-ids .planning/phases/05-pipeline-tune-up/05-06-PLAN.md TUNE-03 TUNE-04` |0| No ready IDs; both shared requirements remain blocked |

No current hosted run was dispatched or pushed for this partial draft: run URL unavailable.
[Prior direct CI39706507168](https://github.com/szTheory/muhtari/actions/runs/39706507168)
and [prior called CI39706506980](https://github.com/szTheory/muhtari/actions/runs/39706506980)
belong to05-05, not this draft. Existing PR20 remains at implementation51a3f95 remotely.

The full `cmake --workflow --preset ci` and generated-corpus
`cmake -E env MUHTARI_ROM_DIR="$PWD/build/ci/built" cmake --workflow --preset library`
are unrun for this migration. Compatibility central-reason/equality controls, trusted-base
production wiring and final aggregate/domain independence are not proven by the eight passing tests.
No owner ROMs, settings, secrets, lane promotions, merge or downstream plan were touched.

## Measured allocation stop

Frozen B12885, S2, N1970 and C14853 are unchanged. Before this plan actual14841 and
quality920 exactly consumed the quality allocation. The first draft measured14892;
extracting common reason-pair/loosening checks reduced it to14888. No control/comment
was deleted to clear the gate and no scoped allowance was enlarged.

| Changed path versus5535b57 | Net physical lines |
|---|---:|
| quality.json |8|
| tests/cmake/quality.cmake |50|
| tests/cmake/perf_compare.cmake |-27|
| tests/perf/tests.cmake |16|
| Total draft increase |47|

| Component | Fixed allocation | Draft actual incremental lines |
|---|---:|---:|
| Count/inventory |368|368|
| BE relocation/equality |205|193|
| Digest |447|447|
| Quality, including domain controls/glue |920|967|
| Reproduced fixes |30|30|
| Reserve |0|0|
| Total |1970|2005|

B12885 - S2 + actual2005 =14888: **35 over C14853 and47 over quality920**.
The unused BE12 is not reserve. Remaining legacy-parser/store retirement may save lines,
but unimplemented compatibility controls and trusted-base caller glue also cost lines;
unmeasured future savings are not credited. The orchestrator confirmed the explicit plan stop
and that no cap/allocation increase is authorized.

Actual tokens are ceil(10809 realized task-diff characters/4)=2703, including the retained
uncommitted draft, excluding planning metadata. The ledger measures one commit from5535b57
to df6b095 at summary creation; it does not count proposed changes as commits.

## Deviations and deferred issues

The plan's explicit allocation-overrun stop fired during Task1 before GREEN. This is a
measured construction failure requiring bounded replanning, not permission to weaken controls
or silently baseline growth. WINDOWS entries16 and17 record the unrun caller acceptance
and allocation stop. There are no newly introduced code stubs; Tasks2–3 remain unimplemented.
The old POKEY revision-migration reason is spent: origin/main and current ledger both
contain its replacement passing predicate. Recovery should retire it rather than extend it.

The draft's reader and complete policy invariants still need review in recovery; only the
listed tests were exercised. No claim is made that trusted-base wiring, all domain deletion/
identity cases or the compatibility requirements are implemented. No authentication gate occurred.

## Recovery and next action

Required decision: choose a fault-preserving consolidation within quality920 and C14853,
or explicitly revise the bounded migration scope through the existing new-evidence gap route.
No larger cap, BE transfer, hidden exclusion, comment stripping or loss of controls is authorized.

Next supported recovery command: `$gsd-plan-phase 5 --gaps`, using this new measured evidence
and `05-06-HANDOFF.md`; it is not executed here. Do not run05-07 or mark05-06/shared requirements
complete until a revised executable plan fits the unchanged allocation and is verified.

## Self-Check: PASSED

The RED commit exists in HEAD ancestry. All four draft files exist, the final eight-test
selection and counter ran with exit0 after the last implementation edit, and the measured
overrun is explicitly retained. This summary and its handoff exist before state updates.
Self-check confirms the halt record only; no completed migration is claimed.
