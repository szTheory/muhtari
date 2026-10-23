---
phase: 05-pipeline-tune-up
plan: "07"
status: complete
tasks_completed: 2
subsystem: infra
tags: [ci, cachegrind, nightly, portability, evidence]
requires:
  - phase: 05-10
    provides: Accepted native and PowerPC execution contracts
  - phase: 05-13
    provides: Accepted retained authorities and strict quality919/total14840 reconciliation
provides:
  - Armed test-card performance comparison after fresh deterministic controls and observed nightly recovery
  - Actual main-lane execution audit with WASM, library and property remaining advisory
affects: [05-08, 05-09]
tech-stack:
  added: []
  patterns: [measured arming, distinct actual lane executions, unchanged legacy domain authorities]
key-files:
  created:
    - .planning/phases/05-pipeline-tune-up/05-07-SUMMARY.md
  modified:
    - .github/workflows/ci.yml
    - .github/workflows/nightly.yml
    - docs/testing.md
    - .planning/phases/05-pipeline-tune-up/deferred-items.md
key-decisions:
  - Arm only the measured test card after two fresh same-commit zero-spread triples, a numeric authenticated PR base and actual nightly issue recovery.
  - Retain WASM9, library5 and property2 as advisory because none has ten distinct successful qualifying main executions.
  - Keep DMA/TIA/POKEY outside the Ir claim without their own controls and proved charged integration fit; four available fixtures alone confer no coverage.
  - Accept the scoped mandatory proofs while explicitly leaving the extra final whole-CI run pending its RetroArch runner and roll-up; no overall green is claimed.
requirements-completed: []
requirements-contributed: [TUNE-04, TUNE-02]
coverage:
  - id: armed-test-card
    description: Existing exact performance comparator is armed only after actual readiness evidence
    requirement: TUNE-04
    verification:
      - kind: unit
        ref: cmake --preset ci && ctest --preset ci -R '^perf[.]gate[.]' --output-on-failure; exit0;7/7
        status: pass
      - kind: integration
        ref: CI39836658273 and CI39836675687; independent3837262/3837262/3837262 triples; spread0; actual perf producers succeeded
        status: pass
      - kind: integration
        ref: CI39836667094 attempt2; numeric head3837262/base3837262/repeat3837262; actual current PR merge and first-parent identities authenticated
        status: pass
      - kind: integration
        ref: CI39839399512 perf producer120036186233; actual MUHTARI_PERF_ARMED=true; delta_bp=0 verdict=PASS; quality120041448984 succeeds
        status: pass
    human_judgment: false
  - id: nightly-failure-recovery
    description: Actual forced failure updates the rolling issue and normal job results subsequently close it
    requirement: TUNE-04
    verification:
      - kind: e2e
        ref: Nightly39836662246 designated Forced failure after artifact12450673532; reporter succeeded and issue21 comment6349059125 records actual failed need
        status: pass
      - kind: e2e
        ref: Nightly39837920876 all9 jobs successful; reporter closed issue21 with comment6349240011; gh issue view exit0 confirms CLOSED
        status: pass
    human_judgment: false
  - id: evidence-limited-lanes
    description: Advisory statuses and unchanged required membership follow distinct actual successful main-lane executions
    requirement: TUNE-02
    verification:
      - kind: other
        ref: python3 build/plan-evidence/readiness-audit.py; exit0; all10 direct main-push CI runs/11 attempts and6 main Nightly runs/attempts audited
        status: pass
      - kind: integration
        ref: cmake --workflow --preset ci; exit0;609 normal plus6 release tests; unchanged exact inventories
        status: pass
      - kind: integration
        ref: PATH="/opt/homebrew/opt/llvm@21/bin:$PATH" cmake --workflow --preset hygiene; exit0;24/24 including live roll-up and omission controls
        status: pass
    human_judgment: false
  - id: fixed-budget-and-coverage
    description: No workload or lane promotion increases counted scope or makes an unmeasured gameplay claim
    requirement: TUNE-04
    verification:
      - kind: other
        ref: gate_count exit0;14840 physical lines; explicit strict14841/C14853 jq gate exit0; all146 inventory entries compared to625db7a
        status: pass
      - kind: other
        ref: cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake; exit0;19 files; testing.md40849/40960 bytes
        status: pass
      - kind: other
        ref: Existing DMA/TIA/POKEY runner60/360-frame attempts all exit0; no new Ir fixture is claimed without independent controls
        status: pass
    human_judgment: false
actuals:
  tokens: 2340
  tasks: 2
  commits: 2
plan_head_before: 625db7a8176dc310413903e99b8124237c12821e
plan_head_after: 7ed793e81ccf07a07642681b79c08122be641996
duration: 38min plus closeout
started: 2026-10-22T19:02:39Z
completed: 2026-10-22
---

# Phase 5 Plan 7: Controlled test-card arming and actual lane history Summary

**The +1.00% test-card gate is armed after two zero-spread triples, a current numeric PR base and a real nightly failure/issue-close cycle; WASM/library/property remain advisory at9/5/2.**

Both tasks are complete. Active acceptance becomes7/9:01,02,10,04,05,13,07.
Plans05-08/05-09 remain unstarted. Halted05-06/05-11/05-12 stay0/3 histories.
Shared TUNE-02/TUNE-04 remain pending; the readiness query returns ready[] and blocks both.
No phase completion, promotion, new allocation, extra gap round, ship or merge occurred.

## Performance

- Started2026-10-22T19:02:39Z; mandatory proofs and final scope-limited acceptance recorded around19:41 UTC.
- Two tasks; three task paths plus one existing deferred-seed update.
- Actual2340 tokens is9360 realized task/seed diff characters divided by4, excluding closing SUMMARY/STATE/ROADMAP metadata. It is not a harness-token count.
- The persisted plan ledger measures two task commits from625db7a through7ed793e before closing metadata commits.

## Accomplishments

- Enabled only the existing measured test-card gate after every readiness predicate was actually proved.
- Observed designated nightly failure after artifact upload, an actual rolling-issue update and a normal successful run closing that issue.
- Audited all available qualifying main-run pages and attempts; retained honest advisory lane contracts and exact required membership.
- Kept quality919/920, strict14840/14841 and C14853 with no counted growth or weakened control.

## Task Commits

1. Task1, fresh controls and nightly recovery — `2a68b3b`, `ci(05-07): arm controlled test-card performance comparison`.
2. Task2, audit actual lane executions — `7ed793e`, `docs(05-07): retain advisory lanes from actual main history`.

Normal hooks ran. The sandbox initially refused writes to the read-only Git directory; the already authorized commits succeeded with the Git write escalation, without a hook bypass.

## Accepted dependency and retained contracts

Before dispatch, the agent read accepted05-13 and verified the live counter14840,
strict fit, unchanged runtime/check implementations since31c52d9, current accepted local
609+6/library43 evidence and actual direct/called required evidence. Direct
[CI39832530167](https://github.com/szTheory/muhtari/actions/runs/39832530167)
was completed success; release-called required CI/quality succeeded in
[Release39832530253](https://github.com/szTheory/muhtari/actions/runs/39832530253)
while its overall advisory WASM fetch failure remained explicit. Halted06/11/12 and
earlier greens were not substituted for that acceptance.

`quality.json`, both retained allow stores, all seven perf cases and their added
wrong-ROM/malformed/zero/negative assertions, exact arithmetic and independent judges
remain. Legacy domain expiry is reviewed behavior, not immutable grants, anti-renewal
or automatic restoration.

## Fresh deterministic controls and current numeric base

All readiness runs use immutable625db7a8176dc310413903e99b8124237c12821e.
Its runtime implementation equals accepted31c52d9. Each control's actual perf job
completed successfully; both full control workflows also completed success.

| Run | Attempt / actual perf job | Measurements Ir/frame | Spread |
|---|---|---|---:|
| [CI39836658273](https://github.com/szTheory/muhtari/actions/runs/39836658273) |1 /120026721454|3837262,3837262,3837262|0|
| [CI39836675687](https://github.com/szTheory/muhtari/actions/runs/39836675687) |1 /120031254369|3837262,3837262,3837262|0|

Both jobs report Ubuntu26.04 image20261012.163.1 and Valgrind3.26.0.
The current PR artifact identifies default GCC15.2.0-16ubuntu1/Valgrind3.26.0 on that
same image. Workflow code retains the same compiler/tool selection, equal-length
`perf/head` and `perf/base` install paths, `env -i`, `--cache-sim=no`, and
integer `(Ir360-Ir60)/300`. Every observed value is positive.

[Direct PR CI39836667094](https://github.com/szTheory/muhtari/actions/runs/39836667094)
attempt2 completed success. Authentic artifact12451696892, created19:14:42 UTC,
has digest `sha256:53933698bbed701dd9a4e1281389648bb4beb0bd7e799cd9d42a45505b42d2c0`.
It reports measured head3837262, baseline3837262 and repeated3837262.
Producer merge122a83b85a6268f0576d004c1ed0cc904615c2a7 has actual first parent
65b360d1afd37b7dad9f94dcff497c85df84fd1d and second parent625db7a.
Its source URL, producer, units, direction, workload and tools match the actual run;
there is no NO_BASE or unstable-base substitution.

The first PR perf attempt120026758268 hit the unchanged five-minute Cachegrind
package-install bound before building/measuring; quality/required rejected the missing
evidence. One unchanged-head perf-job rerun recovered it. Successful original producers
remain successful original executions: the attempt API reassigns some job IDs while
retaining their older execution timestamps. No copied timestamp is a new sample.

After all readiness predicates passed, final task head7ed793e was pushed.
Its actual armed perf producer120036186233 in
[CI39839399512](https://github.com/szTheory/muhtari/actions/runs/39839399512)
logs `MUHTARI_PERF_ARMED: true` and
`head=3837262 base=3837262 delta_bp=0 verdict=PASS`.
Authentic perf artifact12452172206, created19:22:22 UTC, reports repeated3837262,
producer mergec7ebbbbe135b6691be93be55c9b507e96f1edabe and the same authenticated
65b360d base/tools/workload. Its digest is
`sha256:d52200efa28e6a8d08d65277466de46b85c0b554cd2edf0aac6b1b1e8a0dc71f`.
The nine quality artifacts are present; quality120041448984 succeeds.

**Extra final whole-CI status is explicitly pending.** At19:40:54 UTC, attempt2 is
queued solely on RetroArch120041448613; the required roll-up is not yet present.
No overall green or completed final required roll-up is claimed. Mandatory scoped
acceptance uses the fresh controls/current numeric PR, actual armed perf, completed
nightly recovery, post-edit local workflows, audit and budget proofs above.

## Actual nightly failure, issue update and recovery

[Forced Nightly39836662246](https://github.com/szTheory/muhtari/actions/runs/39836662246)
fails only its designated `bench-wasm32 / Forced failure` after a successful bench
and artifact12450673532 upload. Every other job, including complete native/PowerPC
equality and the advisory property lane, succeeds. Reporter120030161057 succeeds
and [updates issue21](https://github.com/szTheory/muhtari/issues/21#issuecomment-6349059125)
from actual needs results.

[Normal recovery39837920876](https://github.com/szTheory/muhtari/actions/runs/39837920876)
has all nine jobs successful. The reporter120034563283
[closes issue21](https://github.com/szTheory/muhtari/issues/21#issuecomment-6349240011).
The agent confirms CLOSED through `gh issue view`; this was not owner homework.

One accidental second forced dispatch,
[39836680032](https://github.com/szTheory/muhtari/actions/runs/39836680032),
was cancelled. Some jobs had already started. Its always-reporter actually opened
issue21 for cancelled needs; the intended failure then updated it and recovery closed
it. That extra execution and its cost are retained, not erased or treated as a product
failure sample.

| Actual run | Outcome | Observed job execution seconds | Runner minutes |
|---|---|---:|---:|
| Intended forced39836662246 |designated failure; report success|1290|21.50|
| Accidental duplicate39836680032 |cancelled; report success|509|8.48|
| Normal recovery39837920876 |all9 successful; issue closed|1332|22.20|

These are API started/completed execution-time sums for actually started jobs,
including setup/report work, not billed-minute claims. Intentional/cancelled control
cost totals3131 seconds; none of these branch runs counts toward main promotion.

## Existing diagnostic decision

The existing card plus DMA/TIA/POKEY would meet the numerical four-fixture ceiling.
Actual local runner attempts execute the admitted DMA image and already-built
soundtest/POKEY images without adding bytes:

| Existing fixture |60 frames, exit0|360 frames, exit0|420-frame native wall total |
|---|---:|---:|---:|
| DMA |0.0125s|0.0668s|0.0793s|
| TIA soundtest |0.0138s|0.0745s|0.0883s|
| POKEY |0.0574s|0.3306s|0.3880s|

These are one-off native wall observations, not Cachegrind Ir or a new gate.
The local `env -i valgrind --version` probe exits127: that tool is unavailable.
The existing hosted producer measures only the test card. No additional fixture has
its own two controlled triples, numeric PR/base/threshold proof or measured charged
invocation/glue fit inside the one remaining strict/quality line. No extra workload
is admitted and no gameplay performance protection is claimed. Wall-clock tails
remain advisory; this does not prove all possible expansions infeasible.

## Actual main-lane audit

`gh api --paginate --slurp` fetched every main-push CI run page and every attempt's
job pages, plus every qualifying main Nightly page/attempt. The audit covers10
direct main-push CI runs across11 attempts and6 main scheduled Nightly runs/attempts.
It checks actual completed successful lane jobs and their successful inventory/
comparison steps. An overall workflow green alone is not counted.

| Main CI run |WASM actual lane|Library actual lane|
|---|---|---|
| [39617761785](https://github.com/szTheory/muhtari/actions/runs/39617761785)|skipped|success|
| [39533130206](https://github.com/szTheory/muhtari/actions/runs/39533130206)|success|success|
| [39532369201](https://github.com/szTheory/muhtari/actions/runs/39532369201)|success|success|
| [39530116644](https://github.com/szTheory/muhtari/actions/runs/39530116644)|attempts1/2 succeed; count once|attempts1/2 fail|
| [39470886147](https://github.com/szTheory/muhtari/actions/runs/39470886147)|success|success|
| [39470341335](https://github.com/szTheory/muhtari/actions/runs/39470341335)|success|success|
| [39321738931](https://github.com/szTheory/muhtari/actions/runs/39321738931)|success|absent|
| [39321295936](https://github.com/szTheory/muhtari/actions/runs/39321295936)|success|absent|
| [38983378694](https://github.com/szTheory/muhtari/actions/runs/38983378694)|success|absent|
| [38983173972](https://github.com/szTheory/muhtari/actions/runs/38983173972)|success|absent|

Property actually succeeds in
[39722666868](https://github.com/szTheory/muhtari/actions/runs/39722666868) and
[39544379935](https://github.com/szTheory/muhtari/actions/runs/39544379935).
It is absent in main Nightly runs
[39369810454](https://github.com/szTheory/muhtari/actions/runs/39369810454),
[39199935193](https://github.com/szTheory/muhtari/actions/runs/39199935193),
[39097219369](https://github.com/szTheory/muhtari/actions/runs/39097219369) and
[39002643243](https://github.com/szTheory/muhtari/actions/runs/39002643243).

Thus observed distinct successful executions are WASM9/10, library5/10 and prop2/10:
deficits are at least1,5,8. These sub-threshold counts cannot authorize promotion;
real ledger/base and exact current contracts would also be required before one.
PR controls, branch dispatches, unrelated workflows, missing/skipped/cancelled jobs
and duplicate attempts confer no eligibility. No extra main executions were generated.

No promotion changes were earned: six native required hash files, separate WASM
native-equality/inventory checks, actual library ledger/base execution, PowerPC PR
compilation/full-nightly roles and report-only property behavior remain. Property's
continue-on-error/report-needs and bounds are unchanged. Live roll-up/omission
controls pass; no inventory or artifact count was loosened.

## Fixed budget and actual verification commands

All146 counted entries were compared against625db7a. Only ci.yml and nightly.yml
change within that inventory, with645->645 and244->244 physical lines. Every other
counted path, quality.json, comparator, allow store, inventory and control is unchanged.

| Component | Fixed allocation | Actual |
|---|---:|---:|
|Count/inventory|368|368|
|BE relocation/equality|205|193|
|Digest|447|447|
|Quality including controls/glue|920|919|
|Reproduced fixes|30|30|
|Reserve|0|0|

B12885-S2+N1970=C14853 remains fixed. Incremental actual1957 yields14840,
within strict14841 and quality920. BE12 is not reserve. Source8580/6533 ratios
remain advisory. Final tightening/retirement is05-09 work.

| Command |Exit|Result / run evidence |
|---|---:|---|
|cmake --preset ci && ctest --preset ci -R '^perf[.]gate[.]' --output-on-failure|0|7/7 after arming edit|
|cmake --workflow --preset ci|0|post-final-edit609 normal and6 release tests|
|PATH="/opt/homebrew/opt/llvm@21/bin:$PATH" cmake --workflow --preset hygiene|0|post-final-edit24/24|
|cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake|0|609 executed once each|
|cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake|0|6 executed once each|
|cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/readiness-final-count.json -P tests/cmake/gate_count.cmake|0|14840 at final task HEAD7ed793e|
|jq -e '.gate_lines <= 14841 and .gate_lines <= 14853' build/plan-evidence/readiness-final-count.json|0|true|
|cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake|0|19 files; testing.md40849/40960 bytes|
|python3 build/plan-evidence/readiness-audit.py|0|actual9/5/2, all pages/attempts|
|gh workflow run ci.yml --ref phase/05-pipeline-tune-up|0 twice|independent controls39836658273/39836675687 above|
|gh workflow run nightly.yml --ref phase/05-pipeline-tune-up -f force_failure=true|0|forced39836662246; duplicate/cancel separately retained above|
|gh run cancel 39836680032|0|cancel request; actual started cost/reporter retained|
|gh workflow run nightly.yml --ref phase/05-pipeline-tune-up -f force_failure=false|0|normal39837920876 closes issue21|
|gh run list --workflow ci.yml --limit 20 --json databaseId,headSha,event,status,conclusion,url|0|fresh actual control/current-run inventory|
|gh run rerun 39836667094 --job 120026758268|0|same-head perf recovery; attempt2 success|
|gh run rerun 39839399512 --job 120036186384|0|same-head Windows recovery; extra final roll-up still pending|
|gh run view 39839399512 --json attempt,status,conclusion,jobs|0|attempt2 queued RetroArch; perf/quality succeed; no overall green|
|gh issue view 21 --json state,url,comments|0|actual CLOSED and failure/recovery links above|
|git diff --check 625db7a..HEAD; git diff --check HEAD|0/0|current plan and working changes clean|
|git diff --check origin/main...HEAD|2|same three inherited EOF blank-line warnings; not a passing broad check|

Generated fixtures, API snapshots, downloaded artifacts, one-off scripts and logs stay
ignored under build/plan-evidence. No log, ROM, BIOS, save byte or new dependency is committed.
Local-only commands have no hosted run URL; hosted evidence links are given above.

## Issues Encountered

- The accidental duplicate forced dispatch is preserved with actual issue creation,
  cancellation and509 seconds of started-job work; it was not silently discarded.
- Readiness PR Cachegrind installation hit its unchanged bound; one same-head rerun
  recovered the measured comparison without changing tools, controls or limits.
- Extra final CI39839399512 attempt1 Windows x64 job120036186384 passes current609+6
  and inventories, then unchanged trusted-base605 install.consumer emits no output and
  fails its required output regex after2.19s. Attempt2/job120039214618 succeeds with
  exact current609+6/base605+6 inventories. The prior consumer seed is updated;
  no cause or source repair is claimed. Its dependent RetroArch/roll-up remains pending.
- The broad diff check still exits2 on inherited EOF blanks in be-equality.yml:63,
  gate-lines.txt:148 and historical05-01-SUMMARY.md:421. Existing WINDOWS/deferred
  history remains; no extra cleanup, allocation or gap round was added.
- The automatic final-head
  [PR Nightly39839399510](https://github.com/szTheory/muhtari/actions/runs/39839399510)
  is completed success. The automatic
  [Release39839399527](https://github.com/szTheory/muhtari/actions/runs/39839399527)
  is still queued at closeout. No completed Release/rehearsal or phase08 acceptance
  is claimed, and active runs were not cancelled.

## Deviations from Plan

No implementation scope deviation. Conditional fixture/lane expansion was not earned
and was correctly omitted. Operational duplicate/retry observations and the scoped
extra whole-CI pending status are disclosed above. No mandatory plan verification was
skipped or left unrun. No goal-blocking stub or new security surface was introduced.

All seven supplied assumptions remain unresolved: TUNE-01 unclassified; TUNE-02
adjacency, empty and ordering; TUNE-03 unclassified; TUNE-04 unclassified; TUNE-05
unclassified. The TUNE-04 edge-probe category and its descriptorless prohibition on
claiming unmeasured gameplay/unqualified portability remain flagged/unverified;
the earlier TUNE-03 physical-boundary prohibition also remains flagged/unverified.
Concrete controls do not silently resolve the classifier lifecycle.

## Next Phase Readiness

Plan05-08 is next and remains unstarted;05-09 remains pending behind it.
Shared requirements and phase acceptance remain open. The exact next command is
`$gsd-execute-phase 5`. Stop after this execution step.

## Self-Check: PASSED

All claimed files exist; both task commits are ancestors of HEAD. Current-plan and
working-tree diff checks pass. The extra whole-CI pending status is explicitly
excluded from any overall-success claim.
