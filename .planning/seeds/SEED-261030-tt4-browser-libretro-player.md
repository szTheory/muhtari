---
id: SEED-261030-tt4
status: dormant
planted: 2026-10-30
planted_during: v3.0 phase 12 debug continuation
trigger_when: owner starts a separate browser-player project, or browser play and Playstead embedding are considered at a milestone boundary
scope: large
audit_acknowledged:
  milestone: v3.0
  at: 2026-11-10
  status: dormant
---

# A reusable browser emulator player for Playstead and other sites

## Why This Matters

Owner direction, 2026-10-30: explore whether a browser equivalent of RetroArch
already exists; bank a possible separate open-source player that meets the
projects' quality bar and can also serve Playstead. Start with a bounded
primary-source fan-out and high-level recommendations, without a full spec or
implementation. Prefer small, flat dependencies and understood permissive reuse.

The opportunity is a reusable host for muhtari and later cores: local game
selection, responsive sound/picture/input, safe progress and embedding that does
not require every consumer to build emulator infrastructure again.

## When to Surface

**Trigger:** the owner initiates the separate project, or the next milestone
weighs browser play and Playstead embedding. Capture is not promotion into the
current milestone. Existing debug safety stops and live budgets remain intact.

## Scope Estimate

**Large** for a reusable multi-core browser host; a single-core playback spike
would be smaller. The effort estimate is directional until existing products,
distribution rights and real browser behavior are assessed. No schedule promised.

## Breadcrumbs

- [Existing browser build seed](SEED-008-browser.md): owns muhtari's potential
  browser build. This seed adds the distinct reusable player project idea.
- [Shared intent](../INTENT.md) and
  [Playstead handoff](../research/PLAYSTEAD-HANDOFF.md): core/host ownership and
  existing native quick route remain relevant.
- [Research synthesis](../research/browser-player/RECOMMENDATION.md): bounded
  landscape, sibling fit, tradeoffs, open questions and adversarial pass.
- [Fan-out baseline](../research/fan-out/prompt.txt): relevant roles, primary
  evidence, dependency ladder and diminishing-returns limit.

## Notes

This is an owner idea with research enrichment, not an accepted technical design.
The current WASI test lane does not itself supply a browser player. Keep the
browser player separate from muhtari's three existing deliverables and from
Playstead's current implementation until the owner selects a project scope.
