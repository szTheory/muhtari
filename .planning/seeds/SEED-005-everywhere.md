---
id: SEED-005
status: dormant
planted: 2026-10-15
planted_during: preparation, before milestone v1.0
trigger_when: milestone v5.0 has shipped, or a port, the libretro buildbot or Playstead integration is wanted sooner
scope: large
audit_acknowledged:
  milestone: v1.0
  at: 2026-10-21
  status: dormant
---

# SEED-005: muhtari everywhere — consoles, handhelds and Playstead (v6.0)

## Why This Matters

Portability is a stated goal: the core is integer-only, allocation-free after load and free of globals precisely so
it can run on low-power devices. This milestone turns that into shipped builds and wider reach.

## When to Surface

**Trigger:** v5.0 shipped, or the owner promotes a port, the buildbot submission or Playstead integration.

## Scope Estimate

**Large** — a milestone: libretro buildbot submission with the CMake templates and a `.info` PR; console static
builds (PSP, 3DS, Vita, Wii, Switch) as compile lanes with pinned devkitPro and pspdev containers; big-endian as a
supported tier; frame-time and battery budgets measured on a Raspberry Pi and a handheld; Playstead's
`a7800` system ID (one-way there) with a RetroArch-plus-core pin; asking RetroAchievements to list muhtari for
hardcore; BizHawk's native-core route for TASVideos.

## Breadcrumbs

- `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` section 4: device budgets and feasibility.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` sections 1 and 2: buildbot, RetroAchievements.
- `.planning/preparation/ENGINEERING.md` section 9: portability lanes.

## Notes

The DS/DSi are out of reach at full accuracy (derived); PSP and Old 3DS are marginal. The browser build moved to
SEED-008 (owner priority, 2026-10-15).
