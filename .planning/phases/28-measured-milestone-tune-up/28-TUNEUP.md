# Phase 28 Tune-up: source-qualified delivery and ledger counts

## Source-qualified delivery evidence

These five hosted receipts are locked historical evidence from earlier delivery snapshots. Each conclusion stays bound to the SHA shown here; none establishes acceptance for the later Phase 28 execution head. `0s` is run-level queue time at GitHub timestamp precision, not runner dispatch latency. No unrecorded dispatch delay is inferred.

| Evidence | Exact source | Workflow and receipt | Outcome, timing, queue, skipped or cancelled work | Limit |
|---|---|---|---|---|
| Published stable v0.17.0 CI | `333380b118f62b618cb6df29cdfdd4388efc7b22`; 20 release assets point to this source | `cmake --workflow --preset ci`; [run 44214206424](https://github.com/szTheory/muhtari/actions/runs/44214206424) | Passed in 47m18s; queue 0s at timestamp precision. Windows ARM64 took 43m15s, followed by WASM32 at 3m49s. No jobs were cancelled. | Required CI for the published source only. |
| Published stable v0.17.0 Release | `333380b118f62b618cb6df29cdfdd4388efc7b22` | Release workflow; [run 44214206354](https://github.com/szTheory/muhtari/actions/runs/44214206354) | Passed publication in 52m07s. Critical path: Windows ARM64 41m41s → WASM32 3m54s → publish 5m45s → latest-install 10s. No jobs were cancelled. | Confirms the existing publication and install receipt for this exact source. |
| Recorded current-main CI snapshot | `f709d493e6660b3b53a98b49b693ab91a714dcb0` | `cmake --workflow --preset ci`; [run 44233889699](https://github.com/szTheory/muhtari/actions/runs/44233889699) | Passed in 43m50s; queue 0s at timestamp precision. Windows ARM64 took 39m07s, including 24m09s in quality observations, then WASM32 took 4m32s. No jobs were cancelled. | The long quality step locates time but does not identify safe work to remove. This is a recorded main snapshot, not the Phase 28 execution head. |
| Recorded current-main Release snapshot | `f709d493e6660b3b53a98b49b693ab91a714dcb0` | Release workflow; [run 44233889579](https://github.com/szTheory/muhtari/actions/runs/44233889579) | Succeeded in 15s. Release, equality, publish, and install jobs were skipped because no version change was due. | No new package publication or install verification is established by this run. |
| Scheduled report-only Nightly | `f709d493e6660b3b53a98b49b693ab91a714dcb0` | Nightly workflow; [run 44236642588](https://github.com/szTheory/muhtari/actions/runs/44236642588) | The report-only 10,000-case `prop` job was cancelled at its existing 9-minute job cap; completeness failed because the workload was incomplete. Other jobs completed. | Not required CI, not a completed timeout sample, and not a root-cause diagnosis. No retry or cap change was made. |

The later v10.0 audit supersedes Phase 27's earlier draft-PR wording: PR #77 and release PR #78 merged, and v0.17.0 is the stable release. The failed v0.15.0 tag/draft remains historical and unpublished. These distinctions are supported by the [v10.0 audit](../../milestones/v10.0-MILESTONE-AUDIT.md#delivery-evidence) and the exact-source receipts above. The Phase 27 `3 Ir/frame` trace-cost result is source-specific CPU instrumentation evidence, not a pipeline-saving measurement.

## Maintenance disposition

| Dimension | Evidence and disposition |
|---|---|
| Performance | No product performance change is indicated by these workflow receipts. The 24m09s quality-observation step is visible, but one timing observation does not identify safe removable work. The Phase 27 `3 Ir/frame` result measures a different source-specific workload. |
| CI/CD | Preserve workflow structure, timeouts, caches, parallelism, resource limits, and required/advisory membership. Exact-source required CI and publication are green; the single recorded main pair does not isolate a safe pipeline change. |
| Ease of use | No runner command, install flow, or user interaction change is in scope. The evidence table keeps publication, current-main CI, skipped publication work, and report-only Nightly distinct for maintainers. |
| Architecture | No dependency or runtime change. Keep public API 8, ABI 1, existing legacy 1/6, YM 2/7, and Souper 3/8 state formats, and all three deliverables unchanged. |
| Documentation | Add this maintainer evidence record with source identity, outcome, timing, skipped/cancelled work, and evidence limits adjacent. No user-facing documentation change is needed. |
| Upgrade path | No migration or compatibility change is selected. Extended Windows namespace behavior remains in Phase 29; this report does not claim that behavior. |

The bounded decision is no pipeline change. `MU-CI-021` requires comparable completed samples for timeout decisions; `XFER-012` and the incomplete Nightly workload do not support inferring a cause from cancellation. Revisit a cache or topology proposal only after repeated comparable completed measurements and a per-step profile identify a safe saving while preserving hash equality, artifact joins, and gate membership. GitHub's [dependency caching guidance](https://docs.github.com/en/actions/concepts/workflows-and-actions/dependency-caching) notes that cached contents are restored as-is and untrusted; no cache or audit mechanism is added here.

## Register reconciliation

The required baseline is blob `fe94888ffdfb0deeed942adaf97dd4f110737a4d`. Before editing, `gsd_run windows status --raw` exited 1: metadata said open/waived/fixed/total `17/0/35/52`, while the canonical fenced JSON entries yielded `16/0/36/52`. An independent tally of that canonical entry block also yields 52 entries: 16 open, 0 waived, and 36 fixed. The repair changes only `open_count: 17` to `open_count: 16` and `fixed_count: 35` to `fixed_count: 36`; `waived_count: 0`, `total_count: 52`, `last_updated: 2026-10-24T22:19:18.466Z`, all canonical entries, and all other bytes remain unchanged. The existing `windows status --raw` validator then exits 0.

The baseline also contains a rendered-table mirror mismatch at id 51: its Markdown row says `open`, while the canonical JSON entry says `fixed`. The GSD validator reads the fenced JSON as the entry source. This pre-existing mirror difference is unchanged because the plan permits only the two aggregate replacements; it remains visible for phase verification and is not represented here as fixed.

| Command or check | Exit | Result | Run URL |
|---|---:|---|---|
| Pre-edit `gsd_run windows status --raw` | 1 | Reproduced the recorded frontmatter-versus-canonical-entry mismatch. | N/A-local |
| Initial preservation-control invocation | 1 | A local Node command had a missing closing parenthesis in the unrelated-byte control; syntax error only, no file was changed by that check. | N/A-local |
| Initial rendered Markdown row tally | 1 | Found 52 visible rows with 17 open and 35 fixed. Source review found the preserved id 51 Markdown/JSON mirror mismatch; the structured JSON tally below is the GSD ledger entry source. | N/A-local |
| Initial report-format check invocation | 1 | The checker escaped the final newline as a literal backslash-plus-`n`, so its EOF assertion was invalid. A character-code newline check passed afterward; the report bytes were unchanged. | N/A-local |
| Independent canonical JSON entry tally | 0 | 52 entries: 16 open, 0 waived, 36 fixed. | N/A-local |
| Required baseline-to-working-file byte comparison, including clean, stale-count, and unrelated-byte controls | 0 | The whole file equals the baseline blob with exactly the two permitted substitutions; normalized bytes match. | N/A-local |
| Post-edit `gsd_run windows status --raw` | 0 | Existing validator accepts 52 canonical entries with counts 16/0/36. | N/A-local |
| Report evidence, six-dimension, newline, and whitespace checks | 0 | All five receipt identities/URLs and six maintenance dimensions are present; no trailing whitespace; one final newline. | N/A-local |
| `git diff --check -- .planning/WINDOWS.md` | 0 | No tracked ledger whitespace errors. The untracked report is covered by its explicit whitespace check above. | N/A-local |

## Local verification

The full configured workflow remains pending until Task 2 runs it after cheap checks. Its eventual result will be local evidence only and will not establish real Windows namespace behavior or acceptance for any of the five historical hosted sources.

| Check | Exit | Result | Run URL |
|---|---:|---|---|
| `cmake --workflow --preset ci` | pending | Task 2 will run after count, byte-preservation, report, whitespace, hygiene, and planning preflight checks pass. | N/A-local |
