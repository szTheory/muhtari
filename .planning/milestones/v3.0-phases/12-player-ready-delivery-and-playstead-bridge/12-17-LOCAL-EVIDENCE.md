# Plan 12-17 local WASI evidence

**Disposition:** focused real-target WASI verification passed; the complete local WASI lane halted during build at the existing `tmpfile()` use in `tests/core/test_state.c`. No portable tests or full-lane JUnit report were produced. No PLAY-03 or Phase 12 acceptance is claimed.

## Frozen source and toolchain

- Frozen source HEAD: `23d18aac72566081852f6a0b39651164fa212c2f` (`build/phase12-gap17-evidence/source.sha`). Protected source, pin, preset and inventory hashes are in ignored `build/phase12-gap17-evidence/protected-files.json`.
- Host: Darwin arm64; CMake 4.4.3; Ninja 1.13.2; Python 3.14.4.
- Official `wasi-sdk-28` metadata matched the exact 111,397,330-byte arm64 macOS asset and SHA-256 `092d1859ad6338de40c16ee0779102ff19c067dd863ad816ba5afd8bd8cb709f`. One non-resumable asset download completed with exit 0. Archive inspection accepted 10,703 paths, links and file types, with 364,439,587 expanded regular-file bytes, then extracted privately.
- SDK version 28.0; Clang 21.1.4-wasi-sdk; LLD 21.1.4. Official tag metadata confirms LLVM revision `222fc11f2b8f25f6a0f4976272ef1bb7bf49521d` and wasi-libc revision `d181bdd429f0067c92d817909b01e4970ad7d70a`. Eleven SDK, LLVM and libc license/notice documents were retained with source URLs and hashes in ignored `build/phase12-gap17-evidence/notice-manifest.json`.
- The SDK compiler resource directory, bundled `wasm-ld`, builtins archive and target libc resolve inside this one extracted SDK. `clang.cfg` binds the bundled `share/wasi-sysroot`; the SDK sysroot contains 9,811 files with manifest SHA-256 `77dcfe121982be41558eb9f35f1880543be3e7c2b34cd8936bf7760e29cda381`. Component hashes and the SDK sysroot identity are retained in ignored `sdk-identities.json`.
- Existing pinned native Wasmtime 48.0.6 (build `72bc45bd1`) is unchanged; executable SHA-256 is `9dad9691c32e097241d1f70371e36bea39ec3d51499134133340f77ecd9e6758`.

## Focused actual-target result

The prior `build/wasm32` cache and JUnit were preserved under ignored `build/phase12-gap17-evidence/prior-wasm32`. The new ordinary `wasm32` preset configured from the unchanged repository root. The generated Ninja commands compile the actual `tests/policy/retroarch_preflight.c` with `wasm32-wasi`, `lime1`, signal emulation and strict warnings, then link with `--stack-first` and `-lwasi-emulated-signal`. The artifact is a genuine WebAssembly module; its SHA-256 is retained with the command transcript.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset wasm32` | 0 | Exact registration inventory accepted. |
| `cmake --build --preset wasm32 --target muhtari-test-preflight` | 0 | Actual mock compiled and linked as WASI. |
| `ctest --test-dir build/wasm32 -R '^policy[.]retroarch[.](preflight_api|child_failure)$' --no-tests=error --output-on-failure --output-junit ../phase12-gap17-evidence/tracer-wasi.xml` | 0 | Both owning tests passed once; JUnit reports 2 tests, 0 failures, 0 skips. Runtime is the existing pinned Wasmtime. |

The compiler’s optional `-print-sysroot` probe exited 1 because this Clang build does not implement that option. The initial `-###` link-only probe omitted C compilation because `/dev/null` was used as input; its validator therefore did not find compile-only flags. These were metadata-probe limitations, not lane failures or tool changes. The SDK config, actual generated compile/link commands and target tests establish the paths and semantics directly.

## Complete lane result

`cmake --workflow --preset wasm32` configured successfully, then stopped in its build step after 88 of 230 Ninja actions when compiling `tests/core/test_state.c:2022`: `tmpfile()` is deprecated because WASI does not define it, and `-Werror` makes this diagnostic fatal. The same unsupported call is present in the unchanged root source. The failure is preserved in ignored `build/phase12-gap17-evidence/full-workflow.stdout`; command exit is 1. The task scope prohibits a source change or retry. The workflow did not reach the test step, and there is no new full JUnit file to audit. The previous cache and JUnit remain preserved in `prior-wasm32`.

The configured wasm32 inventory was accepted, and the focused mock proved the coherent SDK can compile, link and run those two registrations. This does not close the complete local WASI prerequisite: the full workflow and portable JUnit completeness check remain blocked on a supported disposition of the existing `tmpfile()` use.

## Acceptance boundary

The focused pass establishes only the two named mock registrations. It does not establish complete inventory execution, hosted sysroot equivalence, cross-platform hashes, actual RetroArch input, packaged-core readiness or PLAY-03. Plan 12-14/15 history remains unchanged; Phase 12 remains accepted 0/4 and Plan 12-16 stays blocked pending all prerequisites.
