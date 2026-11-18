---
phase: 27-headless-developer-trace
verified: 2026-11-17T20:16:28Z
status: passed
score: 15/15 must-haves verified
covered_files:
  - .github/workflows/ci.yml
  - .planning/phases/27-headless-developer-trace/27-01-PLAN.md
  - .planning/phases/27-headless-developer-trace/27-01-SUMMARY.md
  - .planning/phases/27-headless-developer-trace/27-02-PLAN.md
  - .planning/phases/27-headless-developer-trace/27-02-SUMMARY.md
  - .planning/phases/27-headless-developer-trace/27-03-PLAN.md
  - .planning/phases/27-headless-developer-trace/27-03-SUMMARY.md
  - .planning/phases/27-headless-developer-trace/27-04-PLAN.md
  - .planning/phases/27-headless-developer-trace/27-04-SUMMARY.md
  - .planning/phases/27-headless-developer-trace/27-05-PLAN.md
  - .planning/phases/27-headless-developer-trace/27-06-PLAN.md
  - .planning/phases/27-headless-developer-trace/27-06-SUMMARY.md
  - CMakeLists.txt
  - cmake/TraceLoopConfig.cmake
  - docs/maintaining.md
  - docs/runner.md
  - docs/testing.md
  - runner/main.c
  - src/bus.c
  - src/cpu.c
  - src/frame.c
  - src/internal.h
  - src/maria.c
  - tests/cmake/trace_loop_config.cmake
  - tests/inventory/ci.txt
  - tests/inventory/wasm32.txt
  - tests/perf/tests.cmake
  - tests/perf/trace.cmake
  - tests/runner/test_save.c
  - tests/runner/test_trace.c
  - tests/runner/tests.cmake
  - tests/runner/trace.cmake
covered_digest: "v4:sha256:b0f83c4c62441f63ab0773d286b3ee121cd11bb66047a4300ae95c99a58957a7"
behavior_unverified: 0
overrides_applied: 0
re_verification:
  previous_status: passed
  previous_score: 15/15
  gaps_closed: []
  gaps_remaining: []
  regressions: []
advisory: []
decision_coverage:
  honored: 9
  total: 9
  not_honored: []
---

# Phase 27: Headless developer trace Verification Report

**Phase Goal:** Developers can inspect deterministic Atari 7800 instruction and bus-access events through the existing headless runner without changing emulated results or public compatibility.

**Verified:** 2026-11-17T19:05:11Z
**Status:** passed
**Re-verification:** No — the previous passing report was stale after compiler-portability files changed; this report checks the current worktree.

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | A developer can select one invocation frame and inclusive address range and receive a separate versioned JSONL trace with event kind, master tick, beam position, and CPU context; bus records include address, direction, and value. | ✓ VERIFIED | `runner/main.c` provides the CLI and serializer; the actual-runner smoke and fixed-value oracle passed in the prior full local CI run. |
| 2 | Instruction-start-PC and CPU bus-address filters are independent, inclusive, and retain observation order, including instruction-before-opcode-bus ties. | ✓ VERIFIED | `trace_capture_event` uses the event-specific filter without sorting; the generated numeric oracle and filter controls passed. |
| 3 | Event timestamps use the CPU bus data edge and records contain the documented beam coordinates and sampled registers. | ✓ VERIFIED | The bus producers and fixed numeric oracle verify event ticks, beam coordinates, register values, and ordering. |
| 4 | The runner attaches and detaches the private observer only around the selected invocation frame, and the frame loop chooses the traced path once at its entry. | ✓ VERIFIED | `runner/main.c` brackets the call with the instance callback; `src/frame.c` chooses the traced entry once per frame. The instance/detach tests are registered and passed. |
| 5 | The event cap distinguishes exactly-full output from one-extra-match truncation and lets emulation finish. | ✓ VERIFIED | `runner.trace.limits` checks the exact-limit and one-extra cases and the later frame hashes; it passed in the recorded local CI run. |
| 6 | Capture memory and serialized output have checked finite bounds grounded in source invariants. | ✓ VERIFIED | Checked arithmetic and source-derived bounds are documented in `27-BOUNDS.md`; the registered bounds test passed. |
| 7 | Invalid or conflicting trace options fail before emulation and cannot overwrite inputs or other outputs. | ✓ VERIFIED | Current `runner.trace.paths` passed; previous source-qualified Windows x64 and arm64 path-control jobs passed. The newer compiler-only and test-harness changes do not alter the path implementation. |
| 8 | Trace open/write/flush/sync/close/replace failures return failure, preserve an old destination, and do not publish a fresh complete-looking trace. | ✓ VERIFIED | The injected actual-main I/O fault matrix is registered and passed in the recorded local CI run. |
| 9 | A generated diagnostic produces an exact numeric trace oracle independent of runtime output generation. | ✓ VERIFIED | `tests/runner/test_trace.c` compares the real runner output with fixed expected records and rejects corrupted values and order; its oracle passed. |
| 10 | Tracing leaves deterministic stdout, frame hashes, canonical state hashes/bytes, and supported legacy/YM/Souper behavior unchanged. | ✓ VERIFIED | The trace-on/off equality matrix passed for the recorded NTSC/PAL, cartridge, and state cases. |
| 11 | The feature stays private and preserves public API 8, ABI 1, behavior revision, and state formats; observer pointers are excluded from canonical state. | ✓ VERIFIED | The public header and serializer are unchanged; API/ABI/state tests and byte/hash equality checks passed. |
| 12 | An unobserved frame uses the ordinary CPU/bus path without a per-access trace conditional; trace objects can be omitted in a product-only build. | ✓ VERIFIED | The ordinary and trace variants are separately compiled, dispatch is at frame entry, and trace-disabled configuration and runner-refusal controls passed. |
| 13 | Runner documentation accurately explains invocation, filtering, record fields, bounds, truncation, error behavior, and CPU-only limits. | ✓ VERIFIED | `docs/runner.md` and `docs/testing.md` match the tested CLI/schema; documentation budget checks passed. |
| 14 | Required local product, hygiene, compatibility, and fixture-cleanliness checks pass without public/state-format changes or committed runtime media. | ✓ VERIFIED | Current `cmake --workflow --preset ci` passed all 736 tests, generated all three packages, and passed all 8 release checks. The prior native fixture preflight and hygiene results remain recorded; current source diff adds no public/state-format changes or media. |
| 15 | A real generated CPU workload proves the trace-disabled instruction-cost delta is at most 32 Ir/frame. | ✓ VERIFIED | The accepted Linux Cachegrind receipt is tied to PR head `4630288…` and merge checkout `fa72f56…`: stable repeated samples, `delta_raw=900`, threshold `9600`, and `delta_ir_frame=3`. The later compiler/harness corrections do not modify the measured CPU, bus, frame, runner, perf workflow, or perf producer files. Exact-head hosted CI, Release, and Nightly acceptance is recorded below. |

**Score:** 15/15 observable truths verified (0 present, behavior-unverified).

## Current-head hosted verification

The exact current source `4dba7cfd982108e291736bc5e34c01f90edf025a` passed all automatic hosted checks after the compiler fixes. [CI run 44157601581](https://github.com/szTheory/muhtari/actions/runs/44157601581) passed the affected GCC and Clang build lanes and all required Linux, BE32, library, ASan, macOS, Windows, hash-equality, RetroArch, wasm32, and quality checks. [Release run 44157601747](https://github.com/szTheory/muhtari/actions/runs/44157601747) passed equality, nested CI, the temporary release rehearsal, install-line check, and cleanup. The [Nightly run 44157601814](https://github.com/szTheory/muhtari/actions/runs/44157601814) also passed. All three runs report the same exact head SHA.

The Release rehearsal briefly published a non-latest temporary prerelease as designed. Its cleanup job succeeded; the release lookup then returned HTTP 404 and the tag lookup returned an empty list. The normal stable-release jobs were skipped. PR 77 remains open and draft, and this verification does not merge it.

## Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `runner/main.c`, `src/internal.h`, `src/frame.c`, `src/cpu.c`, `src/bus.c` | Private trace capture, filtering, observer lifetime, and ordinary-path dispatch | ✓ VERIFIED | The previous report's implementation and wiring checks remain valid; the trace-specific tests passed and no product source changed in the final pin-fetch plan. |
| `tests/runner/test_trace.c`, `tests/runner/trace.cmake`, `tests/runner/test_save.c` | Numeric oracle, bounds, equality, and output-failure controls | ✓ VERIFIED | Registered tests and prior full local CI results cover these controls. |
| `CMakeLists.txt`, `tests/runner/tests.cmake`, `tests/runner/test_save.c`, `docs/maintaining.md` | GCC stream-wrapper ownership annotations and X/Open declaration visibility for runner consumers | ✓ VERIFIED | GCC 15.3 optimized warning-control compile, Clang 21 Linux/glibc declaration control, focused runner tests, formatting, full local CI, and exact-head hosted GCC/Clang CI lanes pass; review report is clean. |
| `tests/perf/trace.cmake`, `.github/workflows/ci.yml` | Same-source CPU Cachegrind comparison and source-qualified receipt | ✓ VERIFIED | The perf job now fetches the four existing pinned inputs before its tests-on configure, retains the five-minute cap, and produced the accepted receipt. |
| `docs/runner.md`, `docs/testing.md` | User-facing trace contract and test instructions | ✓ VERIFIED | Documentation budget checks passed and the recorded docs match the runner contract. |
| `27-CI-EVIDENCE.md`, `27-TRACE-ACCEPTANCE.md`, `27-06-CHECKPOINT.md` | Exact-source receipts, acceptance, and owner boundary history | ✓ VERIFIED | The earlier failed attempts remain terminal; the separate authorized continuations, current exact-head green runs, and remaining PR boundary are recorded. |

## Key Link Verification

| From | To | Via | Status | Details |
|---|---|---|---|---|
| `runner/main.c` | `src/frame.c` | Callback is installed around the selected `muhtari_run_frame` call and cleared after it | WIRED | Manual call-path trace confirmed instance ownership and selected-frame dispatch. |
| `src/cpu.c` | `src/bus.c` | Traced CPU calls the separately compiled trace bus producers | WIRED | CMake symbol-renaming and object definitions preserve separate ordinary and trace paths. |
| `.github/workflows/ci.yml` | `scripts/fetch-pin.sh` | Four existing pins are fetched before tests-on configure | WIRED | The new step fetches `wasmtime-x86_64-linux`, `7800basic-wasm`, `7800openbios-asm`, and `rcheevos` into the existing ignored destinations. |
| `.github/workflows/ci.yml` | `tests/perf/trace.cmake` | Same-source CPU workload and Cachegrind measurement write the receipt | WIRED | The recorded receipt's inner producer is `tests/perf/trace.cmake`; the outer quality artifact preserves its source identity and samples. |
| `docs/runner.md` | `runner/main.c` | Documented command, fields, limits, and errors match the implemented CLI | WIRED | The earlier actual-runner and documentation checks remain source-qualified. |

The generic structural link query checks literal target references and misses several CMake/runtime links. Those links were traced manually through the actual CMake symbol-renaming definitions, runner callback attach/detach path, actual-runner CTest cases, and workflow commands.

## Data-Flow Trace

| Artifact | Data | Source | Produces real data | Status |
|---|---|---|---|---|
| Runner JSONL trace | CPU instruction and bus events | Diagnostic cartridge executed by the real core and runner | Yes | ✓ FLOWING; the fixed-value numeric oracle passed. |
| `quality-perf.json.trace_cost` | ON/OFF instruction counts and delta | Cachegrind over the same generated CPU workload and its recorded source | Yes | ✓ FLOWING; downloaded receipt validation passed for merge source `fa72f560…`, joining authorized head `4630288…` and base `89b6ff8…`. This predates current compiler/harness commits and does not establish hosted CI acceptance for current HEAD. |

## Behavioral Spot-Checks

| Behavior | Command or run | Result | Status |
|---|---|---|---|
| Trace oracle, equality, bounds, limits, POSIX paths, and save-faults | `ctest --test-dir build/ci --output-on-failure -R '^(runner[.]trace[.](oracle|equality|bounds|limits|paths)|runner[.]save[.]faults)$'` | Exit 0; all six current-source runner checks passed | ✓ PASS |
| API, ABI, and state compatibility | Prior local `ctest` evidence in `27-TRACE-ACCEPTANCE.md` | Exit 0; five compatibility tests passed | ✓ PASS |
| Fresh fixture prerequisite path | Native CI Release configure, build `muhtari-run` and `muhtari-test-trace`, generate/run `trace-cost.a78` | All commands exit 0; runner stdout contains `diagnostic a5` | ✓ PASS; local fixture reachability only |
| Current full local CI | `cmake --workflow --preset ci` | Exit 0; all 736 tests passed, three native packages were generated, and all 8 release checks passed | ✓ PASS |
| Cheap checks before the full workflow | `git diff --check`; Clang-format on `tests/runner/test_save.c`; focused harness build and CTest | All exited 0 before the long workflow started | ✓ PASS |
| GCC and Clang portability controls | Commands and isolated revert controls in `.planning/debug/resolved/ci-gcc-clang-build.md` | GCC 15.3.0 optimized full-warning compilation passed; isolated annotation removal reproduced the original warning. Clang 21 targeting Linux/glibc passed with `_XOPEN_SOURCE=700`; removing it reproduced undeclared `realpath`. Exact-head hosted GCC and Clang lanes passed. | ✓ PASS locally and hosted |
| Hooks-off instruction cost | [PR CI run 44123425194](https://github.com/szTheory/muhtari/actions/runs/44123425194), Linux perf job `134180732604`; artifact `14347390138` | Exit 0; two stable repetitions per series; raw delta 900 ≤ 9,600 over 300 frames (3 Ir/frame). The exact `jq -e` source/provenance/arithmetic check also exited 0. | ✓ PASS for the recorded source |
| Historical CI and Release on the earlier source | [CI run 44123425194](https://github.com/szTheory/muhtari/actions/runs/44123425194); [Release run 44123425418](https://github.com/szTheory/muhtari/actions/runs/44123425418) | Both failed on the GCC and Clang diagnostics later fixed. The Linux `perf` receipt from that source remains valid for TRACE-06. | Historical failure retained; superseded for current-source acceptance |
| Current-head CI, Release, and Nightly | [CI run 44157601581](https://github.com/szTheory/muhtari/actions/runs/44157601581); [Release run 44157601747](https://github.com/szTheory/muhtari/actions/runs/44157601747); [Nightly run 44157601814](https://github.com/szTheory/muhtari/actions/runs/44157601814) | All three runs passed on exact source `4dba7cfd982108e291736bc5e34c01f90edf025a`. Release rehearsal, install-line, and cleanup passed; its temporary prerelease and tag are absent. PR 77 remains draft and unmerged. | ✓ PASS for current source; merge remains stopped by the PR instruction |

The historical full CI and Release failures are retained as evidence for the earlier source; the current exact head passed local CI and all three hosted workflows. The recorded trace-cost receipt remains source-qualified to its measured source. This verdict does not claim the PR has merged or authorize another push, dispatch, rerun, or stable release.

## Regression Gate

The prior phase is documentation-only. Its verification and summaries identify no prior-phase automated test files, so no regression suite was selected or run.

## Probe Execution

No phase-declared probe script or repository `scripts/*/tests/probe-*.sh` was found.

## Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| TRACE-01 | 27-01 | Opt-in deterministic instruction and bus trace for a selected frame and range | ✓ SATISFIED | Actual-runner smoke, filter controls, and numeric oracle passed. |
| TRACE-02 | 27-01 | Record event/timing/beam/CPU fields and document filter semantics | ✓ SATISFIED | Fixed-value records and ordering passed; runner documentation matches. |
| TRACE-03 | 27-01, 27-02, 27-04 | Finite capture, truncation, early refusal, truthful I/O failures, and Windows paths | ✓ SATISFIED | Current local bounds/limits/path tests and prior source-matched Windows x64/arm64 path jobs passed. |
| TRACE-04 | 27-01, 27-02, 27-03 | Exact diagnostic oracle and unchanged frame/canonical-state results | ✓ SATISFIED | Numeric oracle and trace-on/off equality matrix passed. |
| TRACE-05 | 27-01, 27-02, 27-03 | Runner-only seam with public API/ABI/state compatibility | ✓ SATISFIED | Public header and serializer are unchanged; API/ABI/state tests passed. |
| TRACE-06 | 27-01, 27-03, 27-04, 27-05, 27-06 | Hooks-off instruction-cost gate with observer-selected instrumentation | ✓ SATISFIED | The accepted source-qualified Linux Cachegrind receipt reports 3 Ir/frame against the 32 Ir/frame ceiling. Its source identity is recorded; current CPU/bus/frame and perf producer files are unchanged. Failed pre-measurement attempts remain separately recorded. |
| TRACE-07 | 27-03 | Runner documentation and fixture cleanliness | ✓ SATISFIED | Documentation budget, hygiene, and tracked-tree checks passed. |

**Orphaned requirements:** None. Every PLAN requirement ID maps to TRACE-01 through TRACE-07 in `.planning/REQUIREMENTS.md`; the superseded 27-05 plan is historical and not resumed.

## Test Quality Audit

The current trace oracle compares actual runner JSONL against fixed numeric records, while the equality test compares trace-on/off stdout, frame/state hashes, and canonical state bytes. All six named current-source checks passed. No disabled tests or circular expected-value generation were found in the linked CTest sources; assertion strength is behavioral/value-level for the oracle, equality, bounds, limit, path, and I/O cases.

## Decision Coverage

All 9 trackable CONTEXT decisions are honored. No new decision override was needed.

## Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| — | — | No unresolved `TBD`, `FIXME`, or `XXX` marker was found in the modified implementation/workflow scope. | — | — |

The current compiler-portability review found no issues after the mode-specific test-harness argument check was added. The final report and disposition history are recorded in `27-REVIEW.md`, `27-REVIEW-FIX.md`, and `27-REVIEW-DISPOSITION.md`.

## Human Verification Required

None. The phase's acceptance criteria were exercised locally and the exact current head passed hosted CI, Release, and Nightly. The PR remains draft and unmerged under its existing instruction; that separate delivery boundary is not an acceptance gap for the verified code.

## Gaps Summary

The trace feature, fixed-value oracle, deterministic compatibility checks, path and I/O controls, documentation, and compiler fixes are verified. The accepted Linux Cachegrind result is preserved for its recorded source and proves the hooks-off cost bound; the current CPU/bus/frame and perf producer files are unchanged from that measurement. Local CI and exact-head hosted CI, Release, and Nightly pass. PR 77 remains open and draft; no merge or stable release is claimed.

---

_Verified: 2026-11-17T20:16:28Z_
_Verifier: the agent (gsd-verifier)_
