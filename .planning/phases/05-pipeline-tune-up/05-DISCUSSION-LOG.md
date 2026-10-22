# Phase 5: Pipeline tune-up - Discussion Log

> **Audit trail only.** Research, planning and execution consume `05-CONTEXT.md`, not this log.

**Date:** 2026-10-21
**Areas considered:** weekly/monthly reporting; CI reliability and cost; quality/performance/portability;
release safety, freshness and proportionality.

**Revision:** the original discussion below is preserved as history. The final section records the later
owner-authorized budget/acceptance/planning revision; current decisions are in CONTEXT D-10, D-17, D-19–20.

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

## Reopened budget, acceptance and planning — 2026-10-21

The owner selected **Update it** from Update/View/Skip. The agent then offered three areas: budget rule
(retain hard 10%, bounded exception or measured replacement), Phase 5 acceptance (eliminate all overage or
ship measured improvement), and planning prerequisite (full tested construction or allocated measured task).
The owner requested all relevant angles, fan-out research, primary sources, an adversarial pass, coherent
recommendations and automatic adoption: “let's do what u rec”. This replaces further interview questions;
no individual technical option is attributed to a specific multiple-choice answer.

Three read-only typed advisory roles reviewed (1) budget/metrics, (2) acceptance/planning and (3) adversarial
safety/gaming. A targeted synthesis challenge checked allowance renewal, savings accounting, expiry and
governance overhead. Relevant lenses were maintainers, security/legal/privacy, C architecture, QA/test validity,
CI/SRE, delivery and release consumers. UI, graphics engines and unrelated language ecosystems add no decision
to this infrastructure scope. No further fan-out was useful after the same policy and controls converged.

| Decision | Alternatives considered | Adopted recommendation | Tradeoff and structural response |
|---|---|---|---|
| Budget rule | Hard 10%; finite legacy-debt exception; absolute ratchet; reporting only | Audited absolute line ratchet, ratios advisory | Needs initial inventory and explicit allocations; avoids denominator gaming and an unsupported numerical promise |
| Scope growth | No growth ever; baseline plus automatic headroom; fixed fault-linked allowance | Exact baseline B, executed savings S, fixed scoped N including controls/glue/reserve; C = B - S + N | Required capabilities may increase total, but no extra inherited-rewrite allowance, replenishment or candidate-baseline laundering |
| Phase acceptance | Reach 857; waive until a date; ship behavior within allocated absolute ceiling | Retained-fault measured simplification, all five TUNE outcomes, actual within C, final tightening | Old ratio becomes diagnostic rather than a hidden failed requirement or indefinitely renewed debt |
| Planning prerequisite | Full replacement prototype; unconstrained executor exploration; allocated initial slice | Boundary/fault map and ranges in planning; first bounded task freezes B, measures S and fixes N/C before additions | Estimates are not evidence of compliance; affected expansion stops on unresolved count, control loss or overrun |
| Temporary versus permanent growth | Repeated 60-day waivers; silently raise policy; explicit new scope | D-10 temporary exceptions for unchanged scope; prior scope adoption/allocation for permanent gate machinery | Required digest/quality code persists honestly; other quality bounds cannot be loosened through the structural allowance |
| Governance machinery | New classifier, allowance interpreter or taxonomy; one existing metric/table | Existing quality metric plus file/block inventory and short allocation table | Review judges scope/fault equivalence; CI checks actual numeric bound/expiry, not prose |

The strict-ratio option remains logically possible: the tested experiment is a failed construction, not proof
that every implementation is impossible. Retaining it would retain an unestablished shipment condition. A
temporary 10% exception would recreate that blocker at expiry without a measured convergence path. Reporting
only was considered too weak against gradual machinery growth. These are project judgments; no external
source prescribes a suitable LOC ceiling for this repository.

### Adversarial findings incorporated

- Partial inventory 2,764, failed incomplete candidate 2,262 and overinclusive sensitivity 8,607 cannot initialize
  the new cap. Use an exact audited commit and counting boundary; no invented 3,000/3,500-line ceiling.
- Count dedicated negative controls and invocation/configure/workflow enforcement wherever they live; ordinary
  product behavior tests remain distinct. Any operational-only exclusion needs an exact documented boundary.
- S is an executed net saving including changed control cost, not a forecast; no deletion can pay twice.
- N covers only named required additions and a fixed stated reserve. Unused reserve retires at final tightening;
  redistribution is visible and does not increase total. No unbounded integration exemption.
- A temporary overrun needs a credible removal/consolidation exit and expiry checked on every run. It cannot
  renew itself, turn into baseline or fund safety/control deletion.
- Preserve named safety outcomes, not redundant script shapes. No compliance through line golf, relocation,
  source padding, dependencies hiding checks or deleting unique evidence.
- Do not replace the line-count problem with a policy framework or repeated full implementation research.

### Sources and policy synchronization

[Google SRE](https://sre.google/workbook/eliminating-toil/) considers automation development/maintenance cost;
[NIST SSDF](https://doi.org/10.6028/NIST.SP.800-218) supports risk-based security requirements and reviewed
exceptions. [GitHub's required-status documentation](https://docs.github.com/en/pull-requests/how-tos/merge-and-close-pull-requests/troubleshooting-required-status-checks)
explains skipped-job success semantics, reinforcing exact executed evidence. These inform tradeoffs and safety
requirements; the absolute ratchet is our project-specific inference.

The revised contract is synchronized in AGENTS rule 7, TUNE-03, Phase 5 roadmap criterion 3, ENGINEERING §11,
DEC.10 and CONTEXT D-10/D-17/D-19–20. RESEARCH retains its experiment and copied old constraints as explicit
history beneath a supersession banner; only affected guidance needs refresh. No implementation, new dependency,
hosted write, CI dispatch, cap measurement, phase verification or second GSD step occurred during discussion.

The next command is `$gsd-plan-phase 5 --research`: refresh the changed contract, reuse valid prior findings,
then plan the bounded initial measurement and required improvements. No full replacement prototype is required.


## Measured migration halt: delegated scope resolution — 2026-10-22

The owner invoked discussion again, then requested all relevant perspectives, primary sources where useful,
adversarial review and automatic adoption of a coherent recommendation. This explicitly selected all three
remaining areas and delegated the choices below; no multiple-choice interview response is invented. Earlier
D-01–20 remain unless explicitly narrowed by current CONTEXT D-09/D-10/D-21–23.

Three read-only advisory reviews covered scope/architecture/maintainer cost, security/test validity, and
construction/CI economics. The security recommendation initially favored preserving full centralization if a
fixed budget for the complete remaining migration could be adopted; it also identified the incomplete mixed
caller cutover and legacy exception-lifecycle limitation. The other two reviews recommended explicit deferral
under the fixed allocation. The synthesis adopts deferral because the complete cost is unmeasured and the
38-line deficit only prices unfinished Task 1. No arbitrary cap or automatic extra gap round is adopted.

| Area | Alternatives considered | Adopted recommendation | Tradeoff and control |
|---|---|---|---|
| Domain ownership | Retain separate stores; complete central migration; leave partially migrated code | Explicitly defer domain centralization and retain one authority per semantic bound | Narrows D-09/TUNE-03; no claim of unified central reasons or complete migration |
| Allocation | Keep fixed allocation; add 38; estimate larger complete migration; seek more generalized savings | Preserve quality920 and the complete fixed allocation; stop new migration attempts | 38 funds only the current deficit, not completion; no proof of impossibility, forecast savings or BE12 transfer |
| Exception lifecycle | Central immutable grants/from/to/renewal; existing reviewed domain expiry | Aggregate keeps central protections; domains retain explicitly limited legacy semantics | A waived compatibility loss can become the next base's nonpass state; no automatic restoration or anti-renewal claim |
| Draft reconciliation | Blanket restore; commit mixed draft; selective planned reconciliation | Preserve six draft files and RED history; plan a coherent retained-store implementation | Retire only central-capability-specific assertions under explicit deferral; retain original domain and aggregate faults |
| Acceptance and scheduling | Reuse prior green runs; release downstream immediately; require current evidence | Remain blocked pending revised plans and later current acceptance | Full local CI/library, current direct perf/library/quality, called CI and authentic joins; no historical GREEN substitution |

**Concrete evidence:** latest named construction recovered nine of 47 required lines. Fresh counter invocation
`cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/discussion-count.json -P tests/cmake/gate_count.cmake`
returned exit 0 and 14,879 physical lines, source 8,580/6,533. It is an observation, not passing budget evidence:
quality958 exceeds920 by38; total exceeds14,841 by38 and C14,853 by26. No implementation tests or hosted
acceptance were rerun in this discussion. Historical eight-case success remains in 05-11-SUMMARY only.
`gh issue list --label digest --state open --json number,title,body,url --limit 2` returned exit 0 with no issue.
No current run URL exists for this local measurement; prior run URLs remain predecessor evidence only.

**Adversarial findings retained:** the dirty perf comparator reads central domains, while production still
passes ALLOW without trusted BASE_POLICY; compatibility still reads its own store. Passing selected tests
cannot accept this partial cutover. An exception never excuses current ledger/results mismatch. Authenticate
every original observation before extrema, including a smaller forged lane. Keep exact +1.00%/+1.01% arithmetic,
duplicate-base/input/NO_BASE and report-only/armed semantics, all relevant ordinary controls, and actual counts.
Do not conflate a reviewed legacy expiry with immutable central grant or restoration guarantees. The spent
POKEY migration needs accepted-base/replacement evidence before removal, not renewal or expiry-date guessing.

**Primary sources rechecked:** [Google SRE](https://sre.google/workbook/eliminating-toil/) supports evaluating
automation cost, not a repository-specific LOC allowance. [GitHub secure use](https://docs.github.com/en/actions/reference/security/secure-use)
supports minimum permissions and safe untrusted-data handling; deferral grants no new privileges. The scoped
choice is project judgment. No outside emulator source, dependency, service or product capability was added.

**Policy synchronization:** revised TUNE-03, CONTEXT D-09/D-10/D-21–23, ENGINEERING §8 and DEC.52 explicitly
state retained domain stores. A research supersession notice prevents copied old constraints from overriding
the new context. Existing roadmap/plan histories remain untouched: the next planning step must refresh their
active dependencies and acceptance, including 05-07's central-POLICY assumption and 05-09's store-retirement
requirement. 05-06/05-11 are not completed by this decision; no extra gap round is automatically authorized.

**Discretion and deferral:** central domain migration can be revisited only with concrete new evidence and
explicitly adopted scope. Reconciliation mechanics belong to the next plan, with no new allocation, broad
savings hunt, blanket restoration or routine owner confirmation. TUNE-03/TUNE-04 and final phase acceptance
remain pending. No execution, push, dispatch, arming, promotion, publication, ship, merge or second GSD step ran.

Next command: `$gsd-plan-phase 5`. Reuse unaffected research and revise only the changed scope/dependencies;
do not launch another migration recovery by default.

## One-line reconciliation halt: bounded resolution — 2026-10-22

The owner invoked discussion after05-12 halted and again delegated all relevant role reviews, primary-source
research, adversarial consideration and automatic adoption of coherent recommendations. This authorizes updating
context and selecting all three remaining areas. It does not invent interview answers, enable a chain flag or
start planning/execution. Existing plans require a later targeted replan. Current decisions are D-24–26.

Three read-only advisors covered architecture/construction, security/test validity, and delivery/governance.
Their common recommendation is one named separation of responsibilities, with unchanged scope and allocations.

| Area | Options considered | Adopted recommendation | Tradeoff and stopping condition |
|---|---|---|---|
| Measured fit | Consumer-owned numeric reads; restore accepted inline authentication; redistribute allocation; revise scope/cap | Plan only consumer-owned numeric reads | Remove3 helper lines, add1 per measured consumer: forecast minus1, not measured credit. No spare line or acceptance is inferred |
| Authentication | Shared auth/status helper; accepted duplicate authentication; broader helper redesign | Keep sharing; separate numeric consumption | Original identities precede reduction; typed values precede comparisons. CMake caller-scope coupling remains explicit |
| Controls | Retain all original and added faults; remove assertions to fit; compress formatting | Preserve controls, comments and readable flow | Keep seven perf cases, four added input/allowance assertions, ordinary controls and exact SHA diagnostic |
| Allocation | Keep strict14841/quality920; treat BE12 as reserve; increase by1; temporary waiver | No allocation change | BE12 is not reserve. A convenient refactor is not new required scope; one-line growth would not price remaining work |
| Delivery | Named finite attempt; generalized savings search; accept selected tests/prior greens; pause | Targeted replanning from new measured evidence, then current full acceptance | No gap-budget reset; a semantic/fit failure or additional required cost stops expansion. Preserve all halt histories |

**Role synthesis:** maintainers get a helper with a narrower responsibility; QA retains fault-specific assertions
and missing/malformed evidence rejection; security retains authentication before reduction; release/DevOps retain
current direct/called producer and base identity; owner delivery avoids an unbounded recovery loop. Players and
embedders retain the existing deterministic evidence. Unrelated UI, graphics and language ecosystems change no
decision here. A calendar cleanup was considered but excluded; additional fan-out has diminishing value.

**Adversarial pass:** removing conditional extraction must not leave stale macro outputs in use. In the aggregate,
structural initialization/advisory unavailable paths continue, every other non-measured status fails, then the
number is read before bounds/reporting. In the join the read belongs inside measured, before extrema; the allowed
unavailable base gate_lines branch never uses it. No assertion, typed check, identity field or unknown-status
rejection is traded away. The accepted join already authenticates every original item: the helper is sharing,
not a new trust guarantee. Returning to accepted authentication alone loses5 saved lines while retaining6 new
perf-control lines, forecasting14847/quality926. It is not an independently fitting fallback.

**Seed, not requirement:** the current forged-SHA fixture leaves its observation equal to the others; it proves
identity rejection but not a below-extremum masking case despite that claim. Keep the exact rejection and record
the test-harness gap as a tooling seed. Do not spend another review/gap round or expand acceptance because of it.

**Evidence:**
`cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/discussion-count.json -P tests/cmake/gate_count.cmake`
returned exit0:14842 physical lines, source8580/6533. Counter success is not fit acceptance; quality921 and
strict14841 remain exceeded by1. The count is current at feee6a98 with three retained drafts, not an immutable
accepted implementation. `gh issue list --label digest --state open --json number,title,body,url --limit 2`
returned exit0 and no issue. No implementation tests or hosted acceptance ran; no current run URL exists.

[CMake 4.3 macro semantics](https://cmake.org/cmake/help/v4.3/command/macro.html) explain caller-scope expansion
and support explicit value ownership. [Google SRE toil guidance](https://sre.google/workbook/eliminating-toil/)
supports weighing development and maintenance costs. [GitHub secure use](https://docs.github.com/en/actions/reference/security/secure-use)
supports existing minimum privileges and untrusted-input precautions. None prescribes this line budget or
claims a local static forecast has passed. No outside emulator source or new dependency was opened/added.

The next plan must preserve05-06/05-11/05-12 as halted evidence and retain the three drafts selectively. Only
current strict fit and complete local/direct/called acceptance after the final edit release07/08/09. Task2/3
costs remain unproved; no forecast savings, unused allowance or historical green finances them. TUNE-03/TUNE-04
remain pending. Discussion changes no implementation, quality.json, allocation, plan or phase completion.

Next command: `$gsd-plan-phase 5`. Reuse unaffected research; target D-24–26 and dependencies. Stop here.
