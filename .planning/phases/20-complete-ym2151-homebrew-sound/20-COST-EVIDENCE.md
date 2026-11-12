# YM cost and final evidence

Status: implementation and focused checks pass; independent review and final-source gates are pending. The current source adds a public restore-time YM/bus chronology guard after the measurements below. These timings are retained prior-source evidence and will be replaced with final-source measurements before acceptance.

## Executed workload and measurements

The ordinary exact fixture is `65c25495be4a0d73ae83a9f3f742186ffda34bff4058c3a5bc304bed428258d6`. The qualified adverse fixture is `11b6fa4251de4771cfcc19b86ebc9f8c6679ddab211f3b4db42221c47535290b`. Both are actual generated 16,512-byte cartridges, executed through C, runner, loaded native libretro, actual Wasm and extracted packages. The adverse test verifies all eight channels/32 operators, full feedback/carrier summing, noise/LFO, shortest running timers, dual POKEY and coinstalled XM gates. It masks CPU IRQ servicing during address/data traffic; the separate hostile test exercises real IRQ entry/return and jammed CPU progression. It is not a proof of the slowest possible legal cartridge.

Each timing case warms up twelve frames, serializes a baseline, then restores and executes three complete sixty-frame windows. Both observers include callbacks, copying and canonical video/audio hashing. The final native observer also retains the same timed duplicated-mono check as Node. Native uses monotonic host time; portable uses Node performance.now. Native AppleClang 21 runs on Darwin/arm64; the exact pinned SDK/Node runs linux/amd64 in Docker on that host. Values are descriptive for this execution context, not a pure-core comparison, low-power prediction or speed threshold.

Prior-source complete measurements, source identity `2fa30a6de9e6375e5dd8f9f0fd5ebfded199d7d672f51f3ae9e8dddc825b9900` (before the final chronology guard and timed native duplicate-channel check):

| Workload | Region | Native ms/frame min / median / max | Wasm/Node ms/frame min / median / max |
|---|---|---|---|
| Ordinary | NTSC | 2.1719 / 2.1725 / 2.1807 | 3.4465 / 3.4809 / 3.5384 |
| Ordinary | PAL | 2.6067 / 2.6276 / 2.6658 | 3.5950 / 3.6707 / 3.7006 |
| Adverse | NTSC | 2.8359 / 2.8424 / 2.9061 | 4.8780 / 4.9379 / 5.0481 |
| Adverse | PAL | 3.4199 / 3.4379 / 3.4580 | 5.3830 / 5.4007 / 5.7859 |

This run emitted a 57,902-byte loader and 214,852-byte Wasm companion. Source/toolchain/platform, exact loader/companion digests, fixture hashes and all windows are in ignored `build/ci/tests/emscripten/ym-native.json`, `ym-worst-native.json`, `ym-measurement.json` and `ym-worst-measurement.json`. The portable producer refreshes those files, invalidates stale reports, validates the exact no-skip inventory and replays the clean extracted pair. Raw logs/JSON are not committed.

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

Pinned Homebrew ASan failed before main even for a tiny unrelated C probe. The identical unchanged flags with available Apple Clang passed deliberate memory/UB fault controls and the earlier YM focus. This qualifies a separate local Apple sanitizer check; it is not pinned Linux sanitizer acceptance. Fresh final-source sanitizer evidence remains pending. No machine restart, reinstall or repeated known-failing startup is selected.

An existing related-project Linux image has GCC 13.3 and CMake 3.28; the repository's already-qualified CMake 4.4 pin can serve a no-FP lane if Ninja qualifies. It lacks clang-21, PowerPC cross compilers, qemu-ppc and Wasmtime. Big-endian/WASI execution is not claimed; no packages are installed to manufacture availability. Actual emitted Wasm is independently covered above.

## Command/exit index

All URLs are N/A-local. Final-source commands and results will be recorded here after independent review; historical failures retain their own ignored logs.

| Scope | Command / receipt | Result |
|---|---|---|
| Native focused state/YM/observer | build/v6-phase20-07-guard-verified.log | exit 0, after chronology fix |
| Stress qualification | build/v6-phase20-07-worst-qualified.log | exit 8; guest mapping defect |
| Corrected stress | build/v6-phase20-07-qualified-cost.log | exit 0 |
| Stale-tick reproduction | build/v6-phase20-07-clock-guard-before.log | exit 8; accepted malformed state |
| Prior-source portable cost/package | sh scripts/emscripten.sh build/ci; build/v6-phase20-07-portable-cost.log | exit 0; final source refresh pending |
| Docs/notices/version budgets | build/v6-phase20-07-docs-fixed.log | exit 0, unchanged budgets |
| Public instance/observer extents | build/v6-instance-footprint*.json; build/v6-ym-extents*.json | exit 0; requested-byte/type measurements |

Historical Phase 12/14 host/remote stops and nine deferred v3 requirements remain unchanged. No host/device/browser, remote dispatch, push, merge or publication occurs in this local milestone scope.
