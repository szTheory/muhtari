---
phase: 03-games-play-in-retroarch
verified: 2026-10-20T08:05:00Z
status: gaps_found
score: 129/129 truths verified (5 roadmap criteria, 124 plan truths); 68/69 prohibitions held, 1 violated
covered_files:
  - ".github/workflows/ci.yml"
  - ".github/workflows/nightly.yml"
  - ".gitignore"
  - ".planning/phases/03-games-play-in-retroarch/03-01-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-01-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-02-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-02-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-03-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-03-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-04-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-04-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-05-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-05-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-06-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-06-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-07-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-07-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-08-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-08-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-09-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-09-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-10-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-10-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-11-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-11-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-12-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-12-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-13-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-13-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-14-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-14-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-15-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-15-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-16-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-16-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-17-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-17-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-18-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-18-SUMMARY.md"
  - ".planning/phases/03-games-play-in-retroarch/03-19-PLAN.md"
  - ".planning/phases/03-games-play-in-retroarch/03-19-SUMMARY.md"
  - "CMakeLists.txt"
  - "CMakePresets.json"
  - "README.md"
  - "docs/compatibility.md"
  - "docs/embedding.md"
  - "docs/emulation-tests.md"
  - "docs/hardware/bus.md"
  - "docs/hardware/cartridge.md"
  - "docs/hardware/controllers.md"
  - "docs/hardware/maria.md"
  - "docs/hardware/riot.md"
  - "docs/hardware/sally.md"
  - "docs/libretro.md"
  - "docs/runner.md"
  - "docs/testing.md"
  - "host/status_text.c"
  - "include/muhtari.h"
  - "libretro/CMakeLists.txt"
  - "libretro/libretro.c"
  - "libretro/muhtari_libretro.info.in"
  - "pins.txt"
  - "runner/exit_status.h"
  - "runner/main.c"
  - "runner/movie.c"
  - "runner/movie.h"
  - "src/boot.c"
  - "src/bus.c"
  - "src/cart.c"
  - "src/cpu.c"
  - "src/frame.c"
  - "src/instance.c"
  - "src/internal.h"
  - "src/maria.c"
  - "src/ports.c"
  - "src/riot.c"
  - "src/tia.c"
  - "tests/abi/tests.cmake"
  - "tests/api/v2.c"
  - "tests/api/v3.c"
  - "tests/boot/test_boot.c"
  - "tests/boot/tests.cmake"
  - "tests/built/chain.c"
  - "tests/built/coverage.c"
  - "tests/built/coverage.txt"
  - "tests/built/expected.txt"
  - "tests/built/frames.cmake"
  - "tests/built/openbios.cmake"
  - "tests/built/tests.cmake"
  - "tests/bus/test_bus.c"
  - "tests/cart/test_cart.c"
  - "tests/cart/tests.cmake"
  - "tests/cart/write_images.c"
  - "tests/cmake/ci_required.cmake"
  - "tests/cmake/compat_ratchet.cmake"
  - "tests/cmake/firmware_scan.cmake"
  - "tests/cmake/info_claims.cmake"
  - "tests/cmake/perf_compare.cmake"
  - "tests/compat/allow.txt"
  - "tests/compat/ledger.csv"
  - "tests/compat/library.cmake"
  - "tests/compat/pin.cmake"
  - "tests/compat/predicate.c"
  - "tests/compat/targets.cmake"
  - "tests/compat/tests.cmake"
  - "tests/core/test_core.c"
  - "tests/core/tests.cmake"
  - "tests/diag/assemble.cmake"
  - "tests/diag/bank6.a78"
  - "tests/diag/bank6.cfg"
  - "tests/diag/bank6.s"
  - "tests/diag/cpu.a78"
  - "tests/diag/cpu.cfg"
  - "tests/diag/cpu.s"
  - "tests/diag/dli.a78"
  - "tests/diag/dli.cfg"
  - "tests/diag/dli.s"
  - "tests/diag/dma.a78"
  - "tests/diag/dma.cfg"
  - "tests/diag/dma.s"
  - "tests/diag/driver.c"
  - "tests/diag/expected.csv"
  - "tests/diag/lib.s"
  - "tests/diag/linear16.a78"
  - "tests/diag/linear16.cfg"
  - "tests/diag/linear16.s"
  - "tests/diag/modes.a78"
  - "tests/diag/modes.cfg"
  - "tests/diag/modes.s"
  - "tests/diag/sg144.a78"
  - "tests/diag/sg144.cfg"
  - "tests/diag/sg144.s"
  - "tests/diag/stats.csv"
  - "tests/diag/tests.cmake"
  - "tests/frame/test_frame.c"
  - "tests/frame/tests.cmake"
  - "tests/fuzz/loader.c"
  - "tests/fuzz/run.c"
  - "tests/fuzz/run_cart.h"
  - "tests/fuzz/run_seeds.c"
  - "tests/fuzz/tests.cmake"
  - "tests/header/header.c"
  - "tests/header/header.cpp"
  - "tests/hygiene/tests.cmake"
  - "tests/input/test_input.c"
  - "tests/input/tests.cmake"
  - "tests/inventory/ci.txt"
  - "tests/inventory/fuzz.txt"
  - "tests/inventory/library.txt"
  - "tests/inventory/nofp.txt"
  - "tests/inventory/wasm32.txt"
  - "tests/libretro/host.c"
  - "tests/libretro/rcheevos.c"
  - "tests/libretro/tests.cmake"
  - "tests/maria/test_maria.c"
  - "tests/maria/tests.cmake"
  - "tests/perf/tests.cmake"
  - "tests/policy/tests.cmake"
  - "tests/prop.h"
  - "tests/prop/test_prop.c"
  - "tests/prop/tests.cmake"
  - "tests/release/firmware.txt"
  - "tests/retroarch/run_retroarch.cmake"
  - "tests/riot/test_riot.c"
  - "tests/roms/manifest.txt"
  - "tests/runner/boot_to_play.c"
  - "tests/runner/boot_to_play.cmake"
  - "tests/runner/png_read.c"
  - "tests/runner/png_read.h"
  - "tests/runner/test_movie.c"
  - "tests/runner/test_png.c"
  - "tests/runner/tests.cmake"
  - "tests/tests.cmake"
  - "tools/compat_doc.cmake"
covered_digest: "v3:sha256:8c5cdcaba22a87d3448806a9466ef0785dadd0c9a533c1ec088c3da955563721"
behavior_unverified: 0
overrides_applied: 0
gaps:
  - truth: "Prohibition (plans 03-01, 03-04, 03-19 and the CLAUDE.md Style rule): MUST NOT put planning IDs in code or comments"
    status: failed
    reason: "src/ports.c:59 reads 'a whole frame sees one input (DEC.25).' DEC.25 is a decision number from .planning/preparation/DECISIONS.md, added in this phase by fb79feb (plan 03-04). No automated check caught it, though plan 03-01 marks this prohibition verification: automated. The phase goal is not affected; this is a one-line comment fix, already open as review finding WR-05."
    artifacts:
      - path: "src/ports.c"
        issue: "Line 59 cites the planning ID DEC.25 in place of a behavioural source"
    missing:
      - "Replace '(DEC.25)' with the behavioural source, e.g. '(docs/embedding.md, Input)'. Fixing WR-05 in the code-review fix round closes this gap, and no new plan is needed"
---

# Phase 3: Games play in RetroArch Verification Report

**Phase goal:** Linear and SuperGame cartridges render with exact MARIA DMA timing, boot by the fast path or OpenBIOS,
take joystick and console-switch input, and play in RetroArch with frame hashes identical on every platform; merging
publishes the release.
**Verified:** 2026-10-20T08:05:00Z
**Status:** gaps_found. The goal is achieved. One must-not prohibition is violated by a one-line comment (WR-05).
**Re-verification:** No. This is the initial verification.

## Goal Achievement

### Observable Truths (roadmap success criteria)

| # | Truth | Status | Evidence |
|---|-------|--------|----------|
| 1 | `ctest -L basic` builds the pinned samples, checks their SHA-256 and matches every pinned 600-frame chain on every platform. The samples and the diagnostics cover linear 16/32/48 KB, SuperGame (plain, RAM, bank 6, 144 KB), RM0/RM2/RM3, write mode, kangaroo, holey 8/16, indirect 1/2-byte and DLI. `ctest -L diag` reports DMA costs, the overrun abort, the DLI tick, the WSYNC release and the slow-access cost equal to A7800-MARIA/SYSTEM. | ✓ VERIFIED | Local: `ctest -L 'basic\|diag\|input\|prop\|bios\|title'` exit 0, 105/105. `basic.coverage -V` shows every claim PASS from the core's observed feature mask (e.g. `diag/modes kangaroo ... mask 071b PASS`, `diag/sg144 board-sg144 ... rom 147456 PASS`). `diag.dma -V`: 27 records PASS, each with "stats agree", including the overrun records over_11..over_16 (13 objects kept at 14 and 16). `diag.cpu`: 4 slow-access rows PASS. `diag.dli`: 11 rows PASS (zone DLI, wsync_1/3, rdy_store, MSTAT edges). Hosted: ci.yml run 39383824898 on b727c42 succeeded on all six build legs plus be32, and be32 runs the `portable` label (which includes `basic.*`) with a junit inventory check. Only `.planning/` files changed between b727c42 and HEAD 7b623b7. The expected.csv rows (b3e7013) were committed before the ROM ran (1ae7f27); I confirmed this with `git merge-base --is-ancestor`. |
| 2 | Every sample reaches its title-screen predicate within 300 frames by the fast boot with no BIOS and by `--boot bios` through the CC0 OpenBIOS built in CI, counted from the hand-off. | ✓ VERIFIED (scoped, see note) | `built.openbios` checks digest `46b5bc2b…` and passes. Spot check: `muhtari-run --frames 120 --hash-frame 120 build/ci/built/simple/simple.bas.a78` gives `07c13a65…`, which equals the ledger row. `--boot bios --bios openbios.bin --frames 640` gives `handoff_frame 323`, and frame 640 has the same hash. The hosted `library` job (success in run 39383824898) evaluates all 39 runnable pr-tier rows on both paths. Note: 6 of the 34 samples use the weak `boots@F` fallback that CONTEXT D-16 permits (5 use rand seeded from INTIM, plus hiscoretable). 2 are `skipped` (backuptest, languagetest), because their own sources draw nothing: I read `backuptest.bas` (only a `backup` directive) and `languagetest.bas` ("A useless program that does nothing"). Both are still pinned by their 600-frame `basic.*` chains. |
| 3 | `muhtari-run --movie FILE` drives both stick types on both ports and every console switch. `ctest -L input` matches A7800-CARTRIDGES-AND-PERIPHERALS and counts lag. `runner.boot_to_play` plays one game from boot and fails its negative controls. | ✓ VERIFIED | `ctest -L input` passes 12/12. The real runner runs `runner.movie.ports` (ProLine and joystick on port 1 and port 2, all four distinct from the empty movie), `runner.movie.switches` (R, S, P and both difficulties distinct) and `runner.movie.lag`, and all pass. `runner.boot_to_play -V`: "mask 0: the first difference is at frame 122 (F=121); the difference moved 17 px; sign flipped". The negative controls no_input, port2, static_mutant, late_mutant and mirrored each fail with their expected mask (`expect_fail` regex matched). |
| 4 | The libretro core's memory descriptors pass rcheevos' own resolver. The hosted RetroArch job boots a diagnostic ROM, and its screenshot equals the runner's frame. | ✓ VERIFIED | `tests/libretro/rcheevos.c:166` calls the real `rc_libretro_memory_init(..., RC_CONSOLE_ATARI_7800)`, built from the fetched rcheevos sources (only `rc_hash` is stubbed). `libretro.rcheevos` passes. In run 39383824898 the `retroarch` job's steps "RetroArch runs the dma diagnostic", "The screenshots equal the runner's frames" (pixel compare of `dma-runner` against `dma-shot`, plus a grep of the frame-120 hash against the `diag/dma` ledger row) and "The compare's negative control" all succeeded. |
| 5 | `fuzz.regress.run` and `ctest -L prop` pass on every platform with their nightly runs. The `library` preset regenerates `docs/compatibility.md` in CI. Merging publishes a release with no manual step. | ✓ VERIFIED (the merge is confirmed at ship) | `fuzz.regress.run` and `prop.*` carry the `portable` label in `tests/inventory/ci.txt`, so they run on all legs and be32. Nightly run 39383824826 on b727c42: the `prop` job ran "10,000 cases per property" with success. The `fuzz` job (fuzz preset, whose inventory holds fuzz.run and fuzz.regress.run, `-max_len=49152 -timeout=2`) succeeded. The `library` job runs `cmake --workflow --preset library` with `fetch-depth: 0` and succeeded. `policy.compat_doc_fresh` passes. Publishing is confirmed at ship per rule 9: the release-please pipeline published 0.1.0 (PR #8) and 0.2.0 (PR #11) with no manual step, and the ship title passes `scripts/check-title.sh` (exit 0). |

**Score:** 5/5 roadmap criteria and 124/124 plan truths verified, with 0 present but behaviour-unverified. 68 of the 69 plan prohibitions hold and 1 is violated (see Gaps).

**Plan truths:** each of the 124 plan-frontmatter truths maps to a named test in `build/ci`. I confirmed that each test exists (`ctest -N`) and that it passes, through the labelled local run, a named-test run, or the green hosted run 39383824898 or 39383824826. Hosted-only truths (job-time measurements, timeouts with run IDs, the probe PR) were checked against the workflow files and `gh`. PR #12 "ci: probe the phase workflows" is CLOSED unmerged. Every behaviour-dependent truth has a passing behavioural test: `prop.input_at_vblank` covers the input latch, `core.load.keeps_previous` the failed-load rollback, `frame.hold_split_wsync_across_dma` the hold split, and `bios.k_first_fetch` the hand-off ordering.

### Required Artifacts

`gsd query verify.artifacts` was run over all 19 plans: 52 of 53 passed. The one miss is plan 03-03's `src/maria.c` `contains: dli_queued`. It is a case mismatch: the code uses `MUHTARI__LINE_DLI_QUEUED` and `MUHTARI__FEAT_DLI_QUEUED`, and `src/maria.c:628-631` stores `muhtari__bus_raise_nmi`'s return value. `frame.dli_queued` passes. The artifact is ✓ VERIFIED.

| Artifact | Status | Details |
|----------|--------|---------|
| `src/maria.c`, `src/frame.c`, `src/bus.c`, `src/boot.c`, `src/cart.c`, `src/ports.c` | ✓ VERIFIED | Substantive, compiled in `muhtari_objs`, exercised by maria/frame/diag/basic tests |
| `include/muhtari.h` (API 3), `tests/api/v3.c` | ✓ VERIFIED | `abi.api_v3` passes |
| `runner/main.c`, `runner/movie.c` | ✓ VERIFIED | Spot checks above; `runner.movie_parse` and `runner.movie.*` pass |
| `libretro/libretro.c` | ✓ VERIFIED | `libretro.host.*` (11) and `libretro.rcheevos` pass; hosted RetroArch is green |
| `tests/diag/*.a78` (7) | ✓ VERIFIED | `diag.rebuild` rebuilds them all against the manifest digests |
| `tests/compat/ledger.csv`, `docs/compatibility.md`, `tools/compat_doc.cmake`, `tests/cmake/compat_ratchet.cmake` | ✓ VERIFIED | 42 rows; the page was regenerated and matches |
| `.planning/seeds/SEED-007-milestone-tune-up.md` | ✓ VERIFIED | Records the gate-code tally (18.4 %), its commands and the owner cuts |

### Key Link Verification

`gsd query verify.key-links` was run over all 19 plans: 39 of 39 links verified, exit 0 for every plan.

### Data-Flow Trace (Level 4)

| Artifact | Data | Source | Real data | Status |
|----------|------|--------|-----------|--------|
| runner frame hash | caller rows | MARIA span render from line RAM filled by DMA from cart ROM | yes: the simple hash differs from the no-content test card, and the BIOS path converges on the same title hash | ✓ FLOWING |
| diag mailbox | $2200 records | the ROM's INTIM measurements, read through `muhtari__peek` | yes: 27 dma records, cross-checked against per-line stats | ✓ FLOWING |
| libretro screenshot | `video_cb` | the same core frame | pixel-equal to the runner's (hosted) | ✓ FLOWING |

### Behavioral Spot-Checks

| Behavior | Command | Result | Status |
|----------|---------|--------|--------|
| Fast boot renders a sample, deterministically | `muhtari-run --frames 120 --hash-frame 120 build/ci/built/simple/simple.bas.a78` (twice) | `07c13a65…` both times, equal to the ledger, exit 0 | ✓ PASS |
| BIOS path hand-off | `muhtari-run --boot bios --bios build/ci/built/openbios/openbios.bin --frames 640 --hash-frame 640 simple.bas.a78` | `handoff_frame 323`, frame 640 = `07c13a65…`, exit 0 | ✓ PASS |
| Phase labels | `ctest -L 'basic\|diag\|input\|prop\|bios\|title'` in build/ci | 105/105, exit 0 | ✓ PASS |
| Runner, libretro, fuzz, policy, load, API tests | `ctest -R '^(runner\.boot_to_play\|runner\.movie\|libretro\.\|fuzz\.regress\.run\|policy\.compat_doc\|core\.load\|core\.no_alloc\|abi\.api_v3\|built\.openbios\|release\.firmware)'` | 51/51, exit 0 | ✓ PASS |
| Full suite | `cmake --workflow --preset ci` (orchestrator, HEAD 7b623b7) | exit 0, 587/587 plus 6/6 release | ✓ PASS |
| Hosted | `gh run view 39383824898` / `39383824826` | CI success with 19 jobs; Nightly success (report job skipped by design) | ✓ PASS |

### Probe Execution

No `scripts/*/tests/probe-*.sh` exists, and no plan declares one. The "probe" in this phase is the probe pull request (#12), which is closed unmerged.

### Requirements Coverage

| Requirement | Source plans | Status | Evidence |
|-------------|--------------|--------|----------|
| GAME-01 | 03-03, 03-05, 03-08, 03-09, 03-19 | ✓ SATISFIED | Criterion 1: 34 sample rows in `tests/built/expected.txt`, plus `basic.coverage` |
| GAME-02 | 03-01, 03-02, 03-03, 03-06, 03-07, 03-19 | ✓ SATISFIED | Criterion 1: `diag.*` with the stats cross-check |
| GAME-03 | 03-02, 03-03, 03-04, 03-12, 03-13, 03-14, 03-18, 03-19 | ✓ SATISFIED (scoped) | Criterion 2: the 2 skipped samples draw nothing by design, and 6 use the D-16 fallback |
| GAME-04 | 03-04, 03-10, 03-11, 03-13, 03-19 | ✓ SATISFIED | Criterion 3 |
| GAME-05 | 03-04, 03-15, 03-16, 03-19 | ✓ SATISFIED | Criteria 3 and 4 |
| GAME-06 | 03-14, 03-17, 03-18, 03-19 | ✓ SATISFIED | Criterion 5. The owner's personal-dump run stays optional and is not a verification item. |

No requirement is orphaned. REQUIREMENTS.md maps exactly GAME-01 to GAME-06 to Phase 3, and every one is claimed by plans and by SUMMARY `requirements-completed`.

### Anti-Patterns Found

All 136 non-planning files changed since merge-base d698364 were scanned. None has a TBD, FIXME or XXX marker, and none has a TODO, HACK, PLACEHOLDER or "not yet implemented".

| File | Line | Pattern | Severity | Impact |
|------|------|---------|----------|--------|
| `src/ports.c` | 59 | Planning ID `DEC.25` in a code comment | ✗ Prohibition violated (gap) | Breaks the CLAUDE.md Style rule and the plan prohibitions. No functional impact. |
| `src/cart.c` | 625-640 | The headerless hash-match path skips `cart_fits` (WR-01) | ⚠️ Warning | Memory safety rests on the generated table. Only an inconsistent `cartdb.c` could reach it, because a hash match implies the dump's real size. |
| `include/muhtari.h` | 69, 211 | Contract comments contradict behaviour (WR-02) | ⚠️ Warning | Embedders could mis-size audio or cache the CART_RAM pointer across a reload |
| `libretro/libretro.c` | 491-498 | No `video_cb` on a failed frame (WR-03) | ⚠️ Warning | Error path only, e.g. BIOS rejecting a cart. Normal play is unaffected. |
| `libretro/libretro.c` | 554-579 | BIOS lookup stops at the first fopen-able file (WR-04) | ⚠️ Warning | Optional BIOS path only |

The info findings IN-01 to IN-05 are recorded in 03-REVIEW.md. None of them affects the goal.

**Rule 7 gate-code budget:** narrow check code is 18.4 % of `src/`, over the tenth. Plan 03-19's truth makes this an owner scoping item for ship, recorded in SEED-007 with commands and numbers, and this phase added only the ratchet and the doc generator (verified with `git diff --name-status d698364 HEAD -- tests/cmake scripts tools`). It is ⚠️ a warning for ship, not a verification item.

### Human Verification Required

None. Under rule 6 the agent made every judgement call here: the docs read against the code, and RetroArch was checked by the hosted pixel compare. Two items are left for ship (rule 9), and neither is UAT: the merge that publishes 0.3.0, and the owner's decision on the rule 7 budget in SEED-007.

### Gaps Summary

The phase goal is achieved. Every roadmap criterion and every GAME requirement has passing local and hosted evidence on code identical to HEAD.

One must-not prohibition is violated. `src/ports.c:59` cites the planning ID `DEC.25` in a comment. Code review found it as WR-05, and it is still open. The cheapest closure is the WR-05 fix in the code-review fix round, a one-line comment change, followed by `execute-phase` re-verification before ship. A gap re-plan is not needed. Fixing WR-01 to WR-04 in the same round is advisable but does not gate the goal.

---

_Verified: 2026-10-20T08:05:00Z_
_Verifier: Claude (gsd-verifier)_
