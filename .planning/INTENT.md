# Shared project intent and provenance

**Source:** owner direction in the 2026-10-29 milestone/onboarding conversation,
combined with the standing contracts linked below. These are product preferences
and accepted instructions; technical research and observed behavior have separate
evidence. Updated 2026-11-10.

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

Owner direction, 2026-10-30: bank a possible separate open-source browser emulator
player, compatible with libretro and embeddable by Playstead and other sites.
Research existing products before choosing to build; retain the same quality and
small-dependency posture. The [seed](seeds/SEED-261030-tt4-browser-libretro-player.md)
is a high-level idea, not an active milestone or a change to muhtari's deliverables.

The same conversation adds a second possible project: an optional emulator
interface alongside libretro, investigating better observability, automated play,
configuration and host integration. Preserve libretro support; compare existing
solutions before inventing a standard. The [interface seed](seeds/SEED-261030-zk3-emulator-interface.md)
keeps performance/superiority claims as hypotheses and gives a later session an
entry point for deeper research.

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
GSD scope. An explicitly requested autonomous milestone continues through dependent
steps under [AGENTS rule 8](../AGENTS.md); other work keeps its normal step boundary.

Owner reaffirmation, 2026-11-10: apply this brief at milestone scoping and phase
discussion, with relevant specialist fan-out and an adversarial synthesis. Adopt
ordinary recommendations during an authorized autonomous run; ask only at a real
scope, authority, risk, cost or irreversible-action boundary. Start useful automated
verification early, including integration, end-to-end, smoke and seam checks. A
throwaway local check is valid; retain a check in CI only for recurring regression
value. Inspect each CI/release run for measured iteration-time improvements while
preserving its required coverage. This is standing guidance, not another UAT gate.

Owner reaffirmation, 2026-11-11: use upstream scoping to keep milestone and later
phase work directionally correct and economical. Retain the high-signal result,
source identity, confidence, alternatives and revisit triggers in the existing
project second brain; no new memory service or dependency is requested. The
[successor research](research/next-milestone/SUMMARY.md) applies the fan-out brief
through relevant consumer, hardware, provenance and operations lenses plus one
adversarial synthesis. Its proposed scope is an input to milestone selection,
not permission to invent a core defect or implement downstream phases early.

## Shared emulator family: learning flows both ways

Local sibling inspection, 2026-11-11: `libfuturo` now implements the independent
core/host contract, a direct Muhtari adapter and native/browser reference
consumers. Reconcile the earlier browser-player/interface seeds with that
project before proposing another host or interface. Keep browser scheduling,
devices, storage and consumer qualification there; Muhtari owns its machine
implementation, public API and portable libretro/core artifacts. See the
[responsibility split](PRODUCT-ROADMAP.md#sibling-responsibility-boundaries--inspected-2026-11-11)
for implementation sources and exact evidence limits. This inspection does not
select a new ABI, change libretro support or authorize sibling operations.

Owner scope correction, 2026-11-11: the separate `twentysixhundo` project owns
Atari 2600 emulation. Remove 2600 mode from Muhtari's future scope and retire
SEED-006; Muhtari remains focused on Atari 7800. Existing image recognition and
refusal are unaffected.

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

## YM2151 milestone authority — 2026-11-11

The owner selected `$gsd-new-milestone YM2151 homebrew sound` and explicitly
allowed this milestone to run in gsd-autonomous style. Adopt the saved successor
proposal for ordinary in-scope decisions and continue through requirements,
roadmap, phases, local verification/review and local milestone closeout without
routine acceptance prompts. Preserve material compatibility decisions, original
Phase 12/14 stops and separate remote/host/identity/publication/merge authority.
This is milestone v6.0, not a package-version request. The agent owns the work;
no owner checkout switch or blanket UAT is required.
