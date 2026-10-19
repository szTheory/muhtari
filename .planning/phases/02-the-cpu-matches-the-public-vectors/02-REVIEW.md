---
phase: 02-the-cpu-matches-the-public-vectors
reviewed: 2026-10-19T00:00:00Z
depth: standard
files_reviewed: 4
files_reviewed_list:
  - docs/hardware/bus.md
  - src/bus.c
  - src/internal.h
  - tests/bus/test_bus.c
findings:
  critical: 0
  warning: 1
  info: 2
  total: 3
status: issues_found
---

# Phase 02: Code Review Report

**Reviewed:** 2026-10-19
**Depth:** standard
**Files Reviewed:** 4
**Status:** issues_found

## Summary

This re-review covers fix round 2 (62b6bdc), which makes `muhtari__bus_request_halt` and `muhtari__bus_raise_nmi`
return 1 when they take a request and 0 when they ignore it. The diff base is 2cca8b3. The HALT side is sound. Once
the gate has counted two writes it never closes again, because `halt_gate_writes` only goes up, so a request that
returns 1 always produces its window.

The NMI side over-promises. `raise_nmi` returns 1 for an edge that `latch_nmi` will later throw away without any
signal. A scratch probe confirmed this (details under WR-05). That is the same "the caller cannot tell" gap the fix
was meant to close, just moved one step later. The two info items are a doc sentence that names one reason for a 0
when there are two, and test gaps around the return-1 path. Under project rule 7, the test-gap item is a seed, not a
requirement.

## Warnings

### WR-05: `raise_nmi` returns 1 for an edge that is later dropped at latch

**File:** `src/bus.c:111-117`, `src/bus.c:261-273`, `src/internal.h:286-289`, `docs/hardware/bus.md:154-157`

**Issue:** `muhtari__bus_raise_nmi` returns 1 whenever MEN = 1 and no edge is pending. `latch_nmi` then drops the
edge without any signal in two cases:

1. **An earlier edge is latched but not yet taken.** `nmi_phi2 != 0` (`bus.c:114`), and the CPU clears it only when
   it takes the NMI (`cpu.c:608`, `cpu.c:1168`).
2. **MEN was cleared between the raise and the latch.** This can happen through an unlocked INPTCTRL write.

The bus doc's own sentence on the first case ("a latched edge the CPU has not taken keeps its `nmi_phi2` against a
later one") contradicts "returns ... 1 when it is taken" two lines later. The header says "returns 1 when it takes
the request", which a caller will read as "this edge will be latched".

**Evidence:** I built a scratch probe against `src/*.c`: power on, set INPTCTRL to `$07`, `raise_nmi(m, 6)`, four
reads, then `raise_nmi(m, ticks + 2)` and four more reads. Its output was
`first=1 phi2=2 second=1 phi2_after=2 pending=0`. The second edge returned 1 and was then thrown away.

**Fix:** A raise cannot predict whether the CPU will take the earlier edge before the new one latches. So either
make the drop observable, or narrow the contract so it promises only what the raise knows.

The smallest correct change is to narrow the contract in the header and the doc, for example: "Returns 1 when the
edge is queued for latching and 0 when it is ignored. A queued edge is still dropped at its latch when an earlier
latched edge has not been taken or MEN has been cleared."

If MARIA needs to know, add a counter that the tests can assert:

```c
static void latch_nmi(muhtari *m, uint64_t index)
{
    m->bus.nmi_pending = 0u;
    if (men(m) && m->bus.nmi_phi2 == 0u) {
        m->bus.nmi_phi2 = index;
    } else {
        m->bus.nmi_dropped++; /* edge queued by raise_nmi but never latched */
    }
}
```

Add a `nmi.one_edge_at_a_time` sub-case that raises a second edge after the first has latched, and pin down
whichever contract you choose.

## Info

### IN-04: The HALT doc names one reason for a 0 return; the gate is the other

**File:** `docs/hardware/bus.md:141-143`

**Issue:** The doc says "The request returns 0 when it is ignored and 1 when it is taken, so MARIA can check that it
asked after the last window ended." But a request made while the gate is shut (`halt_gate_writes < 2`, `bus.c:251`)
also returns 0. A MARIA that reads 0 as "I asked before the last window ended" would misreport the gate-shut case,
which happens before the BIOS's two INPTCTRL writes. The NMI paragraph does list both of its reasons ("under MEN = 0
or behind another"), so the two paragraphs are also inconsistent.

**Fix:** Name both reasons, matching the NMI paragraph: "The request returns 0 when it is ignored, with the gate
shut or behind another window, and 1 when it is taken." Drop the "so MARIA can check..." clause, or say that MARIA
must also know the gate state before it can read a 0 that way.

### IN-05: Nothing tests that a request after a window, or an edge after a latch, returns 1 (seed, test code)

**File:** `tests/bus/test_bus.c:288-296`, `tests/bus/test_bus.c:434-435`, `tests/bus/test_bus.c:496-509`

**Issue:** The case comment says "Once a window ends, the next request is taken". That path is checked only through
`halt_three_reads`, which discards the return (`test_bus.c:291`), so a return of 0 alongside a working window would
pass. `nmi.one_edge_at_a_time` checks the 1 for a first edge and the 0 for a blocked second edge. It never checks
that `raise_nmi` returns 1 again once `nmi_pending` clears, and that is exactly the path WR-05 shows is misleading.

**Fix:** Have `halt_three_reads` return the request's result through an out-parameter, or call `expect_taken` on
the request in `halt_one_window_at_a_time` before the timing check. Add an NMI sub-case that latches a first edge,
then raises another and asserts both the return and the resulting `nmi_phi2`. Under project rule 7, record this as
a seed.

---

_Reviewed: 2026-10-19_
_Reviewer: Claude (gsd-code-reviewer)_
_Depth: standard_
