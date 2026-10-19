---
phase: 02-the-cpu-matches-the-public-vectors
plan: 05
subsystem: cpu
tags: [nmi, interrupts, poll-mark, rdy, halt, brk, jam, flat-bus]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 1's cpu.c, flat bus and vector harness; plan 2's 256 green cpu.vectors tests
provides:
  - struct muhtari__cpu poll_phi2, stored once per instruction right after the deciding cycle's bus call
  - The end-of-instruction NMI check, the 7-cycle NMI entry and BRK's $FFFA hijack in src/cpu.c
  - Flat-bus fields nmi_at (a scripted edge by phi2 count) and hold_call/hold_count (a scripted hold)
  - Six cpu.nmi.* cases (labels sally, portable) and a poll-mark check on every cpu.vectors.XX test
  - The NMI section of docs/hardware/sally.md
affects: [plan 6's real-bus NMI latch and WSYNC case, phase 3's MARIA DLI, any later savestate of struct muhtari__cpu]

actuals:
  tokens: 6837
  tasks: 2
  commits: 3
plan_head_before: 141c37c07ece4d1cd1e19dd619c8a02a9046c519
plan_head_after: 12fa3bfd9450bce9f0a27d2d33c9ce789df40b31

tech-stack:
  added: []
  patterns:
    - "The final access of an instruction goes through last_fetch, last_read, last_write or last_implied, which store the poll mark first; paths whose final access is a push or pull call poll_mark explicitly"
    - "CPU timing cases check the expected value and that the nearest wrong model gives a different one"

key-files:
  created: []
  modified:
    - src/cpu.c
    - src/internal.h
    - tests/cpu/vector_bus.c
    - tests/cpu/vector_bus.h
    - tests/cpu/test_cpu.c
    - tests/cpu/test_vectors.c
    - tests/cpu/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/sally.md

key-decisions:
  - "The poll mark is stored right before each instruction's final bus access through last_* helpers, so one store per instruction replaces the seed's per-cycle poll_latch"
  - "BRK stores its mark after cycle 4 and chooses $FFFA after pushing P, comparing against that mark"
  - "The vector harness gives each JAM step a stale mark of before + 12 (past the edge and the JAM's last read), so a JAM that polled or stored a mark fails"
  - "The flat bus's hold counts bus calls from 1 since the machine was cleared (calls), so a hold names one call of one instruction"

patterns-established:
  - "NMI cases: nmi_run(origin, program, size, nmi_at, hold_call, hold_count) and expect_return(what, got, expected, model, wrong)"

requirements-completed: []

coverage:
  - id: D1
    description: "An edge in the penultimate cycle is taken after the instruction and one in the last cycle after the next, each against the wrong model's return address"
    requirement: CPU-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.nmi\\.(penultimate_taken_after|last_cycle_taken_after_next)$'"
        status: pass
      - kind: other
        ref: "src/cpu.c nmi_due mutated to nmi_phi2 != 0, then ctest -R '^cpu\\.nmi\\.' (3 of 4 failed); src/cpu.c restored"
        status: pass
    human_judgment: false
  - id: D2
    description: "A taken branch within its page polls at cycle 1, a page-crossing branch at cycle 3, and LDA zp takes the same cycle-2 edge after LDA"
    requirement: CPU-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.nmi\\.branch_no_cross_polls_early$'"
        status: pass
      - kind: other
        ref: "src/cpu.c branch marking at cycle N - 1 within the page, then ctest -R '^cpu\\.nmi\\.' (branch case failed); src/cpu.c restored"
        status: pass
    human_judgment: false
  - id: D3
    description: "An edge by BRK's cycle 4 vectors through $FFFA with B set in the pushed P and return address BRK + 2; an edge in cycle 5 runs the BRK handler's first instruction, then the NMI with B clear"
    requirement: CPU-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.nmi\\.hijacks_brk$'"
        status: pass
      - kind: other
        ref: "src/cpu.c without the BRK hijack, and with the NMI pushing B set, then ctest -R '^cpu\\.nmi\\.' (hijacks_brk failed each time); src/cpu.c restored"
        status: pass
    human_judgment: false
  - id: D4
    description: "A hold in the deciding cycle moves the poll point and a hold in the last cycle does not, against the start-counted and end-counted models"
    requirement: CPU-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.nmi\\.held_'"
        status: pass
      - kind: other
        ref: "RED: the hold fields declared but not applied, then ctest -R '^cpu\\.nmi\\.held' (held_deciding_cycle_taken_after failed: got $0202, the start-counted address); nmi_due compared with phi2 - 1 (held_last_cycle_taken_after_next failed: got $0202); a start-counted CLC (held_deciding failed)"
        status: pass
    human_judgment: false
  - id: D5
    description: "Every non-JAM cpu.vectors.XX test checks poll_phi2 against the deciding cycle, and every JAM test checks that an edge during its reads stays latched, no NMI is taken and the mark is unchanged"
    requirement: CPU-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -L '^vectors$'"
        status: pass
      - kind: other
        ref: "src/cpu.c mutated four ways (PHA marked after its push, TXS without a mark, JAM falling through to the NMI check, JAM storing a mark), then ctest on cpu.vectors.48, .9a, .02 and .f2 (each failed with a poll, pc or poll_phi2 line); src/cpu.c restored"
        status: pass
    human_judgment: false
  - id: D6
    description: "The probe PR's ci.yml run on the head is green on every leg, MSVC included"
    requirement: CPU-03
    verification:
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39183525965"
        status: pass
    human_judgment: false

duration: 17min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 5: NMI by the phi2 count of the deciding cycle Summary

**SALLY now takes an NMI by φ2 count. Each instruction stores one poll mark, `m->cpu.poll_phi2 = m->bus.phi2`, right after its deciding cycle's bus call returns. At the end of the instruction, an edge latched at or before that mark runs the 7-cycle NMI entry. Because the mark is read from the bus's φ2 count, RDY and HALT holds land where they fall. BRK hijacks to $FFFA by its cycle 4, a JAM never polls, and every `cpu.vectors.XX` test now checks the mark. Probe PR run 39183525965 is green.**

## Performance

- **Duration:** 17 min
- **Started:** 2026-10-19T00:47:56Z
- **Completed:** 2026-10-19T01:05:09Z
- **Tasks:** 2
- **Files modified:** 10

## Accomplishments

- **Where each opcode class stores the mark** (src/cpu.c):

  | Class | Where the mark is stored |
  |-------|--------------------------|
  | Loads, ALU reads, BIT, LAX, LAS, memory NOPs | `read_at` or `load`, through `last_read` |
  | Stores, SAX | `last_write` at the store |
  | SHA, SHX, SHY, TAS | `store_sh`'s write |
  | Read-modify-writes, unofficial ones included | `rmw`'s second write |
  | Immediates, ANE, LXA, ALR, immediate NOPs | `last_fetch` of the operand |
  | Implied and accumulator forms | `last_implied` |
  | PHA, PHP | before the push |
  | PLA, PLP | before the pull |
  | JMP abs and JSR | before the high byte |
  | JMP (ind) | before the pointer's high byte |
  | RTS | before the final fetch |
  | RTI | before pulling PCH |
  | Branches | the opcode fetch's φ2, kept in a local when the branch is not taken or stays in its page; before the final read when it crosses a page |
  | BRK | after cycle 4, the PCL push |
  | JAM, the NMI entry | none |

- **NMI entry:** `nmi()` clears `nmi_phi2`, then makes two dummy reads at PC, pushes PCH, PCL and `(P & ~B) | U`, sets I and reads $FFFA/$FFFB. It stores no mark.
- **BRK:** after pushing P, `nmi_due` against the cycle-4 mark picks $FFFA (and consumes the edge) or $FFFE.
- **Flat bus:**
  - `nmi_at` latches `nmi_phi2 = nmi_at` on the call that takes φ2 from below `nmi_at` to `nmi_at` or past it, unless an edge is already latched.
  - `hold_call` and `hold_count` add whole φ2 counts to one call, counted from 1 by `calls`.
- **Six cases** in `muhtari-test-cpu`, labelled `sally;portable`. Each prints the wrong model's address when it fails.
- **Vector harness:** for each test, `poll - before` must be 4 for BRK, 1 for a three-cycle branch, and N - 1 otherwise. A JAM gets `nmi_at = before + 3` and a stale mark of `before + 12`. It must keep the edge latched and the mark unchanged, and the usual compare (PC, S, RAM and 11 reads) proves that no NMI was taken.

## Recorded one-off checks (AGENTS rule 6)

- **Mutation runs on src/cpu.c**, each built with `cmake --build build/ci --target muhtari-test-cpu muhtari-test-vectors` and restored from a scratch copy (`cmp` exit 0):
  - `nmi_due` as `nmi_phi2 != 0`: 3 of 4 `cpu.nmi.*` failed.
  - Branch within the page marking at N - 1: `branch_no_cross_polls_early` failed.
  - No BRK hijack: `hijacks_brk` failed.
  - The NMI pushing B set: `hijacks_brk` failed.
  - `nmi_due` against `phi2 - 1`: `held_last_cycle_taken_after_next` failed (got $0202).
  - A start-counted CLC: `held_deciding_cycle_taken_after` failed (got $0202).
  - PHA marking after its push: `cpu.vectors.48` failed (`poll expected 2 actual 3`).
  - TXS without a mark: `cpu.vectors.9a` failed.
  - A JAM falling through to the NMI check: `cpu.vectors.02` and `.f2` failed (`pc expected $0d8d got $0000`).
  - A JAM storing a mark: `cpu.vectors.02` and `.f2` failed (`poll_phi2 expected 12 actual 11`).
- **Local lanes:**
  - `cmake --workflow --preset ci` exit 0 (338 tests).
  - `ctest --test-dir build/ci -R '^cpu\.nmi\.'` exit 0 (6 tests).
  - `ctest --test-dir build/ci -L '^vectors$'` exit 0 (266 tests).
  - `cmake --workflow --preset hygiene` exit 0 (20 tests).
  - `scripts/hygiene.sh --tree` exit 0.
  - clang-format 21 `--dry-run --Werror` on every changed C file: exit 0.

## Hosted evidence

- **Run on `768c7cc`:** https://github.com/szTheory/muhtari/actions/runs/39183343441 failed on windows-x64 and windows-arm64. MSVC C4310 rejected `(uint8_t)~FLAG_B` under /WX, and that was fixed in `12fa3bf`.
- **Run on head `12fa3bf`:** https://github.com/szTheory/muhtari/actions/runs/39183525965. `gh run watch --exit-status` exited 0, and `CI required` concluded `success`. No job concluded other than success or skipped.

## Task Commits

1. **Task 1: End-to-end NMI** - `7c39888` (feat)
2. **Task 2: Holds and the per-vector poll check** - `768c7cc` (test)
3. **MSVC fix for Task 1's NMI push** - `12fa3bf` (fix)

## Files Created/Modified

- `src/internal.h`: `poll_phi2` in `struct muhtari__cpu`
- `src/cpu.c`: `poll_mark`, the `last_*` helpers, the branch rule, `nmi_due`, `nmi()`, BRK's vector choice, JAM's early return
- `tests/cpu/vector_bus.{c,h}`: `nmi_at`, `hold_call`, `hold_count`, `calls`
- `tests/cpu/test_cpu.c`: the six `nmi.*` cases
- `tests/cpu/test_vectors.c`: `deciding_cycle`, `check_poll`, the JAM edge
- `tests/cpu/tests.cmake`: the `cpu.nmi.*` registrations
- `tests/inventory/{ci,wasm32}.txt`: six lines each
- `docs/hardware/sally.md`: the NMI section, the holds paragraph and two sources

## Decisions Made

- The mark goes in through the final-access helpers rather than a statement before every final access, so a path cannot store it twice. The path-specific cases (push, pull, branch, BRK) call `poll_mark` directly.
- The JAM's stale mark is `before + 12`. `before + 11` equals the mark a JAM would store after its last read, so that mutation passed until the value moved.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] MSVC rejected the NMI's pushed-P mask**
- **Found during:** Task 2's hosted run
- **Issue:** `(uint8_t)~FLAG_B` truncates a constant: C4310, an error under /W4 /WX on windows-x64 and windows-arm64.
- **Fix:** `(uint8_t)((m->cpu.p & ~FLAG_B) | FLAG_U)`, the form `pull_p` already uses.
- **Files modified:** src/cpu.c
- **Verification:** run 39183525965 green on both Windows legs
- **Committed in:** 12fa3bf

**2. [Rule 2 - Missing check] The JAM branch could not see a JAM that polled**
- **Found during:** Task 2's mutation runs
- **Issue:** the harness zeroes the CPU before each test, so a JAM's stale mark (0) lay before the edge, and a JAM that ran the NMI check still passed.
- **Fix:** the JAM step starts with `poll_phi2 = before + 12`.
- **Files modified:** tests/cpu/test_vectors.c
- **Verification:** both JAM mutations now fail `cpu.vectors.02` and `.f2`
- **Committed in:** 768c7cc

---

**Total deviations:** 2 auto-fixed (1 bug, 1 missing check). **Impact on plan:** both were needed for the plan's own criteria. No scope added.

## TDD Gate Compliance

Task 2 carries `tdd="true"` (`workflow.tdd_mode` is false). The hold cases and their bus fields were written first with the hold not applied. `held_deciding_cycle_taken_after` then failed on its return-address assertion with the start-counted model's $0202. `held_last_cycle_taken_after_next` cannot fail without a hold, so its RED is the end-counted mutation, which failed with $0202. Then the hold was applied in `log_cycle`, and both passed. The behaviour under test was already in Task 1's `src/cpu.c`, so there is no `feat` commit for Task 2. The tests, the bus hold and the poll check landed together as `test(02-05)` `768c7cc`, so that no red commit sits in history.

## Issues Encountered

- `clang-format` is not on PATH. The hygiene lane's `MUHTARI_CLANG_FORMAT` is `/opt/homebrew/opt/llvm@21/bin/clang-format`.

## Known Stubs

None.

## User Setup Required

None.

## Next Phase Readiness

- Plan 6's `src/bus.c` must record `nmi_phi2` as the φ2 count of the latching cycle, counting held cycles. `cpu.c` needs nothing more.
- CPU-03 is not marked complete; plan 6 adds the real-bus latch and its WSYNC case.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files: src/cpu.c (contains poll_phi2), docs/hardware/sally.md (`## NMI`), tests/cpu/test_vectors.c (`poll expected`, `nmi_at`): all present
- Commits 7c39888, 768c7cc, 12fa3bf: all ancestors of HEAD; `git rev-list --count 141c37c..HEAD` = 3

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
