# Phase 16 Toolchain Research

**Researched:** 2026-11-10  
**Question:** Smallest reproducible Emscripten build and loader contract for the existing libretro core, with browser output and an offline Node host.  
**Confidence:** MEDIUM — official Emscripten/CMake references and registry metadata checked; no SDK was installed or build run.

## Recommendation

Produce a modularized ES module and its companion `.wasm`, linked as a no-entry Emscripten executable from the existing libretro adapter and static core. Reuse the generated loader in browser and Node; keep Node's host shim to standard Node APIs and the Emscripten runtime. Do not make the static archive the runtime deliverable: it is only a link-time input. Avoid raw standalone Wasm for this first slice because it shifts callback registration and initialization into a custom loader. Emscripten documents modularized output as an async factory and recommends avoiding dynamic linking in most projects. [CITED: [Modularized Output](https://emscripten.org/docs/compiling/Modularized-Output.html), [Building Projects](https://emscripten.org/docs/compiling/Building-Projects.html), [Dynamic Linking](https://emscripten.org/docs/compiling/Dynamic-Linking.html)]

## Build and link contract

- The current native adapter is a CMake `MODULE`; its implementation links the core and host support: `add_library(${name} MODULE libretro.c ${PROJECT_SOURCE_DIR}/runner/sha256.c)` and `target_link_libraries(${name} PRIVATE muhtari::muhtari muhtari_host)`. [VERIFIED: `libretro/CMakeLists.txt:5-10`]
- The existing `wasm32` preset is the separate WASI/Wasmtime lane and explicitly sets `"MUHTARI_BUILD_LIBRETRO": "OFF"`. Preserve it; add a distinct Emscripten configuration using `emcmake` or Emscripten's CMake toolchain. [VERIFIED: `CMakePresets.json:105-115`]
- For Emscripten, link an executable-shaped loader target with no `main` entry, outputting `.mjs` plus `.wasm`; do not let CMake `MODULE` silently select Emscripten side-module/dynamic-link behavior. The existing `muhtari` archive may remain the static link input. Keep flags target-scoped so native and WASI targets remain unchanged. [CITED: [Emscripten CMake integration](https://emscripten.org/docs/compiling/Building-Projects.html), [CMake toolchain files](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)]
- Explicitly retain the libretro functions the host calls and the allocator functions used to pass fixture/state buffers. Export Emscripten's `addFunction` runtime method and allow table growth: the host supplies JS callbacks as C function pointers, and Emscripten documents both the signature requirement and table-growth requirement. [CITED: [Emscripten JS/C interop](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html)]
- The existing adapter exposes callback setters including `retro_set_environment`, `retro_set_video_refresh`, `retro_set_audio_sample_batch`, `retro_set_input_poll` and `retro_set_input_state`; it prefers the batch audio callback if both audio forms are set. [VERIFIED: `libretro/libretro.c:424-457`]
- The environment callback must answer the commands exercised by initialization and content loading; the source declares no-game support and controller/options metadata there. The JS host should return false for unsupported environment requests rather than claim capabilities it does not implement. [VERIFIED: `libretro/libretro.c:421-434`]
- Copy video/audio callback bytes synchronously before returning to Wasm. Emscripten notes that memory growth replaces the underlying buffer and invalidates old typed-array views; reacquire the module's heap view after any call that may grow memory. [CITED: [Emscripten JS/C interop](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html)]
- In Node, read the `.wasm` and owned fixture from local files and instantiate the generated factory with those bytes; the test path must not use network access. Audit the produced Wasm's real imports and exports as build evidence instead of assuming a toolchain-independent import list. [CITED: [Emscripten Module object](https://emscripten.org/docs/api_reference/module.html), [Node.js WebAssembly example](https://nodejs.org/learn/getting-started/nodejs-with-webassembly)]

## Toolchain pin and integrity

Pin **Emscripten 7.0.0**. The official project marks 7.0.0 as its stable latest release, published 2026-11-06; its emsdk release map resolves it to `6afbd83493d647051772d491124187d4e9a0cda9`. [CITED: [Emscripten 7.0.0 release](https://github.com/emscripten-core/emscripten/releases), [emsdk release map](https://github.com/emscripten-core/emsdk/blob/main/emscripten-releases-tags.json)]

For the one Linux CI execution, use the official `emscripten/emsdk:7.0.0` image by immutable OCI digest, not the tag:

| Platform | Digest | Registry compressed size |
|---|---|---:|
| Multi-platform index | `sha256:6bd7aef93408b10bf7b36642cb7c91119b302f720b9ec2d20439f73cf07fe894` | — |
| Linux arm64 | `sha256:aad444c42262d98a6fd4df8020254a1e44a87a2929e83abf1561d5be16cdee93` | 799.61 MB |
| Linux amd64 | `sha256:3803e37bf748ddee14d31cf712fcab09ca367fca83bbefeb78ce4c1f284e6f3f` | 825.81 MB |

The registry lists both platform manifests; `docker buildx imagetools inspect emscripten/emsdk:7.0.0` returned the index and leaf digests above during this research. The immutable digest pins the toolchain image and its bundled Node runtime; record the chosen platform explicitly in the build evidence. [VERIFIED: Docker Registry manifest; [official image tags](https://hub.docker.com/r/emscripten/emsdk/tags), inspected 2026-11-10]

The native emsdk installer can install an exact SDK version, but a cross-platform direct install would need its component downloads and fixity handled separately. Prefer the single digest-pinned Linux image for required browser-artifact/Node coverage, integrated into the existing Linux native job after its native build so both observations use the same checkout. Docker is then needed only to reproduce this optional lane locally; do not add it to the native preset or six-platform native matrix. A different image size or verified native per-platform archive pin is a revisit trigger. [CITED: [emsdk install documentation](https://emscripten.org/docs/tools_reference/emsdk.html), [official image](https://hub.docker.com/r/emscripten/emsdk)]

## Alternatives and pitfalls

| Form | Fit | Decision |
|---|---|---|
| Modularized `.mjs` + `.wasm` | Generated loader serves browser and Node; JS callback table is supported. | **Use.** |
| Standalone `.wasm` | No generated JS loader; standalone mode uses WASI conventions and requires explicit initialization/import handling. | Revisit only if a second non-JS runtime becomes a real consumer. |
| Static `.a` only | Useful to link into a host executable; it is not directly loadable by browser or Node. | Keep as an internal build product only. |

- Do not conflate the WASI preset with browser Emscripten output or its loader.
- Do not cache typed-array views across callbacks or calls that may grow memory.
- Measure combined JS and Wasm artifact bytes, then warmed Node throughput against the same-source native build and same fixture. These are reproducible host measurements, not browser/device acceptance.
- The Emscripten 7.0.0 release is only four days old at research time. It is the exact current recommendation, but a compile/link failure in the planned output mode is a concrete trigger to evaluate the immediately preceding stable release, 6.0.13. No behavior has been built locally yet.

## Availability and limits

Local probe on 2026-11-10: Node `v26.10.0`, CMake `4.4.3`, Docker `29.5.2` are available; `emcc` and `emsdk` are absent, and the Emscripten image is not cached. The Docker daemon is available. The first arm64 image acquisition is about 800 MB compressed; transfer and compile times were not measured. No image pull, SDK install, browser run, remote CI or host UI action was performed.

## Sources

- Emscripten official docs: [downloads and emsdk](https://emscripten.org/docs/getting_started/downloads.html), [CMake integration](https://emscripten.org/docs/compiling/Building-Projects.html), [modularized output](https://emscripten.org/docs/compiling/Modularized-Output.html), [settings reference](https://emscripten.org/docs/tools_reference/settings_reference.html), [JS/C interop](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html), [Module API](https://emscripten.org/docs/api_reference/module.html).
- Emscripten official release and emsdk map: [release page](https://github.com/emscripten-core/emscripten/releases), [release tags](https://github.com/emscripten-core/emsdk/blob/main/emscripten-releases-tags.json).
- Official Emscripten container: [image overview](https://hub.docker.com/r/emscripten/emsdk), [tag digests](https://hub.docker.com/r/emscripten/emsdk/tags).
- CMake and Node primary docs: [CMake toolchains](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html), [Node.js WebAssembly](https://nodejs.org/learn/getting-started/nodejs-with-webassembly).

## Confidence and unresolved checks

- **Module/loader contract: MEDIUM.** Official docs establish the forms and callback mechanism; the first actual Emscripten build must verify this adapter's compile/link surface.
- **Pin/integrity: MEDIUM.** Release and OCI digests were checked from official sources/registry metadata; image acquisition and offline execution remain untested.
- **CMake wiring: MEDIUM.** Existing target seams and CMake toolchain pattern are verified, but Emscripten-specific `muhtari_host` compatibility and final imports/exports are implementation checks.
- **Revisit trigger:** if the target cannot build as a no-entry modularized ES module without adding broad host/filesystem code, stop and compare a smaller C shim or the preceding stable toolchain before expanding the loader contract.
