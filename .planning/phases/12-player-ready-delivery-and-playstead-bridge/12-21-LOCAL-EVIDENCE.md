# Plan 12-21 local evidence

**Disposition at task 2:** The generated native and WASI state-control routes and the complete local WASI lane pass. Final native/hygiene gates are recorded below as they run; Phase 12 remains accepted 0/4. This is local prerequisite evidence only and makes no claim about RetroArch input delivery, released-core readiness, hosted CI, or Playstead.

## Frozen source and preserved failure

- Base: `018de7728f746e7c41f86b8e21f7383be0ff636d` on `phase/12-player-ready-delivery-and-playstead-bridge`.
- `tests/diag/tests.cmake` SHA-256 after the route repair: `434b37c9d6ef1b5b4ea56350b82bf909811c3859cec9b9abc7fd2bbaa153a497`.
- The Plan 12-20 full WASI JUnit and audit receipts, CTest logs, generated failing script, and focused timeout sample were copied before configuring or running tests. Preserved JUnit SHA-256: `ef565ee095c262068696f6bd8071a34cb72bfff75c1126967eef0586bebf3656`; preserved generated script SHA-256: `e12cb4e016d178c4275bfe5460f18cd28ef368fa3c603723396518724b4310da`.
- The prior complete report had 622 cases and three failures: `retroarch.states.controls` invoked its WASI driver without the configured emulator; `runner.movie.v2_edges` and `runner.region.pal` hit their existing scaled deadlines. Those outcomes remain in the preserved report.
- Protected tracked-file baseline was frozen at the plan base; the 1,051-file inventory and per-file hashes are in ignored `build/phase12-gap21-evidence/protected-identities.json`.

## Retained SDK and runtime

The prior Plan 12-20 component and notice manifests were requalified read-only before the first build. All identities still match; no SDK/runtime was downloaded or installed.

| Component | Requalified identity |
|---|---|
| wasi-sdk archive | 111,397,330 bytes; SHA-256 `092d1859ad6338de40c16ee0779102ff19c067dd863ad816ba5afd8bd8cb709f` |
| SDK / LLVM | SDK 28.0; Clang 21.1.4-wasi-sdk; LLVM revision `222fc11f2b8f25f6a0f4976272ef1bb7bf49521d` |
| wasi-libc | revision `d181bdd429f0067c92d817909b01e4970ad7d70a`; 9,811 sysroot files; saved tree SHA-256 `77dcfe121982be41558eb9f35f1880543be3e7c2b34cd8936bf7760e29cda381` |
| Components and notices | Clang, LLD, builtins archive, target libc, and all 11 notice documents match their saved identities |
| Runtime | Pinned native Wasmtime 48.0.6, build `72bc45bd1`; executable SHA-256 `9dad9691c32e097241d1f70371e36bea39ec3d51499134133340f77ecd9e6758` |

The retained local SDK uses a different wasi-libc sysroot from hosted CI, so a local pass does not establish hosted acceptance.

## Task 1 — state-control emulator route

The generated template now preserves the configured emulator as a CMake list. Native generation produces an empty list, so native calls still execute the driver directly. WASI generation produces the pinned Wasmtime command and preopen followed by the driver. Both existing `--fault-controls` and `--prelaunch-controls` calls use that list; test logic, assertions, fixtures, and deadlines are unchanged.

| Command | Exit | Result |
|---|---:|---|
| `python3 build/phase12-gap21-evidence/run_bounded.py configure-native native cmake --preset ci` | 0 | Native configuration accepted the existing registration inventory. |
| `python3 build/phase12-gap21-evidence/run_bounded.py build-native-tracer native cmake --build --preset ci --target muhtari-test-retroarch-states muhtari-test-retroarch-receipt` | 0 | Both owning native targets built under existing warnings. |
| `python3 build/phase12-gap21-evidence/run_bounded.py ctest-native-tracer native ctest --test-dir build/ci -R '^retroarch[.]states[.]controls$' --no-tests=error --output-on-failure --output-junit ../phase12-gap21-evidence/states-native.xml` | 0 | One native state-control case passed with no failure, error, or skip. |
| `python3 build/phase12-gap21-evidence/run_bounded.py configure-wasi wasi cmake --preset wasm32` | 0 | Actual WASI configuration used the requalified SDK and pinned emulator. This starts the fixed 20-minute WASI window at `2026-11-07T16:41:26.342450Z`, cutoff `2026-11-07T17:01:26.342450Z`. |
| `python3 build/phase12-gap21-evidence/run_bounded.py build-wasi-tracer wasi cmake --build --preset wasm32 --target muhtari-test-retroarch-states` | 0 | Actual WASI state driver built under the existing warning policy. |
| `python3 build/phase12-gap21-evidence/run_bounded.py ctest-wasi-tracer wasi ctest --test-dir build/wasm32 -R '^retroarch[.]states[.]controls$' --no-tests=error --output-on-failure --output-junit ../phase12-gap21-evidence/states-wasi.xml` | 0 | One actual WASI case passed with no failure, error, or skip; the generated script invoked both driver-control modes through Wasmtime. |

The new native and WASI focused reports are in ignored `build/phase12-gap21-evidence/states-native.xml` and `states-wasi.xml`; their SHA-256 values are `659bc15f9c653db9995a33f6055fc6141213cec605c6bc24405f0ac138b429a4` and `2108f2a021034de556ef3fce760e2a1f92c7d67997f0a2de56b5eb5d96d6d698`. The focused checks launched no RetroArch or content process and made no permission/API call. Run URL: N/A-local.

## Task 2 — complete WASI lane and runner scheduling decision

The complete corrected-source lane passed before any runner scheduling change. The prior full report and focused timing sample remain preserved. Since neither named timeout recurred, `tests/runner/tests.cmake` remains byte-identical to the plan base; no `RUN_SERIAL` property, before/after metadata checker, copied metadata fault, or extra focused runner invocation was needed.

| Command | Exit | Result |
|---|---:|---|
| `python3 build/phase12-gap21-evidence/run_bounded.py workflow-wasi-initial wasi cmake --workflow --preset wasm32` | 0 | Complete local WASI workflow finished in 69.144 seconds without timeout or scratch-cap breach. |
| `python3 build/phase12-gap21-evidence/run_bounded.py audit-junit-wasi-initial wasi cmake -DJUNIT=build/wasm32/junit.xml -DINVENTORY=tests/inventory/wasm32.txt -DLABELS=portable -P tests/cmake/check_junit.cmake` | 0 | Exact portable inventory accepted 622 tests, each once, with no failures, errors, or skips. |

The full report SHA-256 is `7817a1d3a3a056fe832cccff8ac9ad139830d58b81e807f3bca58281733bac06`. In that complete report, `retroarch.states.controls` passed in 0.166 seconds; `runner.movie.v2_edges` passed in 5.370 seconds under its unchanged six-second limit; and `runner.region.pal` passed in 3.405 seconds under its unchanged four-second limit. No cause is attributed to the prior timeout results. The full WASI result establishes only this retained local advisory SDK lane, not hosted CI or PLAY-03 acceptance. Run URL: N/A-local.

## Resource bounds

The fixed 45-minute local cutoff is `2026-11-07T17:24:24.621416Z`. The WASI cutoff above includes configuration through the final complete portable JUnit audit. At the end of the WASI audit, attributable growth was 1,392,690 bytes against the 2 GiB cap. Reused SDK/archive/runtime sizes are recorded separately in ignored `build/phase12-gap21-evidence/budget.json`.

