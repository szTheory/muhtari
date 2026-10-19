---
created: 2026-10-19T19:32:58.000Z
title: Close the phase 2 re-review findings when MARIA and the run loop land
area: core
severity: minor
files:
  - src/bus.c
  - src/internal.h
  - tests/bus/test_bus.c
  - tests/cpu/test_cpu.c
  - docs/hardware/bus.md
  - docs/hardware/sally.md
---

## Problem

The re-review of phase 2's fix round (02-REVIEW.md as committed in 2cca8b3, 2026-10-19) found no blocker and three
findings. Each one is about a caller or a run loop that phase 3 builds, so they wait for phase 3 instead of a second
phase 2 fix round. The ids are the re-review's own: the first review's WR-01 and IN-01 are different findings, kept in
the 2026-10-19 todo on the nightly filter and the ROM tables.

- WR-01: `muhtari__bus_request_halt` and `muhtari__bus_raise_nmi` return `void`. A request made while a window or an
  edge is in flight is ignored, as docs/hardware/bus.md says, but the caller cannot tell. A MARIA that schedules ahead
  would lose DMA windows or DLIs, and no test would fail.
- WR-02: `cpu.jam.step_reads_ffff` leaves `poll_phi2` at 0, so its "the latched NMI, not taken" check still passes if
  a jammed step falls through to the NMI poll. This is test code, so AGENTS rule 7 makes it a seed.
- IN-01: the JAM section of docs/hardware/sally.md says "a frame still ends", but `src/frame.c` does not run the CPU
  yet, so no test shows that a frame ends on a jammed ROM.

## Solution

Fold these into phase 3's MARIA and run-loop plans:

- have both entry points return 1 when the request is taken and 0 when it is ignored, have MARIA check the result,
  and have `bus.halt.one_window_at_a_time` and `bus.nmi.one_edge_at_a_time` check the 0;
- in `cpu.jam.step_reads_ffff`, set `poll_phi2` to `phi2` along with `nmi_phi2` before the jammed steps, so the NMI is
  due by `nmi_due`'s rule and only "no NMI while jammed" passes;
- once the run loop runs the CPU, add a test that a frame ends on a jammed ROM, and keep the JAM sentence only as far
  as that test shows.
