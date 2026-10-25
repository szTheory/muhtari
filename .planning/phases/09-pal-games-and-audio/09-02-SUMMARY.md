---
phase: 09-pal-games-and-audio
plan: "02"
subsystem: input
tags: [pal, controllers, timing, battery, lightgun]
requires:
  - phase: 09-01
    provides: Selected console timing, PAL geometry and transactional region installation
provides:
  - PAL physical-time keypad, paddle and EEPROM deadlines in existing port owners
  - Native lower-row optical and selected-period pending-event coverage
  - Both-region battery, transactional and allocation lifecycle regression coverage
affects: [09-03, 09-07, 09-08]
tech-stack:
  added: []
  patterns:
    - Scale physical-time fits before one final integer ceil
    - Extend existing bus-data-tick and lifecycle consumers across both regions
key-files:
  created: []
  modified:
    - src/ports.c
    - tests/input/test_input.c
    - tests/core/test_core.c
    - tests/core/test_pal.c
    - docs/hardware/controllers.md
key-decisions:
  - Retain NTSC numeric predicates and native motion/optical tick fits; convert only physical-time models.
  - Distinguish a real D64 last-row response crossing VBLANK from a seeded pending deadline crossing the physical frame end.
  - Report the established producer's local pre-plan comparison without claiming hosted or phase-versus-main acceptance.
requirements-completed: []
coverage:
  - id: physical-time-pins
    description: PAL keypad2838, EEPROM35469 and unrounded-fit paddle deadlines through actual bus ticks
    requirement: PAL-03
    verification:
      - kind: unit
        ref: input.directions; input.savekey.protocol; input.savekey.pins; core.battery.deadline
        status: pass
    human_judgment: false
  - id: native-optical-boundaries
    description: PAL last-row emission, clipping, blanked and colour-kill controls, pending wrap and NULL-video equivalence
    requirement: PAL-02
    verification:
      - kind: integration
        ref: input.lightgun; core.pal.lifecycle; core.pal.tracer
        status: pass
    human_judgment: false
  - id: region-battery-lifecycle
    description: Both regions and EEPROM placements preserve spans, committed bytes, generations and allocation invariants
    requirement: PAL-01
    verification:
      - kind: integration
        ref: core.battery.reset; core.battery.no_poll; core.no_alloc_after_load; core.pal.lifecycle
        status: pass
    human_judgment: false
  - id: local-product-package
    description: Complete current/base local product workflows and actual three-ZIP producer observations
    requirement: PAL-03
    verification:
      - kind: other
        ref: cmake --workflow --preset ci; tests/cmake/quality_evidence.cmake BUILD_BASE=ON
        status: pass
    human_judgment: false
actuals:
  tokens: 15900
  tasks: 3
  commits: 4
plan_head_before: 2096b281014d91b7d1665636c8c68f0eb579e798
plan_head_after: 634596b9d18fdedc34dc60ec4fc9e55e1510af2b
duration: 13min
completed: 2026-10-25
status: complete
---

# Phase 9 Plan 2: Physical peripherals and lower raster Summary

PAL physical-time pins now use selected integer deadlines, with lower-raster optical and battery lifecycle regressions covering both console regions.

## Accomplishments

- Keypad recovery selects2838 PAL ticks at row release, reset and device latch; NTSC remains2864. EEPROM STOP selects35469 PAL ticks and retains35796 NTSC ticks. Five milliseconds remains selected compatibility policy, not measured latency.
- Paddle conversion applies6502639/6562500 to the unrounded resistance fit. Literal PAL positions0/32767/32768/65535 produce1861/94877/94879/187895 ticks. Both pots on both ports preserve charge on changed settings, repeated polling, dump/release, JAM and HALT. The uint64 numerator and rounding addition remain bounded.
- Existing optical vectors now run both regions, including PAL aim291, rejected292, raster307/x319, aperture ends, nonvisible following rows, bright/dark/offscreen and colour kill, peeks, D6 history, HALT/JAM and video present/NULL. Real D64 last-pixel pulses cross VBLANK before the physical frame end. A separately seeded pending deadline tests absolute-event retention across142102 without enlarging the optical fit. PAL NULL-video execution matches CPU, sensors, raw-event digest, PCM and synthesis state with non-silent audio.
- EEPROM deadline/reset/no-poll cases run both regions and both placements through actual CPU pin traffic. Eighth-control-bit data ticks at busy−1/exact/+1 independently prove NACK/ACK ordering. Reset retains full battery metadata, bytes and generation only when programming is already due. Failed opposite-region candidates and allocation failures preserve complete live state. All nine controller families retain opposite SaveKey/AtariVox configurations across the expanded no-allocation matrix.

## Task Commits

1. Task1 RED: `e770594` — selected physical-time controller assertions.
2. Task1 GREEN: `a5dc4e2` — selected PAL peripheral conversion and hardware documentation.
3. Task2: `81dca26` — lower-row optical history, NULL-video and pending-wrap regression coverage.
4. Task3: `634596b` — regional battery/allocation lifecycle, exact ACK boundaries, stronger raw/PCM observations and formatting.

Four commits are measured from the persisted pre-plan ledger through the tested source HEAD. The metadata commit is separate. Actual tokens are ceil63600 realized diff characters/4 over the five implementation/test/documentation files, on the estimate's scale.

## Verification Evidence

Every command below exited0 unless explicitly identified as RED or an experiment. All run URLs are N/A-local; no hosted acceptance is claimed.

- Task1: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(input[.]|core[.]battery[.]|core[.]pal[.])' --no-tests=error --output-on-failure` —23/23 pass.
- Task2: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(input[.]lightgun|core[.]pal[.]|maria[.])' --no-tests=error --output-on-failure` —15/15 pass.
- Expanded focused regression: `cmake --build --preset ci && ctest --preset ci -R '^(input[.]|core[.](battery[.]|no_alloc_after_load|pal[.]))' --output-on-failure` —24/24 pass.
- Final source: `cmake --workflow --preset ci` —652 ordinary tests,27.75s, and6 release checks,0.10s. Earlier successful full runs before the final optical fault-control addition were30.14s and35.67s; they are not substituted for the final run.
- `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` —6/6 release checks,0.10s.
- `git diff --check`, `scripts/hygiene.sh --tree`, and `clang-format --dry-run --Werror src/ports.c tests/input/test_input.c tests/core/test_core.c tests/core/test_pal.c` —pass; formatter is the installed LLVM21 tool found by the repository's Homebrew hint.
- `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 09-02 --commits-only --raw` —resolves all four task commits on this branch.

### Local current/base package producer

`env GITHUB_EVENT_PATH="$PWD/build/ci/quality-peripherals-local-event.json" cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-peripherals-local.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DSOURCE=N/A-local -DBUILD_BASE=ON -P tests/cmake/quality_evidence.cmake` exited0. The local descriptor records the real ledger base in `before` and explicitly labels its local scope; it is not a GitHub event or hosted attestation.

Current 634596b9d18fdedc34dc60ec4fc9e55e1510af2b and genuine pre-plan base 2096b281014d91b7d1665636c8c68f0eb579e798 have matching AppleClang21/Darwin-arm64 and fixture/sample identities. The base workflow independently passes652+6,32.62s+0.10s. The producer validates both complete inventories and package integrity.

| Scope | Library ZIP | Libretro ZIP | Runner ZIP | Total bytes |
|---|---:|---:|---:|---:|
| Current local slice | 81703 | 86211 | 94774 | 262688 |
| Pre-plan local base | 81273 | 85807 | 94386 | 261466 |

These report-only measurements establish this local slice comparison, not six-native, hosted or phase-versus-main acceptance. No replacement size quota is introduced. The producer's clean temporary detached base worktree was removed after its actual HEAD and clean tracked state were verified. No log, ROM or save artifact is committed.

## TDD Gate Compliance

Task1 tests preceded implementation. `ctest --preset ci -R '^(input.directions|input.savekey.protocol)$' --output-on-failure --output-junit tdd-red-peripherals.xml` exited8: both named tests executed and failed planned PAL numeric assertions. EEPROM observed35796 rather than35469 ticks; paddle/keypad exact-edge assertions observed the retained NTSC deadlines. The unchanged complete JUnit report and command/result record live only in ignored build output. `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/tdd-red-peripherals.json --raw` exited0 with RED_EVIDENCE_OK. Semantic inspection confirmed real STOP/pin and slow-bus assertions, not collection or setup errors. GREEN subsequently passed23/23.

Tasks2/3 expand already implemented geometry/lifecycle behavior. Investigation confirmed the existing owners satisfy those contracts, so they are regression-only test commits; no artificial RED or unnecessary production repair was introduced. Formatting is bundled with Task3; no separate behavior refactor was needed.

## Deviations and Failed Experiments

- The native D64 fit cannot span the2270-tick physical gap after the last pixel. Cross-frame pending-event coverage therefore seeds an absolute deadline after a real last-pixel emission, while ordinary D64 is verified across VBLANK. An attempted2300-tick test delay was rejected at compile time because delay storage is uint8; production delay/storage remain unchanged.
- The first regional optical run used NTSC's two-call startup assumption and failed its host-return assertion for PAL. The PAL fast-handoff's first future boundary already reaches the next-origin last row; the test now explicitly preserves each region's call count. No scheduler defect was found.
- Test observer installation initially followed load and correctly returned ERR_ARGUMENT. Moving private observer setup before load introduced its deliberate setup allocation; the test now records post-load allocation counts and proves no later allocations. Stronger raw-event hashing and non-silent PCM comparisons pass.
- Lifecycle test edits briefly retained an unused helper and expected the wrong rejection status for an empty image; warning/error checks rejected those test mistakes. The redundant helper was removed and the assertion uses existing ERR_INVALID_IMAGE. A transient misspelled status identifier also failed compilation and was corrected. None of these failures was counted as RED evidence or a product defect.
- The parent owns shared STATE/ROADMAP updates under GSD-HANDOFF section3.1. This executor leaves those files untouched and does not complete PAL-01/02/03; full phase acceptance remains pending successor plans.

## Remaining Scope and Risks

No known stub, skipped test, unrun planned verification or new threat surface remains in this slice. Optical/paddle fits and EEPROM compatibility duration remain unmeasured hardware policy. The existing native motion cadence and NTSC predicates are unchanged. Cross-platform/hosted acceptance and the remaining PAL host, palette, audio and diagnostic work belong to successor plans. No routine owner UAT is requested.

## Self-Check: PASSED

All five declared modified files and this summary exist. All four task commits are ancestors of HEAD; evaluation-scope resolves them. No tracked deletions occurred. Current/base product and release checks pass. Shared state remains the parent's responsibility.
