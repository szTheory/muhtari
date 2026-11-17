# Phase 27 trace acceptance

**Source reviewed:** `c60c192` (review fixes for trace output path conflicts and trace-disabled build configuration). The trace equality acceptance is in `1b56832`; CPU cost gate wiring is in `6342616`.

**Acceptance state:** TRACE-01–05 and TRACE-07 have passing local evidence. TRACE-06 remains **pending** until the existing Linux `perf` job produces a source-qualified Cachegrind receipt. Local controls are not that receipt.

## Proof matrix

| Requirement | Evidence | Result |
|---|---|---|
| TRACE-01 | `cmake --workflow --preset ci` (exit 0; includes `runner.trace.filters`, `runner.trace.arguments` and `runner.trace.equality`; run URL: N/A-local) | Pass |
| TRACE-02 | `cmake --workflow --preset ci` (exit 0; includes the generated numeric oracle and `core.trace.edges`; run URL: N/A-local) | Pass |
| TRACE-03 | `cmake --workflow --preset ci` (exit 0; includes `runner.trace.bounds`, `runner.trace.limits`, `runner.trace.paths` and `runner.trace.io`; run URL: N/A-local) | Pass |
| TRACE-04 | `cmake --workflow --preset ci` (exit 0; generated trace-on/off hash/state equality across NTSC/PAL legacy, XM, YM and Souper fixtures; run URL: N/A-local) | Pass |
| TRACE-05 | `ctest --test-dir build/ci --output-on-failure -R '^abi[.](api_v8|symbols)$|^runner[.]state[.]'` (exit 0; API 8, ABI 1 and state consumers; run URL: N/A-local) | Pass |
| TRACE-06 | `perf.trace.controls` passes in local CI, but `command -v valgrind` exits 1 on Darwin arm64. No local Cachegrind run and no authorized delivery CI receipt yet. | Pending |
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

The existing Linux `perf` job builds same-source Release runners with the trace loop on and off, checks that both execute the generated CPU workload, and runs four untraced Cachegrind measurements with `env -i --tool=cachegrind --cache-sim=no`. Repeated measurements must agree; the absolute raw difference must be at most 9,600 instructions over 300 frames (32 per frame). Its ephemeral `quality-perf.json` receipt records source, tool, fixture, raw samples and runner/archive size observations. The measurement command is wired into CI but has not run here or on an authorized delivery run. No remote workflow was dispatched and no hosted CI run URL exists yet.

## Required review

| Area | Disposition |
|---|---|
| Capture bounds and comparator | Pass: fixed-size event storage, inclusive event-type-specific address filtering, sticky truncation on the first extra match, checked raw threshold and repeated-sample stability controls. Bounds are derived in [27-BOUNDS.md](27-BOUNDS.md). |
| Observer lifetime and state | Pass: the runner installs callbacks only around the selected frame and detaches them immediately after the call. Trace events are private, excluded from canonical serialization, and trace-on/off saved state bytes and hashes match. Public API 8, ABI 1 and state formats 1/6, 2/7 and 3/8 are unchanged. |
| File lifecycle | Pass: the selected frame must complete before sibling-temp serialization and atomic replacement. Existing files survive failures; no fresh trace is published for a refused/failed frame. Path, I/O and incomplete-footer cases are in the local CI suite. |
| Ordinary execution path | Pass: with no observer the ordinary frame loop remains selected; the extra CPU/bus objects are compile-time excluded when `MUHTARI_TRACE_LOOP=OFF`. Local builds and compatibility tests pass. Authentic hooks-off instruction cost remains pending TRACE-06 evidence. |
| CI and scope | Pass: the CPU cost check is wired into the existing perf job before its workflow-dispatch early return. The test-card ratchet, artifact identity and job topology remain intact; no remote workflow, device, RetroArch or external-host check was run. |

The code review fixed the junction alias, trace-off test configuration, and ordinary Win32 trailing-dot/space collision findings. The final review has one deferred compatibility warning: the comparator can conservatively reject distinct destinations when one uses the explicit `\\?\` namespace. This namespace is not documented as part of the runner contract; the collision guard fails closed, so the warning describes a rejected path pair rather than an overwrite risk. See [27-REVIEW-DISPOSITION.md](27-REVIEW-DISPOSITION.md) and the [follow-up seed](../../todos/pending/2026-11-17-define-extended-windows-output-path-support.md). Windows-specific code and tests were not executed on this Darwin arm64 host.

No public header, state serializer, ROM/BIOS/save fixture or runtime trace artifact was added. Generated cartridges, states, traces and Cachegrind files remain in ignored build or runner-temporary directories. The Souper source fixture is an existing owned image in the ROM manifest.
