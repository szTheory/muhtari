# Phase 5: Pipeline tune-up - Context

**Gathered:** 2026-10-21
**Status:** Ready for planning

<domain>
## Phase Boundary

Deliver TUNE-01–05 from `.planning/REQUIREMENTS.md`: weekly triage and monthly hosted CI-efficiency reporting,
measured reliable CI, one quality ratchet within the existing gate-code budget, controlled performance arming,
and release/docs/settings freshness with a shipped improvement to the weakest quality dimension.

This is discussion, not implementation or phase verification. The owner explicitly requested consideration of
all relevant perspectives, primary-source research, adversarial analysis and coherent recommendations, and
instructed the agent to follow those recommendations. Decisions below are agent-selected under that delegation;
they are not invented answers to an interview. No additional capability or new GSD step is authorized here.

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

- **D-09:** One committed root `quality.json` owns quality bounds and exceptions, and one aggregate `quality`
  check consumes existing outputs under `CI required`. Use CMake `string(JSON)`, not a new language or platform.
  Reuse existing semantic validators for hashes, compatibility, ABI, sanitizers and performance; do not rebuild
  their judges inside a framework. Consolidate duplicate threshold/exception stores when migrating a metric,
  retaining original semantic evidence and controls. Report units, direction, producer, fixture/configuration,
  commit and availability for each metric. Baseline only what was actually measured.
- **D-10:** Compare policy changes with the trusted PR base: floors cannot fall, ceilings cannot rise, metrics
  cannot disappear, and new thresholds cannot legitimize the same PR's regression silently. A loosening needs
  the corresponding key/from/to, reason, date and bounded expiry (at most 60 days), visible in review and reports;
  expired or unrelated reasons do not authorize it. Tightening is free. A measured value must satisfy the effective
  bound too: comparing two JSON policies alone proves nothing. Missing/malformed required evidence fails; a
  named advisory/unavailable metric has no fabricated passing value. Validate boundary, missing-data, expired-
  exception and baseline-tampering cases using the existing negative-control pattern.
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
  version inputs and all source affecting archives. A push without a change to one of four trigger paths is NOT
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
- **D-17:** Meet the existing tenth-of-src gate-code budget through removal of duplicate work and shared existing
  primitives. Do not relax the ratio, pad `src/`, compress readable code into line golf, move enforcement out of
  the inventory, or rename it a test driver to hide it. Preserve safety-critical firmware/archive, identity,
  privacy, deterministic-core and release checks. Count every repository-check implementation, including existing/new embedded
  workflow logic and moved implementations, and publish the file inventory plus broad sensitivity count. Use
  the existing full `src/*.c`/`src/*.h` denominator consistently, also showing the authored-only denominator so
  generated cartdb data cannot conceal the proportion. This session reproduced the historical narrow baseline:
  1,367/8,577 = 15.94%; without `src/cartdb.c`, 1,367/6,530 = 20.93%. At the unchanged full denominator the
  maximum whole-line budget is 857; new checks must pay for themselves within it. Formal planning must establish
  a feasible simplification path with every retained fault mapped, before execution adds more enforcement.
- **D-18:** Keep work small and cohesive, prioritize the reproduced faults and simplification, and stop expanding
  analysis when another lens changes no decision or verification. Security, reliability, maintainer cost,
  embedder/player compatibility, test validity and release consumers are relevant; UI design, rendering-engine
  rewrites, distributed services and unrelated language ecosystems add no phase requirement. Do not reopen
  settled emulator choices or turn all dormant review seeds into mandatory work. Export lessons in the existing
  pasteable template only when asked; do not start an export workflow as a second GSD step.

### Agent's Discretion

Internal schema names, helper boundaries, batching/API page sizes, readable report formatting, per-test cost
values and the exact simplifications are researcher/planner choices within these contracts. The numbered
choices are coherent outcomes, not a task breakdown; formal phase research still runs. No dependency addition
is justified merely by a convenient parser or reporting abstraction.

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
  TUNE-01–05, phase goal and historical exceptions; no phase implementation completed yet.
- `.planning/preparation/README.md` — reference index.
- `.planning/preparation/ENGINEERING.md` §§3–9, 11–13 — inventory, CI, release, reporting, ratchet and budgets.
- `.planning/preparation/DECISIONS.md` DEC.07–15, DEC.43–54, DEC.56 — locked process and portability flip conditions.
- `.planning/seeds/SEED-007-milestone-tune-up.md` — recurring scope and historical gate inventory; do not rewrite
  its dormant status as part of this discussion.
- `.planning/RETROSPECTIVE.md`, `.planning/LESSONS.md` — local evidence versus inbound hypotheses.
- `.planning/milestones/v1.0-phases/04-sound/deferred-items.md` — reproduced compatibility-output race.
- `.planning/milestones/v1.0-MILESTONE-AUDIT.md` — inherited evidence gaps; do not erase accepted history.
- The four full todo paths in Folded Todos above, and the two paths in Reviewed Todos below.

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

No external emulator source was opened. Existing preparation already records sibling release/CI precedents;
no new framework/ecosystem dependency is warranted by this phase.
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
- Test card perf is report-only; WASM/library are outside `CI required`; nightly property testing is advisory.
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
