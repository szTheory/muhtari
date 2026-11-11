# Phase 17 tune-up evidence

## Publication correction

The kickoff correction updates current publication records to point to the isolated v4.0 source: [PR #47](https://github.com/szTheory/muhtari/pull/47) merged, [release PR #48](https://github.com/szTheory/muhtari/pull/48) merged, and [v0.12.0](https://github.com/szTheory/muhtari/releases/tag/v0.12.0) published with 19 native assets. The earlier v4.0 ship-preparation entries remain historical checkpoints; the last section of the [ship receipt](../../milestones/v4.0-SHIP.md) states the current outcome. Its earlier [PR #46](https://github.com/szTheory/muhtari/pull/46) entry remains a separate failed combined-source attempt, including the held-input capture failure. PR #46 is not the shipped source and its result is not overwritten.

| Record | Current navigation or status | Local target resolves |
|---|---|---|
| [PROJECT](../../PROJECT.md) | Current milestone is v5.0; v4.0 publication and nine deferred v3.0 requirements are stated separately. | `milestones/v4.0-MILESTONE-AUDIT.md`; `PRODUCT-ROADMAP.md` |
| [ROADMAP](../../ROADMAP.md) | v4.0 is shipped via PR #47 and v0.12.0; the v5.0 phase/acceptance boundary and original host stops remain explicit. | `milestones/v4.0-MILESTONE-AUDIT.md`; `milestones/v4.0-ROADMAP.md`; `milestones/v4.0-SHIP.md`; `research/v5.0/SUMMARY.md` |
| [STATE](../../STATE.md) | Current focus is Phase 17; v4.0 publication, PR #46 separation, and the Phase 12/14 stops are recorded. | `PROJECT.md`; `REQUIREMENTS.md`; `ROADMAP.md`; `milestones/v4.0-SHIP.md`; `PRODUCT-ROADMAP.md`; `milestones/v4.0-MILESTONE-AUDIT.md`; `debug/workspace-reconciliation-2026-11-10.md` |
| [Product roadmap](../../PRODUCT-ROADMAP.md) | Immediate v5.0 bundle work follows the published v4.0 core; no browser/device or old host acceptance is claimed. | `milestones/v4.0-SHIP.md`; `research/v5.0/SUMMARY.md`; referenced seed records |
| [Prompt coverage](../../preparation/PROMPT-COVERAGE.md) | Rows 60–62 preserve deferred host scope and authorize local evidence gathering without renewing it. | Local homes are recorded in the linked planning and project files. |

This is the already landed kickoff correction; this plan adds no second publication or pipeline change. The report links to the controlling records above rather than copying their mutable status prose.

## Saved receipt qualification

The audits below read the original files in ignored `build/milestone-preflight/`; none was edited or projected into a synthetic receipt.

| Source | Recorded identity | Audit and outcome |
|---|---|---|
| [Candidate CI](https://github.com/szTheory/muhtari/actions/runs/42918085281) | Attempt 1; head `3654cbc4071d14431c31e8bb2b5b15d182928b7a`; completed/success. | `cmake -DINPUT=build/milestone-preflight/v4-candidate-ci.json -DEXPECTED_SHA=3654cbc4071d14431c31e8bb2b5b15d182928b7a -P tools/check_ci_run.cmake` — exit 0; exactly one `CI required` aggregate passed. |
| [Release-PR CI](https://github.com/szTheory/muhtari/actions/runs/42923477030) | Attempt 2; head `4bfbb4403b17453092ee4adfc73f1ae7ee6ade4b`; completed/success. | `cmake -DINPUT=build/milestone-preflight/v4-release-pr-ci.json -DEXPECTED_SHA=4bfbb4403b17453092ee4adfc73f1ae7ee6ade4b -P tools/check_ci_run.cmake` — exit 0; exactly one `CI required` aggregate passed. |
| [Release v0.12.0](https://github.com/szTheory/muhtari/actions/runs/42931446855) | Attempt 2; head `363b2e338f69ad7a71c4c7df93f76eea6e6fed0a`; completed/success. | A one-off assertion against the original `v4-release.json` checked that exact head, attempt, URL, run completion, one successful `release-ci / CI required` aggregate, and valid terminal job outcomes — exit 0. The unchanged checker expects the CI aggregate name `CI required`, so this release-specific name was qualified directly instead of renaming the receipt. Non-success jobs: `ci`, `install-line`, `cleanup`, `release-ci / title`, `release-ci / perf`, `equality`, and `rehearsal` were all event-skipped. They are not reported as executed checks. |
| [Candidate Nightly](https://github.com/szTheory/muhtari/actions/runs/42918085345) | Attempt 1; head `3654cbc4071d14431c31e8bb2b5b15d182928b7a`; run cancelled. | Saved receipt only; the property job was cancelled and its downstream report job skipped. It does not add acceptance to the candidate CI result. |

These are separate source-specific outcomes: candidate CI and release-PR CI pass their own required aggregate; the published release has its own successful `release-ci / CI required`; the candidate Nightly is censored. Earlier combined-source and host failures remain in the [v4.0 ship receipt](../../milestones/v4.0-SHIP.md). No new remote read, dispatch, rehearsal, publication, merge, or host/device action was used. Run URLs are the links above; the receipt files remain ignored local evidence.
