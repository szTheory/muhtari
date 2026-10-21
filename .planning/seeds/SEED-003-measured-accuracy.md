---
id: SEED-003
status: dormant
planted: 2026-10-15
planted_during: preparation, before milestone v1.0
trigger_when: milestone v3.0 has shipped, or a measured accuracy gap blocks a known game
scope: large
audit_acknowledged:
  milestone: v1.0
  at: 2026-10-21
  status: dormant
---

# SEED-003: Measured accuracy and the XM expansion (v4.0)

## Why This Matters

v1.0–v3.0 use the best-supported timing model with named, cited constants; several (the HALT hand-shake, hole
costs for indirect objects, the light-gun latency) are fitted, not measured. This milestone replaces fits with
measurements and adds the hardware homebrew increasingly targets.

## When to Surface

**Trigger:** v3.0 shipped, or a game shows a gap the fitted model cannot close.

## Scope Estimate

**Large** — a milestone: a Verilator build of the MiSTer core's licensor-only files (MARIA, SALLY, POKEY; never its
GPL framework or firmware) as a nightly timing referee; the own diagnostic ROMs run on the owner's console through a
flash cart, results transcribed and checked by on-screen CRC; refits of `halt_window()` and DMA constants with a
behaviour-revision bump each; our POKEY compared cycle by cycle with the RTL on recorded register traces; the XM
expansion (YM2151 from a C port of ymfm, XM RAM and POKEY); the host-side composite artifact filter; nightly
p99.999 frame-time trends against the budgets.

## Breadcrumbs

- `.planning/preparation/CONFORMANCE.md` sections 3 and 4: oracles, diagnostic ROMs, the hardware-run protocol.
- `.planning/preparation/A7800-MARIA.md` sections 4, 8 and 11: fitted constants, artifacting, open questions.
- `.planning/preparation/A7800-AUDIO.md` sections 5 and 6: POKEY verification plan, YM2151.
- `.planning/preparation/ENGINEERING.md` section 8: tail-latency measurement.

## Notes

The MiSTer pin used for hole behaviour must predate its holey-DMA regression (see A7800-MARIA).
