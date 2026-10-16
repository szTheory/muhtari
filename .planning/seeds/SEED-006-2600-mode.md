---
id: SEED-006
status: dormant
planted: 2026-10-15
planted_during: preparation, before milestone v1.0
trigger_when: milestone v2.0 (the whole 7800 library) has closed and the owner promotes 2600 mode
scope: large
---

# SEED-006: Atari 2600 cartridges in 7800 mode

## Why This Matters

The real console plays nearly every 2600 cartridge. Owner decision (2026-10-15): support it after the 7800 library
is complete, keeping the seams clean meanwhile. Until then, `muhtari-run --probe` and the loader recognise 2600
images and refuse them with a distinct "unsupported: 2600 (planned)" status, and Stella remains the reference.

## When to Surface

**Trigger:** v2.0 closed and the owner promotes it at a milestone boundary.

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
