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

## v11.0 main delivery checks

The merged Phase 29 source `119e6e30e9f29644a4c555a34172c35574cd7f67` passed full main CI in [run 44574938376](https://github.com/szTheory/muhtari/actions/runs/44574938376). The run took 45m11s with 0s queue at run timestamp precision; the Windows ARM64 job took 41m34s and the report-only WASM32 job took 3m24s. `CI required` passed at 42m32s, 2m39s before the report-only tail completed. No jobs were cancelled. The WASM32 tail extends the run's final timestamp but did not delay the required roll-up. One sample does not justify moving or removing this report-only coverage.

The matching main Release run [44574938682](https://github.com/szTheory/muhtari/actions/runs/44574938682) took 33s and created release PR [#82](https://github.com/szTheory/muhtari/pull/82) for v0.18.0. Release CI, equality, rehearsal, install and publication did not run because the release-please pull request was created for review; this run does not establish a published package.

## Maintenance disposition

| Dimension | Evidence and disposition |
|---|---|
| Performance | No product performance change is indicated by these workflow receipts. The 24m09s quality-observation step is visible, but one timing observation does not identify safe removable work. The Phase 27 `3 Ir/frame` result measures a different source-specific workload. |
| CI/CD | A repeated release-PR pattern supports one narrow pre-merge matrix skip. Keep full exact-source release CI/equality and all publication gates; no timeout, cache, concurrency, resource, or test change is supported. |
| Ease of use | No runner command, install flow, or user interaction change is in scope. The evidence table keeps publication, current-main CI, skipped publication work, and report-only Nightly distinct for maintainers. |
| Architecture | No dependency or runtime change. Keep public API 8, ABI 1, existing legacy 1/6, YM 2/7, and Souper 3/8 state formats, and all three deliverables unchanged. |
| Documentation | Add this maintainer evidence record with source identity, outcome, timing, skipped/cancelled work, and evidence limits adjacent. No user-facing documentation change is needed. |
| Upgrade path | No migration or compatibility change is selected. Extended Windows namespace behavior remains in Phase 29; this report does not claim that behavior. |

The original main-run sample still supports no timeout or topology change. Four release-please PRs provide separate repeated evidence for skipping only their redundant pre-merge matrix; exact-source release gates remain intact below. `MU-CI-021`, `XFER-012` and the incomplete Nightly workload still do not support inferring a timeout or cache change.

## Follow-up: release-please pre-merge CI cost

Four merged release-please PRs changed exactly the same three generated files: `version.txt`, `CHANGELOG.md` and `.release-please-manifest.json`. Their Windows x64 and ARM64 jobs passed but repeated the full matrix immediately before the release workflow rebuilt the exact release source:

| Release PR | CI run | Windows x64 | Windows ARM64 |
|---|---|---:|---:|
| [#55](https://github.com/szTheory/muhtari/pull/55) | [43308325912](https://github.com/szTheory/muhtari/actions/runs/43308325912) | 26m04s | 43m37s |
| [#64](https://github.com/szTheory/muhtari/pull/64) | [43626073214](https://github.com/szTheory/muhtari/actions/runs/43626073214) | 26m48s | 42m58s |
| [#72](https://github.com/szTheory/muhtari/pull/72) | [43797178106](https://github.com/szTheory/muhtari/actions/runs/43797178106) | 25m50s | 42m52s |
| [#78](https://github.com/szTheory/muhtari/pull/78) | [44208262141](https://github.com/szTheory/muhtari/actions/runs/44208262141) | 23m47s | 43m16s |

This repeats roughly 67–70 Windows runner-minutes per metadata PR and puts about 43 minutes on the pre-merge critical path. CI now skips heavy jobs only when the same-repository `release-please--branches--main` PR targets `main` and its API-listed diff and changed-file count are exactly those three unique paths. Hygiene, title and `CI required` still run; any missing, extra, duplicate, renamed or unlisted path fails closed to full CI. The API filter rejects line feeds in either current or prior filenames before flattening paths into the classifier's line protocol; it emits only the failure sentinel, which keeps the heavy matrix enabled. This closes a newline-filename allowlist spoof found in review. `ci.classify` exercises exact and mismatched paths plus embedded, trailing and renamed-file newline cases; a workflow self-test rejects a weakened same-repository guard. The standard code review is clean after the correction. `cmake --workflow --preset hygiene` passed 31/31 locally in 32.46s. `act -l -W .github/workflows/ci.yml` parsed the changed workflow; hosted checks remain authoritative for release evidence.

| Cheap local preflight | Exit | Result | Run URL |
|---|---:|---|---|
| `sh -n scripts/classify-changes.sh` | 0 | Classifier shell syntax passes. | N/A-local |
| `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` | 0 | All 25 checked documentation files are within budget. | N/A-local |
| `act -l -W .github/workflows/ci.yml` | 0 | Workflow and job graph parse; no container job was run. | N/A-local |
| `cmake --workflow --preset hygiene` | 0 | All 31/31 tests pass, including exact release paths, newline-spoof rejection, path mismatch, partial-listing failure behavior and trusted-branch fault controls; test stage 32.46s. | N/A-local |
| `scripts/hygiene.sh --tree` | 0 | Repository tree checks pass with the new jq filter and regression cases. | N/A-local |
| `git diff --check` | 0 | No whitespace errors in the closeout changes. | N/A-local |

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

Cheap checks passed before the configured workflow. Its local result below is bound to the Phase 28 worktree at task commit `d0c2f04`; it does not establish real Windows namespace behavior or acceptance for any of the five historical hosted sources. Four exact existing `pins.txt` inputs were materialized through the repository fetch script into ignored `build/pins`; no dependency declaration or source file changed.

| Check | Exit | Result | Run URL |
|---|---:|---|---|
| Initial detached shell launch for `cmake --workflow --preset ci` | No CMake exit captured | The wrapper returned after printing the configure-step banner, wrote no result marker, and no process for this worktree remained. Treated as an incomplete launch, not workflow evidence. The later managed-session run supplied the result. | N/A-local |
| First managed `cmake --workflow --preset ci` | 1 | Configure stopped because `wasmtime`, `7800basic`, and OpenBIOS were absent from this fresh build tree; no build or tests ran. | N/A-local |
| `scripts/fetch-pin.sh wasmtime-aarch64-macos build/pins/wasmtime` | 0 | Existing Wasmtime 48.0.6 pin fetched and SHA-256 verified. | N/A-local |
| `scripts/fetch-pin.sh 7800basic-wasm build/pins/7800basic` | 0 | Existing 7800basic 0.41 pin fetched and SHA-256 verified. | N/A-local |
| `scripts/fetch-pin.sh 7800openbios-asm build/pins/openbios` | 0 | Existing OpenBIOS source pin fetched and SHA-256 verified. | N/A-local |
| Second managed `cmake --workflow --preset ci` | 1 | Configure then identified the missing pinned `rcheevos` test input; no build or tests ran. | N/A-local |
| `scripts/fetch-pin.sh rcheevos build/pins/rcheevos` | 0 | Existing rcheevos 12.1.0 pin fetched and SHA-256 verified. | N/A-local |
| `cmake --preset ci` | 0 | Configure-only preflight completed successfully after all missing inputs were materialized; no test result is inferred from this command. | N/A-local |
| Plan-specified count and normalized whole-file byte comparison | 0 | Exact two-value diff, normalized equality, and clean, stale-count, and unrelated-byte controls passed again before the full workflow. | N/A-local |
| Canonical JSON entry tally | 0 | 52 entries: 16 open, 0 waived, and 36 fixed. | N/A-local |
| Initial final-report result-string assertion | 1 | Its sentinel required the wording `100% tests passed out of 736`; the report records the equivalent `736/736 CI tests`. The CMake output itself reported 100% passed; the corrected check used the report's recorded wording. | N/A-local |
| Report receipt/maintenance-content and whitespace checks | 0 | Five hosted receipt identities, six maintenance dimensions, final newline, no trailing whitespace, and no personal path. | N/A-local |
| `git diff --check HEAD -- .planning` | 0 | No whitespace errors in tracked planning changes. | N/A-local |
| `scripts/hygiene.sh --tree` | 0 | Repository tree passed hygiene checks after the report and local pin setup. | N/A-local |
| `gsd_run windows status --raw` | 0 | Existing ledger validator accepts 52 canonical entries with counts 16/0/36. | N/A-local |
| `gsd_run query validate.consistency --raw` | 0 | Planning consistency reported `passed`. | N/A-local |
| `gsd_run query roadmap.analyze --raw` | 0 | Phase 28 and Phase 29 remain active, with Phase 29 dependent on Phase 28 and neither phase marked complete. | N/A-local |
| Final managed `cmake --workflow --preset ci` | 0 | All five workflow steps ran: configure, build, 736/736 CI tests, package generation for the library/runner/libretro, and 8/8 release checks. The generated archives and build outputs remain ignored; no logs or packages are committed. This local run does not verify Windows path behavior. | N/A-local |
