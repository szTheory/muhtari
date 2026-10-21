# Stack

**Project:** muhtari · **Researched:** 2026-10-15 · Details and pins: `.planning/preparation/ENGINEERING.md`.

| Layer | Choice | Why |
|---|---|---|
| Language | C17, `CMAKE_C_EXTENSIONS OFF`; public header also valid C++ | Embeds in any host, console SDK or WASM |
| Build | CMake (≥ 3.25 for embedders; presets v11 locally and in CI), Ninja, CTest; one entrypoint `cmake --workflow --preset <lane>` | No script to keep in step with CI |
| Runtime dependencies | None; the core links only `memcpy`/`memmove`/`memset`/`memcmp` | Portability and determinism |
| Vendored | `libretro.h` (MIT) only | The libretro ABI |
| Test harness | Plain CTest executables, an in-repo check header and a small seeded property harness | No framework dependency |
| Fixture tools (pinned; the `ci` preset fetches them on any host) | 7800basic WASM package under wasmtime (samples, dasm, 7800sign), dasm for OpenBIOS, cc65 at a pinned commit, qemu-user + PowerPC GCC for the big-endian lane, Valgrind for instructions/frame | Reproducible test inputs with no committed third-party ROM bytes |
| Formatting and analysis | clang-format 21, clang-tidy analyzer checks | One version locally and in CI |
| CI/CD | GitHub Actions (Ubuntu 26.04, macOS 15, Windows 2025/11-arm), release-please 17.6.0 via its v5.0.0 action, a GitHub App token, artifact attestations | Automatic, verifiable releases |
| Hosts | RetroArch (Metal build) on the owner's Mac; the runner for automation and Playstead | No GUI in this repository |
