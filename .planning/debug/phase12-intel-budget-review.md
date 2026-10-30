---
status: local-correction-verified-live-closed
date: 2026-10-30
scope: Approved Intel-only local correction verified; no live allowance
---

# Intel producer outer-job budget review

Complete historical successful Intel jobs support a **37-minute Intel-only outer
deadline correction under DEC.47**: `ceil(2 × 1085 / 60) = 37`. The current19m
comment still cites an older567s pooled non-Windows maximum. This is enough to
recommend a local correction of historical reliability margin; it does **not**
establish that today's expanded677+6 producer completes successfully, explain the
slowdown, or authorize a run. No workflow/source change was made.

## Facts and coverage

ENGINEERING §4 defines twice the slowest successful job among the last40, rounded
up to minutes, with sample count and run ID. GSD-HANDOFF §3.1 requires complete
successful work, preserving failures. Neither cancelled attempt supplies a
completed whole-job timing sample.

The audit fetched100 newest `ci.yml` run metadata records, then72 latest-job lists
in chronological batches. They contain43 distinct successful Intel jobs,
15cancelled,13failed and one absent Intel job. The first40 successful distinct
run executions are also the latest40 by actual completion time within this
window. The28 remaining metadata records all have updates older than the selected
boundary; no claim is made to audit all261 repository CI runs or all historical
rerun attempts. One successful execution per run is counted, avoiding copied
successful peer receipts becoming extra samples on reruns.

All40 selected job receipts have successful current workflow, both exact-inventory
consumers, independent-base quality production, commit identity, all three uploads
and job completion; each whole job concludes `success`. Runs with unrelated
failures can contribute a successful Intel job without becoming accepted CI runs.
The job span uses API `started_at` through `completed_at`, excluding queue time.

Direct logs were read for the13 most recent selected jobs:12 were available,
and11 of those explicitly set `MUHTARI_TIMEOUT_SCALE: 2`. Those12 show all four
successful ordinary/release current/base suites and three completed uploads. The
older boundary run40753756094 predates scale2 and is not an exact scaled-lane
sample. Run41075600234's log returned HTTP503 twice; its successful complete job
receipt remains evidence, but no direct-log details are invented. Further history
would not change the maximum or the absence of an exact677-test successful job.

## Decisive complete samples

| Successful Intel job | Whole job | Current/base ordinary | Current/base release | Direct producer/upload evidence |
|---|---:|---|---|---|
| [40783431377 / 123150624076](https://github.com/szTheory/muhtari/actions/runs/40783431377/job/123150624076) | 1085s, slowest of40 | 662/662;525.59s/443.36s | 6/6 | Complete quality producer; all3 finalized uploads; job success |
| [40956113335 / 123736612675](https://github.com/szTheory/muhtari/actions/runs/40956113335/job/123736612675) | 877s | 673/673;373.07s/402.99s | 6/6 | Complete quality producer; all3 uploads; job success |
| [41086569269 / 124169251753](https://github.com/szTheory/muhtari/actions/runs/41086569269/job/124169251753) | 624s, newest success | 676/676;255.03s/293.44s | 6/6 | Complete quality producer; all3 uploads; job success |

The maximum's API job span is22:54:14–23:12:19UTC. Its log/step interval is
22:54:06–23:12:07UTC; minor platform timestamp skew changes neither completeness
nor the rounded37m calculation. The successful sample is not a cancelled job
whose tests happened to finish.

Maximum source identity: run head is2ba227ae27698a55c41713a9db289a5776af44c1;
actual checkout/log identity is PR mergef302605c2b4f4339bd654ba80952662cf5a2401b,
with genuine based2ec6bb692514062fe905bb0259460716087008a. GitHub's merge commit
treef148edf78d23832fc8a64f58f927603eb136532e exactly equals the local2ba227ae tree.
This binds the locally compared source to the actual built merge, rather than
assuming a PR run executes the head commit. The maximum and current source have
byte-identical native build-job blocks and `tests/cmake/quality_evidence.cmake`.
Their native preset behavior is unchanged; the preset diff concerns only a library
comment. Their inventories differ by15 added ordinary cases,662→677.

The maximum, newest successful676-test sample and cancelled attempt2 logs all
explicitly report macOS15.7.9/build24G830, image`macos-15`
revision`20260824.0482.1`, and AppleClang17.0.0.17000013 for both builds. Matching
labels alone were not used as image/toolchain evidence. Matching image/toolchain
still does not establish matching CPU capacity, runner load or performance.

## Comparability and remaining uncertainty

These are comparable **whole-job historical reliability samples** because the
Intel lane executes the same complete current/base workflow shape, same exact
inventory/three-package/quality/authentication/upload contracts and unchanged
producer script. The maximum is already a scale2 execution on the exact observed
image/compiler. The newer676-case success bridges most of the expanded workload.
The proposed37m restores the existing rule's margin for that job; it does not scale
every platform or weaken a test.

They are **not exact current-workload calibration**: the maximum ran662+6 and the
newest success676+6, while the current source runs677+6 on each side. No successful
full677+6 producer/job sample was found. Mark the correction provisional for that
expanded workload and obtain a later explicitly authorized exact-source complete
producer verdict before accepting it. No assertion that37m guarantees completion
or that the new tests caused the slowdown follows from these observations.

[Attempt2](https://github.com/szTheory/muhtari/actions/runs/41099366366/attempts/2)
passes677+6 current/base, with ordinary533.41s/545.41s, but cancels inside the
quality step after its final inventory output; every upload skips. This confirms
the outer limit interrupted broader production, not that the whole producer
succeeded. Its cancelled1212s API span is right-censored and excluded from every
bound calculation. Attempt1's all-step-success/job-cancelled discrepancy stays
unproved and excluded as well. Its prior artifacts do not turn attempt2 into fresh
successful production. The required audit remains failed; accepted0/4 and plans
12-02 through12-04 blocked; no new child/image exists.

## Decision and smallest next scope

| Choice | Consequence |
|---|---|
| Do nothing | Preserve the required stop, but retain a historical-margin defect and no basis for expecting another identical retry to fit. Appropriate until new local authority arrives. |
| Recommended: Intel-only37m outer deadline and provenance comment | Restore the existing DEC.47 historical margin while preserving every source/test/quality/join/pin/permission contract. At most18 more Intel runner-minutes per future execution; no runner cost now. Exact expanded-workload success remains unproved. |
| Strongest alternative: profile or restructure current/base jobs | Could reduce the critical path, but changes scheduling/producer ownership/artifact trust and needs a larger investigation and validation scope. Existing publication/DAG profiling is a seed, not necessary for this correction. |

The next authorization should be **local implementation only**: change the one
native timeout expression so Intel is37, Windows stays68 and other native lanes
stay19; retain a comment citing1085s/run40783431377, historical40-job coverage and
the expanded-workload provisional limitation. Use existing timeout-comment,
workflow/policy consumers and a fresh full local workflow with separate focused
JUnit evidence; assert all six lane budgets and all source/test/join contracts are
unchanged, and provide a reviewable diff. No new framework or permanent tests-of-
tests harness is needed for a scalar configuration correction.

**Stopping rule:** stop after that local change, existing affected consumers/full
workflow and one bounded review, or at the first unresolved local contract failure.
Do not push, dispatch, rerun, cancel or launch content. A later live attempt needs
separate concrete source, dependent-job closure, runner budget and content scope;
37m by itself authorizes none. Relative to the former19m Intel limit, any future
same-closure live request must explicitly account for the additional18-minute
Intel ceiling. This report proposes no new live total or spending window.

Exact proposed workflow diff shape, **not applied**:

```diff
-    timeout-minutes: ${{ startsWith(matrix.os, 'windows') && 68 || 19 }}  # provisional 2 x Windows2021s/other567s, n=12/23, run 40725919037
+    # Intel: 2 x 1085s, 40 successful historical jobs, run 40783431377;
+    # provisional for expanded677+6 work. Windows/other provenance below unchanged.
+    timeout-minutes: ${{ startsWith(matrix.os, 'windows') && 68 || matrix.runner == 'macos-15-intel' && 37 || 19 }}  # 2 x Windows2021s/other567s, n=12/23, run 40725919037; Intel run 40783431377
```

Offline recipe for that future authorized implementation: first inspect actual
configured test names with `ctest --preset hygiene -N` and `ctest --preset ci -N`;
then run the existing timeout provenance consumer directly:
`cmake -DDIR=.github/workflows -P tests/cmake/timeouts.cmake`. Run the existing
timeout-comment pass/fault controls with
`ctest --preset hygiene -R '^hygiene[.]timeout_comments([.]self_test)?$' --no-tests=error --output-on-failure --output-junit /tmp/muhtari-intel-timeout-focused.xml`.
Use `ctest --preset ci -R '^policy[.](ci_run|retroarch[.]preflight_gate)[.]controls$|^policy[.]retroarch[.]preflight_gate$' --no-tests=error --output-on-failure --output-junit /tmp/muhtari-intel-workflow-focused.xml`
only after confirming those actual names; do not invent a renamed check. Assert
the expression's six lane results68/68/37/19/19/19 in a scratch evaluator and
compare the candidate native job block against the original after removing only
the deadline/comment lines. Then `cmake --workflow --preset ci`, both ordinary/
release exact JUnit consumers, existing docs budgets, `git diff --check` and one
bounded diff review. No workflow run is needed to make that local change
reviewable. These are a future recipe, not claimed executed verification.

Cheapest falsifiable countercheck: the existing complete maximum log must show
all four suites, final producer return and three uploads with job success; it does.
If the native build/quality source comparison or exact image/compiler binding had
failed, the historical correction would need revision; both comparisons pass.
A later complete exact677+6 producer exceeding1110s would show37m lacks twice-work
margin and require new measurement, not automatic retry or further deadline growth.

## Command / exit / run evidence

Raw API responses and logs stay only in scratch
`/tmp/muhtari-intel-budget.VY2XZc`; no log is committed.

| Command | Exit | Run/source |
|---|---:|---|
| `gh api 'repos/szTheory/muhtari/actions/workflows/ci.yml/runs?per_page=100'` | 0 | CI history metadata,100 records |
| `python3 /tmp/muhtari-intel-budget.VY2XZc/history.py` | 0 | Calls `gh api 'repos/szTheory/muhtari/actions/runs/RUN/jobs?per_page=100&filter=latest'` for72 runs; every call0;40 selected successful jobs below |
| `gh api repos/szTheory/muhtari/actions/jobs/123150624076/logs --allow-escape-sequences` | 0 | [Complete maximum](https://github.com/szTheory/muhtari/actions/runs/40783431377/job/123150624076) |
| Same job-log API for124169251753 and123736612675 | 0/0 | Newest success and recent673-case sample linked above |
| Initial same three log requests without `--allow-escape-sequences` | 1/1/1 | Client refuses terminal escape sequences; corrected reads0, not hosted test failures |
| `python3 /tmp/muhtari-intel-budget.VY2XZc/comparability.py` twice | 1/1 | One job-log request124130889743 returned HTTP503 both times; other12 direct logs0; incomplete-log assertion honestly rejects the all13 claim |
| `gh api repos/szTheory/muhtari/git/commits/f302605c2b4f4339bd654ba80952662cf5a2401b` | 0 | Maximum actual merge source/tree/parents |
| `gh api 'repos/szTheory/muhtari/actions/runs/40783431377/artifacts?per_page=100'` | 0 |23 artifacts, all3 Intel names/IDs match log and are nonexpired |
| `python3 /tmp/muhtari-intel-budget.VY2XZc/audit.py` | 0 | N/A-local;40 full successful receipts, sample ordering/max/arithmetic, exact merge tree, native block/producer equality and available direct-log completeness assertions |
| `curl -fsS https://www.githubstatus.com/api/v2/summary.json -o /tmp/muhtari-intel-budget.VY2XZc/health.json` | 0 | Primary API: All Systems Operational, updated2026-10-30T20:37:37Z; no causal attribution |

## Successful sample ledger

Every row is a distinct successful Intel job with all required producer/upload
steps successful. Attempt is the job's reported attempt; seconds exclude queue.
Rows are ordered by run creation; sorting by completion selects the same40.

| Run/job link | Job | Attempt | Seconds |
|---|---:|---:|---:|
| [41086569269](https://github.com/szTheory/muhtari/actions/runs/41086569269/job/124169251753) |124169251753|1|624|
| [41075600234](https://github.com/szTheory/muhtari/actions/runs/41075600234/job/124130889743) |124130889743|1|650|
| [41062201311](https://github.com/szTheory/muhtari/actions/runs/41062201311/job/124084546980) |124084546980|1|825|
| [40956113335](https://github.com/szTheory/muhtari/actions/runs/40956113335/job/123736612675) |123736612675|1|877|
| [40946415397](https://github.com/szTheory/muhtari/actions/runs/40946415397/job/123735899249) |123735899249|3|685|
| [40946368330](https://github.com/szTheory/muhtari/actions/runs/40946368330/job/123702751369) |123702751369|1|616|
| [40940608451](https://github.com/szTheory/muhtari/actions/runs/40940608451/job/123683049910) |123683049910|1|619|
| [40854285272](https://github.com/szTheory/muhtari/actions/runs/40854285272/job/123381298474) |123381298474|1|531|
| [40783431377](https://github.com/szTheory/muhtari/actions/runs/40783431377/job/123150624076) |123150624076|1|1085|
| [40777366563](https://github.com/szTheory/muhtari/actions/runs/40777366563/job/123130069508) |123130069508|1|644|
| [40777253646](https://github.com/szTheory/muhtari/actions/runs/40777253646/job/123129686182) |123129686182|1|533|
| [40768779586](https://github.com/szTheory/muhtari/actions/runs/40768779586/job/123111878293) |123111878293|2|725|
| [40753756094](https://github.com/szTheory/muhtari/actions/runs/40753756094/job/123066056011) |123066056011|2|580|
| [40748084806](https://github.com/szTheory/muhtari/actions/runs/40748084806/job/123027449906) |123027449906|1|489|
| [40725919037](https://github.com/szTheory/muhtari/actions/runs/40725919037/job/122969806457) |122969806457|2|567|
| [40708749399](https://github.com/szTheory/muhtari/actions/runs/40708749399/job/122887986600) |122887986600|1|538|
| [40702203420](https://github.com/szTheory/muhtari/actions/runs/40702203420/job/122865216523) |122865216523|1|479|
| [40419374588](https://github.com/szTheory/muhtari/actions/runs/40419374588/job/121910114554) |121910114554|1|506|
| [40414104123](https://github.com/szTheory/muhtari/actions/runs/40414104123/job/121892328675) |121892328675|1|472|
| [40249027703](https://github.com/szTheory/muhtari/actions/runs/40249027703/job/121336676442) |121336676442|1|374|
| [40246033915](https://github.com/szTheory/muhtari/actions/runs/40246033915/job/121335046179) |121335046179|2|317|
| [40245012617](https://github.com/szTheory/muhtari/actions/runs/40245012617/job/121324882883) |121324882883|1|305|
| [40242459269](https://github.com/szTheory/muhtari/actions/runs/40242459269/job/121317652961) |121317652961|1|347|
| [40238824962](https://github.com/szTheory/muhtari/actions/runs/40238824962/job/121313489313) |121313489313|2|318|
| [40233948646](https://github.com/szTheory/muhtari/actions/runs/40233948646/job/121291421180) |121291421180|1|264|
| [40229267518](https://github.com/szTheory/muhtari/actions/runs/40229267518/job/121277313947) |121277313947|1|313|
| [40203183179](https://github.com/szTheory/muhtari/actions/runs/40203183179/job/121198519795) |121198519795|1|343|
| [40201179371](https://github.com/szTheory/muhtari/actions/runs/40201179371/job/121192522147) |121192522147|1|259|
| [40201154961](https://github.com/szTheory/muhtari/actions/runs/40201154961/job/121192438802) |121192438802|1|208|
| [40198835865](https://github.com/szTheory/muhtari/actions/runs/40198835865/job/121185576457) |121185576457|1|354|
| [40157749294](https://github.com/szTheory/muhtari/actions/runs/40157749294/job/121062682317) |121062682317|1|274|
| [40153312357](https://github.com/szTheory/muhtari/actions/runs/40153312357/job/121049247172) |121049247172|1|296|
| [40139744520](https://github.com/szTheory/muhtari/actions/runs/40139744520/job/121008798030) |121008798030|1|236|
| [40135571745](https://github.com/szTheory/muhtari/actions/runs/40135571745/job/121002299364) |121002299364|2|231|
| [40132929297](https://github.com/szTheory/muhtari/actions/runs/40132929297/job/120988352148) |120988352148|1|245|
| [40114033103](https://github.com/szTheory/muhtari/actions/runs/40114033103/job/120931543775) |120931543775|1|223|
| [40112030202](https://github.com/szTheory/muhtari/actions/runs/40112030202/job/120925591372) |120925591372|1|487|
| [40112001036](https://github.com/szTheory/muhtari/actions/runs/40112001036/job/120925517069) |120925517069|1|290|
| [40109819309](https://github.com/szTheory/muhtari/actions/runs/40109819309/job/120919208481) |120919208481|1|281|
| [40104749040](https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120910042039) |120910042039|2|295|

## Proposed canonical bookkeeping for the root owner

Append, without replacing existing halt/coverage/failure records:

> Read-only Intel budget audit inspected72 latest-job records within100 newest
> CI runs and selected40 distinct successful complete Intel jobs. Maximum1085s
> is run40783431377/job123150624076; DEC.47 gives37m. Direct maximum/newest logs
> prove662+6 and676+6 current/base, complete quality production and all uploads;
> exact image/compiler and maximum actual merge-tree identity are checked.
> Historical margin correction is recommended locally only and provisional for
> expanded677+6; no successful full current-workload job is claimed. Cancelled
> attempts remain failed/censored, attempt1 discrepancy and slowdown cause
> unproved. No workflow/source/live change occurred. Accepted0/4 and downstream
> blocks remain. ZERO push/dispatch/rerun/cancel/content allowance remains.
> Next boundary: authorize local Intel-only37m timeout/provenance correction plus
> existing offline verification/review; live scope and cost require a separate
> concrete decision. See phase12-intel-budget-review.md for command/exit/run evidence.

Do not alter STATE/ROADMAP accepted counts, requirements, canonical coverage or
the spent targeted-retry ledger. Context may be cleared. The next separate command
is `$gsd-debug continue phase12-packaged-input-tracer` after local scope is decided;
the command itself grants no live allowance. Root presents the scope decision;
this investigator is stopped at that authority boundary.

## Approved local correction receipt

The owner answered **"yes i approve"** to the local-only correction, existing
offline checks/full workflow and one bounded review on2026-10-30. That supersedes
the preceding local deadline-change stop only. The historical report and proposed
diff above retain their original read-only scope; this receipt records the later
implementation. No live budget was renewed.

The native build expression now gives Intel37m, both Windows lanes68m and the
other three lanes19m. Its comment cites1085s/n=40/run40783431377 and preserves the
expanded-workload provisional limitation. All other workflow bytes match the
pre-change snapshot after reversing only this expression and its two new comment
lines. No test, per-test timeout/scale, matrix, producer, join, pin or permission
changed. No permanent new test harness was added for this scalar correction.

Verification (all commands exit0, run URL **N/A-local**):

- `python3 /tmp/muhtari-intel-local.748pDp/verify.py`: evaluates the actual
  expression for all six actual matrix rows; rejects the old expression,
  wrong-runner and changed-Windows-budget variants; compares the full workflow
  snapshot and original three planning-edit hashes; preserves canonical prefix
  and accepted0/4.
- `cmake -DDIR=.github/workflows -P tests/cmake/timeouts.cmake` and
  `ctest --preset hygiene -R '^hygiene[.]timeout_comments([.]self_test)?$'
  --no-tests=error --output-on-failure --output-junit
  /tmp/muhtari-intel-local.748pDp/timeout-focused.xml`: existing provenance
  consumer and its representative pass/fault controls.
- `ctest --preset ci -R '^policy[.](ci_run[.]controls|retroarch[.]preflight_gate)$'
  --no-tests=error --output-on-failure --output-junit
  /tmp/muhtari-intel-local.748pDp/workflow-focused.xml`: existing outcome and
  preflight controls. Focused reports remain separate from full inventories.
- `actionlint -shellcheck= -ignore 'label ".*" is unknown'
  .github/workflows/ci.yml`: schema/expression validation with only inherited
  unsupported runner-label warnings and inherited shellcheck diagnostics excluded;
  no workflow control was changed to satisfy the validator. Official GitHub
  [expression documentation](https://docs.github.com/en/actions/reference/workflows-and-actions/expressions)
  was fetched before editing.
- `python3 /tmp/muhtari-intel-local.748pDp/workflow.py`: executes
  `cmake --workflow --preset ci` in the foreground with the existing four-worker
  FIFO jobserver convention. Complete677 ordinary tests, packaging and6 release
  tests pass. `tests/cmake/check_junit.cmake` with `JUNIT=build/ci/junit.xml`,
  `INVENTORY=tests/inventory/ci.txt`, `EXCLUDE=release`, then
  `JUNIT=build/ci/junit-release.xml`, `LABELS=release`, each exit0; every required
  ordinary/release test ran exactly once.
- `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake`,
  `sh scripts/hygiene.sh --tree` and `git diff --check`: existing local handoff
  checks. Raw logs and scratch evaluators remain outside the repository.

One bounded independent review returns **PASS**, with no substantive findings:
expression precedence/truthy results, six-lane selection, complete historical
calibration, provisional scope and unchanged commands/contracts are confirmed.
No second review or optional audit was added.

This closes the authorized local correction only. Exact-current-workload hosted
Intel producer success, attempt1's discrepancy, slowdown cause and actual keyboard
delivery remain unproved; every failed receipt and accepted0/4 remain. No push,
dispatch, rerun, cancel or content launch occurred. Any future live proposal must
bind a concrete corrected source/parent, dependent-job closure, finite spending
window and content limit, accounting for18 additional Intel runner-minutes. This
receipt proposes no live scope or total. Context may be cleared; next separate
command is `$gsd-debug continue phase12-packaged-input-tracer`, for agent-owned
read-only scope preparation and health/receipt checks. It grants no live authority.
