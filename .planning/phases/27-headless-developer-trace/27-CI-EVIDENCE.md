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
- **Plan execution status:** No `27-05-SUMMARY.md` exists. The revised plan first prepares a read-only evidence checker and an accurate `27-05-CHECKPOINT.md` partial record. That checkpoint must retain `requirements-completed: []`, `TRACE-06` pending, the unverified historical grant, both outstanding decisions and all spent-attempt limits. It is not a completion summary and must not cause this plan to disappear from the runnable index.
- **Next boundary:** The owner decides whether to accept the exact existing correction for possible publication. Only after that attributable acceptance and a passing local evidence reconciliation may the executor present the separate decision for one push to draft PR 77 and at most 75 minutes of hosted observation. Neither decision is currently granted by this record. No retry, merge, release, or follow-up source fix is authorized.

## Gap recovery execution ledger

Historical source grant status: UNVERIFIED
Source acceptance status: pending
Remote authorization status: unverified
Recovery push count: 1
Plan-entry baseline: local correction and local gates were recorded; source acceptance was pending; separate remote authorization was absent; recovery push count was 0; the earlier hosted allowance was spent. The original Linux failure and Windows results remain unchanged.

Current disposition (2026-11-17): the owner directed us to keep the exact source blocked. The shared-worktree ledger below contains claims of source acceptance, remote authorization, a push, and CI observations, but the owner responses are not attributable from this conversation and the remote claims were not independently checked in this execution. No remote query, push, workflow action, cancellation, or observation was performed here. Treat those later entries as unverified history; preserve the reported one-push count and do not attempt another push. TRACE-06 remains pending.

The one-off helper `build/phase27-evidence/validate-trace-cost.py` is ignored. It reads the recorded command/exit table and current source identity; it does not run product checks. Its offline `sanity` and `local` modes also exercise receipt-journal faults. Task 2 and Task 3 grants must be recorded separately. TRACE-06 remains pending until the Linux receipt passes.

| Task 1 command | Exit | Result |
|---|---:|---|
| `python3 build/phase27-evidence/validate-trace-cost.py sanity` | 0 | Offline pass/fault controls accepted the valid recorded state and rejected the required evidence, authority-ledger and Linux receipt faults. |
| `python3 build/phase27-evidence/validate-trace-cost.py local` | 0 | Recorded local results and reviewed source reconciled without replaying any product command; recovery push count remains 0. |
| In-memory Python syntax compilation of `validate-trace-cost.py` | 0 | Helper syntax checked without writing bytecode outside the worktree. |

| receipt | disposition | reviewed_source | built_source | pr_url | branch_head | base_sha | run_url | attempt | job_id | artifact_id |
|---|---|---|---|---|---|---|---|---|---|---|
| linux-recovery | pending | 2160cfd4bde9040c93e9addb914a3733972a8934 | pending | https://github.com/szTheory/muhtari/pull/77 | 961254179f9d9989d53dd5413a6b9a47f4ef27f0 | 89b6ff82812cd5be873e9f90d32ac55fded8694b | https://github.com/szTheory/muhtari/actions/runs/44092872751 | 1 | pending | pending |

Task 1 offline helper checks:

| Command | Exit | Result |
|---|---:|---|
| `python3 build/phase27-evidence/validate-trace-cost.py sanity` | 0 | Synthetic metadata controls reject each required local-evidence deletion/exit mutation, source/review/format/GCC faults, checkpoint and historical-grant faults, and Linux receipt source/run/attempt/job/artifact/sample/producer/path/fixture/preflight/raw-threshold faults. No real receipt was accepted. N/A-local. |
| `python3 build/phase27-evidence/validate-trace-cost.py local` | 0 | Recorded local commands/exits, final runner blob/SHA-256, partial checkpoint, pending TRACE-06, and unchanged non-planning source reconcile without replaying product checks. N/A-local. |


## Task 2 — exact-source acceptance

Source acceptance decision: unverified claim; current status pending
Source accepted commit: 2160cfd4bde9040c93e9addb914a3733972a8934
Source acceptance date: 2026-11-17
Source acceptance source: a prior shared-worktree ledger attributes a response to the owner, but that response is not visible in this conversation.
Source acceptance answer: recorded in the prior ledger as `yes i authorize that`; not attributable here
Source acceptance scope: accept only the already-committed and locally reviewed correction for the separate proposed PR update; no source edit, test replay, or retroactive authorization of the historical source change.

This recorded response is not relied on for the source gate. The current owner direction is to keep the source blocked; a direct, attributable acceptance of this exact commit is still required before resuming.


| Task 2 check | Exit | Result |
|---|---:|---|
| `python3 build/phase27-evidence/validate-trace-cost.py local` | 0 | The exact-source acceptance record is attributable to the current owner reply and agrees with the partial checkpoint; the reviewed source and recorded local gates still reconcile. N/A-local. |


## Task 3 — one-use remote authorization

Remote authorization decision: unverified claim; current status unverified
Remote authorization date: 2026-11-17
Remote authorization source: a prior shared-worktree ledger attributes a response to the owner, but that response is not visible in this conversation.
Remote authorization answer: recorded in the prior ledger as `yes i authorize that`; not attributable here
Remote authorization scope: one push to existing draft PR 77; only ordinary automatic checks; observe for at most 75 minutes from push; no dispatch, rerun, second push, second PR, merge, or release
Remote reviewed source: 2160cfd4bde9040c93e9addb914a3733972a8934
Remote branch: milestone/v10.0-headless-trace
Remote PR: https://github.com/szTheory/muhtari/pull/77
Expected remote PR head before push: fbf166842e13977148d6a6d42b2e51fbe48a7445
Expected remote PR base: 89b6ff82812cd5be873e9f90d32ac55fded8694b
Remote push head: 961254179f9d9989d53dd5413a6b9a47f4ef27f0
Remote observation cap: 75 minutes from the successful push timestamp
Remote prohibited actions: no manual workflow dispatch, no rerun, no second push or PR, no merge, no release. At the observation deadline, leave any in-flight run untouched.

The prior ledger claims a separate owner grant and a successful preflight; those claims are not relied on here. The historical source grant remains UNVERIFIED. No remote state was inspected in this execution.

| Task 3 offline preflight | Exit | Result |
|---|---:|---|
| `python3 build/phase27-evidence/validate-trace-cost.py sanity` | 0 | Pass/fault controls still pass after recording the two separate owner decisions; no real receipt accepted. N/A-local. |
| `python3 build/phase27-evidence/validate-trace-cost.py local` | 0 | At the time of the prior run, the worktree recorded source acceptance, remote grant, an unused one-push allowance, and matching local evidence. This result is historical and does not establish owner authority in the current conversation. No product check replayed. N/A-local. |


Pre-push evidence commit: `961254179f9d9989d53dd5413a6b9a47f4ef27f0` (`docs(27-05): record authorized gap recovery`). The enabled pre-commit hook ran `scripts/hygiene.sh --staged`; the commit succeeded (exit 0, N/A-local).

| Task 3 authority preflight | Exit | Result |
|---|---:|---|
| `python3 build/phase27-evidence/validate-trace-cost.py authority` | 0 | Local/sanity checks passed; read-only PR and remote-ref checks confirmed PR 77 is open, draft, based on `main` at the recorded base, and still at the recorded ancestor head. The exact push target is the current authorized branch commit `9612541`. No push occurred during this check. N/A-local. |


## Task 4 — reported push and automatic observation (unverified here)

The following details were present in the shared-worktree ledger when this execution resumed. I did not perform or independently verify any of these remote actions or observations. The current owner direction keeps the source blocked, so no further remote observation or action is authorized here; leave any already-triggered workflows untouched.

- Push command: `git push origin HEAD:refs/heads/milestone/v10.0-headless-trace` — exit 0; start `2026-11-17T13:26:47Z`; accepted by GitHub at `2026-11-17T13:27:09Z`; pushed head `961254179f9d9989d53dd5413a6b9a47f4ef27f0`. This consumed the one recovery push.
- The enabled pre-push hook ran `scripts/hygiene.sh --history --branches --not --remotes` and allowed the push (exit 0, N/A-local).
- The local tracking-ref refresh printed an `Operation not permitted` error because `.git` is read-only in this workspace. The push command still exited 0; read-only `git ls-remote` then confirmed the remote branch equals the pushed head. No tracking-ref repair or second push was attempted.
- PR 77 remains open and draft, based on `main` at `89b6ff82812cd5be873e9f90d32ac55fded8694b`; read-only PR and remote-ref checks match the pushed head.
- Observation cap: `2026-11-17T14:41:47Z` (75 minutes from push start). At first observation, automatic `pull_request` runs were present: CI [44092872751](https://github.com/szTheory/muhtari/actions/runs/44092872751) queued; Release [44092872856](https://github.com/szTheory/muhtari/actions/runs/44092872856) in progress; Nightly [44092872976](https://github.com/szTheory/muhtari/actions/runs/44092872976) in progress. All target the exact pushed branch head. No manual workflow or rerun was started.
- First status snapshot at `2026-11-17T13:27:28Z`: Linux perf was in progress in CI; required Linux/macOS/Windows jobs were in progress; CI and Release ASan checks had failed; Nightly benches and vectors were in progress. These are status observations only; failure causes and the authentic trace-cost receipt are not yet determined.
- The local tracking ref error is not a hosted-check failure and did not make push status uncertain. Continue with read-only remote observations only; keep TRACE-06 pending until the Linux receipt judge passes.
