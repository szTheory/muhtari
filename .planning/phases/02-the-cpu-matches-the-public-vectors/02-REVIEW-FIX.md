---
phase: 02-the-cpu-matches-the-public-vectors
fixed_at: 2026-10-19T20:14:36Z
review_path: .planning/phases/02-the-cpu-matches-the-public-vectors/02-REVIEW.md
iteration: 2
findings_in_scope: 2
fixed: 1
skipped: 1
status: partial
---

# Phase 2: Code Review Fix Report

**Fixed at:** 2026-10-19T20:14:36Z
**Source review:** .planning/phases/02-the-cpu-matches-the-public-vectors/02-REVIEW.md
**Iteration:** 2 (the second and last fix round under rule 7)

**Summary:**
- Findings in scope: 2 (WR-01, WR-02; IN-01 is info and out of scope)
- Fixed: 1
- Skipped: 1

## Fixed Issues

### WR-01: A HALT window or NMI edge that is ignored is dropped without a word, and the caller cannot tell

**Files modified:** `src/bus.c`, `src/internal.h`, `docs/hardware/bus.md`, `tests/bus/test_bus.c`
**Commit:** 62b6bdc
**Applied fix:** `muhtari__bus_request_halt` and `muhtari__bus_raise_nmi` now return `int`: 1 when they take
the request, 0 when they ignore it. They ignore it when the HALT gate is shut, when MEN = 0, or when a window or
edge is already in flight. The prototypes' comment in `src/internal.h` and the HALT and NMI sections of
`docs/hardware/bus.md` say so. The bus tests check the return value as well as the timing:
- `bus.halt.one_window_at_a_time`: the first request returns 1, and a second request returns 0 whether it comes
  after the fall or before it.
- `bus.nmi.one_edge_at_a_time`: the first edge returns 1 and the second returns 0.
- `bus.halt.gate_two_writes`: a request with the gate shut returns 0.
- `bus.nmi.latched_in_cycle`: an edge with MEN clear returns 0.

A small `expect_taken` helper in `tests/bus/test_bus.c` wraps `expect_count`. The other callers (the RIOT test,
the NMI timing tests, the `halt_three_reads` helper) still ignore the return value. That is valid C, and no
attribute forces a check. No MARIA exists yet to use the result.

## Skipped Issues

### WR-02: `cpu.jam.step_reads_ffff` cannot catch a jammed step that polls for an NMI

**File:** `tests/cpu/test_cpu.c:410`, `tests/cpu/test_cpu.c:430`
**Reason:** This finding is about test code, so rule 7 makes it a seed, not a requirement. The review says so
too. I confirmed the analysis: `poll_phi2` stays 0 after the memset and the JAM, so `nmi_due` (`src/cpu.c:596`)
is false whatever the jammed guard does. The suggested one-line change (set `poll_phi2` to `phi2` before
snapshotting `before`) is right, and it is left to the seed or todo the orchestrator records.
**Original issue:** The test latches an NMI with `nmi_phi2 = phi2`, but `poll_phi2` is still 0, so `nmi_due` is
false. A regression in which the jammed guard falls through to `if (nmi_due(m)) nmi(m);` would still pass the
"the latched NMI, not taken" assertion.

## Verification

All gates ran in the main checkout: `workflow.use_worktrees` is `false`, so no worktree was created. The tree was
the WR-01 change before commit 62b6bdc.

- `cmake --workflow --preset ci`: exit 0, 408/408 tests passed, plus 6/6 in the release step.
- `cmake --workflow --preset hygiene`: exit 0, 24/24 tests passed.

An earlier ci run failed `embed.subdirectory` because the linker crashed with `Bus error: 10`. The machine
restarted during that run. The re-run above passed with no change to the code.

---

_Fixed: 2026-10-19T20:14:36Z_
_Fixer: Claude (gsd-code-fixer)_
_Iteration: 2_
