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
| Phase 21 execution documentation baseline | `bcc53f292b699cf1618f6157ee7167c36ce49010` | `git rev-parse HEAD` exited 0 at plan start. `git status --short --branch` exited 0 and showed `milestone/v7.0-souper-mapper` plus a pre-existing modified `.planning/STATE.md`. `git diff --name-only 461dda5545bcc202fb4b90c1232fb2e9a00b6c28..HEAD` exited 0 and showed planning/documentation paths and `AGENTS.md`, with no product, test, workflow, or script path. | N/A-local | This is the actual execution starting HEAD, later than the research snapshot. Its planning/docs ancestry does not establish a new CI pass; the pre-existing STATE edit is outside this report. |
| Phase 20 local gates and cost measurements | Final local-gates source `daab4c82c0bb3393b3e93ee2b23ead09236f16ec`; separately reviewed product source `5158bb649160f081f6f1253da3be3099b93c8a03` | The [Phase 20 cost report](../../milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-COST-EVIDENCE.md) records its local gate commands and results. `git diff --name-status daab4c82c0bb3393b3e93ee2b23ead09236f16ec..461dda5545bcc202fb4b90c1232fb2e9a00b6c28` exited 0 and showed implementation, test, workflow, publisher, and planning changes between the sources. | N/A-local | Retain these measurements only for their named Phase 20 sources. They are not release-source or current-branch performance measurements. |

The execution baseline comparison is a comparison of tracked commits. The dirty
STATE entry shown by `git status` was not part of that commit-to-commit comparison
and is not included in this plan's report changes.

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

| Historical record | Outcome retained | Owning lesson |
|---|---|---|
| [Release run 43318681115](https://github.com/szTheory/muhtari/actions/runs/43318681115) on `8d73321953f417dc973a4cb93cfb4b8521772150` | Equality cancelled at its 30-minute deadline; not acceptance or a timing sample. | [Resume publication after a release gate is cancelled](../../LESSONS.md#resume-publication-after-a-release-gate-is-cancelled-2026-11-12) |
| [Release run 43330805540](https://github.com/szTheory/muhtari/actions/runs/43330805540) | Intel macOS job cancelled at its 19-minute cap after its tests and package checks passed; the run remains cancelled history. | [Intel macOS CI needs a separate job budget](../../LESSONS.md#intel-macos-ci-needs-a-separate-job-budget-2026-11-12) |
| [Release run 43342898744](https://github.com/szTheory/muhtari/actions/runs/43342898744) | Early recovery failed before CI/equality because draft lookup received HTTP 403. | [Give release recovery the token scopes its draft requires](../../LESSONS.md#give-release-recovery-the-token-scopes-its-draft-requires-2026-11-13) |
| [Release run 43355803935](https://github.com/szTheory/muhtari/actions/runs/43355803935) | Recovery target succeeded while CI, equality, publish, and latest-install descendants were skipped; the draft remained unchanged. | [Override skipped prerequisites without defeating cancellation](../../LESSONS.md#override-skipped-prerequisites-without-defeating-cancellation-2026-11-13) |
| [Release run 43367755963](https://github.com/szTheory/muhtari/actions/runs/43367755963) | Wrong source baseline caused the quality gate to fail and publication to remain skipped; not retried. | [Reusable workflow checkout ref and dispatch baseline](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13) |
| [PR run 43375527959](https://github.com/szTheory/muhtari/actions/runs/43375527959) | Windows quality control exposed fixture-fidelity failure under Windows-style checkout settings. | [Reusable workflow checkout ref and dispatch baseline](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13) |
| [PR run 43378704058](https://github.com/szTheory/muhtari/actions/runs/43378704058) | Windows x64 quality control failed its producer-environment assertion. | [Reusable workflow checkout ref and dispatch baseline](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13) |
| Nightly PR runs [43375528130](https://github.com/szTheory/muhtari/actions/runs/43375528130) and [43378704172](https://github.com/szTheory/muhtari/actions/runs/43378704172) | Both deep property jobs were incomplete at the nine-minute job cap: the first timed out cases and the second was cancelled. | [Reusable workflow checkout ref and dispatch baseline](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13) |
| [WASM CI run 43381459806](https://github.com/szTheory/muhtari/actions/runs/43381459806) | Shared policy-test registry update was missing from the WASM32 inventory, so configure failed; this was not a product pass. | [Reusable workflow checkout ref and dispatch baseline](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13) |

The [Phase 20 cost report](../../milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-COST-EVIDENCE.md)
also retains its early stress-topology failure, malformed stale-tick failure,
early sanitizer startup failure, and unqualified GCC13 no-FP compile failure.
Its later source-qualified passes do not erase those earlier results.

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
