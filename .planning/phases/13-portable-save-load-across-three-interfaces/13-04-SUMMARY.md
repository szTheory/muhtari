---
phase: 13-portable-save-load-across-three-interfaces
plan: "04"
subsystem: core-state
tags: [c, portable-state, property-testing, fuzzing]
requires:
  - phase: 13-portable-save-load-across-three-interfaces
    provides: API 8 caller-owned state envelope and battery rollback contract from plan 13-03
provides:
  - Independent continuation observations for representative supported board, region, device, and expansion topologies.
  - Seeded properties for canonical repeated continuation, restore refusal, and preservation of independent machine state.
  - Bounded hostile-state recipes in the existing run-fuzz replay consumer.
affects: [state-verification, phase-13-portable-save-load-across-three-interfaces]
actuals:
  tokens: 19679.75
  tasks: 3
  commits: 5
plan_head_before: 35de5600e03d70b25df3027b4f2b2a27af4c6d3f
plan_head_after: d4079a663ffc4b0bb9f70b415ebf139cf27863ef
commits: 5
tech-stack:
  added: []
  patterns:
    - Use independent RAM, pin, pixel, and numeric observations alongside canonical state and resumed output comparisons.
    - Generate seeded state mutations in existing property consumers without shared mutable test storage.
    - Treat fuzz input bytes as bounded recipe selectors and reuse the existing scratch allocation for state slices.
key-files:
  created: []
  modified:
    - src/state.c
    - tests/core/test_state.c
    - tests/prop/test_prop.c
    - tests/prop/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/fuzz/run.c
    - .planning/phases/13-portable-save-load-across-three-interfaces/13-CONTINUATION-MATRIX.md
    - .planning/phases/13-portable-save-load-across-three-interfaces/13-FIELD-PROOF.md
key-decisions:
  - Preserve the unresolved STATE-02 automatic shape classification and edge-probe row; the explicit hostile-state matrix remains the contract.
  - Use generated media and software observations only; do not claim SOUPER, mid-MARIA-burst snapshots, physical equivalence, or cross-platform portability from these local runs.
patterns-established:
  - Every continuation representative has a test-owned independent observation that can detect common-mode state omissions.
  - Hostile fuzz recipes repair the state envelope CRC and assert intended status plus next-run preservation.
requirements-completed: []
coverage:
  - id: D1
    description: Independent continuation checks cover representative supported software topologies with mailbox, pin, pixel, storage, audio, and counter observations.
    requirement: STATE-03
    verification:
      - kind: unit
        ref: "Direct C17 core harness: test-state topologies, fields, and battery; all exit 0; full commands in 13-CONTINUATION-MATRIX.md."
        status: pass
    human_judgment: false
  - id: D2
    description: Seeded properties compare independent continuation and repeated canonical saves, and verify malformed or incompatible restores preserve future observations and metadata.
    requirement: STATE-02
    verification:
      - kind: unit
        ref: "Direct C17 property harness: state_continuation and state_refusal, 100 cases each at seed 20261020; both exit 0; commands in 13-CONTINUATION-MATRIX.md."
        status: pass
    human_judgment: false
  - id: D3
    description: Existing run-fuzz replay reaches repaired-CRC HALT, PCM-count, and cross-owner clock validators using bounded recipes and checks refusal preservation.
    requirement: STATE-04
    verification:
      - kind: integration
        ref: "Direct C17 fuzz replay: test-fuzz-run --seeds, 123 inputs, exit 0; ASan/UBSan harness replay also exit 0; commands in 13-CONTINUATION-MATRIX.md."
        status: pass
    human_judgment: false
  - id: D4
    description: Repeated battery changed, equal, and refused restore behavior is included in the independent core and seeded refusal checks.
    requirement: STATE-05
    verification:
      - kind: unit
        ref: "Direct C17 core harness battery selector and prop.state_refusal, exit 0; commands in 13-CONTINUATION-MATRIX.md."
        status: pass
    human_judgment: false
duration: 17m
completed: 2026-11-01
status: complete
---

# Phase 13 Plan 04: Independent State Continuation Summary

**Representative topology checks, seeded continuation and refusal properties, and bounded hostile-state fuzz recipes now exercise independent state observations across the existing test consumers.**

## Performance

- **Duration:** 17 minutes from the first plan task commit through the final scoped verification and summary preparation.
- **Started:** 2026-11-01T21:36:54Z (first task commit).
- **Completed:** 2026-11-01T21:53:15Z.
- **Tasks:** 3.
- **Files modified:** 9 plan-owned files before this summary.

## Accomplishments

- Built a 13-row representative topology matrix with independent mailbox, pin, pixel, storage, pending-work, audio, and counter observations. The core selectors compare uninterrupted and restored output and include a deliberate independent-oracle fault control.
- Added `prop.state_continuation` and `prop.state_refusal`, using the existing deterministic seed/case/shrink runner. Each passed 100 cases with seed 20261020; the refusal property covers repaired-CRC invalid fields, compatibility failures, aliases, input shape errors, future execution preservation, and battery metadata behavior.
- Extended the existing `fuzz.regress.run` consumer with bounded repaired-CRC state recipes. It reuses the existing cartridge scratch allocation for three fixed-size state slices and verifies valid `UINT64_MAX` HALT sentinel handling plus refusal preservation after hostile HALT, PCM count, and cross-owner clock recipes.
- Fixed state validation to classify inactive PCM ring cells using the candidate payload's decoded ring cursor and count. This allows a valid nonempty PCM continuation to load into an independent machine.

## Verification Receipts

All evidence is local; there are no hosted run URLs.

| Command | Result |
|---|---|
| Strict C17 core harness build from `tests/core/test_state.c` and `tests/cart/images.c`, linked against `build/ci/libmuhtari.a` | Exit 0. |
| `/private/tmp/muhtari-state-13-04/test-state topologies`, `fields`, and `battery` | Each exited 0. |
| Strict C17 property harness build from `tests/prop/test_prop.c`, `tests/fuzz/run_seeds.c`, and `runner/sha256.c` | Exit 0. |
| `/private/tmp/muhtari-state-13-04/test-prop state_continuation /does/not/exist`; same command with `state_refusal` | Both exited 0; each ran 100 cases with seed 20261020. |
| Strict C17 `MUHTARI_FUZZ_REPLAY` harness build from `tests/fuzz/run.c` and `tests/fuzz/run_seeds.c` | Exit 0. |
| `/private/tmp/muhtari-state-13-04/test-fuzz-run --seeds` | Exit 0; replayed 123 inputs in 2.29 seconds. |
| ASan/UBSan replay harness build and `/private/tmp/muhtari-state-13-04/test-fuzz-run-sanitized --seeds` | Both exited 0; 123 inputs completed in 2.95 seconds. The linked core archive was not sanitizer-instrumented. |
| `cmake --build build/ci --target muhtari` | Exit 0; Ninja reported no work to do. |
| `git diff --check 35de5600e03d70b25df3027b4f2b2a27af4c6d3f..HEAD` | Exit 0. |

The exact compiler invocations, focused selector commands, replay details, and local limitations are recorded in [13-CONTINUATION-MATRIX.md](./13-CONTINUATION-MATRIX.md). The configured `build/ci` cache has `MUHTARI_TESTS=OFF`, so the requested CTest selectors could not run without reconfiguring. Full CI and rcheevos integration remain unavailable because `build/pins/wasmtime`, `build/pins/7800basic`, and `build/pins/openbios` are missing. No ROM, BIOS, pinned asset, or package was fetched or generated. Cross-platform, hosted, RetroArch, physical-hardware, and Phase 12 evidence was not attempted.

## Task Commits

1. **Task 1: Exercise every supported continuation owner through independent observations** — `0c37886` (test; includes the candidate PCM-ring validation fix).
2. **Task 2: Add seeded repeated-state and rejection-preservation properties** — `d70aea6` (test), `2b7b945` (fix: remove shared mutable test storage), and `7eb3655` (docs: correct fixture comments).
3. **Task 3: Reach hostile state validators through the existing run fuzz consumer** — `d4079a6` (test).

The task commits are all descendants of plan base `35de5600e03d70b25df3027b4f2b2a27af4c6d3f`; the summary metadata commit is separate.

## TDD Evidence

- **Task 1 RED:** Before the validator change, the exact-source topology case rejected a valid saved PCM ring (`pcm_read=5`, `pcm_count=2`) with `MUHTARI_STATUS_ERR_INVALID_STATE`. After validation used the candidate payload's cursor and count, `topologies`, `fields`, and `battery` all passed.
- **Task 2:** The first refusal-property run exposed a test-oracle mistake: a private digest included host battery tokens and other host-only fields. The property was corrected to compare public canonical state bytes and battery metadata separately. That failure is not counted as product RED evidence; the final two properties passed 100 seeded cases each.
- **Task 3:** The new recipe checks passed through the existing replay consumer, but no pre-change product RED was recorded. The plan adds evidence to existing behavior and does not claim an artificial implementation failure.

## Decisions Made

- Preserve the unresolved STATE-02 automatic shape classification and flagged edge-probe row rather than inferring a new contract.
- Keep SOUPER unsupported. The matrix exercises accepted software profiles but does not imply mid-MARIA-burst save points, physical equivalence, OpenBIOS compatibility, or cross-platform results.
- Keep the existing fuzz case names, bounded run recipe, and timeout budgets; the full run replay measured 2.29 seconds before sanitizer instrumentation and 2.95 seconds with an ASan/UBSan harness.

## Files Created and Modified

- `src/state.c` — validates inactive PCM cells against the candidate payload's decoded ring state.
- `tests/core/test_state.c` — independent topology observations, continuation comparisons, and fault sensitivity.
- `tests/prop/test_prop.c`, `tests/prop/tests.cmake`, and native/WASI inventories — deterministic seeded state continuation and refusal properties.
- `tests/fuzz/run.c` — bounded hostile state mutation and preservation assertions in the existing replay consumer.
- `13-CONTINUATION-MATRIX.md` and `13-FIELD-PROOF.md` — topology rationale, field coverage, exact commands, and evidence boundaries.

## Deviations from Plan

**1. [Rule 1 - Bug] Fixed valid independent PCM continuation rejection.**
- **Found during:** Task 1.
- **Issue:** Validation used destination live PCM ring state when classifying candidate payload ring cells, so a valid nonempty saved FIFO was refused on an independent instance.
- **Fix:** Use the decoded candidate payload's ring cursor and count for that validation.
- **Files modified:** `src/state.c`, `tests/core/test_state.c`.
- **Verification:** The original topology failure became a pass; `topologies`, `fields`, and `battery` selectors all exited 0.
- **Committed in:** `0c37886`.

**2. Task 2's initial refusal-property failure was a test-oracle error, not accepted RED evidence.** The oracle compared private digest state including host-only battery tokens. The final property compares public canonical bytes and battery metadata separately. The review also removed shared mutable test globals and made all three hostile mutations run deterministically in every case (`2b7b945`).

**3. The Task 2 and Task 3 additions did not retain product-level RED runs.** Task 2's initial failure was corrected as a test defect, and Task 3 extends evidence around existing state validation. This is recorded as a TDD cadence limitation rather than represented as a product regression.

**Total deviations:** 1 correctness fix, 2 TDD evidence limitations, and 1 review fix within plan scope. No architectural changes or external inputs were introduced.

## Issues Encountered

- No CTest selectors were available in the existing cached build because `MUHTARI_TESTS=OFF`; focused source harnesses were compiled and run directly without repeating CI configuration.
- Full CI/rcheevos verification remains blocked by missing pinned build inputs (`wasmtime`, `7800basic`, and `openbios`).
- No stubs, skipped tests, TODOs, or new threat surfaces were found in plan-owned changed files. `.planning/WINDOWS.md` was not modified because its existing mismatch is known and the plan instruction prohibited retrying it.

## Next Plan Readiness

Plan 13-04 provides named core, property, and fuzz evidence for the next sequential phase plan. The broader STATE-01 through STATE-05 requirements remain pending until the required native/big-endian and interface evidence is complete; STATE-02 classification remains explicitly unresolved. Full CI/rcheevos and the Phase 12 halt remain open. No owner action is required; the root orchestrator proceeds with the next authorized plan.

---
*Phase: 13-portable-save-load-across-three-interfaces*
*Completed: 2026-11-01*

## Self-Check: PASSED

- The summary file exists and its four coverage entries passed `uat classify-coverage` with no schema errors.
- All five task commits (`0c37886`, `d70aea6`, `2b7b945`, `7eb3655`, `d4079a6`) are ancestors of the plan head.
- The plan diff and summary whitespace checks passed; the summary records the full CI and portability limits.
