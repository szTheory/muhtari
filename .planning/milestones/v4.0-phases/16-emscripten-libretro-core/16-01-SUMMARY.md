---
phase: 16-emscripten-libretro-core
plan: 01
subsystem: testing
tags:
  - libretro
  - emscripten
  - node
requires: []
provides:
  - Pinned Emscripten production loader and companion Wasm output
  - Native numeric observer and direct offline Node consumer
  - Required Emscripten CTest inventory, Linux CI gate and local reproduction command
affects:
  - libretro
  - ci
  - emscripten
actuals:
  tasks: 3
  commits: 2
plan_head_before: 969784c
plan_head_after: 110af6e
tech-stack:
  added:
    - Emscripten SDK 7.0.0 toolchain lane
    - Node.js built-in modules for the offline consumer
  patterns:
    - Native numeric checks precede cross-artifact hash comparison
    - Build-owned source identity ties native and WebAssembly receipts together
key-files:
  created:
    - tests/libretro/observe.c
    - tests/emscripten/host.mjs
    - tests/emscripten/tests.cmake
    - tests/inventory/emscripten.txt
    - scripts/emscripten.sh
  modified:
    - libretro/CMakeLists.txt
    - tests/libretro/tests.cmake
    - tests/inventory/ci.txt
    - tests/tests.cmake
    - CMakePresets.json
    - .github/workflows/ci.yml
    - docs/libretro.md
key-decisions:
  - Use the pinned Linux amd64 SDK leaf with Unix Makefiles and the existing verified CMake 4.4.3 pin.
  - Keep native and WASI outputs unchanged; Emscripten-only exports and tests are target-scoped.
  - Return false for unsupported frontend environment commands in the offline consumer.
requirements-completed:
  - WASM-01
  - WASM-02
  - WASM-03
  - WASM-04
coverage:
  - id: WASM-01
    description: The exact pinned SDK emits a no-entry modular ES loader and companion Wasm with a recorded import/export surface.
    requirement: WASM-01
    verification:
      kind: integration
      ref: sh scripts/emscripten.sh build/ci; exit 0; local run URL N/A
      status: pass
    human_judgment: false
  - id: WASM-02
    description: Direct Node loading exercises the production callbacks, fixture lifecycle, state, no-game behavior and isolated instances.
    requirement: WASM-02
    verification:
      kind: integration
      ref: build/emscripten/junit.xml; emscripten.artifact ran once and passed
      status: pass
    human_judgment: false
  - id: WASM-03
    description: Independent numeric oracles, malformed-input/state refusal, replay and named observation controls pass.
    requirement: WASM-03
    verification:
      kind: integration
      ref: cmake --workflow --preset ci and sh scripts/emscripten.sh build/ci; both exit 0
      status: pass
    human_judgment: false
  - id: WASM-04
    description: Same-workload artifact sizes and native/Node throughput are recorded with environment limits.
    requirement: WASM-04
    verification:
      kind: other
      ref: build/ci/tests/emscripten/measurement.json; local run URL N/A
      status: pass
    human_judgment: false
duration: 69min
completed: 2026-11-10
status: complete
---

# Phase 16 Plan 01: Emscripten libretro core summary

The production adapter now builds as a no-entry `.mjs` factory plus `.wasm`, with a required offline Node consumer and an independently checked native observation.

## Accomplishments

- Added the Emscripten-only production link contract, explicit libretro/memory exports, callback table helpers, fresh heap views and a build-owned C-source identity. Native MODULE and WASI configurations remain separate.
- Added a bounded native adapter/public-library observer. It checks 320x242 output, 48 kHz audio, exact per-frame audio counts, SWCHA 255/239/254, malformed state/content behavior and deterministic continuation before emitting hashes.
- Added a direct Node consumer for the emitted factory. It verifies all five finite scenarios, state refusal/preservation, two continuation replays, no-game behavior and interleaved instance isolation. Named faults reject missing exports/callbacks, changed hashes and incomplete scenarios.
- Added an exact two-test Emscripten inventory, local pinned-container entry point, a required Linux x64 CI step and reproduction documentation.
- The final build-owned measurement reports a 57,902-byte loader, 191,040-byte Wasm and 248,942 combined bytes. Its three-window medians were 798.945392 native frames/s and 996.2247233919802 Node frames/s. These are descriptive only: native used AppleClang on macOS arm64 while Node ran Linux x64 through Docker Desktop amd64 emulation; they do not predict browser or device performance.

## Verification

| Check | Result |
|---|---|
| `cmake --workflow --preset ci` | Exit 0; native suite, packaging and release tests passed |
| `sh scripts/emscripten.sh --preflight build/ci` | Exit 0 |
| `sh scripts/emscripten.sh build/ci` | Exit 0; two Emscripten tests ran once, JUnit validated, report and Wasm surface produced |
| Native and release JUnit inventory checks | Exit 0; every selected case ran once |
| `cmake -DROOT=. -P tests/cmake/budgets.cmake` | Exit 0; 22 documentation files within budget |
| `sh scripts/hygiene.sh --tree` | Exit 0 |
| `git diff --check b2c6234a6cb8a74a2dd99edc2eb60c3b9760b540 --` | Exit 0 |
| Missing Node, Wasm and native-evidence controls | Each produced its expected attributed failure; local receipts are under ignored `build/ci/tests/emscripten/` |

Local runs have no remote run URL. The arm64 image pull failed because Docker could not overwrite the already cached index digest; that diagnostic was preserved. The researched amd64 leaf was selected and explicitly run under arm64-host emulation. The initial invalid `-sNO_ENTRY=1` setting was replaced with the SDK-supported `-Wl,--no-entry` after the compiler named the setting error. Runtime diagnostics also required exporting `HEAPU8` and `HEAPU32` for callback copies and environment replies.

## Plan commits

Parent reconciliation recorded two cohesive implementation commits after all fixes and checks:

- `103529c` — production Emscripten core, native observer, direct Node consumer and controls (Tasks 1–2).
- `110af6e` — pinned reproduction, presets, required Linux integration and guide (Task 3).

The test-first missing-artifact receipt predates implementation and is retained build-owned; no separate RED commit is claimed. The executor sandbox prevented Git writes. All task commits are reachable from the canonical milestone branch.

## Self-Check: PASSED

The loader, Wasm, native observer, Node consumer, CTest inventory, script, workflow, docs and this summary are present. The native and Emscripten test inventories ran without skips.

## Review closeout

The first independent source review found five issues. One fix round resolved all five; the original reviewer reassessed the changes and reports zero current findings. The complete native workflow and pinned Emscripten lane passed after the fixes. See [fix evidence](16-FIX-REPORT.md) and [review disposition](16-REVIEW-DISPOSITION.md). Windows replacement follows the existing platform pattern; Windows execution remains for hosted CI at ship.

## Next Phase Readiness

Implementation and local acceptance are ready for the parent-owned source review, verifier and Git/state bookkeeping. Measurement JSON, raw logs and toolchain receipts remain ignored under `build/`.

---
*Phase: 16-emscripten-libretro-core*
*Completed: 2026-11-10*

## Ship gate correction

Hosted run 42903652231 exposed an observer-only mono/stereo PCM bug that silent local fixtures had masked: the old byte comparison read twice the public library's mono sample count. The observer now compares both adapter channels per native sample and hashes the duplicated mono stream without reading unused buffer space. A non-silent signed/extreme-sample control reproduces the old bug; changed left/right channels remain rejected. Shipping also formats the observer and inherited trace helper with pinned clang-format and admits the exact owned frontend-state fixture in the existing quality policy with pass/fault controls. See [shipping evidence](../../v4.0-SHIP.md). This changes no production core behavior or earlier host acceptance.
