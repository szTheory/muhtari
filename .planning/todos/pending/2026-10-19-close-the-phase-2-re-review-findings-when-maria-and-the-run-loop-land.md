---
created: 2026-10-19T19:32:58.000Z
title: Close the phase 2 re-review findings when MARIA and the run loop land
area: core
severity: minor
files:
  - tests/cpu/test_cpu.c
  - docs/hardware/sally.md
  - src/bus.c
  - src/internal.h
  - docs/hardware/bus.md
  - tests/bus/test_bus.c
---

## Problem

The re-review of phase 2's fix round (02-REVIEW.md as committed in 2cca8b3, 2026-10-19) found no blocker and three
findings. Fix round 2 (62b6bdc) fixed WR-01: the bus entry points now return 1 when they take a request and 0 when
they ignore it. Two findings are left. The ids are the re-review's own: the first review's WR-01 and IN-01 are
different findings, kept in the 2026-10-19 todo on the nightly filter and the ROM tables.

- WR-02: `cpu.jam.step_reads_ffff` leaves `poll_phi2` at 0, so its "the latched NMI, not taken" check still passes if
  a jammed step falls through to the NMI poll. This is test code, so AGENTS rule 7 makes it a seed; fix round 2
  skipped it for that reason.
- IN-01: the JAM section of docs/hardware/sally.md says "a frame still ends", but `src/frame.c` does not run the CPU
  yet, so no test shows that a frame ends on a jammed ROM.

## Solution

Fold these into phase 3's run-loop plan:

- in `cpu.jam.step_reads_ffff`, set `poll_phi2` to `phi2` along with `nmi_phi2` before the jammed steps, so the NMI is
  due by `nmi_due`'s rule and only "no NMI while jammed" passes;
- once the run loop runs the CPU, add a test that a frame ends on a jammed ROM, and keep the JAM sentence only as far
  as that test shows;
- have MARIA check the 1 or 0 that `muhtari__bus_request_halt` and `muhtari__bus_raise_nmi` now return, once the
  third review's findings below say what that 1 promises.

## The third review's findings

The review after fix round 2 (02-REVIEW.md as committed in 4f8de62, 2026-10-19) found one warning and two info items.
Both fix rounds AGENTS rule 7 allows are spent, so they wait here for the phase 3 plan that builds MARIA's NMI and HALT
calls.

- WR-05: `muhtari__bus_raise_nmi` returns 1 for an edge that `latch_nmi` later drops, either because an earlier
  latched edge has not been taken or because MEN was cleared before the latch. A probe printed
  `first=1 phi2=2 second=1 phi2_after=2 pending=0`. Narrow the contract in `src/internal.h` and
  docs/hardware/bus.md to "queued for latching", or count dropped edges in `latch_nmi`, depending on what MARIA
  needs. Then pin it with a `nmi.one_edge_at_a_time` sub-case that raises a second edge after the first latches.
- IN-04: docs/hardware/bus.md's HALT paragraph names only "asked before the last window ended" as the reason for a 0.
  A request with the gate shut also returns 0. Name both reasons, as the NMI paragraph does.
- IN-05 (test code, a seed): no test asserts the 1 for a request after a window ends (`halt_three_reads` discards
  it), or for an edge raised after the first latches.
