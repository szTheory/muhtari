---
id: SEED-004
status: dormant
planted: 2026-10-15
planted_during: preparation, before milestone v1.0
trigger_when: milestone v4.0 has shipped, or homebrew developers, TAS authors or AI users ask for tools sooner
scope: large
---

# SEED-004: Tools for developers, ROM hackers, TAS authors and AI agents (v5.0)

## Why This Matters

7800 homebrew developers lack symbols, bank-aware watchpoints and a view of DMA use per line; no 7800 environment
exists for reinforcement learning; TASVideos accepts only BizHawk. These tools turn the core's observability into
the features that set it apart.

## When to Surface

**Trigger:** v4.0 shipped, or one of those audiences asks first.

## Scope Estimate

**Large** — a milestone: the hook API (instruction, bus, line and frame observers) through the dual-compiled
traced loop with a proof that hooks-off cost is unchanged; a bounded `run` with stop reasons; bank-aware
breakpoints and watchpoints; traces with tick and beam position; display-list dumps and a per-line DMA overlay;
symbols from dasm, cc65 and 7800basic; a homebrew lint (DMA overrun, POKEY never left init, two-button hazard);
`muhtari-run --patch` (IPS/BPS, fuzzed); Python bindings with a vectorised Gymnasium-style environment (target ≥
5,000 frames/s per thread, measured before claimed); BK2 import; Lua in the runner on demand.

## Breadcrumbs

- `.planning/preparation/HOSTS-AND-INTEGRATION.md` section 5: inspection, hooks, Python vs Lua, the Gym env,
  movies, debugger surfaces.
- `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` sections 2 and 3: developer pain points, AI and TAS needs.
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` section 6: patching and identity.
- `.planning/preparation/ARCHITECTURE.md`: observability design.

## Notes

The inspection API (side-effect-free peek, CPU/MARIA getters, per-line statistics) lands earlier, in v1.0–v3.0.
