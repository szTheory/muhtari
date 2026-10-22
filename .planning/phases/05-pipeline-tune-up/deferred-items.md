## Deferred Items

- Tooling seed: release-called Windows arm64 run39706506980/job119588102339 times out unchanged `built.openbios` at1.24s against1s, causing12 fixture-dependent tests not to run; the native workflow exits8 and produces no quality evidence. The native digest control is not the failure. Retain the timeout and missing-evidence rejection; retry the same commit and record both attempts rather than claiming a source repair.
  status: open

- Advisory WASM seed: release-called run39832530253 attempt2/job120020998725 loses a pinned-tool fetch with `curl` exit35, connection reset, before tests; its inventory step and rehearsal/install/cleanup skip. Called `CI required` and quality pass, authentic eight-artifact join passes, and direct current WASM passes. Overall Release remains failure. Preserve the advisory status and failed evidence; no further rerun or source edit is part of recovery acceptance.
  status: open

- Tooling seed: installed actionlint rejects the inherited `concurrency.queue: max` in release.yml, while GitHub accepts and executes that workflow. Focused schema validation suppresses only that unsupported syntax and the already known hosted runner labels. No workflow concurrency or safety control changes to satisfy the older validator.
  status: open

- Tooling seed reproduced during quality integration: both Windows native workflows fail the existing `policy.digest.controls` at `tests/policy/digest.cmake:52` with `digest valid preview failed: no such file or directory`, exit8. The new quality controls pass; required Windows observation/base artifacts cannot be produced. Evidence: https://github.com/szTheory/muhtari/actions/runs/39694430448/job/119548291959 and job119548291973. The digest source is unchanged by this plan. Preserve its control and route a scoped tooling repair; no product requirement is added.
  status: fixed by scoped debug; CI39699892851 Windows current609+6/base605+6 accepted, tests retained

- Unreproduced tooling observation: run39697609787 x64 job119559093658 built trusted base65b360d successfully but its DLL-CRT consumer emitted no output. The identical unchanged base subsequently passes full605+6 on x64 and arm64 in runs39698420789/39699026057/39699892851. No cause or source fix is claimed; failed-base rejection was retained and temporary diagnostics removed. This is a seed, not a new requirement.
  status: open

- Tooling seed: full actionlint reports SC2017 and SC2059 in the unchanged RetroArch pixel negative-control shell. Schema/action validation with shellcheck disabled passes. No pipeline control is weakened to silence these inherited advisory diagnostics.
  status: open

- Tooling seed: `tests/cmake/budgets.cmake` accepts `-DROOT=.` but its relative document glob observes only the README/header (two files). The registered `docs.budgets` test supplies the absolute source root and correctly rejects oversized documents; use an absolute root for direct checks. Reproduced with `cmake -DROOT=. -P tests/cmake/budgets.cmake` versus `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake`, exits 0/1 while the candidate testing page exceeded its unchanged budget. Out of scope for this plan; no production check was weakened or removed.
  status: open


## Discussion metadata seed — 2026-10-22

The registered state mutation handlers re-derived `progress.completed_plans: 6` while the active roadmap/body
remain 5/9; historical/superseded summaries are present. This discussion completed no implementation task.
`state.add-decision` records the active count and new context precedence; do not use the generated counter to
accept halted 05-06/05-11 or release downstream execution. Investigate the generic counter only if recurring;
this tooling finding adds no phase requirement, check, dependency or allocation. No direct STATE/ROADMAP edit
was used to override registered handler behavior.

## Recovery execution evidence — 2026-10-22

- Inherited whitespace seed: `git diff --check origin/main...HEAD` exits2 for new blank lines at EOF in `.github/workflows/be-equality.yml:63`, `tests/policy/gate-lines.txt:148` and historical `05-01-SUMMARY.md:421`. All three predate this recovery; its own diff check exits0. Preserve historical summaries and the fixed implementation scope; no whitespace repair or acceptance pass is inferred.
  status: open

- Hosted timeout seed: current direct run39832530167 attempt1/macOS x64 job120012226551 times out `cpu.decimal.clark` at3.17s (CTest exit8). Current release-called run39832530253 attempt1/Windows arm64 job120012202401 times out unchanged trusted-base605 `built.samples.self_test` at4.03s (CTest8; quality producer1), and called ASan job120012202359 cancels at its five-minute bound during independent-base evidence. Both quality jobs reject missing artifact sets with exit1. One failed-job rerun per workflow preserves these attempts without timeout or implementation edits; a subsequent pass is only a flake candidate.
  status: open
