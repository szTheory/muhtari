---
phase: 16-emscripten-libretro-core
verified: 2026-11-10T21:14:07.564911Z
status: passed
score: 7/7 must-haves verified
covered_files:
  - .github/workflows/ci.yml
  - .planning/milestones/v4.0-phases/16-emscripten-libretro-core/16-01-PLAN.md
  - .planning/milestones/v4.0-phases/16-emscripten-libretro-core/16-01-SUMMARY.md
  - CMakePresets.json
  - docs/libretro.md
  - libretro/CMakeLists.txt
  - scripts/emscripten.sh
  - tests/emscripten/host.mjs
  - tests/emscripten/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/emscripten.txt
  - tests/libretro/observe.c
  - tests/libretro/tests.cmake
  - tests/tests.cmake
covered_digest: "v4:sha256:644ca08fe95185253049e15c470dd180e94ca6acff7ad893ea70d202b9892960"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 0
  total: 0
  not_honored: []
human_verification: []
---

# Phase 16: Emscripten libretro core Verification Report

**Phase Goal:** Integrators can build and load a browser-targeted form of the existing libretro core, and maintainers can verify its deterministic behavior with an offline host.  
**Verified:** 2026-11-10T20:32:36Z  
**Status:** passed  
**Re-verification:** No

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | Integrators can reproduce the pinned production Emscripten loader and companion Wasm, identify the loader contract and actual surfaces, while native MODULE and WASI configuration remains separate. | ✓ VERIFIED | The CMake Emscripten branch is scoped to the production target; native MODULE and RGB565 paths remain separate, and the WASI preset keeps libretro disabled. The final lane produced the pinned artifact and wasm-surface.json. Docs describe the loader, imports and browser-facing factory contract. |
| 2 | A pinned offline Node host loads the emitted artifact with the owned fixture, registers callbacks and completes libretro load/run/serialize/unserialize/unload operations. | ✓ VERIFIED | Final emscripten.artifact JUnit case passed with 0 failures, 0 disabled and 0 skipped. It directly imports the generated factory with local Wasm bytes; output reports 14 parity frames. |
| 3 | Frame, audio and input observations match independent native expectations before hash comparison, without adding ROM/BIOS bytes. | ✓ VERIFIED | native.json records numeric oracles: 320x242, 48 kHz, 800/801 audio frames and SWCHA 255/239/254. The observer compares public-library frame/audio values with the adapter before emitting hashes; the Node host checks numeric values before hash equality. The implementation diff adds no ROM/BIOS fixtures. |
| 4 | Malformed content/state is refused according to adapter semantics, applicable state is preserved, and restored continuation replays identically. | ✓ VERIFIED | Native receipt flags confirm refusal and preservation. The passing artifact test exercises null/empty/short/long/corrupt states, malformed-content transitions and two continuation replays. |
| 5 | Two factory instances retain separate memories and state when runs and restores are interleaved. | ✓ VERIFIED | Core.isolatedFactory creates a second production factory and checks each instance’s saved state after interleaved execution and restore. The artifact CTest passed and reports instance isolation. |
| 6 | Native observer/fault tests run without Docker in ordinary CI, and required Linux CI loads the actual Emscripten artifact with an exact non-skipping inventory. | ✓ VERIFIED | Retained cmake --workflow --preset ci receipt exited 0; libretro.observe and controls passed in the 703-test workflow. The workflow invokes scripts/emscripten.sh on Linux x64 without advisory handling. Final Emscripten JUnit lists both inventoried cases, run with no skips; inventory validation passed. |
| 7 | Maintainers can reproduce same-source size and warmed-throughput measurements with runtime and interpretation limits recorded. | ✓ VERIFIED | Final measurement.json records 57,902 loader bytes, 191,040 Wasm bytes, 248,942 combined bytes, 12 warm-up frames and three 60-frame windows. Native and Node medians are 798.945392 and 996.2247233919802 frames/s. It identifies AppleClang 21.0.0.21000101, Node v24.19.0/linux-x64, Darwin arm64 host, Linux/aarch64 Docker daemon and selected Linux/amd64 SDK leaf; it makes no browser/device claim. The summary narrative matches this final receipt. |

**Score:** 7/7 truths verified (0 present, behavior-unverified)

### Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| libretro/CMakeLists.txt | Target-scoped Emscripten production link and explicit exports | ✓ VERIFIED | Substantive production target links the existing adapter/library/host and is consumed by the Emscripten test lane. |
| tests/libretro/observe.c | Independent native observer, numeric assertions and refusal controls | ✓ VERIFIED | Compiled with the adapter and public library, registered in native CTest, and writes evidence only after assertions. |
| tests/emscripten/host.mjs | Direct offline artifact consumer, parity/refusal/isolation controls and report | ✓ VERIFIED | Invoked by artifact/controls tests and report mode; imports the production module and uses real fixture/callback data. |
| tests/emscripten/tests.cmake | Required Node/evidence inputs and artifact test registration | ✓ VERIFIED | Included by the Emscripten inventory branch; registers only the two inventoried tests. |
| scripts/emscripten.sh | Pinned reproduction and evidence checks | ✓ VERIFIED | Called by CI and documented. Final lane exit 0; container build/tests ran offline with read-only source and separate writable build/evidence mounts. |
| docs/libretro.md | Reproduction, loader/callback contract and measurement limits | ✓ VERIFIED | Substantive user-facing guide documents pin, command, output paths, factory use and limits. |

The artifact query reported all 6 declared artifacts passing. Wiring was checked separately below.

### Key Link Verification

| From | To | Via | Status | Details |
|---|---|---|---|---|
| libretro/CMakeLists.txt | tests/emscripten/host.mjs | Generated loader/Wasm and retained factory/runtime exports | ✓ WIRED | CMake emits the production artifact; Emscripten CTest passes its loader path to Node, which imports the module and checks required exports. |
| tests/libretro/observe.c | tests/emscripten/host.mjs | Shared fixture and native observation JSON | ✓ WIRED | Observer writes build/ci/tests/emscripten/native.json; the Emscripten test passes that exact path. Host checks fixture SHA-256 and source identity before using observations. |
| .github/workflows/ci.yml | scripts/emscripten.sh | Required Linux x64 step after native tests | ✓ WIRED | Workflow invokes the script without continue-on-error/advisory handling and uploads its artifacts. |
| tests/tests.cmake | tests/inventory/emscripten.txt | Inventory-selected include and exact registration/JUnit validation | ✓ WIRED | Emscripten configuration includes tests/emscripten/tests.cmake; the generic inventory checker uses the selected inventory, and the final JUnit check confirms each test ran once. |

The generic key-link matcher missed three generated-file/variable-based links. Manual producer-to-consumer tracing and the passing direct-artifact receipt verify them.

### Data-Flow Trace (Level 4)

| Artifact | Data variable | Source | Produces real data | Status |
|---|---|---|---|---|
| Production .mjs/.wasm | callbacks and libretro state | Existing core built as the production adapter | Yes; Node executes emitted artifact | ✓ FLOWING |
| native.json | numeric oracles, frame/audio hashes and state observations | Owned generated fixture; independent public-library instance and actual adapter | Yes; checks precede report writing | ✓ FLOWING |
| host.mjs | video/audio/input and continuation observations | Actual Wasm callbacks, current heap views and native receipt | Yes; callback bytes are copied and compared | ✓ FLOWING |
| measurement.json | artifact sizes and throughput windows | Built artifacts and native/Node runs over same fixture/workload | Yes; fixed workload and environment metadata are recorded | ✓ FLOWING |

### Behavioral Spot-Checks

| Behavior | Command/evidence | Result | Status |
|---|---|---|---|
| Native numeric oracle and fault controls | Final cmake --workflow --preset ci receipt; libretro.observe and .controls | Exit 0; both named tests passed and native observer receipt contains expected numeric/refusal results | ✓ PASS |
| Production artifact lifecycle, parity, state replay and instance isolation | Final sh scripts/emscripten.sh build/ci receipt; emscripten.artifact | Exit 0; JUnit reports direct 14-frame artifact verification and replay/isolation checks | ✓ PASS |
| Missing callback/export, mismatch and incomplete-observation controls | Same Emscripten receipt; emscripten.controls | Exit 0; test ran with no skips and expected fault diagnostics | ✓ PASS |

These are retained final-source receipts; no full workflow was repeated during verification.

### Probe Execution

No probes were declared in the phase plan/summary, and no scripts/*/tests/probe-*.sh files were found.

### Decision Coverage

The centralized check returned “No trackable decisions in CONTEXT.md.” No trackable decision was omitted from shipped artifacts.

### Test Quality Audit

| Test File | Linked Req | Active | Skipped | Circular | Assertion Level | Verdict |
|---|---|---|---|---|---|---|
| tests/libretro/observe.c | WASM-03, WASM-04 | libretro.observe and controls passed in native CI | 0 reported for these cases | No; independent public-library output and numeric oracles precede adapter hashes | Value and behavioral | PASS |
| tests/emscripten/host.mjs | WASM-02, WASM-03, WASM-04 | emscripten.artifact and controls ran in final JUnit | 0 disabled, 0 skipped | No; consumes the independently checked native receipt and separately executes Wasm | Value and behavioral | PASS |

**Disabled tests on requirements:** 0. **Circular expected-value patterns:** 0. **Insufficient assertions:** 0. The native receipt is observer output, not a self-generated fixture: independent numeric expectations and public-library results are checked first.

### Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| WASM-01 | 16-01-PLAN.md | Pinned browser-consumable build and loader/surface contract | ✓ SATISFIED | Production artifact built from pinned SDK; actual surface captured and reproduction documented. |
| WASM-02 | 16-01-PLAN.md | Offline Node lifecycle with owned fixture and callback capture | ✓ SATISFIED | Passing artifact CTest exercises emitted factory and callbacks. |
| WASM-03 | 16-01-PLAN.md | Independent native parity, refusal/fault controls and no new ROM/BIOS bytes | ✓ SATISFIED | Native and artifact tests pass; receipts record numeric, state/content and fault controls. |
| WASM-04 | 16-01-PLAN.md | Reproducible size/throughput report with stated limits | ✓ SATISFIED | Final measurement records same-source sizes/windows and environment; no browser/device claim. |

All Phase 16 requirements are claimed by the plan; none is orphaned. Nine deferred v3.0 requirements and Phase 12/14 stops remain unchanged and outside this offline phase.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| tests/emscripten/host.mjs | 213 | return null for unsupported core option | Info | Intentional sentinel: environment callback returns false for unsupported options. It does not produce placeholder output or hide a real-data path. |

No debt markers or TODO/HACK placeholders, console-log-only implementations, or hardcoded empty user-visible data were found in implementation files.

## Human Verification Required

N/A — infrastructure/tooling phase with no user-facing UI or external service behavior. Observable lifecycle, fault controls, outputs and measurements are automated; no behavior-dependent truth remains unexercised.

## Gaps Summary

None. All roadmap success criteria and merged plan-specific must-haves are verified against source wiring and fresh local receipts. Local run URLs are N/A; no remote CI, live host, browser/device run, publication or merge was performed.

---

_Verified: 2026-11-10T20:32:36Z_  
_Verifier: the agent (gsd-verifier)_


## Shipping re-verification

Inline verifier recheck after reconciling current main: the only changed covered file is docs/libretro.md. Historical release identity prose moved byte-for-byte into the linked milestone receipt; all sections from Per-game launch paths onward, including the Emscripten reproduction and loader contract, remain byte-identical. The current version scanner, next-version scanner and unchanged document budgets exit 0. The two-worker FIFO invocation of the unchanged native workflow exits 0; native and release JUnit inventories validate without skips. `sh scripts/emscripten.sh build/ci` exits 0 with its exact two-case inventory, 14 parity frames, replay and instance isolation. Run URL N/A-local. The CLI regenerated covered fields after this check; no acceptance scope or host allowance changed.
