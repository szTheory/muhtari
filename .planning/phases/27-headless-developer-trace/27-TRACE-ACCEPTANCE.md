# Phase 27 trace acceptance

**Implementation baseline:** `c60c192` includes the trace path-conflict and trace-disabled configuration review fixes; trace equality acceptance is in `1b56832`, and cost gate wiring is in `6342616`. The accepted GCC portability correction is `2160cfd4bde9040c93e9addb914a3733972a8934`; the missing-pin workflow correction is `4630288226b705b4eb1f4177b3736d8ac0b072ca`. The Linux receipt was produced from PR merge checkout `fa72f560fa30a52ef5cde2a6083f4289dbc980be` on that head and the recorded `main` base.

**Acceptance state:** TRACE-01–07 have passing evidence, including the source-qualified Linux Cachegrind receipt for TRACE-06. The first 2026-11-17 run remained a terminal pre-measurement failure. A separately authorized single update to draft PR 77 produced a passing receipt on the corrected workflow source. The full automatic CI and Release workflows still failed on independent required jobs; this phase-specific cost result is not a green full-CI or shipping result.

## Proof matrix

| Requirement | Evidence | Result |
|---|---|---|
| TRACE-01 | `cmake --workflow --preset ci` (exit 0; includes `runner.trace.filters`, `runner.trace.arguments` and `runner.trace.equality`; run URL: N/A-local) | Pass |
| TRACE-02 | `cmake --workflow --preset ci` (exit 0; includes the generated numeric oracle and `core.trace.edges`; run URL: N/A-local) | Pass |
| TRACE-03 | `cmake --workflow --preset ci` (exit 0; includes `runner.trace.bounds`, `runner.trace.limits`, `runner.trace.paths` and `runner.trace.io`; run URL: N/A-local) | Pass |
| TRACE-04 | `cmake --workflow --preset ci` (exit 0; generated trace-on/off hash/state equality across NTSC/PAL legacy, XM, YM and Souper fixtures; run URL: N/A-local) | Pass |
| TRACE-05 | `ctest --test-dir build/ci --output-on-failure -R '^abi[.](api_v8|symbols)$|^runner[.]state[.]'` (exit 0; API 8, ABI 1 and state consumers; run URL: N/A-local) | Pass |
| TRACE-06 | On authorized PR run [44123425194](https://github.com/szTheory/muhtari/actions/runs/44123425194), Linux perf job `134180732604` passed and produced `quality-perf` artifact `14347390138`. The receipt binds PR head `4630288226b705b4eb1f4177b3736d8ac0b072ca` and base `89b6ff82812cd5be873e9f90d32ac55fded8694b` through merge source `fa72f560fa30a52ef5cde2a6083f4289dbc980be`; two stable repetitions report raw delta 900, threshold 9600, and 3 Ir/frame. Local `perf.trace.controls` remains controls-only; Valgrind is unavailable on Darwin arm64. The earlier run [44092872751](https://github.com/szTheory/muhtari/actions/runs/44092872751) stays a separate terminal pre-measurement failure. | Pass |
| TRACE-07 | `ctest --test-dir build/ci --output-on-failure -R '^docs[.]budgets$|^docs[.]budgets[.]controls$'` (exit 0) and `cmake --workflow --preset hygiene` (exit 0; run URLs: N/A-local) | Pass |

## Commands and environment

| Command | Exit | What it establishes |
|---|---:|---|
| `cmake --workflow --preset ci` | 0 | Full local CI workflow passed on the reviewed source: build, 736 tests, package step and all 8 release checks. Includes the equality matrix, public API/ABI/state consumers, perf comparator controls, docs budget checks and the new trace-disabled configuration control. Run URL: N/A-local. |
| `cmake --workflow --preset hygiene` | 0 | All 30 hygiene and policy checks passed, including formatting, repository tree, analyzer and firmware scan. Run URL: N/A-local. |
| `ctest --test-dir build/ci --output-on-failure -R '^abi[.](api_v8|symbols)$|^runner[.]state[.]'` | 0 | Five API, ABI and state compatibility tests passed. Run URL: N/A-local. |
| `ctest --test-dir build/ci --output-on-failure -R '^docs[.]budgets$|^docs[.]budgets[.]controls$'` | 0 | Both documentation budget checks passed. Run URL: N/A-local. |
| `/opt/homebrew/opt/llvm@21/bin/clang-format --style=file --dry-run --Werror runner/main.c src/internal.h tests/runner/test_save.c tests/runner/test_trace.c` | 0 | All changed C files match the repository formatter. |
| `git diff --check` | 0 | No whitespace errors in the final source/docs diff. |
| `uname -s && uname -m` | 0 | Local platform is Darwin arm64. |
| `command -v valgrind` | 1 | Valgrind is unavailable locally, so authentic Cachegrind evidence is N/A-local. |
| `gh pr view 77 --repo szTheory/muhtari --json state,isDraft,headRefName,headRefOid,baseRefName,baseRefOid,url` | 0 | Confirms the authorized PR remains open and draft at the measured head and recorded `main` base. |
| `gh run view 44123425194 --repo szTheory/muhtari --json headSha,headBranch,event,url,status,conclusion,createdAt,jobs` | 0 | Confirms the `pull_request` event ran for the authorized head and Linux perf job `134180732604` succeeded. The full CI workflow later failed on unrelated required lanes. |
| `gh run download 44123425194 --repo szTheory/muhtari --name quality-perf --dir /private/tmp/muhtari-trace-cost-44123425194` | 0 | Retrieved the `quality-perf` artifact outside the repository (artifact ID `14347390138`; artifact SHA-256 `bcb5f9896dda1c204ae88c82aad1e935780d6d71a2f969e35d7ea75afb323f92`). |
| `jq -e` receipt provenance, repeated-sample and threshold assertions against `/private/tmp/muhtari-trace-cost-44123425194/quality-perf.json` | 0 | Confirms the run URL, PR merge source/base/head, `tests/perf/trace.cmake` producer, four stable two-sample series, `status=PASS`, raw delta 900 ≤ 9600 and 3 Ir/frame. Exact command is recorded in [27-CI-EVIDENCE.md](27-CI-EVIDENCE.md). |

The existing Linux `perf` job builds same-source Release runners with the trace loop on and off, checks that both execute the generated CPU workload, and runs four untraced Cachegrind measurements with `env -i --tool=cachegrind --cache-sim=no`. Repeated measurements must agree; the absolute raw difference must be at most 9,600 instructions over 300 frames (32 per frame). Its ephemeral `quality-perf.json` receipt records source, tool, fixture, raw samples and runner/archive size observations. The terminal run `44092872751` failed before measurement because CI lacked pinned fixture inputs. The separately authorized update run `44123425194` passed the `perf` job and produced a receipt whose PR merge source, head and base match the authorized update. Its repeated samples are stable, raw delta is 900 (3 Ir/frame), and threshold is 9,600. The artifact and validation command are recorded in [27-CI-EVIDENCE.md](27-CI-EVIDENCE.md). The full CI required aggregate still fails because independent platform lanes fail and required artifacts are missing; it does not change the passing receipt's measurement.

## Required review

| Area | Disposition |
|---|---|
| Capture bounds and comparator | Pass: fixed-size event storage, inclusive event-type-specific address filtering, sticky truncation on the first extra match, checked raw threshold and repeated-sample stability controls. Bounds are derived in [27-BOUNDS.md](27-BOUNDS.md). |
| Observer lifetime and state | Pass: the runner installs callbacks only around the selected frame and detaches them immediately after the call. Trace events are private, excluded from canonical serialization, and trace-on/off saved state bytes and hashes match. Public API 8, ABI 1 and state formats 1/6, 2/7 and 3/8 are unchanged. |
| File lifecycle | Pass: the selected frame must complete before sibling-temp serialization and atomic replacement. Existing files survive failures; no fresh trace is published for a refused/failed frame. Path, I/O and incomplete-footer cases are in the local CI suite. |
| Ordinary execution path | Pass: with no observer the ordinary frame loop remains selected; the extra CPU/bus objects are compile-time excluded when `MUHTARI_TRACE_LOOP=OFF`. Local builds and compatibility tests pass. The authentic Linux receipt reports a 3 Ir/frame hooks-off delta against the 32 Ir/frame limit. |
| CI and scope | The existing cost check and workflow topology remain unchanged. The owner authorized one update push to draft PR 77 followed by read-only observation of its automatic checks. Linux perf passed with the accepted source-qualified receipt; CI, Release and Nightly remained failed on the independent GCC mismatched-deallocation and Clang `realpath` build errors and resulting missing quality artifacts. Release publication steps were skipped. No manual workflow, rerun, second push, merge, release, device, RetroArch or external-host check occurred. See [27-CI-EVIDENCE.md](27-CI-EVIDENCE.md) for source-specific results and the observation cap. |

The code review fixed the junction alias, trace-off test configuration, and ordinary Win32 trailing-dot/space collision findings. The final review has one deferred compatibility warning: the comparator can conservatively reject distinct destinations when one uses the explicit `\\?\` namespace. This namespace is not documented as part of the runner contract; the collision guard fails closed, so the warning describes a rejected path pair rather than an overwrite risk. See [27-REVIEW-DISPOSITION.md](27-REVIEW-DISPOSITION.md) and the [follow-up seed](../../todos/pending/2026-11-17-define-extended-windows-output-path-support.md). Windows-specific code and tests were not executed on this Darwin arm64 host.

No public header, state serializer, ROM/BIOS/save fixture or runtime trace artifact was added. Generated cartridges, states, traces and Cachegrind files remain in ignored build or runner-temporary directories. The Souper source fixture is an existing owned image in the ROM manifest.
