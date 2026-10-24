---
phase: 07-battery-saves
plan: "02"
status: complete
subsystem: core
tags: [hsc, battery, api, compatibility]
requires:
  - phase: 07-battery-saves
    provides: 07-01 named fixture and exact behavior annotation evidence prerequisite
provides:
  - Production HSC SRAM and optional installed firmware through existing bus owners
  - Additive API 5 stable battery and read-only installed-ROM views with frozen load topology
  - Executed old-layout client and early three-component archive feasibility
affects: [07-03, 07-04, 07-05, 07-07, 07-09]
tech-stack:
  added: []
  patterns:
    - Validate small load metadata and unpublished firmware staging before installing live storage
    - Preserve old struct minima and copy only complete appended input fields
key-files:
  created:
    - tests/api/v5.c
    - tests/api/v4-layout.c
  modified:
    - include/muhtari.h
    - src/internal.h
    - src/instance.c
    - src/cart.c
    - tests/core/test_core.c
    - tests/core/tests.cmake
    - tests/abi/tests.cmake
    - docs/embedding.md
    - docs/hardware/cartridge.md
key-decisions:
  - Keep one authoritative raw battery array and a separate installed const firmware view.
  - Use private fixed firmware staging rather than a large stack candidate or extra load allocation.
  - Treat firmware SHA-1 as identification; unknown explicitly supplied firmware warns and remains accepted.
requirements-completed: []
requirements-contributed: [SAVE-01, SAVE-03]
coverage:
  - id: hsc-executed-tracer
    description: Host restore precedes cartridge execution and changed HSC stores alone advance the retained equality token
    requirement: SAVE-01
    verification:
      - kind: integration
        ref: "core.load.keeps_previous and core.hsc.tracer; focused CTest exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: hsc-hardware-and-firmware
    description: All SRAM and installed ROM bytes agree through permitted CPU MARIA and peek paths with firmware failures preserving live execution
    requirement: SAVE-01
    verification:
      - kind: integration
        ref: "core.hsc.decode and core.hsc.firmware; focused CTest exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: stable-battery-api
    description: Additive queries and frozen topology preserve old media layouts and reject invalid placements transactionally
    requirement: SAVE-03
    verification:
      - kind: integration
        ref: "abi.api_v1 through abi.api_v5, abi.api_v4_layout, abi.api_version.self_test, core.hsc.topology and core.hsc.sizes; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: documentation-and-archive
    description: Public HSC semantics and signature-only limitations match executed behavior and actual local archives fit the unchanged bound
    requirement: SAVE-03
    verification:
      - kind: other
        ref: "Agent read docs against implemented API, bytewise stub and tests; docs.api_names and docs.budgets in full CI; exit 0; URL N/A-local"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset ci; 619 tests plus 6 release tests; exactly three ZIPs sum 218325 <=399937; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 17303
  tasks: 3
  commits: 5
plan_head_before: 7432a27610ab8d3aba70cae971f626b45a9b75ef
plan_head_after: 010e4cdf05102601a7a5c550fbcc440bfc6ee520
duration: 11min
started: 2026-10-24T02:28:40Z
completed: 2026-10-24
---

# Phase 7 Plan 2: HSC embedder path Summary

**HSC hardware, explicit firmware and an additive stable battery API now work through real cartridge execution while frozen API-4 callers retain their layout.**

## Accomplishments

- Added API 5 / ABI 1 / behavior 4 with complete-field media tails, HSC mode/firmware and port overrides, one authoritative raw battery span, firmware identity/warnings and a separate read-only installed-ROM view. Absent storage is NULL/0; active topology is frozen at load with exact lengths 2048/32768/34816. Only HSC hardware is executable in this plan; serial protocol expansion belongs to 07-03.
- HSC SRAM at $1000–$17FF and ROM at $3000–$3FFF use existing cartridge decode after MEN/BIOS/audio priority. A real CPU program reads a restored byte, stores it twice and reads it back; only the changed store increments generation. All SRAM and firmware bytes are compared across CPU, MARIA cartridge access and peeks. Reset preserves committed bytes/token; queries are observers and unsigned wrap is exercised.
- Exact-size supplied firmware is staged/copied and SHA-1 identified. Unknown identity and missing signatures warn independently. Invalid sizes/pairs, conflict with OFF, dual EEPROM placements and failed allocation retain live topology/bytes/token and subsequent execution. Stub ROM is FF except C6/FE; docs explicitly deny routines, formatting, screens and full OpenHSC support.
- Added a separate client with frozen API-4 declarations and no current header, linked against the current archive, plus API-5 exhaustive name coverage. Old minima, future tails, output canaries and NULL/error paths are exercised. Exact inventories include the seven new tests wherever applicable.
- Graduated hardware and ownership prose, corrected preparation's BIOS/HSC and generation conflations, preserved all sample/frame/audio digest values, and measured actual three-component archives below the existing ceiling.

## Task Commits

1. Task 1 RED `dce5efe`; GREEN `52fccab` — missing signature assertion, then production HSC tracer.
2. Task 2 RED `3b0b56c`; GREEN `ad44fa7` — current/frozen clients, exact API-5 negative control and HSC edge tests.
3. Task 3 `010e4cd` — documentation, coordinated live version judges and bounded load staging after full-CI feedback.

The ledger measures five task commits from the recorded pre-plan head through the final task head. Actual tokens are 69,211 realized diff characters divided by four, rounded up, excluding this summary and subsequent tracking.

## Commands, exits and run URLs

Every result here is local; run URL is N/A-local. No log or runtime save/firmware bytes are committed.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^core[.]load[.]keeps_previous$' --no-tests=error --output-on-failure` | 0 | Executed tracer; repeated at the tracer feedback gate before expansion |
| `ctest --preset ci -R '^core[.]load[.]keeps_previous$' --no-tests=error --output-on-failure --output-junit hsc-red.xml` | 8 expected | Selected test actually failed on absent C6/FE signatures, not setup/discovery |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/hsc-red.json --raw` | 0 | RED_EVIDENCE_OK; unchanged actual JUnit report and semantic assessment retained only in ignored build output |
| `ctest --preset ci -R '^abi[.]api_version[.]self_test$' --no-tests=error --output-on-failure --output-junit hsc-api-red.xml` | 8 expected | Actual consumer rejected injected untested_field with exit 1; stale K <= 4 matcher failed against the intended K <= 5 diagnostic |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/hsc-api-red.json --raw` | 0 | RED_EVIDENCE_OK; target assertion semantically inspected before updating its literal matcher |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(abi[.]api|core[.]hsc[.]|core[.]load[.])' --no-tests=error --output-on-failure` | 0 | 17/17 focused checks |
| `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` | 0 | Three actual ZIP components and 6/6 release checks |
| `cmake --workflow --preset ci` | 0 | Final run: 619/619 non-release and 6/6 release tests, all five workflow steps |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 619 tests ran once each |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 6 release tests ran once each |
| `cmake -P build/ci/policy/quality_fixture/hash-check.cmake` with an actual runner report whose sole revision field was changed from 4 to 3 | 1 expected | Actual hashes_board consumer specifically rejected malformed diagnostic field 7, audio_behaviour_revision 3; Python probe asserted both exit and exact diagnostic, exit 0 |
| Python exact three-component ZIP enumeration/sum | 0 | Exactly one library, runner and libretro archive; 218325 <=399937 |
| Python comparison against pre-plan built/audio expected files, normalizing only explicit revision 3-to-4 metadata | 0 | Every old data row, digest, tool pin and numeric sample expectation otherwise byte-identical |
| `scripts/hygiene.sh --tree` and `git diff --check` | 0 each | No findings |

Final real archive measurement after the passing workflow:

| Filename | Bytes |
|---|---:|
| `muhtari-library-macos-arm64.zip` | 70,261 |
| `muhtari-runner-macos-arm64.zip` | 78,809 |
| `muhtari-libretro-macos-arm64.zip` | 69,255 |
| Total | 218,325 |

This is early local feasibility. No six-native/worst-platform acceptance, hosted persistence or final phase acceptance is claimed.

## TDD Gate Compliance

Task 1 RED executed the pre-existing load-keeps-previous test and failed specifically on absent HSC signature reads; it compiled and discovered exactly one target. The actual JUnit output passed the classifier and semantic review before GREEN. GREEN added the complete production path and expanded the test through import, cartridge instructions, changed/same stores and power cycle. The tracer feedback gate repeated successfully before Task 2.

Task 2 RED used the actual API name consumer and its injected-field negative control. The consumer correctly produced the new API-5 finding while the old matcher rejected it. The unchanged actual JUnit report passed machine classification; semantic inspection established the planned version-diagnostic assertion failed. GREEN changed the specific literal matcher, kept required exit 1/untested_field rejection, completed hardware checks and ran all 17 focused cases. No refactor-only stage was needed. The first build attempt for Task 1 had a local-shadow compiler error; it was corrected before capturing valid RED and never authorized implementation. Global TDD mode is false; task-level procedures were followed.

## Deviations from Plan

1. **Rule 3 — coordinated version integration.** Full CI exposed old API-4 assertions in the standalone C/C++ header controls and a behavior-3 guard in the existing diagnostic driver. Updated those literals to API 5/behavior 4. The audio corpus likewise carries a behavior field on each row; advanced only that field from 3 to 4, keeping every frame/sample/rate/synthesis/hash value unchanged. These necessary dependent files were omitted from the plan's file list. Exact old-byte comparison and passing existing consumers prove the scope. Commit `010e4cd`.
2. **Rule 1 — load candidate stack footprint.** The initial full battery candidate caused Apple clang to introduce forbidden chkstk_darwin. Replaced it with small metadata and a fixed per-instance 4096-byte unpublished firmware staging buffer. The one persistent data array remains authoritative; staging is never exposed, persisted or read by emulation. No new allocation, compiler helper exception or dependency was admitted. Existing abi.undefined, firmware failure/alias tests and full CI pass. Commit `010e4cd`.
3. **Rule 3 — exact inventory maintenance.** Added new API/HSC registrations to the applicable inventories during this plan so its real CI can run; deferring them until 07-04 would make configure refuse the new tests. No existing inventory entry was removed. Commits `3b0b56c`/`ad44fa7`.

These fixes consumed no phase code-review fix round or gap round; they were current-task implementation feedback. No bound or predicate was relaxed.

## Architecture and claims review

Instance loading owns validation/staging/commit, cartridge decode owns mapped HSC accesses, and ports own future serial protocol. Queries expose the actual single persistent span rather than mirrored RAM or another memory kind. Existing allocators, SHA-1, bus owners, CTest and producer/consumer controls were reused without dependencies or new registries. Tests derive byte patterns and SHA-1 expectations independently and exercise the real public API, CPU and decode paths.

The signature-only transparency prohibition is supported by an exhaustive bytewise stub test and matching public prose, not firmware compatibility inference. Its descriptor-less flagged-unverified metadata remains for the phase review disposition; this plan does not manufacture an owner check or check descriptor. All ten original assumption flags retain their lifecycle until their assigned evidence/review closes them. Serial protocol, host files and fresh-process persistence remain later evidence.

## Self-Check: PASSED

Both new API source files and all modified owners exist; all five task commits are reachable. Final local CI, exact inventories, three-archive bound, protected digest comparison, hygiene and whitespace checks passed after the final code changes. No unexpected deletion, untracked generated file, unrun plan verification, missing feature stub or new unplanned security surface remains. The intentional signature-only ROM is the specified product limitation. Ready for 07-03; the phase and whole SAVE requirements remain pending.
