# Shared project intent and provenance

**Source:** owner direction in the 2026-10-29 milestone/onboarding conversation,
combined with the standing contracts linked below. These are product preferences
and accepted instructions; technical research and observed behavior have separate
evidence. Updated 2026-10-29.

## Gamer value comes first

Make a core people can actually use to enjoy games: reliable launch, good sound
and picture, responsive controls, safe progress, easy save/load and clear recovery.
Ship downloadable releases early and often. The immediate product proof is playing
7800 games in Playstead, the owner's emulator frontend. Save/load states are high
priority; AI play, speedrunning and TAS matter later and must not delay ordinary play.

Muhtari supplies a C library, headless runner and libretro core. The frontend owns
windows, audio/input devices, menus and progress UI. The quickest Playstead route
uses an external RetroArch process with muhtari. The longer-term goal is a calm,
integrated Playstead play/Continue experience; a Playstead-owned native libretro host
is a candidate route, requiring its own product and technical validation.

Provenance: owner steering after successful local Choplifter/Ninja Golf sanity
checks; [FEATURES research](research/FEATURES.md) records the inspected Playstead
revision, relevant dirty files, brand/product sources and unresolved integration.

## Reduce setup toil and meet the reader where they are

Keep the README short, clear and gamer-facing: value, honest current status,
download/install, load a game, controls and a useful next link. Include GUI and
command-line happy paths. Explain optional BIOS and distinguish battery saves
from save states. Put build, testing and maintainer detail in docs. Update affected
docs with changes; test executable instructions through existing mechanisms.

Automate setup and verification wherever practical. Keep machine-specific launchers,
ROMs, BIOS, saves, screenshots and personal results outside Git. The owner verifies
nothing by default; voluntary gameplay feedback is welcome, scoped evidence and
does not become recurring manual UAT. [Onboarding observations](LESSONS.md#local-player-onboarding-2026-10-29)
record what was actually observed, without universal compatibility claims.

## Retain useful understanding

Maintain [rolling horizons](PRODUCT-ROADMAP.md) at every milestone start and close:
Immediate is the accepted scope; Next is the preferred follow-on with alternatives;
Mid/Long stay directional and carry rationale and promotion/revisit triggers. Only
the active roadmap is an execution contract. Do upstream scope/dependency work
without taking over later discussion, detailed planning, implementation or shipping.

Persist high-signal research, rejected alternatives, confidence, provenance and
conditions that would change a recommendation. Use the [fan-out brief](research/fan-out/prompt.txt)
to investigate independent decisions through relevant roles, prefer primary sources,
perform one adversarial synthesis and stop when more work would not change a choice.
Follow approved recommendations without routine reconfirmation inside the active
GSD step; preserve [the one-step limit](../AGENTS.md).

## Shared emulator family: learning flows both ways

The owner intends to build a core for each major console they care about, sharing
product goals and engineering DNA. Muhtari is currently the most mature example
and can provide useful starting points for NES, Game Boy, Neo Geo and other siblings.
It can learn from them as they find better approaches. Transfer intent, seams,
verification strategies and demonstrated mechanisms; validate hardware behavior,
dependencies and tool workarounds in each receiving project. Maturity is not proof
that every implementation choice should be copied.

Keep lessons local and discoverable by default. A sibling agent can start at this
directory's [index](README.md), inspect the source evidence and adapt the posture.
Inbound lessons remain hypotheses until reproduced; exports are produced only
when explicitly requested. [LESSONS protocol](LESSONS.md#protocol) is authoritative.

## Small, understandable dependencies

Another small understood permissive copy can be better than another dependency.
Remove the need, reuse existing code, use standard facilities and the platform,
then add the minimum. A justified dependency is allowed; compare actual correctness,
security, transitive scope, provenance, update burden and lifetime cost. Copied code
also needs notices and maintenance. No GPL/LGPL/unlicensed emulator source is read.

Provenance: owner reiterated this preference in the current conversation; standing
[AGENTS style](../AGENTS.md), [PROVENANCE](../PROVENANCE.md) and
[ENGINEERING §11](preparation/ENGINEERING.md#11-docs-standards) retain exact policy.

## Evidence and revalidation

| Kind | How to use it |
|---|---|
| Owner intent | Dated here and mapped in PROMPT-COVERAGE; do not reinterpret a preference as a measured fact. |
| Settled decision | Read DECISIONS and its source; reopen with new evidence. |
| Research | Retain exact source/revision, confidence and unknowns; source-backed does not mean locally tested. |
| Reproduced lesson | Keep the mechanism and command/exit/run evidence in LESSONS. |
| Voluntary play report | Preserve the title/setup and what the player actually said; do not infer duration, full completion or calibration. |
| Sibling takeaway | Treat as a receiving-project hypothesis until its mechanism is reproduced there. |

Recheck moving host APIs/pins and dirty sibling sources before implementation.
Refresh scope at milestone boundaries and when a concrete player failure or new
evidence changes priority. Avoid repeating settled research simply because a new
session starts.
