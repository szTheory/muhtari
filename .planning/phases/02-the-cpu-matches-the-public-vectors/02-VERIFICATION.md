---
phase: 02-the-cpu-matches-the-public-vectors
verified: 2026-10-19T19:37:48Z
status: passed
score: 5/5 must-haves verified (criterion 5 on pre-merge evidence, confirmed at /gsd-ship after the merge)
covered_files: [".github/workflows/ci.yml",".github/workflows/nightly.yml",".planning/phases/02-the-cpu-matches-the-public-vectors/02-01-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-01-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-02-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-02-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-03-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-03-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-04-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-04-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-05-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-05-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-06-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-06-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-07-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-07-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-08-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-08-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-09-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-09-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-10-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-10-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-11-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-11-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-12-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-12-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-13-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-13-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-14-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-14-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-15-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-15-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-16-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-16-SUMMARY.md","ASSET_POLICY.md","CMakeLists.txt","CMakePresets.json","docs/embedding.md","docs/hardware/bus.md","docs/hardware/cartridge.md","docs/hardware/riot.md","docs/hardware/sally.md","docs/maintaining.md","docs/runner.md","docs/testing.md","host/status_text.c","host/status_text.h","include/muhtari.h","libretro/muhtari_libretro.info.in","pins.txt","runner/exit_status.h","runner/main.c","runner/md5.c","runner/md5.h","scripts/fetch-pin.sh","scripts/publish.sh","src/bus.c","src/cart.c","src/cartdb.c","src/cartdb.h","src/cpu.c","src/instance.c","src/internal.h","src/riot.c","src/sha1.c","src/sha1.h","src/tia.c","tests/abi/tests.cmake","tests/alloc.h","tests/api/v1.c","tests/api/v2.c","tests/built/build.cmake","tests/built/expected.txt","tests/built/tests.cmake","tests/bus/test_bus.c","tests/bus/tests.cmake","tests/cart/images.c","tests/cart/images.h","tests/cart/test_cart.c","tests/cart/test_cartdb.c","tests/cart/test_sha1.c","tests/cart/tests.cmake","tests/cart/write_images.c","tests/cmake/abi.cmake","tests/cmake/budgets.cmake","tests/cmake/fetch_vectors.cmake","tests/cmake/firmware_scan.cmake","tests/cmake/no_float.cmake","tests/cmake/no_test_entries.cmake","tests/cmake/rom_hash_coverage.cmake","tests/cmake/vectors_pin.cmake","tests/cmake/vectors_sample_match.cmake","tests/core/test_core.c","tests/cpu/decimal/clark-6502.bin","tests/cpu/decimal/clark-65c02.bin","tests/cpu/decimal/clark.s","tests/cpu/full.cmake","tests/cpu/targets.cmake","tests/cpu/test_cpu.c","tests/cpu/test_reader.c","tests/cpu/test_vectors.c","tests/cpu/tests.cmake","tests/cpu/vector_bus.c","tests/cpu/vector_bus.h","tests/embed/CMakeLists.txt","tests/fuzz/loader.c","tests/fuzz/replay_main.h","tests/fuzz/seeds.c","tests/fuzz/seeds.h","tests/fuzz/tests.cmake","tests/header/header.c","tests/header/header.cpp","tests/hygiene/tests.cmake","tests/inventory/ci.txt","tests/inventory/fuzz.txt","tests/inventory/hygiene.txt","tests/inventory/nofp.txt","tests/inventory/vectors-full.txt","tests/inventory/wasm32.txt","tests/release/firmware.txt","tests/release/tests.cmake","tests/riot/test_riot.c","tests/riot/tests.cmake","tests/roms/manifest.txt","tests/runner/test_runner.c","tests/runner/tests.cmake","tests/tests.cmake","tests/vectors/6502-sample.n65v","tests/vectors/6502-v1.sha256","tests/vectors/fixtures/02.json","tests/vectors/fixtures/README.md","tests/vectors/fixtures/a9-bad-a.json","tests/vectors/fixtures/a9.json","tests/vectors/n65v.c","tests/vectors/n65v.h","tests/vectors/sst_json.c","tests/vectors/sst_json.h","tests/vectors/vecconv.c","tools/cartdb.cmake"]
covered_digest: "v3:sha256:99092bede274b6ef927dc976132fc20db30ea69dbc71760f1479c04307c7be58"
behavior_unverified: 0
overrides_applied: 0
re_verification:
  previous_status: passed
  previous_score: 5/5
  reason: "stale after code-review fix round 1 (be6d152, e0b2a25, 5d6c2ae) changed 13 covered files"
  gaps_closed: []
  gaps_remaining: []
  regressions: []
---

# Phase 2: The CPU matches the public vectors — Verification Report

**Phase goal:** The SALLY CPU matches the public 6502 vectors on every opcode and every bus cycle, the bus applies the 7800's slow cycles, HALT and RDY, RIOT and INPTCTRL behave as documented, and the runner probes and loads cartridge images safely; merging publishes the release.
**Verified:** 2026-10-19T19:37:48Z, branch `phase/02-the-cpu-matches-the-public-vectors` at 1350b30 (last code commit 5d6c2ae; everything after it touches only `.planning/`). The branch is 14 commits ahead of its remote; nothing from the fix round has been pushed.
**Status:** passed
**Re-verification:** Yes. The report of 2026-10-19T04:47:19Z (passed, 5/5) went stale when code-review fix round 1 changed 13 covered files. It had no gaps, so this run re-checks all five criteria. The changed files get a full check and the rest get a regression check.

## What changed since the last verification

`git diff 19e4c76..HEAD -- . ':!.planning'` touches 13 files (+163/−13):

- **be6d152 (WR-02):** only comments and docs. The `MUHTARI__SIZE_TAG_MAX` comment in `src/instance.c` and "Structs and sizes" in `docs/embedding.md` now say that a size tag promises that many readable bytes, and that the 4096-byte cap only limits how far a wrong tag can make the library read. No behaviour changed.
- **e0b2a25 (WR-03):** `muhtari__bus_request_halt` now returns early unless `halt_stage == HALT_NONE`, and `muhtari__bus_raise_nmi` returns early while `nmi_pending` is set. I traced both through `src/bus.c`. `events()` moves HALT_REQUESTED to HALT_LOW and then back to HALT_NONE once the window's hold is done, so the next request is accepted. `latch_nmi()` clears `nmi_pending` on every latch, so a dropped edge can never block later edges for good. The rule is written up in docs/hardware/bus.md (HALT and NMI) and in the entry-point comment in src/internal.h, and both match the code.
- **5d6c2ae (WR-04):** `muhtari__cpu_step` now checks `jammed` first. A step on a jammed CPU makes one `muhtari__bus_read(m, 0xFFFF)` and returns, so it takes no NMI and changes no register. docs/hardware/sally.md (JAM) and src/internal.h say the same. The vector harness still steps each case once on a cleared CPU (test_vectors.c:183), so the guard never runs on vector cases. The JAM opcodes still give their 11-read pattern, and the full-set runs below confirm it.

All three new tests have real assertions, and each names the wrong model it rules out. `bus.halt.one_window_at_a_time` checks tick counts for a second request made both before and after /HALT falls, and checks that the next request after the window is taken. `bus.nmi.one_edge_at_a_time` checks that `nmi_phi2` is 2, not 4. `cpu.jam.step_reads_ffff` checks one $FFFF read per step, phi2+1, every register, the poll mark, `jammed`, and that the latched NMI is not taken. 02-REVIEW-FIX.md records each test failing against the source before the fix. I did not repeat that stash run.

## Commands run by the verifier (this run)

| Command | Exit | Result |
| ------- | ---- | ------ |
| `cmake --workflow --preset ci` | 0 | 408 of 408, then `ci-release` 6 of 6. Labels: vectors 266, sally 22 (was 19, plus the 3 new tests), riot 16, cart 19, built 5, fuzz 2, runner 30. 256 `cpu.vectors.XX` tests |
| `cmake --workflow --preset hygiene` | 0 | 24 of 24 |
| `cmake --build build/vectors-full && ctest --test-dir build/vectors-full -L vectors-full -E fetch -j8` | 0 | 257 of 257: the full SingleStepTests set at its pin, built from HEAD |
| `build/ci/tests/muhtari-test-vectors build/vectors-full/vectors-full/6502/v1/XX.json XX` for 02 12 f2 69 e9 00 4c 6c ee 8b | 0 | `0 of 10000 failed` for each, including three JAM opcodes after the jammed-step change |
| asan lane in `ubuntu:26.04` arm64 (docs/testing.md recipe, plus clang-21, lld-21, libclang-rt-21-dev, `cmake --workflow --preset asan`) | 0 | 397 of 397 |
| `grep -nE "TBD\|FIXME\|XXX"` over the 13 changed files | 1 (no match) | no debt markers |
| `scripts/check-title.sh "feat: the CPU matches the public 6502 vectors, with the 7800 bus, RIOT and cartridge probing"` | 0 | |
| `gh run list --branch phase/02-the-cpu-matches-the-public-vectors` | 0 | the latest hosted runs are still on 3fe7728 (CI 39199084217, Nightly 39199084192, Release 39199084187, all success). Nothing has run on the fix commits yet, because they are not pushed |

## Goal Achievement

### Observable truths (roadmap success criteria)

| # | Truth | Status | Evidence |
| - | ----- | ------ | -------- |
| 1 | `ctest -L vectors` matches every opcode of the committed sample on final state and every bus cycle, decimal mode included, on every pull request; `ctest -L vectors-full` matches the full set at its pin nightly | VERIFIED | This run: all 256 `cpu.vectors.XX` pass in the ci workflow. The rebuilt vectors-full tree passes 257 of 257, and 10 opcodes give 0 of 10000 failed on the HEAD binary, including JAM 02, 12 and F2 after the WR-04 change. Since the last report, `tests/cpu/test_vectors.c`, the sample, the pin and nightly.yml are unchanged (they are not in the fix diff). The hosted nightly run 39199084192 was on 3fe7728, so it does not cover the fix commits. The local full-set run does |
| 2 | `ctest -L sally` passes: 4- and 6-tick cycles, HALT only after two INPTCTRL writes, stopping reads and writes, RDY holding only reads with a read-modify-write's two writes completing, NMI in the last cycle taken after the next instruction | VERIFIED | 22 of 22 sally tests pass: the 19 from before plus bus.halt.one_window_at_a_time, bus.nmi.one_edge_at_a_time and cpu.jam.step_reads_ffff. The HALT gate still checks `halt_gate_writes < 2u` before anything else, and RDY still holds only on `is_read`. The new early returns only refuse a request while one is in flight, and every earlier timing case still passes. asan 397 of 397 |
| 3 | `ctest -L riot` passes: timer through HALT and WSYNC per interval, flag, SWCHA/SWCHB with DDRs and power-on values, RIOT RAM $0480–$04FF, INPTCTRL lock, open bus | VERIFIED | 16 of 16 riot tests pass. `src/riot.c` and `tests/riot/` are not in the fix diff. The HALT change lands in `bus.c`, through which the timer-through-HALT case runs, and riot.timer.counts_through_halt still passes |
| 4 | `muhtari-run --probe FILE` prints A78 fields, board, region, peripherals and four hashes; header-stripped identity through the hash list; distinct statuses and messages for 2600, unsupported board, malformed, truncated and oversized; counting-allocator test and `fuzz.regress.loader` pass on every platform; nightly loader fuzz | VERIFIED | `runner/`, `src/cart.c`, `src/cartdb.c`, `host/` and `tests/cart/` are not in the fix diff. runner 30 of 30, cart 19 of 19 and fuzz 2 of 2 pass in this run's ci workflow and again under asan. The fix's only change in `src/instance.c` is a comment. The direct probe observations from the last report (all fields and 8 hashes; exit 77 for `2600-4k` and `souper-v4`; exit 1 with distinct messages for `bad-magic-1000`, `header-only`, `supergame-short` and `oversized`; `source hash` from `muhtari-run-testdb`) rest on code the fix round did not touch. The be32, wasm32 and nofp lanes on the fix commits rest on the fixer's container runs (02-REVIEW-FIX.md: be32 372, wasm32 370, nofp 12, all exit 0). The hosted run on the ship pull request runs them again before the merge (see below) |
| 5 | Merging the phase's pull request publishes a release through the Phase 1 pipeline with no manual step | VERIFIED (pre-merge evidence; confirmed at ship) | Only a merge can show this, so `/gsd-ship` confirms it after the merge (CLAUDE.md rule 9), not UAT. Evidence now: the ship title passes `scripts/check-title.sh` (exit 0). The release workflow, release-please config and `scripts/publish.sh` are not in the fix diff. The Release run 39199084187 rehearsal was green on 3fe7728 |

**Score:** 5/5 truths verified, 0 present but behavior-unverified.

### Plan must-haves (02-01 to 02-16)

These are unchanged from the last report. The fix round added behaviour (one HALT window and one NMI edge at a time, idle reads from a jammed CPU) that no plan truth contradicts. Every test named for a plan truth still passes in this run's ci and hygiene workflows.

### Required artifacts

| Artifact | Status | Details |
| -------- | ------ | ------- |
| `src/cpu.c` | VERIFIED | jammed guard at the head of `muhtari__cpu_step`. The full set passes on the rebuilt binary |
| `src/bus.c` | VERIFIED | Both early returns sit before any state changes. `rearm`, `events` and `latch_nmi` are unchanged, and the state they clear lets the next request in |
| `src/internal.h`, `src/instance.c` | VERIFIED | Comment-only changes that match the code |
| `docs/hardware/bus.md`, `docs/hardware/sally.md`, `docs/embedding.md` | VERIFIED | I read them against the code. JAM no longer hands idling to the run loop, which now matches `cpu.c` |
| `tests/bus/test_bus.c`, `tests/cpu/test_cpu.c` and their tests.cmake | VERIFIED | The new cases are registered under `sally portable`, appear in the ci.txt and wasm32.txt inventories, and ran in this ci run |
| everything else in covered_files | VERIFIED (regression) | Not in the fix diff, and the ci, hygiene, vectors-full and asan runs pass |

### Key links

| From | To | Status | Details |
| ---- | -- | ------ | ------- |
| muhtari-test-vectors | src/cpu.c objects | WIRED | Rebuilt from HEAD. 257 full-set tests pass |
| bus.c HALT gate | INPTCTRL writes and halt_stage | WIRED | `halt_gate_writes` is counted in `inptctrl_write`. `halt_stage` goes back to HALT_NONE in `events()` |
| bus.c NMI raise | latch_nmi | WIRED | `nmi_pending` is set in `raise_nmi` and cleared on every latch, in `events()` and in `hold()` |
| cpu_step jammed guard | bus read | WIRED | Goes through `muhtari__bus_read`, so phi2 and the bus log count it (checked by cpu.jam.step_reads_ffff) |
| muhtari-run --probe | muhtari_probe | WIRED | Unchanged. runner.* 30 of 30 |
| nightly.yml | vectors-full and fuzz presets | WIRED | Unchanged. The local vectors-full preset passes |

### Data-flow trace

Not applicable. The phase draws no UI. Probe output values come from `muhtari_probe` over the file's bytes, as traced in the last report, and that code is unchanged.

### Behavioral spot-checks

| Behavior | Command | Result | Status |
| -------- | ------- | ------ | ------ |
| One HALT window at a time | `ctest` bus.halt.one_window_at_a_time (in the ci run) | Passed | PASS |
| One NMI edge at a time | `ctest` bus.nmi.one_edge_at_a_time | Passed | PASS |
| Jammed step reads $FFFF only | `ctest` cpu.jam.step_reads_ffff | Passed | PASS |
| JAM and other opcodes on the full set | `muhtari-test-vectors …/XX.json XX` ×10 | 0 of 10000 failed each | PASS |
| Memory safety of the changed paths | asan lane in container | 397 of 397 | PASS |

### Probe execution

There are no `scripts/*/tests/probe-*.sh` scripts in this repository, and the phase declares none.

### Requirements coverage

| Requirement | Source plans | Description | Status | Evidence |
| ----------- | ------------ | ----------- | ------ | -------- |
| CPU-01 | 5 plans | Committed sample, every opcode, state and every cycle, decimal mode, unstable constant | SATISFIED | Criterion 1 |
| CPU-02 | 4 plans | Full set at its pin, nightly | SATISFIED | Criterion 1: local 257 of 257 at HEAD |
| CPU-03 | 3 plans | `ctest -L sally` timing, HALT, RDY, NMI | SATISFIED | Criterion 2: 22 of 22 |
| CPU-04 | 3 plans | `ctest -L riot`, INPTCTRL, open bus | SATISFIED | Criterion 3: 16 of 16 |
| CPU-05 | 9 plans | `--probe`, hash list, refusal classes, counting allocator, fuzz replay, nightly fuzz | SATISFIED | Criterion 4 |

The plans' `requirements:` fields list only CPU-01 to CPU-05. REQUIREMENTS.md maps exactly those five to Phase 2 (lines 176 to 180), so no requirement is orphaned.

### Anti-patterns found

| File | Line | Pattern | Severity | Impact |
| ---- | ---- | ------- | -------- | ------ |
| 13 changed files | - | TBD, FIXME or XXX | none found | - |
| 02-16-SUMMARY.md | coverage entry for criterion 5 | `status: pending-ship` | Info | Carried over from the last report. Set it to `pass` at ship once the release run is confirmed |
| 02-REVIEW.md (re-review) | WR-01, WR-02, IN-01 | deferred to phase 3 in 02-REVIEW-DISPOSITION.md | Info | None fails a criterion. WR-01 (a dropped HALT or NMI request is silent) has no caller until MARIA. WR-02 is test code, a seed under rule 7. IN-01's frame loop is phase 3 work. The first-round seeds WR-01, IN-02 and IN-03 are tracked in a pending todo |
| hosted CI | - | fix commits not yet run on hosted CI | Info | Not a gap. This run checked them locally (ci, hygiene, vectors-full, asan), and the fixer ran be32, wasm32 and nofp in containers. The ship pull request's required CI runs every lane on the head before the merge can happen. /gsd-ship confirms a CI run exists on the head commit (rule 9) |

### Human verification required

None. I ran every check above. The merge is the owner's consent action, and the release it publishes is confirmed at `/gsd-ship` (rule 9), not in UAT.

### Gaps summary

No gaps. The fix round's changes are small and match their docs, and each comes with a test that fails before the fix and passes after it. They cause no regression: the full SingleStepTests set still matches, the 22 sally tests, the 16 riot tests and the cart, runner and fuzz tests all pass, and asan is clean. The phase goal holds at 1350b30. What remains is ship-time confirmation: hosted CI on the pushed head and the release that the merge publishes.

---

_Verified: 2026-10-19T19:37:48Z_
_Verifier: Claude (gsd-verifier)_
