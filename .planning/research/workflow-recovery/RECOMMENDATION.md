---
title: "Recover development without repeating workflow loops"
date: 2026-10-24
status: adopted
scope: Archive measurement policy, complete CI outcome audit, bounded GSD recovery
---

# Decision

Retire only the historical compressed-archive cutoff. Preserve required authenticated
package-size observations and every package-content, provenance, ABI, correctness
and performance contract. Audit complete CI outcomes using the existing required
aggregate. Calibrate the Apple control timeout through the existing measured rule.
Resolve only the halted plan's acceptance gate, then stop with the next plan ready.

The owner requested relevant specialist fan-out, adversarial synthesis, adoption and
durable lessons. This is the separately authorized resource-purpose review deferred
by the earlier proportional-gates decision. It does not claim the old limit passed.

## Reproduced causes

The GSD stop itself worked: a canonical halted summary blocked nine dependent
plans and exposed zero runnable plans. Four plans were accepted, two tasks of
the fifth implemented. Repeated helpers or a general replan would not resolve
the acceptance condition.

The cutoff was a historical observation, not a discovered capacity. Phase05-05's
"Independent baseline and archive calibration" records an initial399952 base and
then minimum399937 despite unchanged bases399949/399953. Commits79e9e8c/51a3f95
document this. No present target download, flash or working-set need establishes
exactly399937 aggregate compressed bytes as the product boundary.

Corrected source9a64bbc measured Windows401687, genuine base401724: candidate
excess1750bytes,0.438%. A verified improvement saved570Windows bytes but did not
resolve the gate. Further compression work has no demonstrated consumer capacity
objective. [Historical failure and corrective run](https://github.com/szTheory/muhtari/actions/runs/40135571745);
[canonical halt](../../phases/08-controllers-at-the-pins/08-05-SUMMARY.md).

Avoidable loops amplified that stop:

- A handoff initially missed an independent required RetroArch link failure.
  A standalone compile omitted the new owned SHA source;9a64bbc corrected it.
- Apple quality controls repeatedly hit30s. Complete successful x64 runs measured
  22.79/19.76s in40135571745/job121002299364 and21.77/20.78s in40132929297/job120988352148.
  A killed30s sample is right-censored, not full duration.
- Windowsarm current/base publication controls took756.47/744.81s; the controller
  diagnostic took0.28/0.27s. A~28s frontend job waited for the whole native matrix.
- An advisory GSD scanner parsed a backticked assembly command as a missing file.
  Runtime summarized counts include halted plans; they are not accepted counts.
- Documentation fences produced valid behavior-identical dispatch parents, but
  dispatch same-head repeatability did not prove phase-versus-main performance.
- Repeated unchanged waiting updates and state reconciliation added no evidence.

## Coherent recommendations

| Decision | Adopt | Alternative and tradeoff |
|---|---|---|
| Archive cutoff | Retire only numeric blocking; require authenticated observations | Continued micro-optimization preserves a number without a demonstrated capacity and repeats future cliffs |
| Replacement quota/waiver | None | A bigger round number or candidate-fitting waiver moves the cliff without a consumer need |
| Measurement authority | Per-native current/base validation, reports and joined-JSON peaks | Logging alone loses machine consumers; optional evidence loses missing/forged-data detection |
| CI outcome | Unique existing `CI required` owns membership; expose all failures/skips | Another job registry drifts; requiring advisory jobs silently changes policy |
| Apple control timeout | Provisional46s,ceil(2x22.79); same cases/oracles | Another blind retry leaves the undersized deadline; deleting cases loses coverage |
| GSD recovery | Keep canonical halt history; close only revisedTask3 after evidence | Deleting the summary triggers safe-resume; early status changes invent completion |
| Specialist work | Relevant independent reviews, one adversarial synthesis, implement | Recursive fan-out without decision-changing evidence adds context and cost |
| Critical path | Seed measured publication-control profiling | Simultaneous DAG/cache redesign obscures coverage and trust changes |

Compressed distribution bytes include installed files, compiler output and archive
metadata; they do not measure emulator working set or frame cost. This does not
attribute this excess to timestamps.
[CPack archive backend](https://cmake.org/cmake/help/latest/cpack_gen/archive.html),
[archive reproducibility](https://reproducible-builds.org/docs/archives/).
New limits need named consumer objectives and comparable measurements, as
ENGINEERING §11 already requires.

## Implementation and acceptance

Remove only `archive_bytes` from bound policy and permit its literal retirement
alongside `gate_lines`. Keep the real exactly-three-ZIP producer. The join validates
each native current/base archive SHA,run,producer,configuration,units,measured status,
integer value and environment. Retain per-lane output and peak observations in both
joined JSON files. No report direction, advisory conversion, sentinel ceiling,
dependency or registry is added. Unrelated deletion, loosening, identity, missing
evidence and forged-value faults remain blocking.

Exercise retirement and authentic above-old-limit passes, and malformed/missing/
unavailable/forged/current/base faults through existing consumers. Audit existing
GitHub JSON at exact source/completed run, printing all non-success jobs before the
required verdict. Keep library/WASM results visible and advisory. Apply the Apple
deadline calibration; keep Linux/Windows deadlines and all semantic assertions.

Intermediate device plans record local full-workflow package measurements; final
phase acceptance owns the full exact-source matrix. Earlier hosted probes need a
concrete platform/integration uncertainty. Batch local checks before one new exact-source hosted acceptance. Inspect all jobs
and actual artifacts. Retry only a named transient hypothesis with a finite attempt
and preserved failure history. Repeated semantic failures require a fix or a stop.
GitHub reruns retain the original SHA/ref; failed dependencies can prevent later jobs.
[Dependencies](https://docs.github.com/en/actions/how-tos/write-workflows/choose-what-workflows-do/use-jobs),
[reruns](https://docs.github.com/en/actions/how-tos/manage-workflow-runs/re-run-workflows-and-jobs).
CMake's [TIMEOUT](https://cmake.org/cmake/help/latest/prop_test/TIMEOUT.html) is finite
runtime allowance, not a semantic oracle.

After revised acceptance passes, append explicit policy-resolution evidence to the
canonical halt summary and close only plan05. Assert5/14 accepted, plan06 ready,
no INPUT requirement or phase completion. Run no later hardware/GSD step here.

## Durable operation

One orchestrator owns shared phase position/accepted counts; executors own task
changes/evidence. Establish SDK labels once. Treat advisory scanners as advisory,
checking real declared files, commits and consumers instead of editing honest prose
to satisfy a heuristic. Keep global installed runtime fixes separate from this repo.

Run affected standalone consumers locally when shared dependencies change. Use
dispatch for exploration with its actual base/scope labelled, and the stable PR base
for phase-versus-main acceptance. Report changed evidence and material wait estimates.

Google SRE distinguishes repetitive operational work from engineering that removes
its cause. Here the improvement is removing unsupported gates and missed-result
mechanisms rather than automating more retries.
[Eliminating toil](https://sre.google/sre-book/eliminating-toil/).

The [prompt](prompt.txt), AGENTS pointer, GSD handoff and LESSONS ledger carry these
rules into future work. Stop further review when it would not change a choice.
Publication-control profiling is a seed, not a new condition for development.

## Verification record

Final command/exit/run evidence is appended after implementation. Historical
threshold failures remain failures; revised acceptance is not fitting the old cap.


### Local recovery checks

All local run URLs are N/A-local. No logs are committed.

| Command | Exit | Observation |
|---|---:|---|
| cmake --preset ci; ctest --preset ci -R '^policy[.](ci_run\|quality)[.]controls$' --no-tests=error --output-on-failure | 0 | Both consumers pass; audit covers22 pass/fault cases, quality retirement and every retained fault boundary |
| cmake --workflow --preset ci | 0 | 647 ordinary tests, all3 packages and6 release tests |
| cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake | 0 | Exactly647 tests once each |
| Same JUnit consumer with junit-release.xml and -DLABELS=release | 0 | Exactly6 release tests once each |
| cmake -DINPUT=build/ci-recovery-prior-run.json -DEXPECTED_SHA=9a64bbc7aba72bf7ae98e2a3e796907a6eb04cb9 -P tools/check_ci_run.cmake | 1 | Correctly identifies attempt2 and reports quality plus CI required failures, with skipped title visible |
| Revised existing quality_join against downloaded actual9a64bbc/986e4dd eight-lane inputs | 0 | Required authenticated401687/401724 peaks retained in joined JSON; this is policy-transition replay, not a new source run |
| scripts/hygiene.sh --tree; git diff --check | 0 | No findings |

The new exact-source hosted result is recorded below after it completes. Until then,
the canonical05 halt and accepted4/14 remain unchanged.
