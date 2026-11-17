---
phase: 27-headless-developer-trace
verified: 2026-11-17T00:22:51Z
status: gaps_found
score: 13/15 must-haves verified
covered_files:
  - .github/workflows/ci.yml
  - .planning/phases/27-headless-developer-trace/27-01-PLAN.md
  - .planning/phases/27-headless-developer-trace/27-01-SUMMARY.md
  - .planning/phases/27-headless-developer-trace/27-02-PLAN.md
  - .planning/phases/27-headless-developer-trace/27-02-SUMMARY.md
  - .planning/phases/27-headless-developer-trace/27-03-PLAN.md
  - .planning/phases/27-headless-developer-trace/27-03-SUMMARY.md
  - .planning/phases/27-headless-developer-trace/27-BOUNDS.md
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
covered_digest: "v4:sha256:bc4e0d0e5d37bef9b1c5920100dd4aa75dfc1626228deea95c33e327950b2b53"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 9
  total: 9
  not_honored: []
gaps:
  - truth: "Windows-specific trace output path collision controls pass on the supported Windows runners."
    status: partial
    reason: "The junction and ordinary trailing-dot/space controls are wired under CMAKE_HOST_WIN32, but this Darwin arm64 verification exercised only the POSIX branch. The exact-source Windows CI jobs have not run."
    artifacts:
      - path: tests/runner/trace.cmake
        issue: "The Windows-only junction and trailing-suffix cases exist but have no Windows execution result for this source."
      - path: .github/workflows/ci.yml
        issue: "The existing Windows x64 and arm64 CI jobs were not triggered on the current source."
    missing:
      - "Passing existing Windows x64 and arm64 CI receipts for the exact source revision."
  - truth: "A real CPU workload measures no-observer instruction cost at no more than 32 Ir/frame on this source."
    status: partial
    reason: "The same-source Cachegrind measurement and threshold gate are wired into the existing Linux perf job, and local comparator controls pass, but no authentic Cachegrind receipt exists for this revision. Valgrind is unavailable on this Darwin arm64 host."
    artifacts:
      - path: tests/perf/trace.cmake
        issue: "The real measurement mode has not produced its source-qualified trace-cost receipt."
      - path: .github/workflows/ci.yml
        issue: "The existing Linux PR perf job has not run on this source."
    missing:
      - "A passing source-qualified Cachegrind trace_cost receipt from the existing Linux PR perf job."
---

# Phase 27: Headless developer trace verification

**Phase Goal:** Developers can inspect deterministic Atari 7800 instruction and bus-access events through the existing headless runner without changing emulated results or public compatibility.

**Verified:** 2026-11-17T00:22:51Z  
**Status:** gaps_found  
**Re-verification:** No — initial verification

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | A developer can select one invocation frame and inclusive address range and receive a separate versioned JSONL trace with event kind, master tick, beam position, and CPU context; bus records include address, direction, and value. | VERIFIED | `runner/main.c` validates `--trace`, `--trace-frame`, and `--trace-range`, captures only the selected frame, and serializes the fields. `tests/runner/trace.cmake` drives the actual runner; the oracle and smoke CTests passed. |
| 2 | Instruction-start-PC and CPU bus-address filters are independent, inclusive, and retain observation order, including instruction-before-opcode-bus ties. | VERIFIED | `trace_capture_event` filters by PC for instructions and address for bus events without sorting. `tests/runner/test_trace.c` pins same-tick ordering and exact filter results; `runner.trace.oracle` passed. |
| 3 | Event timestamps use the CPU bus data edge and records contain the documented beam coordinates and sampled registers. | VERIFIED | Trace producers in `src/bus.c` derive the event tick from the data edge and frame origin; fixed numeric prefix/tail records and register checks in `test_oracle` passed through the actual runner. |
| 4 | The runner attaches and detaches the private observer only around the selected invocation frame, and the frame loop chooses the traced path once at its entry. | VERIFIED | `runner/main.c` sets and clears the instance callback around `muhtari_run_frame`; `src/frame.c` dispatches to `muhtari__trace_cpu_step` only when the callback is non-null. The instance/detach edge tests are registered in `core.trace.edges`. |
| 5 | The event cap distinguishes exactly-full output from one-extra-match truncation and lets emulation finish. | VERIFIED | `trace_capture_event` sets a sticky truncated flag only when another matching event arrives at the limit. `runner.trace.limits` checks exact/full and one-extra cases and compares later frame hashes; it passed in the recorded full CI workflow. |
| 6 | Capture memory and serialized output have checked finite bounds grounded in source invariants. | VERIFIED | `runner/main.c` bounds event count, event allocation, maximum line and output size with checked arithmetic/static assertions; `27-BOUNDS.md` derives the limit. `runner.trace.bounds` is registered in CI and the recorded full CI run passed. |
| 7 | Invalid or conflicting trace options fail before emulation and cannot overwrite inputs or other outputs. | UNCERTAIN | The Darwin/POSIX actual-main path matrix and option tests pass, and Windows junction/trailing-suffix cases are present. The Windows-only branch has not been executed on a Windows runner for this source; see the first gap. |
| 8 | Trace open/write/flush/sync/close/replace failures return failure, preserve an old destination, and do not publish a fresh complete-looking trace. | VERIFIED | `write_sibling_atomic_stream` commits only after successful write, flush, sync, close, and replace. The injected actual-main fault matrix is wired by `runner.trace.io`; the recorded full CI passed. |
| 9 | A generated diagnostic produces an exact numeric trace oracle independent of runtime output generation. | VERIFIED | `test_trace.c` writes the build-tree cartridge as input and compares runner JSONL against fixed record values, counts, ticks, registers and order. It also rejects corrupted tick, tie order and register values. The oracle CTest passed. |
| 10 | Tracing leaves deterministic stdout, frame hashes, canonical state hashes/bytes, and supported legacy/YM/Souper behavior unchanged. | VERIFIED | `runner.trace.equality` compares traced and untraced runs across NTSC/PAL and the listed cartridge/state cases. It checks frame/hash output and saved-state bytes; the test passed. |
| 11 | The feature stays private and preserves public API 8, ABI 1, behavior revision, and state formats; observer pointers are excluded from canonical state. | VERIFIED | `include/muhtari.h` and `src/state.c` are unchanged against `origin/main`; versions remain API 8 and ABI 1. The five API/ABI/state regression tests passed, and the equality matrix compares canonical state bytes. |
| 12 | An unobserved frame uses the ordinary CPU/bus path without a per-access trace conditional; trace objects can be omitted in a product-only build. | VERIFIED | The ordinary `src/cpu.c`/`src/bus.c` objects remain separate from compile-time-renamed trace variants. `src/frame.c` dispatches once per frame. The trace-disabled configuration test passed, and a product-only OFF runner rejected a trace request with exit 77 without creating a file. |
| 13 | Runner documentation accurately explains invocation, filtering, record fields, bounds, truncation, error behavior, and CPU-only limits. | VERIFIED | `docs/runner.md` contains the tested option/schema contract and limits; `docs/testing.md` lists the recurring checks. The full CI workflow's documentation budget checks passed. |
| 14 | Required local product, hygiene, compatibility, and fixture-cleanliness checks pass without public/state-format changes or committed runtime media. | VERIFIED | The recorded final-source `cmake --workflow --preset ci` run passed 736 tests, 8 release checks and packaging; hygiene passed all 30 checks. `git diff` confirms no public-header/state-serializer changes or ROM/BIOS/save/trace artifacts. |
| 15 | A real generated CPU workload proves the trace-disabled instruction-cost delta is at most 32 Ir/frame. | UNCERTAIN | The local comparator-control CTest passed, but controls are not a Cachegrind measurement. There is no `quality-perf.json`/trace-cost receipt in the build tree, and `valgrind` is unavailable locally; see the second gap. |

**Score:** 13/15 observable truths verified. Two truths await exact-source automated CI evidence; no implementation failure was observed in local checks.

## Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `runner/main.c` | Trace option validation, capture, filtering, serialization and atomic output | VERIFIED | Substantive CLI implementation; actual-runner smoke, oracle, limits, paths and I/O cases are registered. |
| `src/internal.h`, `src/frame.c`, `src/cpu.c`, `src/bus.c` | Private event seam and dual-compiled dispatch/producers | VERIFIED | Event structure is fixed-width and bounded; the callback is per-instance; traced symbols are renamed at compile time. |
| `tests/runner/test_trace.c`, `tests/runner/trace.cmake`, `tests/runner/test_save.c` | Generated numeric, compatibility, boundary and I/O/path acceptance | VERIFIED | CTest drives actual `muhtari-run`; independent expected records, exit codes, files and fault outcomes are asserted. |
| `tests/perf/trace.cmake`, `.github/workflows/ci.yml` | CPU-backed Cachegrind measurement and same-run receipt | PRESENT, WIRED | Existing Ubuntu perf job builds both variants and runs the generated workload before the normal PR artifact step. No receipt exists yet. |
| `docs/runner.md`, `docs/testing.md` | User-facing trace contract and recurring test instructions | VERIFIED | The examples and limitations match the CLI and output schema. |
| `cmake/TraceLoopConfig.cmake`, `tests/cmake/trace_loop_config.cmake` | Clear product-only OFF configuration and rejection of incompatible tests-on build | VERIFIED | Both option combinations are controlled; the dedicated CTest passed. |

## Key Link Verification

| From | To | Via | Status | Details |
|---|---|---|---|---|
| `runner/main.c` | `src/frame.c` | callback set around selected `muhtari_run_frame` and cleared on return | WIRED | Manual call-path trace: `runner/main.c` → public frame call → `src/frame.c` checks the private callback and chooses the traced CPU entry point. |
| `src/cpu.c` | `src/bus.c` | trace variant calls renamed trace bus functions | WIRED | `CMakeLists.txt` gives trace objects symbol-renaming definitions; traced bus producers live in the separately compiled trace object. |
| `src/bus.c` | `tests/runner/trace.cmake` | actual runner JSONL compared to numeric/order expectations | WIRED | CMake registers the script; it invokes the generated fixture, real runner, and fixed-value `muhtari-test-trace` oracle. |
| `runner/main.c` | `src/internal.h` | private event allocation and callback fields | WIRED | `runner/main.c` consumes `muhtari__trace_event` and the instance callback declared in the private header. |
| `tests/runner/test_save.c` | `runner/main.c` | injected wrappers exercise real runner entry on I/O faults | WIRED | The test-save target uses the existing actual-main harness to count frames, writes and replacement operations. |
| `.github/workflows/ci.yml` | `tests/perf/trace.cmake` | same-source Linux Cachegrind gate and receipt | WIRED | The `perf` job calls measure mode and embeds its output in the existing PR `quality-perf.json`; execution is still pending. |
| `docs/runner.md` | `runner/main.c` | documented CLI/schema behavior | WIRED | Documentation matches option validation, output fields, bounds and exit statuses; the docs budget and runner CTests pass. |

The generic key-link query missed several links because they are CMake/runtime connections rather than literal path references. Each was traced manually through the target definitions and test invocations above.

## Data-Flow Trace

| Artifact | Data | Source | Produces real data | Status |
|---|---|---|---|---|
| `muhtari-run` JSONL | CPU instruction and bus event records | Generated/owned cartridge bytes executed by the real core | Yes | FLOWING; fixed numeric oracle passes. |
| `trace-cost.json` and `quality-perf.json` | Instruction counts, delta, tool/source/fixture identity | Cachegrind over same-source ON/OFF runners and generated CPU workload | No receipt for current source | NOT YET PRODUCED; local controls are synthetic boundary cases only. |

## Behavioral Spot-Checks

| Behavior | Command | Result | Status |
|---|---|---|---|
| Exact numeric trace, trace-on/off compatibility, POSIX paths, and OFF config guard | `ctest --test-dir build/ci --output-on-failure -R '^(runner[.]trace[.](oracle|equality|paths|disabled_config))$'` | exit 0, 4/4 passed | PASS |
| Perf threshold controls | `ctest --test-dir build/ci --output-on-failure -R '^perf[.]trace[.]controls$'` | exit 0, 1/1 passed | PASS (controls only) |
| Public API/ABI and state regressions | `ctest --test-dir build/ci --output-on-failure -R '^abi[.](api_v8|symbols)$|^runner[.]state[.]'` | exit 0, 5/5 passed | PASS |
| Product-only trace-loop OFF refusal | `build/trace-off-local/runner/muhtari-run --frames 1 --trace /tmp/muhtari-verifier-trace-off.jsonl --trace-frame 1 --trace-range 0:65535 tests/diag/souper.a78` | exit 77; diagnostic says trace support is unavailable; no output file created | PASS |
| Full local CI and hygiene | `cmake --workflow --preset ci`; `cmake --workflow --preset hygiene` | exit 0, 736 CI tests plus 8 release checks; exit 0, 30 hygiene/policy checks; both N/A-local | PASS (recorded final-source runs) |
| Authentic Cachegrind measurement | `command -v valgrind` | exit 1 on Darwin arm64; no receipt file found | NOT RUN; required Linux PR evidence remains pending |

The POSIX `runner.trace.paths` branch passed locally. The Windows-only junction and trailing-suffix branch has no Windows execution result on this source; the existing Windows x64/arm64 CI jobs are required to verify it.

## Probe Execution

No phase-declared `probe-*.sh` or repository `scripts/*/tests/probe-*.sh` was found. Not applicable.

## Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| TRACE-01 | 27-01 | Opt-in deterministic instruction and bus trace for selected frame/range | SATISFIED | Actual-runner smoke, oracle and filter controls pass. |
| TRACE-02 | 27-01 | Record event/timing/beam/CPU fields and document filter semantics | SATISFIED | Fixed numeric oracle checks fields/order; runner docs cover schema. |
| TRACE-03 | 27-01, 27-02 | Finite capture, truncation, early argument refusal and truthful file failures | PARTIAL | Current-host controls pass; Windows-only path collision controls await Windows CI. |
| TRACE-04 | 27-01, 27-03 | Exact fixture oracle and unchanged frame/canonical state hashes | SATISFIED | `runner.trace.oracle` and `runner.trace.equality` pass. |
| TRACE-05 | 27-01, 27-02, 27-03 | Runner-only private seam with API/ABI/state compatibility | SATISFIED | Public header/state serializer unchanged; API/ABI/state tests pass. |
| TRACE-06 | 27-01, 27-03 | Hooks-off instruction-cost gate and observer-selected instrumentation | PARTIAL | Selection/wiring and control tests pass; authentic ≤32 Ir/frame receipt is missing. |
| TRACE-07 | 27-03 | User docs and no committed ROM/BIOS/save/log bytes | SATISFIED | Docs budget checks pass; no such changed files or runtime artifacts are tracked. |

**Orphaned requirements:** None. The three plans declare TRACE-01 through TRACE-07; all map to this phase.

## Test Quality Audit

| Test File | Linked Requirements | Active | Skipped | Circular | Assertion Level | Verdict |
|---|---|---:|---:|---:|---|---|
| `tests/runner/test_trace.c` | TRACE-01, 02, 04, 05 | Yes | 0 | No | Exact numeric values, event order, fields, hashes and bytes | PASS; fixture bytes are generated as input, expected trace rows are fixed independently. |
| `tests/runner/trace.cmake` | TRACE-01, 02, 03, 04 | Yes | 0 | No | Actual-runner behavior, exit codes, file contents and corruption rejection | PASS; registered in CMake and both CI/WASI inventories. |
| `tests/runner/test_save.c` | TRACE-03 | Yes | 0 | No | Actual-main fault injection and filesystem side effects | PASS on Darwin/POSIX; Win32 cases are present but not executed here. |
| `tests/perf/trace.cmake` | TRACE-06 | Yes | 0 | No | Exact threshold boundary/fault controls; measured samples in real mode | PASS for controls; real measurement awaits Linux CI. |
| `tests/cmake/trace_loop_config.cmake` | TRACE-06 | Yes | 0 | No | Configure failure/success and diagnostic | PASS; dedicated CTest passed. |

Disabled-test scan found no skip markers in the linked C/CMake tests. The exact trace oracle does not compute expected events by calling Muhtari; the parity matrix intentionally compares the same source with trace off/on and is not used as cost evidence. The misleading local performance signal is `perf.trace.controls`: it tests the comparator with synthetic samples and does not measure instruction cost. The uncovered execution path is the Windows-specific path branch, which requires the existing Windows CI runners.

**Disabled tests on requirements:** 0. **Circular patterns detected:** 0. **Insufficient assertions:** 0.

## Decision Coverage

All trackable CONTEXT.md decisions are honored by shipped artifacts. The decision-coverage query returned 9 total, 9 honored, and no unhonored decisions.

## Anti-Patterns Found

No unresolved `TBD`, `FIXME`, or `XXX` markers were found in phase-modified implementation or test files. The textual “not implemented” occurrences in documentation describe unsupported devices and system errors; they are not stubs in the trace path.

## Human Verification Required

None. This phase has no subjective UAT requirement. Both remaining items are automated evidence from the existing Windows and Linux PR CI jobs.

## Gaps Summary

The implementation, numeric oracle, parity matrix, resource bounds, local compatibility checks, docs, and local hygiene are present and pass. Two exact-source CI facts remain: Windows must execute the platform-specific path controls, and Linux must produce the authentic Cachegrind receipt proving the 32 Ir/frame ceiling. The local performance comparator controls are not a substitute. No later phase in this milestone addresses either item, and no remote workflow was triggered.

---

_Verified: 2026-11-17T00:22:51Z_  
_Verifier: the agent (gsd-verifier)_
