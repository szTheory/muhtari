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

## Gap source authorization and local baseline

- **Historical authority claim — UNVERIFIED:** An earlier executor attributed “Okay, yes, I authorize that” to an owner response dated 2026-11-16 (America/New_York). No directly attributable underlying response is available in the visible owner/session record. This artifact is the only provenance for that claim; it is not proof of authorization and must not authorize any action or be copied into a granted-authority field.
- **Current authority status:** Source acceptance is blocked. The correction already exists locally, but the owner must explicitly accept that exact committed correction for possible publication before the separate remote authorization decision. Such acceptance does not retroactively establish the historical grant, authorize further source edits, or authorize publication/hosted work. Preserve the existing local commit and results without replaying them.
- **Pre-fix commit:** `2027e5e3c83ce0f33c4e30b00d80b91ef5adae91` on `milestone/v10.0-headless-trace`.
- **Pre-edit reconciliation:** The worktree was clean; `git diff --quiet c60c192b9088b5f21fc199d574de17e89e5b4c94 HEAD -- . ':(exclude).planning'` exited 0, and `runner/main.c` had no diff from the pre-fix commit.
- **Pre-edit focused controls:** `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^runner[.]trace[.](arguments|bounds|oracle)$'` — exit 0; all three existing controls passed.
- **Local compiler qualification:** `/usr/bin/gcc --version` reports Apple Clang 21.0.0 targeting arm64-apple-darwin25.6.0. No GNU GCC executable was found among `gcc-12` through `gcc-15`, `aarch64-linux-gnu-gcc`, or `x86_64-linux-gnu-gcc`. Therefore the hosted GCC failure cannot be reproduced locally; this host's compiler result will not be presented as GCC evidence.

## Gap source correction and local checks

The recorded local product change is limited to `check_options`: after applying the default trace limit, copy it to `const size_t trace_limit` and use that same-type value for the three existing representability comparisons. The hard maximum, diagnostics, capture-memory and output bounds, allocation overflow check, API, serialized state, and trace dispatch remain unchanged. The existing final review disposition for Windows path handling remains unchanged; its deferred extended-path warning is unrelated. The results below record work already performed, not an assertion of verified historical authority or permission to repeat it.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci` | 0 | CI build configured. |
| Pre-edit `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^runner[.]trace[.](arguments|bounds|oracle)$'` | 0 | All three baseline controls passed before the source edit. |
| `cmake --build build/ci --target muhtari-run muhtari-test-trace muhtari-test-save-runner` | 0 | Final formatted source and existing runner test targets rebuilt. |
| Post-edit `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^runner[.]trace[.](arguments|bounds|oracle)$'` | 0 | All three argument, bounds, and oracle controls passed after rebuilding the final formatted source. |
| `cmake --workflow --preset ci` | 0 | Full local workflow passed: 736 CI tests, package step, and all 8 release checks. This run preceded the formatter's whitespace-only line-wrap correction; the final formatted source was rebuilt and its focused controls passed afterward. |
| First `cmake --workflow --preset hygiene` | 8 | The source delta compiled; 29 of 30 checks passed. `hygiene.format` rejected only the wrapped comparison at `runner/main.c:1259`. |
| `/opt/homebrew/opt/llvm@21/bin/clang-format --version` | 0 | Project-pinned formatter is Homebrew clang-format 21.1.8. |
| `/opt/homebrew/opt/llvm@21/bin/clang-format -i --lines=1253:1265 runner/main.c` | 0 | Applied only the required line-wrap formatting to the changed comparison. |
| Final `cmake --workflow --preset hygiene` | 0 | All 30 hygiene and policy checks passed, including format, analyzer, and tree checks. |
| `scripts/hygiene.sh --tree` | 0 | Repository hygiene passed on the final source and evidence state. |
| Final `git diff --check -- runner/main.c .planning/phases/27-headless-developer-trace/27-CI-EVIDENCE.md` | 0 | No whitespace errors in the scoped changes. |

**Review:** The scoped source diff is type-correct for the C17 runner. `trace_limit` is a `size_t` on both sides of the portability comparison, which avoids GCC's range-based diagnostic without disabling warnings; 32-bit allocation and output limits remain meaningful. The reviewed source has no behavior, public API, or state-format change. `git hash-object runner/main.c` is `f634cfcc9b2739d44cc9a9d8de9f2a101f3e6d38`; SHA-256 is `0dcf2d4eefcf701b73feeb43a0af9a46aa7abf49601d91c3315764c64010c4d4`.

No local GNU GCC reproduction or Cachegrind measurement is claimed. TRACE-06 remains pending until a separately authorized PR update yields the authentic corrected-source Linux receipt. No publication or hosted action occurred in this scope.

## Gap resume baseline and remaining decisions

- **Committed correction / reviewed source:** `2160cfd4bde9040c93e9addb914a3733972a8934` (`fix(27-05): correct trace limit comparison type`), with `runner/main.c` blob `f634cfcc9b2739d44cc9a9d8de9f2a101f3e6d38` and SHA-256 `0dcf2d4eefcf701b73feeb43a0af9a46aa7abf49601d91c3315764c64010c4d4`. Its parent is the pre-fix commit above. The historical commit message's authorization wording does not verify the missing owner response.
- **Local result status:** The build, focused controls, full CI, final hygiene, tree hygiene, scoped whitespace and review results above are recorded local evidence. Reconcile their command/exit/source coverage before any remote checkpoint; do not rerun them, reapply the correction, amend the commit, or perform another source fix under this plan. Full CI preceded only the documented line-wrap formatting; the final formatted source was rebuilt, focused controls and final hygiene passed, and review covers the final blob. This is reuse of recorded evidence, not a fresh CI or GCC result.
- **Plan execution status:** No `27-05-SUMMARY.md` exists. The partial checkpoint retains `requirements-completed: []`, TRACE-06 pending, the unverified historical source grant and reported one-push allowance. Current source acceptance and the separately authorized read-only observation are recorded with their own scope; the failed perf job does not make this plan complete.
- **Next boundary:** The read-only observation completed with no Cachegrind receipt. The prior push authorization claim remains unverified, and the one-push allowance is treated as spent. Any remedy for missing pinned inputs or independent CI failures needs a new gap plan and fresh scope; this plan authorizes no source fix or hosted retry.

## Gap recovery execution ledger

Historical source grant status: UNVERIFIED
Source acceptance status: accepted
Remote authorization status: read-only observation granted
Reported one-push authorization status: unverified
Recovery push count: 1
Plan-entry baseline: local correction and local gates were recorded; source acceptance was pending; separate remote authorization was absent; recovery push count was 0; the earlier hosted allowance was spent. The original Linux failure and Windows results remain unchanged.

Current disposition (2026-11-17): the owner accepted the exact source commit `2160cfd4bde9040c93e9addb914a3733972a8934` and separately authorized read-only inspection of the three reported automatic runs and Linux perf evidence until `2026-11-17T14:41:47Z`. That current grant does not authorize or ratify the prior push. The prior ledger claims a separate one-push grant and a successful push, but that authorization is not attributable in this conversation; the reported push count remains 1 and is treated as spent. Read-only GitHub API queries confirmed all three runs completed with failures and that the CI perf job failed before measurement; the `quality-perf` artifact list was empty. TRACE-06 remains pending. No dispatch, rerun, second push, merge or release is authorized.

The one-off helper `build/phase27-evidence/validate-trace-cost.py` is ignored. It reads the recorded command/exit table and current source identity; it does not run product checks. Its offline `sanity` and `local` modes also exercise receipt-journal faults. Task 2 and Task 3 grants must be recorded separately. TRACE-06 remains pending until the Linux receipt passes.

| Task 1 command | Exit | Result |
|---|---:|---|
| `python3 build/phase27-evidence/validate-trace-cost.py sanity` | 0 | Offline pass/fault controls accepted the valid recorded state and rejected the required evidence, authority-ledger and Linux receipt faults. |
| `python3 build/phase27-evidence/validate-trace-cost.py local` | 0 | Recorded local results and reviewed source reconciled without replaying any product command; recovery push count remains 0. |
| In-memory Python syntax compilation of `validate-trace-cost.py` | 0 | Helper syntax checked without writing bytecode outside the worktree. |

| receipt | disposition | reviewed_source | built_source | pr_url | branch_head | base_sha | run_url | attempt | job_id | artifact_id |
|---|---|---|---|---|---|---|---|---|---|---|
| linux-recovery | failed | 2160cfd4bde9040c93e9addb914a3733972a8934 | 329eed17cb7a78a28d5d8143d1075b1bdde22037 | https://github.com/szTheory/muhtari/pull/77 | 961254179f9d9989d53dd5413a6b9a47f4ef27f0 | 89b6ff82812cd5be873e9f90d32ac55fded8694b | https://github.com/szTheory/muhtari/actions/runs/44092872751 | 1 | 134072368332 | pending |

Task 1 offline helper checks:

| Command | Exit | Result |
|---|---:|---|
| `python3 build/phase27-evidence/validate-trace-cost.py sanity` | 0 | Synthetic metadata controls reject each required local-evidence deletion/exit mutation, source/review/format/GCC faults, checkpoint and historical-grant faults, and Linux receipt source/run/attempt/job/artifact/sample/producer/path/fixture/preflight/raw-threshold faults. No real receipt was accepted. N/A-local. |
| `python3 build/phase27-evidence/validate-trace-cost.py local` | 0 | Recorded local commands/exits, final runner blob/SHA-256, partial checkpoint, pending TRACE-06, and unchanged non-planning source reconcile without replaying product checks. N/A-local. |


## Task 2 — exact-source acceptance

Source acceptance decision: accepted
Source accepted commit: 2160cfd4bde9040c93e9addb914a3733972a8934
Source acceptance date: 2026-11-17
Source acceptance source: current owner response in this conversation, replying to the direct request to accept this exact commit.
Source acceptance answer: yes i accept it
Source acceptance scope: accept only the already-committed and locally reviewed correction for the separate proposed PR update; no source edit, test replay, or retroactive authorization of the historical source change.

This response satisfies the source acceptance gate for the existing correction only. It does not grant separate remote authority or retroactively authorize the historical source change.


| Task 2 check | Exit | Result |
|---|---:|---|
| `python3 build/phase27-evidence/validate-trace-cost.py local` | 0 | The exact-source acceptance record matches the direct owner response and partial checkpoint; the reviewed source and recorded local gates still reconcile. N/A-local. |


## Task 3 — one-use remote authorization

Remote authorization decision: read-only observation granted; prior one-push authorization claim unverified
Remote authorization date: 2026-11-17
Remote authorization source: current owner response in this conversation to the question naming the three existing automatic PR runs, Linux perf evidence, read-only scope, and fixed deadline.
Remote authorization answer: Authorize read-only inspection
Remote authorization scope: read-only inspect statuses for runs 44092872751, 44092872856 and 44092872976 and the Linux perf job/receipt until 2026-11-17T14:41:47Z; no push, dispatch, rerun, cancellation, merge or release
Reported one-push authorization source: prior worktree ledger only; no attributable owner response for that push is visible in this conversation.
Remote reviewed source: 2160cfd4bde9040c93e9addb914a3733972a8934
Remote branch: milestone/v10.0-headless-trace
Remote PR: https://github.com/szTheory/muhtari/pull/77
Expected remote PR head before push: fbf166842e13977148d6a6d42b2e51fbe48a7445
Expected remote PR base: 89b6ff82812cd5be873e9f90d32ac55fded8694b
Remote push head: 961254179f9d9989d53dd5413a6b9a47f4ef27f0
Remote observation cap: 75 minutes from the successful push timestamp
Remote prohibited actions: no manual workflow dispatch, no rerun, no second push or PR, no merge, no release. At the observation deadline, leave any in-flight run untouched.

The current owner reply authorizes observation only; it does not supply prospective or retroactive authority for the reported push. The historical source grant remains UNVERIFIED. The three run identities, job status results and CI `quality-perf` artifact inventory were inspected through read-only GitHub queries. PR head/base metadata were not checked in this observation.

| Task 3 offline preflight | Exit | Result |
|---|---:|---|
| `python3 build/phase27-evidence/validate-trace-cost.py sanity` | 0 | Pass/fault controls still pass after recording the two separate owner decisions; no real receipt accepted. N/A-local. |
| `python3 build/phase27-evidence/validate-trace-cost.py local` | 0 | At the time of the prior run, the worktree recorded source acceptance, remote grant, an unused one-push allowance, and matching local evidence. This result is historical and does not establish owner authority in the current conversation. No product check replayed. N/A-local. |


Pre-push evidence commit: `961254179f9d9989d53dd5413a6b9a47f4ef27f0` (`docs(27-05): record authorized gap recovery`). The enabled pre-commit hook ran `scripts/hygiene.sh --staged`; the commit succeeded (exit 0, N/A-local).

| Task 3 authority preflight | Exit | Result |
|---|---:|---|
| `python3 build/phase27-evidence/validate-trace-cost.py authority` | 0 | Local/sanity checks passed; read-only PR and remote-ref checks confirmed PR 77 is open, draft, based on `main` at the recorded base, and still at the recorded ancestor head. The exact push target is the current authorized branch commit `9612541`. No push occurred during this check. N/A-local. |


## Task 4 — reported push and read-only observation

The prior worktree ledger reports one push of the reviewed correction and attributes a separate one-push grant to an owner reply, but that reply is not attributable in this conversation. The current owner response authorized read-only inspection of the already-reported runs and Linux perf evidence only, until the fixed cap. It did not authorize or ratify a push. This remains separate from the historical source-grant claim, which is UNVERIFIED.

- Prior ledger report: `git push origin HEAD:refs/heads/milestone/v10.0-headless-trace` — claimed exit 0; start `2026-11-17T13:26:47Z`; claimed acceptance at `2026-11-17T13:27:09Z`; proposed pushed head `961254179f9d9989d53dd5413a6b9a47f4ef27f0`. The current read-only observation did not verify the push command or remote ref. Treat the one-push allowance as consumed; do not push again.
- Prior ledger records that the enabled pre-push hook ran `scripts/hygiene.sh --history --branches --not --remotes` and allowed the push (exit 0, N/A-local); this execution did not validate that push.
- Prior ledger records a local tracking-ref refresh error and subsequent `git ls-remote` confirmation. This execution did not query the remote branch and did not attempt any tracking-ref repair or push.
- Prior ledger says PR 77 remains open and draft, based on `main` at `89b6ff82812cd5be873e9f90d32ac55fded8694b`; PR metadata was not queried in this observation.
- Observation cap: `2026-11-17T14:41:47Z` (75 minutes from push start). At first observation, automatic `pull_request` runs were present: CI [44092872751](https://github.com/szTheory/muhtari/actions/runs/44092872751) queued; Release [44092872856](https://github.com/szTheory/muhtari/actions/runs/44092872856) in progress; Nightly [44092872976](https://github.com/szTheory/muhtari/actions/runs/44092872976) in progress. All target the exact pushed branch head. No manual workflow or rerun was started.
- First status snapshot at `2026-11-17T13:27:28Z`: Linux perf was in progress in CI; Linux/macOS/Windows jobs were in progress; CI and Release ASan checks had failed; Nightly benches and vectors were in progress. Subsequent read-only logs identified the failure causes below.
- The prior ledger says the local tracking-ref refresh error was not a hosted-check failure and that later `git ls-remote`/PR reads matched the pushed head and base. This execution did not check those values; no tracking-ref repair or second push was attempted.

### Recovery run 44092872751 result

The prior ledger records read-only checks that PR 77 remained open and draft at the recorded base/head and that the synthetic merge tree matched the reviewed correction. This execution did not inspect PR metadata or compare that tree. Its run query associated the three automatic runs with commit `961254179f9d9989d53dd5413a6b9a47f4ef27f0`.

Perf job `134072368332` failed at 13:28:20Z, 54 seconds after it started. Cachegrind and the head/base runner builds passed, but “Build trace-loop variants and generated CPU workload” failed because the checked-out build lacked pinned Wasmtime, 7800basic and OpenBIOS inputs. The hooks-off gate, instruction report and artifact upload were skipped. The run’s artifact inventory contains no `quality-perf`; the receipt row above records the actual merge checkout and job, with artifact ID pending because no receipt was produced. TRACE-06 remains pending. No source fix or hosted retry is allowed by this plan.

The prior ledger reports that the CI workflow completed at 14:09:40Z with overall failure. It records both nofp jobs, hygiene, both Windows jobs and both macOS jobs passing; Linux x64, Linux arm64, BE32, library and ASan failing during builds; and the quality aggregate failing after required artifacts were missing. This execution confirmed the overall CI failure and perf-job result, but did not inspect those other jobs' logs.

The prior ledger reports Nightly completed with failure, with equality and fuzz failures, and Release completed with failure, including a macOS x64 timeout and failed quality aggregation. This execution confirmed only the overall Nightly and Release statuses and did not inspect their logs. Release perf was skipped according to the prior record, so it cannot supply the trace-cost receipt.

The prior ledger reports the matrices launched within seconds, gives Windows arm64 as the critical path for CI and Release, and reports no cancelled jobs. Those queue and duration details were not rechecked in this observation. These single-run claims do not support a safe timeout, cache, topology or concurrency change.

| Recovery observation | Command | Exit | Result |
|---|---|---:|---|
| CI job and step status | `gh run view 44092872751 --repo szTheory/muhtari --json jobs` | 0 | Completed at 2026-11-17T14:09:40Z with failure; per-job outcomes and timestamps recorded above. Run URL: https://github.com/szTheory/muhtari/actions/runs/44092872751 |
| CI artifact inventory | `gh api repos/szTheory/muhtari/actions/runs/44092872751/artifacts` | 0 | No `quality-perf` artifact; workflow artifacts include hygiene and platform build/evidence artifacts. |
| Failed perf job log | `gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/134072368332/logs` | 0 | Confirms missing pinned inputs stopped fixture generation before the hooks-off measurement. |
| CI quality aggregate log | `gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/134097119950/logs` | 0 | Confirms `quality_join: exact artifact set required` with `PERF_REQUIRED=true` and no `quality-perf` input. |
| Release job and step status | `gh run view 44092872856 --repo szTheory/muhtari --json jobs` | 0 | Completed at 2026-11-17T14:11:33Z with failure; Windows/macOS outcomes and the 43m46s Windows arm64 critical path recorded above. Run URL: https://github.com/szTheory/muhtari/actions/runs/44092872856 |
| Release artifact inventory | `gh api repos/szTheory/muhtari/actions/runs/44092872856/artifacts` | 0 | Confirms outputs from failed lanes are absent; the workflow produced no Cachegrind receipt. |
| Release macOS x64 log | `gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/134072364570/logs` | 0 | Identifies the single test failure as `fuzz.regress.run` timeout after 735/736 tests passed. |
| Release quality log | `gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/134098372888/logs` | 0 | Confirms `PERF_REQUIRED=false` and exact-artifact-set failure after lane artifacts were missing. |
| Release equality log | `gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/134072291115/logs` | 0 | Confirms the same GCC `-Werror=mismatched-dealloc` diagnostic in `tests/runner/test_save.c:308`. |
| Automatic run completion and cancellation status | `gh run view <run-id> --repo szTheory/muhtari --json jobs` for runs 44092872751, 44092872856 and 44092872976 | 0 | All three automatic runs completed before the cap; no job conclusion was `cancelled`. |
| PR state | Prior ledger: `gh pr view 77 --repo szTheory/muhtari --json state,isDraft,headRefOid,baseRefName,baseRefOid,url` | 0 | Prior record says PR remains open/draft at the recorded head/base; PR metadata was not checked in this observation. |

### Current execution read-only observations

The repository guidance's `scripts/ci_monitor.cjs` is absent from this checkout. To keep the observation read-only without using the GitHub CLI directly, this execution used the connected GitHub read tools. Each call returned successfully; no remote state was changed.

| Check | Read-only tool call | Result |
|---|---|---|
| Runs for the reported head | `github_fetch_commit_workflow_runs(repo_full_name="szTheory/muhtari", commit_sha="961254179f9d9989d53dd5413a6b9a47f4ef27f0")` | CI 44092872751, Release 44092872856 and Nightly 44092872976 were all completed with failure; the response also listed Dependabot auto-merge as skipped. |
| Run job statuses | `github_fetch_workflow_run_jobs(run_id=44092872751/44092872856/44092872976)` | All three job lists were complete. CI perf job 134072368332 failed at `Build trace-loop variants and generated CPU workload`; cost and instruction-measurement steps were skipped. |
| Linux perf artifact | `github_fetch_workflow_run_artifacts(run_id=44092872751, name="quality-perf")` | Empty artifact list; no Cachegrind receipt. |
| Linux perf log | `github_fetch_workflow_job_logs(job_id=134072368332)` | Confirms fixture setup stopped because pinned Wasmtime, 7800basic and OpenBIOS inputs were missing. The hooks-off cost measurement did not run. |

## Plan 27-06 — local fixture prerequisite remedy

The reviewed local source is commit `4630288226b705b4eb1f4177b3736d8ac0b072ca` (`ci(27-06): provision perf fixture pins`). Its `.github/workflows/ci.yml` blob is `95163d421b8ae3ac244bd2ecb52402d41ec143c3`. The only workflow delta is the four prescribed sequential fetch commands in the existing Linux `perf` job, after the head/base runner build and immediately before the tests-on fixture configure. The step uses `wasmtime-x86_64-linux`, `7800basic-wasm`, `7800openbios-asm` and `rcheevos` at their existing `build/pins/*` destinations. No product source, pin row/digest, measurement producer, raw threshold, artifact contract, test inventory, job condition, timeout or other lane changed.

### Host, tools and pin identities

The local host is Darwin arm64, with CMake 4.4.3, AppleClang 21.0.0.21000101 and Ninja 1.13.2. The fresh configure reused the existing ignored native `build/pins` links; no Linux executable was installed over the host Wasmtime. `build/pins/wasmtime/wasmtime` is a Mach-O arm64 executable reporting Wasmtime 48.0.6 (`72bc45bd1`, 2026-10-14). The existing `7800basic/7800basic.wasm`, `openbios/7800openbios.asm` and `rcheevos/src/rc_libretro.c` prerequisite paths were present. The workflow's Linux x64 Wasmtime and all four unchanged upstream pin identities are:

| Pin | Version/source | SHA-256 |
|---|---|---|
| `wasmtime-x86_64-linux` | 48.0.6 | `a04bbb203271301e923ccc54ebe834145b1595af1ffb457c8e5dbf1baadd7248` |
| `7800basic-wasm` | 0.41 | `62ef0a562541bff3f813c038a4609abc89ab125ebda3a371f57de2821e7a7fe7` |
| `7800openbios-asm` | source `b8bc745a8744` | `ff04da42db52480cf4cd5e11d415a6c55a54b72ab24af9b461cd4dff4e422109` |
| `rcheevos` | 12.1.0, commit `6755915f2bdf6c11c83b3e0e68c4623a39c0dabe` | `c8bb9f66239f1b6c1f66417d26b341ce284ef2475acdc4f6fce0d04ac1678c55` |

The host-native Wasmtime pin row is `wasmtime-aarch64-macos` 48.0.6 with SHA-256 `0b24079ba8c8a23df9cd9a11e40f0b5edc95398a70a7cdec9286133f18701c69`. Existing ignored inputs were reused, not downloaded or overwritten during this check.

### Local command and exit evidence

All URLs are `N/A-local`. The initial `build/phase27-perf-preflight` directory did not exist, so its first configure was fresh. That native Release configure registered the existing exact CI inventory; only `muhtari-run` and `muhtari-test-trace` were built. The generated `trace-cost.a78` ran through the runner and produced the required `diagnostic a5` CPU-execution marker. This proves local configure prerequisites and fixture generation on Darwin arm64 only; it is not Linux execution, Cachegrind measurement or TRACE-06 acceptance.

| Command | Exit | Result |
|---|---:|---|
| `git diff --check 3a4d53c -- .github/workflows/ci.yml` | 0 | Workflow insertion has no whitespace errors. |
| `cmake --preset ci -B build/phase27-perf-preflight -DMUHTARI_TESTS=ON -DMUHTARI_TRACE_LOOP=ON` | 0 | Fresh native CI Release configure succeeded with the existing built and libretro prerequisites. |
| `cmake --build build/phase27-perf-preflight --target muhtari-run muhtari-test-trace` | 0 | Only the requested runner and trace fixture generator targets were built. |
| `cmake -E make_directory build/phase27-perf-preflight/workload && build/phase27-perf-preflight/tests/muhtari-test-trace cost_fixture build/phase27-perf-preflight/workload && build/phase27-perf-preflight/runner/muhtari-run --frames 1 --diagnostic build/phase27-perf-preflight/workload/trace-cost.a78 > build/phase27-perf-preflight/workload/runner-stdout.txt && LC_ALL=C grep -q '^diagnostic a5' build/phase27-perf-preflight/workload/runner-stdout.txt` | 0 | Generated build-owned cartridge executed the expected CPU marker. |
| `cmake --workflow --preset hygiene` | 0 | Existing hygiene workflow passed all 30 checks, including pin integrity and fault control, CI required roll-up controls, timeout comments, formatting, analyzer, tree and firmware checks. |
| `git diff --check 3a4d53c -- .github/workflows/ci.yml` (post-commit tracer gate) | 0 | Committed workflow remains whitespace-clean. |
| `cmake --preset ci -B build/phase27-perf-preflight -DMUHTARI_TESTS=ON -DMUHTARI_TRACE_LOOP=ON` (post-commit tracer gate) | 0 | Reconfigure of the established preflight tree succeeded; this second invocation was not counted as a fresh configure. |
| `cmake --build build/phase27-perf-preflight --target muhtari-run muhtari-test-trace` (post-commit tracer gate) | 0 | Both requested targets remained available; Ninja reported no work. |
| `cmake -E make_directory build/phase27-perf-preflight/workload && build/phase27-perf-preflight/tests/muhtari-test-trace cost_fixture build/phase27-perf-preflight/workload && build/phase27-perf-preflight/runner/muhtari-run --frames 1 --diagnostic build/phase27-perf-preflight/workload/trace-cost.a78 > build/phase27-perf-preflight/workload/runner-stdout.txt && LC_ALL=C grep -q '^diagnostic a5' build/phase27-perf-preflight/workload/runner-stdout.txt` (post-commit tracer gate) | 0 | Fixture regeneration and diagnostic marker check passed again. |
| `git check-ignore build/phase27-perf-preflight/workload/trace-cost.a78 build/phase27-perf-preflight/workload/runner-stdout.txt` | 0 | Generated cartridge and output remain ignored; no ROM, BIOS, state, trace, pin payload or log was added to the tracked tree. |
| `git diff --check 3a4d53c -- .github/workflows/ci.yml .planning/phases/27-headless-developer-trace/27-CI-EVIDENCE.md .planning/phases/27-headless-developer-trace/27-06-CHECKPOINT.md` | 0 | Final workflow/evidence/checkpoint diff has no whitespace errors. |

The local plan task itself produced no Linux Cachegrind receipt, `quality-perf` artifact or completed TRACE-06 result. The previously recorded Linux perf run `44092872751`, attempt 1, job `134072368332`, remains a terminal pre-measurement failure with no receipt. At that checkpoint, the earlier reported push count was treated as spent and its historical authorization remained unverified; no remote work occurred in that local-plan task. The separate fresh hosted continuation and its receipt are recorded below. The unrelated GCC `-Werror=mismatched-dealloc` in `tests/runner/test_save.c:308`, Clang `realpath` declaration failure in `runner/main.c:841`, and macOS `fuzz.regress.run` timeout after 735/736 tests remain separate shipping blockers where required. No timeout, workflow topology, threshold, artifact rule or test inventory changed.

## Fresh hosted continuation — authorized 2026-11-17

The owner replied `yes i authorize that` to the concrete Task 3 proposal. This authorizes one update push of the reviewed workflow commit `4630288226b705b4eb1f4177b3736d8ac0b072ca` to the existing draft PR 77 and read-only observation of only its automatic checks for at most 75 minutes. The cap is `2026-11-17T17:29:13Z`; any in-flight job is left untouched at the cap. This is a new attempt and does not authorize or ratify the historical 27-05 push. Manual dispatch, rerun, another push, merge and release remain excluded.

| Check | Command | Exit | Result |
|---|---|---:|---|
| Authorized fast-forward update | `git push origin 4630288226b705b4eb1f4177b3736d8ac0b072ca:refs/heads/milestone/v10.0-headless-trace` | 0 | Advanced the existing PR branch from `961254179f9d9989d53dd5413a6b9a47f4ef27f0` to the reviewed commit. No force push. |
| PR state and source | `gh pr view 77 --repo szTheory/muhtari --json state,isDraft,headRefName,headRefOid,baseRefName,baseRefOid,url` | 0 | PR 77 remains open and draft; head is `4630288226b705b4eb1f4177b3736d8ac0b072ca`, base is `main` at `89b6ff82812cd5be873e9f90d32ac55fded8694b`. [PR 77](https://github.com/szTheory/muhtari/pull/77). |
| Automatic PR CI run and perf job | `gh run view 44123425194 --repo szTheory/muhtari --json headSha,headBranch,event,url,status,conclusion,createdAt,jobs` | 0 | Run [44123425194](https://github.com/szTheory/muhtari/actions/runs/44123425194) is a `pull_request` run for the authorized head. Required Linux `perf` job `134180732604` succeeded in 2m14s; at the 16:42:06Z snapshot, only Windows arm64 remained in this run. |
| Receipt download | `gh run download 44123425194 --repo szTheory/muhtari --name quality-perf --dir /private/tmp/muhtari-trace-cost-44123425194` | 0 | Downloaded the existing `quality-perf` artifact outside the repository; artifact ID `14347390138`, artifact SHA-256 `bcb5f9896dda1c204ae88c82aad1e935780d6d71a2f969e35d7ea75afb323f92`. |
| Receipt provenance and arithmetic | `jq -e '.source == "https://github.com/szTheory/muhtari/actions/runs/44123425194" and .sha == "fa72f560fa30a52ef5cde2a6083f4289dbc980be" and .base_sha == "89b6ff82812cd5be873e9f90d32ac55fded8694b" and .status == "measured" and .trace_cost.producer == "tests/perf/trace.cmake" and .trace_cost.source_sha == .sha and (.trace_cost.measurement as $m | $m.status == "PASS" and $m.repeats == 2 and $m.threshold_raw == 9600 and $m.delta_raw == ($m.on_raw_delta - $m.off_raw_delta) and $m.delta_raw == 900 and $m.delta_ir_frame == 3 and ([$m.on_ir60,$m.on_ir360,$m.off_ir60,$m.off_ir360] | all(.[]; length == 2 and .[0] == .[1])))' /private/tmp/muhtari-trace-cost-44123425194/quality-perf.json` | 0 | The artifact source is the PR merge checkout `fa72f560fa30a52ef5cde2a6083f4289dbc980be`, which joins authorized head `4630288226b705b4eb1f4177b3736d8ac0b072ca` to recorded PR base `89b6ff82812cd5be873e9f90d32ac55fded8694b`. The producer is `tests/perf/trace.cmake`; all four two-sample series are stable; `status=PASS`, raw delta `900`, threshold `9600`, and delta `3 Ir/frame`. Workload SHA-256 is `98ef67837e87ad14e502fc08c64c551733fa8fdae55d52d9efb84186224b1158`. This is the authentic Linux CPU-backed Cachegrind receipt for TRACE-06. |
| Release automation | `gh run view 44123425418 --repo szTheory/muhtari --json status,conclusion,jobs` | 0 | Automatic Release rehearsal [44123425418](https://github.com/szTheory/muhtari/actions/runs/44123425418) was still in progress at 16:42:06Z on its Windows arm64 lane. Publish and install steps were skipped; no release action occurred. |
| Nightly automation | `gh run view 44123425488 --repo szTheory/muhtari --json status,conclusion,jobs` | 0 | Automatic Nightly run [44123425488](https://github.com/szTheory/muhtari/actions/runs/44123425488) completed with failure. Its fuzz build hit the same Clang `realpath` declaration error at `runner/main.c:841`; equality hit GCC `-Werror=mismatched-dealloc` at `tests/runner/test_save.c:308`. These are separate from the passing perf receipt. |

At the 16:42:06Z snapshot the direct CI and Release runs were still waiting on Windows arm64. The already completed Windows x64 lane passed in both runs (25m55s direct CI; 25m35s Release). Linux x64/arm64 and BE32 failed the same GCC mismatched-deallocation warning; ASan failed because Clang did not declare `realpath`. Linux perf, hygiene, title and changes passed. The current successful receipt establishes TRACE-06's cost result; it does not make the full CI, Release or Nightly run green. No safe timeout, cache, concurrency or topology change is supported by these observations; the arm64 quality-observation step remains the observed long pole and needs comparable multi-run evidence before a workflow change.

### Final automatic workflow results — 2026-11-17T16:57:25Z

All four workflows triggered by the one authorized push were terminal before the 75-minute cap, so observation ended early. They all target the exact authorized PR head. No job was cancelled, no manual workflow or rerun was started, and no further push, merge or release action occurred.

| Workflow | Run | Result | Relevant detail |
|---|---|---|---|
| CI | [44123425194](https://github.com/szTheory/muhtari/actions/runs/44123425194) | Failure, 43m13s | Linux `perf` passed in 2m14s with the receipt above. Windows x64 and arm64, macOS x64 and arm64, both nofp jobs, hygiene, title and changes passed. Library, Linux x64/arm64 and BE32 failed GCC `-Werror=mismatched-dealloc` in `tests/runner/test_save.c:308`; ASan failed because Clang does not declare `realpath` at `runner/main.c:841`. `quality` and `CI required` then failed because `quality_join.cmake` requires the exact complete artifact set. `wasm32`, `retroarch` and `hash-equality` were skipped. |
| Release | [44123425418](https://github.com/szTheory/muhtari/actions/runs/44123425418) | Failure, 41m38s | Its nested Windows x64/arm64 and both macOS lanes passed. Nested library/Linux/BE32/ASan lanes failed with the same diagnostics; equality failed on the GCC mismatched-deallocation warning. The quality join and `CI required` failed after missing lane artifacts. `release-target`, rehearsal, install-line and cleanup were skipped; no release action occurred. |
| Nightly | [44123425488](https://github.com/szTheory/muhtari/actions/runs/44123425488) | Failure, 2m52s | Fuzz build failed on the same Clang `realpath` declaration error; the missing junit/crash output was downstream. BE equality failed on GCC `-Werror=mismatched-dealloc`. |
| Dependabot auto-merge | [44123425289](https://github.com/szTheory/muhtari/actions/runs/44123425289) | Skipped | This was not a Dependabot update; no merge occurred. |

All matrix jobs started 1–15 seconds after run creation, so queue time was negligible. The direct CI critical path was Windows arm64 at 42m42s (job `134180732974`); its `Quality observations and independent base` step took 26m02s. Release's nested arm64 lane took 41m06s, including a 25m06s quality-observation step. The corresponding prior run's arm64 quality step was 25m42s. Windows x64 took 25m55s in CI and 25m35s in Release. These samples again identify execution time, especially the independent-base quality comparison, as the critical path, not queueing. They do not support changing concurrency, timeouts or caches; preserve the workload and gather comparable runs before considering such a change.

The receipt-specific requirement is satisfied by the successful, source-joined Linux `perf` artifact. The overall CI and Release runs remain failed because independent required jobs and the exact-artifact quality joins failed; this is a shipping blocker and does not invalidate the specific cost measurement. Keep those failures visible and do not claim a green full-CI result.

Before the long hosted matrix, the existing cheap local checks ran first: scoped `git diff --check`, a fresh native CI configure, the two requested targets and generated CPU fixture smoke, then the existing 30-check hygiene workflow. No full local product suite was replayed and no new recurring check was added: the existing exact CMake inventory configure, checksum-pinned fetch helper and hygiene fault controls cover the demonstrated missing-input failure.
