---
phase: 02-the-cpu-matches-the-public-vectors
plan: 06
subsystem: bus
tags: [bus, tia, wsync, rdy, halt, nmi, inptctrl, open-bus, slow-cycles]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 5's poll mark (poll_phi2) and nmi_phi2 contract in src/cpu.c
provides:
  - src/bus.c, which defines muhtari__bus_read and muhtari__bus_write, with the 7800 and 2600 decodes, 4- and 6-tick cycles, open bus, INPTCTRL, the two-write HALT gate, the fitted HALT window, RDY and the NMI latch
  - muhtari__bus_request_halt and muhtari__bus_raise_nmi, the entry points phase 3's MARIA calls
  - src/tia.c, with WSYNC on the TIA's 456-tick line (tia_phase) and the TIA read drivers
  - muhtari__power_on, called by muhtari_create; 4 KiB of system RAM in struct muhtari
  - the CPU object in the shipped muhtari archive
  - muhtari-test-bus with 16 bus.* cases (13 labelled sally, 3 labelled riot)
  - docs/hardware/bus.md
affects: [plan 7's RIOT (RIOT space returns open bus with 6-tick cycles until then), phase 3's MARIA (HALT, NMI, MARIA WSYNC, MSTAT), any later savestate of struct muhtari__bus]

actuals:
  tokens: 12016
  tasks: 3
  commits: 4
plan_head_before: b34c0ed878516f471e90720ca6d6bfd2330cecf5
plan_head_after: f4ff266c881d76b8117010adab9f4ea0bfed934e

tech-stack:
  added: []
  patterns:
    - "One compare per cycle against bus.next_deadline; RDY low or /HALT low forces the slow path (deadline 0), pending HALT and NMI requests set it to their tick"
    - "Held RDY and HALT cycles are whole 4-tick phi2 cycles added in hold(), which also latches a pending NMI edge at the held cycle's own index"
    - "Bus tests act as the CPU on a static instance after muhtari__power_on, and each timing check names the nearest wrong model and requires that its value differs"

key-files:
  created:
    - src/bus.c
    - src/tia.c
    - tests/bus/test_bus.c
    - tests/bus/tests.cmake
    - docs/hardware/bus.md
  modified:
    - CMakeLists.txt
    - src/internal.h
    - src/instance.c
    - tests/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/cmake/abi.cmake

key-decisions:
  - "HALT_RUN_CYCLES = 2 is the one named fitted HALT constant: /HALT falls at the first cycle boundary at or after the request, c0 and c1 run, and c2 is frozen for m = ceil((until - fall) / 4) whole 4-tick cycles"
  - "A cycle's NMI latch is its data tick (4 fast, 5 slow); a held cycle's is its own tick 4. A raise while MEN = 0 is dropped, and a raise while an untaken edge is latched merges into it"
  - "A held read, after its last held cycle, runs at its own decoded length (4 for an opcode fetch from RAM)"
  - "abi.undefined allows __ImageBase on Windows x64: MSVC addresses cpu.c's switch jump table through it now that cpu.c is in the archive"

patterns-established:
  - "Bus cases: fresh(inptctrl), load(m, program, size) at $1800, expect_ticks/expect_count/expect_byte/expect_address(what, got, expected, model, wrong)"

requirements-completed: [CPU-03]

coverage:
  - id: D1
    description: "The muhtari archive holds the CPU object with src/bus.c, and abi.undefined, abi.globals and abi.symbols pass on it on every hosted leg"
    requirement: CPU-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^(bus\\.cycle\\.fast_4_ticks|abi\\.(undefined|globals|symbols)|cpu\\.vectors\\.a9)$'"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39185135366"
        status: pass
    human_judgment: false
  - id: D2
    description: "Cycle length: fast addresses take 4 ticks, the TIA, the RIOT and their mirrors take 6, $0240 takes 4, and every cycle takes 6 while MEN = 0, each against the other length"
    requirement: CPU-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^bus\\.cycle\\.'"
        status: pass
    human_judgment: false
  - id: D3
    description: "RDY from TIA WSYNC holds reads only, until the TIA line wraps at tick + tia_phase = 0 mod 456; INC $02 takes 26 ticks with both writes landing, and the next fetch is held"
    requirement: CPU-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^bus\\.rdy\\.'"
        status: pass
      - kind: other
        ref: "src/bus.c mutated to hold writes too, and src/tia.c mutated to ignore tia_phase, then muhtari-test-bus rdy.* (each failed); sources restored, cmp exit 0"
        status: pass
    human_judgment: false
  - id: D4
    description: "INPTCTRL locks, keeps four bits ($16 acts as $06) and is written at any TIA mirror; LDA $0240 returns $02; TIA reads drive D7 or D7-D6 over the open-bus byte; MARIA addresses return the open-bus byte"
    requirement: CPU-04
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -L '^riot$'"
        status: pass
      - kind: other
        ref: "src/bus.c mutated to ignore LOCK, then muhtari-test-bus inptctrl.lock (failed); source restored"
        status: pass
    human_judgment: false
  - id: D5
    description: "The HALT gate opens after two unlocked TIA-range writes (mirrors and RMW dummy writes count, locked writes do not); the window gives c0 = c1 = 4 and c2 = 4 + 4m for reads and writes, and starts at a slow cycle's end"
    requirement: CPU-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^bus\\.halt\\.'"
        status: pass
      - kind: other
        ref: "HALT_RUN_CYCLES mutated to 1 (latency_two_cycles, waits_for_slow_fall and freezes_writes failed); the gate mutated to open at 1 write (gate_two_writes failed); source restored"
        status: pass
    human_judgment: false
  - id: D6
    description: "The NMI latch names the cycle whose data tick falls at or after the edge, held cycles included, and nothing while MEN = 0; through the real bus an edge in LDA's last cycle is taken after the next instruction, and an edge in CLC's RDY-held fetch after CLC"
    requirement: CPU-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^bus\\.nmi\\.'"
        status: pass
      - kind: other
        ref: "hold() mutated to latch the held span's last cycle, then muhtari-test-bus nmi.held_fetch_after_wsync (failed: 114 for 25); source restored"
        status: pass
    human_judgment: false
  - id: D7
    description: "ctest -L sally lists 19 tests and passes locally and on the probe PR's head on every leg"
    requirement: CPU-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -L '^sally$'"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39185135366"
        status: pass
    human_judgment: false

duration: 18min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 6: The 7800 bus under SALLY Summary

**`src/bus.c` now drives SALLY's bus in the shipped archive:**

- 4- and 6-tick cycles from the 7800 and 2600 decodes, and an open-bus byte.
- INPTCTRL, and the two-write HALT gate.
- The fitted HALT window (`HALT_RUN_CYCLES = 2`).
- RDY from TIA WSYNC on a 456-tick TIA line, holding reads only.
- An NMI latch that records the held cycle an edge fell in.

16 `bus.*` cases check all of this against the nearest wrong model. Probe PR run 39185135366 is green on every leg.

## Performance

- **Duration:** 18 min
- **Started:** 2026-10-19T01:08:02Z
- **Completed:** 2026-10-19T01:25:45Z
- **Tasks:** 3
- **Files modified:** 12

## Accomplishments

- **Archive:** `muhtari` is now `$<TARGET_OBJECTS:muhtari_objs> $<TARGET_OBJECTS:muhtari_cpu>`, and `src/bus.c` and `src/tia.c` are in `muhtari_objs`. `muhtari_create` calls `muhtari__power_on`, which zeroes the CPU, the bus and the 4 KiB of RAM and sets `next_deadline = UINT64_MAX`.
- **Decode** (`docs/hardware/bus.md`, Memory map):
  - With MEN = 1: the TIA is `$00-$1F` of pages 0-3 (slow), MARIA `$20-$3F` (fast, open bus), RAM `$40-$FF` of pages 0 and 1, which map to `$2040`/`$2140`.
  - `$0240`/`$0340` select nothing (fast). The RIOT ports `$280`/`$380` and RIOT RAM `$480`/`$580` are slow and return open bus until plan 7.
  - RAM is `$1800-$27FF`. Everything else is cartridge space and returns open bus.
  - With MEN = 0, the 2600 decode applies (A12, A7), and every cycle is slow.
- **Fitted values, as named and cited in code and docs:**
  - `HALT_RUN_CYCLES 2u` (src/bus.c, "fitted, not measured"; `docs/hardware/bus.md, HALT`).
  - The WSYNC release: the first tick after the write's data tick at which tick + `tia_phase` is a multiple of `TIA_LINE_TICKS 456u` (src/tia.c; `docs/hardware/bus.md, RDY`, marked fitted).
  - The rule that the held read runs in the first cycle starting at or after the release (same section).
- **Entry points:**
  - `muhtari__bus_request_halt(m, at, until)` does nothing below 2 gate writes.
  - `muhtari__bus_raise_nmi(m, at)` does nothing while MEN = 0.
  - Both re-arm `next_deadline`.
- **Docs:** `docs/hardware/bus.md` (8.9 KB) has the sections Clocks, Memory map, Slow cycles, Open bus, INPTCTRL, RDY, HALT and NMI. The NMI section points to `docs/hardware/sally.md`, NMI.

## Recorded one-off checks (AGENTS rule 6)

- **Mutation runs:** each was built with `cmake --build build/ci --target muhtari-test-bus` and restored from a scratch copy (`cmp` exit 0).
  - RDY holding writes: `rdy.holds_reads_only` and `rdy.rmw_completes_both_writes` failed.
  - `tia_phase` ignored: `rdy.holds_reads_only` failed (462 for 362).
  - LOCK ignored: `inptctrl.lock` failed.
  - `HALT_RUN_CYCLES` 1: `halt.latency_two_cycles`, `halt.waits_for_slow_fall` and `halt.freezes_writes` failed.
  - The gate opening at 1 write: `halt.gate_two_writes` failed.
  - The NMI latched at the held span's last cycle: `nmi.held_fetch_after_wsync` failed (114 for 25). The return-address half alone still passes, because 114 is before the poll mark 115. The bus-level half is the check that catches it.
- **Local lanes:**
  - `cmake --workflow --preset ci` exit 0 (354 tests).
  - `ctest --test-dir build/ci -L '^sally$'` exit 0 (19 tests).
  - `ctest --test-dir build/ci -N -L '^riot$'` lists the three `bus.inptctrl`/`bus.open_bus` cases.
  - `cmake --workflow --preset hygiene` exit 0 (20 tests).
  - `scripts/hygiene.sh --tree` exit 0.

## Hosted evidence

- **Run on `6c73500`:** https://github.com/szTheory/muhtari/actions/runs/39184977244 failed on windows-x64 only. `abi.undefined` rejected `__ImageBase`, which was fixed in `f4ff266`.
- **Run on head `f4ff266`:** https://github.com/szTheory/muhtari/actions/runs/39185135366. `gh run watch --exit-status` exited 0, the conclusion was `success`, and no job concluded other than success or skipped. be32 wall time: 32 s (01:23:42Z to 01:24:14Z).

## Task Commits

1. **Task 1: CPU and bus in the archive, fast cycles** - `5c67941` (feat)
2. **Task 2: Slow cycles, open bus, INPTCTRL, TIA drivers, WSYNC RDY** - `e652c6d` (feat)
3. **Task 3: HALT gate and window, NMI entry point** - `6c73500` (feat)
4. **MSVC x64 `__ImageBase` in abi.undefined** - `f4ff266` (fix)

## Files Created/Modified

- `src/bus.c`: decode, cycle lengths, `rearm`/`events`/`hold`, INPTCTRL and gate, HALT window, NMI latch, both entry points
- `src/tia.c`: `muhtari__tia_read` (driver tables), `muhtari__tia_write` (WSYNC release)
- `src/internal.h`: bus fields, INPTCTRL bits, RAM base and size, `ram[]`, and the declarations
- `src/instance.c`: `muhtari__power_on`, called by `muhtari_create`
- `CMakeLists.txt`: bus.c and tia.c in `muhtari_objs`, and `muhtari_cpu` in the archive
- `tests/bus/test_bus.c`, `tests/bus/tests.cmake`, `tests/tests.cmake`: `muhtari-test-bus` and its 16 cases
- `tests/inventory/{ci,wasm32}.txt`: 16 lines each
- `tests/cmake/abi.cmake`: the `__ImageBase` row for Windows x64
- `docs/hardware/bus.md`: new

## Decisions Made

- After its last held cycle, a held read runs at its decoded length rather than a fixed 4 ticks. The plan's cases all hold opcode fetches from RAM, where the two are the same.
- A raise while an untaken edge is still latched merges into it. A raise whose cycle comes after MEN has cleared is dropped.
- The NMI case through WSYNC checks the latch at bus level (`nmi_phi2` = 25, a held cycle) and then through the CPU (return address `$1803`). `cpu.c` clears `nmi_phi2` inside the same `cpu_step`, so the latch cannot be observed through the CPU run alone.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] MSVC x64 references `__ImageBase` in the archive**
- **Found during:** Task 3's hosted run
- **Issue:** cpu.c now sits in `muhtari`. MSVC on x64 addresses its switch jump table through the linker-defined `__ImageBase`, and `abi.undefined` failed on windows-x64. windows-arm64 passed.
- **Fix:** added a helper row `"^Windows/(AMD64|x64|x86_64)$:__ImageBase"` with its reason, following the ELF `_GLOBAL_OFFSET_TABLE_` row.
- **Files modified:** tests/cmake/abi.cmake (outside the plan's file list)
- **Verification:** run 39185135366 green on windows-x64
- **Committed in:** f4ff266

**2. [Rule 1 - Bug] A wrong expectation in a test of my own**
- **Found during:** Task 3's RED run
- **Issue:** two `nmi.latched_in_cycle` cases put edges at ticks 10 and 9. Both fall before the slow cycle's data tick, 13, so the two cases could not tell the models apart.
- **Fix:** the cases use ticks 13 (the fall: cycle 3) and 14 (the cycle's end: cycle 4).
- **Committed in:** 6c73500

---

**Total deviations:** 2 auto-fixed (1 blocking, 1 test bug). **Impact on plan:** both were needed to land the plan's own criteria. No scope added.

## TDD Gate Compliance

Tasks 2 and 3 carry `tdd="true"`, and `workflow.tdd_mode` is false. Each RED ran locally before the implementation:

- **Task 2:**
  - Four cases failed on their planned assertions: `rdy.holds_reads_only` (10 for 462), `rdy.rmw_completes_both_writes` (34 for 466), `inptctrl.lock` and `open_bus.write_only` ($ff for $3f).
  - `cycle.slow_6_ticks`, `cycle.men_off_all_slow` and `open_bus.unmapped` were already green. Task 1's action specifies the decode, the cycle lengths and the open-bus byte, so they landed there.
- **Task 3:**
  - The entry points were declared as no-ops first. Seven cases then failed on their planned assertions, for example `c2: expected 24 ticks got 4` and `nmi_phi2 expected 25 got 0`.
  - `halt.gate_counts_rmw_dummy` was already green, because Task 2's action counts gate writes.

Following plan 5's convention, each task's tests and implementation landed together in one `feat(02-06)` commit, so no red commit sits in history. There is no separate `test(02-06)` commit.

## Issues Encountered

None beyond the deviations above.

## Known Stubs

- `src/bus.c` returns the open-bus byte for the RIOT, RIOT RAM, MARIA's registers and the cartridge/BIOS space. This is intentional: plan 7 adds the RIOT, and phase 3 adds MARIA and cartridges, as D-15 and the plan specify. None of it blocks this plan's goal.

## User Setup Required

None.

## Next Phase Readiness

- Plan 7's `src/riot.c` plugs into `DEV_RIOT` in `decode()`, and its timer counts `bus.phi2`, which includes held cycles.
- `docs/testing.md` does not yet name the `sally` and `riot` labels (D-21 lists that page for this phase).
- CPU-03 is complete. CPU-04 still needs plan 7's RIOT cases.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files: src/bus.c (`muhtari__bus_request_halt`), src/tia.c, tests/bus/test_bus.c, tests/bus/tests.cmake, docs/hardware/bus.md: all present
- Commits 5c67941, e652c6d, 6c73500, f4ff266: all ancestors of HEAD; `git rev-list --count b34c0ed..HEAD` = 4

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
