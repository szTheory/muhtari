---
phase: 13-portable-save-load-across-three-interfaces
plan: "06"
subsystem: libretro
tags: [c, libretro, portable-state, save-load]
requires:
  - phase: 13-portable-save-load-across-three-interfaces
    provides: "Canonical library state API and runner state boundary from plans 13-01 through 13-05."
provides:
  - "Loaded libretro serialization callbacks delegate the canonical library state API."
  - "Basic savestate metadata is backed by a loaded-module test and a missing-evidence fault control."
affects: [libretro, portable-state, frontend-capability-claims]
actuals:
  tokens: 7384
  tasks: 2
  commits: 4
plan_head_before: f5aea9aa5742168d5d66ae181aa7fb0695caeef5
plan_head_after: ae043499bfe08eadfd83f86d140067e64d8f8f30
commits: 4
tech-stack:
  added: []
  patterns:
    - "Libretro state callbacks use the core's exact canonical byte format."
    - "The .info capability checker requires a registered consumer for every advertised true claim."
key-files:
  created: []
  modified:
    - libretro/libretro.c
    - tests/libretro/host.c
    - tests/libretro/tests.cmake
    - tests/inventory/ci.txt
    - libretro/muhtari_libretro.info.in
    - tests/cmake/info_claims.cmake
    - docs/libretro.md
    - .planning/phases/13-portable-save-load-across-three-interfaces/deferred-items.md
key-decisions:
  - "Rollback-netplay context 3 remains refused until a big-endian format and its evidence boundary are defined."
  - "The .info claim covers basic callbacks only; it does not assert RetroArch rewind, run-ahead, or netplay qualification."
patterns-established:
  - "Serialize accepts caller capacity at least the canonical size and leaves the extra tail untouched; restore requires the exact canonical size."
  - "Lifecycle failures report no stale serialized bytes, while restore keeps the live battery span in place."
requirements-completed: []
coverage:
  - id: D1
    description: "Loaded libretro save and restore use the library's canonical bytes with safe context and buffer-length behavior."
    requirement: FRONT-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/state06/out -R '^libretro\\.host\\.state$' --no-tests=error --output-on-failure; exit 0, 1/1. Final grouped run: exit 0, 7/7. Run URL: N/A-local."
        status: pass
    human_judgment: false
  - id: D2
    description: "Loaded-module lifecycle and battery checks prove no stale bytes, in-place save RAM, and changed-restore generation behavior."
    requirement: STATE-05
    verification:
      - kind: integration
        ref: "libretro.host.state in build/state06/out; final grouped CTest exit 0, 7/7. Run URL: N/A-local."
        status: pass
    human_judgment: false
  - id: D3
    description: "The savestate metadata claim passes only when the actual loaded-module evidence row is present."
    requirement: FRONT-01
    verification:
      - kind: integration
        ref: "libretro.info_claims.savestate and libretro.info_claims.missing_state.self_test; final grouped CTest exit 0, 7/7. Run URL: N/A-local."
        status: pass
    human_judgment: false
duration: 17m
completed: 2026-11-01
status: complete
---

# Phase 13 Plan 06: Libretro Savestate Callbacks Summary

**Loaded libretro save/load now uses the library's canonical state bytes, with guarded basic savestate metadata and no claim of actual-host or rollback-netplay support.**

## Performance

- **Duration:** 17 minutes.
- **Started:** 2026-11-01T17:21:12-05:00.
- **Completed:** 2026-11-01T17:38:06-05:00.
- **Tasks:** 2.
- **Files modified:** 8 plan-owned source, test, documentation, and deferred-item files.

## Accomplishments

- Implemented loaded-module `retro_serialize_size`, `retro_serialize`, and `retro_unserialize` through the public library state API. The adapter preserves extra serialization capacity, requires exact restore size, refuses explicit context 3, and uses fully validated canonical bytes for normal, run-ahead contexts 1 and 2, and an unavailable context query.
- Added loaded-module continuation checks against an independent library instance, including video/audio/state equality, lifecycle refusal, pending input, and battery-span/generation behavior.
- Enabled `savestate = "true"` only after the host-state consumer passed, and guarded the claim with actual positive and missing-inventory-row checks. Documentation keeps actual RetroArch, rewind, run-ahead, and rollback-netplay qualification outside this plan.

## Task Commits

1. **Task 1: Restore through the loaded module using the same library bytes** — `6de8514` (test), `bf6f77e` (feat).
2. **Task 2: Turn on only proven basic savestate metadata and guard its evidence** — `1223fd4` (test), `ae04349` (feat).

## Files Created/Modified

- `libretro/libretro.c` — delegates state operations to the loaded core and refuses the unsupported rollback context.
- `tests/libretro/host.c`, `tests/libretro/tests.cmake`, `tests/inventory/ci.txt` — prove state callback behavior through the loaded module and register the consumer.
- `libretro/muhtari_libretro.info.in`, `tests/cmake/info_claims.cmake` — advertise only the tested callback claim and map it to evidence.
- `docs/libretro.md` — documents callback semantics, context limits, persistence ownership, and actual-host boundary.
- `.planning/phases/13-portable-save-load-across-three-interfaces/deferred-items.md` — records an unrelated pre-existing docs byte-budget overage.

## Decisions Made

- Context 3 is refused because it requires big-endian rollback state; this plan adds no second codec and does not reinterpret the unresolved STATE-02 shape assumption.
- `savestate = "true"` means basic loaded callback support only. The plan does not qualify an actual RetroArch host, rewind, run-ahead, or netplay.
- Phase-level requirements remain pending until their complete acceptance evidence, including exact-source native and big-endian results in plan 13-08, is available.

## TDD Gate Compliance

- **Task 1 RED:** The registered `libretro.host.state` CTest executed and exited 8 because the new consumer expected a nonzero module state size matching the library (45,788 bytes), while the stub returned 0. `gsd_run check tdd-red-evidence` classified the report as `RED_EVIDENCE_OK`. After implementation, the tracer rerun passed 1/1.
- **Task 2 RED:** The actual `info_claims.cmake` consumer executed and exited 8 because `savestate = "true"` had no mapping. `gsd_run check tdd-red-evidence` classified the report as `RED_EVIDENCE_OK`. The positive claim and missing-row fault control then passed through the checker.

## Verification

All receipts are local; run URLs are N/A-local.

- `cmake -S build/state06 -B build/state06/out -G Ninja && cmake --build build/state06/out --target muhtari_test_libretro_host muhtari_libretro && ctest --test-dir build/state06/out -R '^(libretro\.host\.state|libretro\.info_claims(\..*)?|libretro\.vendored)$' --no-tests=error --output-on-failure --output-junit build/state06/junit-green-13-06-final.xml` — exit 0, 7/7.
- `LC_ALL=C sort -c tests/inventory/ci.txt` — exit 0.
- `git diff --check` — exit 0.
- `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` — exit 1 because the unchanged `docs/embedding.md` is 43,947 bytes against the 40,960-byte budget. This finding is recorded in `deferred-items.md`; no unrelated document was changed.
- Full `cmake --preset ci` and rcheevos integration were not run: local pins `build/pins/wasmtime`, `build/pins/7800basic`, `build/pins/openbios`, and `build/pins/rcheevos` are absent. No ROM, BIOS, pin, RetroArch, hosted, Phase 12, or identity-bound action was run. The focused harness compiled the exact adapter and host-test sources against tracked fixtures.
- `tests/libretro/vendored` passed. The vendored `libretro.h` and provenance were left unchanged.

## Deviations from Plan

None. The repository-wide CI configure was excluded by the execution constraints; plan-scoped exact-source checks were used instead.

## Issues Encountered

- The documentation budget script reports the pre-existing embedding-guide overage described above. Plan 13-06 changes only `docs/libretro.md` and does not attempt to reduce the unrelated file.
- `.planning/WINDOWS.md` has the previously documented count mismatch. It was not appended to or retried.
- STATE-02 remains unresolved/unclassified as directed by the phase design. Exact-source big-endian execution and the phase acceptance boundary remain assigned to plan 13-08.

## User Setup Required

None.

## Next Phase Readiness

Plan 13-06 is complete and its artifacts are ready for the next sequential plan. Do not treat this as phase acceptance: plan 13-08 still needs exact-source native and big-endian receipts. No user action is needed for this plan.

## Self-Check: PASSED

The summary exists, and all four task commits are ancestors of the recorded plan head. The measured task commit count is four.

---
*Phase: 13-portable-save-load-across-three-interfaces*
*Completed: 2026-11-01*
