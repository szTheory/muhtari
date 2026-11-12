---
id: SEED-006
status: retired
retired: 2026-11-11
retirement_reason: Atari 2600 emulation belongs to the sibling project twentysixhundo
planted: 2026-10-15
planted_during: preparation, before milestone v1.0
trigger_when: none; retired by owner direction
scope: large
audit_acknowledged:
  milestone: v1.0
  at: 2026-10-21
  status: dormant
---

# SEED-006: Atari 2600 cartridges in 7800 mode

## Disposition — retired

Owner correction, 2026-11-11: the separate `twentysixhundo` project owns Atari
2600 emulation. Muhtari remains focused on Atari 7800. This supersedes the
original proposal below; it is retained only as historical context. Existing
2600 image recognition/refusal remains supported.

## Why This Matters

The real console plays nearly every 2600 cartridge. Owner decision (2026-10-15): support it after the 7800 library
is complete, keeping the seams clean meanwhile. Until then, `muhtari-run --probe` and the loader recognise 2600
images and refuse them with a distinct "unsupported: 2600 (planned)" status, and Stella remains the reference.

## When to Surface

No active trigger. Retired; exclude this seed from milestone candidates.

## Scope Estimate

**Large** — close to a second machine: TIA video racing the beam, the 2600 bank-switching schemes, the INPTCTRL
2600 lock path and the BIOS's 2600 fallback. MAME's TIA video files are BSD-3 (cross-check only); the MiSTer
`TIA.sv` is facts-only; Stella is GPL and never opened.

## Breadcrumbs

- `.planning/preparation/A7800-SYSTEM.md`: INPTCTRL, the BIOS's 2600 path.
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` section 1: 2600 image detection.
- `PROVENANCE.md` sections 2 and 3: which TIA sources are usable.

## Notes

The architecture keeps the seams: TIA is its own module, INPTCTRL is modelled in full, and cart detection already
classifies 2600 images.
