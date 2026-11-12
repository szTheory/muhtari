---
phase: 19-measured-milestone-tune-up
researched: 2026-11-11
confidence: high
---

# Phase 19 Research: measured milestone tune-up

## Recommendation

Use one report-only plan. Verify the current publication correction already committed in `fef5691`, audit the retained exact-source v5.0 release and current-main CI receipts, record the six SEED-007 dimension judgments and efficiency limits, then run the existing native workflow. Do not add a second publication edit, test, cache, deadline, topology, or required/advisory-policy change unless a present fault is reproduced.

This is the smallest route that satisfies TUNE-10. The strongest alternative is another pipeline or quality-gate change; the available complete runs identify long jobs but show no cancellation and near-zero queue waits, while the receipts do not establish a recurring trend or safe structural optimization. A new audit/metrics subsystem would conflict with SEED-009's proportionality rule.

## Current publication record

The pre-kickoff snapshot said v5.0 was verified locally and that v0.12.0 remained published. The v6.0 kickoff added the terminal v5.0 ship receipt and updated PROJECT, STATE, ROADMAP and PRODUCT-ROADMAP to record v0.13.1 as published and v6.0 as selected. Current README install links are version-free `releases/latest` links. The change is already in `fef5691`; reusing and verifying it is the phase's one small correction. No second discrepancy was found in the current records. [VERIFIED: `fef5691^:.planning/PROJECT.md`, `fef5691^:.planning/STATE.md`, `fef5691^:.planning/PRODUCT-ROADMAP.md`; `fef5691:.planning/PROJECT.md`, `fef5691:.planning/STATE.md`, `fef5691:.planning/ROADMAP.md`, `fef5691:.planning/PRODUCT-ROADMAP.md`, `README.md`; exact current outcomes: `.planning/milestones/v5.0-SHIP.md:177-208`]

## Source-specific hosted evidence

The retained receipt separates two accepted sources:

| Gate | Source | Result | Run |
|---|---|---|---|
| Published v0.13.1 | `f62a74819985513cbed70259747607d4a5069c21` | Release workflow, attempt 1, completed/success | [43105582439](https://github.com/szTheory/muhtari/actions/runs/43105582439) |
| Current main after maintenance | `12953bcf615d6e13575f41644bad81f0bb627743` | CI workflow, attempt 1, completed/success | [43119220184](https://github.com/szTheory/muhtari/actions/runs/43119220184) |

The original run/job JSON is preserved in ignored `build/v5-release-run-api.json`, `build/v5-release-jobs-api.json`, `build/v5-fixture-main-run-api.json` and `build/v5-fixture-main-jobs-api.json`. `build/v5-final-efficiency.json` contains both exact final sources and an earlier main candidate (`50ee9be`, run `43113274132`); do not combine that superseded candidate with either accepted source.

Recalculation from the retained job timestamps gives:

- Published release: 29 terminal jobs, no cancelled jobs or steps; release Windows ARM64 2,579s (42m59s), release equality 1,233s (20m33s), publisher 341s (5m41s), and last install-line job 7s. Run start to last job completion is 3,143s (52m23s).
- Current main: 20 terminal jobs, no cancelled jobs or steps; Windows ARM64 2,474s (41m14s), Windows x64 1,468s (24m28s), and ASan 574s (9m34s). The required aggregate completed at 00:03:50Z; advisory wasm32 completed at 00:07:21Z. Run start to last job completion is 2,738s (45m38s).
- The retained efficiency summary records run queue `0.0s`; recalculation from the original `created_at` and `started_at` timestamps gives completed non-skipped job waits of 1–4s for release and 1–6s for current main. Timestamps have one-second resolution. These two runs do not establish a queue trend, billed runner time, or a 30-day/40-run profile.

Preserve two distinct failures: same-source main run `43105582302` on v0.13.1 source `f62a748` timed out `built.openbios` at 1.01s against 1s and left 15 dependents unrun; the earlier v0.13.0 release run `43078250838` timed out Windows x64 `fuzz.regress.run` at 14.05s against 14s. Neither is erased by a later green run. Intermediate cancelled shipping candidates and the failed advisory Nightly property workload remain historical and are not acceptance evidence. [VERIFIED: `.planning/milestones/v5.0-SHIP.md:137-175`; `build/v5-final-efficiency.json:36-99`; retained final API run/job receipts listed above]

These observations support retaining caches, deadlines, job topology, quality floors and advisory/required membership. Windows ARM64 remains the longest job; this is a profiling observation, not evidence that a job can be removed or the DAG safely changed.

## Plan and verification implications

- Produce one compact `19-TUNEUP.md` with current publication navigation, six linked 1–5 judgments, exact-source receipts, job/queue/cancellation limits, local baseline, no-change disposition and preserved stops.
- Keep measurements separate from judgments. The scores are assessments, not product grades. No YM performance result exists before Phase 20 implementation.
- Reuse `python3 build/ship-local-gate.py`, which invokes unchanged `cmake --workflow --preset ci` through the existing one-token FIFO jobserver. Reuse the existing hygiene workflow and the already completed `sh scripts/hygiene.sh --tree` baseline; the staged hook will check the final committed docs.
- Run only narrow local document/link and `git diff --check` assertions for the report. Do not add a recurring test for a reversible report-only change.
- No hosted/remote refresh, dispatch, publication, merge, application, host/device control or phase 20 work is needed or authorized here.

## Sources

- `.planning/ROADMAP.md` Phase 19 and `.planning/REQUIREMENTS.md` TUNE-10.
- `.planning/seeds/SEED-007-milestone-tune-up.md` and `.planning/seeds/SEED-009-proportional-maintenance-review.md`.
- `.planning/preparation/ENGINEERING.md` §11 and `.planning/preparation/GSD-HANDOFF.md` §3.1.
- `.planning/milestones/v5.0-SHIP.md` terminal publication/efficiency receipt.
- `.planning/milestones/v5.0-phases/17-measured-milestone-tune-up/17-TUNEUP.md` prior six-dimension report pattern.
- Ignored retained JSON sources listed above; no web or live service was queried.
