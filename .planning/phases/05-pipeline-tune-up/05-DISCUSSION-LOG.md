# Phase 5: Pipeline tune-up - Discussion Log

> **Audit trail only.** Research, planning and execution consume `05-CONTEXT.md`, not this log.

**Date:** 2026-10-21
**Areas considered:** weekly/monthly reporting; CI reliability and cost; quality/performance/portability;
release safety, freshness and proportionality.

## Discussion contract

The owner requested all relevant angles, breadth and depth until diminishing returns, primary-source research,
an adversarial pass, coherent recommendations and automatic adoption of those recommendations. They specifically
prefer another copy and paste over another dependency and a smaller, flatter dependency tree.

The agent selected all four areas and synthesized the decisions inline. No multiple-choice questions were asked,
no unreturned question was treated as consent, and no specific technical option is attributed to an owner answer.
The explicit delegation replaced the default four-question-per-area interview. No automatic GSD flags, chain
settings or next workflow were used. Context capture was authorized by the invoked discuss skill.

## Weekly triage and monthly evidence

| Point | Selected recommendation | Strongest alternative | Tradeoff, pattern or footgun | Reconsider when |
|---|---|---|---|---|
| D-01: delivery | One rolling digest issue and report artifact; retain latest monthly section | One new issue each week | Rolling delivery avoids backlog/notifications; overwriting the monthly section each week would lose useful context | A real archival/discovery need exceeds the artifact/history solution |
| D-02: content | Small actionable triage queue using existing thresholds and evidence | Broad automated triage/bot actions | Reporting is useful without trusting a bot to close issues, upgrade dependencies or change settings | Repeated observed human toil justifies a separately scoped capability |
| D-03: statistical meaning | Explicit windows, attempts, sample sizes, provenance and unknown fields | Estimate missing values from run-level timestamps | Honest unknowns are less polished but prevent false queue, flake and utilization conclusions | The API or a measured producer provides the missing quantity |
| D-04: operation | Existing Actions schedule, gh/jq/sh, trusted publisher, unprivileged preview | External scheduler/reporting framework | Native cron can slip; manual recovery is sufficient for weekly reporting. Untrusted titles must remain data | Native schedule omissions actually defeat the reporting goal and justify changed scope |

Maintainer/product lens: one place to inspect, obvious next actions, no report spam. Security lens: minimal write
permissions and no App secret in PR code. Reliability lens: pagination, rerun attempts and failed report writes
cannot disappear behind a green summary. Data lens: a failed run passing on retry is a candidate requiring diagnosis.

## CI reliability and cost

| Point | Selected recommendation | Strongest alternative | Tradeoff, pattern or footgun | Reconsider when |
|---|---|---|---|---|
| D-05: output race | Independent writer paths and existing exact negative control | Targeted resource lock | Isolation removes contention and retains parallelism; whole-suite serialization or retries hide the cause | A resource genuinely cannot be isolated |
| D-06: bounds | Twice the maximum comparable successful sample, explicit provisional bounds and timeout lower bounds | Uniformly larger limits | Measured limits retain hang detection; old short workloads and timed-out observations cannot define current successful maxima | New workload/tool/runner evidence invalidates the sample |
| D-07: lane split | Six native legs required, PR BE compile, full nightly BE plus same-commit equality | Keep full BE on each PR | BE runtime regressions may surface after merge; the existing policy explicitly accepts that trade above five minutes. Nightly/release evidence must remain real | A useful independent BE runtime slice fits the five-minute cost rule |
| D-08: optimization | Remove duplicate work first, cache only after the existing measured threshold | Add ccache or broader download caching now | More caches mean invalidation and fetch-path blind spots; tests with no historical failures still prevent rare faults | Compile share reaches 40% of a critical-path leg and cold/cached parity is shown |

Observed [full run 39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435): `be32` 681 s,
slowest native build 186 s, ASan 144 s. This supports invoking the pre-existing BE cost condition; it does not
establish p95s, a complete 40-run audit or a final timeout. The prior compatibility race has local reproduction
history and shared output construction is still visible in `tests/policy/tests.cmake`.

QA/emulator lens: preserve semantic fixtures, exact inventory and native equality. DevOps lens: distinguish
critical-path execution from dependency wait and GitHub status lag. Architecture lens: test-owned outputs are
simpler than a synchronization layer. Cost lens: reduce actual work, not merely move minutes off the displayed PR.

## Quality, performance and portability

| Point | Selected recommendation | Strongest alternative | Tradeoff, pattern or footgun | Reconsider when |
|---|---|---|---|---|
| D-09: policy | One quality.json/check using existing producers and semantic judges | Independent metric scoreboards | Consolidation reduces drift; a new generic framework could cost more code than it removes | A metric's independently measured churn warrants a data shard under the same check |
| D-10: exceptions | Base-relative bounds and explicit matching dated expiring reasons | Automatic rebaseline with each PR | Review remains meaningful; changing the threshold and result together otherwise launders a regression | A deliberate scoped policy decision changes the published quality contract |
| D-11: evidence | Gate only current measured required outputs; show missing/advisory dimensions | Populate every aspirational preparation metric immediately | Smaller scope is truthful. An absent coverage producer cannot mean 100%, and live backlog/history is not reproducible PR evidence | A real producer, identity and freshness contract exist |
| D-12: perf | Fresh deterministic controls then arm +1%; wall clock advisory | Arm now from old controls, or wall-clock gate | Cachegrind is repeatable but the test-card workload covers little game emulation; representative diagnostics need their own controls | A dedicated quiet benchmark host or controlled new workload changes available evidence |
| D-13: promotion | Honor ten green executed-main-run conditions per lane | Make every advisory lane required now | Insufficient history stays visible; skipped or rerun jobs do not become independent qualifying runs | Fresh lane-level evidence meets the existing promotion rule |

Performance lens: deterministic work and bounded execution matter more than shared-host wall time. Test-validity
lens: negative controls must reject the intended fault, not any process failure. Security lens: a PR cannot
supply its own historical green baseline. Maintainer lens: one policy store should not duplicate each subsystem's
judge. Player/host lens: label test-card-only performance and advisory portability honestly.

Direct main-push CI history returned ten records at this check, one failed and one incomplete. This does not
prove WASM/library/property eligibility. No new CI controls or intentionally failing nightly dispatch were run
inside the discussion step; those are execution/ship evidence to obtain, not fabricated completed checks.

## Release safety, freshness and proportionality

| Point | Selected recommendation | Strongest alternative | Tradeoff, pattern or footgun | Reconsider when |
|---|---|---|---|---|
| D-14: priority | CI/CD is the provisional weakest dimension; establish evidence-linked six-dimension baseline | Optimize the emulator core first | Reproduced infrastructure failures and 681-second BE cost are concrete; a core rewrite lacks a reproduced phase need | New measured evidence identifies another scoped weakest dimension |
| D-15: rehearsals | Measure duplication; only reuse an equivalent successful full-input rehearsal when worthwhile | Always rehearse a release-touching PR | Safe reuse is harder than comparing four paths. Current source changes alter archives even if publishing scripts do not change | Proving equivalence needs more enforcement than the saved cost; retain conservative behavior |
| D-16: freshness/safety | Read docs against release/code, test install/settings, minimally correct reproduced release/bot claims | Add a broad bot/admin state machine | Narrow truthful behavior is easier to maintain. API failure must not masquerade as release deletion; signer checks must precede publication | Evidence shows a narrow claim leaves a recurring fault unprevented |
| D-17: budget | Simplify honestly to the existing tenth-of-src limit | Relax the ratio or relocate/minify checks | Safety judges and readable code remain; generated data must not conceal the numerator or denominator | Only an explicit future owner policy change could relax the rule; none is assumed here |
| D-18: stop rule | Resolve scoped outcome choices and stop when more analysis changes none | Exhaust every role/ecosystem indefinitely | Relevant lenses expose failure modes; unrelated UI/graphics/language comparisons add noise | A new reproduced fact changes a decision or its verification |

An evidence-linked scoring rubric for execution: 1 = reproduced reliability failures plus missing promised
controls; 2 = significant current evidence gap; 3 = shipped with known drift; 4 = demonstrated usable contract;
5 = demonstrated contract plus sustained measured trend. Provisional ratings, not benchmark outputs:

| Dimension | Provisional rating | Evidence/reason |
|---|---|---|
| CI/CD | 1 | Reproduced timeout/race, over-budget checks, missing recurring report/ratchet, costly required BE |
| Performance | 2 | Bounded integer implementation and nightly trends, but unarmed test-card-only comparison |
| Docs/README | 3 | Concise released docs exist; hash-format/lane/timeout descriptions need reconciliation |
| Ease of use | 4 | Published one-line installation and actual RetroArch validation, release run 39533130210 |
| Architecture | 4 | Three deliverables, bounded instance state and cross-platform deterministic tests |
| Upgrade path/semver | 4 | API/ABI/behavior separation and tested attested automated releases |

Formal research should confirm these with current evidence before implementation; a numeric rating is judgment,
not an invented measurement. Repository settings were actually checked: exit 0, `no drift on szTheory/muhtari`.
Historical narrow gate count reproduced 1,367/8,577 (15.94%), or 20.93% without generated cartdb data. This is a
lower-bound inventory snapshot; no claim of budget compliance or completed simplification is made.

## Adversarial pass and reconciled recommendations

| Failure scenario | Required response |
|---|---|
| GitHub API returns 403/5xx halfway through pagination | Fail required collection; do not publish a misleading complete report |
| A rerun archive contains only retried jobs | Collect attempt-specific evidence; do not label missing prior jobs skipped or green |
| Monthly report is overwritten on a later Monday | Preserve the last monthly section and its window |
| Two dispatches both see no digest issue | Serialize the publisher; one rolling issue, failed writes visible |
| Title/body contains shell syntax | Structured JSON, quoted data and body files; never evaluate it |
| A negative test overwrites a positive test's output | Isolate outputs; keep targeted parallel reproduction |
| A timed-out slow job is excluded and the maximum becomes artificially small | Preserve timeout lower bounds and mark bounds provisional until successful current-workload evidence exists |
| BE moves nightly and silently vanishes from equality | Exact same-commit native/BE pair, inventory and nightly reporter dependencies |
| A PR loosens a threshold while reporting a new measurement | Compare trusted base plus matching dated expiring reason; still check measured value |
| Required metric producer fails to upload | Missing evidence fails; no absent-as-zero success |
| Old perf controls passed but the fixture/compiler changed | Obtain current controls before arming that workload |
| A release-touching PR later changes core source only | Rehearse unless complete effective inputs are equivalent; four-path comparison is insufficient |
| A failed deletion lookup is read as 404 | Distinguish transport/API errors before claiming cleanup |
| Gate code is moved into YAML or a different directory | Keep it in the numerator; publish broad inventory and denominator sensitivity |

No unresolved product preference remains after the delegated synthesis. Remaining uncertainty is empirical:
current workload distributions, safe rehearsal savings, promotion history and feasible code-budget reduction.
Those are explicit researcher/planner tasks, not owner homework and not permission to weaken the outcomes.

## Sources and verification limits

Primary references checked on 2026-10-21, with links and application in `05-CONTEXT.md`:
GitHub workflow events, workflow-job REST API, run-log/partial-rerun semantics and script injection; CMake
`RESOURCE_LOCK` and `COST`. Local engineering/decisions/retrospective plus selected prior contexts supplied
settled contracts and sibling precedents. No external emulator source was opened; no new dependency is selected.

Commands/evidence inspected:

- `gh api repos/szTheory/muhtari/actions/runs/39614799435/jobs --paginate` — exit 0,
  [run](https://github.com/szTheory/muhtari/actions/runs/39614799435).
- `gh run list -R szTheory/muhtari --workflow ci.yml --limit 40 --json ...` — exit 0; latest forty CI records.
- Direct main-push CI REST listing — exit 0; insufficient to certify any individual lane's promotion.
- `scripts/apply-repo-settings.sh --check szTheory/muhtari` — exit 0, no drift.
- Digest and nightly open-issue reads — exit 0, no matching open issues. No outbound issue mutation performed.
- One-off Python count of historical gate files and src — exit 0; lower-bound baseline above.
- One attempted `gh run view ... --log` could not write its default cache under the filesystem sandbox (exit 1).
  No perf-control result was inferred from that failed read; controls remain unverified here.

A checkpoint was written as each completed area's decision set was captured. It is removed once the canonical
context and this audit log are verified. No tests, workflows, source, docs, seeds or todo statuses were modified
by the discussion. Artifact checks at commit are recorded in the session's tool evidence, not committed logs.

## Agent's Discretion

Schema details, API batching, report presentation, implementation boundaries, cost values and simplification
choices stay with formal research/planning. Do not interpret illustrative mechanisms as new public contracts.

## Deferred Ideas

The two phase-2 todo files remain reviewed but not adopted wholesale; emulator/parser/test seeds need reproduction
and a relevant future scope. New board/controller/save/PAL functionality stays in later phases. No dashboard,
service, speculative dependency, bot state machine, extra UI or additional GSD step is introduced.
