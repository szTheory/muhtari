---
phase: 22-complete-souper-homebrew-mapper
plan: "03"
subsystem: cartridge-state
tags: [souper, snapshots, compatibility, runner, libretro, emscripten]
requires:
  - phase: 22-complete-souper-homebrew-mapper
    provides: Exact Souper profile, CPU/MARIA maps, and lifecycle policy from plans 22-01 and 22-02
provides:
  - Complete Souper format-3 / behavior-8 snapshots with transactional validation and allocation-stable restore
  - Source-qualified legacy and YM snapshot compatibility plus C, runner, loaded libretro, and portable continuation checks
  - Final Souper resource evidence and synchronized integrator documentation
affects: [cartridge, state, runner, libretro, emscripten, v7.0]

actuals:
  tokens: 28493.75
  tasks: 2
  commits: 6
commits: 6
plan_head_before: 088af21798a6a39001da8d3651289a2f0e4c5b43
plan_head_after: 13d498279c9fbedb0fe29cc771a0eb95797a30d7

tech-stack:
  added: []
  patterns:
    - Extend the canonical state visitor only for the matching cartridge topology while preserving prior profile bytes and identity
    - Validate complete snapshot semantics before applying state into stable instance-owned memory
    - Judge consumer continuation independently, then use route hashes as integration evidence

key-files:
  created:
    - docs/emscripten-hosting.md
    - .planning/phases/22-complete-souper-homebrew-mapper/22-REVIEW.md
  modified:
    - src/state.c
    - src/instance.c
    - tests/core/test_state.c
    - tests/core/state_compat.cmake
    - tests/diag/driver.c
    - tests/libretro/observe.c
    - tests/libretro/souper_observe.cmake
    - tests/emscripten/host.mjs
    - tests/cmake/hashes.cmake
    - docs/hardware/cartridge.md
    - docs/runner.md
    - docs/libretro.md
    - docs/testing.md
    - .planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md

key-decisions:
  - "Souper uses format 3 / behavior 8; non-YM remains format 1 / behavior 6, YM remains format 2 / behavior 7, and API 8 / ABI 1 stay unchanged."
  - "Validate all topology, identity, payload, latch, and EXRAM semantics before mutation; retain the existing EXRAM allocation and do not allocate during restore."
  - "Consumer replay and literal observations establish correctness; hashes across separately initialized consumers are not required to be byte-identical."
  - "Keep measurements descriptive and source-qualified; they do not establish physical-cartridge accuracy or change resource quotas."

patterns-established:
  - "Topology-specific state envelopes extend the existing ordered visitor without changing older state identities or bytes."
  - "Portable and native consumer receipts carry strict SHA-256 fields and independently judged continuation results."

requirements-completed:
  - SOUPER-01
  - SOUPER-02
  - SOUPER-03
  - SOUPER-04
  - SOUPER-05
  - SOUPER-06
  - SOUPER-07
coverage:
  - id: SOUPER-01
    description: The exact Souper header and geometry profile is admitted, while malformed and unsupported forms refuse before allocation or mutation.
    requirement: SOUPER-01
    verification:
      - kind: unit
        ref: "cart.souper.profile; cart.load.no_alloc_on_reject; diag.souper_tracer"
        status: pass
    human_judgment: false
  - id: SOUPER-02
    description: CPU bank, EXRAM, aliases, segmented views, and register transitions match literal boundary expectations.
    requirement: SOUPER-02
    verification:
      - kind: unit
        ref: "core.souper.cpu; diag.souper_tracer"
        status: pass
    human_judgment: false
  - id: SOUPER-03
    description: MARIA character remapping and the HALT-sensitive fetch view match independent pixel and transition observations.
    requirement: SOUPER-03
    verification:
      - kind: integration
        ref: "bus.souper.halt_views; diag.souper_tracer"
        status: pass
    human_judgment: false
  - id: SOUPER-04
    description: Reset, reload, separate instances, complete state continuation, and invalid-state transactional refusal are verified.
    requirement: SOUPER-04
    verification:
      - kind: unit
        ref: "core.state.souper; core.souper.lifecycle; tests/core/state_compat.cmake"
        status: pass
    human_judgment: false
  - id: SOUPER-05
    description: The generated diagnostic uses independent literal CPU and MARIA expectations and keeps malformed-profile and non-Souper controls.
    requirement: SOUPER-05
    verification:
      - kind: e2e
        ref: "diag.souper_tracer; tests/diag/expected.csv"
        status: pass
    human_judgment: false
  - id: SOUPER-06
    description: Public C, runner, loaded native libretro, and portable consumers resume the same supported mapper workload without adding BupChip audio behavior.
    requirement: SOUPER-06
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; sh scripts/emscripten.sh build/ci"
        status: pass
    human_judgment: false
  - id: SOUPER-07
    description: Integrator documentation states the exact profile, geometry, reset and state policy, unsupported forms, and software-model evidence limits.
    requirement: SOUPER-07
    verification:
      - kind: other
        ref: "docs/hardware/cartridge.md; docs/runner.md; docs/libretro.md; docs/testing.md; docs/emscripten-hosting.md; docs.budgets; policy.claims_wording"
        status: pass
    human_judgment: false

duration: 94min
completed: 2026-11-14
status: complete
---

# Phase 22 Plan 03: Complete Souper snapshots and consumers

Souper snapshots now preserve every mapper latch and all EXRAM bytes, while the existing legacy and YM snapshot contracts retain their original formats, identities, bytes, and continuation behavior.

## Performance

- **Duration:** 94 minutes, measured from the first plan commit through final verification.
- **Tasks:** 2
- **Files changed:** 32 plan files; the summary and review report are additional planning artifacts.

## Accomplishments

- Added the additive Souper format-3 / behavior-8 state profile, with API 8 and ABI 1 unchanged. The full 32 KiB EXRAM span and all stateful mapper latches are serialized, semantically validated before apply, and restored into the existing allocation without post-load allocation.
- Retained the format-1 / behavior-6 and format-2 / behavior-7 state identities and layouts. The source-qualified compatibility harness uses the genuine pre-Souper producer recorded in plan 22-01 and checks old and current producers in both directions; it also preserves the historical v6 matrix.
- Extended the owned diagnostic through public C, separate runner processes, the loaded native libretro module, and the pinned portable module. Each consumer checks its own continuation and refusal behavior; literal CPU/MARIA values remain the independent oracle.
- Finalized documentation for exact declarations and geometry, mapping and aliases, EXRAM power-cycle versus console Reset, topology-specific state versions, unsupported audio/payload forms, and the software-model evidence limit.

## Resource and Compatibility Evidence

- Souper instance allocation is **557,056 bytes** for the selected 512 KiB ROM plus 32 KiB EXRAM. The portable Souper snapshot is **43,745 bytes**. The current native instance remains **91,720 bytes**, matching the measured pre-Souper instance size. Restore allocation counters remain unchanged.
- The 120-frame local benchmark measured Souper at **2,655,043 milli-fps** (p50 372 µs, p90 409 µs, p99 448 µs, max 495 µs) and the ordinary fixture at **8,270,728 milli-fps** (p50 116 µs, p90 136 µs, p99 171 µs, max 177 µs). These are descriptive measurements on the recorded local toolchain, not hardware performance claims or new cutoffs.
- The final local quality receipt for source `13d4982` records **368,271 archive bytes**, 721 native tests, 19 ABI tests, zero ABI findings, and zero warnings (`source: N/A-local`). The existing authenticated joined GitHub Actions evidence remains historical only: [run 39699892851](https://github.com/szTheory/muhtari/actions/runs/39699892851) reports current/base three-component archives of 399,368 / 399,952 bytes. No remote workflow was dispatched for this plan.
- The pinned portable report identifies core source `9ae23ca0bb5b35d66c880d689bf1b796446203c38bbf71710e7bfff1e6acf3a9`, Souper fixture `1329616ea47630cd10e0813b1005d12c4a3b28aa1dd04f4ed0804cd2afa2a7f7`, and state format 3 / behavior 8. It confirms two own-consumer replays and transactional refusal preservation. Native and portable consumer hashes remain independently recorded; cross-consumer byte equality is not assumed.

## Checks

- `cmake --workflow --preset ci` — passed: 721 native tests and the release/package checks (8 total); exact CI, release, and portable inventory audits passed.
- `sh scripts/emscripten.sh build/ci` — passed on the final reviewed tree: all 7 pinned portable tests, including Souper replay and control cases; offline libretro verification covered 14 parity frames, state replay, and instance isolation.
- `ctest --preset asan --output-on-failure --no-tests=error -R '^(cart[.]souper[.]profile|core[.]souper[.](cpu|lifecycle)|core[.]state[.]souper|bus[.]souper[.]halt_views|diag[.](souper|souper_tracer)|libretro[.]host[.]souper)$'` — passed, 9/9 focused mapper, lifecycle, state, diagnostic, and consumer checks. The local `clang-21` alias was unavailable, so the existing AppleClang 21.0 compiler paths were selected for this build without changing repository presets.
- `ctest --preset ci --verbose --no-tests=error -R '^libretro[.]observe[.]souper$'` — passed after receipt validation was tightened.
- `cmake -DROOT=. -P tests/cmake/budgets.cmake` — passed; 24 documentation files are within their existing budgets.
- `sh scripts/hygiene.sh` — passed with exit 0. The targeted staged hygiene check and `git diff --check` also passed during task commits.
- `gsd_run check tdd-red-evidence build/ci/tdd-22-03-task1-red.json --raw` — passed: the named `core.state.souper` test failed on the intended unavailable-state/version assertions before implementation; the committed implementation then passed the focused and full suites.
- No actual RetroArch session, physical cartridge check, host action, remote dispatch, publication, push, or merge was run or implied.

## Task Commits

1. **Task 1 RED:** `fbaaca8` — add failing Souper snapshot version assertion.
2. **Task 1 GREEN:** `5eb8d42` — add complete Souper snapshots and compatibility checks.
3. **Task 2 initial test hypothesis:** `63cb764` — require identical snapshots across separate consumers; this assertion was removed after semantic review.
4. **Task 2 consumer coverage:** `869597c` — exercise portable Souper replay routes.
5. **Task 2 integration and docs:** `ac8eac2` — qualify consumer replay and resource bounds.
6. **Review fix:** `13d4982` — reject truncated Souper state hashes.

## TDD Evidence

Task 1's RED record was classified as `RED_EVIDENCE_OK`: the target test observed behavior 7 / format 2 and unavailable Souper state before the implementation; the format-3 / behavior-8 implementation made it pass. Task 2's first cross-consumer byte-equality assertion was not a valid behavioral contract for independently initialized consumers. It was removed, and each consumer now verifies its own restore and continuation; only source-qualified compatibility within the same profile requires identical serialized bytes.

## Decisions Made

- Souper state is additive and topology-specific: format 3 / behavior 8, while legacy and YM envelopes retain formats 1/6 and 2/7 respectively. API 8 / ABI 1 remain fixed.
- Full semantic validation precedes mutation; restore reuses the current allocation and preserves the existing no-allocation-after-load contract.
- Independent consumer continuation and literal mapping observations are correctness evidence. Cross-route hash agreement is integration evidence only.
- Resource numbers remain descriptive; no unrelated quota, timeout, cache, or CI structure was retuned.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Test bug] Removed an invalid cross-consumer snapshot equality assertion**
- **Found during:** Task 2
- **Issue:** Separately initialized library and adapter consumers do not have a contract requiring byte-identical snapshots at every observation point.
- **Fix:** Removed the equality assertion; each consumer now verifies its own repeated restore and continuation, with independent hashes retained in the receipt.
- **Files modified:** `tests/libretro/observe.c`, `tests/emscripten/host.mjs`
- **Commit:** `869597c`

**2. [Rule 3 - Documentation budget] Moved portable-host detail into its focused guide**
- **Found during:** Task 2
- **Issue:** Expanding the shared embedding/testing guides exceeded the existing 40 KiB per-file documentation budget.
- **Fix:** Kept those guides concise and placed the portable-host instructions in `docs/emscripten-hosting.md`.
- **Files modified:** `docs/embedding.md`, `docs/testing.md`, `docs/emscripten-hosting.md`
- **Verification:** `cmake -DROOT=. -P tests/cmake/budgets.cmake` passed.
- **Commit:** `ac8eac2`

**3. [Rule 1 - Receipt validation] Reject truncated state hashes**
- **Found during:** Phase review
- **Issue:** Receipt controls accepted lowercase hexadecimal digests without requiring the complete SHA-256 length.
- **Fix:** Require exactly 64 lowercase hexadecimal characters for all state and continuation hashes, and add pass/fault coverage.
- **Files modified:** `tests/libretro/souper_observe.cmake`
- **Verification:** Focused observer, full native workflow, and focused ASan checks passed after the fix.
- **Commit:** `13d4982`

The standard review covered 36 files and is clean after the receipt fix. The final stub scan found no placeholder, TODO, FIXME, or empty-value stubs in the changed plan files. The state schema changes are part of the planned, validated snapshot boundary; no additional network, authentication, filesystem, or unplanned trust-boundary surface was introduced.

## Issues Encountered

The optional append to `.planning/WINDOWS.md` was declined because its existing frontmatter counts disagree with its entries (17 open / 35 fixed in frontmatter versus 16 open / 36 fixed in the ledger). The ledger was left unchanged; the plan deviations are recorded above.

## Self-Check: PASSED

- Summary file exists at the required phase path.
- All six task commits are ancestors of the plan head.
- Measured plan commit count is 6, matching `actuals.commits`; the persisted base and plan head are recorded above.
- Coverage classification reports all seven requirements automatically covered with no schema errors.
