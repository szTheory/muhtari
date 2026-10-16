---
id: SEED-002
status: dormant
planted: 2026-10-15
planted_during: preparation, before milestone v1.0
trigger_when: milestone v2.0 has shipped, or a frontend (Playstead) needs save states or deterministic replay before then
scope: large
---

# SEED-002: Save states, rewind, run-ahead and input movies (v3.0)

## Why This Matters

Save states, rewind and run-ahead are table stakes in RetroArch, and deterministic input movies are what TAS
authors, machine-assisted speedruns and Playstead's continuation checks all need. muhtari's exact frame rate and
byte-identical output on every platform make its movies portable in a way no current 7800 emulator offers.

## When to Surface

**Trigger:** v2.0 shipped, or Playstead asks for its continuation protocol first.

## Scope Estimate

**Large** — a milestone: a fixed-size, versioned state written by one visitor (validate before apply; ticks
capped; byte-wise little-endian), forks at every frame of the fixture set identical on six platforms; RetroArch
`savestate` levels raised one at a time under configure-time guards; rewind and preemptive frames; the own text
movie format with `--record`, lag-frame counts and power-on/state/SRAM starts; Playstead's continuation protocol
(qualify, initial-run, continue; exits 0/77/1) and adapter pin; property tests (save → load → continue equals an
uninterrupted run). A sub-frame input queue only if a game or TAS is shown to need it.

## Breadcrumbs

- `.planning/preparation/HOSTS-AND-INTEGRATION.md` sections 1 (serialize, `.info` claims), 4 (runner options,
  Playstead mapping) and 5 (movies, lag frames, sub-frame input).
- `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` section 3: TAS and speedrun requirements, BK2 contents.
- `.planning/preparation/ARCHITECTURE.md`: state visitor and versioning.
- `.planning/preparation/ENGINEERING.md` section 3: the `fuzz.state` and `fuzz.movie` targets.

## Notes

Precedent from the NES sibling: its state work took one milestone of five phases and found a tick-overflow loop
through a CRC-valid state, hence the tick cap.
