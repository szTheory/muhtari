---
phase: 14-proven-frontend-states-and-responsive-play
plan: "01"
subsystem: testing
tags: [atari-7800, c, diagnostics, HSC]

requires:
  - phase: 13-portable-save-load-across-three-interfaces
    provides: Public-library state serialization and battery access
provides:
  - Owned NTSC progress fixture with a stable RAM marker and 16-bit counter
  - Independent public-library picture, boundary, HSC, and replay oracle
affects: [14-02, 14-03, 14-04, 14-05]

actuals:
  tokens: 9147
  tasks: 2
  commits: 2
plan_head_before: 157cab649796ffc5fd150a96e57c75a4c835aa56
plan_head_after: f88bb41552a52dbd891091ab3c9259aefa88165a

tech-stack:
  added: []
  patterns: [Owned assembly diagnostic driven through the public library]

key-files:
  created:
    - tests/diag/frontend-states.s
    - tests/diag/frontend-states.cfg
    - tests/diag/frontend-states.a78
  modified:
    - tests/diag/driver.c
    - tests/roms/manifest.txt

key-decisions:
  - "Place the fixture identity and little-endian counter at RAM $2300 and $2304."
  - "Treat HSC as a complete raw 2048-byte image, with its counter at offsets 0x100 and 0x101."
  - "Derive the visible glyph and completed-call frame phase from the fixture and existing MIT display code."

requirements-completed: []
coverage:
  - id: D1
    description: "Owned fixture and local public-library oracle provide repeatable progress, picture, HSC, and save-replay evidence; the pinned RetroArch host round trip remains pending."
    requirement: FRONT-02
    verification:
      - kind: integration
        ref: "build/ci/tests/muhtari-test-diag --frontend-progress tests/diag/frontend-states.a78 build/ci/tests/frontend-progress"
        status: pass
      - kind: integration
        ref: "ctest --preset ci -R '^diag\.rebuild$' --output-on-failure"
        status: pass
      - kind: integration
        ref: "build/ci/tests/muhtari-test-diag --frontend-progress-media on committed-0001.raw, committed-0100.raw, and committed-0000.raw"
        status: pass
    human_judgment: false

duration: 17min
completed: 2026-11-03
status: complete
---

# Phase 14 Plan 01: Proven frontend states and responsive play Summary

**An owned NTSC fixture now drives an independent public-library oracle for visible progress, 16-bit boundaries, complete HSC persistence, and save/restore/replay.**

## Performance

- **Duration:** 17 minutes
- **Started:** 2026-11-03T04:00:25Z
- **Completed:** 2026-11-03T04:17:08Z
- **Tasks:** 2
- **Implementation files modified:** 5

## Accomplishments

- Added a firmware-free linear/HSC fixture with a `MUHT` identity marker at RAM `$2300`, a little-endian 16-bit counter at `$2304`, and one progress step per completed visible frame.
- Extended the public-library diagnostic driver to independently check the expected native pixels, startup and counter progression, `00FF` carry, `FFFF` wrap, the complete 2048-byte raw HSC image, and save/advance/restore/replay behavior.
- Built and manifest-pinned the fixture using the existing assembler KEEP path. Pictures, state, raw HSC images, and observations were written only under `build/ci/tests/frontend-progress`.

## Task Commits

1. **Task 1: fixture and public-library oracle** — `7b5f896` (feat)
2. **Task 2: manifest-admitted fixture and exact binary identity** — `f88bb41` (feat)

The source pin points to the first commit; the second adds the rebuilt binary and matching SHA-256 manifest row.

## Files Created/Modified

- `tests/diag/frontend-states.s` and `tests/diag/frontend-states.cfg` — owned NTSC linear/HSC-only progress fixture.
- `tests/diag/frontend-states.a78` and `tests/roms/manifest.txt` — rebuilt binary and exact source/SHA-256 admission.
- `tests/diag/driver.c` — independent frame, picture, boundary, HSC, and replay checks through the shipped library.

## Decisions Made

- The display oracle uses the existing MIT font and display-list conventions, with expected glyph rows encoded independently in the driver.
- The HSC oracle checks every raw byte: the initial image is all `FF`; after commit only offsets `0x100` and `0x101` hold the little-endian progress value.
- The fixture uses the existing serializer and battery APIs. No alternate state format or host-specific implementation was added.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Corrected fixture loop and visible-cell expectations**
- **Found during:** Task 1
- **Issue:** Early picture checks disagreed with the fixture because the 195-entry map clear depended on the 6502 sign bit and the selected map cell did not match the intended screen position.
- **Fix:** Used an explicit terminal counter condition and placed the progress digits at the source-derived map cell. The driver also uses the raw HSC offsets corresponding to the CPU-visible `$1100/$1101` window.
- **Files modified:** `tests/diag/frontend-states.s`, `tests/diag/driver.c`
- **Verification:** The final public-library oracle, media checks, and `diag.rebuild` passed.
- **Committed in:** `7b5f896`

### Process Deviation: TDD RED commit omitted

The task is marked `tdd="true"`. The oracle exposed implementation mismatches during iteration and the final checks pass, but the expected RED evidence was not preserved through the required classifier and there is no preceding `test(14-01)` commit. This is a TDD discipline deviation; the plan is `type: execute`, and no plan-level TDD halt was active.

**Total deviations:** 1 auto-fixed code issue and 1 TDD process deviation. The fixes stayed within the fixture and diagnostic scope.

## Issues Encountered

The isolated worktree did not contain the already-pinned local build tools. Reused the matching existing cache by copying the pinned tool artifacts into ignored `build/pins`; no packages were installed and no network fetch was used.

The optional broken-windows append was rejected because the existing `.planning/WINDOWS.md` frontmatter counts disagree with its entries (frontmatter 17 open/35 fixed; entries 16 open/36 fixed). This pre-existing ledger issue was left unchanged and recorded in `deferred-items.md`.

## Verification

- `cmake --preset ci` — exit 0.
- `cmake --build --preset ci` — exit 0.
- `build/ci/tests/muhtari-test-diag --frontend-progress tests/diag/frontend-states.a78 build/ci/tests/frontend-progress` — exit 0; startup, `0000..0040`, carry, wrap, HSC, and save/restore/replay passed.
- `ctest --preset ci -R '^diag\.rebuild$' --output-on-failure` — exit 0; rebuilt fixture identity matched the manifest.
- `--frontend-progress-media` checked the complete 2048-byte HSC images for counts 1, 256, and 0 — all passed.
- Repository commit hooks ran normally; the configured commit identity passed the noreply check.

No RetroArch process or CI workflow was launched. The automatic probe classifications remain unresolved. FRONT-02 actual-host evidence and FRONT-03/FRONT-04 remain pending later Phase 14 plans.

## User Setup Required

None.

## Next Phase Readiness

The owned fixture and local oracle are ready for plans 14-02 and 14-03. The later source-bound host receipt remains responsible for actual RetroArch verification.

---
*Phase: 14-proven-frontend-states-and-responsive-play*
*Completed: 2026-11-03*

## Self-Check: PASSED

- SUMMARY and all five planned implementation files exist.
- Task commits `7b5f896` and `f88bb41` are ancestors of the current branch HEAD.
