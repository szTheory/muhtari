# Phase 27 CI evidence

- **Checked:** 2026-11-17 UTC
- **Branch:** `milestone/v10.0-headless-trace` at `5d516cd1670d1d0fe4a647241c5df972999e508a`
- **Intended PR base:** `main` at local `origin/main` `89b6ff82812cd5be873e9f90d32ac55fded8694b`
- **Reviewed product source:** `c60c192b9088b5f21fc199d574de17e89e5b4c94`
**Source comparison:** `git diff --quiet c60c192b9088b5f21fc199d574de17e89e5b4c94 HEAD -- . ':(exclude).planning'` — exit 0; tracked non-planning source is identical.

## Receipt inventory

| receipt | disposition | reviewed_source | built_source | run_url | attempt |
|---|---|---|---|---|---|
| windows-x64 | pending | c60c192b9088b5f21fc199d574de17e89e5b4c94 | pending | pending | pending |
| windows-arm64 | pending | c60c192b9088b5f21fc199d574de17e89e5b4c94 | pending | pending | pending |
| linux-trace_cost | pending | c60c192b9088b5f21fc199d574de17e89e5b4c94 | pending | pending | pending |

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
