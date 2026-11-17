---
phase: 27-headless-developer-trace
verified: 2026-11-17T17:27:20Z
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
  - .planning/phases/27-headless-developer-trace/27-06-CHECKPOINT.md
  - .planning/phases/27-headless-developer-trace/27-06-PLAN.md
  - .planning/phases/27-headless-developer-trace/27-06-SUMMARY.md
  - .planning/phases/27-headless-developer-trace/27-BOUNDS.md
  - .planning/phases/27-headless-developer-trace/27-CI-EVIDENCE.md
  - .planning/phases/27-headless-developer-trace/27-REVIEW-DISPOSITION.md
  - .planning/phases/27-headless-developer-trace/27-REVIEW-FIX.md
  - .planning/phases/27-headless-developer-trace/27-REVIEW.md
  - .planning/phases/27-headless-developer-trace/27-TRACE-ACCEPTANCE.md
  - .planning/phases/27-headless-developer-trace/deferred-items.md
  - .planning/todos/pending/2026-11-17-define-extended-windows-output-path-support.md
  - CMakeLists.txt
  - cmake/TraceLoopConfig.cmake
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
covered_digest: "v4:sha256:3a91f0b5f7e11b0d5d9c9cbf2eb2200a7fa6c17e06b5af6d0adc78a65532cbc4"
behavior_unverified: 0
overrides_applied: 0
re_verification:
  previous_status: gaps_found
  previous_score: 14/15
  gaps_closed:
    - "A generated CPU workload proves the hooks-off instruction-cost delta is at most 32 Ir/frame on the measured source."
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

**Verified:** 2026-11-17T17:27:20Z
**Status:** passed
**Re-verification:** Yes — after gap closure

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
| 7 | Invalid or conflicting trace options fail before emulation and cannot overwrite inputs or other outputs. | ✓ VERIFIED | The runner path checks and POSIX tests passed locally; exact-source Windows x64 and arm64 path-control jobs passed. The hosted Clang build failure recorded below is not a path-control result and is not treated as a green build. |
| 8 | Trace open/write/flush/sync/close/replace failures return failure, preserve an old destination, and do not publish a fresh complete-looking trace. | ✓ VERIFIED | The injected actual-main I/O fault matrix is registered and passed in the recorded local CI run. |
| 9 | A generated diagnostic produces an exact numeric trace oracle independent of runtime output generation. | ✓ VERIFIED | `tests/runner/test_trace.c` compares the real runner output with fixed expected records and rejects corrupted values and order; its oracle passed. |
| 10 | Tracing leaves deterministic stdout, frame hashes, canonical state hashes/bytes, and supported legacy/YM/Souper behavior unchanged. | ✓ VERIFIED | The trace-on/off equality matrix passed for the recorded NTSC/PAL, cartridge, and state cases. |
| 11 | The feature stays private and preserves public API 8, ABI 1, behavior revision, and state formats; observer pointers are excluded from canonical state. | ✓ VERIFIED | The public header and serializer are unchanged; API/ABI/state tests and byte/hash equality checks passed. |
| 12 | An unobserved frame uses the ordinary CPU/bus path without a per-access trace conditional; trace objects can be omitted in a product-only build. | ✓ VERIFIED | The ordinary and trace variants are separately compiled, dispatch is at frame entry, and trace-disabled configuration and runner-refusal controls passed. |
| 13 | Runner documentation accurately explains invocation, filtering, record fields, bounds, truncation, error behavior, and CPU-only limits. | ✓ VERIFIED | `docs/runner.md` and `docs/testing.md` match the tested CLI/schema; documentation budget checks passed. |
| 14 | Required local product, hygiene, compatibility, and fixture-cleanliness checks pass without public/state-format changes or committed runtime media. | ✓ VERIFIED | The recorded final-source local CI workflow passed 736 tests and 8 release checks; the latest hygiene run passed all 30 checks. The new native preflight built the runner and fixture generator, executed the generated CPU diagnostic, and confirmed `diagnostic a5`; generated files remain ignored. |
| 15 | A real generated CPU workload proves the trace-disabled instruction-cost delta is at most 32 Ir/frame. | ✓ VERIFIED | The authorized Linux perf job produced `quality-perf` from the PR merge checkout. Independent validation of its downloaded JSON passed: exact run/source/producer identity, stable repeated samples, `delta_raw=900`, threshold `9600`, and `delta_ir_frame=3`. |

**Score:** 15/15 observable truths verified (0 present, behavior-unverified).

## Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `runner/main.c`, `src/internal.h`, `src/frame.c`, `src/cpu.c`, `src/bus.c` | Private trace capture, filtering, observer lifetime, and ordinary-path dispatch | ✓ VERIFIED | The previous report's implementation and wiring checks remain valid; the trace-specific tests passed and no product source changed in the final pin-fetch plan. |
| `tests/runner/test_trace.c`, `tests/runner/trace.cmake`, `tests/runner/test_save.c` | Numeric oracle, bounds, equality, and output-failure controls | ✓ VERIFIED | Registered tests and prior full local CI results cover these controls. |
| `tests/perf/trace.cmake`, `.github/workflows/ci.yml` | Same-source CPU Cachegrind comparison and source-qualified receipt | ✓ VERIFIED | The perf job now fetches the four existing pinned inputs before its tests-on configure, retains the five-minute cap, and produced the accepted receipt. |
| `docs/runner.md`, `docs/testing.md` | User-facing trace contract and test instructions | ✓ VERIFIED | Documentation budget checks passed and the recorded docs match the runner contract. |
| `27-CI-EVIDENCE.md`, `27-TRACE-ACCEPTANCE.md`, `27-06-CHECKPOINT.md` | Exact-source receipts, acceptance, and owner boundary history | ✓ VERIFIED | The earlier failed attempt remains terminal; the separate authorized continuation and its limited result are recorded. |

## Key Link Verification

| From | To | Via | Status | Details |
|---|---|---|---|---|
| `runner/main.c` | `src/frame.c` | Callback is installed around the selected `muhtari_run_frame` call and cleared after it | WIRED | Manual call-path trace confirmed instance ownership and selected-frame dispatch. |
| `src/cpu.c` | `src/bus.c` | Traced CPU calls the separately compiled trace bus producers | WIRED | CMake symbol-renaming and object definitions preserve separate ordinary and trace paths. |
| `.github/workflows/ci.yml` | `scripts/fetch-pin.sh` | Four existing pins are fetched before tests-on configure | WIRED | The new step fetches `wasmtime-x86_64-linux`, `7800basic-wasm`, `7800openbios-asm`, and `rcheevos` into the existing ignored destinations. |
| `.github/workflows/ci.yml` | `tests/perf/trace.cmake` | Same-source CPU workload and Cachegrind measurement write the receipt | WIRED | The receipt's inner producer is `tests/perf/trace.cmake`; the outer quality artifact preserves its source identity and samples. |
| `docs/runner.md` | `runner/main.c` | Documented command, fields, limits, and errors match the implemented CLI | WIRED | The earlier actual-runner and documentation checks remain source-qualified. |

The generic structural link query checks literal target references and misses several CMake/runtime links. Those links were traced manually through the actual CMake target definitions, runner call paths, and workflow commands.

## Data-Flow Trace

| Artifact | Data | Source | Produces real data | Status |
|---|---|---|---|---|
| Runner JSONL trace | CPU instruction and bus events | Diagnostic cartridge executed by the real core and runner | Yes | ✓ FLOWING; the fixed-value numeric oracle passed. |
| `quality-perf.json.trace_cost` | ON/OFF instruction counts and delta | Cachegrind over the same generated CPU workload and source | Yes | ✓ FLOWING; downloaded receipt validation passed. Merge source `fa72f560…` joins authorized head `4630288…` and base `89b6ff8…`. |

## Behavioral Spot-Checks

| Behavior | Command or run | Result | Status |
|---|---|---|---|
| Trace oracle, equality, POSIX paths, and trace-disabled configuration | Prior local `ctest`/full CI evidence in `27-TRACE-ACCEPTANCE.md` | Exit 0; exact trace, compatibility, and refusal checks passed | ✓ PASS |
| API, ABI, and state compatibility | Prior local `ctest` evidence in `27-TRACE-ACCEPTANCE.md` | Exit 0; five compatibility tests passed | ✓ PASS |
| Fresh fixture prerequisite path | Native CI Release configure, build `muhtari-run` and `muhtari-test-trace`, generate/run `trace-cost.a78` | All commands exit 0; runner stdout contains `diagnostic a5` | ✓ PASS; local fixture reachability only |
| Hygiene and repository policy | `cmake --workflow --preset hygiene` | Exit 0; all 30 checks passed | ✓ PASS |
| Hooks-off instruction cost | [PR CI run 44123425194](https://github.com/szTheory/muhtari/actions/runs/44123425194), Linux perf job `134180732604`; artifact `14347390138` | Exit 0; two stable repetitions per series; raw delta 900 ≤ 9,600 over 300 frames (3 Ir/frame). The exact `jq -e` source/provenance/arithmetic check also exited 0. | ✓ PASS |
| Full hosted CI and Release | [CI run 44123425194](https://github.com/szTheory/muhtari/actions/runs/44123425194); [Release run 44123425418](https://github.com/szTheory/muhtari/actions/runs/44123425418) | Both failed overall. Linux `perf` passed, but other required lanes failed on GCC `-Werror=mismatched-dealloc` and Clang's missing `realpath` declaration; quality roll-ups lacked required artifacts. Release publication/install steps were skipped. | ✗ NOT GREEN; shipping remains blocked |

The full CI and Release failures are distinct from the passing trace-cost producer. This verdict does not claim a green full-CI run or authorize a further push, dispatch, rerun, merge, or release.

## Regression Gate

The prior phase is documentation-only. Its verification and summaries identify no prior-phase automated test files, so no regression suite was selected or run.

## Probe Execution

No phase-declared probe script or repository `scripts/*/tests/probe-*.sh` was found.

## Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| TRACE-01 | 27-01 | Opt-in deterministic instruction and bus trace for a selected frame and range | ✓ SATISFIED | Actual-runner smoke, filter controls, and numeric oracle passed. |
| TRACE-02 | 27-01 | Record event/timing/beam/CPU fields and document filter semantics | ✓ SATISFIED | Fixed-value records and ordering passed; runner documentation matches. |
| TRACE-03 | 27-01, 27-02, 27-04 | Finite capture, truncation, early refusal, truthful I/O failures, and Windows paths | ✓ SATISFIED | Local controls and source-matched Windows x64/arm64 path jobs passed. |
| TRACE-04 | 27-01, 27-02, 27-03 | Exact diagnostic oracle and unchanged frame/canonical-state results | ✓ SATISFIED | Numeric oracle and trace-on/off equality matrix passed. |
| TRACE-05 | 27-01, 27-02, 27-03 | Runner-only seam with public API/ABI/state compatibility | ✓ SATISFIED | Public header and serializer are unchanged; API/ABI/state tests passed. |
| TRACE-06 | 27-01, 27-03, 27-04, 27-05, 27-06 | Hooks-off instruction-cost gate with observer-selected instrumentation | ✓ SATISFIED | Exact-source Linux Cachegrind receipt reports 3 Ir/frame against a 32 Ir/frame ceiling. The failed pre-measurement attempts remain separately recorded. |
| TRACE-07 | 27-03 | Runner documentation and fixture cleanliness | ✓ SATISFIED | Documentation budget, hygiene, and tracked-tree checks passed. |

**Orphaned requirements:** None. Every PLAN requirement ID maps to TRACE-01 through TRACE-07 in `.planning/REQUIREMENTS.md`; the superseded 27-05 plan is historical and not resumed.

## Test Quality Audit

The prior verification found no disabled requirement-linked tests, circular expected-value generation, or insufficient assertions. The final report carries that result forward; the new workflow-only plan adds no test and does not weaken the existing oracle or CI inventory.

## Decision Coverage

All 9 trackable CONTEXT decisions are honored. No new decision override was needed.

## Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| — | — | No unresolved `TBD`, `FIXME`, or `XXX` marker was found in the modified implementation/workflow scope. | — | — |

The standard code review of `.github/workflows/ci.yml` found no issues. Its full report and reconciled prior-finding dispositions are recorded in `27-REVIEW.md` and `27-REVIEW-DISPOSITION.md`.

## Human Verification Required

None. The phase has no subjective acceptance item; the source-qualified Linux receipt is machine-verifiable. The separate failed CI and Release gates remain a shipping stop.

## Gaps Summary

The prior verifier's only gap is closed by the accepted Linux Cachegrind receipt: the measured raw delta is 900 against the 9,600-instruction limit, with stable repeated samples tied to the authorized PR head, base, and merge checkout. The workflow fetch fix and local generated-fixture path are also verified. All Phase 27 success criteria and requirements pass. The automatic CI and Release workflows remain failed in separate build lanes, so this report does not establish shipping readiness or a green full-CI result.

---

_Verified: 2026-11-17T17:27:20Z_
_Verifier: the agent (gsd-verifier)_
