---
phase: 02-the-cpu-matches-the-public-vectors
plan: 07
subsystem: riot
tags: [riot, mos-6532, timer, pa7, swcha, swchb, power-on, cachegrind]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 6's bus (DEV_RIOT decode, 6-tick cycles, bus.phi2 counting held cycles, the HALT gate and muhtari__bus_request_halt, TIA WSYNC)
provides:
  - src/riot.c, the MOS 6532 with a lazy timer computed from phi2, the timer and PA7 flags, ports with direction registers and 128 bytes of RAM
  - muhtari__riot_read, muhtari__riot_write and muhtari__riot_reset, called by the bus and muhtari__power_on
  - struct muhtari__riot inside struct muhtari
  - muhtari-test-riot with 13 riot.* cases; ctest -L riot lists 16 with the three bus cases
  - docs/hardware/riot.md
  - The measured call-overhead answer for the CPU-to-bus seam (0.31 %)
affects: [phase 3's input (SWCHA, SWCHB pins and the console switches), any later savestate of struct muhtari__riot, phase 3's decision on one translation unit]

actuals:
  tokens: 11243
  tasks: 3
  commits: 2
plan_head_before: f0fa13a705292a56fe340f80c39ff80c0ea7bdfd
plan_head_after: 63bc085c3ab3d2c7e9d70fc25abb8885f441fd52

tech-stack:
  added: []
  patterns:
    - "The timer is stored as the value written, an interval shift and the phi2 count of the writing cycle; INTIM and the flag are computed on read with shifts, so there is no per-cycle work and no 64-bit division"
    - "RIOT cases find the access at p = target with idle_to(m, w, target), where p = bus.phi2 + 1 - w, so held cycles are counted the way the timer counts them"

key-files:
  created:
    - src/riot.c
    - tests/riot/test_riot.c
    - tests/riot/tests.cmake
    - docs/hardware/riot.md
  modified:
    - CMakeLists.txt
    - src/internal.h
    - src/bus.c
    - src/instance.c
    - tests/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/bus.md

key-decisions:
  - "The power-on timer is modelled as 256 written on 1024T in the cycle before the first, so it reads $FF at p = 1, $FE at p = 1025, and the flag stays clear for 262,144 cycles"
  - "PA7's edge detector input is the level XOR the rising-edge select, and the flag sets when it falls; this fits the datasheet's output-pin and polarity-change sentences and is marked as a model in riot.md"
  - "A timer flag cleared by a read stays clear until the next timer write; whether a later wrap sets it again is not modelled, because the datasheet does not say"
  - "Cachegrind gap between the shipped archive and one translation unit is 0.31 % (48.449 vs 48.299 Ir per emulated cycle), under 5 %, so the core stays split and nothing goes to the owner"

patterns-established:
  - "RIOT cases: written(i) writes N = 52 at interval i; intim_at(i, p) and timint_at(i, p) read at p on a fresh machine; timint(m) leaves $ff on the bus first so undriven bits would show"

requirements-completed: [CPU-04]

coverage:
  - id: D1
    description: "INTIM reads N - 1 - (p - 1) / I at 1T, 8T, 64T and 1024T through the real bus, against reading N at once and N - 1 - p / I"
    requirement: CPU-04
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^riot\\.timer\\.reads_n_minus_1$'"
        status: pass
      - kind: other
        ref: "src/riot.c mutated to drop the - 1 in (p - 1), then muhtari-test-riot timer.reads_n_minus_1 (exit 1); source restored, cmp exit 0"
        status: pass
    human_judgment: false
  - id: D2
    description: "The timer flag is clear at p = N I and set at N I + 1; INTIM reads $FF there, counts one per cycle and wraps; the datasheet's $AC at p = 500 on 8T holds"
    requirement: CPU-04
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^riot\\.timer\\.(flag_at_n_interval_plus_1|counts_past_underflow)$'"
        status: pass
    human_judgment: false
  - id: D3
    description: "A timer read in the setting cycle keeps the flag; a later timer read or write clears it; TIMINT does not; a write after a clear sets it again"
    requirement: CPU-04
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^riot\\.timer\\.(read_in_setting_cycle_keeps_flag|access_clears_flag)$'"
        status: pass
      - kind: other
        ref: "src/riot.c mutated so the setting-cycle read clears (keeps_flag exit 1) and so a write keeps the clear (access_clears_flag exit 1); source restored, cmp exit 0"
        status: pass
    human_judgment: false
  - id: D4
    description: "The timer counts HALT-frozen cycles, RDY-held cycles after STA $02, and a 6-tick cycle as one"
    requirement: CPU-04
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^riot\\.timer\\.(counts_through_halt|counts_through_wsync|slow_cycle_counts_one)$'"
        status: pass
    human_judgment: false
  - id: D5
    description: "PA7 driven as an output sets its flag on the falling edge by default and the rising edge after the $0285 write; TIMINT reads $40 with D5-D0 = 0 and clears it"
    requirement: CPU-04
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^riot\\.pa7\\.edge_flag$'"
        status: pass
      - kind: other
        ref: "src/riot.c mutated to ignore the polarity write, and to not clear PA7 on a TIMINT read (each exit 1); source restored, cmp exit 0"
        status: pass
    human_judgment: false
  - id: D6
    description: "SWCHA and SWCHB read (OR & DDR) | ~DDR; power-on gives DDRA = DDRB = ORA = $00, ORB from the profile ($FF, so CTLSWB $14 reads SWCHB $FF, and $EB under $00), the timer at $FF on 1024T and flags clear; the RIOT RAM is 128 bytes at $0480 and $0580 in 6-tick cycles, zeroed at power-on"
    requirement: CPU-04
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^riot\\.(swcha\\.ddr|swchb\\.ddr|power_on\\.defaults|ram\\.window)$'"
        status: pass
      - kind: other
        ref: "src/riot.c mutated so ports read the output register (swcha.ddr, swchb.ddr, power_on.defaults exit 1); source restored, cmp exit 0"
        status: pass
    human_judgment: false
  - id: D7
    description: "ctest -L riot lists 16 tests and passes locally and on the probe PR's head commit on every hosted leg"
    requirement: CPU-04
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -L '^riot$'"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39186310654"
        status: pass
    human_judgment: false
  - id: D8
    description: "docs/hardware/riot.md gives the registers, the timer formula with the datasheet's 417T example and the figure's #416 label, the flags, PA7, the ports, the RAM, the power-on state and the unconnected IRQ, within its budget"
    requirement: CPU-04
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^docs\\.budgets$'"
        status: pass
      - kind: other
        ref: "grep -n '417' docs/hardware/riot.md (exit 0)"
        status: pass
    human_judgment: false
  - id: D9
    description: "The one-off Cachegrind comparison of Ir per emulated cycle, the shipped split archive against cpu.c and bus.c in one translation unit, gives 0.31 %"
    requirement: CPU-04
    verification:
      - kind: other
        ref: "docker run --rm -v <tree>:/src:ro -v <scratch>/cg:/cg:ro ubuntu:26.04 sh /cg/run.sh (exit 0; both valgrind runs exit 0)"
        status: pass
    human_judgment: false

duration: 12min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 7: The RIOT (MOS 6532) Summary

**`src/riot.c` puts the MOS 6532 on the bus:**

- A timer computed from φ2 when it is read, so it counts through HALT, WSYNC and slow cycles at no per-cycle cost.
- The timer and PA7 flags, with the datasheet's clearing rules.
- SWCHA and SWCHB with direction registers, and the profile's ORB at power-on.
- 128 bytes of slow RAM.

13 `riot.*` cases check it through the real bus, each against the nearest wrong model, and `ctest -L riot` lists 16
with the three bus cases. The one-off Cachegrind check finds a 0.31 % call-overhead gap. Probe PR run 39186310654 is
green on every leg.

## Performance

- **Duration:** 12 min
- **Started:** 2026-10-19T01:28:27Z
- **Completed:** 2026-10-19T01:40:46Z
- **Tasks:** 3
- **Files modified:** 12

## Accomplishments

- **Decode:** `bus.c` routes `DEV_RIOT` to `muhtari__riot_read` and `muhtari__riot_write`. A9 (RS) picks the ports
  and timer over the RAM, so the same code serves the 7800 decode (`$0280`, `$0380`, `$0480`, `$0580`) and the
  2600 decode with MEN = 0.
- **Timer** (`docs/hardware/riot.md`, Timer): `struct muhtari__riot` holds N, the interval as a shift and the φ2
  count of the writing cycle. INTIM is `N − 1 − ((p − 1) >> shift)` up to p = N·I, then `(0xFF − (p − N·I − 1)) & 0xFF`.
- **Flags:**
  - TIMINT returns D7 for the timer and D6 for PA7, with D5-D0 = 0.
  - A timer read clears the timer flag except at p = N·I + 1, and a write starts a new timer.
  - Reading TIMINT clears only PA7's flag.
- **PA7:** the edge detector runs after every ORA, DDRA and edge-control write, so it sees output pins too.
- **Power-on:** `muhtari__power_on` calls `muhtari__riot_reset`. That sets ORB from `profile.riot_orb_power_on`, the
  other registers to $00, and the timer to $FF on 1024T, and zero-fills the RIOT RAM.
- **Docs:** `docs/hardware/riot.md` (9.5 KB) has the sections Registers, Timer, Flags, PA7, Ports, RAM, Power-on
  and Interrupts. The `bus.md` memory map now points to it for `$0280` and `$0480`.

## Call-overhead measurement (one-off check)

The harness is `harness.c` and `run.sh`, kept in the session scratch directory and not in the tree. It runs
`LDX #0; loop: INX; STA $1900,X; BNE loop; JMP loop` for 20,000,000 instructions on a power-on instance (MEN set,
locked) from RAM at `$1800`.

- **Container:** `ubuntu:26.04` on arm64 (aarch64), with gcc 15.2.0 and valgrind 3.26.0.
- **Release flags:** `-std=c17 -O3 -DNDEBUG -fPIC -fvisibility=hidden -fno-stack-protector -U_FORTIFY_SOURCE`, as
  CMake's Release build plus `muhtari_core_options`.
- **split:** each core file compiled to its own object and archived. The harness links `libmuhtari.a`, so cpu.c and
  bus.c are separate objects, as shipped.
- **onetu:** the harness `#include`s `src/cpu.c` and `src/bus.c` and links the other objects.

Commands, both inside `docker run --rm -v <tree>:/src:ro -v <scratch>/cg:/cg:ro ubuntu:26.04 sh /cg/run.sh`
(exit 0):

| Build | Command | Exit | I refs | φ2 cycles | Ir per cycle |
|-------|---------|------|--------|-----------|--------------|
| split | `valgrind --tool=cachegrind --cache-sim=no --cachegrind-out-file=/out/split.cg ./split 20000000` | 0 | 3,228,269,516 | 66,631,988 | 48.449 |
| onetu | `valgrind --tool=cachegrind --cache-sim=no --cachegrind-out-file=/out/onetu.cg ./onetu 20000000` | 0 | 3,218,256,542 | 66,631,988 | 48.299 |

The gap is **0.31 %** (split over onetu). It is under the 5 % threshold, so the owner has no phase 3 decision here,
and `src/bus.c` is unchanged. Both builds end at the same φ2, tick count and PC. Afterwards
`git status --porcelain` printed nothing.

## Recorded one-off checks (AGENTS rule 6)

- **Mutation runs:** each built `muhtari-test-riot` and restored the source from a scratch copy (`cmp` exit 0).
  - Task 1: INTIM without the `− 1` in `(p − 1)` made `timer.reads_n_minus_1` fail at every interval.
  - Task 2: a read in the setting cycle clearing the flag (`read_in_setting_cycle_keeps_flag` exit 1); the polarity
    write ignored (`pa7.edge_flag` exit 1); TIMINT not clearing PA7 (`pa7.edge_flag` exit 1); ports reading the
    output register (`swcha.ddr`, `swchb.ddr`, `power_on.defaults` exit 1).
  - A timer write that kept the clear first survived (`access_clears_flag` exit 0). The case then gained a
    clear-then-write check, and the same mutant failed it.
- **Local lanes:**
  - `cmake --workflow --preset ci` exit 0 (367 tests).
  - `ctest --test-dir build/ci -L '^riot$'` exit 0, 16 tests.
  - `ctest --test-dir build/ci -N -L '^riot$'` lists 16.
  - `cmake --workflow --preset hygiene` exit 0 (20 tests).

## Hosted evidence

- **Run on head `63bc085`:** https://github.com/szTheory/muhtari/actions/runs/39186310654. `gh run watch
  --exit-status` exited 0, the conclusion was `success`, and every job concluded success. be32 wall time: 31 s
  (01:39:04Z to 01:39:35Z).

## Task Commits

1. **Task 1: RIOT timer read through the bus, muhtari-test-riot** - `81d57d8` (feat)
2. **Task 2: Flags, PA7, ports, power-on values, RAM, riot.md** - `63bc085` (feat)
3. **Task 3: Cachegrind measurement and hosted run**: no commit. It changed no tracked file; the measurement lives
   in scratch and is recorded here.

## Files Created/Modified

- `src/riot.c`: register select, lazy INTIM, timer and PA7 flags, ports, RAM, RES state
- `src/internal.h`: `struct muhtari__riot`, `MUHTARI__RIOT_RAM_SIZE`, the three RIOT declarations
- `src/bus.c`: `DEV_RIOT` reads and writes go to the RIOT
- `src/instance.c`: `muhtari__power_on` calls `muhtari__riot_reset`
- `CMakeLists.txt`: `src/riot.c` in `muhtari_objs`
- `tests/riot/test_riot.c`, `tests/riot/tests.cmake`, `tests/tests.cmake`: `muhtari-test-riot` and its 13 cases
- `tests/inventory/{ci,wasm32}.txt`: 13 lines each
- `docs/hardware/riot.md`: new
- `docs/hardware/bus.md`: the two RIOT rows of the memory map

## Decisions Made

- **Power-on timer:** it reads as 256 written on 1024T in the cycle before the first. That gives the "$FF at 1024T"
  default from one formula with no special case.
- **PA7 detector:** the input is the level XOR the rising-edge select, and the flag sets when that input falls. It
  covers both datasheet sentences, output pins and polarity changes. `riot.md` marks it as a model. The test reads
  TIMINT after the polarity write, as the plan says, and does not assert that a polarity change sets the flag.
- **Later wraps:** a cleared timer flag stays clear until the next write. `riot.md` says this is not modelled from
  the datasheet. No test asserts it either way.
- **WSYNC case:** it writes 200 rather than 52, so that N·I is past the roughly 113 held cycles at 1T, and the
  check can read at exactly p = N·I and p = N·I + 1.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] The bus page still said the RIOT returns open bus**
- **Found during:** Task 2
- **Issue:** two rows of the `docs/hardware/bus.md` memory map said "the open-bus byte, until the RIOT exists".
- **Fix:** the rows now say the RIOT and its RAM, and point to `riot.md`.
- **Files modified:** docs/hardware/bus.md (outside the plan's file list)
- **Committed in:** 63bc085

**2. [Rule 1 - Bug] A gap in my own case**
- **Found during:** Task 2's mutation runs
- **Issue:** `access_clears_flag` could not tell whether a timer write reset the read-cleared state.
- **Fix:** added a clear, then a write, then a check at the new N·I + 1.
- **Committed in:** 63bc085

**3. [Rule 1 - Bug] The loop target in the harness**
- **Found during:** Task 3
- **Issue:** the plan's example loop, `STA $1800,X`, would overwrite the program at `$1800`.
- **Fix:** the harness stores at `$1900,X`.
- **Files modified:** none in the tree

---

**Total deviations:** 3 auto-fixed (all Rule 1). **Impact on plan:** none on scope.

## TDD Gate Compliance

Task 2 carries `tdd="true"`, and `workflow.tdd_mode` is false. RED ran locally before the implementation: the flag
register was still Task 1's stub, which returns $00. Seven cases failed on their planned assertions, for example
`1T: TIMINT at p = N I + 1: expected $80 got $00` and `TIMINT after PA7 falls: expected $40 got $00`. Five were
already green: `counts_past_underflow`, `swcha.ddr`, `swchb.ddr`, `power_on.defaults` and `ram.window`. Task 1's
action builds the ports, the power-on state, the RAM and the INTIM formula, so those cases landed with it. Following
plans 5 and 6, the tests and the implementation landed together in one `feat(02-07)` commit, and there is no
separate `test(02-07)` commit.

## Issues Encountered

- `hygiene.format` rejected two layouts in Task 2's code. Running `/opt/homebrew/opt/llvm@21/bin/clang-format -i`
  (the hygiene preset's binary) fixed them before the commit.

## Known Stubs

None. The RIOT's input pins read high because nothing drives them yet; `riot.md`, Ports says so. Phase 3's input
connects them.

## User Setup Required

None.

## Next Phase Readiness

- CPU-04 is complete: `ctest -L riot` is green locally and on every hosted leg.
- `docs/testing.md` still does not name the `sally` and `riot` labels. D-21 lists that page for this phase.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files: src/riot.c, tests/riot/test_riot.c, tests/riot/tests.cmake and docs/hardware/riot.md are all present.
- Commits 81d57d8 and 63bc085 are both ancestors of HEAD. `git rev-list --count f0fa13a..HEAD` = 2.

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
