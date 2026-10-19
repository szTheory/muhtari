---
phase: 02-the-cpu-matches-the-public-vectors
reviewed: 2026-10-19T19:30:00Z
depth: standard
files_reviewed: 13
files_reviewed_list:
  - docs/embedding.md
  - docs/hardware/bus.md
  - docs/hardware/sally.md
  - src/bus.c
  - src/cpu.c
  - src/instance.c
  - src/internal.h
  - tests/bus/test_bus.c
  - tests/bus/tests.cmake
  - tests/cpu/test_cpu.c
  - tests/cpu/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/wasm32.txt
findings:
  critical: 0
  warning: 2
  info: 1
  total: 3
status: issues_found
---

# Phase 2: Code Review Report

**Reviewed:** 2026-10-19T19:30:00Z
**Depth:** standard
**Files Reviewed:** 13
**Status:** issues_found

## Narrative Findings (AI reviewer)

## Summary

This re-review covers the fix round (commits be6d152, e0b2a25 and 5d6c2ae, diffed against 4f620d2). I read the 13 files in full where they changed, and the code they call: `events`, `hold`, `latch_nmi` and `rearm` in `src/bus.c`; `jam`, `nmi_due` and `nmi` in `src/cpu.c`; `muhtari__power_on`; and the CPU test bus in `tests/cpu/vector_bus.c`.

The three fixes do what the fix report says:
- `muhtari__bus_request_halt` now ignores a request while `halt_stage != HALT_NONE`. `events` sets `HALT_NONE` before it applies the hold, inside the same bus call, so no caller can see a window half applied.
- `muhtari__bus_raise_nmi` now ignores an edge while `nmi_pending` is set. Every path that clears `nmi_pending` goes through `latch_nmi`, so a pending edge is either latched or dropped under MEN = 0. It never sticks.
- The jammed guard in `muhtari__cpu_step` makes exactly one bus read per step, so ticks always advance. That keeps rule 4's bound for a jammed ROM. `muhtari__power_on` clears `jammed`, `nmi_pending` and `halt_stage`.
- The inventories gained the three new tests in sorted order, with matching labels.

I traced the two new bus tests by hand against `events`, and their expected and wrong-model values are right.

There is no blocker. Two warnings remain:
- The one-at-a-time rule trades a silent overwrite for a silent drop that no caller can detect.
- The new JAM test cannot catch the exact regression its NMI assertion names.

Under rule 7, WR-02 is about test code, so it is a seed, not a requirement.

## Warnings

### WR-01: A HALT window or NMI edge that is ignored is dropped without a word, and the caller cannot tell

**File:** `src/bus.c:248-258`, `src/bus.c:262-270`, `src/internal.h:281-288`
**Issue:** Both entry points return `void`. The previous review's WR-03 objected that a second request silently replaced the first, which "silently corrupts cycle counts". The fix makes the second request silently vanish instead. The CPU still runs through a DMA window it should have lost, or misses a DLI, and nothing reports it.

This rule constrains the caller, and the caller is MARIA, which does not exist yet. A MARIA that schedules ahead would hit it. For example, it might queue line N+1's window before the bus reaches line N's `halt_at`, or raise zone N+1's NMI before zone N's edge latches. All of its later requests would be dropped, and no test or assertion would fail. docs/hardware/bus.md says "MARIA asks for the next window after this one ends", but MARIA has no way to learn whether the bus accepted a request.
**Fix:** Return whether the request was taken. The MARIA side and its tests can then assert that it was:
```c
/* Returns 1 when the window is taken, 0 when it is ignored (gate shut, or
   one already in flight). */
int muhtari__bus_request_halt(muhtari *m, uint64_t at_tick, uint64_t until_tick)
{
    struct muhtari__bus *b = &m->bus;
    if (b->halt_gate_writes < 2u || b->halt_stage != HALT_NONE) {
        return 0;
    }
    ...
    return 1;
}
```
Do the same for `muhtari__bus_raise_nmi`. Then make `bus.halt.one_window_at_a_time` and `bus.nmi.one_edge_at_a_time` check the 0 return as well as the timing.

### WR-02: `cpu.jam.step_reads_ffff` cannot catch a jammed step that polls for an NMI

**File:** `tests/cpu/test_cpu.c:410`, `tests/cpu/test_cpu.c:430`
**Issue:** The test latches an NMI with `nmi_phi2 = phi2` (11 after the JAM's eleven reads), then checks "the latched NMI, not taken". JAM stores no poll mark, and the machine was memset, so `poll_phi2` is still 0. `nmi_due` requires `nmi_phi2 <= poll_phi2`, which is 11 <= 0, so it is false.

The most likely regression is a guard that reads `$FFFF` and then falls through to the usual `if (nmi_due(m)) nmi(m);`. That regression would pass this assertion. The assertion failed against the pre-fix source only because LDA #$55 ran and stored a fresh mark. It never tested the jammed path's NMI rule. This is a seed under rule 7.
**Fix:** Make the NMI due by the CPU's own rule before the jammed steps, so that only an explicit "no NMI while jammed" passes:
```c
    machine.m.bus.nmi_phi2 = machine.m.bus.phi2;
    machine.m.cpu.poll_phi2 = machine.m.bus.phi2; /* due by nmi_due's rule */
    struct muhtari__cpu before = machine.m.cpu;
```

## Info

### IN-01: The JAM doc says a frame still ends, but no frame loop runs the CPU yet

**File:** `docs/hardware/sally.md:158-160`
**Issue:** "so time keeps moving and a frame still ends" describes a run loop that does not exist yet: `src/frame.c` never calls `muhtari__cpu_step`. The per-step guarantee is true and tested. The frame-level claim is a promise that no test checks yet.
**Fix:** Write "so time keeps moving" for now. When the run loop lands, add a test that a frame ends on a jammed ROM.

---

_Reviewed: 2026-10-19T19:30:00Z_
_Reviewer: Claude (gsd-code-reviewer)_
_Depth: standard_
