---
phase: 03-games-play-in-retroarch
plan: 13
subsystem: core
tags: [bios, openbios, boot, reset, inptctrl, runner, movies]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "The built_openbios fixture (plan 03-12), the movie reader and runner options (plans 03-10 and 03-11), and muhtari_load's BIOS validation and copy (plan 03-04)"
provides:
  - "muhtari_load with a BIOS boots through it: power-on at line 0, t 0 with CTRL $00, frame 1 of 117,174 ticks, then RES (S = $FD, I set, PC from $FFFC)"
  - "The BIOS mapped while EXT = 0: A15 = 1 with MEN = 1 (mirrored), A12 = 1 with MEN = 0 (top 4 KiB), for the CPU and for MARIA's DMA"
  - "The hand-off frame K: the frame of the first opcode fetch from cartridge space once EXT is 1, recorded once; internal muhtari__boot_handoff_frame"
  - "A sticky ERR_BIOS_REJECTED_CART when INPTCTRL locks with MEN = 0 before the hand-off"
  - "muhtari-run --boot fast|bios and --bios FILE, bios_sha256 on stderr, handoff_frame K, exit 77 on a rejected cartridge"
  - "A movie's boot and bios-sha256 keys applied, with the --boot conflict rule"
affects: [03-18, title predicates, libretro BIOS option, movies]

actuals:
  tokens: 11321
  tasks: 3
  commits: 4
plan_head_before: 1568892458e94eb67c0919150e13f6a16cc66d88
plan_head_after: 6bf9abdd6d79d536738b019a289a024c866afdff

tech-stack:
  added: []
  patterns:
    - "A CPU-set sync flag cleared by the next bus read marks opcode fetches for the bus"
    - "A boot-state struct in the instance holds what a boot observes (hand-off frame, rejection)"

key-files:
  created:
    - tests/boot/test_boot.c
    - tests/boot/tests.cmake
    - .planning/phases/03-games-play-in-retroarch/03-13-SUMMARY.md
  modified:
    - src/boot.c
    - src/bus.c
    - src/cpu.c
    - src/frame.c
    - src/instance.c
    - src/internal.h
    - src/maria.c
    - runner/main.c
    - runner/exit_status.h
    - tests/tests.cmake
    - tests/runner/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/bus.md
    - docs/hardware/sally.md
    - docs/embedding.md
    - docs/runner.md
    - docs/emulation-tests.md
    - .planning/WINDOWS.md

key-decisions:
  - "Input set before a BIOS boot's first frame is latched at power-on, so movie line 1 counts under boot bios; frame 1 starts at line 0, inside vertical blank but not at its start"
  - "The fast boot records K = 1; the runner prints handoff_frame only with --boot bios, and none when no hand-off came"
  - "The 2600-path refusal fires only with a BIOS loaded and before the hand-off; run_frame stops the rejecting frame and returns the status with no samples"
  - "Below $8000 the cartridge still answers while the BIOS is mapped, and MEN = 0 with EXT = 1 stays undriven at A12 = 1, as no 2600 cartridge runs"
  - "--bios without an effective boot bios is a usage error, so a BIOS file is never silently ignored"

patterns-established:
  - "BIOS-path tests take built_openbios and built_simple and run on the ci and wasm32 inventories only"

requirements-completed: [GAME-04]

coverage:
  - id: D1
    description: "OpenBIOS built from source boots simple through power-on and RES with the hand-off frame K = 323, inside [300, 340]"
    requirement: GAME-03
    verification:
      - kind: integration
        ref: "tests/boot/test_boot.c#handoff_simple (ctest -R '^bios\\.handoff_simple$', exit 0)"
        status: pass
      - kind: other
        ref: "https://github.com/szTheory/muhtari/actions/runs/39366092107 (conclusion success, head commit 88d6774)"
        status: pass
    human_judgment: false
  - id: D2
    description: "The K window's edges (300 and 340 pass, 299 and 341 fail) and only the first cartridge fetch after EXT rises counting"
    requirement: GAME-03
    verification:
      - kind: unit
        ref: "tests/boot/test_boot.c#k_window and #k_first_fetch (ctest -R '^bios\\.k_', exit 0)"
        status: pass
    human_judgment: false
  - id: D3
    description: "Pause held on frames 1-3 skips the splash with K = 3, below 30; OpenBIOS first reads SWCHB in frame 3"
    requirement: GAME-03
    verification:
      - kind: integration
        ref: "tests/boot/test_boot.c#splash_skip (ctest -R '^bios\\.splash_skip$', exit 0)"
        status: pass
    human_judgment: false
  - id: D4
    description: "The BIOS's 2600 path returns ERR_BIOS_REJECTED_CART from that frame on, with no samples, until power cycle or load; the runner exits 77, and 1 on an invalid BIOS"
    requirement: GAME-03
    verification:
      - kind: integration
        ref: "tests/boot/test_boot.c#rejects_cart (ctest -R '^bios\\.rejects_cart$', exit 0)"
        status: pass
      - kind: integration
        ref: "tests/runner/tests.cmake#bios.rejected and #bios.invalid (ctest -R '^runner\\.bios\\.', exit 0)"
        status: pass
    human_judgment: false
  - id: D5
    description: "A movie's boot bios boots through --bios FILE with its digest checked before frame 1; a missing --bios or a conflicting --boot exits 2, a digest mismatch exits 1 naming both"
    requirement: GAME-04
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#movie.boot_bios, movie.bios_mismatch, movie.boot_bios_no_file, movie.boot_conflict, usage.boot_bios_no_file (ctest -R '^runner\\.(movie\\.(boot|bios)|usage\\.boot)', exit 0)"
        status: pass
      - kind: other
        ref: "https://github.com/szTheory/muhtari/actions/runs/39366784570 (conclusion success, head commit 6bf9abd)"
        status: pass
    human_judgment: false
  - id: D6
    description: "Docs for the BIOS mapping, RES, booting with a BIOS, the runner options and output lines, the movie boot key and the bios tests; the exit table matches --help"
    requirement: GAME-04
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^(docs\\.|runner\\.exit\\.table)' (exit 0)"
        status: pass
    human_judgment: false

duration: 17min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 13: The BIOS path Summary

**`muhtari_load` with a BIOS now boots through it from power-on and RES. OpenBIOS, built in CI, hands `simple` off in frame 323. The runner gained `--boot` and `--bios`, prints `handoff_frame K`, and exits 77 when the BIOS takes its 2600 path. A movie's `boot bios` and `bios-sha256` keys now apply.**

## Performance

- **Duration:** 17 min
- **Started:** 2026-10-20T03:41:40Z
- **Completed:** 2026-10-20T03:58:09Z
- **Tasks:** 3
- **Files modified:** 21

## Accomplishments

- **Power-on and RES.** `muhtari__boot_bios` sets the power-on state: INPTCTRL $00, MARIA's registers 0 (so CTRL is $00), RAM and the RIOT at power-on, and the beam at line 0, t 0. Frame 1 has a deadline of 117,174 ticks. It then runs `muhtari__cpu_reset`: two reads at PC, three stack reads that take S from 0 to $FD, I set, and PC from $FFFC/$FFFD. With MEN = 0 that vector comes from the BIOS's top 4 KiB.
- **BIOS mapping.** While EXT = 0 and a BIOS is loaded, the BIOS answers every A15 = 1 address with MEN = 1, mirrored, and every A12 = 1 address with MEN = 0, from its top 4 KiB. MARIA's DMA reads it through the same rule. The Fuji splash renders correctly: frame 100's PNG was viewed.
- **Hand-off frame.** The CPU raises `bus.sync` for each opcode fetch. The first fetch from cartridge space with EXT = 1 records `m->boot.handoff_frame` once. The fast boot records 1.
- **K values.** For `simple`, **K = 323**. With Pause held on frames 1-3 (**N = 3**: OpenBIOS first reads SWCHB in frame 3, measured with the lag counter), **K = 3**.
- **2600-path refusal.** An INPTCTRL write that locks with MEN = 0, with a BIOS loaded and before the hand-off, sets a sticky `m->boot.rejected`. `muhtari_run_frame` stops that frame and returns `MUHTARI_STATUS_ERR_BIOS_REJECTED_CART` with `audio_count` 0, and keeps returning it until the next load or power cycle. OpenBIOS rejects the input cart (`$FFF8` = 0) in frame 2.
- **Runner.** `muhtari-run --boot fast|bios --bios FILE` prints `bios_sha256 <hex>` on stderr and `handoff_frame K` (or `none`) before `lag_frames`. It reads K through the internal `muhtari__boot_handoff_frame`; the reason is in a code comment and in docs/runner.md. A rejected cartridge exits 77 and an invalid BIOS exits 1.
- **Movie keys.** A movie's `boot` key wins over the command line. A conflicting `--boot` exits 2, and `boot bios` without `--bios` exits 2. The file's SHA-256 must equal `bios-sha256` before any frame runs, or the run exits 1 naming both digests.
- **Movie line 1.** Under a BIOS boot, frame 1 starts at power-on, on line 0. That line lies inside vertical blank but is not its start. Input set before frame 1 is latched at power-on, so movie line 1 counts under `boot bios`. Under the fast boot, line 1 still reaches no game. This is documented in docs/runner.md, Movies.
- **Broken window.** WINDOWS.md item 5 ("valid BIOS copied but machine fast-boots") is marked fixed.

## Task Commits

1. **Task 1 (tracer): OpenBIOS boots simple end to end:** `88d6774` (feat). For the tracer gate (interactive, end-of-phase, automated-only `<verify>`), the verify was re-run. The `ci` workflow exited 0, `bios.handoff_simple` passed with K = 323, and `scripts/hygiene.sh --tree` exited 0. Hosted run 39366092107 succeeded on all 18 jobs, including asan, be32, wasm32, nofp, perf and retroarch.
2. **Task 2 (TDD): contracts and the movie boot key:** RED `f7a9b41` (test), GREEN `8d3d733` (feat).
3. **Task 3: docs:** `6bf9abd` (docs). Hosted run 39366784570 on this head succeeded on all 18 jobs.

## TDD Gate Compliance

- **RED** (`f7a9b41`): ran `ctest --test-dir build/ci -R '^(bios\.rejects_cart|runner\.movie\.(boot_bios|bios_mismatch|boot_bios_no_file|boot_conflict)|runner\.bios\.rejected)$' --output-junit`, which exited 8.
  - The classifier returned `RED_EVIDENCE_OK` (`target_test_failed`, target `bios.rejects_cart`, 6 of 9 failing).
  - Semantic assessment: every target ran and failed on its planned assertion. `bios.rejects_cart` got status 0 where 10 was expected and 801 samples where 0 was expected. `runner.bios.rejected` exited 0 where 77 was expected. The four movie cases exited 2 with the old "not available yet" refusal. None of the failures came from a build, fixture or parse fault.
- **Already green before GREEN:** `bios.k_window`, `bios.k_first_fetch`, `bios.splash_skip`, `runner.bios.invalid` and `runner.usage.boot_bios_no_file` passed at RED. They test behaviour that Task 1's tracer had to build (K once per boot, the option checks, invalid-BIOS status) for `bios.handoff_simple` to pass. They were checked to exercise that behaviour: `k_first_fetch` asserts at least three re-entries with K unchanged.
- **GREEN** (`8d3d733`): all 62 `bios.*` and `runner.*` tests pass.
- **REFACTOR:** none needed.

## Files Created/Modified

- `src/boot.c`: `muhtari__boot_bios`, `muhtari__boot_bios_maps`, `muhtari__boot_bios_byte` and `muhtari__boot_handoff_frame`; the fast boot records K = 1.
- `src/cpu.c`: `muhtari__cpu_reset`, and the sync flag on opcode fetches.
- `src/bus.c`: `DEV_BIOS` in the decode and the peeks, hand-off detection, and the rejection in the INPTCTRL write.
- `src/maria.c`: DMA reads the mapped BIOS.
- `src/frame.c`: the rejected status (checked first and after the run), and the power-on latch.
- `src/instance.c`: load and power cycle boot through the BIOS when one is loaded.
- `src/internal.h`: `struct muhtari__boot`, `bus.sync` and the new declarations.
- `runner/main.c`: `--boot`, `--bios`, `read_bios` with the movie digest check, the `handoff_frame` line, the 77 mapping and the movie boot precedence.
- `runner/exit_status.h`: the BIOS cases in rows 1 and 77.
- `tests/boot/test_boot.c` and `tests/boot/tests.cmake`: the five `bios.*` cases.
- `tests/runner/tests.cmake`: seven runner cases, and `movie.conflict` without the old boot-bios refusal.
- `tests/tests.cmake`: includes `tests/boot/` beside `built/`.
- `tests/inventory/ci.txt` and `tests/inventory/wasm32.txt`: twelve lines each.
- Docs:
  - `docs/hardware/bus.md`: the BIOS mapping, the rejection and the power-on start.
  - `docs/hardware/sally.md`: a Reset section.
  - `docs/embedding.md`: Booting with a BIOS.
  - `docs/runner.md`: the options, output lines, movie keys, timing and precedence.
  - `docs/emulation-tests.md`: the bios section.

## Decisions Made

See key-decisions. The main one is the latch at power-on. On a console, the switch and stick pins already show what is held when power comes on, so the input set before frame 1 applies from power-on. This makes movie line 1 meaningful under `boot bios`, and it changes nothing under the fast boot.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 2 - Missing functionality] MARIA's DMA reads the BIOS**
- **Found during:** Task 1.
- **Issue:** The plan named only bus.c for the mapping. MARIA's fetch in src/maria.c reads cartridge space directly, so the OpenBIOS display lists would have read the cartridge in place of the Fuji graphics.
- **Fix:** MARIA's fetch now goes through the same `muhtari__boot_bios_maps` and `muhtari__boot_bios_byte`.
- **Files modified:** src/maria.c.
- **Verification:** the frame-100 PNG shows the Fuji, and K = 323.
- **Commit:** 88d6774.

**2. [Rule 2 - Missing functionality] Input latched at the BIOS boot's power-on**
- **Found during:** Task 1, while measuring the splash skip.
- **Issue:** frame.c latched input only at a VBLANK start, so Pause "held from frame 1" would have applied only from frame 2.
- **Fix:** Frame 1 of a BIOS boot is 117,174 ticks long and is recognised by that length. Its input is latched at power-on.
- **Files modified:** src/frame.c, docs/runner.md.
- **Verification:** `bios.splash_skip` passes.
- **Commit:** 88d6774.

**3. [Rule 2 - Missing functionality] `--bios` without boot bios, and boot bios without a cartridge, are usage errors**
- **Found during:** Task 1.
- **Issue:** Without these checks, a BIOS file passed with the fast boot would be silently ignored.
- **Fix:** Both cases exit 2.
- **Files modified:** runner/main.c.
- **Verification:** `runner.usage.boot_bios_no_file` passes.
- **Commit:** 88d6774.

**4. [Scope] Exit table rows changed in Task 2, not Task 3**
- **Found during:** Task 2.
- **Issue:** `runner.exit.table` requires `--help` and docs/runner.md to match in the same commit.
- **Fix:** docs/runner.md's two exit rows changed with runner/exit_status.h.
- **Commit:** 8d3d733.

**Total deviations:** 3 auto-fixed (Rule 2) and 1 move of a doc change between tasks.
**Impact:** All were needed for the plan's truths to hold. There was no scope creep.

## Unreleased Pins

No frame or title pin moved. Every `basic.*` and `frame.*` test passes unchanged, because the fast-boot path is untouched apart from setting K = 1.

## Issues Encountered

None.

## Known Stubs

None.

## Threat Flags

None. The runner reads one more user-supplied file, the BIOS. It goes through the existing bounded `read_image`, and `muhtari_load` validates its size and vector.

## User Setup Required

None.

## Next Phase Readiness

The BIOS path is ready for the title predicates on both paths, which complete GAME-03 in plan 03-18's library job. Probe PR #12 stays a draft and was pushed to; it is not ready and has not been merged.

## Self-Check: PASSED

- All created files exist: tests/boot/test_boot.c, tests/boot/tests.cmake and this SUMMARY.
- Commits 88d6774, f7a9b41, 8d3d733 and 6bf9abd are ancestors of HEAD.
- `cmake --workflow --preset ci` exited 0: 562 tests plus 6 release tests.
- The hygiene workflow passed 24 of 24, and `scripts/hygiene.sh --tree` exited 0.
- Hosted runs 39366092107 and 39366784570 succeeded.
