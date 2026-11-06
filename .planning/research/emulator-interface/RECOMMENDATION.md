---
title: Optional emulator interface — initial recommendation and handoff
summary: A possible interface project alongside libretro, with evidence-seeking opportunities, adoption risks and a future-session prompt.
read_when: Before deeper exploration of a libretro companion or competing emulator-interface standard.
updated: 2026-10-30
---

# Bank the interface opportunity; test the missing contract first

Owner direction, 2026-10-30: capture a tangential project idea for a cleaner,
potentially more capable emulator interface while retaining libretro support.
Consider observability, AI/automated runs, configuration, performance, existing
approaches and community experience. This is an initial read for a later session,
not a standard proposal or an implementation plan. The
[seed](../../seeds/SEED-261030-zk3-emulator-interface.md) remains dormant.

## Initial read

There are plausible opportunities, especially **truthful capability discovery,
safe inspection and deterministic automation contracts**. The evidence does not
yet justify a wholly new standard, establish unmet community-wide demand or show
a performance advantage over libretro.

Libretro already defines frontend callbacks, timing, memory access, configuration
options and optional serialization. Its calls have no explicit instance handle;
the [official core guide](https://docs.libretro.com/development/cores/developing-cores/)
describes a single-instance contract. Separate module/worker/process instances
can still provide isolation; multiple sessions alone do not prove a new ABI is
needed. Muhtari's existing C API already has instance handles, structured input,
bounded one-frame execution, caller buffers and revision metadata. The
[initial research](initial-read.md) inventories actual public operations and
separates them from missing hooks/state APIs and old research proposals.

Existing alternatives matter: MAME and BizHawk provide substantial host-specific
debug/script tools; Gymnasium supplies the agent/environment convention, and
Stable-Retro adds game-specific variables/rewards/termination profiles. A shared
emulator API cannot infer what arbitrary game memory means. RetroArch's “AI
Service” chiefly concerns screenshot processing/translation and is a different
contract from deterministic training. Sources and one first-person performance
report are in the initial read; that report is anecdotal, not a prevalence study.

| Opportunity | Promising direction to investigate | Footgun / competing remedy |
|---|---|---|
| Lifecycle and portability | Explicit instance, buffer, callback and thread ownership; native and WASM adapters with clear unsupported results. | An elegant header does not establish thread safety or erase browser ABI/build requirements. Existing per-instance C calls may suffice. |
| Observability | Optional read-only machine/region descriptions and snapshots; opt-in trace/step capabilities with bounded output. | Raw RAM pointers can be mutable; reads can have hardware side effects; trace volume/hot-path overhead need controls and measurements. The core must implement the capability truthfully. |
| Automated play | Deterministic input/step/reset provenance, headless operation and reproducible observations; thin Gymnasium adapters. | Batching is only a speed hypothesis. Reward, goals and episode ends belong to game/environment profiles. Do not put models, cloud credentials or a networking stack into the core ABI. |
| Configuration and progress | Discover supported/effective settings, compatibility and recoverable errors; explicit save/state ownership. | Device routing, UI defaults and cloud conflicts are host responsibilities. Machine-readable metadata is not universal autoconfiguration or portable save states. |

## Recommendation and adversarial pass

Keep libretro and muhtari's direct C API. If later authorized, investigate one
real automation/inspection workflow through the current calls and the smallest
adapter. Extend the existing API only for a demonstrated missing operation;
consider an optional capability profile after that. A second independent core
and two distinct host/tool consumers are useful evidence before calling the
result a standard; this is a research heuristic, not a new repository gate.

The strongest objection is adoption cost: a new interface can become another
incomplete abstraction, duplicate configuration/state concepts, accumulate
console-specific exceptions and force every core/host to maintain more code.
Adapters cannot synthesize missing core debugging or state semantics. New ABI
design also does not change the licensing of existing cores. Independent
conformance cases and versioning/compatibility ownership matter more than the
choice of a modern language or schema. Keep dependencies small, but compare
the safety/correctness cost of copied code as well as packages.

Before any superiority claim, compare equal emulated work, observation fidelity,
copying/presentation and disabled-tooling cost. A wrapper plus a good host may
solve the problem without a competing standard. A new standard becomes more
credible only if concrete workflows cannot be expressed cleanly today, an
optional adapter/profile repeatedly fails to suffice, and independent consumers
are willing to implement and maintain the same contract. Start with software
7800 execution; GPU, netplay, cloud automation and complex machines would need
separate evidence rather than being promised by this initial read.

## Entry point for a deeper session

Use this prompt from the muhtari repository:

> Explore the dormant emulator-interface seed and linked initial read at greater
> depth, without implementing a standard. Read current muhtari and Playstead
> contracts and recheck source revisions. Compare libretro, existing direct C
> APIs and tools, optional capability profiles/adapters, and a new standard.
> Identify concrete consumer workflows and distinguish API gaps from host,
> core and per-game responsibilities. Use bounded primary-source specialist
> fan-out, community reports as qualified anecdotes, and one adversarial
> synthesis. Preserve permissive provenance, libretro support, small dependencies
> and existing safety stops. Recommend the smallest discriminating experiment,
> its evidence and what would falsify the need for a new interface. Stop before
> project creation, a full spec or implementation unless separately authorized.

The [browser-player idea](../browser-player/RECOMMENDATION.md) is a possible
consumer and an independent decision. Neither idea requires the other to start.
