# Plan 12-20 local evidence

**Disposition:** The battery file observer passes on native and on the retained WASI target, and the complete native workflow passes. The complete portable WASI gate remains blocked: its exact JUnit report contains three failed cases outside this plan's two source owners. This does not satisfy Phase 12-16's complete local WASI prerequisite. Phase 12 remains accepted 0/4; no hosted, host-input, permission, content, package-readiness or Playstead result is inferred.

## Source and preserved failure

- Plan base: `624390c03e9e929da501f1faaeffecb94d202587`; source task commit: `c5b8395`.
- Final `tests/core/test_state.c` SHA-256: `4415174cb35659dd43a4416381b55784d422b63a27d896410f45bcc225604777`.
- Final `tests/core/tests.cmake` SHA-256: `e9b5b7320a6a4db06988fd3e1c148b0ee54fd7a39eedac553355c66eae0ab802`.
- The 235-file frozen source/pin/preset/inventory identity set is retained in ignored `build/phase12-gap20-evidence/protected-identities.json`. Only the two declared test sources changed. Presets, toolchain, inventories, JUnit consumer, pins, core, runner, adapter and other test sources are unchanged.
- The previous `build/wasm32` cache (18,817,918 bytes; no JUnit report) is preserved in ignored `build/phase12-gap20-evidence/prior-wasm32`. Plan 12-17 receipts and its retained `prior-wasm32`/JUnit copy remain untouched.
- Plan 12-17's original complete WASI workflow remains terminal at `tests/core/test_state.c:2022`: `tmpfile()` was deprecated/undefined under SDK 28 with warnings-as-errors. Its command exited 1 before tests and produced no complete JUnit report. The frozen old source and log remain at `build/phase12-gap17-evidence/source.sha` and `build/phase12-gap17-evidence/full-workflow.stdout`.

## Retained local SDK and runtime requalification

The exact retained bundle was reused; no download, installation or fallback tool was used. `build/phase12-gap20-evidence/toolchain-requalification.json` records the current read-only requalification.

| Component | Requalified identity |
|---|---|
| wasi-sdk archive | 111,397,330 bytes; SHA-256 `092d1859ad6338de40c16ee0779102ff19c067dd863ad816ba5afd8bd8cb709f` |
| SDK / LLVM | SDK 28.0; Clang 21.1.4-wasi-sdk; LLVM revision `222fc11f2b8f25f6a0f4976272ef1bb7bf49521d` |
| wasi-libc | revision `d181bdd429f0067c92d817909b01e4970ad7d70a`; 9,811 sysroot files; saved tree SHA-256 `77dcfe121982be41558eb9f35f1880543be3e7c2b34cd8936bf7760e29cda381` |
| Bundled target components | Clang, LLD 21.1.4, builtins archive and target libc hashes all match the saved identities |
| Notices | All 11 retained SDK, LLVM and wasi-libc notice documents match their byte counts and hashes |
| Runtime | Pinned native Wasmtime 48.0.6, build `72bc45bd1`; executable SHA-256 `9dad9691c32e097241d1f70371e36bea39ec3d51499134133340f77ecd9e6758` |

The process-local `clang-21` and `clang++-21` aliases resolve into that one SDK. The generated compile and link commands use the target SDK, `wasm32-wasi`, `lime1`, signal emulation and `--stack-first`. The build-owned fixture path is inside the existing source/build preopen; no preopen or runtime setting changed. This retained local SDK is advisory and has a different wasi-libc sysroot from hosted CI.

## Fixture and tracer results

`muhtari-test-state` alone receives `STATE_HOST_FILE_PATH`, derived from `${PROJECT_BINARY_DIR}/tests/state-battery-host.tmp`. `test_battery()` removes only that exact stale path, opens it with standard C11 exclusive mode `w+x`, writes and flushes the existing marker, and requires a nonnegative cursor. The file-local observer checks cursor, seek, exact read length, stream error, content and restored cursor. The battery case requires the intact file to pass and wrong-cursor and changed-marker controls to reject; it restores the marker, closes the stream and checks path removal. All existing battery/state/replay assertions and the `core.state.battery` name, labels, timeout, inventory and preopen remain intact.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci` | 0 | Native CI configuration accepted the unchanged inventory. |
| `cmake --build --preset ci --target muhtari-test-state` | 0 | Strict-warning native target built. |
| `ctest --test-dir build/ci -R '^core[.]state[.]battery$' --no-tests=error --output-on-failure --output-junit ../phase12-gap20-evidence/tracer-native.xml` | 0 | One owning case passed; no failure or skip; the build-owned fixture was absent afterward. |
| `cmake --preset wasm32` | 0 | Exact WASI registration inventory and retained SDK compiler aliases configured. |
| `cmake --build --preset wasm32 --target muhtari-test-state` | 0 | Actual WASI target compiled and linked. |
| `ctest --test-dir build/wasm32 -R '^core[.]state[.]battery$' --no-tests=error --output-on-failure --output-junit ../phase12-gap20-evidence/tracer-wasi-final.xml` | 0 | Final formatted source passed once under pinned Wasmtime; no failure or skip; the fixture was absent afterward. |

The first native workflow capture recorded exit 125 because the ignored budget monitor raced a generated-file replacement. The monitor was corrected to tolerate files disappearing during a size sample. Its subsequent exact workflow run is authoritative; no source or test change resulted from the capture issue.

## Complete local workflow results

| Command | Exit | Result |
|---|---:|---|
| `cmake --workflow --preset wasm32` | 8 | Completed the portable run and wrote a new 622-case `build/wasm32/junit.xml`; three cases failed. This run preceded formatting-only source normalization; the final formatted source's owning WASI tracer was rerun successfully. No WASI workflow retry or timeout/source change followed. |
| `cmake -DJUNIT=build/wasm32/junit.xml -DINVENTORY=tests/inventory/wasm32.txt -DLABELS=portable -P tests/cmake/check_junit.cmake` | 1 | Exact consumer rejected the three failed cases; the report had no skips. |
| `cmake --workflow --preset ci` | 0 | Final-source native ordinary workflow passed 700/700 tests; package generation and all 6 release tests passed. |
| `cmake --preset hygiene` | 0 | Existing hygiene tools configured without acquisition. |
| First `ctest --test-dir build/hygiene -R '^hygiene[.]format$' --no-tests=error --output-on-failure` | 8 | Found three formatting-only wrapping issues in the marker restoration call. |
| Final `ctest --test-dir build/hygiene -R '^hygiene[.]format$' --no-tests=error --output-on-failure` | 0 | Corrected source passes the existing formatter gate. |
| `scripts/hygiene.sh --tree` | 0 | Tracked tree hygiene passed. |

The full WASI failures are:

- `retroarch.states.controls`: its generated `tests/diag/retroarch_states_controls.cmake` invokes the WASI `muhtari-test-retroarch-states` module directly with `execute_process`, without the configured emulator. The observed result is `exec format error`. The needed wrapper is outside this plan's source owners.
- `runner.movie.v2_edges`: hit its existing scaled 6-second CTest limit (base 3 seconds at scale 2) during the full portable run.
- `runner.region.pal`: hit its existing scaled 4-second CTest limit (base 2 seconds at scale 2) during the full portable run.

The latter two logs end at their configured deadlines while running the existing multi-process runner checks. A standalone rerun would not clear the exact full-lane failure, so neither failed case nor the full WASI lane was retried. Fixing the emulator wrapper or changing timeout policy exceeds this plan's declared owners and preserved settings.

The total verification window was fixed at `2026-11-07T14:42:12Z` with a `15:27:12Z` cutoff; the WASI window was fixed at `2026-11-07T14:49:24Z` with a `15:09:24Z` cutoff. All work finished within the original cutoffs. The 2 GiB scratch/build cap was not approached: the monitor's maximum attributable measurement was 209,342,651 bytes. The reused archive and SDK sizes are separately retained in ignored `toolchain-requalification.json` and the budget record.

## Bounded fixture review

**Pass for this fixture repair:** the file belongs to the test's binary directory and its target; no product API, core storage, general file abstraction, dependency, test registration or new preopen was added. The exclusive open avoids truncating a competing file. Each observation rejects failed I/O and verifies the cursor/content contract, while `CHECK` failures still flow through close and removal. The two fault controls fail at the intended cursor/content boundary. The extra work is one small local file and a few fixed-size operations in the existing case. No fixture defect or threat surface was found under `.planning/preparation/ENGINEERING.md` §11.

This review does not clear the unrelated full-lane failures, hosted CI's distinct sysroot, actual RetroArch delivery, any permission state or PLAY-03. No RetroArch/content process, event-post API, Accessibility/TCC/settings operation, remote CI, publication or Phase 14 action occurred.
