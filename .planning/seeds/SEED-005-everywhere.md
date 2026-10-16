---
id: SEED-005
status: dormant
planted: 2026-10-15
planted_during: preparation, before milestone v1.0
trigger_when: milestone v5.0 has shipped, or a port, the libretro buildbot or Playstead integration is wanted sooner
scope: large
---

# SEED-005: muhtari everywhere — consoles, handhelds, the web and Playstead (v6.0)

## Why This Matters

Portability is a stated goal: the core is integer-only, allocation-free after load and free of globals precisely so
it can run on low-power devices. This milestone turns that into shipped builds and wider reach.

## When to Surface

**Trigger:** v5.0 shipped, or the owner promotes a port, the buildbot submission or Playstead integration.

## Scope Estimate

**Large** — a milestone: libretro buildbot submission with the CMake templates and a `.info` PR; console static
builds (PSP, 3DS, Vita, Wii, Switch) as compile lanes with pinned devkitPro and pspdev containers; big-endian as a
supported tier; a Raspberry Pi budget measured on a device; a WASM web demo and a WASI hash lane; Playstead's
`a7800` system ID (one-way there) with a RetroArch-plus-core pin; asking RetroAchievements to list muhtari for
hardcore; BizHawk's native-core route for TASVideos.

## Breadcrumbs

- `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` section 4: device budgets and feasibility.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` sections 1, 2 and 6: buildbot, RetroAchievements, WASM.
- `.planning/preparation/ENGINEERING.md` section 9: portability lanes.

## Notes

The DS/DSi are out of reach at full accuracy (derived); PSP and Old 3DS are marginal.
