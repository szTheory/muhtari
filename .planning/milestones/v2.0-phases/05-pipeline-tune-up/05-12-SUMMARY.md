---
phase: 05-pipeline-tune-up
plan: "12"
status: halted
subsystem: infra
tags: [cmake, quality, performance, retained-stores, allocation]
requires:
  - phase: 05-05
    provides: Accepted ordinary aggregate, authentic original observations and fixed allocation
provides:
  - Restored retained perf allowance assertions with verified intentional RED
  - Selected semantic proof and measured one-line Task1 fit halt
affects: [05-07, 05-08, 05-09]
tech-stack:
  added: []
  patterns: [legacy ALLOW authority, shared original-observation authentication]
key-files:
  created:
    - .planning/phases/05-pipeline-tune-up/05-12-SUMMARY.md
  modified:
    - tests/perf/tests.cmake
    - tests/cmake/quality.cmake
    - tests/cmake/quality_join.cmake
    - tests/policy/quality.cmake
key-decisions:
  - Stop at the explicit Task1 gate after measuring quality921 and total14842; no control deletion, new allocation or gap round.
requirements-completed: []
requirements-contributed: [TUNE-03, TUNE-04]
coverage:
  - id: retained-perf-contract
    description: All seven perf cases retain exact arithmetic, verdict and input faults through ALLOW
    requirement: TUNE-04
    verification:
      - kind: unit
        ref: ctest --preset ci -R '^(perf[.]gate[.]|policy[.]quality[.]controls$)' --no-tests=error --output-on-failure; exit0;8/8
        status: pass
      - kind: other
        ref: cmake -DROM=testcard -DHEAD=1010000 -DBASE1=1000000 -DBASE2=1000000 -DALLOW=tests/perf/allow.txt -DARMED=true -DNO_BASE=false -P tests/cmake/perf_compare.cmake; exit0;delta_bp=100 verdict=PASS
        status: pass
    human_judgment: false
  - id: original-observation-authentication
    description: Aggregate controls and exact forged smaller-lane SHA rejection precede reduction
    requirement: TUNE-03
    verification:
      - kind: unit
        ref: policy.quality.controls in final eight-test selection; exit0
        status: pass
    human_judgment: false
  - id: strict-fit
    description: Quality fits920 and total fits14841 without changing other component use
    requirement: TUNE-03
    verification:
      - kind: other
        ref: gate_count exit0 observes14842; jq explicit14841/14853 gate exit1;quality921
        status: fail
    human_judgment: false
  - id: compatibility-and-current-callers
    description: Compatibility lifecycle, POKEY audit and current full local and hosted callers are accepted
    requirement: TUNE-04
    verification:
      - kind: integration
        ref: Task2/3 unrun after Task1 fit halt; no current hosted run URL or producer/base identity
        status: unknown
    human_judgment: false
actuals:
  tokens: 2657
  tasks: 0
  commits: 1
plan_head_before: ab92b4d57902ca7883bdf4ae920ded9de2b5b367
plan_head_after: 94d5c49b01feafb96833d5d1ce61786ab81dc261
duration: approx5min halted
completed: 2026-10-22
---

# Phase 5 Plan 12: Retained-store reconciliation Summary

**The retained perf path and original aggregate faults pass selected checks, but the bounded reconciliation halts one line above quality920 and total14841.**

Zero of three tasks are complete. Task1's explicit fit gate failed at14842/quality921.
Tasks2–3 never started. No GREEN, push, dispatch, current hosted acceptance or downstream
scheduling release exists. Three implementation files remain uncommitted: quality.cmake,
quality_join.cmake and the exact forged-SHA expectation in tests/policy/quality.cmake.

## Lineage and TDD

Execution began on phase/05-pipeline-tune-up at ab92b4d57902ca7883bdf4ae920ded9de2b5b367.
Accepted5535b57168ef31a8993c28646920a8b88ed0e98a remains the14841/quality920 reference.
Historical RED df6b0959dfbb31f843ada7fe3cbfc7e1b1f24c25 remains an ancestor;05-06 and05-11
remain halted histories, not completed migrations. No historical SUMMARY/HANDOFF/VERIFICATION
was changed. D-21–23 retain the existing authorities; this is no architectural change.

The new dispatcher restored empty/current/expired legacy allowance fixtures and every original
seven-case verdict assertion. Four additional checks cover incorrect ROM, malformed allow row,
zero HEAD and negative BASE2, using exact diagnostics within existing named cases.

Before changing the comparator, perf.gate.allowed executed and failed its intended assertion:
expected exit0 and delta_bp=200 verdict=PASS for a current matching ALLOW row; actual exit1
and delta_bp=200 verdict=FAIL because the dirty central reader ignored ALLOW. CTest exited8.
The unchanged generated JUnit report and record remain only in ignored build/plan-evidence.
The SDK classifier returned RED_EVIDENCE_OK, exit0; semantic inspection confirmed the named
target assertion, not a fixture, syntax, discovery or count failure.

RED commit94d5c49b01feafb96833d5d1ce61786ab81dc261 is
`test(05-12): restore retained perf allowance contract`; repository hooks and noreply identity
were used. There is no GREEN commit because selected passing tests do not override the fit gate.

## Reconciliation and measured cost

The deferred domains object and quality_domain reader were removed selectively. The retained
perf ALLOW parser and100-basis-point default now equal the accepted version byte-for-byte;
quality.json likewise equals its accepted version. No threshold changed. Aggregate-only bound
and reason wrappers returned to their accepted inline form. Useful quality_observation remains
shared with the join, authenticating each original item before extrema. Exact SHA values and
whitespace-tolerant forged-SHA expectation remain, with all ordinary aggregate controls.

| Path | Accepted5535b57 | Starting draft | Final | Final minus accepted |
|---|---:|---:|---:|---:|
| quality.json |101|109|101|0|
| tests/cmake/perf_compare.cmake |95|68|95|0|
| tests/perf/tests.cmake |67|83|73|6|
| tests/cmake/quality.cmake |230|284|238|8|
| tests/cmake/quality_join.cmake |114|101|101|-13|
| tests/policy/quality.cmake |201|201|201|0|
| Complete inventory |14841|14879|14842|1|

All146 whole-file inventory entries were compared against5535b57. Only the four counted paths
with final changes above differ; every other counted path is byte-identical. This includes
the sixth draft's zero-line-change exact diagnostic assertion. There are no hidden caller,
helper, fixture or store costs. Net recovery is37 from the starting14879; the previously
credited9-line recovery is not credited twice. The final four input/allow controls cost6 lines
over the accepted dispatcher; no control was removed to erase the measured deficit.

| Component | Fixed allocation | Accepted actual | Final actual |
|---|---:|---:|---:|
| Count/inventory |368|368|368|
| BE relocation/equality |205|193|193|
| Digest |447|447|447|
| Quality, including controls/glue |920|920|921|
| Reproduced fixes |30|30|30|
| Reserve |0|0|0|
| Total incremental scope |1970|1958|1959|

B12885-S2+N1970=C14853 remains fixed. Actual12885-2+1959=14842 is11 below C,
but one above strict14841; quality14842-13921=921 is one above920. BE12 is not reserve.
Full/authored source8580/6533 is unchanged and ratios remain advisory. No padding, line
density, comment/control loss, hidden exclusion, threshold change or allocation transfer
was used. The exact hard gate stops expansion; no further savings search was attempted.

## Verification evidence

All current checks are local; run URL is unavailable. Final verification followed the last
implementation edit. Commands and exits are recorded here; raw logs are not committed.

| Command | Exit | Result |
|---|---:|---|
| cmake --preset ci && ctest --preset ci -R '^perf[.]gate[.]allowed$' --no-tests=error --output-on-failure --output-junit ../plan-evidence/reconciliation-red.xml |8| Intended current-ALLOW PASS assertion fails before comparator change |
| node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/plan-evidence/reconciliation-red.json --raw |0| RED_EVIDENCE_OK; one named assertion failure |
| cmake --preset ci && ctest --preset ci -R '^(perf[.]gate[.]\|policy[.]quality[.]controls$)' --no-tests=error --output-on-failure |0| All seven perf cases and aggregate/join control case pass8/8 |
| cmake -DROM=testcard -DHEAD=1010000 -DBASE1=1000000 -DBASE2=1000000 -DALLOW=tests/perf/allow.txt -DARMED=true -DNO_BASE=false -P tests/cmake/perf_compare.cmake |0| delta_bp=100 verdict=PASS |
| cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/reconciliation-count.json -P tests/cmake/gate_count.cmake |0| Full physical count14842 |
| jq -e '.gate_lines <= 14841 and .gate_lines <= 14853' build/plan-evidence/reconciliation-count.json |1| false; explicit strict fit failure |
| python3 physical-line and byte comparison of every inventory path against git show5535b57:path |0| Only four quality-component paths differ; other components invariant |
| git diff --check |0| No whitespace errors |
| scripts/hygiene.sh --tree |0| Tree hygiene passes |
| git merge-base --is-ancestor df6b095 HEAD && git merge-base --is-ancestor 94d5c49 HEAD |0| Both RED histories retained |

## Deferred acceptance and legacy limits

Task2's compatibility equality/lifecycle extension and POKEY audit are unrun. The compatibility
judge, library caller, ledger, allow store and lifecycle docs were not edited. No fresh accepted-base
or generated-corpus replacement evidence was obtained in this execution. The old POKEY row was
left with its existing expiry; it was neither retired on date alone nor renewed.

Both domain stores and production callers remain active. Performance uses the restored legacy
ALLOW contract; compatibility retains unconditional fresh ledger/results equality and its legacy
trusted-pass exception behavior. As accepted by D-23, a waived merged regression may become the
next base's nonpass state. No immutable domain grant, anti-renewal, automatic restoration or unified
exception governance is claimed. Full proof/documentation of those retained limits remains Task2.

Full `cmake --workflow --preset ci`, generated-corpus library with verified origin/main,
documentation budgets, current direct numeric perf/executed library/quality, release-called required
CI, JUnit inventory authentication and exact nine/eight-artifact joins are unrun after the Task1 stop.
DIRECT_RUN/CALLED_RUN, current producer/merge/head/base SHA, source and run/attempt identities
remain unset, not guessed. No current run URL exists. Prior accepted05-05
[direct39706507168](https://github.com/szTheory/muhtari/actions/runs/39706507168) and
[called39706506980](https://github.com/szTheory/muhtari/actions/runs/39706506980) remain earlier
evidence only and cannot accept this draft.

## Deviations, actuals and remaining assumptions

The explicit planned Task1 fit halt fired; this is a measured construction miss, not a new semantic
RED or architectural decision. WINDOWS20 records the one-line miss; WINDOWS21 records unrun
Task2/3 acceptance. Historical ledger entries remain unchanged. No goal-blocking stub or skipped
test was introduced; untouched later tasks are explicitly unimplemented/unrun. No new endpoint,
auth flow or filesystem trust boundary was introduced. No authentication gate occurred.

Actual tokens2657=ceil(10626 realized implementation-diff characters/4) against starting HEAD,
including retained draft lineage. The persisted per-plan ledger measures one RED commit from
ab92b4d to94d5c49 at summary creation. The following summary/state commits are metadata only.

All seven surfaced edge rows remain flagged unresolved: TUNE-01 unclassified; TUNE-02 adjacency,
empty and ordering; TUNE-03 unclassified; TUNE-04 unclassified; TUNE-05 unclassified. The descriptorless
physical-boundary prohibition remains unresolved/flagged-unverified. Concrete controls do not
resolve classifier lifecycle. Shared TUNE-03/TUNE-04 remain pending; active completion stays5/9.
Neither06 nor11 becomes complete. Plans07/08/09 remain blocked on accepted12.

No new allocation, gap round, owner UAT, arming, promotion, hosted push, ship, merge or second GSD
step ran. Next command in a fresh session: `$gsd-discuss-phase 5` to address this concrete hard-gate
miss; it authorizes no allocation change or automatic gap round.

## Self-Check: PASSED

The new summary, all affected files and both retained allow stores exist; both RED commits are
ancestors of HEAD. The per-plan ledger measures one commit and the recorded before/after heads
match. Final8/8 selected checks, production ALLOW invocation, hygiene and diff checks pass while
the explicit numeric gate fails. Historical artifacts remain unchanged. This verifies the halt
record only and does not claim task, plan or phase completion.
