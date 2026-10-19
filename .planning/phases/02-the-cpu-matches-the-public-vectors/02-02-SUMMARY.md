---
phase: 02-the-cpu-matches-the-public-vectors
plan: 02
subsystem: cpu
tags: [sally, 6502, decimal-mode, singlesteptests, jam, ane-lxa]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 1's muhtari_cpu OBJECT library, the flat vector bus, the harness and the committed 256-opcode sample
provides:
  - NMOS decimal ADC, SBC and ARR in src/cpu.c (RRA and ISC through them)
  - cpu.vectors.00 to cpu.vectors.ff, 256 registered per-opcode tests
  - the harness's jammed assertion for the twelve JAM opcodes
  - muhtari-test-cpu (one case per argv) and cpu.unstable.sally_constant
  - MUHTARI__ANE_LXA_SALLY, the default profile's ANE and LXA constant
  - docs/hardware/sally.md and the docs/hardware/*.md budget
affects: [02-03 cpu.vectors.reader and fixtures, 02-06 bus.c and the NMI cases in muhtari-test-cpu, 02-16 timeouts and probe close]

actuals:
  tokens: 10100
  tasks: 3
  commits: 3
plan_head_before: e5476542d1a72eb73587c144ef8ffbf541cf5766
plan_head_after: 3168cc435c5cff221ba4ca3f36c62d1e80bcf8c4

tech-stack:
  added: []
  patterns:
    - "Per-opcode tests come from a CMake foreach over RANGE 255 with lowercase hex built from a digit string; the inventory lines come from a shell loop, then C-locale sort"
    - "CPU rule tests are cases of muhtari-test-cpu, hand-assembled const uint8_t[] programs between clang-format off and on, one instruction per line"
    - "A profile default that a CPU test pins is a MUHTARI__ macro in internal.h, shared by muhtari_create and the test, because CPU test programs never link muhtari"

key-files:
  created:
    - tests/cpu/test_cpu.c
    - docs/hardware/sally.md
  modified:
    - src/cpu.c
    - src/internal.h
    - src/instance.c
    - tests/cpu/test_vectors.c
    - tests/cpu/tests.cmake
    - tests/cpu/targets.cmake
    - tests/cmake/budgets.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt

key-decisions:
  - "The default ANE and LXA constant is named MUHTARI__ANE_LXA_SALLY in internal.h, so cpu.unstable.sally_constant pins the shipped default without linking the library"
  - "SBC in decimal mode computes A from the tutorial's sequence 3 first, then runs the binary path for every flag and replaces A"
  - "No ci.yml timeout changed: be32 took 45 s with the 256 vector tests (43 s before), inside its budget"

patterns-established:
  - "CPU rule tests: muhtari-test-cpu <case>, exit 0 pass, 1 fail, 2 usage or unknown case"

requirements-completed: [CPU-01]

coverage:
  - id: D1
    description: "All 256 opcodes match the committed sample on final registers, raw P, RAM and every bus cycle, decimal mode included, with no waiver"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -L '^vectors$' --output-on-failure (exit 0, 257 tests)"
        status: pass
      - kind: other
        ref: "test $(ctest --test-dir build/ci -N -R '^cpu\\.vectors\\.[0-9a-f][0-9a-f]$' | grep 'Test *#' | wc -l) = 256"
        status: pass
    human_judgment: false
  - id: D2
    description: "Decimal ADC, SBC, RRA, ISC and ARR, and every other opcode, match all 10,000 upstream tests of each 6502/v1 file"
    requirement: CPU-01
    verification:
      - kind: other
        ref: "muhtari-test-vectors <scratch>/sst/6502/v1/XX.json XX for XX in 00..ff (every one 0 of 10000 failed)"
        status: pass
    human_judgment: false
  - id: D3
    description: "The twelve JAM opcodes set jammed and every other opcode leaves it clear, asserted by the harness on every test"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -L '^vectors$' (cpu.vectors.02 and the other JAMs pass with the assertion)"
        status: pass
    human_judgment: false
  - id: D4
    description: "With the default constant, LDA #$00; LDX #$FF; ANE #$FF and LDA #$00; LXA #$FF leave A = $00"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.unstable\\.sally_constant$'"
        status: pass
      - kind: other
        ref: "muhtari-test-cpu unstable.sally_constant with the constant mutated to $EE (exit 1, a expected $00 got $ee on both programs)"
        status: pass
    human_judgment: false
  - id: D5
    description: "docs/hardware/sally.md has every section src/cpu.c cites, and docs.budgets holds docs/hardware pages to 40,960 bytes"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^docs\\.budgets$'"
        status: pass
      - kind: other
        ref: "cmake -DROOT=<scratch with a 41,000-byte docs/hardware/big.md> -P tests/cmake/budgets.cmake (exit 1, names big.md)"
        status: pass
    human_judgment: false
  - id: D6
    description: "The probe PR's ci.yml run on the head is green on every leg with the 256 vector tests, be32 under qemu included"
    requirement: CPU-01
    verification:
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39180164525"
        status: pass
    human_judgment: false

duration: 9min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 2: Every opcode of SALLY on the public vectors Summary

**NMOS decimal ADC, SBC and ARR from the 6502.org tutorial and 64doc bring SALLY to all 256 opcodes on the committed sample, and on all 2,560,000 upstream tests. The harness now asserts `jammed` for the twelve JAMs, a new `muhtari-test-cpu` pins the $00 ANE/LXA default, and `docs/hardware/sally.md` records the facts. Every hosted leg of probe PR #9 is green.**

## Performance

- **Duration:** 9 min
- **Started:** 2026-10-19T00:11:39Z
- **Completed:** 2026-10-19T00:20:33Z
- **Tasks:** 3
- **Files modified:** 11

## Accomplishments

- `adc` takes Appendix A's sequence 1 for A and C when D is set, with N and V from the sum before the high-digit correction and Z from the binary sum. `sbc` keeps every flag binary and takes A from sequence 3 in unsigned arithmetic. `arr` gains the 64doc decimal path. RRA and ISC needed no change.
- `cpu.vectors.00` to `cpu.vectors.ff` (labels `portable,vectors`, TIMEOUT 10, provisional) are registered from a CMake loop. All 256 pass, along with their 256 inventory lines in `ci.txt` and `wasm32.txt`.
- The harness fails any test where `jammed` disagrees with "opcode is one of the twelve JAMs". It never steps a jammed CPU, because every test starts from a cleared CPU.
- `cpu.unstable.sally_constant` runs the two programs on the flat bus with `MUHTARI__ANE_LXA_SALLY` and requires A = $00 after each.
- `docs/hardware/sally.md` (9,255 bytes) covers registers, the status register, cycle order, decimal mode, the unofficial and unstable opcodes, and JAM, with sources. `docs.budgets` now globs `docs/hardware/*.md` at 40,960 bytes.

## Recorded one-off checks (AGENTS rule 6)

- **Full upstream set:** `build/ci/tests/muhtari-test-vectors <scratch>/sst/6502/v1/XX.json XX` for every XX from 00 to ff. Each printed `0 of 10000 failed` (0 opcodes failing). The 33 opcodes plan 1's census listed as failing now pass.
- **RED stand-in for the constant test:** with `MUHTARI__ANE_LXA_SALLY` set to `0xEEu`, `muhtari-test-cpu unstable.sally_constant` exited 1 with `LDA #$00; LDX #$FF; ANE #$FF: a expected $00 got $ee` and `LDA #$00; LXA #$FF: a expected $00 got $ee`. Restored to `0x00u`, it exits 0. An unknown case exits 2.
- **Budget negative control:** `cmake -DROOT=<scratch> -P tests/cmake/budgets.cmake` with a 41,000-byte `docs/hardware/big.md` exited 1 with `docs/hardware/big.md: 41000 bytes, budget 40960`.
- **Local lanes:** `cmake --workflow --preset ci` exit 0 (323 tests). `cmake --workflow --preset hygiene` exit 0 (18 tests). `scripts/hygiene.sh --tree` exit 0. `cmake -DDIR=.github/workflows -P tests/cmake/timeouts.cmake` exit 0 (22 timeouts, each with a run ID or provisional).

## Hosted evidence

- **Run on head `3168cc4`:** https://github.com/szTheory/muhtari/actions/runs/39180164525. `gh run watch --exit-status` exited 0, and `CI required` concluded `success`. Every job succeeded.
- **be32:** 45 s wall time with the 256 vector tests under qemu (`vectors = 14.91 sec*proc (257 tests)`, 288 tests passed). The 5-minute budget leaves room; the previous run took 43 s.
- **Build legs:** linux-x64 10 s, linux-arm64 10 s, macos-arm64 24 s, macos-x64 27 s, windows-x64 41 s, windows-arm64 51 s. Others: nofp 7 s (ubuntu-26.04) and 16 s (arm), wasm32 19 s (286 tests, vectors 4.36 s), asan 12 s, hygiene 16 s, perf 24 s, retroarch 21 s, hash-equality 4 s, CI required 3 s.
- **Timeouts:** no job came near its `timeout-minutes`, so `.github/workflows/ci.yml` is unchanged.

## Task Commits

1. **Task 1: decimal mode, JAM assertion, 256 registered tests:** `c4d9ab4` (feat)
2. **Task 2: the unstable constant, sally.md, the hardware-docs budget:** `f293544` (test), `3168cc4` (docs)
3. **Task 3: the hosted run:** no file change

## Files Created/Modified

- `src/cpu.c`: `adc_binary`, decimal `adc`, `sbc` and `arr`
- `src/internal.h`, `src/instance.c`: `MUHTARI__ANE_LXA_SALLY`
- `tests/cpu/test_vectors.c`: the `jammed` assertion
- `tests/cpu/test_cpu.c`, `tests/cpu/targets.cmake`: `muhtari-test-cpu`
- `tests/cpu/tests.cmake`: the 256-test loop and `cpu.unstable.sally_constant`
- `tests/inventory/{ci,wasm32}.txt`: 257 new lines each, with `cpu.vectors.a9` folded into the loop's lines
- `docs/hardware/sally.md`: the page
- `tests/cmake/budgets.cmake`: the `docs/hardware/*.md` glob

## Decisions Made

- The default constant gets a name in `internal.h`. CPU test programs never link `muhtari`, so they cannot call `muhtari_create`. A shared macro lets the test pin the value that `muhtari_create` actually ships.
- Decimal SBC computes A from the pre-instruction A, v and C before the binary path runs, so the flags and A both come from the same inputs.

## Opcodes the sample showed wrong

Only the 33 decimal-mode users plan 1's census named: ADC `61 65 69 6d 71 75 79 7d`, RRA `63 67 6f 73 77 7b 7f`, ARR `6b`, SBC `e1 e5 e9 ed f1 f5 f9 fd eb`, and ISC `e3 e7 ef f3 f7 fb ff`. The decimal paths fixed all of them. No other opcode needed a change (research A7 held), JAM and the SH* stores included.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 2 - Missing critical] Named the default ANE and LXA constant**
- **Found during:** Task 2
- **Issue:** The default $00 is set only inside `muhtari_create`, which `muhtari-test-cpu` cannot call. A test on a zeroed machine would not catch a changed default.
- **Fix:** Added `MUHTARI__ANE_LXA_SALLY 0x00u` to `src/internal.h`. `src/instance.c` assigns it, and the test runs with it.
- **Files modified:** src/internal.h, src/instance.c
- **Verification:** the $EE mutation run above. `abi.*` and the ci workflow pass.
- **Committed in:** f293544

**2. [Rule 3 - Blocking] clang-format packed the four-byte program one byte per line**
- **Found during:** Task 2 (`hygiene.format` after staging `test_cpu.c`)
- **Issue:** clang-format 21 reflowed the four-element LXA array, which broke the one-mnemonic-per-line layout.
- **Fix:** The program arrays sit between `/* clang-format off */` and `/* clang-format on */`, the same remedy research Pitfall 3 gives for data tables.
- **Files modified:** tests/cpu/test_cpu.c
- **Verification:** `cmake --workflow --preset hygiene` exit 0 with the file staged
- **Committed in:** f293544

---

**Total deviations:** 2 auto-fixed (1 missing critical, 1 blocking). **Impact:** two lines in the core and one formatting fence. No scope change.

## TDD Gate Compliance

Task 2 carries `tdd="true"`, but the behaviour under test (ANE and LXA reading the profile constant) already existed from plan 1, so a test against the real default could not fail first. A mutation run stood in for RED: it failed on the planned A assertion, as recorded above. That evidence is console output, not a TAP, JUnit, swift-testing or unittest report, so it was not passed through `check tdd-red-evidence`. `workflow.tdd_mode` is off for this project. The `test(02-02)` commit `f293544` carries both the test and the macro.

## Issues Encountered

None beyond the deviations.

## Known Stubs

None. Plan 1's stub (binary-only `adc`) is resolved.

## User Setup Required

None.

## Next Phase Readiness

- Plan 02-03 can add `cpu.vectors.reader`, the fixtures and `cpu.vectors.self_test` on top of the 256 tests.
- `muhtari-test-cpu` is ready for the NMI cases (`cpu.nmi.*`) that arrive with the bus.
- CPU-01 is in `requirements-completed` here. REQUIREMENTS.md marks it only when the other plans that declare it finish (`requirements.ready-ids` reported 0 of 1 ready).
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files: src/cpu.c, tests/cpu/test_cpu.c, tests/cpu/test_vectors.c, tests/cpu/tests.cmake, tests/cpu/targets.cmake, docs/hardware/sally.md, tests/cmake/budgets.cmake: all present
- Commits c4d9ab4, f293544, 3168cc4: all ancestors of HEAD; `git rev-list --count e547654..HEAD` = 3

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
