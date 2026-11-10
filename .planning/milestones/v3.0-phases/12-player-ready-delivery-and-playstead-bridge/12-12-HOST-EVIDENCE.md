# Phase 12 Plan 12-12: Host Evidence Ledger

**Disposition:** Halted. The one newly authorized ordinary candidate run is bound to its frozen source and has a complete terminal receipt, but hygiene failed and the host job never ran. PLAY-03 remains unproven.

## Frozen request and run

| Field | Evidence |
|---|---|
| Root branch and frozen source | `phase/12-player-ready-delivery-and-playstead-bridge`, `d22dca50abea335aefcb917cf428d32f5324f93c` |
| Evidence ref | `phase/12-host-evidence-20261106`, advertised at the exact frozen source |
| One ordinary request | `workflow_dispatch`, attempt 1; `keyboard_preflight_only=false`, `keyboard_probe_only=false`, `input_trace_diagnostic=false`, `phase14_host=false` |
| T0 and run creation | `2026-11-06T13:41:30Z`; run created `2026-11-06T13:41:32Z` |
| Terminal receipt | Completed with conclusion `cancelled`; required aggregate `CI required` failed |
| Run URL | https://github.com/szTheory/muhtari/actions/runs/42227540379 |
| Attempt accounting | One evidence-ref push, one dispatch, one ordinary cancellation; all three allowances are spent |

The first local push command used an unquoted zsh refspec and exited 1 before any remote update. Read-only checks found no evidence ref, pull request, or run. The corrected ordinary push exited 0; `git ls-remote` confirmed the remote ref at the frozen SHA. GitHub reported a local remote-tracking-ref write error because `.git` was read-only, but that did not change the successful remote result. No pull request was created and the push did not start a second workflow.

## Command and receipt ledger

| Action | Command or receipt | Exit / result |
|---|---|---|
| Offline policy consumer | `cmake -E make_directory build/phase12-gap12-evidence/offline && cmake -E chdir build/phase12-gap12-evidence/offline cmake "-DROOT=$PWD" "-DWORK=$PWD/build/phase12-gap12-evidence/offline/work" -P "$PWD/tests/policy/retroarch_preflight.cmake"` | 0; output empty |
| Initial malformed push | `git push origin "$SOURCE:refs/heads/phase/12-host-evidence-20261106"` | 1; local refspec parsing error, no remote mutation |
| Corrected single push | `git push origin "${SOURCE}:refs/heads/phase/12-host-evidence-20261106"` | 0; remote advertised SHA matched SOURCE |
| Single workflow request | `gh workflow run ci.yml --repo szTheory/muhtari --ref phase/12-host-evidence-20261106 -f keyboard_preflight_only=false -f keyboard_probe_only=false -f input_trace_diagnostic=false -f phase14_host=false` | 0; URL recorded above |
| Exact run observation | `gh run view 42227540379 --repo szTheory/muhtari --json attempt,createdAt,headBranch,headSha,status,conclusion,url,event,jobs` | 0; unique exact source/ref/attempt receipt, 20 jobs |
| One cancellation | `gh run cancel 42227540379 --repo szTheory/muhtari` | 0; submitted after the required hygiene job failed while the run was active |
| Exact-run audit | `cmake -DINPUT=build/phase12-gap12-evidence/run-terminal.json -DEXPECTED_SHA=d22dca50abea335aefcb917cf428d32f5324f93c -P tools/check_ci_run.cmake` | 1; correctly rejected the failed required aggregate |
| Exact-run artifacts | `gh run download 42227540379 --repo szTheory/muhtari --dir build/phase12-gap12-evidence/artifacts-all` | 0; all seven available artifacts downloaded |
| Exact-run job logs | `gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/{job_id}/logs` | 0 for hygiene, CI required, and seven build/test jobs; canceled zero-step downstream jobs and skipped title job had no downloadable logs |
| Generic log bundle attempt | `gh run view 42227540379 --repo szTheory/muhtari --log` | 1; gh could not write its cache archive in the read-only home. Direct exact-job logs above were retrieved instead. |

Command outputs, job receipts, artifact inventory, and downloaded binaries remain under ignored `build/phase12-gap12-evidence/`; none are committed. The initial refspec error is retained as prelaunch evidence and did not consume the push or dispatch allowance.

## Terminal jobs and failure cause

| Result | Count | Disposition |
|---|---:|---|
| Success | 6 | `changes`, `library`, `perf`, both `nofp` jobs, and `be32` |
| Failure | 2 | `hygiene` and the authoritative `CI required` aggregate |
| Skipped | 1 | `title`, allowed to skip for this non-PR dispatch |
| Cancelled | 11 | Seven build matrix jobs plus `quality`, `hash-equality`, `retroarch`, and `wasm32` |

`hygiene` failed at `2026-11-06T13:42:37Z` in `hygiene.format`. The hosted clang-format check flagged:

- `tests/input/history_pixels.c`
- `tests/input/history_pixels.h`
- `tests/input/test_input.c`
- `tests/retroarch/compare.c`
- `tests/retroarch/input_trace_helper.h`
- `tests/retroarch/test_input_trace_helper.c`

No source repair is included in this plan. `CI required` then failed because `hygiene` failed and required jobs were canceled. The allowed cancellation stopped the run while other matrix jobs were active. The Windows ARM64 build/test/package step was canceled after its log reached 603 of 700 tests; its `openbios-stage-windows-arm64` artifact records `prepare-output`, `transform-source`, `assemble`, `verify-output`, and `complete`, but does not establish completion of the full build/test/package job. The macOS ARM64 job was canceled at 42 of 700 tests. Linux ARM64 and x64 archives and hash artifacts were uploaded, but these are not the macOS ARM64 candidate package required for host acceptance.

The run produced no `archives-macos-arm64`, `hashes-macos-arm64`, or `retroarch-frames` artifact. The RetroArch job was canceled before any step started. Therefore no actual frontend version was observed, no candidate core or `.info` member was bound to this receipt, and no held/released image, independent comparator, DMA comparison, negative control, or fresh-process battery result exists. The configured RetroArch pin is version 1.22.2 with DMG SHA-256 `81b79121ba26d539064ae13b4d0419a120c3d165afbe656cf5f5412b15fdb434`; this run did not install or observe it.

## Predicate disposition

| Required evidence | Result | Basis |
|---|---|---|
| Frozen source, evidence ref, unique run and attempt | PASS | Remote ref and terminal receipt identify SOURCE, dedicated branch, `workflow_dispatch`, attempt 1 |
| Requested ordinary mode; probe, trace, and Phase 14 switches off | PASS | Sole saved dispatch command sets all four optional inputs to `false` |
| Successful required CI aggregate | FAIL | `CI required` failed; exact-run audit exited 1 |
| macOS ARM64 candidate archive and extracted core/`.info` identities | MISSING | Required artifact jobs were canceled before artifact production |
| Actual pinned RetroArch host launch | MISSING | RetroArch job had no steps; no observed frontend version |
| Independently correct held and released images | MISSING | No RetroArch frames or comparator verdicts |
| DMA comparison and retained fault control | MISSING | Host job never started |
| Two fresh-process battery checks | MISSING | Host job never started |
| PLAY-03 candidate-host acceptance | NOT MET | Multiple required predicates are missing and the aggregate failed |

This run does not establish released-core replay, Playstead behavior, Phase 12 completion, milestone completion, or any Phase 14 state. Previous acceptance rows and earlier failed attempts remain unchanged. No structural workflow, timeout, cache, or concurrency change is supported by this single canceled observation.

---
*Recorded: 2026-11-06.*
