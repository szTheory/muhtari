---
phase: 22-complete-souper-homebrew-mapper
plan: "01"
subsystem: cartridge
tags: [souper, cartridge, diagnostics, libretro, emscripten]

requires:
  - phase: 21-measured-milestone-tune-up
    provides: Selected v7.0 Souper mapper scope and source-qualified evidence boundaries
provides:
  - Exact v3/v4 admission and refusal rules for the 512 KiB Souper software profile
  - Initial CPU/MARIA map with zeroed, instance-owned 32 KiB EXRAM
  - Generated literal-oracle diagnostic through the library, runner, native libretro and portable loader
  - Transactional load refusal and deterministic reset/reload lifecycle coverage
affects: [22-02, 22-03, cartridge-loading, cartridge-state]

actuals:
  tokens: 15933.25
  tasks: 2
  commits: 4
commits: 4
plan_head_before: 687e3a3e8bbc6f1fa010844dd7cc0c81cb5aa4ef
plan_head_after: b3159ffa9171d24e757bb4d754da23fa38e96392

tech-stack:
  added: []
  patterns:
    - Generated cartridge fixture with literal CPU and MARIA expectations independent of production mapping code
    - Explicit per-board state-unavailable guard until complete snapshot support lands

key-files:
  created:
    - tests/diag/souper.s
    - tests/diag/souper.cfg
    - tests/diag/souper.a78
    - tests/diag/expected.csv
    - tests/libretro/souper_observe.cmake
    - tests/emscripten/souper.cmake
  modified:
    - src/cart.c
    - src/state.c
    - tests/cart/test_cart.c
    - tests/core/test_core.c
    - tests/diag/driver.c
    - tests/libretro/observe.c
    - tests/emscripten/host.mjs
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/inventory/emscripten.txt
    - docs/hardware/cartridge.md

key-decisions:
  - "Require exact 512 KiB body and declared size; an effective v4 Souper declaration with invalid options, audio, or geometry refuses instead of falling back to v3."
  - "Keep Souper snapshots unavailable until complete format support is implemented in plan 22-03."
  - "Keep the profile as a software-model claim; no BupChip audio or physical-cartridge accuracy is claimed."

patterns-established:
  - "Keep expected mapping bytes and pixels literal and independently authored from mapper logic."
  - "Keep mapper EXRAM ownership within each cartridge instance and clear it on load and power cycle."

requirements-completed: [SOUPER-01, SOUPER-05, SOUPER-06, SOUPER-07]
coverage:
  - id: D1
    description: Exact Souper declarations and geometry are admitted; malformed or unsupported variants refuse transactionally.
    requirement: SOUPER-01
    verification:
      - kind: unit
        ref: "cart.souper.profile and cart.load.no_alloc_on_reject"
        status: pass
    human_judgment: false
  - id: D2
    description: A generated patterned cartridge checks initial CPU and MARIA mapping against literal expected observations.
    requirement: SOUPER-05
    verification:
      - kind: integration
        ref: "diag.souper_tracer"
        status: pass
    human_judgment: false
  - id: D3
    description: The same diagnostic runs through the library, runner, loaded native libretro core and portable consumer, with non-Souper controls retained.
    requirement: SOUPER-06
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; ctest --preset ci --parallel 1; sh scripts/emscripten.sh build/ci"
        status: pass
    human_judgment: false
  - id: D4
    description: Cartridge documentation identifies the exact software profile, reset model, refusal cases, audio exclusions and evidence limits.
    requirement: SOUPER-07
    verification:
      - kind: other
        ref: "docs/hardware/cartridge.md, Souper section"
        status: pass
    human_judgment: false

duration: 108min
completed: 2026-11-14
status: complete
---

# Phase 22 Plan 01: Souper Initial Map Summary

**Exact Souper profile admission and a generated initial-map diagnostic now run through every existing local consumer.**

## Performance

- **Duration:** 1h 48m
- **Started:** 2026-11-14T00:36:33Z
- **Completed:** 2026-11-14T02:24:56Z
- **Tasks:** 2
- **Files modified:** 24

## Accomplishments

- Added a generated 512 KiB tracer with literal CPU mailbox and MARIA pixel expectations, exercised through the C library, headless runner, loaded native libretro core and production portable loader.
- Implemented the initial Souper map with 32 KiB zeroed EXRAM owned by each cartridge instance; load and power cycle reset its initial latches and memory.
- Enforced exact v3/v4 profile admission, transactional refusal, allocation-failure preservation, and the documented error distinction for truncation and extra payload.
- Preserved the explicit Souper snapshot-unavailable behavior pending complete snapshot support in plan 22-03.
- Recorded the pre-Souper source baseline for a later independent comparison: commit `687e3a3e8bbc6f1fa010844dd7cc0c81cb5aa4ef`, tree `8227b8daf2aeac98ecd837cb3ef9679b091b8d02`, with `src/cart.c` blob `aa2a88db2e456a7cd3dffdb47930cd54510654b1`.

## Task Commits

1. **Task 1 RED:** `7ecbdf6` — failing Souper initial-map tracer.
2. **Task 1 GREEN:** `8b38164` — initial Souper map and end-to-end tracer.
3. **Task 2 RED:** `2adc6c2` — profile admission and lifecycle regressions.
4. **Task 2 GREEN:** `b3159ff` — exact Souper admission and refusal.

The plan metadata commit is recorded in the execution completion report.

## Checks

- The first full native workflow hit timeout limits on two tests under default parallel load. `ctest --preset ci --parallel 1 --output-on-failure --no-tests=error` then passed all 716 tests, including the Souper tracer and the exact-inventory audit.
- `cpack --preset ci` passed, producing the library, runner and libretro packages; `ctest --preset ci-release --parallel 1 --output-on-failure --no-tests=error` passed all 8 release checks.
- The Emscripten route first timed out its control test only under concurrent load. The complete `sh scripts/emscripten.sh build/ci` rerun with the test preset serialized passed all 7 tests, including the Souper portable test and inventory/report checks. The temporary preset edit was restored and is not part of the changes.
- Task 2's required focused command passed all six cases: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci --output-on-failure --no-tests=error -R '^(cart[.]souper[.]profile|cart[.]load[.]no_alloc_on_reject|core[.]souper[.]lifecycle|core[.]load[.]keeps_previous|core[.]no_alloc_after_load|diag[.]souper_tracer)$'`.
- The TDD red-evidence checker accepted the Task 2 cart-profile failure as the intended RED result; the subsequent implementation and all six focused checks passed.
- No physical, host, remote, publication or merge check was run. Those are outside this plan.

## Decisions Made

- Valid effective v4 declarations retain precedence; malformed mapper 4 options, audio declarations or geometry refuse rather than falling through to a compatible v3 profile.
- Souper snapshot size/save/load stay unavailable without mutation until plan 22-03 can add complete state support.
- `$8007` does not queue or emit BupChip audio. Existing POKEY parsing remains on the common loader path.

## Deviations from Plan

The default parallel native workflow timed out two time-bounded tests, and the first parallel portable run timed out its control test. After investigating the load-sensitive failures, complete serial reruns passed. This changed verification scheduling only; no check or timeout was weakened. A best-effort Windows ledger append was rejected because its existing frontmatter counts disagree with its entries (16 open and 36 fixed entries versus frontmatter 17 open and 35 fixed); the verification deviation is recorded here instead.

**Total deviations:** 1 verification scheduling adjustment. **Impact:** No implementation scope change; all required workloads passed on serial rerun.

## Issues Encountered

The project-wide broken-windows ledger could not accept the optional deviation entry because of the pre-existing count mismatch described above. No source or test issue remains from this plan.

STATE.md initially had `Plan: Not started`, which the state-advance command could not parse after the first plan summary existed. I reconciled the position to plan 1 of 3, reran the state advance, and confirmed plan 2 of 3 is now next.

## Known Stubs

None. Souper state unavailability is an explicit, tested compatibility boundary for this plan; plan 22-03 supplies complete state support.

## Next Phase Readiness

Plan 22-02 can extend this production initial map with register-selected ROM/EXRAM modes. Plan 22-03 remains responsible for complete state support. This plan does not establish Phase 22 acceptance: banking modes, MARIA transitions and snapshot continuation remain open.

---
*Phase: 22-complete-souper-homebrew-mapper*
*Completed: 2026-11-14*

## Self-Check: PASSED

- Summary file exists at the required phase path.
- All four task commits are ancestors of the current plan head.
