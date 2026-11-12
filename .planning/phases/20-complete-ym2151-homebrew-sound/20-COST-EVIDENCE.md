# YM cost and final evidence

Status: required final local gates passed at `daab4c82c0bb3393b3e93ee2b23ead09236f16ec`; independent review is clean at product head `5158bb649160f081f6f1253da3be3099b93c8a03`. The difference is review prose only. Phase verification owns the final goal judgment. Later planning/archive edits do not change this product snapshot. No publication is claimed.

## Executed workload and measurements

The ordinary exact fixture is `65c25495be4a0d73ae83a9f3f742186ffda34bff4058c3a5bc304bed428258d6`. The qualified adverse fixture is `11b6fa4251de4771cfcc19b86ebc9f8c6679ddab211f3b4db42221c47535290b`. Both are actual generated 16,512-byte cartridges, executed through C, runner, loaded native libretro, actual Wasm and extracted packages. The adverse test verifies all eight channels/32 operators, full feedback/carrier summing, noise/LFO, shortest running timers, dual POKEY and coinstalled XM gates. It masks CPU IRQ servicing during address/data traffic; the separate hostile test exercises real IRQ entry/return and jammed CPU progression. It is not a proof of the slowest possible legal cartridge.

Each timing case warms up twelve frames, serializes a baseline, then restores and executes three complete sixty-frame windows. Both observers include callbacks, copying and canonical video/audio hashing. The final native observer also retains the same timed duplicated-mono check as Node. Native uses monotonic host time; portable uses Node performance.now. Native AppleClang 21 runs on Darwin/arm64; the exact pinned SDK/Node runs linux/amd64 in Docker on that host. Values are descriptive for this execution context, not a pure-core comparison, low-power prediction or speed threshold.

Final-source complete measurements, source identity `dd141eac0f0ff503298a48ceb56fb7d6e09a9e97b41f4a710078fdafd60e13c5`, including the public clock guard, duplicated-channel check and mixed-chip chronological merge:

| Workload | Region | Native ms/frame min / median / max | Wasm/Node ms/frame min / median / max |
|---|---|---|---|
| Ordinary | NTSC | 3.7245 / 3.7555 / 3.7779 | 5.9479 / 6.1513 / 6.3987 |
| Ordinary | PAL | 4.4848 / 4.5138 / 4.5793 | 6.4759 / 6.8638 / 6.9288 |
| Adverse | NTSC | 4.5591 / 4.5622 / 4.8267 | 7.1710 / 7.3210 / 7.6320 |
| Adverse | PAL | 5.4643 / 5.4932 / 5.5628 | 6.6152 / 7.7740 / 7.8836 |

This run emitted a 57,902-byte loader and 215,165-byte Wasm companion, with SHA-256 `cb3cf97da9fe34d317ac9ec27462e715a436ca4940384b162369ed69f5f04095` and `6169cd623f0976e16dcf948a0b3e4c5b06f84d8c1a94a8dea25e84152ee65798`. Node is v24.19.0; the SDK image is the existing emsdk pin `6bd7aef93408b10bf7b36642cb7c91119b302f720b9ec2d20439f73cf07fe894` (linux/amd64 under the Darwin/arm64 Docker host). Source/toolchain/platform, fixture hashes and all windows are in ignored `build/ci/tests/emscripten/ym-native.json`, `ym-worst-native.json`, `ym-measurement.json` and `ym-worst-measurement.json`. Exact native observer/library/runner/module and archive byte hashes, plus all 64 changed product-file hashes, are in `build/v6-phase20-final-identities.json`. The portable producer refreshes reports, invalidates stale reports, validates the exact no-skip inventory and replays the clean extracted pair. Raw logs/JSON are not committed.

## Measured space and functional bounds

The same public allocator probe, separately linked against genuine kickoff and current libraries, measures unloaded create/destroy requested bytes: old 89,144; current 91,720, an increase of 2,576 bytes. Both allocate once and free all requested bytes. The YM struct is 2,568 bytes; synth is 2,480 bytes versus 2,472. These are native allocator/type extents, not RSS, and exclude media storage.

The separately registered private raw observer allocates 1,492,816 bytes versus 829,352 in the old source. It is absent from default public/library/libretro instances. The combined unloaded observed instance is 1,584,536 requested bytes. Observation must be registered before load. Recurring lifecycle/hostile/public buffer tests require no core allocation during frames. Current ordinary/adverse YM state is 179,330 bytes; non-YM format-1/revision-6 extents and bytes are independently identical to the genuine kickoff library across ten cases.

Structural proof is separate from timing:

- Fixed eight channels and 32 operators. Each output edge advances after 64 YM clocks = 128 MARIA ticks. A hot address/data pair consumes four 6502 instructions, twelve fast CPU cycles, at least 48 master ticks. Register writes invalidate caches; prepare still runs at most once per output step.
- Timer A minimum/maximum 64/65,536 YM clocks; B 1,024/262,144. Conversion to absolute bus deadlines uses overflow-safe distances and chronological held-cycle catch-up. Public restore now requires YM time equal completed bus time, preventing replay of an arbitrary historical gap.
- The existing instruction/interrupt bound is `(456 + 2*454 + 12 + 6) * (8+7) = 20,730` ticks. Twelve source changes per tick bound the private carry at `12*(20,730+1)=248,772` six-byte records, plus a 32-record pending stage. Offsets fit uint16. The recurring observer test exercises the complete carry order and signed words.
- The public FIFO remains 1,103 samples, with a 962-sample PAL frame and at most 141 future samples. PCM cadence remains 48 kHz in both regions. No unrelated deadline/filter capacity was widened.
- Signed YM post-DAC words are -32768..32767. Conditional mono gain yields total level -4590..13769, width 18,359 below the old 18,360. Q23/Q30 ring, integrator, delta products and DC headroom retain their old limits and peak below 29,203.6. The independent derivation is [MIXER-BOUND](../../research/ym2151-integration/MIXER-BOUND.md); `ym.mix` exercises endpoints, anti-phase, rounding and actual OPM edges.

No new runtime/space quota is derived from these measurements. Exact input/state/observer bounds serve existing concrete output, allocation and finite-return contracts.

## Retained failures and context limits

The first expanded stress candidate failed its runtime topology test: unlocked TIA writes altered INPTCTRL before setup completed. The guest now locks 7800 mode before those writes; all-channel/dual-POKEY/timer assertions pass. Earlier candidate timings are excluded. The final audit also reproduced acceptance of a repaired-CRC stale YM tick; the added test fails before the chronology guard and passes after it without ever executing an unbounded catch-up.

Pinned Homebrew ASan failed before main even for a tiny unrelated C probe. The identical unchanged flags with available Apple Clang pass deliberate memory/UB fault controls and the final YM focus. This qualifies a separate local Apple sanitizer check; it is not pinned Linux sanitizer acceptance. No machine restart, reinstall or repeated known-failing startup is selected.

The final qualified Apple ASan/UBSan focus passes all seventeen YM/state/native-consumer tests. Its two deliberate faults still abort with their expected diagnostics (`build/v6-phase20-final-sanitizer-faults.json`). The first expanded final sanitizer run exposed backward mixed-source timestamps: YM caught up after later POKEY output. The added Release observer reproduces the old defect, and both Release and sanitizer pass after merging YM through TIA events and actual POKEY half edges. All twelve sources participate in both regions. Timer batch bounds, output bounds and no-YM behavior remain intact. Two formal review-fix rounds are complete and clean; no formal gap round was used.

The existing offline Linux image `b757a4c1fc8be8e4760d8652cc39aa3379bd076f968834248d72b081144c3d3b` has GCC13.3 and Make. With the qualified CMake4.4.3 pin, the unchanged no-FP flags/configuration configure, but compilation exits2 because GCC13 does not recognize required `-Wmissing-variable-declarations`. This is an unqualified compiler, not no-FP product acceptance. Its failed log is retained; no warning is dropped and no package is installed. Ninja, clang-21, PowerPC cross compilers, qemu-ppc and Wasmtime are absent. No current no-FP, big-endian or WASI execution pass is claimed. Native integer-source/fault controls and actual emitted Emscripten Wasm pass separately.

## Command/exit index

All URLs are N/A-local. `build/v6-phase20-final-gates.json` records exact argv, exit status, source and individual logs; `build/v6-phase20-final-identities.json` binds actual artifacts. The required gates below all pass. The optional unavailable compiler lane remains failed, separate from acceptance. Historical failures retain their own ignored logs.

| Scope | Command / receipt | Result |
|---|---|---|
| Complete native CI, clean native packages | python3 build/ship-local-gate.py (unchanged cmake --workflow --preset ci with bounded jobserver); build/v6-phase20-final-native-reviewed.log | exit0; 713 main checks/122.30s plus eight release checks/1.52s |
| Unchanged hygiene | python3 build/phase18-hygiene-gate.py; build/v6-phase20-final-hygiene.log | exit0; 30 checks/50.35s, same qualified outside-sandbox boundary |
| Exact frozen BSD-3 seed | cmake -DROOT=. -DSEED=build/v6-ym-seed -DWORK=build/v6-ym-oracle -P tests/audio/ym_oracle.cmake | exit0; source/object receipt unchanged, manual busy divergence checked separately |
| Genuine kickoff/current compatibility and controls | cmake -DROOT=. -DOLD_SOURCE=.worktrees/ym-state-baseline -DOLD_BUILD=build/v6-state-baseline -DNEW_BUILD=build/ci -DWORK=build/v6-state-compat [-DMODE=controls] -P tests/core/state_compat.cmake | both exit0; ten cases, two directions and safe refusal; old products not rebuilt |
| Qualified combined sanitizer | asan preset with /usr/bin/clang and /usr/bin/clang++ in build/v6-ym-sanitize-apple; exact focused ctest argv in final-gates JSON | exit0; seventeen tests; deliberate fault controls detected both faults |
| Production quality producer | cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-current.json -DSHA=daab4c82c0bb3393b3e93ee2b23ead09236f16ec -DCONFIG=ci -DSOURCE=N/A-local -P tests/cmake/quality_evidence.cmake | exit0; full JUnit and additive YM equality evidence admitted |
| Actual emitted and extracted Wasm | sh scripts/emscripten.sh build/ci | exit0; all six exact-inventory tests, clean ZIP, both-region ordinary/adverse reports and controls |
| Actual archive through offline mock publisher | cmake -DROOT=. -DWORK=build/v6-phase20-final-publish -DPRODUCTION_ARCHIVE=build/emscripten/muhtari-libretro-emscripten.zip -DEXPECTED_SHA=daab4c82c0bb3393b3e93ee2b23ead09236f16ec -P tests/policy/publish.cmake | exit0; actual payload and transferred YM receipt checks plus signer/ref/integrity fault controls; no network publication |
| Unqualified GCC13 no-FP supplement | offline docker, nofp preset with installed Make and pinned CMake; exact argv in final-gates JSON | exit2 before product compilation; unsupported required warning; no acceptance |
| Native focused state/YM/observer | build/v6-phase20-07-guard-verified.log | exit0, after chronology fix |
| Stress qualification | build/v6-phase20-07-worst-qualified.log | exit 8; guest mapping defect |
| Corrected stress | build/v6-phase20-07-qualified-cost.log | exit 0 |
| Stale-tick reproduction | build/v6-phase20-07-clock-guard-before.log | exit 8; accepted malformed state |
| Earlier portable cost/package | sh scripts/emscripten.sh build/ci; build/v6-phase20-07-portable-cost.log | exit0 at earlier source; superseded by final measurements above |
| Docs/notices/version budgets | build/v6-phase20-07-docs-fixed.log | exit 0, unchanged budgets |
| Public instance/observer extents | build/v6-instance-footprint*.json; build/v6-ym-extents*.json | exit 0; requested-byte/type measurements |

Public space probes were relinked against final `build/ci/libmuhtari.a` with C17/O2/Wall/Wextra/Werror: `build/v6-instance-footprint.c` uses the frozen public header and current library; `build/v6-ym-extents.c` uses current include/src and actual observer allocation. Both exit0. The frozen genuine old products and prior public probe remain unchanged.

The final local main suite's longest cases are the existing publisher, lane-equality and audio judges. Queue/jobserver bounds, workloads and deadlines are retained. These local runs provide no multi-run, same-host-class evidence for a structural/cache/timeout change, so no such change is made. Failed early native hash inventory and sanitizer runs remain failures, not retrospective passes.

Historical Phase 12/14 host/remote stops and nine deferred v3 requirements remain unchanged. No host/device/browser, remote dispatch, push, merge or publication occurs in this local milestone scope.
