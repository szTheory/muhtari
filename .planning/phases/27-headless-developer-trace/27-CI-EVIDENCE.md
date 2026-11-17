# Phase 27 CI evidence

- **Checked:** 2026-11-17 UTC
- **Branch:** `milestone/v10.0-headless-trace` at `5d516cd1670d1d0fe4a647241c5df972999e508a`
- **Intended PR base:** `main` at local `origin/main` `89b6ff82812cd5be873e9f90d32ac55fded8694b`
- **Reviewed product source:** `c60c192b9088b5f21fc199d574de17e89e5b4c94`
**Source comparison:** `git diff --quiet c60c192b9088b5f21fc199d574de17e89e5b4c94 HEAD -- . ':(exclude).planning'` — exit 0; tracked non-planning source is identical.

## Receipt inventory

| receipt | disposition | reviewed_source | built_source | run_url | attempt |
|---|---|---|---|---|---|
| windows-x64 | candidate | c60c192b9088b5f21fc199d574de17e89e5b4c94 | 070f65f10d1cc2f62aba63ad3e2f931d2e5987c7 | https://github.com/szTheory/muhtari/actions/runs/44001040567 | 1 |
| windows-arm64 | candidate | c60c192b9088b5f21fc199d574de17e89e5b4c94 | 070f65f10d1cc2f62aba63ad3e2f931d2e5987c7 | https://github.com/szTheory/muhtari/actions/runs/44001040567 | 1 |
| linux-trace_cost | failed | c60c192b9088b5f21fc199d574de17e89e5b4c94 | 070f65f10d1cc2f62aba63ad3e2f931d2e5987c7 | https://github.com/szTheory/muhtari/actions/runs/44001040567 | 1 |

## Existing result search

All queries were read-only. They returned no matching pull request or workflow run, so no existing result qualifies for the Windows path checks or the Linux Cachegrind receipt.

| Query | Exit | Result |
|---|---:|---|
| `gh pr list --repo szTheory/muhtari --state all --head milestone/v10.0-headless-trace --json number,url,state,headRefOid` | 0 | `[]` |
| `gh run list --repo szTheory/muhtari --workflow ci.yml --branch milestone/v10.0-headless-trace --limit 10 --json databaseId,event,headSha,status,conclusion,url` | 0 | `[]` |
| `gh run list --repo szTheory/muhtari --workflow ci.yml --commit c60c192b9088b5f21fc199d574de17e89e5b4c94 --limit 100 --json databaseId,event,headSha,status,conclusion,url` | 0 | `[]` |

No qualifying run existed to inspect with `gh run view` or download job logs and artifacts from. No receipt is accepted by discovery alone.

## Existing workflow route and limits

- The current CI workflow has `build (windows-x64)` on `windows-2025` and `build (windows-arm64)` on `windows-11-arm`; both use `ci-msvc`. The existing `runner.trace.paths` case includes Windows junction-alias and trailing-dot/space collision controls.
- The existing Linux `perf` job runs on `ubuntu-26.04` for pull requests. It builds same-source trace-loop ON/OFF runners, executes the CPU-backed Cachegrind measurement, and embeds `trace_cost` in the PR-only `quality-perf` artifact. The artifact receipt must bind to the actual checked-out source.
- The owner authorized at most one draft validation PR for the initial automatic Windows x64, Windows arm64 and Linux Cachegrind CI. It must remain unmerged; workflow dispatch, hosted retries, a second PR and release publication are outside scope. The initial-run observation limit is 75 minutes; any in-flight run must be preserved at the cap.
- With no qualifying existing results, the next step is the already-authorized validation-PR path. Both requirements remain pending until complete, exact-source receipts pass the executable gate and the normal phase verifier reconsiders them.

No CI run was started during this inventory. There are no queue, duration or cancellation observations yet; inspect the single authorized run and record whether its evidence supports any safe efficiency change.

## Task 1 validation

| Check | Exit | Result |
|---|---:|---|
| `python3 build/phase27-evidence/validate.py inventory` | 0 | Exactly one pending row per required receipt; all source/identity fields satisfy the inventory contract. |
| `python3 build/phase27-evidence/validate.py sanity` | 0 | One-off judge accepts the valid fixture and rejects missing-row/source, Windows identity/test, sample-drift and unrounded-threshold faults. |
| Task 1 automated command in `27-04-PLAN.md` | 0 | Scoped committed/index/worktree planning whitespace and untracked evidence checks pass; clean/fault no-index controls distinguish trailing whitespace. |

## Publication authority record

Authority selection: validation-pr
Authority status: granted
Authority source: Forwarded owner authorization dated 2026-11-17 in the orchestrator execution dispatch.
Authority answer: Forwarded owner response, relayed by the orchestrator: one draft validation PR is authorized before Phase 27 verification passes, only to trigger the existing Windows x64, Windows arm64 and Linux Cachegrind CI; keep it unmerged; no manual dispatch or hosted retry.
Authority branch: milestone/v10.0-headless-trace
Authority head: 6cf68302d8158a37d7d724420aaa65211073256e
Authority date: 2026-11-17
Authority scope: one draft validation PR; initial automatic CI only; no dispatch/rerun/merge/release

## Authorized initial automatic run

| Item | Observed value |
|---|---|
| Draft PR | [#77](https://github.com/szTheory/muhtari/pull/77) — open and draft, base `main` at `89b6ff82812cd5be873e9f90d32ac55fded8694b`; branch head `fbf166842e13977148d6a6d42b2e51fbe48a7445`. |
| Workflow | [Run 44001040567](https://github.com/szTheory/muhtari/actions/runs/44001040567) — `pull_request`, attempt 1; started `2026-11-17T02:10:40Z`, completed `2026-11-17T02:53:35Z`, conclusion `failure`; elapsed 42m55s. The run API head is the branch head above. |
| Built checkout | PR merge ref `070f65f10d1cc2f62aba63ad3e2f931d2e5987c7`; read-only fetched. `git diff --quiet c60c192b9088b5f21fc199d574de17e89e5b4c94 070f65f10d1cc2f62aba63ad3e2f931d2e5987c7 -- . ':(exclude).planning'` — exit 0. Both Windows `commit.txt` artifacts equal this SHA; do not substitute the branch SHA. |
| Windows x64 | Job 133764925186 on `windows-2025` succeeded. `MSVC environment`, `Build, test, package`, `Every test ran`, and `The commit built` succeeded. Current-source `cmake --workflow --preset ci-msvc` output reports `runner.trace.paths ... Passed` and `100% tests passed`. Artifact `hashes-windows-x64` id 14293172905 binds `commit.txt` to the built SHA; `quality-windows-x64` id 14294050306. |
| Windows arm64 | Job 133764925225 on `windows-11-arm` succeeded. The same required build/test steps succeeded; current-source `runner.trace.paths` passed and output reports `100% tests passed`. Artifact `hashes-windows-arm64` id 14294257391 binds `commit.txt` to the built SHA; `quality-windows-arm64` id 14294377149. |
| Linux Cachegrind | Required `perf` job 133764925116 on `ubuntu-26.04` failed. `Cachegrind` completed, then `Build the runner at head and base` failed with exit 1 at `runner/main.c:1257:36`: GCC reports that the comparison is always false due to the limited range of the data type (`-Werror=type-limits`). The ON/OFF build and measurement steps did not run. No `quality-perf` artifact was produced. |
| Downstream quality gate | `quality` job 133782140250 failed because `tests/cmake/quality_join.cmake:20` reported `quality_join: exact artifact set required`; the perf receipt was absent. `CI required` failed as a consequence. |
| Other CI results | `hygiene`, `title`, `changes`, both macOS builds, and both Windows builds succeeded. Linux x64/arm64, both Ubuntu nofp jobs, and the library job also failed on the same `runner/main.c:1257:36` warning. ASan failed at `runner/main.c:841` because Clang did not declare `realpath`; BE32 failed at `tests/runner/test_save.c:308` on GCC `-Werror=mismatched-dealloc` for `test_close`. `wasm32`, `hash-equality`, and `retroarch` were skipped after upstream failures. |
| Queue and critical path | Build jobs began 1–15 seconds after the run started. Windows x64 lasted 19m30s; Windows arm64 lasted 42m27s and was the critical path. Its `Quality observations and independent base` step took 25m42s; the corresponding x64 step took 12m55s. No jobs were cancelled. |
| Efficiency disposition | One run shows a long arm64 quality-observation step but does not establish a safe topology, timeout, or cache change. No workflow efficiency change is supported without comparable multi-run evidence. |
| Observation and publication limits | The run finished before the `2026-11-17T03:25:40Z` observation cap. Exactly one push and one draft PR occurred; both `authority --publish` checks passed immediately before the push and PR creation. The PR remains unmerged. No workflow dispatch, hosted retry, rerun, second PR, or release action occurred. |

## Task 2 validation and disposition

- Final `inventory`, `sanity`, and `authority` validators — each exit 0.
- Initial `python3 build/phase27-evidence/validate.py receipts` — exit 1 because this GitHub CLI rejected ANSI escape sequences in a Windows log despite HTTP 200. After confirming `gh api --allow-escape-sequences` support and adding that transport option to the ignored helper, the unchanged validation checks passed both Windows rows then failed closed at the Linux job.
- `python3 build/phase27-evidence/validate.py receipts` after the transport adjustment — exit 1, `required job failed/skipped`; preceding output independently passed Windows x64 (job 133764925186) and arm64 (job 133764925225).
- `python3 build/phase27-evidence/validate.py receipts --accepted` — exit 1, `missing/unaccepted actual receipt: windows-x64`. The combined three-receipt gate did not pass, so no row was marked accepted.
- Final Task 2 scoped whitespace/fault-control command followed by `scripts/hygiene.sh --tree` — exit 0.
- Final Task 2 scoped whitespace/fault-control command followed by `scripts/hygiene.sh --tree` — exit 0.
- `27-TRACE-ACCEPTANCE.md` was not changed: TRACE-03 and TRACE-06 remain pending. The normal phase verifier and existing `gaps_found` shipping stop remain authoritative. No source fix, longer timeout, hosted retry, second PR, or release action is authorized by this plan.
