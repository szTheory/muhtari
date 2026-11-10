# Plan 12-14 local WASI evidence

**Disposition:** inventory correction and configure check passed; the complete local WASI workflow is blocked by the absent WASI C library/sysroot. This is advisory evidence only and does not satisfy PLAY-03.

## Source and tools

- Root checkout base: `4b3d3d5646550a73e522497b5450687a1cfa0a3c`.
- Corrected `tests/inventory/wasm32.txt` blob: `ed7ff43507ff5a2ad13fca816612cc60ecb89ff4`.
- Host: Darwin arm64; CMake 4.4.3.
- Clang and clang++: Homebrew LLVM 21.1.8, wasm32 target listed; executable SHA-256 `625165a3accc542dafef0264b5bbd18721b8e765c7ec819742ec7d7afd2491f6`.
- Wasmtime: 48.0.6, build `72bc45bd1`, native arm64 executable SHA-256 `9dad9691c32e097241d1f70371e36bea39ec3d51499134133340f77ecd9e6758`.
- `ld.lld`/`wasm-ld` and a WASI sysroot were not present in the installed LLVM, Homebrew, or project tool-pin locations. `pins.txt` and `scripts/fetch-pin.sh` provide no WASI SDK/sysroot pin. No pin or package installation was attempted.

## Inventory correction

Added only the two rows missing from run 42239124226, using the labels in their existing test registrations:

- `input.history_cart` — `input,portable`
- `policy.docker_contract.controls` — `policy`

`git diff --check -- tests/inventory/wasm32.txt` exited 0. The CMake configure consumer accepted the complete registered inventory after the edit.

## Commands and results

| Command | Exit | Result |
|---|---:|---|
| `build/pins/wasmtime/wasmtime --version` | 0 | Pinned Wasmtime 48.0.6, native arm64 build. |
| `PATH=/opt/homebrew/opt/llvm@21/bin:$PATH clang-21 --version` | 0 | Clang 21.1.8; wasm32 target available. |
| `cmake --preset wasm32` with the LLVM directory on `PATH` | 1 | The first configure could not resolve the preset's `clang++-21` name. Full output is retained in ignored `build/phase12-gap14-evidence/configure.log`. |
| `cmake --preset wasm32` with a temporary `clang++-21` alias to the same LLVM 21.1.8 executable | 0 | Configure and exact registered-inventory comparison passed. The alias and original cache snapshot are under ignored `build/phase12-gap14-evidence/`. |
| `cmake --workflow --preset wasm32` with the same qualified tool path | 1 | Build stopped because WASI libc headers `assert.h` and `string.h` are unavailable. No portable tests ran. Full output is retained in ignored `build/phase12-gap14-evidence/workflow.log`. |

The pre-configure cache was preserved as ignored `build/phase12-gap14-evidence/CMakeCache.before.txt`; its SHA-256 is `3d85dda6d089b06437a80da884662e89198da89dc9d6a9a6226c9d167ce993f9`. The first compiler-name failure was resolved with an ignored local alias. The workflow failure has a different, diagnosed cause: the WASI sysroot is missing. No further configure or workflow retry was made.

## Acceptance effect

The source data correction is retained, and configure proves that it matches the complete registered set. The plan's complete portable execution and JUnit audit did not pass because the local target libc is unavailable. The WASI lane remains advisory, run 42239124226 remains failed, Phase 12 remains accepted 0/4, Plan 12-01 remains halted, and Plan 12-16 must not publish or dispatch under this blocker. The separate Phase 14 stop is unchanged.
