---
phase: 09-pal-games-and-audio
plan: "05"
subsystem: audio
tags:
  - pal
  - synthesis
  - spectral
  - deterministic
requires:
  - phase: 09-pal-games-and-audio
    provides: Selected PAL timing, bounded synthesis and loaded-host behavior through09-04
provides:
  - Complete arithmetic carry recurrence and actual PAL clock/horizon contracts
  - All8000 independent signed-mass extremizers reconstructed through the production ring
  - Both-region spectral quality, rejected bypass and real hostile-source stress
affects:
  - 09-06
  - 09-07
  - 09-08
tech-stack:
  added: []
  patterns:
    - Streaming coefficient-prefix proof with bounded silent-prefix drains
    - Region-selected physical-frequency captures with unchanged quality judges
key-files:
  created:
    - .planning/phases/09-pal-games-and-audio/09-05-SUMMARY.md
  modified:
    - tests/core/test_pal.c
    - tests/audio/test_contracts.c
    - tests/audio/test_spectral.c
    - tests/audio/test_stress.c
    - tests/audio/tests.cmake
    - docs/hardware/audio.md
key-decisions:
  - Preserve all NTSC predicates and the existing kernel, gain18360, pole1070934451 and85:342 weights.
  - Set only spectral scheduling COST21 from the complete20.08s local workload; retain all timeouts and lane scaling.
  - Keep PAL-03 pending successor diagnostics and final hosted acceptance, with its classification unresolved.
requirements-completed: []
coverage:
  - id: D1
    description: Complete591149-frame arithmetic recurrence and independent startup/full-frame sample counts.
    requirement: PAL-03
    verification:
      - kind: unit
        ref: core.pal.bounds and audio.frames
        status: pass
    human_judgment: false
  - id: D2
    description: Actual456-tick TIA and stretched POKEY edges, half-open ownership, capacity transactionality and NULL parity.
    requirement: PAL-03
    verification:
      - kind: unit
        ref: audio.frames and audio.observer
        status: pass
    human_judgment: false
  - id: D3
    description: Both independently constructed extremizers at all4000 alignments reach exact integrator endpoints without clips.
    requirement: PAL-03
    verification:
      - kind: unit
        ref: core.pal.bounds and core.audio.silence
        status: pass
    human_judgment: false
  - id: D4
    description: Both-region physical-frequency quality retains its strict judge and rejects every nearest-sample bypass control.
    requirement: PAL-03
    verification:
      - kind: integration
        ref: audio.spectral and audio.spectral.controls
        status: pass
    human_judgment: false
  - id: D5
    description: Real maximum-current and hostile mixed-source traffic retains bounded allocation, work, carry and clipping contracts.
    requirement: PAL-03
    verification:
      - kind: integration
        ref: audio.stress and cmake --workflow --preset ci
        status: pass
    human_judgment: false
actuals:
  tokens: 11644
  tasks: 3
  commits: 4
plan_head_before: 5678c7b5fdf5d0961a90791c5a9356a1b2048863
plan_head_after: b8dbe1808ef540d575365d619dcd4dc7ecd9d613
duration: 12min
completed: 2026-10-25
status: complete
---

# Phase 9 Plan 5: PAL Audio Contracts and Reconstruction Summary

**PAL carry, physical source timestamps, all8000 signed-mass reconstructions and both-region spectral/hostile-work checks pass through the existing synthesis path without production changes.**

## Accomplishments

- Arithmetic alone sweeps591149 full frames to568408000 samples/remainder0. Independent checkpoints prove one full frame961/remainder313811 and600 full frames576918/remainder301218. No full-cycle emulation or denominator-sized allocation occurs.
- Actual fast startup141960 gives960/remainder336960; real synthetic BIOS139874 gives946/remainder269046. Both961/962 full-frame counts occur. Tests compare exact-capacity output with NULL PCM/video progression and reject one-short capacity before callbacks, input latches, counters or state change. WSYNC/HALT instruction tails and JAM retain future carry.
- Raw callbacks independently match TIA B events on the456 lattice and POKEY's actual slow/held phi2 edges: timer100 first fires at632 after100 slow plus8 fast cycles, then1048 through a hold. Stable same-tick source order and next-interval deadline ownership remain tested. Near-UINT64 absolute epochs exercise bounded observer offsets through20730; a separately seeded silent completed prefix exercises huge origin-relative quotient/remainder totals without synthesizing that prefix.
- Every591149 interpolated row matches independent signed ties-away, shifted-row64 and floor-half normalization. P10865492/N2476884/Cmax7283993 remain exact. Both0/18360 extremizers at every4000 alignment traverse the real mixed-delta/ring/blocker path and reach the independent P*M/-N*M endpoints, with zero clips and unchanged work, width and headroom limits. The complete PAL bound including1LSB is29203.225608825684, below29203.6 and the one-dB threshold29203.6194898.
- Each region retains132 quality schedules,132 rejected bypasses,72 actual fast/slow POKEY schedules and the existing mixed/high-pass representatives. PAL uses7093788Hz and4000/591149 carried counts, including88/89-sample chunks. Settling12000, capture65536, masks, strict-80dB judge, gain and pole are unchanged. PAL worst TIA is-91.70891dBc; equivalent POKEY-106.09120dBFS; actual POKEY-103.50161dBFS. NTSC worst TIA remains-90.09730dBc and equivalent POKEY-95.41359dBFS.
- Four existing real dual-POKEY/TIA programs run in each region with slow writes, WSYNC/HALT, JAM, split rendering and NULL output. A separate real-register all-ten-DAC case reaches mixed level18360 and retains identical discarded-output state. All execute without allocation after load or clipping.

## Task Commits

1. Task1 — `f4b0965`: prove PAL carry and physical audio horizons.
2. Task2 — `e70ae8c`: reconstruct all PAL signed-mass extremizers.
3. Task3 — `7896e5f`: certify PAL spectral and hostile audio paths.
4. Final contract check — `b8dbe18`: exercise long elapsed PAL sample totals.

The persisted ledger measures four task commits before the summary commit. Actual tokens are ceil46573 realized diff characters/4, not harness usage. The implementation/verification window starts18:02:43UTC and ends about18:14UTC, excluding initial mandatory reading. Six test/documentation files changed. Shared STATE/ROADMAP position and requirement acceptance remain the orchestrator's responsibility under GSD-HANDOFF3.1.

## Verification

All accepted commands below exited0. Every local run URL is N/A-local; no hosted acceptance or committed log is claimed.

- Task1: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(core[.]pal[.]|audio[.]frames|audio[.]observer|pokey[.]clock)' --no-tests=error --output-on-failure` —6/6 in0.89s. The final long-elapsed addition passed `ctest --preset ci -R '^(audio[.]observer|audio[.]frames)$' --no-tests=error --output-on-failure` —2/2 in0.71s.
- Task2: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(core[.]pal[.]bounds|core[.]audio[.]silence)' --no-tests=error --output-on-failure` —2/2; complete PAL coefficient/mass/extremizer/carry workload0.35s, existing NTSC0.03s. No timeout change was needed.
- Task3: `ctest --preset ci -R '^(audio[.]spectral([.]controls)?|audio[.]stress)$' --no-tests=error --output-on-failure` —3/3. Complete production spectral measured16.67s then20.08s; controls0.48–0.50s; expanded stress0.45–0.50s. COST21 starts the production workload early; existing120s/30s deadlines and normal lane scaling remain unchanged.
- Final sourceb8dbe18: `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake --workflow --preset ci` —652 ordinary checks29.23s plus6 release checks0.11s. Complete ordinary/release work remains within60s. In this concurrent full gate, spectral21.04s, controls0.77s, stress0.81s, PAL bounds0.71s, frames1.42s and observer0.00s. Earlier unchanged working source passed652+6 in29.57s/0.10s; source7896e5f passed652+6 in32.39s/0.10s before the additional long-elapsed assertion.
- `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` —6/6 in0.09s. The final-source full workflow independently regenerated the three packages and reran all release checks.
- `git diff --check`, `scripts/hygiene.sh --tree`, and `/opt/homebrew/opt/llvm@21/bin/clang-format --dry-run --Werror tests/core/test_pal.c tests/audio/test_contracts.c tests/audio/test_spectral.c tests/audio/test_stress.c` —exit0. All commits use the configured noreply identity and normal hooks. No production file, fixture, inventory, threshold or hosted workflow changed.
- `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 09-05 --commits-only --raw` —exit0, all four task commits resolve on this branch. Six-file existence and four commit-ancestor checks pass. Stub/threat scan finds no new stub or security surface.

Stress's complete12-frame PAL schedules each produce11537 samples. Across its four seeds, raw record counts are656665/12829/175494/523370, kernel additions/sample at most27, carry high-water at most3 and largest observed pre-clamp peak15328. These are observed finite workload maxima, separate from the unchanged conservative safety proof.

## Local Package Measurements

`env GITHUB_EVENT_PATH="$PWD/build/ci/quality-pal-audio-local-event.json" node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-pal-audio-local.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DSOURCE=N/A-local -DBUILD_BASE=ON -P tests/cmake/quality_evidence.cmake` —exit0 on exact sourceb8dbe1808ef540d575365d619dcd4dc7ecd9d613.

The ignored local event descriptor identifies genuine pre-task5678c7b5fdf5d0961a90791c5a9356a1b2048863 and labels local slice scope. That base is the orchestrator's accepted-wave metadata commit after dispatch's earlier98754eb; the ledger captured actual HEAD before any task commit. The existing producer independently built the base and ran652+6 in30.75s/0.09s. Both reports authenticate their exact SHA, matching toolchain/fixture environment, full inventories and actual three-ZIP sum. A fresh Node assertion checked report identity/status/source, actual component sizes and both six-case release JUnit reports; exit0.

| Source | Library | Libretro | Runner | Total bytes |
|---|---:|---:|---:|---:|
| Candidateb8dbe18 |81763|87472|96375|265610|
| Pre-task5678c7b |81751|87472|96375|265598|

The observed sum difference is12 bytes; archive sizes remain report-only. This is one local macOS-arm64 slice comparison, not joined hosted/six-native evidence, phase-versus-main performance or capacity acceptance. After exact-base and clean-tree assertions, `git worktree remove build/ci/quality-base` exited0. Only the original checkout remains; reports and command output remain ignored build artifacts.

## TDD Gate Compliance

Task-local test-first instructions were honored by writing assertions before any production change. The covered behavior already existed: all three tasks extend tests/documents and no production implementation was needed. Initial failures were test-oracle mistakes, not missing product behavior and not accepted RED evidence. No fabricated RED/GREEN pair or classifier success is claimed; workflow.tdd_mode remains false. The final long-elapsed assertion passed against the same existing production code.

## Deviations and Failed Experiments

- No production architecture, kernel, sample rate or acceptance threshold changed. The final small contract commit completes Task1's long origin-relative count coverage after Task3's complete-workload review. Its explicit silent-prefix seeding avoids an infeasible long emulation run.
- The first focused run reported observer assertion failures: expected1052/213 ignored the documented two running HALT cycles, and expected two callbacks omitted the two actual same-tick AUDC/STIMER changes at0. The direct `build/ci/tests/muhtari-test-audio-contracts` diagnostic exited1 on the reused139-sample NTSC carry assertion. Corrected independent PAL141 and held-cycle equations, and explicitly checked all four callbacks, before the accepted6/6 run. The initial combined invocation yielded while running and its terminal exit was not captured; its failures are retained here and are never counted as passing or RED evidence.
- The producer itself passed on its first invocation. A separate post-producer Node assertion exited1 because it expected a nonexistent `tests.release` metric. Inspection confirmed the producer validates release JUnit without emitting that metric. The corrected assertion validates each actual six-test JUnit report plus the emitted native/archive metrics and exits0. No producer, contract or report was altered to accommodate the assertion.

## Retained Limits and Next Work

FLAGGED ASSUMPTION PAL-03 unclassified edge remains unresolved. Independent equations, actual events/extremizers and spectral controls test the selected behavior; they do not resolve the classification. Retained85:342 PAL analog balance remains explicitly unverified. No physical board calibration or broader retail-playability claim is made.

The591149 sweep is arithmetic only. Actual600-full-frame three-host diagnostic output remains09-07, and complete exact-source hosted/equality/current-base acceptance remains09-08. PAL-03 is not marked fully complete. No owner verification, consent, authentication gate, known stub or required local implementation blocker remains for this plan. No further GSD step, review/verifier or phase completion was launched. Next command in a new session: `$gsd-execute-phase 9`.

## Self-Check: PASSED

All six declared changed files exist. All four recorded task commits are ancestors of the current phase branch, matching the persisted ledger base and measured count. Final-source652+6, package integrity, independent current/base producer and hygiene checks pass. SUMMARY creation and its immediate metadata commit complete this executor's output; the orchestrator owns subsequent shared-state updates.
