---
phase: 02-the-cpu-matches-the-public-vectors
verified: 2026-10-19T04:47:19Z
status: passed
score: 5/5 must-haves verified (criterion 5 on pre-merge evidence, confirmed at /gsd-ship after the merge)
covered_files: [".github/workflows/ci.yml",".github/workflows/nightly.yml",".planning/phases/02-the-cpu-matches-the-public-vectors/02-01-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-01-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-02-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-02-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-03-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-03-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-04-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-04-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-05-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-05-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-06-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-06-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-07-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-07-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-08-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-08-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-09-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-09-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-10-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-10-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-11-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-11-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-12-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-12-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-13-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-13-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-14-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-14-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-15-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-15-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-16-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-16-SUMMARY.md","ASSET_POLICY.md","CMakeLists.txt","CMakePresets.json","docs/embedding.md","docs/hardware/bus.md","docs/hardware/cartridge.md","docs/hardware/riot.md","docs/hardware/sally.md","docs/maintaining.md","docs/runner.md","docs/testing.md","host/status_text.c","host/status_text.h","include/muhtari.h","libretro/muhtari_libretro.info.in","pins.txt","runner/exit_status.h","runner/main.c","runner/md5.c","runner/md5.h","scripts/fetch-pin.sh","scripts/publish.sh","src/bus.c","src/cart.c","src/cartdb.c","src/cartdb.h","src/cpu.c","src/instance.c","src/internal.h","src/riot.c","src/sha1.c","src/sha1.h","src/tia.c","tests/abi/tests.cmake","tests/alloc.h","tests/api/v1.c","tests/api/v2.c","tests/built/build.cmake","tests/built/expected.txt","tests/built/tests.cmake","tests/bus/test_bus.c","tests/bus/tests.cmake","tests/cart/images.c","tests/cart/images.h","tests/cart/test_cart.c","tests/cart/test_cartdb.c","tests/cart/test_sha1.c","tests/cart/tests.cmake","tests/cart/write_images.c","tests/cmake/abi.cmake","tests/cmake/budgets.cmake","tests/cmake/fetch_vectors.cmake","tests/cmake/firmware_scan.cmake","tests/cmake/no_float.cmake","tests/cmake/no_test_entries.cmake","tests/cmake/rom_hash_coverage.cmake","tests/cmake/vectors_pin.cmake","tests/cmake/vectors_sample_match.cmake","tests/core/test_core.c","tests/cpu/decimal/clark-6502.bin","tests/cpu/decimal/clark-65c02.bin","tests/cpu/decimal/clark.s","tests/cpu/full.cmake","tests/cpu/targets.cmake","tests/cpu/test_cpu.c","tests/cpu/test_reader.c","tests/cpu/test_vectors.c","tests/cpu/tests.cmake","tests/cpu/vector_bus.c","tests/cpu/vector_bus.h","tests/embed/CMakeLists.txt","tests/fuzz/loader.c","tests/fuzz/replay_main.h","tests/fuzz/seeds.c","tests/fuzz/seeds.h","tests/fuzz/tests.cmake","tests/header/header.c","tests/header/header.cpp","tests/hygiene/tests.cmake","tests/inventory/ci.txt","tests/inventory/fuzz.txt","tests/inventory/hygiene.txt","tests/inventory/nofp.txt","tests/inventory/vectors-full.txt","tests/inventory/wasm32.txt","tests/release/firmware.txt","tests/release/tests.cmake","tests/riot/test_riot.c","tests/riot/tests.cmake","tests/roms/manifest.txt","tests/runner/test_runner.c","tests/runner/tests.cmake","tests/tests.cmake","tests/vectors/6502-sample.n65v","tests/vectors/6502-v1.sha256","tests/vectors/fixtures/02.json","tests/vectors/fixtures/README.md","tests/vectors/fixtures/a9-bad-a.json","tests/vectors/fixtures/a9.json","tests/vectors/n65v.c","tests/vectors/n65v.h","tests/vectors/sst_json.c","tests/vectors/sst_json.h","tests/vectors/vecconv.c","tools/cartdb.cmake"]
covered_digest: "v3:sha256:e4bb33bb53e493ef063a0992410cf7b5be5826b1c845c9c0467048ab6aaa3eff"
behavior_unverified: 0
overrides_applied: 0
---

# Phase 2: The CPU matches the public vectors — Verification Report

**Phase goal:** The SALLY CPU matches the public 6502 vectors on every opcode and every bus cycle, the bus applies the 7800's slow cycles, HALT and RDY, RIOT and INPTCTRL behave as documented, and the runner probes and loads cartridge images safely; merging publishes the release.
**Verified:** 2026-10-19T04:47:19Z, branch `phase/02-the-cpu-matches-the-public-vectors` at 19e4c76 (the last code commit is 3fe7728; everything after it touches only `.planning/`).
**Status:** passed
**Re-verification:** No. This is the first verification.

## Commands run by the verifier

| Command | Exit | Result |
| ------- | ---- | ------ |
| `cmake --workflow --preset ci` | 0 | 405 of 405 tests passed, then `ci-release` 6 of 6. Labels: vectors 266, sally 19, riot 16, cart 19, built 5, fuzz 2, runner 30 |
| `cmake --workflow --preset hygiene` | 0 | 24 of 24 passed, including policy.cartdb_generated, policy.rom_hash_coverage, hygiene.no_float and hygiene.firmware_scan, each with its self test |
| `muhtari-test-vectors build/vectors-full/vectors-full/6502/v1/XX.json XX` (the HEAD build/ci binary) for 69 e9 61 6b 8b ab 93 02 00 7e | 0 | `0 of 10000 failed` for each opcode: ADC and SBC (decimal mode), ARR, ANE, LXA, SHA, JAM and BRK |
| `ctest -L vectors-full -E fetch` in build/vectors-full | 0 | 257 of 257 passed |
| `muhtari-test-images <scratch>` and then `muhtari-run --probe` on each image | see criterion 4 | |
| `scripts/check-title.sh "feat: the CPU matches the public 6502 vectors, with the 7800 bus, RIOT and cartridge probing"` | 0 | |
| `gh run view` 39199084217 (CI), 39199084192 (Nightly), 39199084187 (Release) | success | all three on head 3fe7728 |

## Goal Achievement

### Observable truths (roadmap success criteria)

| # | Truth | Status | Evidence |
| - | ----- | ------ | -------- |
| 1 | `ctest -L vectors` matches every opcode of the committed sample on final state and on every bus cycle, decimal mode included, on every pull request; `ctest -L vectors-full` matches the full set at its pin in the nightly run | VERIFIED | The ci and wasm32 inventories each hold the 256 tests `cpu.vectors.00` to `cpu.vectors.ff`. A local run prints `0 of 100 failed` for each. `tests/cpu/test_vectors.c` compares the registers, raw P, every RAM byte, the cycle count and each cycle's address, value and direction. It also checks `jammed` against the twelve JAM opcodes and checks the poll mark. It sets `ane_lxa_constant = 0xEE`. The sample has the manifest row with pin 2f6980a2 and matches the `sst-6502-v1` row in pins.txt. Full set: the verifier ran 10 opcodes on the HEAD binary (0 of 10000 failed each) and all 257 local vectors-full tests passed. Hosted Nightly run 39199084192 job `vectors-full`: `100% tests passed out of 258`, `check_junit: 258 tests ran once each`. nightly.yml has the cron `17 4 * * *` and the `vectors-full` job |
| 2 | `ctest -L sally` passes: 4- and 6-tick cycles, HALT only after two INPTCTRL writes and stopping reads and writes, RDY holding only reads so a read-modify-write completes both writes, and an NMI raised in the last cycle is taken after the next instruction | VERIFIED | 19 of 19 passed, all named for the criterion: bus.cycle.{fast_4_ticks, slow_6_ticks, men_off_all_slow}, bus.halt.{gate_two_writes, gate_counts_rmw_dummy, latency_two_cycles, waits_for_slow_fall, freezes_writes}, bus.rdy.{holds_reads_only, rmw_completes_both_writes}, bus.nmi.* and cpu.nmi.*. The verifier read `rdy_rmw_completes_both_writes`: INC $02 with INPTCTRL locked must take 26 ticks (4+4+6+6+6), leave the second write's byte on the bus, and hold the next fetch. Each case also rejects the tick count the nearest wrong model would give. In `src/bus.c`, `muhtari__bus_request_halt` is ignored until `halt_gate_writes >= 2`, and RDY holds only when `is_read` |
| 3 | `ctest -L riot` passes: the timer through HALT and WSYNC at each interval, its flag, SWCHA/SWCHB with direction registers and power-on values, RIOT RAM at $0480–$04FF, INPTCTRL locking and open-bus reads | VERIFIED | 16 of 16 passed: riot.timer.{reads_n_minus_1, flag_at_n_interval_plus_1, counts_past_underflow, read_in_setting_cycle_keeps_flag, access_clears_flag, counts_through_halt, counts_through_wsync, slow_cycle_counts_one}, riot.pa7.edge_flag, riot.swcha.ddr, riot.swchb.ddr, riot.power_on.defaults, riot.ram.window, bus.inptctrl.lock and bus.open_bus.{unmapped, write_only}. `src/riot.c` computes the timer when it is read (`riot_intim`, `riot_timer_flag`) |
| 4 | `muhtari-run --probe FILE` prints the A78 fields, board, region, peripherals and all four hashes; identifies a header-stripped image through the hash list; gives a 2600 image, an unsupported board and a malformed, truncated or oversized file each a distinct status and message; the counting-allocator test and `ctest -R fuzz.regress.loader` pass on every platform; the loader fuzz target runs nightly | VERIFIED | The verifier's probe of `linear-v4-48k` printed kind, source, board, options, sizes, sound, devices, ports, region, title, the 13 `a78_*` fields and 8 hashes (sha1, sha256, md5 and crc32, each headered and headerless), exit 0. Refusals: `2600-4k` exit 77 with `unsupported: 2600 (planned)`. `souper-v4` exit 77 with `unsupported board: souper`. `bad-magic-1000` exit 1 with `not a 7800 or 2600 cartridge image`. `header-only` and `supergame-short` exit 1 with `the image ends before the size its header gives`. `oversized` exit 1 with `larger than 1 MiB plus a 128-byte header` and empty stdout. The core statuses differ for each class (ERR_UNSUPPORTED_2600, ERR_UNSUPPORTED_BOARD, ERR_INVALID_IMAGE, ERR_TRUNCATED_IMAGE, ERR_UNSUPPORTED_SIZE, checked in cart.probe.*). Hash list: `muhtari-run-testdb --probe` on the stripped 7800basic `simple` .bin printed `source hash`, `board linear`, `region ntsc`, sha1 1aa9559b…. The shipped runner prints `source footer` for the same file, and cart.identify.bundled_has_no_test_entries passes. `cart.load.no_alloc_on_reject` (read) checks per refusal class that the allocation count is unchanged, the instance is byte-identical and a failing allocator still gives the refusal status. It and `fuzz.regress.loader` (21 inputs) are in ci.txt and wasm32.txt, and hosted CI 39199084217 passed on all six build legs, asan, be32, nofp and wasm32. Nightly 39199084192 job `loader-fuzz` succeeded |
| 5 | Merging the phase's pull request publishes a release through the Phase 1 pipeline with no manual step | VERIFIED (pre-merge evidence; confirmed at ship) | Only the merge can show this, so `/gsd-ship` confirms it after the merge (CLAUDE.md rule 9); it is not a gap. Evidence now: the ship title passes `scripts/check-title.sh` (exit 0). release-please-config.json has `bump-minor-pre-major: true` with version.txt at 0.1.0, so a `feat:` merge cuts 0.2.0. Release run 39199084187 on head 3fe7728 succeeded, with `rehearsal`, `install-line` and `cleanup` green (`release-please` and `publish` are skipped on pull requests, as they should be). Draft probe PR #9 is CLOSED and was not merged |

**Score:** 5/5 truths verified, 0 present but behavior-unverified.

### Plan must-haves (02-01 to 02-16)

There are about 100 plan truths. Each roadmap criterion above lists the ones the verifier checked through the named tests, all of which pass in the verifier's own ci or hygiene run. The rest rest on artifacts:
- 02-01/02-03/02-04: cpu.vectors.{convert.*, reader, self_test, pin}, cpu.decimal.clark and its 65C02 self test, and policy.rom_hash_coverage all pass.
- 02-08: policy.cartdb_generated byte-compares `src/cartdb.c` with the generator output (hygiene, passed), and cart.sha1.fips passes.
- 02-09/02-10/02-11: cart.probe.*, cart.identify.* and cart.load.* (19 cart tests) pass. abi.* passes.
- 02-12: runner.* (30) passes, including runner.md5 and runner.exit.table.
- 02-13: built.samples rebuilds `simple` and checks both SHA-256s; its self test passes.
- 02-14: tests/cpu/full.cmake registers the fetch, the sample match and 256 per-opcode tests on the upstream JSON. nightly.yml has the vectors-full and loader-fuzz jobs with measured timeouts.
- 02-15: release.firmware_scan and hygiene.firmware_scan pass, each with its self test.
- 02-16: timeouts carry run IDs, hygiene.timeout_comments passes, the probe PR is closed and the title check passes.

### Required artifacts

| Artifact | Status | Details |
| -------- | ------ | ------- |
| `src/cpu.c` (1,467 lines) | VERIFIED | Wired into `muhtari` through the `muhtari_cpu` OBJECT library and the vector harness |
| `src/bus.c`, `src/riot.c`, `src/tia.c` | VERIFIED | Exercised by the sally and riot tests through the real bus |
| `src/cart.c`, `src/cartdb.c`, `src/sha1.c` | VERIFIED | `muhtari_probe` and the internal loader. cartdb is generated and checked byte for byte |
| `runner/main.c`, `runner/md5.c`, `host/status_text.c` | VERIFIED | `--probe` behaviour seen directly |
| `tests/vectors/6502-sample.n65v` (1,677,886 bytes) | VERIFIED | Manifest row and SHA-256 checked by policy.rom_hash_coverage |
| `tests/cpu/full.cmake`, `tests/cmake/fetch_vectors.cmake`, nightly jobs | VERIFIED | Hosted nightly green |

### Key links

| From | To | Status | Details |
| ---- | -- | ------ | ------- |
| muhtari-test-vectors | src/cpu.c objects | WIRED | Same objects as the library. The poll mark and `jammed` are checked per test |
| bus.c HALT gate | INPTCTRL writes | WIRED | `halt_gate_writes` is counted in `inptctrl_write` and read in `muhtari__bus_request_halt` |
| muhtari-run --probe | muhtari_probe | WIRED | Real probe output, statuses become exit codes 0, 1 and 77 |
| muhtari-run-testdb | tests/cart/test_cartdb.c | WIRED | Link-time substitution, `source hash` seen. The shipped runner does not contain the entry |
| nightly.yml | vectors-full and fuzz presets | WIRED | Hosted run 39199084192 |

### Behavioral spot-checks

| Behavior | Command | Result | Status |
| -------- | ------- | ------ | ------ |
| Full-set opcodes on the HEAD binary | `muhtari-test-vectors …/XX.json XX` ×10 | 0 of 10000 failed each | PASS |
| Probe of a clean v4 image | `muhtari-run --probe linear-v4-48k` | all fields and 8 hashes, exit 0 | PASS |
| Refusal classes | `muhtari-run --probe` on 7 refusal images | distinct messages, exit 77 or 1, oversized prints nothing on stdout | PASS |
| Hash-list identity | `muhtari-run-testdb --probe simple.bas.bin` | `source hash` | PASS |

### Probe execution

There are no `scripts/*/tests/probe-*.sh` scripts in this repository. The "probe PR" is a CI rehearsal pull request (#9, closed), and its runs appear above.

### Requirements coverage

| Requirement | Description | Status | Evidence |
| ----------- | ----------- | ------ | -------- |
| CPU-01 | Committed sample, every opcode, final state and every cycle, decimal mode, unstable constant | SATISFIED | Criterion 1, plus cpu.unstable.sally_constant |
| CPU-02 | Full set at its pin, nightly | SATISFIED | Criterion 1: local run plus Nightly 39199084192 |
| CPU-03 | `ctest -L sally` timing, HALT, RDY, NMI | SATISFIED | Criterion 2 |
| CPU-04 | `ctest -L riot`, INPTCTRL, open bus | SATISFIED | Criterion 3 |
| CPU-05 | `--probe`, hash list, refusal classes, counting allocator, fuzz replay and nightly fuzz | SATISFIED | Criterion 4 |

Every ID in the plans' `requirements:` fields is one of CPU-01 to CPU-05. REQUIREMENTS.md maps no other ID to Phase 2, so no requirement is orphaned.

### Anti-patterns found

| File | Line | Pattern | Severity | Impact |
| ---- | ---- | ------- | -------- | ------ |
| (110 phase-changed files) | - | TBD, FIXME or XXX debt markers | none found | - |
| 02-16-SUMMARY.md | coverage entry for criterion 5 | `status: pending-ship`, outside the summary schema's pass, fail and unknown values | Info | Bookkeeping only. Set it to `pass` at ship once the 0.2.0 Release run is confirmed, or record it as `unknown` |
| 02-REVIEW.md | WR-01 to WR-04, IN-01 to IN-03 | open code-review findings (all `open` in 02-REVIEW-DISPOSITION.md) | Warning (advisory) | Left to the separate code-review fix step. None of them fails a criterion: WR-01 limits the nightly `pull_request` path filter, while the scheduled nightly still runs every job. WR-02 concerns a caller passing a struct shorter than its size tag. WR-03 and WR-04 concern HALT and NMI requests that overlap and the run loop for a jammed CPU, which is phase 3 work |

### Human verification required

None. Every check above was run by the verifier. The merge is the owner's consent action, and the release it publishes is confirmed at `/gsd-ship` (rule 9), not in UAT.

### Gaps summary

No gaps. The phase goal holds in the codebase: SALLY matches all 2,560,000 SingleStepTests 6502 vectors on state and every bus cycle, and the 7800 bus timing, HALT, RDY, NMI, RIOT and INPTCTRL behave as their tests require. The probe and loader refuse every bad class distinctly without allocating, and the nightly jobs are wired and green. Only the merge itself can show the release being published, and `/gsd-ship` confirms that after the merge.

---

_Verified: 2026-10-19T04:47:19Z_
_Verifier: Claude (gsd-verifier)_
