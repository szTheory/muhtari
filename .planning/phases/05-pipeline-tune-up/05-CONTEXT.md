# Phase 5: Pipeline tune-up - Context

**Gathered:** 2026-10-21
**Status:** Ready for targeted replanning; execution remains halted
**Revised:** 2026-10-22 after the measured 05-08 release-safety halt. D-27–31 govern the remaining release work and explicitly allocate its complete construction before implementation. Earlier halt records remain historical.

<domain>
## Phase Boundary

Deliver TUNE-01–05 from `.planning/REQUIREMENTS.md`: weekly triage and monthly hosted CI-efficiency reporting,
measured reliable CI, one quality ratchet within an audited absolute gate-code budget, controlled performance arming,
and release/docs/settings freshness with a shipped improvement to the weakest quality dimension.

This is discussion, not implementation or phase verification. The owner explicitly requested consideration of
all relevant perspectives, primary-source research, adversarial analysis and coherent recommendations, and
instructed the agent to follow those recommendations. Decisions below are agent-selected under that delegation;
they are not invented answers to an interview. No additional emulator capability or new GSD step is authorized here.

In the earlier budget revision the owner selected updating context, then explicitly requested all three budget/acceptance/
planning decisions be considered through relevant role reviews, primary sources and an adversarial pass, and
said to adopt the recommendations. That delegation authorizes the explicit replacement of the former hard
10% contract across AGENTS, TUNE-03, the roadmap, ENGINEERING and DEC.10. It does not assert infeasibility of
every possible 857-line implementation. In this revision the owner again requested all relevant areas,
parallel research, an adversarial pass and automatic adoption. That delegation authorizes updating existing
context and adopting D-27–31's narrow release-safety construction/allocation. Targeted planning is the next
session; no further routine approval or invented interview answers are required.

Carry forward three deliverables, integer bounded instance state, permissive reuse, automation-first verification,
one required CI check, preset entrypoints, no new service, CMake `-P`/POSIX sh repository checks, review limits,
and one GSD step per session. Prefer another copy and paste over another dependency; use existing platform tools.
</domain>

<decisions>
## Implementation Decisions

### Weekly triage and monthly evidence

- **D-01:** Use one open issue labelled `digest`, refreshed weekly, plus a job summary and downloadable report.
  Keep the latest monthly CI-efficiency section visible across subsequent weekly updates until replaced. Follow
  ENGINEERING §7's Monday 06:00 UTC schedule; include manual dispatch and a non-publishing preview for verification.
  Do not add a dashboard service, notification bot, token, database or dependency. Pinning the issue is optional;
  do not make delivery depend on an owner pinning it.
- **D-02:** Cover unlabelled issues, PRs idle over seven days, old Dependabot PRs/majors, open nightly failures and
  age, stale/red release PRs, and relevant runner-image deprecations. Keep the report actionable: reason, age,
  evidence link and suggested next action. No automatic issue closing, dependency upgrades, settings mutation,
  reviewer assignment or fresh issue per report. Read this digest at subsequent phase discussions.
- **D-03:** Monthly means a UTC calendar month with an explicit evidence window, ordinarily the preceding month
  on its first Monday. Permit an initial current-month partial report for phase acceptance. Fetch all pages and
  attempts in the window; distinguish direct CI, release-called CI, light planning runs, cancellations, failures
  and intentional failure controls. Report per-job p50/p95 execution, available queue/dependency-wait evidence,
  rerun-pass candidates, runner-minutes per merged PR, critical path, compile share and available CTest utilization.
  A rerun passing is a flake candidate, not proof. Do not call time spent waiting for dependencies runner queue time.
  Report sample counts, tool/workload changes and unavailable data; do not substitute zero or claim a 90-day
  redundancy history from this young repository. Exclude intentional failures from product flake rates but retain
  their cost. Required API failures fail the reporter; optional missing evidence is named in the published report.
- **D-04:** Reuse `gh`, `jq`, POSIX sh and CMake. A scheduled trusted-default-branch publisher gets only the reads
  and issue-write permission it needs; PR previews cannot publish or access App credentials. Treat issue titles,
  PR bodies and branch names as data, never shell source. Write structured report bodies to files. Serialize
  publishers so concurrent dispatches cannot create duplicate digest issues, and fail visibly on a failed write.
  Native cron is best effort: retain manual recovery and expose last successful report time, without an external
  scheduler. No public ROM bytes, private paths, credentials or private title results enter these reports.

### CI reliability and cost

- **D-05:** Fix the reproduced compatibility freshness race by assigning each writer its own generated output,
  including the self-test. Preserve the exact stale-page negative control. Use CTest fixtures for prerequisites;
  reserve `RESOURCE_LOCK` for resources that cannot be isolated. Do not serialize the whole suite or add retries
  to conceal shared-file contention. Verify the affected checks repeatedly together with parallel/random order.
- **D-06:** Re-derive job bounds from the last 40 comparable successful executions, recording slowest duration,
  run URL/ID, sample count, workload and runner. Use twice the maximum, round job bounds up to minutes, retain
  the established five-minute network/roll-up floor, and mark fewer than ten samples provisional. Apply the
  same measurement principle to slow tests and emulation/sanitizer scales. A timeout is a censored lower bound,
  not an elapsed-time success to average away. Workload/toolchain changes invalidate older samples where relevant.
  The temporary 240-second `audio.samples` timeout is not a final measurement; preserve all eight comparisons.
  Measure concurrency and schedule costly tests early before shrinking work or increasing limits again.
- **D-07:** Keep all six native build/test/package legs required. Apply the existing DEC.46/ENGINEERING §9 flip:
  full `be32` execution has exceeded five minutes, so move its execution and full hash equality to nightly and
  keep PR cross-compilation required. Nightly must run the promised full inventory and compare against a native
  artifact built from the same commit with identical fixtures/configuration, with an exact file count. The PR
  required comparison becomes the six native files, plus WASM only if separately promoted. Update roll-up,
  inventory expectations, release-call behavior and docs together; report reduced per-PR BE execution coverage
  explicitly. Do not silently drop BE evidence from release/ship checks. A small PR BE execution slice is an
  option only if measurements show useful independent coverage within five minutes without duplicating machinery.
- **D-08:** No new cache by default. Consider compiler caching only after measured compile share reaches the
  settled 40% of a critical-path leg, and prove cached/cold equivalence before adopting it. Keep pinned-download
  integrity and fetch-path checks. Remove a check only with a named fault and a demonstrably stronger remaining
  signal; lack of failures alone does not prove redundancy. Preserve exact executed inventories, fail-on-missing
  artifacts, no self-skips, bounded hostile workloads and meaningful negative controls.

### One quality ratchet and honest portability

- **D-09:** One committed root `quality.json` owns the aggregate's ordinary bounds and dated reasons, and one
  required `quality` check consumes authenticated existing outputs under `CI required`. Use CMake `string(JSON)`.
  Retain independent semantic judges. For this phase, defer centralization of the performance and compatibility
  domain policies: `tests/perf/allow.txt` and `tests/compat/allow.txt` remain their respective authorities, with
  the existing comparator/ledger contracts. Do not add duplicate central domain thresholds or reasons. This
  explicitly replaces the earlier requirement to retire both stores; it narrows scope rather than claiming
  an equivalent central exception lifecycle. All remaining measured aggregate metrics and mandatory evidence
  remain required. See D-21–23 for the exact limitation and reconciliation acceptance.
- **D-10:** For aggregate metrics, compare policy changes with the trusted PR base: floors cannot fall, ceilings cannot rise, metrics
  cannot disappear, and new thresholds cannot legitimize the same PR's regression silently. A loosening needs
  the corresponding key/from/to, reason, date and bounded expiry (at most 60 days), visible in review and reports;
  expired or unrelated reasons do not authorize it. Tightening is free. A measured value must satisfy the effective
  bound too: comparing two JSON policies alone proves nothing. Missing/malformed required evidence fails; a
  named advisory/unavailable metric has no fabricated passing value. Validate boundary, missing-data, expired-
  exception and baseline-tampering cases using the existing negative-control pattern.
  The gate-code metric has one narrow structural distinction: initialization and explicitly adopted new-scope
  allocations follow D-19; permanent required machinery is not financed through recurring 60-day waivers.
  Under unchanged scope, count overruns still need the exact dated temporary exception and credible exit.
  A new scope allocation cannot loosen another metric, excuse a missing control or authorize self-rebaselining.
- **D-11:** Start with measured inventories, public compatibility, current ABI/warning/analyzer results, archive
  sizes, current fuzz targets/regressions, gate-code count and comparable Ir output. Historical/nondeterministic
  CI-duration observations require a defined comparable sample window; do not gate on an arbitrary shared-host
  outlier or let a PR forge its own hosted history. Scheduled coverage/open-crash/backlog measurements may join
  only with real producers and explicit freshness; today's workflows do not yet produce every metric described
  in preparation. Keep absent dimensions visible as evidence gaps, not zero findings or 100% coverage. Preserve
  independent release/archive and behavioral predicates even where the aggregate consumes their results.
- **D-12:** Arm the existing +1.00% Cachegrind comparison only after two fresh control dispatches on the same
  suitable commit each produce three identical measurements, a current PR produces a numeric valid base, and
  forced-failure/recovery verifies nightly reporting. Preserve equal-length paths, stripped environment, warm-up
  subtraction, matching compiler/tool versions, duplicate base measurements, exact threshold arithmetic and
  fail-closed missing inputs. Verify the current workloads, not just old success summaries. Keep wall-clock tails
  advisory. The present perf job measures the test card only: name that coverage honestly. Research whether a
  small representative existing DMA/TIA/POKEY diagnostic set fits the already allowed four-fixture bound; adding
  a measured fixture requires its own controls, not an unsupported claim that test-card arming protects all games.
- **D-13:** Audit WASM, library and nightly property history against their documented ten-green-main-run
  conditions. Count actual completed successful executions of each lane on distinct qualifying runs, not ten
  workflow IDs, skipped jobs, PR controls or rerun attempts. Promote eligible lanes with their exact artifact and
  inventory contracts; otherwise retain explicit advisory status and the missing evidence. No weakening a
  threshold or manufacturing runs merely to declare promotion. The audit here found only ten direct main-push
  CI records, including one failed and one still incomplete; eligibility has not been established. Recheck at
  execution. Post-merge truths belong to ship, never owner UAT.

### Freshness, release safety and proportionality

- **D-14:** CI/CD is the provisional weakest dimension and this phase's main improvement. Evidence: two audio
  timeouts, the reproduced output race, a required BE job above the agreed cost threshold, absent digest/quality
  infrastructure and over-budget enforcement code. Baseline all six ENGINEERING §12 dimensions using a short
  evidence-linked 1–5 rubric before execution changes; select any different lowest dimension only with new
  measured evidence within the fixed phase scope. Measure before/after critical-path time and runner work on
  comparable workloads, retain correctness/coverage, and ship the reliability fix and latency improvement.
- **D-15:** Measure repeated release rehearsals before changing their selection. Prefer automatic input-based
  selection over an owner-maintained label if duplication is material. Reuse only a successful rehearsal whose
  complete effective inputs remain equivalent: release/publish logic, called CI/build/package code, tools/pins,
  version inputs and all source affecting archives. A push without a change to one of the trigger paths is NOT
  by itself safe to skip: the current archive content may have changed. Initial runs, unknown diffs, force-pushes,
  missing evidence and changed inputs run a full rehearsal. If demonstrating safe reuse costs more machinery
  than it saves, retain the current conservative rehearsal and record the measured decision. A retained reuse
  claim must name the prior run/input identity and current-head validation; no stale green status is relabelled
  as a fresh rehearsal of another commit.
- **D-16:** Check user/maintainer docs against the released behavior, with concise README/install instructions
  and detailed testing/release runbooks in `docs/`. Reconcile stale hash-file counts/audio formats, lane statuses,
  test timeouts and release replay/cleanup promises with code. Run the install line against actual artifacts and
  `scripts/apply-repo-settings.sh --check`; request owner consent only for a needed admin mutation or merge.
  Include reproduced release lookup/attestation/Dependabot issues from the related todo; distinguish API error
  from 404 and verify signer identity before irreversible publication. Choose a narrow true docs claim where
  adding a new bot state machine would violate proportionality. Phase ship confirms release PR health and actual
  publication when applicable; pipeline-only commits use `ci:`/`test:`/`docs:` and do not invent a product release.
- **D-17:** Replace the hard tenth-of-src ceiling with one audited absolute physical-line ratchet in
  `quality.json`; keep 10% and full/authored source ratios diagnostic, with no deadline promising convergence.
  Count every repository-check implementation, dedicated fault-control harness, declarative enforcement,
  shared helper once, invocation contract and embedded workflow/configure predicate, including moved code
  and the counter/manifest/glue itself. Include comments and blanks. Default to whole files; any exact block
  exclusion must be purely operational/generation work with its boundary documented. Ordinary emulator product
  tests and fixture bytes are outside this maintenance metric; embedded repository-policy assertions and
  check controls are not. Publish the file/block inventory and broad sensitivity set. No source padding, line
  golf, hidden driver classification, outsourcing predicates to dependencies or free allowance from core growth.
  Preserve firmware/provenance/archive, signer/identity, privacy, bounded deterministic-core, exact executed
  inventory/no-skip and independent release safety outcomes with meaningful fault controls. Implementations may
  merge or be replaced by stronger evidence; preserving a fault does not require preserving its old script.
  The historical 1,367/8,577 count was narrow; 2,764 is a partial mixed-file sensitivity set, 8,607 overinclusive,
  and 2,262 one failed incomplete construction. None is a complete numerator, lower bound or replacement cap.
- **D-18:** Keep work small and cohesive, prioritize the reproduced faults and simplification, and stop expanding
  analysis when another lens changes no decision or verification. Security, reliability, maintainer cost,
  embedder/player compatibility, test validity and release consumers are relevant; UI design, rendering-engine
  rewrites, distributed services and unrelated language ecosystems add no phase requirement. Do not reopen
  settled emulator choices or turn all dormant review seeds into mandatory work. Export lessons in the existing
  pasteable template only when asked; do not start an export workflow as a second GSD step.

### Budget initialization, acceptance and planning readiness

- **D-19:** Freeze an exact audited starting commit/count `B` in the first bounded execution slice. Measure
  actual retained-fault net savings `S` from its readable simplification, including any changed control/glue
  cost; do not claim forecast savings or double-spend deletions. Before affected additions, fix per-component
  allocations totaling `N` and a phase ceiling `C = B - S + N`. A separately stated integration reserve is part
  of fixed `N`, never extra headroom. Allow additions only for the scoped digest/reporting, quality ratchet,
  BE relocation/comparison and reproduced safety/correctness fixes; each names its requirement/fault and
  includes its controls, shared definitions and invocation costs. Inherited-code refactoring is not a new
  allowance. Redistribute within the total only with the mapping visible. No new capability or allowance is
  implied by a convenient abstraction, dependency or renamed file. Maintain one short allocation table in
  existing planning artifacts and one numeric quality metric, not a classifier, allowance interpreter, fault
  taxonomy engine, policy DSL or prose-validation gate. At close, actual count must fit `C`; retire unused
  allowance/reserve and tighten to accepted actual. Subsequent permanent growth needs explicit scope adoption
  and a fixed from/to allocation before implementation; a proposed baseline edit alone cannot grant it.
  Temporary unchanged-scope overruns follow D-10, with expiry checked even when inherited from the base and
  a credible named removal/consolidation exit. No automatic renewal or waiver-to-baseline conversion.
- **D-20:** Planning may proceed with a complete counting boundary/fault-to-consumer map, component allocation
  ranges, named integration uncertainty and the first bounded baseline/simplification task. It need not first
  implement or prototype the complete phase. That task freezes `B`, demonstrates nonzero fault-preserving
  savings and fixes the numeric `N`/`C` before adding the new enforcement; it does not mint an unlimited budget.
  An unresolved boundary, lost fault/control, absent required evidence or exceeded allocation stops affected
  expansion. Simplify within the ceiling, or use the existing one-gap/new-evidence route for a concrete scope
  revision; do not repeat broad research or silently raise the total. Remaining independent work may proceed.
  Phase acceptance still requires all TUNE-01–05 outcomes and their automated/agent evidence, improved inherited
  machinery with measured retained-fault savings, and final count within the explicitly allocated absolute
  ceiling. A higher total caused by allocated required capabilities is reported honestly. The former 10%
  overage is a historical fact and advisory proportion, not a deferred mandatory debt or temporary waiver.
  Refresh only research affected by this policy change, reuse measured experiments and valid implementation
  guidance, and stop analysis when another lens changes no decision or control. No second GSD step here.

### Scope resolution after the measured construction halt

- **D-21:** Stop the domain-policy migration for this phase and retain the accepted ordinary aggregate ratchet
  plus the existing performance and compatibility judges. The fresh complete count is 14,879; quality use is
  958 against 920, with 38 still needed for the strict 14,841 total and 26 above C14,853. The named shared-reader
  construction saved nine lines, not the required 47. This is evidence against that construction, not proof that
  no smaller implementation exists. Another generalized savings hunt or automatic gap round is not authorized.
  Increasing quality by 38 would fund only unfinished Task 1, not the unmeasured compatibility/caller work.
  B12,885/S2/N1,970/C14,853 and count368/BE205/digest447/quality920/fixes30/reserve0 remain unchanged;
  the unused BE12 is not reserve. No new permanent or temporary allocation, deadline waiver or dependency is
  adopted. A future centralization proposal needs concrete new evidence and its own explicitly adopted scope.
- **D-22:** Preserve the six draft files and committed RED `df6b095` as historical work. During a later planned
  execution, reconcile them selectively into a coherent retained-store implementation; do not blanket restore,
  reset or stash the workspace. The current draft is mixed: perf reads central domains while its production
  caller still supplies ALLOW; compatibility remains on its legacy store. It is not accepted. Retain useful
  original-item authentication and exact forged-SHA controls only where they fit and remain relevant. Reconcile
  central-only fixture assertions with the explicitly deferred capability, while preserving all seven perf cases'
  original arithmetic/verdict/input/exception faults and the ordinary aggregate controls. Preserve historical
  RED/halt records rather than falsely declaring that centralization passed. No implementation runs in discussion.
- **D-23:** Acceptance must identify one active authority per bound. The ordinary aggregate retains its exact
  trusted-base, from/to, grant/expiry, renewal and actual-value controls. Legacy domain exceptions retain their
  existing reviewed expiry/identity behavior, not those stronger central guarantees: in particular, a waived
  compatibility regression that merges can become the next base's nonpass state; the legacy store does not
  retain the original pass obligation indefinitely. Do not advertise immutable domain grants, anti-renewal,
  automatic restoration or a unified central policy. Preserve exact current ledger/results equality by key,
  status and evidence in both directions, passing-base retention outside an authorized legacy exception, exact
  +1.00%/+1.01% verdicts, unstable/missing/nonpositive/NO_BASE handling and armed/report-only semantics.
  An exception never excuses fresh result mismatch or forged original observations. Audit the spent POKEY
  migration against the accepted base and replacement evidence before removal; do not renew it or claim its
  permanent retirement from the date alone. Final reconciliation must fit quality920 and total14,841 with
  unchanged other components, as well as C14,853; full local CI/generated-corpus library and current direct
  numeric perf/library/quality plus release-called CI and authentic artifact joins remain required. Prior green
  runs and the eight selected passing cases cannot accept changed callers. Plans 05-07/08/09 remain blocked
  until revised dependencies and reconciliation acceptance are planned and subsequently satisfied. Preserve
  05-06/05-11 as halted histories; do not mark either complete. TUNE-03/TUNE-04 remain pending. TUNE-04/D-12
  readiness and lane-promotion predicates are unchanged; no arming, promotion, ship or merge occurs here.

### Bounded resolution after the one-line reconciliation halt

- **D-24:** The measured 05-12 result is new evidence for targeted replanning, not permission to resume its
  failed task or reset review/gap limits. Its complete count is 14,842, quality921: one above the unchanged
  strict 14,841/quality920 contract. Adopt exactly one identified construction for the next plan: keep the
  shared original-observation authentication and status reader, but make the two measured-value consumers
  own their typed numeric reads. Remove the helper's three-line conditional numeric-extraction block; read
  the number once in the join's measured branch before extrema, and once in the aggregate after its
  unavailable/required-status handling before bounds/reporting. Inspection forecasts three lines removed and
  two added, a net one-line saving. This narrows helper responsibility without statement compression, deleted
  comments or removed controls. It is not measured savings or accepted fit. Planning must name this finite
  construction and its proof obligations; no calendar refactor, generalized savings search or fallback cleanup
  is authorized. Restoring accepted inline authentication alone would lose five lines of net sharing savings
  and leave the six-line perf-control addition unfunded; it is not an independently fitting fallback.
- **D-25:** Authentication must still validate each original item's identity before a reduction can discard it.
  Preserve exact artifact membership, trusted base SHA/policy, comparable environments, SHA/source/units/
  producer/configuration/direction checks and worst-value reduction. The aggregate retains structural base
  initialization and advisory-unavailable continues, then rejects every other non-measured status before
  its typed numeric read. The join reads a number only inside its measured branch; its permitted unavailable
  base gate_lines path never uses a stale value. Malformed measured numbers still fail before extrema or
  bounds. Retain the exact forged-SHA diagnostic and all ordinary policy controls, all seven perf cases and
  the added wrong-ROM, malformed-allowance, zero-HEAD and negative-BASE2 assertions. The accepted join already
  authenticates original items; sharing adds no new security guarantee under its trusted source contract.
  Preserve useful sharing for its measured construction cost, without claiming stronger evidence than exists.
- **D-26:** Keep B12,885/S2/N1,970/C14,853, quality920 and strict14,841 with unchanged other components. BE12
  remains unavailable as reserve; no permanent/temporary allocation, waiver or scope increase is adopted.
  This discussion explicitly adopts only the named construction under the owner's recommendation delegation.
  Preserve 05-06/05-11/05-12 as halted histories and retain all three current drafts selectively. The planner
  must choose a continuation/replacement representation that preserves those facts and refreshes active
  dependencies; discussion does not mark a plan complete or rewrite historical summaries. A semantic failure,
  measured fit miss or additional required compatibility/caller/control cost stops affected expansion; do not
  credit forecasts, silently absorb growth or launch another automatic gap round. Task2/3 costs remain unproved.
  Acceptance after the last implementation edit still requires full local ci and generated-corpus library
  against a verified base, physical/component and documentation budgets, current direct numeric perf/executed
  library/quality, release-called required CI, exact authenticated nine/eight-artifact joins and executed
  inventories. Record command, exit and actual run URL with producer/base SHA and attempt identities. Eight
  selected cases and predecessor greens remain insufficient. Only accepted reconciliation releases 05-07/08/09;
  arming, promotion, release and final tightening retain their separate existing predicates. Stop further
  advisory fan-out now: security, test validity, architecture, delivery and cost reviews change no other decision.

### Release-safety construction after the measured 05-08 halt

These current decisions supersede the old unchanged-allocation restriction only for the named release-safety
addition below. D-09/D-10/D-21–26's semantic contracts, prior acceptance and halted histories remain intact.

- **D-27:** Adopt a bounded continuation of the existing publisher and synthetic transport fixture for
  TUNE-05/D-16: enforce signer workflow and source ref before publication; distinguish release/tag absence
  from API/transport/schema failure; confirm cleanup; exercise those boundaries through the real script.
  The existing control is RED, not accepted behavior. Preserve 05-08 at 0/3 and RED `e0c18a0`; the next planner
  selects a continuation/replacement representation and refreshes dependencies without rewriting that history.
  Accepted 05-13 and 05-07 remain prerequisites. No implementation runs in discussion.
- **D-28:** Official GitHub CLI advisories are new evidence: signer-workflow prefix matching and case-insensitive
  source-ref matching affect gh through 2.101.0 and are patched in 2.102.0. Local `gh --version` is 2.101.0;
  hosted versions are unmeasured. Require a supported patched gh, at least 2.102.0, in every publisher attestation
  verification context, with an explicit fail-closed version check and meaningful boundary/malformed controls.
  Reuse the platform CLI; any needed installation must follow existing package or verified pinned-download
  practice, with all enforcement/glue charged below. Before PATCH require repository, full signer path
  `$REPO/.github/workflows/release.yml` and intended source ref. Preserve independent archive-commit, exact
  asset, firmware, hash and tamper checks. Do not add `--source-digest "$EXPECTED_SHA"` automatically:
  a delayed release can legitimately have a workflow-trigger commit different from the archived release commit.
  The synthetic transport proves argument placement and short-circuiting, not upstream certificate semantics;
  current hosted rehearsal must exercise the actual patched verifier.
- **D-29:** Use the existing gh transport and one small explicit HTTP-status helper, not a new dependency,
  authentication state machine or generic API framework. Capture command success and structured HTTP status,
  rather than matching an error message or assuming `set -e` propagates through every conditional/substitution.
  Retain complete paginated release lookup for drafts; validate successful listing shape and zero-or-one
  matching positive numeric ID before create/delete. A failed listing, including listing 404, is failure.
  A successful complete authenticated listing supplies repository-access evidence; resource-specific 404 can
  then establish the relevant object's absence, without claiming that 404 independently proves authorization.
  Reject 401/403/429/5xx, transport failure, malformed status/body/ID and surviving objects. Do not swallow a
  failed DELETE and infer success from a failed GET. Explicit already-absent responses may be idempotent;
  confirmation must still use the relevant resource and successful complete lookup. Retain draft IDs and
  release-before-tag deletion. Controls must cover successful/empty/paginated/malformed/failed lookup,
  create/delete failure, already absent/surviving objects, and no publication or creation after its prerequisite
  fails, plus correct signer/ref, wrong signer/ref and tamper. Use shared table-driven cases in the existing
  harness; no upstream cryptographic test suite is adopted.
- **D-30:** Explicitly adopt a fixed **300-line release-safety addition** before further implementation.
  It funds the newly dedicated real-publisher adversarial controls, complete fail-visible lookup/cleanup
  repair and newly evidenced patched-verifier enforcement, including their invocation/glue. It does not fund
  an inherited-code rewrite, rehearsal reuse, unrelated review seeds or another capability. Keep frozen
  B12,885/S2; increase fixes30 to330 and N1,970 to2,270. The phase ceiling becomes C15,153 and the strict
  pre-closeout total becomes15,141; count368/BE205/digest447/quality920/reserve0 otherwise stay unchanged.
  BE12 is not reserve; quality's one unused line is not credited as safety funding. The 27-line reserve below
  belongs inside the release-safety300, not the old global reserve or additional allowance.

  | Release-safety increment, net physical growth | Fixed allocation | Evidence or construction |
  |---|---:|---|
  | Retained first control and registration | 66 | Measured: fixture60, dispatcher4, inventory1, manifest1 |
  | Signer restriction and patched-CLI enforcement | 22 | Estimated production delta in existing publisher/workflow |
  | HTTP/list/create/delete failure and absence handling | 55 | Estimated production delta; one small helper |
  | Remaining complete control matrix | 115 | Estimated fixture expansion, including version boundary/malformed cases |
  | Additional registration/inventory/invocation predicates | 15 | Estimated counted glue; shared work counted once |
  | Release-safety integration reserve | 27 | Explicit uncertainty inside this300 only |
  | Total | 300 | Hard ceiling, not a spending target or measured fit |

  Only66 is measured; the remaining234 is a bounded construction constraint, not a proof that implementation
  will fit. A more conservative384 estimate was considered; no concrete additional required fault established
  its necessity, so the tighter300 is selected without claiming384 wasteful or300 proven feasible. Internal
  redistribution must be visible within300; no other component finances it. The old RED overrun remains a
  historical unaccepted overrun. In later execution update `quality.json` and its provenance to this explicit
  reviewed structural allocation before affected GREEN additions, using existing authenticated adoption rules;
  no temporary waiver, candidate self-authorization or weakening of another metric is allowed. Count the
  complete resulting inventory, including comments/blanks, controls and embedded glue. Unresolved counting,
  lost controls, a measured component/strict-total miss or missing required evidence stops expansion; no
  automatic renewal, extra gap round, generalized savings hunt or compressed statements. At closeout retire
  unused allowance/reserve and tighten to accepted actual, under the existing final-tightening contract.
- **D-31:** Retain conservative rehearsals and measure their cost; implement no reuse or new selection state
  machine in this repair. Current `release.yml` has five trigger paths, including `be-equality.yml`; these are
  selectors, not proof of complete archive-input identity. Correct current docs claiming four paths and narrow
  replay/cleanup/Dependabot claims to actual behavior. A bot's initial eligibility decision does not promise
  revocation after every later human push. Acceptance after the last production/control edit needs selected
  fault-specific controls, full local `cmake --workflow --preset ci`, exact inventories, component/total/doc
  budgets, current required direct and release-called callers with authentic artifact identity, and actual
  patched-gh rehearsal publication/install/cleanup evidence. Prior greens and a synthetic transport cannot
  substitute for changed-path acceptance. Keep 05-09 blocked until accepted release safety; it then performs
  final tightening and actual docs/install/settings checks. No requirement, assumption, portability promotion,
  ship, merge or default-branch digest publication is completed by this discussion. The seven flagged
  assumptions and descriptorless-boundary prohibition remain pending; post-merge truths belong to ship.

### Agent's Discretion

Internal schema names, helper boundaries, batching/API page sizes, readable report formatting, per-test cost
values and the exact simplifications are researcher/planner choices within these contracts. The numbered
choices are coherent outcomes, not a task breakdown; formal phase research still runs. No dependency addition
is justified merely by a convenient parser or reporting abstraction.
The agent may set the concrete allocations from evidence and make routine implementation choices within the
fixed scope/ceiling without another owner confirmation. It may not grant itself a larger scope or total.

### Folded Todos

Keep todos pending until execution supplies evidence; folding is not completion.

- `.planning/todos/pending/2026-10-16-arm-the-instructions-per-frame-gate.md`: controlled performance arming and
  nightly issue recovery, D-12.
- `.planning/todos/pending/2026-10-16-limit-release-rehearsals-to-pushes-that-change-release-inputs.md`: measure
  duplication and decide safe selection versus documented retention, D-15; not a mandate to skip.
- `.planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md`: eligibility audit and conditional
  promotion, D-13; no claim that the ten-run condition is already met.
- `.planning/todos/pending/2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md`:
  reproduce current publish/bot/docs claims, then apply the minimal correction within D-16 and review budgets.
</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read relevant sections before planning or implementing.**

### Scope and settled contracts

- `AGENTS.md` — especially rules 6–10, dependency ladder and summary evidence contract.
- `.planning/PROJECT.md`, `.planning/REQUIREMENTS.md`, `.planning/ROADMAP.md`, `.planning/STATE.md` — active scope,
  TUNE-01–05, phase goal, accepted predecessor work and historical exceptions; phase acceptance remains pending.
- `.planning/preparation/README.md` — reference index.
- `.planning/preparation/ENGINEERING.md` §§3–9, 11–13 — inventory, CI, release, reporting, ratchet and budgets.
- `.planning/preparation/DECISIONS.md` DEC.07–15, DEC.43–54, DEC.56 — locked process and portability flip conditions.
- `.planning/phases/05-pipeline-tune-up/05-RESEARCH.md` — preserve measured experiments and limitations;
  its copied old constraints and 857-line planning prohibition are superseded by this revision, not current policy.
- `.planning/seeds/SEED-007-milestone-tune-up.md` — recurring scope and historical gate inventory; do not rewrite
  its dormant status as part of this discussion.
- `.planning/RETROSPECTIVE.md`, `.planning/LESSONS.md` — local evidence versus inbound hypotheses.
- `.planning/milestones/v1.0-phases/04-sound/deferred-items.md` — reproduced compatibility-output race.
- `.planning/milestones/v1.0-MILESTONE-AUDIT.md` — inherited evidence gaps; do not erase accepted history.
- The four full todo paths in Folded Todos above, and the two paths in Reviewed Todos below.

### Current halt and scope reconciliation

- `.planning/phases/05-pipeline-tune-up/05-08-SUMMARY.md` and
  `.planning/phases/05-pipeline-tune-up/05-08-PLAN.md` — current release-signer RED and measured allocation halt;
  D-27–31 supersede the unchanged-allocation restriction for a bounded next plan, not the historical outcome.
- `.planning/phases/05-pipeline-tune-up/05-13-SUMMARY.md` and
  `.planning/phases/05-pipeline-tune-up/05-07-SUMMARY.md` — accepted reconciliation/arming prerequisites;
  active accepted completion is7/9; no prior hosted evidence accepts changed publisher paths.
- `tests/policy/publish.cmake`, `tests/policy/gate-lines.txt`, `quality.json`,
  `.planning/WINDOWS.md` — existing real-script fixture, complete boundary, ordinary policy and unresolved facts.
- `.planning/phases/05-pipeline-tune-up/05-12-PLAN.md` and
  `.planning/phases/05-pipeline-tune-up/05-12-SUMMARY.md` — historical reconciliation fit halt, exact per-path/component
  counts, retained RED and three drafts; preserve as historical evidence when replanning D-24–26.
- `.planning/phases/05-pipeline-tune-up/05-11-HANDOFF.md` and
  `.planning/phases/05-pipeline-tune-up/05-11-SUMMARY.md` — latest failed construction, six retained drafts,
  unchanged allocations and unrun acceptance; historical records, not an active permission to retry.
- `.planning/phases/05-pipeline-tune-up/05-06-HANDOFF.md` and
  `.planning/phases/05-pipeline-tune-up/05-06-SUMMARY.md` — retained RED lineage and first migration halt.
- `.planning/phases/05-pipeline-tune-up/05-05-SUMMARY.md` — accepted aggregate reference and prior hosted
  evidence, not acceptance of the dirty migration.
- `.planning/phases/05-pipeline-tune-up/05-07-PLAN.md`,
  `.planning/phases/05-pipeline-tune-up/05-08-PLAN.md` and
  `.planning/phases/05-pipeline-tune-up/05-09-PLAN.md` — existing plans requiring dependency/acceptance refresh.
- `docs/quality.md` — current ownership prose must be reconciled with actual retained-store execution.

### Existing producers and integration contracts

- `.github/workflows/ci.yml`, `.github/workflows/nightly.yml`, `.github/workflows/release.yml`,
  `.github/workflows/dependabot-automerge.yml` — actual DAG, permissions, statuses and rehearsal behavior.
- `CMakePresets.json`, `cmake/MuhtariTests.cmake`, `tests/inventory/ci.txt`, `tests/inventory/hygiene.txt` —
  preset parity, timeouts, cost and inventory enforcement; other lane inventories when their contract changes.
- `tests/policy/tests.cmake`, `tests/audio/tests.cmake`, `tests/compat/library.cmake`, `tools/compat_doc.cmake` —
  output ownership, the slow audio workload and compatibility evidence.
- `tests/cmake/perf_compare.cmake`, `tests/perf/tests.cmake`, `tests/perf/allow.txt`,
  `tests/cmake/compat_ratchet.cmake`, `tests/compat/allow.txt` — existing comparison/exception semantics.
- `tests/cmake/ci_required.cmake`, `tests/cmake/hashes.cmake`, `tests/cmake/check_junit.cmake`,
  `tests/cmake/expect_fail.cmake` — fail-closed aggregate, exact evidence and negative controls.
- `scripts/hygiene.sh`, `scripts/classify-changes.sh`, `scripts/apply-repo-settings.sh`, `scripts/publish.sh`,
  `.github/rulesets/main.json`, `tests/cmake/firmware_scan.cmake` — safety faults to preserve during simplification.
- `README.md`, `docs/testing.md`, `docs/maintaining.md`, `docs/emulation-tests.md`, `docs/runner.md`,
  `docs/libretro.md`, `PROVENANCE.md`, `ASSET_POLICY.md` — docs freshness and what evidence may be published.

### Primary external sources checked 2026-10-21

- [Google SRE, Eliminating Toil](https://sre.google/workbook/eliminating-toil/) — weigh automation's development,
  maintenance and operational cost; it supplies no repository-specific LOC threshold. The absolute ratchet
  and allocation procedure are project judgments, not a number prescribed by this source.
- [NIST SSDF](https://doi.org/10.6028/NIST.SP.800-218) — risk-based security requirements and reviewed exceptions;
  does not justify removing firmware, identity, privacy or release protections to satisfy a line metric.
- [GitHub required-status troubleshooting](https://docs.github.com/en/pull-requests/how-tos/merge-and-close-pull-requests/troubleshooting-required-status-checks)
  — skipped jobs can report success; retain evidence that required work actually executed when simplifying.

- [GitHub workflow-job REST API](https://docs.github.com/en/rest/actions/workflow-jobs) — job/step timestamps,
  statuses, logs and attempt-specific listings; establish which queue fields actually exist before deriving them.
- [GitHub workflow-run logs](https://docs.github.com/en/actions/how-tos/monitor-workflows/use-workflow-run-logs) —
  partial rerun log archives omit earlier jobs; collect attempts rather than treating latest logs as all execution.
- [GitHub workflow events](https://docs.github.com/en/actions/reference/workflows-and-actions/events-that-trigger-workflows) —
  PR versus push filtering, default-branch/manual/scheduled execution; cron can be delayed/dropped.
- [GitHub script injection](https://docs.github.com/en/actions/concepts/security/script-injections) — untrusted
  issue/PR data cannot be interpolated into shell source.
- [CTest RESOURCE_LOCK](https://cmake.org/cmake/help/latest/prop_test/RESOURCE_LOCK.html) and
  [CTest COST](https://cmake.org/cmake/help/latest/prop_test/COST.html) — targeted mutual exclusion and expensive-
  first scheduling; prefer independent output ownership when possible. Recheck against the project's CMake version.

### Primary sources rechecked 2026-10-22

- [CMake 4.3 macro documentation](https://cmake.org/cmake/help/v4.3/command/macro.html) — macros expand in
  the caller's scope; consumer-owned numeric reads make value ownership explicit. The one-line construction
  and budget are local engineering judgments, not requirements prescribed by CMake.
- [Google SRE, Eliminating Toil](https://sre.google/workbook/eliminating-toil/) — evaluate maintenance and
  development cost; no prescribed LOC allowance. Deferral is a project judgment, not a source mandate.
- [GitHub secure use reference](https://docs.github.com/en/actions/reference/security/secure-use) — retain
  least privileges and safe handling of untrusted workflow data; policy deferral grants no extra permissions.

No external emulator source was opened. Existing preparation already records sibling release/CI precedents;
no new framework/ecosystem dependency is warranted by this phase.

### Release-safety primary sources checked 2026-10-22

- [GitHub CLI signer-workflow advisory](https://github.com/cli/cli/security/advisories/GHSA-wjmr-j3rp-mh2g)
  and [source-ref advisory](https://github.com/cli/cli/security/advisories/GHSA-4mq3-hpgx-9cx8) — affected
  ranges and patched2.102.0; verified independently through official REST advisory responses and web search.
- [GitHub CLI2.102.0 release](https://github.com/cli/cli/releases/tag/v2.102.0) — published2026-09-30;
  tool-floor evidence, not a claim about installed hosted runners.
- [gh api manual](https://cli.github.com/manual/gh_api) — status/header capture, pagination and slurping;
  installed `gh api --help` was also inspected. Local help describes the interface, not correctness of its
  vulnerable attestation implementation.
- [GitHub REST troubleshooting](https://docs.github.com/en/rest/using-the-rest-api/troubleshooting-the-rest-api)
  —404 can hide insufficient authentication; report access errors honestly.
- [GitHub automatic merging](https://docs.github.com/en/pull-requests/how-tos/merge-and-close-pull-requests/automatically-merging-a-pull-request)
  —automatic disablement after a push by someone without write permission does not promise revocation after
  every human push.
</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets

- Existing preset workflows, `gh`/`jq`, pinned actions, result inventories, hashes, perf verdicts and archive outputs
  supply most data; no codebase maps exist, so scouting followed the infrastructure files directly.
- `tests/policy/tests.cmake` constructs a shared `-DOUT=.../policy/compatibility.md` command for freshness and its
  self-test. Changing ownership avoids adding a synchronization subsystem.
- `tests/cmake/expect_fail.cmake` and `tests/perf/tests.cmake` already verify specific failure codes/verdicts.
- `scripts/apply-repo-settings.sh --check` already checks full settings/labels/ruleset and reported no drift here.

### Established Patterns

- Required roll-up explicitly enumerates allowed skips; changing a job's role requires its synthetic controls too.
- Artifacts and JUnit prove exact execution. One source of truth must consume these, not counts copied by hand.
- Accepted05-07 armed test-card-only perf; WASM/library and nightly property testing remain advisory.
- `scripts/publish.sh` owns release preparation/attestation/lookup/cleanup; its current `attested` path lacks
  the signer restriction already present in downloaded verification. Reuse its generated transport fixture.
- `.planning/**`-only commits skip heavy CI through classification, while hygiene/title/roll-up still report.

### Integration Points

- New digest/reporting and quality consumption belong alongside current workflows and CMake scripts; no runtime
  library, runner or libretro feature is introduced by discussion or by the reporting design.
- Full nightly BE needs same-commit native comparison and appropriate reporter dependencies, not an untracked
  artifact from the latest release. Release reusable CI and required roll-up must remain internally consistent.
- New repository checks count against the same budget regardless of directory or inline workflow placement.
</code_context>

<specifics>
## Specific Ideas and Evidence

**Current release-safety decision after05-08, 2026-10-22:** three parallel read-only advisory reviews considered
security/test validity, proportional allocation and release delivery. Further exchanges challenged allocation
completeness and404/authentication claims; they added no new capability. The security review accepted300 as a
construction constraint; the budget review preferred a more conservative384 estimate. The orchestrator selected
300 with its explicit27 reserve and complete obligations, rather than presenting the estimates as measured fit
or claiming unanimity. Another fan-out changes no decision; stop research here. Rendering/UI rewrites,
distributed services and unrelated ecosystems have no relevant phase requirement.

Fresh observations in this discussion:

- `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/discussion-release-count.json -P tests/cmake/gate_count.cmake`
  —exit0,14906 physical lines; source8580/6533. This measures the retained RED, not accepted fit or completed repair.
- `gh --version` —exit0,2.101.0. `gh api repos/cli/cli/security-advisories/GHSA-wjmr-j3rp-mh2g --jq '{html_url,published_at,vulnerabilities}'`
  and the corresponding `GHSA-4mq3-hpgx-9cx8` query —exit0 each, patched2.102.0. The release-tag API query
  —exit0,2.102.0 published2026-09-30. Official web search independently corroborates both advisories.
- `gh issue list --repo szTheory/muhtari --label digest --state open --json number,title,body,url --limit 2`
  —exit0,empty list. Default-branch publication remains a ship check.

Run URL: unavailable for these local/read-only observations. No hosted workflow was dispatched, no publisher
repair or new test was implemented, and no hosted runner's gh version or current rehearsal success is claimed.
Historical green URLs below and in accepted summaries remain dated predecessor evidence.

**Historical bounded decision after 05-12, 2026-10-22:** the owner again requested all relevant role perspectives,
primary sources, adversarial analysis and automatic adoption of coherent recommendations. Three read-only
advisory reviews covered construction/maintainability, security/test validity and delivery/governance. All
converged on D-24's consumer-owned numeric reads. Security inspected both unavailable branches and confirmed
the proposed read locations preserve rejection before numeric use. The static net-minus-one forecast has not
been implemented or tested. Returning to accepted authentication alone forecasts14,847/quality926; raising
the cap funds no newly adopted capability and would not price remaining acceptance work. No allocation change
is adopted. A broader calendar simplification was considered and excluded as unnecessary scope.

Fresh local observation:
`cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/discussion-count.json -P tests/cmake/gate_count.cmake`
returned exit0:14,842 physical lines, source8,580/6,533, at feee6a98 with the three drafts. This is a successful
measurement of a failing fit, not acceptance. `gh issue list --label digest --state open --json number,title,body,url --limit 2`
returned exit0 and no open digest. No implementation tests, hosted dispatch or acceptance ran in discussion;
run URL is unavailable. Existing 05-12 selected-test results remain historical. The earlier observations below
remain dated history, not the current count. Current source/ownership documentation is read against code;
official CMake macro semantics and Google SRE automation-cost guidance support the reasoning without setting
a repository-specific numeric limit.

**Current scope decision, 2026-10-22:** the owner requested all relevant role perspectives, adversarial review,
primary sources where useful, and automatic adoption of coherent recommendations. Three read-only advisory
reviews covered scope/architecture, security/test validity and construction/CI economics. Two recommended
explicit deferral under the fixed allocation; the security review preferred complete centralization only if
its entire remaining cost received a fixed explicit budget revision. No such measured complete cost exists.
The synthesis adopts deferral, retains the security limitations explicitly and stops further fan-out because
no remaining lens changes the bounded choices. This is not an invented interview answer.

`cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/discussion-count.json -P tests/cmake/gate_count.cmake`
returned exit 0 and observed 14,879 physical lines, source 8,580/6,533. Counter success is not budget acceptance.
`gh issue list --label digest --state open --json number,title,body,url --limit 2` returned exit 0 and an empty
list; no default-branch digest publication is inferred. No current hosted run was created; run URL unavailable
for this discussion's local measurement. Earlier measurements below remain dated historical evidence.


- Refreshed research measured 250 lines of scratch helpers/replacements plus an incomplete quality prototype;
  its 2,262-line construction excludes remaining enforcement and fails the former 857 ceiling. This supports
  reopening the delivery constraint, not claiming impossibility or choosing an invented 3,000/3,500-line cap.
  Three read-only advisory reviews covered budget/metrics, acceptance/delivery, and an independent adversarial
  challenge. They converge on explicit scoped growth, measured simplification, intact safety and bounded planning.
  No exact new numerator or permanent cap has been measured during discussion; those belong to D-19's task.

- Hosted baseline: `gh api repos/szTheory/muhtari/actions/runs/39614799435/jobs --paginate` — exit 0.
  [Run 39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435): BE execution 681 s, slowest
  native build 186 s, sanitizer job 144 s; required roll-up succeeded. These are one-run observations, not p95s.
- `gh run list -R szTheory/muhtari --workflow ci.yml --limit 40 --json ...` and the direct-main-push REST query —
  exit 0; establish sample availability, not a completed monthly efficiency audit.
- `scripts/apply-repo-settings.sh --check szTheory/muhtari` — exit 0, no drift.
- `gh issue list -R szTheory/muhtari --label digest --state open --json number,title,url` — exit 0, none open;
  same query for `nightly` — exit 0, none open. No issue was created during this discussion.
- One-off Python line inventory — exit 0: historical narrow gate set 18 files/1,367 lines, all source 8,577,
  authored source without cartdb 6,530. New/previously omitted check logic can only increase the numerator.
- [Run 39611531779](https://github.com/szTheory/muhtari/actions/runs/39611531779) timed out `audio.samples` twice;
  the archival patch provisionally raised its limit. Planning must measure the final workload, not rebrand that
  temporary allowance as a final solution.

The relevant stakeholder lenses are reconciled: maintainers get fewer reports and faster actionable failures;
players/embedders retain deterministic evidence; security keeps the untrusted-data/privilege boundary and release
identity checks; QA retains fault-specific controls; release consumers receive truthful docs and artifacts.
</specifics>

<deferred>
## Deferred Ideas

The forged-SHA fixture uses an equal-valued observation, so its existing claim of proving a smaller lane hidden
by extrema is overstated. Record this as a tooling seed under rule7, not a new acceptance requirement or an
excuse to remove its exact identity rejection. No broader test-harness redesign is adopted here.

Performance/compatibility domain-policy centralization, shared immutable grant/renewal handling and retirement
of the two legacy exception stores are explicitly deferred by D-21. No deadline, extra allocation or automatic
future execution is attached. The narrower Phase 5 requirement records the surviving exception limitations.


New emulator capabilities remain in phases 6–11 or later milestones. External dashboards, caches without a measured
payoff, broad new analysis services, extra RetroArch cases and new public APIs are not part of this tune-up.
Do not reopen the owner's one-step or automation-first rules.

### Reviewed Todos (not folded)

- `.planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md`:
  its behavior/docs findings already have phase 3 dispositions; remaining test seeds are not pipeline requirements.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md`:
  nightly input coverage must be assessed where this phase changes workflows, but cartdb reload/swap/parser and
  old header-comment findings are not automatically adopted. Reproduce current relevance first; keep dormant
  tooling seeds instead of expanding phase scope by keyword match.
</deferred>

---
*Phase: 05-pipeline-tune-up*
*Context gathered: 2026-10-21*
