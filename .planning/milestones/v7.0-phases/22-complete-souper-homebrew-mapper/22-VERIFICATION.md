---
phase: 22-complete-souper-homebrew-mapper
verified: 2026-11-14T15:53:15Z
status: passed
score: "8/8 must-haves verified"
covered_files:
  - .planning/milestones/v7.0-phases/22-complete-souper-homebrew-mapper/22-01-PLAN.md
  - .planning/milestones/v7.0-phases/22-complete-souper-homebrew-mapper/22-01-SUMMARY.md
  - .planning/milestones/v7.0-phases/22-complete-souper-homebrew-mapper/22-02-PLAN.md
  - .planning/milestones/v7.0-phases/22-complete-souper-homebrew-mapper/22-02-SUMMARY.md
  - .planning/milestones/v7.0-phases/22-complete-souper-homebrew-mapper/22-03-PLAN.md
  - .planning/milestones/v7.0-phases/22-complete-souper-homebrew-mapper/22-03-SUMMARY.md
  - .planning/milestones/v7.0-phases/22-complete-souper-homebrew-mapper/22-04-PLAN.md
  - .planning/milestones/v7.0-phases/22-complete-souper-homebrew-mapper/22-04-SUMMARY.md
  - tests/inventory/ci.txt
  - tests/libretro/host.c
  - tests/libretro/tests.cmake
covered_digest: "v4:sha256:9a1981453aaa991baa1a006145476cb18d54b10f573c047f2338b9c3e0984c0f"
behavior_unverified: 0
overrides_applied: 0
re_verification:
  previous_status: gaps_found
  previous_score: "7/8 must-haves verified; 1 partial"
  gaps_closed:
    - "The generated Souper diagnostic and restored continuation run through the dynamically loaded native libretro module."
  gaps_remaining: []
  regressions: []
decision_coverage:
  honored: 9
  total: 9
  not_honored: []
---

# Phase 22: Complete Souper homebrew mapper Verification Report

**Phase Goal:** A homebrew developer can load the documented Souper mapper-only profile and obtain deterministic CPU/MARIA mapping and snapshot continuation through every existing deliverable and the applicable portable consumer.
**Verified:** 2026-11-14T15:53:15Z
**Status:** passed
**Re-verification:** Archive relocation refresh after gap closure. The prior verdict is retained; covered paths and fingerprint now identify the archived v7.0 sources. No product tests were rerun for this documentation-only refresh.

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | The exact A78 v3/v4 Souper declaration and 512 KiB ROM / 32 KiB EXRAM profile is admitted; malformed, oversized, option-bearing and unsupported-audio forms are rejected before allocation or changing loaded content. | ✓ VERIFIED | `src/cart.c` validates the selected v4/v3 declaration and exact body geometry before transactional install. `cart.souper.profile` and `cart.load.no_alloc_on_reject` passed; the latter asserts the reject path does not allocate. Docs name the software profile and limits. |
| 2 | Real CPU reads and writes expose banked ROM, register aliases and flat/segmented EXRAM at literal boundary values independently authored from the mapper. | ✓ VERIFIED | `core.souper.cpu` and `diag.souper_tracer` passed. `tests/diag/expected.csv` supplies 26 literal `souper_map` rows; the driver consumes them as expectations rather than deriving them from mapper helpers. |
| 3 | MARIA fetches use a distinct tick-aware cartridge view, with character remapping and the trap changing at the existing bus/HALT reader boundary. | ✓ VERIFIED | `bus.souper.halt_views` and `diag.souper_tracer` passed. `src/cart.c` keeps CPU and DMA reads separate; DMA selection is driven by `src/bus.c` at the requested tick. The diagnostic compares mailbox and pixel values to literal expectations. |
| 4 | Reset, unload/reload and separate instances initialize and own mapper latches and EXRAM deterministically. | ✓ VERIFIED | `core.souper.lifecycle` passed. `cart_reset` clears the mapper latches and the full instance-owned RAM span; cart install checks and allocates the complete ROM+EXRAM extent before replacing live content. |
| 5 | Souper format-3 / behavior-8 state restores every latch and EXRAM byte into existing storage; malformed state is rejected before mutation or allocation. | ✓ VERIFIED | `core.state.souper` passed. `src/state.c` visits all Souper latches and the full EXRAM span, validates envelope, topology and payload before apply, and uses existing allocation. The focused test covers continuation and repaired-integrity semantic faults. |
| 6 | Existing legacy format-1 / behavior-6 and YM format-2 / behavior-7 state bytes and continuation remain compatible in both directions; API 8 / ABI 1 remain unchanged. | ✓ VERIFIED | The source-qualified `tests/core/state_compat.cmake` receipt records exit 0 against production source `a2efefb`: 10 pre-Souper legacy cases, 2 pre-Souper YM cases, 10 historical cases, both producer/consumer directions, and semantic/content refusal controls passed. Phase 22-04 and the subsequent review/report changes are test/planning-only; the production files are unchanged. `src/instance.c` selects version 3/8 only for the Souper topology. |
| 7 | The same generated diagnostic works through the public C library, headless runner, loaded libretro module and production portable module, and `$8007` adds no BupChip audio behavior. | ✓ VERIFIED | `libretro.host.souper_trace` loads the actual built module and generated fixture, passes literal mailbox and MARIA row-17 checks, restores the six-frame format-3/behavior-8 snapshot and repeats frames 7–12 with identical observations and module state. Null, short, long and corrupt snapshots preserve state; mailbox and pixel fault controls fail their judges. Existing public-library, runner, static-observer and production Wasm checks pass; `$8007` audio controls pass. |
| 8 | Integrator docs describe the exact profile, reset/EXRAM and snapshot policy, unsupported forms, and software-model evidence limits without claiming physical-cartridge accuracy. | ✓ VERIFIED | `docs/hardware/cartridge.md`, `docs/runner.md`, `docs/libretro.md`, `docs/testing.md` and `docs/emscripten-hosting.md` agree on the profile, state versions and host boundaries. The diagnostic fixture is generated and listed as owned; no copyrighted image is used. The phase review in `22-REVIEW.md` is clean. |

**Score:** 8/8 truths verified.

All five roadmap success criteria pass. The former loaded-native gap is closed by the runtime-loaded CTest path, including literal CPU/MARIA judgments, midpoint restore and refusal-preservation checks. All seven SOUPER requirements are satisfied.

## Deferred Items

None. The milestone roadmap contains only Phases 21 and 22; no later phase covers a Phase 22 gap.

## Required Artifacts

The canonical artifact query passed 4/4 declared artifacts for each plan. The following table covers those declarations and the implementation evidence checked manually.

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `src/cart.c` | Exact profile admission and CPU/MARIA map | ✓ VERIFIED | Substantive parser, checked allocation, per-instance mapping and DMA read paths; exercised by profile, CPU, bus and diagnostic tests. |
| `tests/diag/souper.s` | Owned executable tracer | ✓ VERIFIED | Assembled into the generated fixture through the registered diagnostic path. |
| `tests/diag/expected.csv` | Independent literal expectations | ✓ VERIFIED | 26 static mapper values are read by the diagnostic driver and judged before route comparisons. |
| `tests/libretro/host.c` | Runtime-loaded Souper diagnostic and restored continuation | ✓ VERIFIED | `libretro.host.souper_trace` resolves the production module's public entry points and checks literal mailbox/pixel values, restored frames, final state and refusal controls. |
| `tests/libretro/tests.cmake` | Generated fixture and loaded-module test registration | ✓ VERIFIED | `souper_trace` receives the built module and generated image, with the existing `souper_pattern` fixture ordering generation first. |
| `tests/inventory/ci.txt` | Ordinary native CI membership | ✓ VERIFIED | The exact loaded trace test is inventoried; the existing descriptor case remains registered. |
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
| `tests/diag/tests.cmake` | `tests/libretro/tests.cmake` | `souper_pattern` fixture setup before loaded test | WIRED | CTest fixture setup generates the image before `libretro.host.souper_trace`; the focused CTest run confirms the fixture dependency executes. |
| `tests/libretro/tests.cmake` | `tests/libretro/host.c` | Built module and generated image passed to `souper_trace` | WIRED | The registration supplies `$<TARGET_FILE:muhtari_libretro>` and the generated `.a78` path to the host case. |
| `tests/libretro/host.c` | `libretro/libretro.c` | Runtime-resolved public `retro_*` functions | WIRED | Host opens the built module, resolves entry points, and exercises load/run/memory/serialize/unserialize calls on the production module. |
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
| `tests/libretro/host.c` | Loaded module callbacks, mailbox, pixels and serialized state | Runtime-loaded module and generated `.a78` fixture | Yes; callback geometry, literal CPU/MARIA values and restored module-owned state are judged. | FLOWING |

No rendered/dynamic value ends in a static return, mock, or hardcoded empty prop. The CSV is an independent expected oracle, not input data used to populate the implementation.

## Behavioral Spot-Checks

| Behavior | Command | Result | Status |
|---|---|---|---|
| Admission and no allocation on rejected input | `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^cart[.]souper[.]profile$'` and `... -R '^cart[.]load[.]no_alloc_on_reject$'` | Both named tests passed; the no-allocation check passed 1/1. | PASS |
| CPU mapping and lifecycle | `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^core[.]souper[.]cpu$'` and `... -R '^core[.]souper[.]lifecycle$'` | Both passed 1/1. | PASS |
| Transactional Souper state | `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^core[.]state[.]souper$'` | Passed 1/1. | PASS |
| MARIA/HALT transition | `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^bus[.]souper[.]halt_views$'` | Passed 1/1. | PASS |
| Public library and headless runner diagnostic | `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^diag[.]souper_tracer$'` | Passed; real CPU/mailbox and MARIA/pixel expectations were evaluated. | PASS |
| Static observer and loaded libretro module | `ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^libretro[.]observe[.]souper$'` and `... -R '^libretro[.]host[.]souper(_trace)?$'` | The static observer and both runtime-loaded cases pass. The new `souper_trace` consumes the generated fixture through the built module, judges literal mailbox/pixel output, restores the midpoint snapshot, and exercises malformed-state preservation and oracle fault controls. | PASS |
| Focused loaded-module fixture and trace | `ctest --preset ci --output-on-failure --no-tests=error -R '^libretro[.]host[.]souper(_trace)?$'` | Exit 0; 5 tests passed, including the generated-fixture setup, new trace and existing descriptor case. Independently rerun during this verification. | PASS |
| Full CI workflow on final phase source | `cmake --workflow --preset ci` | Exit 0; all 722 ordinary tests and 8 release tests passed. Local only; no hosted run or URL applies. | PASS |
| Prior state compatibility | `cmake -DROOT="$PWD" -DOLD_SOURCE="$PWD/.worktrees/ym-state-baseline" -DOLD_BUILD="$PWD/build/v6-state-baseline" -DPRE_SOURCE="$PWD/.worktrees/22-03-pre-souper" -DPRE_BUILD="$PWD/build/22-03-pre-souper" -DNEW_BUILD="$PWD/build/ci" -DWORK="$PWD/build/22-verify-state-compat" -DMODE=controls -P tests/core/state_compat.cmake` | Exit 0; current `a2efefb` is recorded in `result.json`; all legacy/YM matrices and controls passed. | PASS |
| Production portable Wasm consumer | `node tests/emscripten/host.mjs --module build/emscripten/libretro/muhtari_libretro.mjs --fixture build/ci/tests/diag/souper/souper.a78 --native build/ci/tests/emscripten/souper-native.json --mode verify` | Passed: 12 native-parity frames, mailbox and format-3 state. | PASS |
| Portable CTest wrapper on host | `ctest --test-dir build/emscripten --output-on-failure --no-tests=error -R '^emscripten[.]souper$'` | Not Run: this build tree was configured in the pinned container and refers to `/src/.../cmake`, which is unavailable on the host. The same registered production host script and generated artifacts passed directly above. | SKIP (environment path) |
| Hygiene | `sh scripts/hygiene.sh` | Exit 0 on the verified tree. | PASS |

Focused Release and ASan CTest runs for `libretro.host.souper(_trace)?` passed with their fixture setup. The full final-source CI workflow passed all 722 ordinary tests and 8 release tests. `sh scripts/hygiene.sh` also exited 0; no test logs were committed. Earlier source-qualified evidence remains applicable to the unchanged production implementation and the static/portable consumers; Phase 22 plan 22-04 adds test-only changes. The portable CTest wrapper remains unavailable from this host because its configured build points to the pinned container's `/src/.../cmake`; the production Node/Wasm consumer itself passed directly. These checks characterize the implemented software model and do not imply physical-cartridge accuracy.

## Probe Execution

Not applicable. No phase plan declares a shell probe, no probe markers occur in the plans/summaries, and this is not a migration/tooling phase.

## Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| SOUPER-01 | 22-01, 22-03 | Exact declarations/geometry and refusal before allocation or mutation | SATISFIED | Profile and no-allocation tests passed; parser/install trace and docs agree. |
| SOUPER-02 | 22-01, 22-02, 22-03 | CPU bank, aliases, EXRAM modes and boundaries | SATISFIED | CPU test and generated tracer passed against independent literal rows. |
| SOUPER-03 | 22-02, 22-03 | Separate MARIA character/fetch mapping and HALT transition | SATISFIED | Bus transition and diagnostic tests passed. |
| SOUPER-04 | 22-01, 22-03, 22-04 | Lifecycle, instance ownership, complete snapshots and transactional refusal | SATISFIED | Core and portable continuation pass. The runtime-loaded module restores the generated diagnostic's midpoint state and repeats frames 7–12; null, short, long and corrupt states preserve the midpoint. |
| SOUPER-05 | 22-01, 22-02, 22-03 | Owned pattern, independent literal expectations and malformed/non-Souper controls | SATISFIED | Owned fixture is admitted by the manifest; literal diagnostic and controls are wired and passed. |
| SOUPER-06 | 22-01, 22-02, 22-03, 22-04 | Public C, runner, loaded libretro, portable consumer; no BupChip audio | SATISFIED | Public diagnostic, runner, static observer, runtime-loaded native module and production Wasm consumer pass. `$8007` audio controls continue to pass. |
| SOUPER-07 | 22-01, 22-02, 22-03 | Integrator profile, reset/state policy, unsupported forms and evidence limits | SATISFIED | Public cartridge, runner, libretro, testing and Emscripten docs reviewed against implementation. |

**Orphaned requirements:** None. REQUIREMENTS.md assigns exactly SOUPER-01 through SOUPER-07 to Phase 22, and all appear in the plans.

## Test Quality Audit

- The expected mapping bytes are literal checked-in rows; test setup generates fixture and report outputs but does not calculate expected values from the production mapper or learn them from a successful run.
- Numeric CPU assertions, pixel/MARIA assertions, state continuation, state refusal and HALT transition are value/behavior-level checks. Consumer hashes are integration receipts after independent judgments, not the only oracle.
- The tests load the public library, run `muhtari-run`, compile the libretro observer directly, load the native module for both descriptor and generated-diagnostic/state checks, and execute the production Wasm module. The generated loaded-module route uses the module's runtime-resolved public entry points; a mock emulator is not involved.
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

## Gaps Summary

No Phase 22 gaps remain. The former loaded-native acceptance gap is closed: the generated diagnostic is loaded by the built module through runtime-resolved public entry points, literal CPU/MARIA judgments pass, restored continuation matches uninterrupted observations and module state, and malformed states preserve the saved state. The existing loaded descriptor case remains registered and passing.

The Emscripten CTest wrapper cannot run from this host because the configured build refers to a container-only `/src/.../cmake` path; direct execution of the production Node/Wasm consumer passed. This is an environment limitation, not a Phase 22 acceptance gap. Phase 21's unresolved probe debt remains attached to Phase 21 and is neither claimed as fixed nor carried as a Phase 22 gap. No physical hardware, RetroArch host session, hosted CI dispatch, publication, or merge was performed or claimed; these boundaries are outside this local Phase 22 verification.

---

_Verified: 2026-11-14T08:00:42Z_
_Verifier: the agent (gsd-verifier)_
