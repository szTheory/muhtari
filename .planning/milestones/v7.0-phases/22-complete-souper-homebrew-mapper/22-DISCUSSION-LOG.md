# Phase 22: Complete Souper homebrew mapper - Discussion Log

> **Audit trail only.** Do not use as input to planning, research, or execution agents.
> Decisions are captured in CONTEXT.md — this log preserves the alternatives considered.

**Date:** 2026-11-13
**Phase:** 22-complete-souper-homebrew-mapper
**Areas discussed:** profile boundary, deterministic reset/EXRAM policy, snapshot compatibility, current consumer contracts

---

The user asked for comprehensive, role-aware fan-out with one synthesized recommendation per relevant point. The active roadmap, requirements, saved v7.0 recommendation, DEC.26/DEC.36/DEC.38, and Phase 20 D-07/D-08 already settle the implementation-facing choices. The owner asked to follow the recommendation for todo matches; no todo was folded into Phase 22.

## Profile boundary

| Option | Description | Selected |
|--------|-------------|----------|
| Exact mapper-only profile | One 512 KiB ROM / 32 KiB EXRAM software model; no BupChip payload or hardware accuracy claim | ✓ |
| Broader game/audio support | Add retail-image, BupChip, flash, or other unqualified variants | |

**Recommendation adopted:** Keep the mapper-only profile already selected in the roadmap, requirements, and saved v7.0 recommendation.
**Notes:** Primary references support the software-model profile; no source establishes physical cartridge accuracy.

## Reset and snapshot compatibility

| Option | Description | Selected |
|--------|-------------|----------|
| Apply existing policy | Zeroed mapper latches, zero-filled EXRAM, and additive Souper-specific state envelope while preserving existing topologies | ✓ |
| Reset global compatibility | Replace prior state envelopes or change existing non-Souper snapshot bytes | |

**Recommendation adopted:** Carry forward the existing DEC.26/DEC.36 and Phase 20 additive-state policy without reopening settled compatibility choices.
**Notes:** Souper uses the next state/behavior pair; exact-source state evidence and tests remain in Phase 22.

## the agent's Discretion

- Choose generated diagnostic patterns and boundary cases that distinguish each supported mapping state.
- Use the pinned and public software references as mapping evidence without implying physical validation.
- Keep libretro memory-map descriptors within the existing stable-window contract.

## Deferred Ideas

- BupChip/BupBoop audio and extra payloads.
- Rikki & Vikki compatibility and physical Souper calibration.
- SUBPAR/SUBLET flash behavior and other Souper variants.
- The five reviewed historical todos, which remain in their existing locations.
