---
created: 2026-10-19T19:32:58.000Z
title: Close the phase 2 re-review findings when MARIA and the run loop land
area: core
severity: minor
files:
  - tests/cpu/test_cpu.c
  - docs/hardware/sally.md
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
- have MARIA check the 1 or 0 that `muhtari__bus_request_halt` and `muhtari__bus_raise_nmi` now return.
