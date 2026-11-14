---
phase: 22-complete-souper-homebrew-mapper
verified: 2026-11-14T07:06:40Z
status: gaps_found
score: "7/8 must-haves verified; 1 partial"
covered_files:
  - .planning/phases/22-complete-souper-homebrew-mapper/22-01-PLAN.md
  - .planning/phases/22-complete-souper-homebrew-mapper/22-01-SUMMARY.md
  - .planning/phases/22-complete-souper-homebrew-mapper/22-02-PLAN.md
  - .planning/phases/22-complete-souper-homebrew-mapper/22-02-SUMMARY.md
  - .planning/phases/22-complete-souper-homebrew-mapper/22-03-PLAN.md
  - .planning/phases/22-complete-souper-homebrew-mapper/22-03-SUMMARY.md
  - docs/embedding.md
  - docs/emscripten-hosting.md
  - docs/hardware/cartridge.md
  - docs/hardware/ym2151.md
  - docs/libretro.md
  - docs/quality.md
  - docs/runner.md
  - docs/testing.md
  - libretro/libretro.c
  - src/cart.c
  - src/instance.c
  - src/internal.h
  - src/state.c
  - tests/api/v8.c
  - tests/audio/expected.txt
  - tests/audio/test_ym2151.c
  - tests/built/expected.txt
  - tests/bus/test_bus.c
  - tests/bus/tests.cmake
  - tests/cart/test_cart.c
  - tests/cart/tests.cmake
  - tests/cmake/hashes.cmake
  - tests/cmake/info_claims.cmake
  - tests/cmake/quality.cmake
  - tests/core/state_compat.c
  - tests/core/state_compat.cmake
  - tests/core/test_core.c
  - tests/core/test_state.c
  - tests/core/tests.cmake
  - tests/diag/driver.c
  - tests/diag/expected.csv
  - tests/diag/souper.a78
  - tests/diag/souper.cfg
  - tests/diag/souper.s
  - tests/diag/tests.cmake
  - tests/emscripten/host.mjs
  - tests/emscripten/souper.cmake
  - tests/emscripten/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/emscripten.txt
  - tests/inventory/wasm32.txt
  - tests/libretro/host.c
  - tests/libretro/observe.c
  - tests/libretro/souper_observe.cmake
  - tests/libretro/tests.cmake
  - tests/policy/quality.cmake
  - tests/roms/manifest.txt
  - tests/runner/tests.cmake
  - tests/state-compat/CMakeLists.txt
covered_digest: "v4:sha256:d9608a69c7f2be089206be53fc76b914d54d2c9b15450be5e7f90b1a64697d44"
behavior_unverified: 1
overrides_applied: 0
gaps:
  - truth: "The generated Souper diagnostic and restored continuation run through the dynamically loaded native libretro module."
    status: partial
    reason: "The generated diagnostic runs through the public library, runner, and statically linked observer; the loaded-module case uses a separate synthetic program and does not test Souper state continuation."
    artifacts:
      - path: "tests/libretro/host.c"
        issue: "The loaded Souper module case checks memory descriptors with a separate inline program."
      - path: "tests/libretro/tests.cmake"
        issue: "The loaded-module Souper test does not receive the generated diagnostic fixture or its literal oracle."
    missing:
      - "Run the generated fixture through the actual built module and assert independent literal CPU and MARIA observations."
      - "Serialize at a fixed diagnostic boundary, restore through the loaded module, and compare the continued observations."
      - "Retain malformed-state refusal and state-preservation coverage for the Souper loaded-module route."
decision_coverage:
  honored: 9
  total: 9
  not_honored: []
---

# Phase 22: Complete Souper homebrew mapper Verification Report

**Phase Goal:** A homebrew developer can load the documented Souper mapper-only profile and obtain deterministic CPU/MARIA mapping and snapshot continuation through every existing deliverable and the applicable portable consumer.
**Verified:** 2026-11-14T06:21:57Z
**Status:** passed
**Re-verification:** No — initial verification

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | The exact A78 v3/v4 Souper declaration and 512 KiB ROM / 32 KiB EXRAM profile is admitted; malformed, oversized, option-bearing and unsupported-audio forms are rejected before allocation or changing loaded content. | ✓ VERIFIED | `src/cart.c` validates the selected v4/v3 declaration and exact body geometry before transactional install. `cart.souper.profile` and `cart.load.no_alloc_on_reject` passed; the latter asserts the reject path does not allocate. Docs name the software profile and limits. |
| 2 | Real CPU reads and writes expose banked ROM, register aliases and flat/segmented EXRAM at literal boundary values independently authored from the mapper. | ✓ VERIFIED | `core.souper.cpu` and `diag.souper_tracer` passed. `tests/diag/expected.csv` supplies 26 literal `souper_map` rows; the driver consumes them as expectations rather than deriving them from mapper helpers. |
| 3 | MARIA fetches use a distinct tick-aware cartridge view, with character remapping and the trap changing at the existing bus/HALT reader boundary. | ✓ VERIFIED | `bus.souper.halt_views` and `diag.souper_tracer` passed. `src/cart.c` keeps CPU and DMA reads separate; DMA selection is driven by `src/bus.c` at the requested tick. The diagnostic compares mailbox and pixel values to literal expectations. |
| 4 | Reset, unload/reload and separate instances initialize and own mapper latches and EXRAM deterministically. | ✓ VERIFIED | `core.souper.lifecycle` passed. `cart_reset` clears the mapper latches and the full instance-owned RAM span; cart install checks and allocates the complete ROM+EXRAM extent before replacing live content. |
| 5 | Souper format-3 / behavior-8 state restores every latch and EXRAM byte into existing storage; malformed state is rejected before mutation or allocation. | ✓ VERIFIED | `core.state.souper` passed. `src/state.c` visits all Souper latches and the full EXRAM span, validates envelope, topology and payload before apply, and uses existing allocation. The focused test covers continuation and repaired-integrity semantic faults. |
| 6 | Existing legacy format-1 / behavior-6 and YM format-2 / behavior-7 state bytes and continuation remain compatible in both directions; API 8 / ABI 1 remain unchanged. | ✓ VERIFIED | Reran `tests/core/state_compat.cmake` against the current `a2efefb` library and genuine pre-Souper producer. Exit 0: 10 pre-Souper legacy cases, 2 pre-Souper YM cases, 10 historical cases, both producer/consumer directions, and semantic/content refusal controls passed. `src/instance.c` selects version 3/8 only for the Souper topology. |
| 7 | The same generated diagnostic works through the public C library, headless runner, loaded libretro module and production portable module, and `$8007` adds no BupChip audio behavior. | ⚠ PARTIAL | The public library, runner, statically linked libretro observer and production Wasm host pass their generated-image/literal-oracle checks. The dynamically loaded native test checks Souper memory descriptors with a separate program; it does not load the generated diagnostic or exercise Souper save/restore continuation. `$8007` audio controls pass. |
| 8 | Integrator docs describe the exact profile, reset/EXRAM and snapshot policy, unsupported forms, and software-model evidence limits without claiming physical-cartridge accuracy. | ✓ VERIFIED | `docs/hardware/cartridge.md`, `docs/runner.md`, `docs/libretro.md`, `docs/testing.md` and `docs/emscripten-hosting.md` agree on the profile, state versions and host boundaries. The diagnostic fixture is generated and listed as owned; no copyrighted image is used. The phase review is clean, and the local quality receipt has zero ABI findings, zero warnings and archive size within the established ceiling. |

**Score:** 7/8 truths verified; truth 7 is partial.

The first four roadmap success criteria pass. The fifth is partial until the same generated diagnostic and a restored continuation run through the dynamically loaded native libretro module. All seven SOUPER requirements remain mapped below; SOUPER-04 and SOUPER-06 are partial at that consumer seam.

## Deferred Items

None. The milestone roadmap contains only Phases 21 and 22; no later phase covers a Phase 22 gap.

## Required Artifacts

The canonical artifact query passed 4/4 declared artifacts for each plan. The following table covers those declarations and the implementation evidence checked manually.

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `src/cart.c` | Exact profile admission and CPU/MARIA map | ✓ VERIFIED | Substantive parser, checked allocation, per-instance mapping and DMA read paths; exercised by profile, CPU, bus and diagnostic tests. |
| `tests/diag/souper.s` | Owned executable tracer | ✓ VERIFIED | Assembled into the generated fixture through the registered diagnostic path. |
| `tests/diag/expected.csv` | Independent literal expectations | ✓ VERIFIED | 26 static mapper values are read by the diagnostic driver and judged before route comparisons. |
| `tests/emscripten/souper.cmake` | Production portable consumer test | ✓ VERIFIED | Registered as `emscripten.souper`; invokes the Node host with the generated Wasm module, fixture and source-qualified native receipt. |
| `tests/bus/test_bus.c` | HALT-sensitive MARIA transition | ✓ VERIFIED | `bus.souper.halt_views` passes and checks the reader-switch transition. |
| `libretro/libretro.c` | Stable-only memory-map publication | ✓ VERIFIED | Actual loaded-core test checks fixed CPU and EXRAM descriptors before and after register changes; dynamic windows remain holes. |
| `src/state.c` | Additive complete Souper visitor and transactional state validation | ✓ VERIFIED | Includes all mapper latches and 32 KiB EXRAM; semantic validation precedes mutation. |
| `tests/core/test_state.c` | Continuation and invalid-state checks | ✓ VERIFIED | `core.state.souper` passes value and continuation assertions, including semantic-fault controls. |
| `tests/core/state_compat.cmake` | Source-qualified prior-state compatibility | ✓ VERIFIED | Re-executed with current source and genuine pre-Souper build; result receipt records current commit and exit 0. |
| `docs/hardware/cartridge.md` | Supported profile and evidence limits | ✓ VERIFIED | Public guide specifies header precedence, exact geometry, map, reset/state policy, unsupported audio/payload forms and software-model confidence. |

## Key Link Verification

The query matcher did not recognize C and CMake data-flow links as direct source references. I traced each declared connection in the implementation and test registrations; each is wired.

| From | To | Via | Status | Details |
|---|---|---|---|---|
| `src/cart.c` | `src/internal.h` | Checked allocation and per-instance mapper state | WIRED | Cart operations use the instance latches and ROM/RAM spans; no global Souper state. |
| `tests/diag/driver.c` | `tests/diag/expected.csv` | Literal mailbox/pixel oracle | WIRED | Driver opens the supplied CSV and judges numeric and pixel observations before comparing consumers. |
| `tests/libretro/observe.c` | `tests/emscripten/host.mjs` | Same fixture and source-qualified result | WIRED | Native observer writes schema-4 receipt; portable host consumes it and executes the production Wasm module. |
| `src/cart.c` | `src/bus.c` | Tick-aware DMA reader identity | WIRED | `muhtari__cart_dma_read` asks the bus which reader is selected at the DMA tick. |
| `tests/diag/souper.s` | `tests/diag/driver.c` | Real CPU/MARIA stimuli | WIRED | CMake assembles the owned template, creates the generated image, and passes it to the driver/runner tests. |
| `libretro/libretro.c` | `tests/libretro/host.c` | Loaded-core memory descriptors and generated Souper continuation | PARTIAL | The loaded host checks descriptors across mapper changes using a separate ROM program; it does not load the generated Souper diagnostic or test loaded-module save/restore. |
| `src/state.c` | `src/internal.h` | Serialized mutable latches and EXRAM | WIRED | Visitor fields correspond to instance-owned latches and the full RAM span; topology validation checks size. |
| `src/instance.c` | `src/state.c` | Topology-specific format version | WIRED | Version selection follows the loaded cartridge topology: legacy 1/6, YM 2/7, Souper 3/8. |
| `tests/core/test_state.c` | `tests/diag/driver.c` | Restore and independent mapping continuation | WIRED | Both paths load state then judge subsequent CPU/MARIA values; state tests retain their own expected values, and the diagnostic uses the separate literal CSV oracle. |
| `tests/libretro/observe.c` | `tests/emscripten/host.mjs` | Native and portable continuation reports | WIRED | Both consumers replay their own restored state; hashes are source/consumer-qualified, not incorrectly assumed equal across separate initialization. |

## Data-Flow Trace (Level 4)

| Artifact | Data variable | Source | Produces real data | Status |
|---|---|---|---|---|
| Generated Souper image | ROM bytes and mapped EXRAM | Owned assembly template plus deterministic pattern generator | Yes; the loaded image drives actual CPU/MARIA accesses. | FLOWING |
| `src/cart.c` | CPU/MARIA read values | Per-instance ROM and EXRAM allocated during successful image install | Yes; the expected values are separately authored in the checked-in CSV. | FLOWING |
| `src/state.c` | Serialized latch and EXRAM bytes | Current instance state | Yes; load validates first, then restores into the existing allocation. | FLOWING |
| `tests/libretro/observe.c` → `host.mjs` | Native receipt and portable frame/state observations | Built observer output, production `.mjs` and `.wasm`, generated fixture | Yes; source identity and fixture digest are checked before replay. | FLOWING |

No rendered/dynamic value ends in a static return, mock, or hardcoded empty prop. The CSV is an independent expected oracle, not input data used to populate the implementation.

## Behavioral Spot-Checks

| Behavior | Command | Result | Status |
|---|---|---|---|
| Admission and no allocation on rejected input | `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^cart[.]souper[.]profile$'` and `... -R '^cart[.]load[.]no_alloc_on_reject$'` | Both named tests passed; the no-allocation check passed 1/1. | PASS |
| CPU mapping and lifecycle | `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^core[.]souper[.]cpu$'` and `... -R '^core[.]souper[.]lifecycle$'` | Both passed 1/1. | PASS |
| Transactional Souper state | `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^core[.]state[.]souper$'` | Passed 1/1. | PASS |
| MARIA/HALT transition | `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^bus[.]souper[.]halt_views$'` | Passed 1/1. | PASS |
| Public library and headless runner diagnostic | `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^diag[.]souper_tracer$'` | Passed; real CPU/mailbox and MARIA/pixel expectations were evaluated. | PASS |
| Static observer and loaded libretro module | `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^libretro[.]observe[.]souper$'` and `... -R '^libretro[.]host[.]souper$'` | The observer exercises the generated diagnostic but compiles `libretro.c` into the test executable. The loaded module test exercises stable descriptors with a separate program; it does not exercise the generated diagnostic/state continuation through that module. | PARTIAL |
| Prior state compatibility | `cmake -DROOT="$PWD" -DOLD_SOURCE="$PWD/.worktrees/ym-state-baseline" -DOLD_BUILD="$PWD/build/v6-state-baseline" -DPRE_SOURCE="$PWD/.worktrees/22-03-pre-souper" -DPRE_BUILD="$PWD/build/22-03-pre-souper" -DNEW_BUILD="$PWD/build/ci" -DWORK="$PWD/build/22-verify-state-compat" -DMODE=controls -P tests/core/state_compat.cmake` | Exit 0; current `a2efefb` is recorded in `result.json`; all legacy/YM matrices and controls passed. | PASS |
| Production portable Wasm consumer | `node tests/emscripten/host.mjs --module build/emscripten/libretro/muhtari_libretro.mjs --fixture build/ci/tests/diag/souper/souper.a78 --native build/ci/tests/emscripten/souper-native.json --mode verify` | Passed: 12 native-parity frames, mailbox and format-3 state. | PASS |
| Portable CTest wrapper on host | `ctest --test-dir build/emscripten --output-on-failure --no-tests=error -R '^emscripten[.]souper$'` | Not Run: this build tree was configured in the pinned container and refers to `/src/.../cmake`, which is unavailable on the host. The same registered production host script and generated artifacts passed directly above. | SKIP (environment path) |
| Hygiene | `sh scripts/hygiene.sh` | Exit 0 on the verified tree. | PASS |

The final ASan `LastTest.log` is a direct recorded test result: all nine selected Souper checks and required fixtures passed (including profile, CPU, lifecycle, state, HALT, tracer and loaded libretro). The local quality receipt `build/ci/quality-souper-plan-final.json` is tied to source `13d4982` and records 721 native tests, 19 ABI tests, zero ABI findings, zero warnings and a 368,271-byte archive. Source files are unchanged since that reviewed source; later differences are planning records. Portable full-lane logs also record successful artifact/control CTests and a 14-frame offline libretro replay. These values characterize the software model and repository resource contract; they do not imply physical-cartridge accuracy.

## Probe Execution

Not applicable. No phase plan declares a shell probe, no probe markers occur in the plans/summaries, and this is not a migration/tooling phase.

## Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| SOUPER-01 | 22-01, 22-03 | Exact declarations/geometry and refusal before allocation or mutation | SATISFIED | Profile and no-allocation tests passed; parser/install trace and docs agree. |
| SOUPER-02 | 22-01, 22-02, 22-03 | CPU bank, aliases, EXRAM modes and boundaries | SATISFIED | CPU test and generated tracer passed against independent literal rows. |
| SOUPER-03 | 22-02, 22-03 | Separate MARIA character/fetch mapping and HALT transition | SATISFIED | Bus transition and diagnostic tests passed. |
| SOUPER-04 | 22-01, 22-03 | Lifecycle, instance ownership, complete snapshots and transactional refusal | PARTIAL | Core and portable continuation pass; loaded native module continuation has not been exercised with the generated Souper diagnostic. |
| SOUPER-05 | 22-01, 22-02, 22-03 | Owned pattern, independent literal expectations and malformed/non-Souper controls | SATISFIED | Owned fixture is admitted by the manifest; literal diagnostic and controls are wired and passed. |
| SOUPER-06 | 22-01, 22-02, 22-03 | Public C, runner, loaded libretro, portable consumer; no BupChip audio | PARTIAL | Public diagnostic, separate runner, static observer and Wasm consumer pass; the generated diagnostic is missing from the dynamically loaded native module path. |
| SOUPER-07 | 22-01, 22-02, 22-03 | Integrator profile, reset/state policy, unsupported forms and evidence limits | SATISFIED | Public cartridge, runner, libretro, testing and Emscripten docs reviewed against implementation. |

**Orphaned requirements:** None. REQUIREMENTS.md assigns exactly SOUPER-01 through SOUPER-07 to Phase 22, and all appear in the plans.

## Test Quality Audit

- The expected mapping bytes are literal checked-in rows; test setup generates fixture and report outputs but does not calculate expected values from the production mapper or learn them from a successful run.
- Numeric CPU assertions, pixel/MARIA assertions, state continuation, state refusal and HALT transition are value/behavior-level checks. Consumer hashes are integration receipts after independent judgments, not the only oracle.
- The tests load the public library, run `muhtari-run`, compile the libretro observer directly, load the native module for descriptor checks, and execute the production Wasm module. The generated Souper diagnostic is not yet connected to loaded native-module save/restore; a mock emulator is not involved.
- Test registration is present in the core, cart, bus, diagnostic, libretro and portable CMake files and CI/portable inventories. No phase-added disabled tests, skipped placeholders, or circular expected-value paths were found.
- Test-generated files are fixture builds, run reports, state images and explicit negative-control inputs; the immutable expected mapping table remains checked in.

## Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| — | — | None found in phase-changed implementation, tests or user docs. No untracked TBD/FIXME/XXX markers, placeholder implementations or empty user-visible data paths. | — | — |

## Human Verification Required

None. The acceptance claims are deterministic core, state, loaded-module and offline portable behavior with literal automated oracles. The documentation explicitly bounds the result to the implemented software model; no physical or RetroArch evidence is claimed or required for this phase.

## Decision Coverage

`22-CONTEXT.md` decision coverage is 9/9 honored. No decision override was applied.

## Integration Gap Summary

The milestone integration checker traced the code and CTest registration and found one partial native libretro path. `libretro.host.souper` dynamically loads the production module, but its descriptor case uses a separate inline ROM program; `libretro.observe.souper` consumes the generated Souper image through an observer that compiles `libretro.c` into its test executable. The loaded module is not yet tested with the generated Souper diagnostic or with save/restore continuation.

This leaves the loaded-native portion of the Phase 22 Plan 22-03 acceptance unmet and SOUPER-04/SOUPER-06 partial. Close it with a focused loaded-module test that loads the generated fixture, judges the literal CPU/MARIA observations, serializes at a fixed diagnostic boundary, restores, and compares the continued observations; retain malformed-state refusal coverage. No product defect has been demonstrated. The host-side CTest wrapper for the container-built Emscripten tree remains unavailable at its container-only `/src` path, while the actual production Node/Wasm consumer passed directly.

---

_Verified: 2026-11-14T07:06:40Z_
_Verifier: the agent (gsd-verifier)_
