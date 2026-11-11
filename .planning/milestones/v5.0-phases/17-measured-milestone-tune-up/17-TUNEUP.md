# Phase 17 tune-up evidence

## Publication correction

The kickoff correction updates current publication records to point to the isolated v4.0 source: [PR #47](https://github.com/szTheory/muhtari/pull/47) merged, [release PR #48](https://github.com/szTheory/muhtari/pull/48) merged, and [v0.12.0](https://github.com/szTheory/muhtari/releases/tag/v0.12.0) published with 19 native assets. The earlier v4.0 ship-preparation entries remain historical checkpoints; the last section of the [ship receipt](../../v4.0-SHIP.md) states the current outcome. Its earlier [PR #46](https://github.com/szTheory/muhtari/pull/46) entry remains a separate failed combined-source attempt, including the held-input capture failure. PR #46 is not the shipped source and its result is not overwritten.

| Record | Current navigation or status | Local target resolves |
|---|---|---|
| [PROJECT](../../../PROJECT.md) | Current milestone is v5.0; v4.0 publication and nine deferred v3.0 requirements are stated separately. | `milestones/v4.0-MILESTONE-AUDIT.md`; `PRODUCT-ROADMAP.md` |
| [ROADMAP](../../../ROADMAP.md) | v4.0 is shipped via PR #47 and v0.12.0; the v5.0 phase/acceptance boundary and original host stops remain explicit. | `milestones/v4.0-MILESTONE-AUDIT.md`; `milestones/v4.0-ROADMAP.md`; `milestones/v4.0-SHIP.md`; `research/v5.0/SUMMARY.md` |
| [STATE](../../../STATE.md) | Current focus is Phase 17; v4.0 publication, PR #46 separation, and the Phase 12/14 stops are recorded. | `PROJECT.md`; `REQUIREMENTS.md`; `ROADMAP.md`; `milestones/v4.0-SHIP.md`; `PRODUCT-ROADMAP.md`; `milestones/v4.0-MILESTONE-AUDIT.md`; `debug/workspace-reconciliation-2026-11-10.md` |
| [Product roadmap](../../../PRODUCT-ROADMAP.md) | Immediate v5.0 bundle work follows the published v4.0 core; no browser/device or old host acceptance is claimed. | `milestones/v4.0-SHIP.md`; `research/v5.0/SUMMARY.md`; referenced seed records |
| [Prompt coverage](../../../preparation/PROMPT-COVERAGE.md) | Rows 60–62 preserve deferred host scope and authorize local evidence gathering without renewing it. | Local homes are recorded in the linked planning and project files. |

This is the already landed kickoff correction; this plan adds no second publication or pipeline change. The report links to the controlling records above rather than copying their mutable status prose.

## Saved receipt qualification

The audits below read the original files in ignored `build/milestone-preflight/`; none was edited or projected into a synthetic receipt.

| Source | Recorded identity | Audit and outcome |
|---|---|---|
| [Candidate CI](https://github.com/szTheory/muhtari/actions/runs/42918085281) | Attempt 1; head `3654cbc4071d14431c31e8bb2b5b15d182928b7a`; completed/success. | `cmake -DINPUT=build/milestone-preflight/v4-candidate-ci.json -DEXPECTED_SHA=3654cbc4071d14431c31e8bb2b5b15d182928b7a -P tools/check_ci_run.cmake` — exit 0; exactly one `CI required` aggregate passed. |
| [Release-PR CI](https://github.com/szTheory/muhtari/actions/runs/42923477030) | Attempt 2; head `4bfbb4403b17453092ee4adfc73f1ae7ee6ade4b`; completed/success. | `cmake -DINPUT=build/milestone-preflight/v4-release-pr-ci.json -DEXPECTED_SHA=4bfbb4403b17453092ee4adfc73f1ae7ee6ade4b -P tools/check_ci_run.cmake` — exit 0; exactly one `CI required` aggregate passed. |
| [Release v0.12.0](https://github.com/szTheory/muhtari/actions/runs/42931446855) | Attempt 2; head `363b2e338f69ad7a71c4c7df93f76eea6e6fed0a`; completed/success. | A one-off assertion against the original `v4-release.json` checked that exact head, attempt, URL, run completion, one successful `release-ci / CI required` aggregate, and valid terminal job outcomes — exit 0. The unchanged checker expects the CI aggregate name `CI required`, so this release-specific name was qualified directly instead of renaming the receipt. Non-success jobs: `ci`, `install-line`, `cleanup`, `release-ci / title`, `release-ci / perf`, `equality`, and `rehearsal` were all event-skipped. They are not reported as executed checks. |
| [Candidate Nightly](https://github.com/szTheory/muhtari/actions/runs/42918085345) | Attempt 1; head `3654cbc4071d14431c31e8bb2b5b15d182928b7a`; run cancelled. | Saved receipt only; the property job was cancelled and its downstream report job skipped. It does not add acceptance to the candidate CI result. |

These are separate source-specific outcomes: candidate CI and release-PR CI pass their own required aggregate; the published release has its own successful `release-ci / CI required`; the candidate Nightly is censored. Earlier combined-source and host failures remain in the [v4.0 ship receipt](../../v4.0-SHIP.md). No new remote read, dispatch, rehearsal, publication, merge, or host/device action was used. Run URLs are the links above; the receipt files remain ignored local evidence.

## Six quality dimensions

Scores are agent judgments about the current evidence and usability, not measured product grades. The [current record set](../../../ROADMAP.md) now points to the shipped source; the correction addresses the specific stale-status gap below.

| Dimension | Score | Evidence and limit |
|---|---:|---|
| Performance | 4/5 | The [performance checks](../../../../.github/workflows/ci.yml) and [Nightly benchmarks](../../../../.github/workflows/nightly.yml) remain present, and their saved candidate jobs pass. These receipts do not contain comparable frame-throughput values, so they justify no performance-budget change. |
| CI/CD | 4/5 | Three distinct candidate, release-PR, and release sources pass their own required aggregate. The cancelled Nightly property job stays visible. The saved responses lack run creation timestamps, so queue time cannot be calculated. |
| Ease of use | 4/5 | The [README](../../../../README.md) has direct RetroArch install instructions and a runner quick start; [testing docs](../../../../docs/testing.md) separate the common local lanes. This phase did not perform a real install or host check. |
| Architecture | 4/5 | The existing [three-deliverable contract](../../../../AGENTS.md) and CMake workflow remain clear and were reused without source or dependency changes. No architecture change is proposed. |
| Docs/README | 3/5 | This is the weakest observed dimension: the current publication status needed the kickoff correction because earlier pending/failed checkpoints could be read as current. Current links now resolve to PR #47, release PR #48, and v0.12.0, while the historical checkpoints remain intact. No recurring prose test is justified by this one corrected issue. |
| Upgrade path | 4/5 | [`pins.txt`](../../../../pins.txt) records dependency identities; [Dependabot policy](../../../../.github/dependabot.yml) groups minor/patch updates and leaves majors for review. No external version, vulnerability, runner announcement, or live settings check was requested or inferred. |

The corrected publication narrative is the one small evidenced improvement. Existing [quality floors](../../../../quality.json) remain unchanged: native 605, ASan 593, hygiene 24, ABI 12, fuzz 4, and title 3 tests; ABI, warning, analyzer, and sanitizer finding ceilings remain zero. The local baseline passes above those floors without changing the ratchet.

## CI timing and cancellation limits

Durations below are computed from the saved job `startedAt`/`completedAt` values. “Job time” sums recorded non-skipped job intervals; it is not billed runner-minutes. The wall span is earliest recorded job start to latest completion, not queue time. Event-skipped jobs have no step records and are not assigned durations.

| Receipt | Recorded job time | First job start → last completion | Slowest path |
|---|---:|---:|---|
| [Candidate CI, attempt 1](https://github.com/szTheory/muhtari/actions/runs/42918085281) | 107m 50s | 40m 01s | Windows arm64 36m 15s; Windows x64 23m 26s. |
| [Release-PR CI, attempt 2](https://github.com/szTheory/muhtari/actions/runs/42923477030) | 111m 22s | 59m 23s | Windows arm64 34m 41s; Windows x64 23m 51s. |
| [Release run, attempt 2](https://github.com/szTheory/muhtari/actions/runs/42931446855) | 129m 31s | 70m 35s | Windows arm64 37m 07s; Windows x64 23m 12s; separate release equality 25m 17s. |
| [Candidate Nightly, attempt 1](https://github.com/szTheory/muhtari/actions/runs/42918085345) | 46m 43s, including cancelled work | 28m 20s | Big-endian equality 28m 20s; the property job ran 9m 15s before cancellation. |

The Nightly `10,000 cases per property` step was cancelled, its `Every test ran` inventory step failed, and the report job was skipped. The [existing ship receipt](../../v4.0-SHIP.md) records the rejected missing inventory from the property log. Passing fuzz, vectors, benchmark, and equality jobs do not establish property acceptance; no retry is authorized by this receipt.

The fixed JSON responses contain job and step timestamps but no workflow `createdAt`, so the required `started_at − created_at` queue measure is unavailable even for first attempts. The release responses are attempt 2 and cannot supply a clean creation-to-start interval from earlier attempt history. The earlier v4.0 receipt's “queue was seconds” observation belongs to a different Nightly source and is not combined here. The response intervals also do not expose billing-rounding data. These observations support keeping current caches, deadlines, matrix topology, quality bounds, and advisory/required membership unchanged; they do not support a 30-day/40-run trend, a flakiness cause, or a safe structural optimization.

## Local baseline

The unchanged local workflow ran at source commit `950c0dd0560cd134a9568d36ffe5f9d6a04fb896`; the only tracked change at that source was this planning report. `python3 build/ship-local-gate.py` — exit 0, run URL `N/A-local`, elapsed 7m 12s. The existing helper used the temporary two-worker FIFO jobserver and invoked `cmake --workflow --preset ci` unchanged: all 692 native tests passed (CTest 315.55s), all three packages were built, and all six release checks passed (CTest 0.51s). The default-parallel `cmake --workflow --preset hygiene` run exited 8 because CTest timed out `hygiene.tree` at 60.04s (29/30 checks passed). The exact consumer, `sh scripts/hygiene.sh --tree`, then passed in 49.47s. One rerun, `CTEST_PARALLEL_LEVEL=1 cmake --workflow --preset hygiene`, passed 30/30 (CTest 54.66s); the preset still launched concurrent jobs, so no serialization or causal contention improvement is claimed. The first failure and rerun are both recorded in the plan summary. These local runs are `N/A-local`; raw output remains in ignored `build/17-01-local-ci.log`, `build/17-01-hygiene.log`, `build/17-01-hygiene-tree.log`, and `build/17-01-hygiene-serial.log`.

## Measured disposition

Keep the current CMake workflow, cache decision, deadlines, runner matrix, quality bounds, and required/advisory policy. The three required aggregates pass on their distinct sources; the Nightly cancellation remains visible and unaccepted. Available job timestamps locate the Windows lanes and release equality as the longest work, but the saved inputs do not include queue creation times or comparable performance/billing series. No timeout, cache, topology, or membership change is supported. The [current product direction](../../../PRODUCT-ROADMAP.md) and [prompt coverage](../../../preparation/PROMPT-COVERAGE.md) continue to describe the local bundle scope and evidence limits.

## Preserved stops

The nine deferred v3.0 requirements remain TUNE-06/07, PLAY-01–04, and FRONT-02–04. The denied Phase 12 diagnostic-app request and the spent Phase 12/14 host and remote scopes remain closed. This report provides no browser/device, actual host, public rehearsal, publication, merge, or new remote acceptance. Later success on another source does not replace those exact-source stops.
