---
phase: 13-portable-save-load-across-three-interfaces
plan: "03"
subsystem: core-state
tags: [c, state-api, crc32, api8, savekey]
requires:
  - phase: 13-portable-save-load-across-three-interfaces
    provides: fixed private state visitor, field proof, and cached machine identity from plans 13-01 and 13-02
provides:
  - API 8 exposes the exact 64-byte state envelope with transactional size/save/load operations.
  - Public validation bounds hostile continuation clocks and preserves live state on refusal.
  - Battery rollback, generation wrap, pending SaveKey replay, and host persistence ownership have executable coverage.
affects: [phase-13-runner-state, phase-13-libretro-state, state-verification]
actuals:
  tokens: 24635.25
  tasks: 3
  commits: 6
plan_head_before: e9d5fb263f1ec4ad23b02c49aad139749fb3a51d
plan_head_after: 6044b65dd72f96634c815a56edfe8e3a420f1d62
commits: 6
tech-stack:
  added: []
  patterns:
    - Validate the complete caller-owned state envelope before applying it in place.
    - Keep battery generation as live host metadata and serialize pending EEPROM protocol state.
key-files:
  created:
    - tests/api/v8.c
  modified:
    - include/muhtari.h
    - src/internal.h
    - src/state.c
    - src/instance.c
    - tests/core/test_state.c
    - tests/abi/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/inventory/nofp.txt
    - tests/api/v7.c
    - tests/header/header.c
    - tests/header/header.cpp
    - docs/embedding.md
    - .planning/phases/13-portable-save-load-across-three-interfaces/13-FIELD-PROOF.md
key-decisions:
  - Keep the live battery generation outside the snapshot and advance it only when committed bytes differ.
  - Preserve pending EEPROM protocol and busy state so the existing device path commits after restore.
requirements-completed: []
coverage:
  - id: D1
    description: The public state API writes and restores the exact envelope, validates before apply, and preserves caller/live bytes on refusal.
    requirement: STATE-01
    verification:
      - kind: unit
        ref: "ctest core.state.(payload|fields|aliases|lifecycle|topologies), temporary state harness; exit 0"
        status: pass
      - kind: integration
        ref: "tests/api/v8.c and API v1-v8 compatibility matrix; exit 0"
        status: pass
    human_judgment: false
  - id: D2
    description: API 8 reports state format 1 while preserving earlier API callers, prefixes, and status values.
    requirement: STATE-01
    verification:
      - kind: integration
        ref: "C17/C++17 header checks and tests/cmake/api_version.cmake; exit 0; extra-field fault control rejected as expected"
        status: pass
    human_judgment: false
  - id: D3
    description: Changed, equal, repeated, and refused restores preserve battery bytes and generation semantics; pending HSC+SaveKey programming resumes on an independent instance without file or state-call observer activity.
    requirement: STATE-05
    verification:
      - kind: unit
        ref: ctest --test-dir /private/tmp/muhtari-state-13-03-tdd/build -R "^core\.state\.(battery|lifecycle|aliases)$" --no-tests=error --output-on-failure --output-junit /private/tmp/muhtari-state-13-03-tdd/build/junit-state-03-3-final.xml; exit 0, 3/3
        status: pass
    human_judgment: false
  - id: D4
    description: The field proof and embedding guide document public continuation bounds, battery rollback, and host-owned persistence.
    verification:
      - kind: other
        ref: ".planning/phases/13-portable-save-load-across-three-interfaces/13-FIELD-PROOF.md and docs/embedding.md"
        status: pass
    human_judgment: false
duration: 35m
completed: 2026-11-01
status: complete
---

# Phase 13 Plan 03: Public State Envelope and Battery Rollback Summary

**API 8 now exposes a bounded transactional state envelope, with battery rollback and pending SaveKey replay checked across independent machines.**

## Performance

- **Duration:** 35 minutes, measured from plan 13-02 completion.
- **Started:** 2026-11-01T20:37:52Z, after plan 13-02 completed.
- **Completed:** 2026-11-01T21:12:52Z.
- **Tasks:** 3.
- **Files modified:** 15 plan-owned files before this summary.

## Accomplishments

- Exposed `muhtari_state_size`, `muhtari_save_state`, and `muhtari_load_state` with API 8, state format 1, and the exact 64-byte envelope. Public loading validates identity, integrity, field domains, continuation clocks, and aliases before mutating the instance.
- Added API 8 caller and inventory coverage while retaining the frozen API prefixes and existing status values.
- Expanded the battery state case to drive real CPU/RIOT SaveKey traffic on an HSC+SaveKey machine, restore the pending page on an independently loaded peer, compare replayed video/audio, and check changed/equal/refused generation behavior including unsigned wrap.
- Documented that state bytes and pending EEPROM work are rollback state while generation and durable files remain host-owned.

## Verification Receipts

All receipts are local; there are no hosted run URLs.

| Command | Result |
|---|---|
| `cmake --build /private/tmp/muhtari-state-13-03-tdd/build --target muhtari-test-state && ctest --test-dir /private/tmp/muhtari-state-13-03-tdd/build -R '^core\.state\.(battery|lifecycle|aliases)$' --no-tests=error --output-on-failure --output-junit /private/tmp/muhtari-state-13-03-tdd/build/junit-state-03-3-final.xml` | Exit 0; all three focused cases passed. |
| `cmake --build build/ci --target muhtari && cmake --build /private/tmp/muhtari-state-13-03-tdd/build --target muhtari-test-state && ctest --test-dir /private/tmp/muhtari-state-13-03-tdd/build -R '^core\.state\.(payload|fields|aliases|lifecycle|topologies)$' --no-tests=error --output-on-failure --output-junit /private/tmp/muhtari-state-13-03-tdd/build/junit-state-03-tracer.xml` | Exit 0; the public API tracer rerun passed all five selected cases. |
| API v1-v8 direct C17 compile/run matrix; C17 and C++17 header checks | Exit 0; frozen callers and current headers compiled and ran. |
| `cmake -DHEADER="$PWD/include/muhtari.h" -DAPI_DIR="$PWD/tests/api" -P tests/cmake/api_version.cmake` | Exit 0; the API-name guard covered 228 header names at API 8. |
| API-version guard with a temporary extra field | Expected exit 1; the fault control reported the deliberately untested field. |
| `git diff --check` | Exit 0. |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" uat classify-coverage --summary .planning/phases/13-portable-save-load-across-three-interfaces/13-03-SUMMARY.md` | Exit 0; all four deliverables were classified as locally auto-covered, with no schema errors. |

The full `cmake --workflow --preset ci` suite was not rerun. Its existing cache had no registered tests, and enabling tests requires missing pinned wasmtime, 7800basic, and OpenBIOS inputs; fetching those inputs and repeating the full configure were excluded. The pinned rcheevos integration and full CI remain unverified. No ROM/BIOS inputs were fetched, and no hosted, RetroArch, or Phase 12 actions were run.

## Task Commits

1. **Task 1 RED:** `c3b6885` — state-size API contract assertion.
2. **Task 1 GREEN:** `9c9911f` — public portable state API.
3. **Task 2 RED:** `d9de53a` — API 8 compatibility assertion.
4. **Task 2 GREEN:** `21836da` — state format version in API 8.
5. **Task 3 tests:** `1358d56` — battery rollback and pending EEPROM cases.
6. **Task 3 docs:** `6044b65` — battery rollback and host ownership docs.

The summary metadata commit is separate.

## TDD Evidence

- **Task 1 RED:** `core.state.lifecycle` failed on the planned state-size assertion: the loaded machine returned `0` instead of payload size plus the 64-byte envelope. The evidence classifier accepted it as `RED_EVIDENCE_OK`; Task 1 GREEN then passed the tracer rerun.
- **Task 2 RED:** the API 8 caller failed on `versions.state_format == 1` because the implementation still reported `0`. Task 2 GREEN passed the API v1-v8 and header checks.
- **Task 3:** the first failure expected EEPROM commit one frame too early and is not valid RED evidence. After correcting the test to execute through the busy deadline, the behavior passed against the implementation already present from Task 1. No production change or artificial failing control was needed.

## Decisions Made

- Reuse the already implemented battery generation comparison and SaveKey commit path; Task 3 adds public contract coverage rather than duplicating those semantics.
- Keep the existing `core.state.battery` CTest registration; no CMake change was needed.

## Files Created and Modified

- `include/muhtari.h`, `src/internal.h`, and `src/state.c` — public API 8 state declarations, statuses, and transactional envelope implementation.
- `src/instance.c`, `tests/api/v8.c`, ABI/header checks, and test inventories — report and verify the new API while retaining older callers.
- `tests/core/test_state.c` — public state, hostility, battery, pending EEPROM, and continuation cases.
- `docs/embedding.md` and `13-FIELD-PROOF.md` — user contract and source-derived field/rollback evidence.

## User Setup Required

None. This plan adds no external service or owner-side configuration.

## Deviations from Plan

**1. Task 3 TDD RED was unavailable because the production behavior already existed.** The expanded public battery test passed after its timing expectation was corrected. Its first attempt expected the EEPROM page to commit during a frame that ended before the saved busy deadline; that was a fixture error, not a product failure. The test was changed to advance through the bounded busy interval, then passed without a production-code change. No synthetic source regression was introduced to claim a RED result.

**Total deviations:** 1 TDD cadence deviation. No architecture or scope changes.

## Issues Encountered

- The Task 3 test initially assumed the page became due in one frame. Inspecting the saved busy deadline showed that a second frame was required; the final test runs the bounded continuation until the existing commit path completes.
- The test case was already registered in `tests/core/tests.cmake`, so no registration edit was needed.
- The required stub scan found no TODO, FIXME, placeholder, coming-soon, empty-value, or not-available stubs in plan-owned changed files.

## Next Plan Readiness

Plan 13-04 can consume the public envelope and API 8 contract. Plan-level requirements remain pending until their full phase paths and portability evidence complete; STATE-02 is still explicitly unclassified. The full CI/rcheevos evidence and Phase 12 halt remain outstanding. No owner action is needed for this plan; the orchestrator proceeds to the next authorized sequential plan.

---
*Phase: 13-portable-save-load-across-three-interfaces*
*Completed: 2026-11-01*

## Self-Check: PASSED

- All 16 plan-owned changed files and this summary exist.
- All six task commits are ancestors of the plan branch head.
- `git diff --check` and the summary whitespace check passed; the coverage classifier reported no schema errors.
