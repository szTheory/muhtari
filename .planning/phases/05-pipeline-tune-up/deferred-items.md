## Deferred Items

- Tooling seed: release-called Windows arm64 run39706506980/job119588102339 times out unchanged `built.openbios` at1.24s against1s, causing12 fixture-dependent tests not to run; the native workflow exits8 and produces no quality evidence. The native digest control is not the failure. Retain the timeout and missing-evidence rejection; retry the same commit and record both attempts rather than claiming a source repair.
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
