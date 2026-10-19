---
phase: 02-the-cpu-matches-the-public-vectors
verified: 2026-10-19T20:24:57Z
status: passed
score: 5/5 must-haves verified (criterion 5 on pre-merge evidence, confirmed at /gsd-ship after the merge)
covered_files: [".github/workflows/ci.yml",".github/workflows/nightly.yml",".planning/phases/02-the-cpu-matches-the-public-vectors/02-01-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-01-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-02-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-02-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-03-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-03-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-04-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-04-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-05-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-05-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-06-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-06-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-07-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-07-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-08-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-08-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-09-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-09-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-10-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-10-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-11-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-11-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-12-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-12-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-13-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-13-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-14-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-14-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-15-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-15-SUMMARY.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-16-PLAN.md",".planning/phases/02-the-cpu-matches-the-public-vectors/02-16-SUMMARY.md","ASSET_POLICY.md","CMakeLists.txt","CMakePresets.json","docs/embedding.md","docs/hardware/bus.md","docs/hardware/cartridge.md","docs/hardware/riot.md","docs/hardware/sally.md","docs/maintaining.md","docs/runner.md","docs/testing.md","host/status_text.c","host/status_text.h","include/muhtari.h","libretro/muhtari_libretro.info.in","pins.txt","runner/exit_status.h","runner/main.c","runner/md5.c","runner/md5.h","scripts/fetch-pin.sh","scripts/publish.sh","src/bus.c","src/cart.c","src/cartdb.c","src/cartdb.h","src/cpu.c","src/instance.c","src/internal.h","src/riot.c","src/sha1.c","src/sha1.h","src/tia.c","tests/abi/tests.cmake","tests/alloc.h","tests/api/v1.c","tests/api/v2.c","tests/built/build.cmake","tests/built/expected.txt","tests/built/tests.cmake","tests/bus/test_bus.c","tests/bus/tests.cmake","tests/cart/images.c","tests/cart/images.h","tests/cart/test_cart.c","tests/cart/test_cartdb.c","tests/cart/test_sha1.c","tests/cart/tests.cmake","tests/cart/write_images.c","tests/cmake/abi.cmake","tests/cmake/budgets.cmake","tests/cmake/fetch_vectors.cmake","tests/cmake/firmware_scan.cmake","tests/cmake/no_float.cmake","tests/cmake/no_test_entries.cmake","tests/cmake/rom_hash_coverage.cmake","tests/cmake/vectors_pin.cmake","tests/cmake/vectors_sample_match.cmake","tests/core/test_core.c","tests/cpu/decimal/clark-6502.bin","tests/cpu/decimal/clark-65c02.bin","tests/cpu/decimal/clark.s","tests/cpu/full.cmake","tests/cpu/targets.cmake","tests/cpu/test_cpu.c","tests/cpu/test_reader.c","tests/cpu/test_vectors.c","tests/cpu/tests.cmake","tests/cpu/vector_bus.c","tests/cpu/vector_bus.h","tests/embed/CMakeLists.txt","tests/fuzz/loader.c","tests/fuzz/replay_main.h","tests/fuzz/seeds.c","tests/fuzz/seeds.h","tests/fuzz/tests.cmake","tests/header/header.c","tests/header/header.cpp","tests/hygiene/tests.cmake","tests/inventory/ci.txt","tests/inventory/fuzz.txt","tests/inventory/hygiene.txt","tests/inventory/nofp.txt","tests/inventory/vectors-full.txt","tests/inventory/wasm32.txt","tests/release/firmware.txt","tests/release/tests.cmake","tests/riot/test_riot.c","tests/riot/tests.cmake","tests/roms/manifest.txt","tests/runner/test_runner.c","tests/runner/tests.cmake","tests/tests.cmake","tests/vectors/6502-sample.n65v","tests/vectors/6502-v1.sha256","tests/vectors/fixtures/02.json","tests/vectors/fixtures/README.md","tests/vectors/fixtures/a9-bad-a.json","tests/vectors/fixtures/a9.json","tests/vectors/n65v.c","tests/vectors/n65v.h","tests/vectors/sst_json.c","tests/vectors/sst_json.h","tests/vectors/vecconv.c","tools/cartdb.cmake"]
covered_digest: "v3:sha256:a7c39c8b575dc053246e2f7d8288b0ab1760691a68bbcabd6686fb98a0d81771"
behavior_unverified: 0
overrides_applied: 0
re_verification:
  previous_status: passed
  previous_score: 5/5
  reason: "stale after code-review fix round 2 (62b6bdc) changed src/bus.c, src/internal.h, docs/hardware/bus.md and tests/bus/test_bus.c"
  gaps_closed: []
  gaps_remaining: []
  regressions: []
---

# Phase 2: The CPU matches the public vectors — Verification Report

**Phase goal:** The SALLY CPU matches the public 6502 vectors on every opcode and every bus cycle, the bus applies the 7800's slow cycles, HALT and RDY, RIOT and INPTCTRL behave as documented, and the runner probes and loads cartridge images safely; merging publishes the release.
**Verified:** 2026-10-19T20:24:57Z, branch `phase/02-the-cpu-matches-the-public-vectors` at d473207. The last code commit is 62b6bdc; everything after it touches only `.planning/`. The branch is 22 commits ahead of its remote, so nothing from either fix round has run on hosted CI yet.
**Status:** passed
**Re-verification:** Yes. The report of 2026-10-19T19:37:48Z (passed, 5/5, at 1350b30) went stale when fix round 2 changed four covered files. That report had no gaps, so this run checks the four changed files in full and re-runs every criterion's gate as a regression check.

## What changed since the last verification

`git diff 1350b30..HEAD -- . ':!.planning'` touches 4 files (+49/−24), all from 62b6bdc (WR-01):

- `src/bus.c`: `muhtari__bus_request_halt` and `muhtari__bus_raise_nmi` now return `int`. Each returns 0 on its early-return path, which is unchanged (HALT: `halt_gate_writes < 2u || halt_stage != HALT_NONE`; NMI: `!men(m) || nmi_pending != 0u`), and returns 1 after it records the request and calls `rearm`. No state change was added or removed, so timing behaviour is the same as at 1350b30.
- `src/internal.h`: the two prototypes now return `int`, and the comment says "Each returns 1 when it takes the request and 0 when it ignores it".
- `docs/hardware/bus.md`: the HALT and NMI paragraphs describe the return value.
- `tests/bus/test_bus.c`: an `expect_taken` helper, plus return-value assertions in `halt.gate_two_writes` (gate shut gives 0), `halt.one_window_at_a_time` (first request 1, second 0 both after and before the fall), `nmi.latched_in_cycle` (MEN clear gives 0) and `nmi.one_edge_at_a_time` (first edge 1, second 0).

The only callers are tests (`grep -rn "bus_request_halt\|bus_raise_nmi" src tests runner host libretro include`). No library or runner code consumes the return yet.

## Commands run by the verifier (this run)

| Command | Exit | Result |
| ------- | ---- | ------ |
| `cmake --workflow --preset ci` | 0 | 408 of 408, then `ci-release` 6 of 6. Labels: vectors 266, sally 22, riot 16, cart 19, built 5, fuzz 2, runner 30, portable 372 |
| `cmake --workflow --preset hygiene` | 0 | 24 of 24 |
| `cmake --build build/vectors-full && ctest --test-dir build/vectors-full -L vectors-full -E fetch -j8` | 0 | 257 of 257: the full SingleStepTests set at its pin, rebuilt from HEAD |
| `ctest --test-dir build/ci -L sally` / `-L riot` / `-L runner` | 0 / 0 / 0 | 22 of 22, 16 of 16, 30 of 30 |
| Mutation check in the scratchpad: copy of `src/`, the HALT early return changed to `return 1`, linked with `tests/bus/test_bus.c` | 1, 1 | `halt.gate_two_writes` and `halt.one_window_at_a_time` fail against the mutant |
| Same, with the NMI early return changed to `return 1` | 1, 1 | `nmi.one_edge_at_a_time` and `nmi.latched_in_cycle` fail against the mutant |
| Same harness, unmutated sources | 0 ×4 | all four cases pass, so the new assertions catch a wrong return |
| Scratch probe of WR-05: power on, INPTCTRL `$07`, `raise_nmi(m, 6)`, 4 reads, `raise_nmi(m, ticks + 2)`, 4 reads | 0 | `first=1 phi2=2 second=1 phi2_after=2 pending=0`: the review's finding reproduces (see Anti-patterns) |
| `grep -nE "TBD\|FIXME\|XXX"` over the 4 changed files | 1 (no match) | no debt markers |
| `scripts/check-title.sh "feat: the CPU matches the public 6502 vectors, with the 7800 bus, RIOT and cartridge probing"` | 0 | |
| `gh run list --branch phase/02-the-cpu-matches-the-public-vectors --limit 3` | 0 | the latest hosted runs are still on the earlier head (Release 39199084187, Nightly 39199084192, both success). No hosted run covers either fix round yet |
| `node ~/.claude/gsd-core/bin/gsd-tools.cjs query verification.fingerprint <phase_dir> <107 files>` | 0 | 139 covered files (the 107 plus the 32 plan and summary files), digest in the frontmatter |

## Goal Achievement

### Observable truths (roadmap success criteria)

| # | Truth | Status | Evidence |
| - | ----- | ------ | -------- |
| 1 | `ctest -L vectors` matches every opcode of the committed sample on final state and every bus cycle, decimal mode included, on every pull request; `ctest -L vectors-full` matches the full set at its pin nightly | VERIFIED | This run: the vectors label passes 266 of 266 in the ci workflow, including all 256 `cpu.vectors.XX`. The rebuilt vectors-full tree passes 257 of 257. `src/cpu.c`, the vector harness, the sample, the pin and nightly.yml are not in the fix diff |
| 2 | `ctest -L sally` passes: 4- and 6-tick cycles, HALT only after two INPTCTRL writes, stopping reads and writes, RDY holding only reads with a read-modify-write's two writes completing, NMI in the last cycle taken after the next instruction | VERIFIED | 22 of 22 sally tests pass. The fix only adds return values; the HALT gate check, `rearm`, `events`, `hold` and `latch_nmi` are byte-for-byte the same as at 1350b30. The new return assertions are shown to bite by the mutation check above |
| 3 | `ctest -L riot` passes: timer through HALT and WSYNC per interval, flag, SWCHA/SWCHB with DDRs and power-on values, RIOT RAM $0480–$04FF, INPTCTRL lock, open bus | VERIFIED | 16 of 16 riot tests pass, including `riot.timer.counts_through_halt`, which calls `muhtari__bus_request_halt` and ignores the return (valid C; the ci build uses the project warning set and passes). `src/riot.c` and `tests/riot/` are not in the fix diff |
| 4 | `muhtari-run --probe FILE` prints A78 fields, board, region, peripherals and four hashes; header-stripped identity through the hash list; distinct statuses and messages for 2600, unsupported board, malformed, truncated and oversized; counting-allocator test and `fuzz.regress.loader` pass on every platform; nightly loader fuzz | VERIFIED | `runner/`, `src/cart.c`, `src/cartdb.c`, `host/` and `tests/cart/` are not in the fix diff. runner 30 of 30, cart 19 of 19 and fuzz 2 of 2 pass in this run. The direct probe observations recorded in the two earlier reports rest on code neither fix round touched. The be32, wasm32 and nofp lanes run again on hosted CI for the ship pull request before the merge |
| 5 | Merging the phase's pull request publishes a release through the Phase 1 pipeline with no manual step | VERIFIED (pre-merge evidence; confirmed at ship) | Only a merge can show this, so `/gsd-ship` confirms it after the merge (rule 9), not UAT. Evidence now: the ship title passes `scripts/check-title.sh` (exit 0), the release workflow, release-please config and `scripts/publish.sh` are not in the fix diff, and the ci workflow's release step passes 6 of 6 |

**Score:** 5/5 truths verified, 0 present but behavior-unverified.

### Plan must-haves (02-01 to 02-16)

Unchanged from the last two reports. Fix round 2 adds a return value that no plan truth contradicts, and every test named for a plan truth still passes in this run's ci and hygiene workflows.

### Required artifacts

| Artifact | Status | Details |
| -------- | ------ | ------- |
| `src/bus.c` | VERIFIED | Both functions return 0 on the unchanged early-return path and 1 after the unchanged state update |
| `src/internal.h` | VERIFIED | The prototypes match the definitions; the comment matches the code for the request itself (see WR-05 for what it does not say about the later latch) |
| `docs/hardware/bus.md` | VERIFIED, with deferred wording findings | I read the HALT and NMI paragraphs against `bus.c`. They match what the request does; WR-05 and IN-04 note that they over-promise what a 1 or a 0 tells MARIA |
| `tests/bus/test_bus.c` | VERIFIED | New assertions are real, registered under `sally portable`, and fail against mutants |
| everything else in covered_files | VERIFIED (regression) | Not in the fix diff, and the ci, hygiene and vectors-full runs pass |

### Key links

| From | To | Status | Details |
| ---- | -- | ------ | ------- |
| muhtari-test-vectors | src/cpu.c objects | WIRED | Rebuilt from HEAD; 257 full-set tests pass |
| bus.c HALT gate | INPTCTRL writes and halt_stage | WIRED | Unchanged; `halt_gate_writes` counted in `inptctrl_write`, `halt_stage` back to HALT_NONE in `events()` |
| bus.c NMI raise | latch_nmi | WIRED | Unchanged; `nmi_pending` set in `raise_nmi`, cleared on every latch |
| request_halt / raise_nmi return | caller | WIRED in tests only | Only the bus tests read it. The production caller is MARIA, which phase 3 adds |
| muhtari-run --probe | muhtari_probe | WIRED | Unchanged; runner 30 of 30 |
| nightly.yml | vectors-full and fuzz presets | WIRED | Unchanged; the local vectors-full preset passes |

### Data-flow trace

Not applicable. The phase draws no UI, and probe output comes from `muhtari_probe` over the file's bytes, which neither fix round touched.

### Behavioral spot-checks

| Behavior | Command | Result | Status |
| -------- | ------- | ------ | ------ |
| A request or edge that is ignored returns 0, a taken one 1 | bus tests against the unmutated and mutated sources | pass on the real code, fail on both mutants | PASS |
| One HALT window and one NMI edge at a time still hold | `ctest` bus.halt.one_window_at_a_time, bus.nmi.one_edge_at_a_time | Passed | PASS |
| Timer counts through HALT | `ctest` riot.timer.counts_through_halt | Passed | PASS |
| Full opcode set | `ctest -L vectors-full` | 257 of 257 | PASS |

### Probe execution

There are no `scripts/*/tests/probe-*.sh` scripts in this repository, and the phase declares none.

### Requirements coverage

| Requirement | Source plans | Description | Status | Evidence |
| ----------- | ------------ | ----------- | ------ | -------- |
| CPU-01 | 5 plans | Committed sample, every opcode, state and every cycle, decimal mode, unstable constant | SATISFIED | Criterion 1 |
| CPU-02 | 4 plans | Full set at its pin, nightly | SATISFIED | Criterion 1: 257 of 257 at HEAD |
| CPU-03 | 3 plans | `ctest -L sally` timing, HALT, RDY, NMI | SATISFIED | Criterion 2: 22 of 22 |
| CPU-04 | 3 plans | `ctest -L riot`, INPTCTRL, open bus | SATISFIED | Criterion 3: 16 of 16 |
| CPU-05 | 9 plans | `--probe`, hash list, refusal classes, counting allocator, fuzz replay, nightly fuzz | SATISFIED | Criterion 4 |

The plans' `requirements:` fields list only CPU-01 to CPU-05, and REQUIREMENTS.md maps exactly those five to Phase 2 (lines 176 to 180, all Complete). No requirement is orphaned.

### Anti-patterns found

| File | Line | Pattern | Severity | Impact |
| ---- | ---- | ------- | -------- | ------ |
| 4 changed files | - | TBD, FIXME or XXX | none found | - |
| `src/internal.h`, `docs/hardware/bus.md` | internal.h:286-289, bus.md:154-157 | WR-05: `raise_nmi` returns 1 for an edge that `latch_nmi` later drops, when an earlier edge is latched but not taken or MEN is cleared before the latch. Reproduced in this run | Warning (deferred) | No criterion fails. The latch behaviour itself matches the doc's sentence that "a latched edge the CPU has not taken keeps its `nmi_phi2` against a later one", and the sally tests pin it. Only the return-value contract, which no production code reads yet, over-promises. Deferred under rule 7 (both fix rounds spent) to the pending todo `2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md`, which names WR-05 and waits for phase 3's MARIA, the first caller |
| `docs/hardware/bus.md` | 141-143 | IN-04: the HALT paragraph names one reason for a 0 return; the shut gate is the other | Info (deferred) | Same todo |
| `tests/bus/test_bus.c` | 288-296, 496-509 | IN-05: no test asserts a 1 for a request after a window ends or an edge after a latch | Info (seed, test code under rule 7) | Same todo |
| 02-16-SUMMARY.md | coverage entry for criterion 5 | `status: pending-ship` | Info | Carried over. Set it to `pass` at ship once the release run is confirmed |
| hosted CI | - | neither fix round has run on hosted CI | Info | Not a gap. This run checked them locally (ci, hygiene, vectors-full). The ship pull request's required CI runs every lane, including be32, wasm32, nofp and asan, on the head before the merge; `/gsd-ship` confirms a CI run exists on the head commit (rule 9) |

### Human verification required

None. I ran every check above. The merge is the owner's consent action, and the release it publishes is confirmed at `/gsd-ship` (rule 9), not in UAT.

### Gaps summary

No gaps. Fix round 2 adds only a return value to two internal bus entry points. Their state handling is unchanged, the new assertions catch a wrong return, and the full vector set, the 22 sally tests, the 16 riot tests and the cart, runner and fuzz tests all pass at HEAD. The third review's WR-05 is real (reproduced here). It is a wording and contract issue in an internal API with no production caller, it fails no success criterion, and it is tracked in a pending todo for phase 3's MARIA plan. What remains is ship-time confirmation: hosted CI on the pushed head and the release that the merge publishes.

---

_Verified: 2026-10-19T20:24:57Z_
_Verifier: Claude (gsd-verifier)_
