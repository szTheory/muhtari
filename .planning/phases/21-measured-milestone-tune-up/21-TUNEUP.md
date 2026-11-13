# Phase 21 measured tune-up

## Current documentation path

The current project records at [PROJECT](../../PROJECT.md), [STATE](../../STATE.md),
[ROADMAP](../../ROADMAP.md), and [PRODUCT-ROADMAP](../../PRODUCT-ROADMAP.md) describe
v7.0 as the Souper homebrew mapper milestone and identify v6.0 as the latest shipped
milestone. The user-facing [README](../../../README.md) points to the version-free
latest-release downloads and the existing runner, embedding, libretro, and testing
guides. The [v6.0 ship receipt](../../milestones/v6.0-SHIP.md) records publication
of [v0.14.0](https://github.com/szTheory/muhtari/releases/tag/v0.14.0), its exact
release source, and the source-specific main-CI result. The
[v6.0 milestone audit](../../milestones/v6.0-MILESTONE-AUDIT.md) covers local
feature acceptance and preserves its platform limits.

This report reuses those records. It does not refresh GitHub state, re-download an
asset, or claim a new product, host, CI, or publication result.

## Source-qualified evidence ledger

| Evidence role | Exact source | Saved result or local command | Run URL | Evidence limit |
|---|---|---|---|---|
| Accepted v0.14.0 publication | `461dda5545bcc202fb4b90c1232fb2e9a00b6c28` | The [ship receipt](../../milestones/v6.0-SHIP.md) records `gh release view v0.14.0 --json assets` exit 0 and `gh run view 43441765548 --json databaseId,status,conclusion,headSha,url,jobs` exit 0. Publication, `publish verification`, and `install-line-latest` passed; no jobs failed or were cancelled. | [Release run 43441765548](https://github.com/szTheory/muhtari/actions/runs/43441765548) — historical receipt | Establishes the accepted release on this source only. It does not accept any later planning snapshot or reopen shipping recovery. |
| Same-source main CI and merged PR #61 | `461dda5545bcc202fb4b90c1232fb2e9a00b6c28` | The [ship receipt](../../milestones/v6.0-SHIP.md) records PR #61 merged at this source and `gh run view 43427086666 --json workflowName,conclusion,headSha,url` exit 0 with main CI passed. | [Main CI run 43427086666](https://github.com/szTheory/muhtari/actions/runs/43427086666) — historical receipt | The saved main-CI receipt does not include detailed queue, per-job duration, cancellation, or runner-minute observations. Do not borrow those from the separate release run. |
| Research-committed documentation snapshot | `815461285bfe450380058bd211927b4e916a2981` | The [Phase 21 research](21-RESEARCH.md) identifies this documentation/planning snapshot and reports no new CI receipt. | N/A — no new receipt | This source identity is not product or CI acceptance. |
| Phase 21 execution documentation baseline | `bcc53f292b699cf1618f6157ee7167c36ce49010` | `git rev-parse HEAD` exited 0 at plan start. `git status --short --branch` exited 0 and showed `milestone/v7.0-souper-mapper` plus `.planning/STATE.md` modified by the orchestrator's Phase 21 start bookkeeping. `git diff --name-only 461dda5545bcc202fb4b90c1232fb2e9a00b6c28..HEAD` exited 0 and showed planning/documentation paths and `AGENTS.md`, with no product, test, workflow, or script path. | N/A-local | This is the actual execution starting HEAD, later than the research snapshot. Its planning/docs ancestry does not establish a new CI pass; the STATE edit is workflow tracking, outside the report changes. |
| Phase 20 local gates and cost measurements | Final local-gates source `daab4c82c0bb3393b3e93ee2b23ead09236f16ec`; separately reviewed product source `5158bb649160f081f6f1253da3be3099b93c8a03` | The [Phase 20 cost report](../../milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-COST-EVIDENCE.md) records its local gate commands and results. `git diff --name-status daab4c82c0bb3393b3e93ee2b23ead09236f16ec..461dda5545bcc202fb4b90c1232fb2e9a00b6c28` exited 0 and showed implementation, test, workflow, publisher, and planning changes between the sources. | N/A-local | Retain these measurements only for their named Phase 20 sources. They are not release-source or current-branch performance measurements. |

The execution baseline comparison is a comparison of tracked commits. The dirty
STATE entry shown by `git status` came from execute-phase's `state.begin-phase`
bookkeeping before this report task started. It was not part of that
commit-to-commit comparison or this plan's report changes.

## Accepted release-run efficiency observations

The accepted [release run 43441765548](https://github.com/szTheory/muhtari/actions/runs/43441765548)
is one saved hosted sample on the published source. The first job started four
seconds after dispatch. Windows ARM64 was the longest observed job at 39m49s;
equality followed at 30m42s. There were zero failed or cancelled jobs. This
identifies the longest observed job, not a reconstructed complete DAG critical
path, a total of billed runner minutes, or a timing trend; those measurements
are unavailable here.

Merged [PR #59](https://github.com/szTheory/muhtari/pull/59) already removes up
to nine minutes of non-gating property work from PR runs while required coverage
remains. The deeper 10,000-case property workload remains for scheduled and
manual runs under its existing limits. This is a previously landed coverage-
preserving change, not a new Phase 21 edit. The same-source [main CI run
43427086666](https://github.com/szTheory/muhtari/actions/runs/43427086666) is a
separate passed receipt; its detailed timing, queue, cancellation, and runner-
minute observations are unavailable and are not inferred from the release run.

## Preserved historical failures and incomplete evidence

These records remain attributed to their original source and outcome. They are
historical evidence, not successful timing samples or instructions to retry.

| Historical run | Source identity recorded in local lesson | Outcome retained | Owning lesson |
|---|---|---|---|
| [Release run 43318681115](https://github.com/szTheory/muhtari/actions/runs/43318681115) | Exact release SHA `8d73321953f417dc973a4cb93cfb4b8521772150` | Equality cancelled at its 30-minute deadline; not acceptance or a timing sample. | [Resume publication after a release gate is cancelled](../../LESSONS.md#resume-publication-after-a-release-gate-is-cancelled-2026-11-12) |
| [Release run 43330805540](https://github.com/szTheory/muhtari/actions/runs/43330805540) | Exact PR head `061da62fbdceac720ce30bd9be8535d1bfe4fffe` | Intel macOS job cancelled at its 19-minute cap after its tests and package checks passed; the run remains cancelled history. | [Intel macOS CI needs a separate job budget](../../LESSONS.md#intel-macos-ci-needs-a-separate-job-budget-2026-11-12) |
| [Release run 43342898744](https://github.com/szTheory/muhtari/actions/runs/43342898744) | Draft target `8d73321953f417dc973a4cb93cfb4b8521772150`; full recovery workflow head is not recorded in the local lesson. | Early recovery failed before CI/equality because draft lookup received HTTP 403. | [Give release recovery the token scopes its draft requires](../../LESSONS.md#give-release-recovery-the-token-scopes-its-draft-requires-2026-11-13) |
| [Release run 43355803935](https://github.com/szTheory/muhtari/actions/runs/43355803935) | Draft target remained unchanged; exact workflow-dispatch head is not recorded in the local lesson. | Recovery target succeeded while CI, equality, publish, and latest-install descendants were skipped; the draft remained unchanged. | [Override skipped prerequisites without defeating cancellation](../../LESSONS.md#override-skipped-prerequisites-without-defeating-cancellation-2026-11-13) |
| [Release run 43367755963](https://github.com/szTheory/muhtari/actions/runs/43367755963) | Lesson records target `8d73321`, caller parent `6abadfd`, and expected target parent `77d9bec` in abbreviated form. | Wrong source baseline caused the quality gate to fail and publication to remain skipped; not retried. | [Reusable workflow checkout ref and dispatch baseline](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13) |
| [PR run 43375527959](https://github.com/szTheory/muhtari/actions/runs/43375527959) | Full run-head SHA is not recorded in the local lesson. | Windows quality control exposed fixture-fidelity failure under Windows-style checkout settings. | [Reusable workflow checkout ref and dispatch baseline](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13) |
| [PR run 43378704058](https://github.com/szTheory/muhtari/actions/runs/43378704058) | Full run-head SHA is not recorded in the local lesson. | Windows x64 quality control failed its producer-environment assertion. | [Reusable workflow checkout ref and dispatch baseline](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13) |
| Nightly PR runs [43375528130](https://github.com/szTheory/muhtari/actions/runs/43375528130) and [43378704172](https://github.com/szTheory/muhtari/actions/runs/43378704172) | Same PR context; individual run-head SHAs are not recorded in the local lesson. | Both deep property jobs were incomplete at the nine-minute job cap: the first timed out cases and the second was cancelled. | [Reusable workflow checkout ref and dispatch baseline](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13) |
| [WASM CI run 43381459806](https://github.com/szTheory/muhtari/actions/runs/43381459806) | Full run-head SHA is not recorded in the local lesson. | Shared policy-test registry update was missing from the WASM32 inventory, so configure failed; this was not a product pass. | [Reusable workflow checkout ref and dispatch baseline](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13) |

Where a saved local lesson omits a run-head SHA, this report preserves the
available target, caller relationship, run ID, and outcome and marks the missing
identity. It does not infer source hashes or issue a hosted query to fill them.

The [Phase 20 cost report](../../milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-COST-EVIDENCE.md)
also retains its early stress-topology failure, malformed stale-tick failure,
early sanitizer startup failure, and unqualified GCC13 no-FP compile failure.
Its later source-qualified passes do not erase those earlier results.

## Six quality dimensions

These are qualitative reviews of the evidence available to this report, not new
gates or product quality metrics. Each score describes confidence in the current
maintenance baseline and links to the source it uses.

| Dimension | Score | Evidence and rationale | Limit |
|---|---:|---|---|
| Performance | 2/5 | The accepted [release receipt](../../milestones/v6.0-SHIP.md) gives one source-specific hosted timing observation; the [Phase 20 cost report](../../milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-COST-EVIDENCE.md) gives detailed local measurements on a different product source. | Weakest evidence dimension: there is no comparable completed release-source series, full DAG critical-path calculation, or billed-runner total. This low score reflects limited evidence, not a measured performance regression. |
| CI/CD | 4/5 | The [release run](../../milestones/v6.0-SHIP.md) and separate [same-source main CI receipt](https://github.com/szTheory/muhtari/actions/runs/43427086666) both passed on `461dda5545bcc202fb4b90c1232fb2e9a00b6c28`; [historical lessons](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13) preserve recovery, source-baseline, Windows fixture, and inventory faults and their corrections. | The later documentation source has no new CI receipt. The saved main-CI receipt lacks timing and cancellation detail; this score does not imply every earlier attempt passed. |
| Ease of use | 4/5 | The [README](../../../README.md) provides a version-free latest-release install/download path, RetroArch and runner quick starts, and a concise docs ladder; the [libretro guide](../../../docs/libretro.md) documents host behavior. | No install, physical audition, or live settings check was performed for this report. |
| Architecture | 4/5 | The project retains its [three-deliverable, deterministic-core contract](../../../AGENTS.md) and existing [CMake workflow presets](../../../CMakePresets.json); this report adds no runtime, dependency, or check layer. | This report-only review does not re-evaluate product architecture or prove physical hardware conformance. |
| Docs/README | 4/5 | Current [project](../../PROJECT.md), [roadmap](../../ROADMAP.md), [product roadmap](../../PRODUCT-ROADMAP.md), and [ship receipt](../../milestones/v6.0-SHIP.md) agree on the latest shipped version and next milestone; the [README](../../../README.md) uses version-free release links. | The README install line and live release state were not executed or refreshed here. |
| Upgrade path | 4/5 | [`pins.txt`](../../../pins.txt) records versions, URLs, and SHA-256 values; [maintainer documentation](../../../docs/maintaining.md) describes verified pin updates and the existing Dependabot path, with [Dependabot configuration](../../../.github/dependabot.yml) checked into the repository. | External package freshness and vulnerabilities were not queried; local pins do not establish current external versions or repository settings. |

Performance is the weakest dimension because the current evidence cannot form a
like-for-like product timing series: the accepted hosted sample is singular, and
the detailed local Phase 20 cost source differs from the published source in
implementation, tests, and workflows. That evidence gap supports preserving
current limits; it does not establish that the product regressed.

## Proportional maintenance disposition

The [SEED-009 review](../../seeds/SEED-009-proportional-maintenance-review.md)
calls for examining delivery friction, false or misattributed evidence, repeated
changes, dependency update burden, and surviving quality bounds. The saved
[lessons](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13)
record corrected recovery-token, skipped-descendant, source-baseline, Windows
fixture/assertion, and shared-inventory failures while retaining their original
failed or incomplete run evidence. These were distinct workflow/evidence issues;
the accepted release and main-CI outcomes remain tied to their own exact source.
No new repeated maintenance obstruction or required defect is demonstrated by
the accepted receipts reviewed here.

The local [pin inventory](../../../pins.txt), [update runbook](../../../docs/maintaining.md),
and [Dependabot configuration](../../../.github/dependabot.yml) provide the
existing reviewable update path. No external freshness or vulnerability query
was run. With one accepted release timing sample, missing main-CI details, and
different-source local measurements, there is no newly demonstrated low-risk
correction that preserves equivalent fault detection.

This review does not revive maintenance LOC quotas or allocations, create a
replacement metric system, or reinterpret the historical archive cutoff as a
current obligation. [DEC.10 and ENGINEERING §11](../../preparation/DECISIONS.md)
keep contract-based architecture review and finite review/gap limits; [DEC.61](../../preparation/DECISIONS.md)
retired only the old compressed archive cutoff while retaining authenticated
measurements and package integrity.

## Preserved quality and authority boundaries

This report changes no product, test, dependency, workflow, timeout, cache,
quality floor, or source-evidence producer. Existing quality floors remain 605
native tests, 12 ABI tests, 4 fuzz tests, 3 title tests, 24 hygiene tests, and
593 sanitizer tests; existing ceilings remain zero ABI findings, warnings,
analyzer findings, and sanitizer findings in [`quality.json`](../../../quality.json).
The performance and compatibility judges keep their separate exception
authorities in `tests/perf/allow.txt` and `tests/compat/allow.txt`, as described
in [ENGINEERING §8](../../preparation/ENGINEERING.md). Required/advisory
membership, named pass/fault controls, package-integrity checks, and the
authenticated actual-current/base three-ZIP measurements stay as configured.
Deadlines, cache policy, workflow topology, and jobserver/workload bounds also
remain unchanged. This report removes or weakens no check, and no product build
or test suite was run for this documentation review. The existing limit of at most two
code-review fix rounds and one gap-closure round also remains unchanged
([AGENTS.md](../../../AGENTS.md)).

All nine deferred v3.0 requirements remain deferred, and every original Phase
12/14 denied, spent, expired, and no-retry scope stays binding ([current
STATE](../../STATE.md), [v3.0 audit](../../milestones/v3.0-MILESTONE-AUDIT.md),
and [v6.0 requirements archive](../../milestones/v6.0-REQUIREMENTS.md)). This
report authorizes no host/device check, hosted query or dispatch, publication,
push, merge, or shipping retry. No product build or test run is justified by
the unchanged product/test/workflow bytes; these one-off document checks do
not establish product acceptance.

## Verification records

The following local commands were run for source identity or report review;
all URLs are N/A-local. Historical hosted results remain separate receipts.

| Command | Result | Scope |
|---|---|---|
| `test -f .planning/milestones/v6.0-SHIP.md`; `test -f .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-COST-EVIDENCE.md` | Both exit 0. | Read-only task precondition. |
| `git cat-file -e '461dda5545bcc202fb4b90c1232fb2e9a00b6c28^{commit}'`; `git cat-file -e 'daab4c82c0bb3393b3e93ee2b23ead09236f16ec^{commit}'` | Both exit 0. | Accepted-release and Phase 20 local-cost objects resolve in this checkout. |
| `git rev-parse HEAD` | Exit 0: `bcc53f292b699cf1618f6157ee7167c36ce49010` at plan start. | Execution documentation baseline before report commits. |
| `git status --short --branch` | Exit 0: `milestone/v7.0-souper-mapper`; a pre-existing `.planning/STATE.md` modification was present. | This plan did not stage or change STATE. |
| `git diff --name-only 461dda5545bcc202fb4b90c1232fb2e9a00b6c28..HEAD` | Exit 0. | Listed planning/docs and `AGENTS.md` paths only; no product, test, workflow, or script path. |
| `git diff --name-status daab4c82c0bb3393b3e93ee2b23ead09236f16ec..461dda5545bcc202fb4b90c1232fb2e9a00b6c28` | Exit 0. | Showed changed implementation, test, workflow, publisher, and planning paths; Phase 20 local measurements are not release-source measurements. |
| `test -s .planning/phases/21-measured-milestone-tune-up/21-TUNEUP.md` | Exit 1 before creation; exit 0 after Task 1. | Expected baseline absence, then report presence. |
| `node -e 'const f=require("fs"),p=require("path"),d=".planning/phases/21-measured-milestone-tune-up",s=f.readFileSync(p.join(d,"21-TUNEUP.md"),"utf8");for(const m of s.matchAll(/\[[^\]]*\]\(([^)]+)\)/g)){const r=m[1];if(!/^https?:\/\//.test(r)&&!r.startsWith("mailto:")&&!f.existsSync(p.resolve(d,r.split("#")[0]||".")))process.exitCode=1}'` | Exit 0. | One-off check that report-relative local Markdown targets exist; lesson heading fragments were reviewed against their source headings. |
| `if test -s .planning/phases/21-measured-milestone-tune-up/21-TUNEUP.md; then git diff --no-index --check /dev/null .planning/phases/21-measured-milestone-tune-up/21-TUNEUP.md; task_report_check=$?; test "$task_report_check" -le 1; else false; fi` | Wrapper exit 0; the clean new-file `git diff --no-index --check` result is expected to be 1, with no whitespace diagnostics. | One-off document whitespace check only. |
| `test -s .planning/phases/21-measured-milestone-tune-up/21-TUNEUP.md`; report-relative Markdown target scan; specified `git diff --no-index --check` wrapper; `git diff --check -- .planning/phases/21-measured-milestone-tune-up/21-TUNEUP.md` | All command exits 0 after the clarification; the wrapper's clean no-index diff returned its expected 1, with no whitespace diagnostics. | One-off recheck of the corrected report only; N/A-local. |

Historical hosted receipts reused here: [v0.14.0 Release run 43441765548](https://github.com/szTheory/muhtari/actions/runs/43441765548)
and [same-source main CI run 43427086666](https://github.com/szTheory/muhtari/actions/runs/43427086666).
These URLs are cited from the saved ship receipt and were not queried again. The
prior failed/cancelled/incomplete runs above are likewise reused history, not
new measurements. No current-branch CI, full DAG critical path, billed-minute
total, comparable release-source timing series, external dependency freshness,
host, physical audition, or live settings result is established by this report.

## Measured disposition

The bounded disposition is **no change beyond this organizing report**. Current
release and project documentation agree on the shipped v0.14.0 baseline. The one
accepted hosted release sample does not establish that another required check is
safe to cut, and the Phase 20 local measurements come from different product,
test, and workflow sources, so they cannot supply a comparable release-source
series. Existing evidence supports recording the limits rather than changing
timeouts, caches, topology, or required coverage.

Revisit this conclusion when comparable completed measurements on the same host
and source identify redundant work with equivalent fault detection. If materially
new evidence contradicts this recommendation or reveals a required defect, record
that source and return a scope checkpoint before any corrective implementation
or new workload; do not silently broaden this report plan.

The plan's spec-less TUNE-11 probe remains `unclassified` and `unresolved`, with
verification, resolution, and reason unset. This report preserves that planning
assumption without claiming to resolve it.
