# Start here: muhtari's project memory

This is the maintained entry point for agents working on muhtari or learning from
it in a sibling emulator project. Read intent and current state first; follow the
relevant source links instead of scanning every historical plan.

| Need | Read |
|---|---|
| Shared goals, gamer priorities, writing voice, research posture and provenance | [INTENT.md](INTENT.md) |
| What ships today and binding constraints | [PROJECT.md](PROJECT.md), [../AGENTS.md](../AGENTS.md) |
| Public summary of what works, what is missing and what completion means | [../docs/status.md](../docs/status.md) |
| Current position and next allowed step | [STATE.md](STATE.md) |
| Current execution contract / requirements | [ROADMAP.md](ROADMAP.md); active `REQUIREMENTS.md` exists only while a milestone is selected. Otherwise, see the [v11.0 requirements archive](milestones/v11.0-REQUIREMENTS.md). |
| CI/CD iteration and release procedures | [AGENTS.md rule 9](../AGENTS.md), [owner intent](INTENT.md#retain-useful-understanding), [maintainer CI/release runbook](../docs/maintaining.md#inspecting-a-ci-run) and [testing guide](../docs/testing.md) |
| Immediate, next, mid and long-term horizons | [PRODUCT-ROADMAP.md](PRODUCT-ROADMAP.md) |
| Where hardware/API/engineering facts live | [preparation/README.md](preparation/README.md) |
| Settled choices and reasons | [preparation/DECISIONS.md](preparation/DECISIONS.md) |
| Reproduced lessons versus imported hypotheses | [LESSONS.md](LESSONS.md) |
| Current successor recommendation, evidence confidence and revisit triggers | [2026-11-19 successor review](research/next-milestone/2026-11-19-successor-review.md) and [rolling roadmap](PRODUCT-ROADMAP.md); the v11.0 audit records completed release/CI evidence |
| Playstead quick route, native-host north star and pasteable handoff | [research/PLAYSTEAD-HANDOFF.md](research/PLAYSTEAD-HANDOFF.md) |
| Reusable bounded specialist brief | [research/fan-out/prompt.txt](research/fan-out/prompt.txt) |
| Historical browser-player idea and alternatives; reconcile with implemented LibFuturo routes in the current synthesis | [Browser-player seed](seeds/SEED-261030-tt4-browser-libretro-player.md), [bounded research](research/browser-player/RECOMMENDATION.md) |
| Historical interface exploration; LibFuturo now implements the interface/consumer routes, with qualification/adoption limits in current research | [Interface seed](seeds/SEED-261030-zk3-emulator-interface.md), [initial read and future-session handoff](research/emulator-interface/RECOMMENDATION.md) |
| Every owner ask and its disposition | [preparation/PROMPT-COVERAGE.md](preparation/PROMPT-COVERAGE.md) |
| Shipped evidence and historical limits | [MILESTONES.md](MILESTONES.md), [milestones/](milestones/) |

## Reuse across emulator projects

Start with INTENT, the relevant lesson and its evidence, then inspect the owning
code or contract. Separate transferable goals and engineering mechanisms from
7800 hardware facts, host-specific assumptions and historical tooling workarounds.
An observation in either direction is a hypothesis in the receiving project until
locally reproduced. Record its source revision, limits, decision and recheck trigger.
Muhtari is the current leading example, not a permanently authoritative template.

Refresh this index, intent links and rolling horizons when a milestone begins or
closes. Keep useful findings in ordinary project artifacts; no external memory
service or bulk lesson export is needed. Detailed plans stay in their phase.

Latest completion: [v11.0 audit](milestones/v11.0-MILESTONE-AUDIT.md) records
the shipped Windows extended-path support and measured CI/CD disposition; its
[requirements](milestones/v11.0-REQUIREMENTS.md), [phase records](milestones/v11.0-phases/)
and [roadmap](milestones/v11.0-ROADMAP.md) retain the evidence. No next
milestone is selected; see [STATE](STATE.md) and the [rolling roadmap](PRODUCT-ROADMAP.md).
Atari 2600 emulation belongs to twentysixhundo; LibFuturo owns
the shared interface and native/browser consumer qualification. Keep those
responsibilities distinct from Muhtari's machine behavior and release artifacts.

## Historical milestone — v6.0 shipped

YM2151 homebrew sound is shipped as v0.14.0. The [current roadmap](ROADMAP.md),
[requirements](milestones/v6.0-REQUIREMENTS.md) and [state](STATE.md) are
authoritative over earlier successor-pending text. Phase 19 is the measured
tune-up; Phase 20 is the complete qualified chip/bus/audio/state and consumer
slice. The selected successor is v7.0; [owner authority](INTENT.md#ym2151-milestone-authority--2026-11-11)
preserves the original host stops, and [v7-SOUPER research](research/next-milestone/v7-SOUPER.md)
records the new profile's limits.
