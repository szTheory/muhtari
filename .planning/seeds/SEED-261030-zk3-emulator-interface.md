---
id: SEED-261030-zk3
status: dormant
planted: 2026-10-30
planted_during: v3.0 phase 12 debug continuation
trigger_when: owner initiates an emulator-interface project or a measured cross-core host limitation warrants a deeper comparison
scope: large
---

# An optional emulator interface alongside libretro

## Why This Matters

Owner direction, 2026-10-30: explore a possible separate interface project that
could improve host integration, observability, AI/automated play, configuration
and performance while keeping libretro support in the emulator family. Bank an
initial read so another session can investigate from this repository later;
do not fully explore, specify or start implementing the standard now.

The useful hypothesis is that explicit lifecycle, capability and automation
contracts could reduce repeated work across cores and hosts. Superiority,
performance gains and demand are unproved. Separate API limitations from host
UX, missing core capabilities and game-specific automation knowledge.

## When to Surface

**Trigger:** the owner starts that separate investigation, or two actual consumers
encounter a demonstrated limitation that cannot be solved more simply with the
existing libretro API, a host tool or a narrow optional profile.

## Scope Estimate

**Large** for a credible public interface and ecosystem; a comparison or adapter
experiment is much smaller. Conformance, adoption, compatibility and maintenance
are part of the cost, not consequences of writing a clean header.

## Breadcrumbs

- [Initial research and handoff](../research/emulator-interface/RECOMMENDATION.md):
  evidence, options, concrete hypotheses and a prompt for the deeper session.
- [Browser-player idea](SEED-261030-tt4-browser-libretro-player.md): a possible
  consumer, not a prerequisite or justification for inventing a new ABI.
- [Tools/AI seed](SEED-004-tools-observability-ai.md): muhtari-specific tool work
  remains distinct from a shared standard.
- `include/muhtari.h`, `libretro/libretro.h` and
  [host research](../preparation/HOSTS-AND-INTEGRATION.md): current interfaces and
  earlier proposed tooling. Read the public API against its implementation.
- [Shared intent](../INTENT.md) and
  [fan-out baseline](../research/fan-out/prompt.txt): permissive provenance, small
  dependencies, observable outcomes and bounded research.

## Notes

Keep this outside active muhtari/Playstead scope. Future research must compare
doing nothing, existing interface plus host improvements, optional extensions
with adapters, and a separate standard. Preserve libretro compatibility. No
performance, portability, save-state compatibility or zero-overhead claim is
established by this capture, and no licensing policy is changed.
