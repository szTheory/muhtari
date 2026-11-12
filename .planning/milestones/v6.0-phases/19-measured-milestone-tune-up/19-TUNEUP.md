# Phase 19 measured tune-up

## Publication baseline

The kickoff correction in `fef5691` updates the current record from the former
v5.0-local / v0.12.0-current wording to the shipped v0.13.1 baseline, and selects
v6.0 for local follow-through. The present [PROJECT](../../../PROJECT.md),
[STATE](../../../STATE.md), [ROADMAP](../../../ROADMAP.md),
[requirements](../../../REQUIREMENTS.md), and [product roadmap](../../../PRODUCT-ROADMAP.md)
agree. Their local links resolve. The [terminal v5.0 ship receipt](../../v5.0-SHIP.md)
records the immutable published source, public archive/attestation checks, latest
installation, PR #53 merge and successful current-main CI. This report reuses
those verified outcomes; it did not re-download assets or refresh hosted state.

The [README](../../../../README.md) keeps version-free `releases/latest` install and
download links for native and Emscripten artifacts, and points to the existing
[portable bundle instructions](../../../../docs/libretro.md#portable-emscripten-bundle).
No second publication or documentation correction was indicated.

## Source-specific receipts

The original run/job JSON was audited in place; no synthetic or normalized
response replaced it.

| Accepted outcome | Exact source and run | Qualification |
|---|---|---|
| Published v0.13.1 | Release source `f62a74819985513cbed70259747607d4a5069c21`, [run 43105582439](https://github.com/szTheory/muhtari/actions/runs/43105582439), attempt 1 | `build/v5-release-run-api.json` and `build/v5-release-jobs-api.json`: completed/success; exact run/source/attempt identities on all jobs; exactly one successful `release-ci / CI required` aggregate. |
| Current main after fixture maintenance | Main source `12953bcf615d6e13575f41644bad81f0bb627743`, [run 43119220184](https://github.com/szTheory/muhtari/actions/runs/43119220184), attempt 1 | `build/v5-fixture-main-run-api.json` and `build/v5-fixture-main-jobs-api.json`: completed/success; exact run/source/attempt identities on all jobs; exactly one successful `CI required` aggregate. |

Both final run responses have no cancelled jobs or steps. The earlier candidate
`50ee9be` / run `43113274132` in `build/v5-final-efficiency.json` is not combined
with either accepted source. The ship receipt remains the source for the
previously checked public bytes, attestations and installation.

## Six quality dimensions

These are judgments about the evidence and maintenance path, not measured product
grades. Each rationale links to the evidence or governing contract.

| Dimension | Score | Evidence and limit |
|---|---:|---|
| Performance | 3/5 | The [saved CI timing observations](#ci-timing-and-cancellation-limits) locate long Windows jobs, but are different workflows and do not measure emulator throughput, billing, or YM behavior. Phase 20 still needs its own bounded-work and cost measurements. |
| CI/CD | 4/5 | The [release and current-main sources](#source-specific-receipts) each pass their own required aggregate, and the [local CI workflow](../../../../.github/workflows/ci.yml) retains its inventory. Two distinct historical timing failures remain visible in the ship receipt; these small samples support preserving current coverage and limits. |
| Ease of use | 4/5 | The [README](../../../../README.md) provides a direct RetroArch install, latest-release downloads and a runner quick start; [portable host instructions](../../../../docs/libretro.md#portable-emscripten-bundle) cover the Emscripten bundle. No application or hardware install was performed for this report. |
| Architecture | 4/5 | The [three-deliverable and deterministic-core contract](../../../../AGENTS.md) remains explicit, and the [existing CMake presets](../../../../CMakePresets.json) supply the local workflow without a new dependency or runtime layer. This says nothing about the not-yet-built YM specialization. |
| Docs/README | 4/5 | Current milestone and release records now agree on v0.13.1 and v6.0, and user-facing links remain version-free. The [ship history](../../v5.0-SHIP.md) keeps older failures as historical checkpoints; the final section identifies the accepted outcome. |
| Upgrade path | 4/5 | [Pinned dependencies](../../../../pins.txt) and the [Dependabot policy](../../../../.github/dependabot.yml) preserve reviewable upgrades. This review made no fresh dependency, runner-image, vulnerability or live settings query, so it does not claim current external versions or settings compliance. |

Performance is the weakest dimension because the retained CI timings are not a
product performance series and no YM implementation exists yet. The v0.13.1
record correction improves publication clarity; it does not fill that
measurement gap. The [quality floors](../../../../quality.json) and required versus
advisory membership remain unchanged.

## CI timing and cancellation limits

Durations below are recalculated from the original job timestamps. They describe
individual executed jobs or run-start-to-last-job completion; parallel job
durations are not added or labeled as wall time or billed time. Event-skipped
jobs are excluded.

| Accepted source | Selected observations | Run start to last job |
|---|---|---:|
| Published release, run 43105582439 | Windows ARM64 2,579s; release equality 1,233s; publisher 341s; final install-line job 7s | 3,143s |
| Current main, run 43119220184 | Windows ARM64 2,474s; Windows x64 1,468s; ASan 574s; required aggregate ended 00:03:50Z, then advisory wasm32 ended 00:07:21Z | 2,738s |

Windows ARM64 is the longest observed final job. The current-main advisory
wasm32 lane finishes after the required aggregate; the release publisher adds a
341s verification segment and the install-line job adds 7s. The final
`build/v5-final-efficiency.json` records run-level queue `0.0s`. Recalculation
from the original `created_at` and `started_at` job timestamps gives active-job
waits of 1–4s for release and 1–6s for current main. These few runs have
whole-second timestamp precision and establish neither a queue trend nor a
30-day/40-run or billed-minutes profile.

Preserve the same-source main failure [run 43105582302](https://github.com/szTheory/muhtari/actions/runs/43105582302):
`built.openbios` reached 1.01s against its 1s bound and 15 dependents did not
run. Preserve the v0.13.0 release failure
[run 43078250838](https://github.com/szTheory/muhtari/actions/runs/43078250838):
Windows x64 `fuzz.regress.run` reached 14.05s against 14s. The ship receipt also
retains earlier superseded cancellations and the unaccepted advisory property
run with missing-JUnit evidence. Later success does not replace or erase those
source-specific failures.

## Local baseline

Both full workflows used the unchanged helpers at invocation HEAD `fc7d0cb`;
product source, tests and configuration remain byte-identical to `fef5691`.
Results are local (`N/A-local`); no run URL applies. Raw logs remain ignored in
`build/` and are not committed.

| Command | Result | Scope |
|---|---|---|
| `python3 build/ship-local-gate.py: exit 0` | `cmake --workflow --preset ci` ran unchanged through the existing helper. All 693 regular tests passed in 272.22s; all six release/archive checks passed in 0.82s. | Native baseline at `fc7d0cb`; product bytes unchanged from `fef5691`. Log: ignored `build/v6-phase19-native.log`. |
| python3 build/phase18-hygiene-gate.py (sandbox): exit 8 | Only `hygiene.tree` timed out at its unchanged 60.02s deadline; the other 29 checks passed. | Same source, preset, membership and deadlines. This outcome is retained, not relabeled as a pass. |
| `ctest --test-dir build/hygiene --output-on-failure -R '^hygiene[.]tree$'` (outside sandbox): exit 0 | The exact unchanged tree test passed in 37.52s at the same 60s deadline. | A scoped execution-boundary comparison, not a diagnosis of a kernel or process cause. |
| python3 build/phase18-hygiene-gate.py (outside sandbox): exit 0 | The unchanged full hygiene preset passed 30/30 in 87.57s. | Same source, commands, membership and deadlines. Log: ignored `build/v6-phase19-hygiene-qualified.log`. |

The same-source targeted and full outside-sandbox passes support execution-boundary overhead as relevant to the timeout; they do not establish a specific root cause.
Scoped process inspection found no leftover attributable process. The earlier
pre-kickoff `sh scripts/hygiene.sh --tree` baseline also passed exit 0, N/A-local; it was not
repeated. No deadline or test membership changed. No new test was added for this
reversible report-only phase.

## Measured disposition

The v6 kickoff already corrected the only present publication-record mismatch;
this report verifies and qualifies that correction. The bounded disposition is
no further current-record or pipeline change. The two accepted hosted sources
are green, final cancellation lists are empty, run-level queue is near zero, and
the selected timing observations do not justify a cache, deadline, topology,
quality-floor, or required/advisory-membership change. The prior timing failures
are preserved but are censored near-bound outcomes, not complete samples from
which to recalibrate a limit. The hygiene outcome was resolved by qualifying
execution context with unchanged checks, not by widening its deadline or
weakening coverage.

Doing nothing would leave the two final sources, six judgments, timing limits
and local gate qualifications harder to inspect together. This compact report
organizes that existing evidence. A broader pipeline intervention or recurring
measurement subsystem is not supported by these few observations. SEED-009's
optional profiling remains an observation, not a new audit program or gate.

## Preserved stops

The nine deferred v3.0 requirements remain TUNE-06/07, PLAY-01–04 and
FRONT-02–04. The denied Phase 12 diagnostic-app request and every original
Phase 12/14 spent attempt, expired window and no-retry stop remain intact. This
report establishes no YM result, live host/device, browser, physical listening,
new remote, publication or merge evidence. Shared planning-state progression
and Phase 19 acceptance remain with the orchestrator.
