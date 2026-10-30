---
phase: 07-battery-saves
plan: "04"
status: complete
subsystem: testing
tags: [battery, properties, i2c, fuzz, lifecycle]
requires:
  - phase: 07-battery-saves
    provides: HSC storage and timed serial EEPROM from plans 02 and 03
provides:
  - Seeded battery lifecycle and observer equivalence properties
  - Shared bounded I2C replay and libFuzzer consumer with a specific invariant fault control
  - Exact native WASM and fuzz inventories retaining existing lane membership
affects: [07-05, 07-08, 07-09]
tech-stack:
  added: []
  patterns:
    - Reuse the fixed seed and shrinking property harness for persistent storage
    - Drive serial state through actual RIOT bus cycles and share replay invariants with libFuzzer
key-files:
  created:
    - tests/fuzz/i2c.c
  modified:
    - tests/prop/test_prop.c
    - tests/prop/tests.cmake
    - tests/core/test_core.c
    - tests/fuzz/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/inventory/fuzz.txt
    - docs/embedding.md
key-decisions:
  - Retain the existing nofp ABI-only inventory, which already includes both new linked clients.
  - Keep the scheduled ASan and UBSan fuzz lane unchanged; local UBSan execution does not substitute for final hosted ASan acceptance.
requirements-completed: []
requirements-contributed: [SAVE-01, SAVE-02, SAVE-03]
coverage:
  - id: battery-lifecycle-property
    description: Six layout and placement combinations retain clean import committed bytes wrapping tokens and continuation after refused loads
    requirement: SAVE-03
    verification:
      - kind: integration
        ref: "prop.battery_lifecycle; 100 and 1000 fixed-seed cases exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: battery-observer-property
    description: Repeated battery ROM and bus observers preserve complete state and time through incomplete serial address traffic
    requirement: SAVE-02
    verification:
      - kind: integration
        ref: "prop.battery_observers; 100 and 1000 fixed-seed cases exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: battery-allocation-lifecycle
    description: All four storage sizes remain allocation-free across frames reset and queries with firmware staging and cartridge allocation refusal
    requirement: SAVE-01
    verification:
      - kind: integration
        ref: "core.no_alloc_after_load plus battery properties and existing firmware tests in full CI; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: bounded-i2c-consumer
    description: Portable replay and fuzz share bounded real-bus invariants for pending data instance region and allocation isolation
    requirement: SAVE-02
    verification:
      - kind: integration
        ref: "fuzz.regress.i2c and fuzz.regress.i2c.self_test; exit 0; specific isolated-instance mutation exits 1 with required diagnostic; URL N/A-local"
        status: pass
      - kind: integration
        ref: "Separate local UBSan libFuzzer build; four I2C checks pass with 3236 runs in 61 seconds; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: complete-local-inventory
    description: Full native CI and release checks include every new property replay and negative control without changing old pins or membership
    requirement: SAVE-03
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; 630 tests plus 6 release checks; exact JUnit inventories exit 0; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 8669
  tasks: 2
  commits: 2
plan_head_before: 5857a4180074fcf811d9fda9f0882726d204911f
plan_head_after: 3628b4af05bc528fe9ef652087e21d6b846a5ddf
duration: 11min
started: 2026-10-24T03:01:54Z
completed: 2026-10-24
---

# Phase 7 Plan 4: Battery robustness Summary

**Fixed-seed lifecycle properties and a bounded shared I2C replay/fuzzer prove storage isolation, observer purity and failed-load continuation without production changes.**

## Accomplishments

- Added `prop.battery_lifecycle` and `prop.battery_observers` to the existing property harness. Cases cycle through absent/HSC/EEPROM/combined storage and both EEPROM ports, then draw data and transaction sizes from the reproducible stream. Existing shrinking replays the same stream at a smaller operation count. Both properties pass 100 and 1000 cases each.
- Clean import precedes execution. Actual HSC writes and RIOT pin page writes update independent expected bytes; same-value HSC stores do not add a change, generation wraps from UINT64_MAX, and interrupted pages do not commit. Frames preserve stable views and peer execution. Malformed media, dual attachment and forced cartridge allocation refusal retain state and subsequent frames; reset retains bytes/token and successful reload starts a new FF baseline without altering the peer.
- Repeated ROM/battery getters and full address-space peeks leave the complete normalized instance state, protocol and tick count unchanged. The property observes an incomplete serial address as well as all non-EEPROM layouts.
- Extended the existing allocation test across all four storage sizes, both EEPROM placements and explicit firmware. Frames, reset and queries allocate nothing after load. Cartridge allocation failure after firmware staging retains the current published span and running cartridge.
- Added `tests/fuzz/i2c.c`, shared by portable replay and libFuzzer. Seven in-memory seeds include empty, full page wrap, both ports, HSC coexistence, interrupted writes, pending reset, every action selector and maximum length. Actions exercise ORA/DDRA, START/STOP/ACK phases, busy waits, pure peeks, reset, invalid selectors and refused hot removal. An independent idle instance and inactive/HSC byte guards detect leakage; equal traffic must leave normalized peers identical.
- Added a narrow compile-time isolated-instance mutation control through that same consumer. The existing `expect_fail.cmake` accepts exactly exit 1 and `i2c invariant: isolated instance changed`; a load/setup failure fails this control. No production failure switch or general test framework was added.
- Registered the portable replay/control in native and WASM inventories, and the target/seeds/replay/control in the existing fuzz lane. No hosted lane or advisory status changed. The nofp inventory was already correct: it contains API-4 layout and API-5 clients and registers only ABI checks. No new property or replay belongs to that scope.

## Task Commits

1. `1b5d4db` — seeded battery lifecycle/observer properties, allocator coverage and embedding documentation.
2. `3628b4a` — shared bounded I2C consumer, fault control and exact inventories.

The measured task diff is 34,674 characters, rounded up after division by four. The two commits are measured between the recorded pre-plan and final task heads; summary/tracking commits are excluded.

## Commands, exits and run URLs

All run URLs here are N/A-local. No log, ROM, save or corpus byte is committed.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(prop[.]battery_|core[.]no_alloc_after_load$|core[.]battery[.]|abi[.]api_v4_layout$)' --no-tests=error --output-on-failure` | 0 | 9/9 focused checks, including the two built fixtures |
| `MUHTARI_PROP_CASES=1000 ctest --preset ci -R '^prop[.]battery_' --no-tests=error --output-on-failure` | 0 | 1000 cases each; lifecycle 3.82 s and observers 3.99 s |
| `ctest --preset ci -R '^fuzz[.]regress[.](i2c|run)$' --repeat until-fail:3 --no-tests=error --output-on-failure` | 0 | I2C 0.03 s in all three runs; existing run replay 0.04 s in all three |
| `ctest --preset ci -R '^fuzz[.]regress[.](i2c|run)' --no-tests=error --output-on-failure` | 0 | Replay and specific fault consumer pass; no omitted/zero selected test |
| `cmake --workflow --preset ci` | 0 | 630/630 non-release tests and 6/6 release checks, all five steps |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 630 tests ran once each |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 6 release tests ran once each |
| `ctest --preset ci -R '^(fuzz[.]regress[.]i2c|prop[.]battery_|core[.]no_alloc_after_load$)' --no-tests=error --output-on-failure` | 0 | Final focused registration check 7/7 after timeout-rationale comment |
| `git diff --check` and `scripts/hygiene.sh --tree` | 0 each | No findings |
| `git diff --exit-code 5857a41 -- tests/built/expected.txt tests/roms/manifest.txt quality.json tests/cmake/quality_join.cmake` | 0 | Old digests, fixtures, numeric predicates and authenticated join unchanged |

Local fuzz configuration used the installed Clang 21 executables explicitly. The ordinary fuzz build compiles successfully. Its binaries time out even in `--write-seeds`, before emulation. A direct replay was interrupted after 55.59 s with exit 130; `sample` exits 0 and locates the spin in recursive ASan shadow-memory initialization through dyld/malloc. This is not accepted ASan execution evidence. No tracked flag or timeout was relaxed.

The following separate, ignored build establishes local UBSan/libFuzzer execution:

`cmake --preset fuzz -B build/fuzz-i2c-ubsan -DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm@21/bin/clang -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm@21/bin/clang++ '-DCMAKE_C_FLAGS=-fsanitize=undefined,fuzzer-no-link -fno-sanitize-recover=all -fno-omit-frame-pointer' '-DCMAKE_CXX_FLAGS=-fsanitize=undefined -fno-sanitize-recover=all -fno-omit-frame-pointer' -DCMAKE_EXE_LINKER_FLAGS=-fsanitize=undefined` — exit 0.

`cmake --build build/fuzz-i2c-ubsan --target muhtari-fuzz-i2c muhtari-fuzz-i2c-replay muhtari-fuzz-i2c-fault && ctest --test-dir build/fuzz-i2c-ubsan -R '^(fuzz[.]i2c|fuzz[.]regress[.]i2c)' --no-tests=error --output-on-failure --output-junit junit-i2c.xml` — exit 0, four checks; 3236 fuzz runs in 61 s, replay 0.06 s.

`cmake --workflow --preset nofp` — exit 1 at its existing GCC-only configure guard on this AppleClang machine. No nofp execution or fresh cross-platform result is claimed. Required final hosted ASan, nofp, 10000-case properties, native/BE and WASM evidence remains assigned to plan 09.

## Bounds and Ownership

The 4096-byte/action cap is an operating-cost bound for hostile serial traffic, large enough for the 66-byte page-wrap sequence, ACK transitions and the emulated busy interval. Each byte triggers at most one pin write, 16 bus reads, a bounded reset/query or rejected load; there are two bounded frames once per whole input. No packet-dependent unbounded loop or host-time wait exists. Replay's observed 0.03 s versus the existing 0.04 s justifies the same integer 1 s floor; normal WASM/host scales still apply. The fuzz target reuses the existing 60 s run, 2 s per-input and 123 s CTest budget. Local UBSan measured the run at 61.01 s and replay at 0.06 s. Hosted timing remains for final acceptance.

The current subsystem owners are unchanged. The property helpers stay local to the property consumer, and serial seeds/invariants stay together in one new fuzz source. `replay_main.h` and `expect_fail.cmake` remain the shared mechanisms. The replay wrapper adds only seed output under an explicitly supplied build directory; it introduces no production file surface or dependency.

## Deviations and TDD Scope

Incremental native/WASM inventory rows were committed with Task 1 because configure enforces exact registrations. The nofp file needed no edit after inspection. No dependency, product interface, allocation, hardware bound or old expectation changed.

Both tasks expand tests over already implemented hardware. Initial focused failures identified test-harness mistakes (the established dual-device error is UNSUPPORTED_DEVICE, and a raw 4 KiB fuzz cartridge requires an A78 header). Those were corrected before acceptance and are not claimed as RED evidence for a missing production behavior. Global TDD enforcement is disabled; no artificial product defect or fabricated RED report was introduced. The new invariant's explicit mutation control proves its rejection path through the actual consumer.

Local ASan startup and the platform-specific nofp refusal are recorded above, with final hosted execution still scheduled in plan 09. They do not leave an unrun plan verification: both task verification commands passed. No review-fix or gap round was consumed, and no owner UAT is requested.

## Limits and Next Readiness

No whole SAVE requirement is marked complete here. Host persistence and final hosted portability remain in later plans. The ten surfaced flags and descriptor-less intent prohibitions are not silently resolved by adding coverage; final phase evidence and review must disposition them.

No known stub, skipped test, product TODO, new unplanned security surface or unrun task verification remains. Plan 05 may now implement the runner save transaction using the verified core contract.

## Self-Check: PASSED

All nine changed task files exist; both task commits are reachable. Full CI, exact inventories, focused final checks and hygiene pass. No tracked file was deleted and no generated output is untracked.
