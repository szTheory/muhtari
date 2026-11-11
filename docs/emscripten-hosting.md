# Emscripten libretro bundle

Extract `muhtari-libretro-emscripten.zip` as a directory and keep the files under `cores/` together. The generated
loader resolves `muhtari_libretro.wasm` and `muhtari-source-id.txt` beside `muhtari_libretro.mjs`; do not move one
without the others. `info/muhtari_libretro.info` is the libretro metadata file.

Import `cores/muhtari_libretro.mjs`, read the adjacent Wasm bytes, and pass them as `wasmBinary` to the loader's
async factory. The resulting module exports the regular libretro callbacks. The host owns callback buffers and
keeps them valid for the duration required by each callback; the core does not own a browser window, audio
device or input device.

`muhtari-bundle.txt` records the source commit, core-source identity and pinned SDK. `SHA256SUMS` binds the
loader and Wasm bytes. Local verification runs the Node lifecycle against a clean extraction and an independent
native oracle; it does not establish real-browser, mobile or device behavior. See the
[full libretro integrator guide](https://github.com/szTheory/muhtari/blob/main/docs/libretro.md#webassembly-build-and-offline-host).
