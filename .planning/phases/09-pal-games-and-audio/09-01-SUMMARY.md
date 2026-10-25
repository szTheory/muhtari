---
phase: 09-pal-games-and-audio
plan: "01"
subsystem: core
tags:
  - pal
  - timing
  - audio
  - runner
  - libretro
requires:
  - phase: 08-controllers-at-the-pins
    provides: Typed inputs, native light emission and bounded device scheduling
provides:
  - Transactional PAL enablement through the library and automatic-region hosts
  - Selected scheduler, startup, geometry and bounded synthesis rational
  - Independent finite coefficient, signed-mass and lifecycle evidence
affects:
  - 09-02
  - 09-03
  - 09-04
  - 09-05
  - 09-08
tech-stack:
  added: []
  patterns:
    - Compact selected timing values in existing owners
    - Streaming independent coefficient-prefix proof
    - Successful-load information controls host extents
key-files:
  created:
    - tests/core/test_pal.c
  modified:
    - src/internal.h
    - src/instance.c
    - src/boot.c
    - src/frame.c
    - src/maria.c
    - src/synth.c
    - src/ports.c
    - runner/main.c
    - libretro/libretro.c
    - tests/boot/test_boot.c
    - tests/maria/test_maria.c
    - tests/libretro/host.c
    - tests/runner/tests.cmake
    - docs/hardware/audio.md
    - docs/hardware/bus.md
    - docs/hardware/maria.md
key-decisions:
  - Keep API6, ABI1 and behavior5 because declarations and retained NTSC observations are unchanged.
  - Use existing isolated test-database substitution for PAL metadata precedence and dual-region preference.
  - Report local candidate package measurements without claiming hosted or independent-base acceptance.
requirements-completed: []
coverage:
  - id: D1
    description: PAL load selects313-line timing and separate fitted fast versus real BIOS startup.
    requirement: PAL-01
    verification:
      - kind: unit
        ref: tests/core/test_pal.c#tracer
        status: pass
      - kind: integration
        ref: bios.k_first_fetch and bios.handoff_simple
        status: pass
    human_judgment: false
  - id: D2
    description: Selected sample arithmetic and physical buffers satisfy the complete PAL safety prerequisites.
    requirement: PAL-03
    verification:
      - kind: unit
        ref: core.pal.bounds
        status: pass
      - kind: unit
        ref: core.pal.tracer
        status: pass
    human_judgment: false
  - id: D3
    description: Native320x292 output and exact rational AV safely reach the actual runner and both loaded formats.
    requirement: PAL-02
    verification:
      - kind: integration
        ref: runner.load.pal
        status: pass
      - kind: integration
        ref: libretro.host.xrgb8888 and libretro.host.rgb565
        status: pass
    human_judgment: false
  - id: D4
    description: Region lifecycle, rejected calls, output extents and existing NTSC and frozen-client contracts remain covered.
    requirement: PAL-01
    verification:
      - kind: unit
        ref: core.pal.lifecycle and maria.dma_line.stats
        status: pass
      - kind: integration
        ref: cmake --workflow --preset ci
        status: pass
    human_judgment: false
actuals:
  tokens: 23863
  tasks: 3
  commits: 4
plan_head_before: 7019a6b29b621a9c881782e5070cc5a50005d2bf
plan_head_after: ad0d661fc0577871be1aac40a27eef6c52642caf
duration: 17min
completed: 2026-10-25
status: complete
---

# Phase 9 Plan 1: Safe PAL Execution Summary

**PAL loads transactionally, runs through the existing scheduler and synthesis path, and reaches the actual runner and both libretro formats with independently proved storage and arithmetic bounds.**

## Performance

- Started: 2026-10-25T16:06:03Z
- Completed: 2026-10-25T16:23:26Z
- Tasks:3; changed files:26 before this summary.
- Actual tokens are ceil(realized committed diff characters/4), not harness usage.
- Four commits are measured from the persisted plan-base ledger through the recorded after-head; the summary commit follows separately.

## Accomplishments

- Committed effective region after successful installation; preserved probe precedence, descriptive metadata, short-tail defaults, status12 and failed-load state.
- Selected PAL313/142102 timing, MSTAT7306/139874, VBE7307, final DMA139860, strict-future fast deadline281976 and actual BIOS first deadline139874. Physical statistics have313 entries.
- Reported320x292, FPS3546894/71051, PAR1843750/1773447 and max962 samples. PAL fast first call returns960/remainder336960; BIOS first946/remainder269046.
- Enumerated all591149 phases and4000 output alignments independently, compared every production coefficient, and proved Q23 normalization8388608, Cmax7283993, P10865492, N2476884 and L1 numerator13342376. Preserved gain18360, pole1070934451 and85:342 weights. Fixed962 work/1103 FIFO storage satisfies the20730-tick tail proof.
- Moved runner output allocation after load/info; native PAL hashes cover93440 bytes and PNG IHDR is320x292. Loaded XRGB8888/RGB565 retain all292 rows,961/962 full-frame samples, selected region/AV and clean release/failure/no-content fallback.
- Added literal snapshots/edges, actual4/16KiB firmware in both regions, stripped OpenBIOS PAL integration, retained battery/pending input, allocation failures, pitch319/320/65536/65537, exact/short audio, padding/canaries, NULL equivalence, native gun boundaries and hostile execution checks.

## Task Commits

1. Task1 RED:8b845bc — public PAL tracer and finite prerequisites.
2. Task1 GREEN:bb4e39c — coherent core/host enablement and matching docs.
3. Task2:824a3ea — startup, event, lifecycle and firmware coverage.
4. Task3:ad0d661 — selected timing/startup/output documentation.

## Verification Evidence

Every command below exited0 unless explicitly identified as RED. Run URL for every local check:N/A-local. No committed logs or hosted pass are claimed.

- Task1: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(core[.]pal[.]|abi[.]api_|core[.]testcard[.]|core[.]load[.]keeps_previous$|audio[.]frames|runner[.]load[.]pal$|libretro[.]host[.](xrgb8888|rgb565)$)' --no-tests=error --output-on-failure` —23/23. Repeated after the GREEN commit for the tracer feedback gate,23/23.
- Task2: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(core[.]pal[.]|bios[.]|maria[.]|core[.]load[.]|core[.]frame[.])' --no-tests=error --output-on-failure` —27/27.
- Final committed sourcead0d661: `cmake --workflow --preset ci` —652 ordinary tests in28.83s plus6 release checks in0.13s. Existing342-row runner equality, frozen API/layout and NTSC predicates remain unchanged.
- `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` —6/6 release checks,0.11s.
- `env -u GITHUB_EVENT_PATH cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-pal-local.json -DSHA=$(git rev-parse HEAD) -DCONFIG=ci -DSOURCE=N/A-local -P tests/cmake/quality_evidence.cmake` —producer verifies652+6 inventories. Atad0d661: library81274, libretro85807, runner94386; sum261467 bytes. Earlier824a3ea observation was81291+85807+94386=261484; neither observation is independent-base or six-native acceptance.
- `ctest --preset ci -R '^docs[.]' --no-tests=error --output-on-failure` —3/3 after prose cleanup. A subsequent complete workflow restored the authoritative full JUnit inventory.
- `git diff --check`, `scripts/hygiene.sh --tree`, file-existence and commit-ancestor checks —exit0. Configured noreply identity and hooks were used for every commit.
- `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan09-01 --commits-only --raw` used the actual separate `--plan 09-01` arguments —exit0, all four task commits resolved on this branch.

The complete finite proof measured0.16–0.19s locally; expanded loaded-format cases remained0.01s, runner smoke0.02s and boot cases at most0.11s. Existing bounded timeouts/COST and lane scaling remain conservative; no hosted cost or platform comparison is inferred.

## TDD Gate Compliance

Task1's named public tracer genuinely failed on the planned assertion: PAL load returned12 rather than0. `ctest --preset ci -R '^core[.]pal[.]tracer$' --no-tests=error --output-on-failure --output-junit /private/tmp/muhtari-pal-red.xml` exited8. The unmodified JUnit report and semantic assessment were supplied to `check tdd-red-evidence build/ci/pal-red-evidence.json`; it returned RED_EVIDENCE_OK before implementation. The target executed and failed on public behavior, not compilation/discovery. RED precedes GREEN; no refactor commit was needed.

Task2 was test-only expansion of the established Task1 behavior. Its additional assertions passed against that implementation; it introduced no production behavior and no additional RED/GREEN implementation cycle. No failing evidence was manufactured or claimed for it. The phase runtime TDD mode remains false.

## Decisions and Limits

API6/ABI1/behavior5 remain because no declarations or retained NTSC observations changed. Automatic PAL works now. Explicit runner/movie PAL tokens and warm-PAL presentation remain successor-plan work, as do reload-only adapter choices, physical-time conversions, full extremizers/spectral checks and sustained/equality certification. None is marked complete here; requirements-completed is empty because PAL-01–03 each span later plans.

The PAL140016 handoff and zero undocumented residue are deterministic fitted policy. Retained85:342 PAL analog balance is explicitly unverified. EEPROM5ms remains the existing compatibility duration. No hardware calibration, measured BIOS parity or broader PAL retail-playability claim is made.

FLAGGED ASSUMPTION PAL-01 unclassified probe edge remains unresolved as required by the plan. Independent literal startup/event/hostile checks establish the specified engineering behavior; they do not resolve that classification or silently promote it to a backstop.

## Deviations from Plan

**[Rule2 - Documentation correctness] Updated stale public-header comments.** Task1 found comments describing every PAL load as unsupported and info as always242/801. Only comments changed in include/muhtari.h; layouts, declarations, API version and status values are untouched. Frozen clients, layout checks and API-name checks pass. Commit:bb4e39c.

Task2 reuses the existing test database by adding its owned source to the PAL test executable in tests/core/tests.cmake, enabling the planned headerless/hash/dual-region assertions without changing installed tables or adding a framework.

## Issues Encountered

During implementation, fatal compiler warnings rejected64-bit constant SIZE_MAX comparisons against uint32 fields; size_t locals preserve checked multiplication on every target. Initial runner PNG verification used an unset OUT variable; the fixture directory now owns that temporary output. Both were corrected before GREEN. No failed check was counted as passing.

No owner setup, authentication gate, required unresolved implementation issue or known stub remains for this plan. STATE/ROADMAP and requirement completion remain the orchestrator's responsibility under the dispatch and GSD-HANDOFF3.1.

## Next Phase Readiness

09-02 may build physical-time device conversions on this selected clock/geometry. Do not infer whole-phase acceptance from this local slice; final exact-source hosted/current-base acceptance remains09-08.

## Self-Check: PASSED

The created PAL test and declared key consumers exist; all four task commits are ancestors ofHEAD and the evaluation-scope resolver finds them on this branch. Focused acceptance and full workflow/package gates pass. No tracked file deletion occurred.
