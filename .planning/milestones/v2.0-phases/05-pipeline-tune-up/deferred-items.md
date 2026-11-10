## Deferred Items

- Tooling seed: release-called Windows arm64 run39706506980/job119588102339 times out unchanged `built.openbios` at1.24s against1s, causing12 fixture-dependent tests not to run; the native workflow exits8 and produces no quality evidence. The native digest control is not the failure. Retain the timeout and missing-evidence rejection; retry the same commit and record both attempts rather than claiming a source repair.
  status: acknowledged

- Advisory WASM seed: release-called run39832530253 attempt2/job120020998725 loses a pinned-tool fetch with `curl` exit35, connection reset, before tests; its inventory step and rehearsal/install/cleanup skip. Called `CI required` and quality pass, authentic eight-artifact join passes, and direct current WASM passes. Overall Release remains failure. Preserve the advisory status and failed evidence; no further rerun or source edit is part of recovery acceptance.
  status: acknowledged

- Tooling seed: installed actionlint rejects the inherited `concurrency.queue: max` in release.yml, while GitHub accepts and executes that workflow. Focused schema validation suppresses only that unsupported syntax and the already known hosted runner labels. No workflow concurrency or safety control changes to satisfy the older validator.
  status: acknowledged

- Tooling seed reproduced during quality integration: both Windows native workflows fail the existing `policy.digest.controls` at `tests/policy/digest.cmake:52` with `digest valid preview failed: no such file or directory`, exit8. The new quality controls pass; required Windows observation/base artifacts cannot be produced. Evidence: https://github.com/szTheory/muhtari/actions/runs/39694430448/job/119548291959 and job119548291973. The digest source is unchanged by this plan. Preserve its control and route a scoped tooling repair; no product requirement is added.
  status: acknowledged

- Intermittent tooling observation: run39697609787 x64 job119559093658 built trusted base65b360d successfully but its DLL-CRT consumer emitted no output. The identical unchanged base subsequently passes full605+6 on x64 and arm64 in runs39698420789/39699026057/39699892851. Reobserved in run39839399512 attempt1/job120036186384: current609+6 and inventories pass, then unchanged-base605 install.consumer emits no output and fails its required output regex after2.19s. One unchanged-head Windows job rerun succeeds in attempt2/job120039214618. No cause or source fix is claimed; failed-base rejection, timeouts and workloads remain. This is a seed, not a new requirement.
  status: acknowledged

- Tooling seed: full actionlint reports SC2017 and SC2059 in the unchanged RetroArch pixel negative-control shell. Schema/action validation with shellcheck disabled passes. No pipeline control is weakened to silence these inherited advisory diagnostics.
  status: acknowledged

- Tooling seed: `tests/cmake/budgets.cmake` accepts `-DROOT=.` but its relative document glob observes only the README/header (two files). The registered `docs.budgets` test supplies the absolute source root and correctly rejects oversized documents; use an absolute root for direct checks. Reproduced with `cmake -DROOT=. -P tests/cmake/budgets.cmake` versus `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake`, exits 0/1 while the candidate testing page exceeded its unchanged budget. Out of scope for this plan; no production check was weakened or removed.
  status: acknowledged


## Discussion metadata seed — 2026-10-22

The registered state mutation handlers re-derived `progress.completed_plans: 6` while the active roadmap/body
remain 5/9; historical/superseded summaries are present. This discussion completed no implementation task.
`state.add-decision` records the active count and new context precedence; do not use the generated counter to
accept halted 05-06/05-11 or release downstream execution. Investigate the generic counter only if recurring;
this tooling finding adds no phase requirement, check, dependency or allocation. No direct STATE/ROADMAP edit
was used to override registered handler behavior.

Reproduced during05-14 closeout: state.record-session re-derived8 completed plans from
the new explicitly halted1/3 SUMMARY. Actual accepted completion remains7/9. Current
STATE frontmatter/body and ROADMAP are reconciled to actual acceptance; no task/plan
completion or requirement mutation follows summary existence. The generic serializer
remains a tooling seed, not a new phase requirement or funded repair.

## Recovery execution evidence — 2026-10-22

- Inherited whitespace seed: `git diff --check origin/main...HEAD` exits2 for new blank lines at EOF in `.github/workflows/be-equality.yml:63`, `tests/policy/gate-lines.txt:148` and historical `05-01-SUMMARY.md:421`. All three predate this recovery; its own diff check exits0. Preserve historical summaries and the fixed implementation scope; no whitespace repair or acceptance pass is inferred.
  status: open

- Hosted timeout seed: current direct run39832530167 attempt1/macOS x64 job120012226551 times out `cpu.decimal.clark` at3.17s (CTest exit8). Current release-called run39832530253 attempt1/Windows arm64 job120012202401 times out unchanged trusted-base605 `built.samples.self_test` at4.03s (CTest8; quality producer1), and called ASan job120012202359 cancels at its five-minute bound during independent-base evidence. Both quality jobs reject missing artifact sets with exit1. One failed-job rerun per workflow preserves these attempts without timeout or implementation edits; a subsequent pass is only a flake candidate.
  status: open

- Tooling seed WR-01: a delayed release checkout can equal the triggering push's before SHA, so the existing producer/join correctly refuse an independent base. Preserve that rejection; a future scoped change should authenticate the selected release parent in both paths. See05-REVIEW.md and05-REVIEW-DISPOSITION.md; no current PR/rehearsal requirement or new gap round.
  status: open, deferred under AGENTS7

- Tooling observation: direct CI39875580833/job120160294530 times out two library cases at2.00s and correctly rejects the incomplete37/39 result set. Current direct/called39876385414/39876385482 execute all43 library checks successfully with unchanged library inputs; the intervening checksum-fixture/job-bound edits are not claimed as a library source repair.
  status: open seed, no demonstrated source cause

- Tooling seed COST-01, reproduced in the final evidence audit: GitHub retry job120180716506 copies the name/start/end of original120172221128 although its created_at is later than the original completion. The actual digest monthly filter counts both209s intervals as418s; skipped retry placeholders can also have negative intervals. Reproduction: python3 build/plan-evidence/release-continuation/prove-copied-cost.py, exit0, using real API records from https://github.com/szTheory/muhtari/actions/runs/39879193669. The450-run preview remains source-linked raw evidence, but its4124.1333 runner-minute total is untrusted and is not used as accepted cost evidence. This closeout uses a one-off audit excluding confirmed copied timestamps and skipped/negative placeholders. A future scoped reporter/control repair must preserve genuine failed/cancelled executions. No new phase requirement or fix round is added under AGENTS7 and D-35.
  status: open, deferred under AGENTS7
