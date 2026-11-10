# Phase 16 Harness Research

**Researched:** 2026-11-10  
**Scope:** Offline Node host, fixture/parity controls, and size/throughput reporting  
**Confidence:** MEDIUM; repository test findings HIGH, Emscripten interop docs LOW due web-search fallback.

## Summary

Add one small Node consumer that loads the emitted Emscripten artifact itself and calls its exported libretro lifecycle. Reuse the native host tests as behavioral reference, but do not treat their `dlopen` path as artifact evidence: they explicitly load a platform module and link `${CMAKE_DL_LIBS}`. [VERIFIED: `tests/libretro/host.c:1-16,58-96`; `tests/libretro/tests.cmake:126-138`]

Use the existing generated `input.a78` fixture, script the same RetroPad presses, capture video/audio, exercise state save/restore/replay, and assert independent numeric values before hashes. Put this in a dedicated Emscripten CMake lane and required Linux job, with an exact inventory and hard failures for missing compiler, Node, exports, artifact, or fixture. The WASI preset is not suitable: it disables libretro. [VERIFIED: `tests/input/tests.cmake:18-59`; `tests/libretro/tests.cmake:161-170`; `CMakePresets.json:105-116`]

**Primary recommendation:** One direct Node-to-artifact host and a dedicated required CTest lane; reuse existing fixture and narrow input/state/callback oracles. No npm dependency, browser UI, or broad port of the existing test simulator. [RECOMMENDATION based on sources below]

<phase_requirements>
## Phase Requirements

| ID | Research support |
|----|------------------|
| WASM-01 | Test the selected artifact’s actual loader form and required exports. |
| WASM-02 | Existing generated input fixture and lifecycle/callback cases provide a narrow consumer script. |
| WASM-03 | Existing numeric input/audio/video/state observations give independent checks before parity hashes. |
| WASM-04 | Report artifact byte sizes and warmed same-fixture host/core rates without a pass/fail speed floor. |
</phase_requirements>

## Project Constraints (from AGENTS.md)

- “Library, runner, libretro core. No window, audio-device or input-device code.” [VERIFIED: `AGENTS.md:53`]
- “No ROM, BIOS or save bytes except fixtures listed in `tests/roms/manifest.txt`.” [VERIFIED: `AGENTS.md:54-56`]
- “same inputs give the same frame and audio hashes on every platform” and “Every call returns within a bounded amount of emulated time.” [VERIFIED: `AGENTS.md:57-59`]
- “Every behaviour change lands with a test that `cmake --workflow --preset ci` runs.” [VERIFIED: `AGENTS.md:60-62`]
- “A new or materially changed check names its concrete fault and exercises representative pass/fault behavior.” [VERIFIED: `AGENTS.md:75-80`]
- Security enforcement and Nyquist validation are explicitly false in `.planning/config.json`; those sections are omitted. [VERIFIED: `.planning/config.json:20-26`]

## Reusable Fixtures and Oracles

- CTest generates `input.a78` under `${PROJECT_BINARY_DIR}/tests/input-carts`; `input.cart_files` owns its fixture setup and `libretro.host.input` consumes it. The generator writes the requested output file, so no ROM bytes need to be added to the repository. [VERIFIED: `tests/input/tests.cmake:18-19,51-59`; `tests/input/test_input.c:2837-2868`; `tests/libretro/tests.cmake:161-170`]
- Native scripted-input expectations are `0xFF` with no input, `0xEF` after port 1 Up, and `0xFE` after port 2 Up: `CHECK_EQ_HEX(last_entry()[E_SWCHA], 0xFFu);`, `CHECK_EQ_HEX(last_entry()[E_SWCHA], 0xEFu);`, `CHECK_EQ_HEX(last_entry()[E_SWCHA], 0xFEu);`. Assert these numeric observations before parity hashes. [VERIFIED: `tests/libretro/host.c:1163-1178`]
- Reuse the existing callback checks: the source defines `#define W 320u` and `#define H 242u`; NTSC audio accepts only `800u` or `801u` samples/frame; state-frame checks compare every video pixel and exact audio samples to an independent native library instance. [VERIFIED: `tests/libretro/host.c:40-42,716-726,2689-2745`]
- Refusal controls already cover a 16-byte invalid image, null/short/extra state payloads, and preserving state after rejection; restore/replay uses a fixed two-frame input sequence. [VERIFIED: `tests/libretro/host.c:2775-2811,2891-2910,2967-3000`]
- Optional moving-picture fixture: `tests/diag/frontend-states.a78` is already manifest-listed as an owned MIT fixture; its source writes a `MUHT` marker and increments a 16-bit progress count. [VERIFIED: `tests/roms/manifest.txt:35`; `tests/diag/frontend-states.s:1-5,26-37,62-73`]

## Node / CMake Guidance

- Make Node import the selected production loader form and call the artifact’s exported `retro_*` functions. Emscripten documents an async factory with `MODULARIZE`; its module can be CommonJS under Node or an ES module via `.mjs`. Select one form and test it directly. [CITED: [Emscripten Modularized Output](https://emscripten.org/docs/compiling/Modularized-Output.html), checked 2026-11-10; LOW]
- For JavaScript callbacks passed as C function pointers, Emscripten documents `addFunction`, `ALLOW_TABLE_GROWTH`, and explicit Wasm signatures; export runtime methods used by external JS. Confirm these against the exact toolchain pin. [CITED: [Emscripten Interacting with Code](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html), checked 2026-11-10; LOW]
- Add a dedicated test lane that short-circuits normal registration and includes only the fixture generator plus artifact-host test. This mirrors existing special inventories and avoids building the native `dlopen` host for Emscripten. Require Node/toolchain at configure time; never optional-register or self-skip the artifact test. [VERIFIED: `tests/tests.cmake:1-27,48-52`; `.planning/preparation/ENGINEERING.md:109-112`]
- Keep the lane’s inventory exact and validate JUnit: existing policy requires every selected inventory entry exactly once, with no skipped tests. [VERIFIED: `tests/cmake/check_junit.cmake:1-7,32-58`; `tests/cmake/no_skip.cmake:1-7,27-38`]
- The current `wasm32` preset uses the WASI toolchain and contains the exact setting `"MUHTARI_BUILD_LIBRETRO": "OFF"`; it cannot prove the Emscripten libretro loader. [VERIFIED: `CMakePresets.json:105-116`]
- Warm the same fixture, inputs, frame count and callback mode for native and Node/Wasm; time fixed windows with monotonic clocks and report throughput/runtime identity. Report `.wasm`, JS loader and combined uncompressed sizes separately. Treat these as headless measurements only; requirements set no throughput threshold or browser/device acceptance. [VERIFIED: `.planning/REQUIREMENTS.md:21-22,30-36`; measurement details are a recommendation]

## Options

| Option | Assessment |
|---|---|
| No change | Reject: current native host does not load the emitted artifact, and WASI explicitly disables libretro. |
| Minimum slice | Recommend: one direct Node host, generated input fixture, numeric callbacks/input, refusal/state replay, exact CTest inventory and required Linux artifact job. |
| Port the full native host suite to JS | Defer: duplicates many in-process option/device/BIOS cases and exceeds this acceptance boundary. Existing decision says real frontend E2E stays small and semantics are proven in-process. [VERIFIED: `.planning/preparation/DECISIONS.md:95-96`] |

## Assumptions / Verification Left

| Claim | Risk if wrong |
|---|---|
| The selected Emscripten pin supports the documented callback-table settings. | Callback setup fails; verify exact pin docs/build. |
| The generated fixture executable can run under the selected Node cross-compilation emulator. | Add a small host-native fixture generator or use the already-manifested static fixture. |

No build or test was run for this read-only harness research subtask.
