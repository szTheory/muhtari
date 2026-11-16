# Phase 26: Measured milestone tune-up

**Requirement:** TUNE-14<br>
**Disposition:** Correct the active v9 chronology and preserve the current workflow limits. No pipeline change is supported by this evidence.

## Source-qualified release and CI evidence

All hosted observations below are read-only GitHub Actions or Releases metadata captured on 2026-11-16. Run outcomes remain attached to their exact head SHA. Run-level queue time is `run_started_at - created_at`; it was zero for each listed workflow. Runner dispatch begins a few seconds later and is reported separately where it affects the critical path.

| Source and evidence | Command or workflow | Outcome, timing and limit |
|---|---|---|
| Published stable [v0.16.0](https://github.com/szTheory/muhtari/releases/tag/v0.16.0), exact source `a0319a9c29886d776e37b818d38d36fe810f486d` | Hosted CI: `cmake --workflow --preset ci`; [run 43801428495](https://github.com/szTheory/muhtari/actions/runs/43801428495) | Success. Started 04:00:20Z, finished 04:44:03Z; queue 0s, elapsed 43m43s. Windows ARM64 39m42s was the longest build, followed by dependent WASM32 3m48s. No jobs were cancelled. |
| Exact-source release for the same published tag and SHA | Release workflow; [run 43801428444](https://github.com/szTheory/muhtari/actions/runs/43801428444) | Success, including release CI, equality, publication and latest-install. Started 04:00:20Z, finished 04:52:43Z; queue 0s, elapsed 52m23s. The longest chain was Windows ARM64 41m52s → WASM32 3m16s → publish 6m31s → latest-install 8s. Equality took 16m45s in parallel. No jobs were cancelled. |
| Scheduled report-only Nightly on `a0319a9c29886d776e37b818d38d36fe810f486d` | `Nightly` workflow; [run 43803812599](https://github.com/szTheory/muhtari/actions/runs/43803812599) | Cancelled. Its `prop` job's 10,000-cases-per-property step was cancelled at the configured 9-minute job limit (job elapsed 555s); the follow-up “Every test ran” check failed because the property workload was incomplete. Other listed Nightly jobs passed. This is not required CI or release evidence, and the run was not retried. |
| Archive merge PR #75, source `bac988bea6a4ec070943c11b83298ac1eee5a8c3` | Hosted CI: `cmake --workflow --preset ci`; [run 43854355858](https://github.com/szTheory/muhtari/actions/runs/43854355858) | Success. Started 11:03:38Z, finished 11:46:42Z; queue 0s, elapsed 43m04s. Windows ARM64 took 40m34s, followed by dependent WASM32 2m14s. No jobs were cancelled. |
| Archive merge Release workflow on the same source | Release workflow; [run 43854356086](https://github.com/szTheory/muhtari/actions/runs/43854356086) | Success in 16s; package work was skipped because no new version was due. This is not a new publication. |
| Current main after state-update PR #76, source `89b6ff82812cd5be873e9f90d32ac55fded8694b` | Hosted CI: `cmake --workflow --preset ci`; [run 43866551295](https://github.com/szTheory/muhtari/actions/runs/43866551295) | Success. Started 12:26:12Z; last job finished 13:12:46Z (46m34s from run start); GitHub finalized the run at 13:12:47Z. Queue 0s. Windows ARM64 took 42m59s, followed by dependent WASM32 3m23s. Windows x64 took 26m26s, macOS x64 17m43s and ASAN 11m41s. No jobs were cancelled. |
| Current-main Release workflow on the same source | Release workflow; [run 43866551368](https://github.com/szTheory/muhtari/actions/runs/43866551368) | Success in 18s; `release-target` passed and package, equality and latest-install work was skipped because no new release was created. This is not a new publication. |

The immediate archive and current-main CI receipts both started without queue delay and show the same path: Windows ARM64 completes the build matrix, then WASM32 runs. Their ARM64 times were 40m34s and 42m59s. That 2m25s spread across two observations does not establish a regression. There is no per-case profile identifying safe work to remove, cache or parallelize.

## Active record reconciliation

The v0.16.0 publication remains attached to its exact source and release receipt. Phase closeout PR #74 has its earlier main CI [43841380444](https://github.com/szTheory/muhtari/actions/runs/43841380444) and Release [43841380334](https://github.com/szTheory/muhtari/actions/runs/43841380334) receipts. Archive PR #75 followed with source `bac988bea6a4ec070943c11b83298ac1eee5a8c3`; post-archive state PR #76 established the current main source `89b6ff82812cd5be873e9f90d32ac55fded8694b`. The active [project](../../PROJECT.md), [milestones](../../MILESTONES.md), [roadmap](../../ROADMAP.md) and [product roadmap](../../PRODUCT-ROADMAP.md) now preserve that order and link the source-specific receipts. Earlier v9 archive-phase records remain historical.

## Lessons and cheap checks

The applicable measured-margin rule is recorded in [LESSONS.md](../../LESSONS.md) as MU-CI-021: use completed, comparable lane samples and keep a censored timeout separate from a completed duration. XFER-012 and the Phase 23 tune-up apply the same discipline to cancelled workflow evidence and workflow structure. The prior tune-up retained the 10,000-case Nightly job as report-only and did not change its cap without evidence that isolates a safe correction. The present cancellation is consistent with that bounded disposition; it does not justify changing a required gate.

This phase changes planning prose only. It changes no source, ROM fixture, test, CMake inventory, shared registry or CI workflow. Cheap local checks were run before treating any longer workload as relevant:

| Check | Exit | What it establishes |
|---|---:|---|
| `git diff --check` after the working-tree corrections | 0 | No whitespace errors in tracked working-tree changes. The one-off validator covers new planning files too. |
| `git diff --check origin/main...HEAD -- .planning/research/next-milestone/2026-11-16-future-roadmap-review.md` before correction | 2 | Reproduced two trailing-space Markdown hard breaks in the saved successor review. They were replaced with explicit `<br>` markers without changing words or citations. The post-commit rerun is recorded below after it completes. |
| `scripts/hygiene.sh --tree` | 0 | The complete current worktree satisfies the repository's hygiene rules. |
| `gsd_run query validate.consistency --raw` using `@opengsd/gsd-core` 1.20.0 | 0 | Active roadmap and requirement records are structurally consistent. |
| `gsd_run query roadmap.analyze --raw` using `@opengsd/gsd-core` 1.20.0 | 0 | Phase order and requirement coverage remain coherent. |
| One-off Phase 26 validator specified in [26-01-PLAN.md](26-01-PLAN.md) | 0 | Its clean control passed and trailing-space control failed as intended; 13 changed planning files are clean; all six expected run IDs appear in the four active summaries; all seven research Actions URLs appear in this report; six relative path-only links resolve. Two earlier runs of the validator exposed the review and report metadata whitespace issues; both were corrected before this passing run. |

No CMake build or test suite was needed: the phase changes no behavior, test, fixture, inventory or workflow. No hosted dispatch or retry, RetroArch session, physical hardware check, release mutation, API/state compatibility change or hardware claim was made.

## Bounded no-change decision

The exact-source release and current required main CI are green. Required CI has no cancelled jobs in the reviewed receipts; the sole cancellation here is the report-only Nightly property workload at its existing cap. Preserve every test, positive and rejection control, required/advisory membership, job limit, cache rule and workflow topology. Make no timeout, cache, parallelism, resource-limit or coverage change. Do not retry the scheduled Nightly run.

Revisit only if a required-source fault reproduces, or a larger comparable completed sample and per-case profile identify redundant work while retaining equivalent fault detection. Reopen the Nightly cap only with a specific question and a fresh authorized remote window. Existing Phase 12/14 host and remote stops, the nine deferred v3.0 requirements, the unpublished v0.15.0 source boundary and GameDrive NO-GO remain unchanged.
