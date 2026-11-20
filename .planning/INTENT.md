# Shared project intent and provenance

**Source:** owner direction in the 2026-10-29 milestone/onboarding conversation,
combined with the standing contracts linked below. These are product preferences
and accepted instructions; technical research and observed behavior have separate
evidence. Updated 2026-11-19.

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

Owner direction, 2026-11-15: write the GitHub README in plain, friendly language
for people evaluating or using the project. Keep it short and current. Link a
maintained status page that says what is available, what remains, and what
evidence would count as completion; refresh both when the project state changes.

Owner direction, 2026-11-19: Muhtari is entering a maturity and usability stage.
Keep its user jobs and personas grounded in current deliverables and dated
consumer evidence; distinguish direct feedback from inference and preserve
ownership boundaries. Each milestone should look for the largest concrete
friction in how players, runner users, homebrew developers and library
integrators get started. Improve the affected docs and comments as work proceeds:
remove generic narration that does not explain intent, keep hardware claims
sourced, and fix the highest-impact code or documentation debt found on those
paths. Avoid a repository-wide style sweep, unsupported compatibility claims,
and new host or device responsibilities.

Owner direction, 2026-11-19: keep the repository root as the canonical
workspace for sequential GSD work. At milestone close, finish publication and
archive work, return root to updated `main`, and leave it clean while preserving
explicitly local settings. Use a separate worktree only for concurrent work or
to protect existing user changes; reconcile its ancestry and dirty contents
into root and remove the temporary checkout before handoff. The next milestone
should start from the root checkout without asking the owner to switch folders.

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
preserving its required coverage. Put cheap, deterministic consistency and sanity
checks ahead of long builds or suites when they can catch a likely failure early;
keep them narrowly tied to that failure mode rather than adding setup with no useful
screening value. Treat one run as a lead, not enough evidence for structural,
timeout, or cache changes. This is standing guidance, not another UAT gate.

Owner direction, 2026-11-19: apply the fan-out brief to every phase discussion.
For each decision, use independent specialist subagents where another perspective
could change the recommendation; consider relevant product, design, engineering,
security, reliability and operations lenses as the domain requires. Compare real
alternatives with their tradeoffs, examples, patterns, footguns and strongest
counterargument; prefer primary sources for niche or changing facts. Synthesize one
coherent recommendation per decision and stop when added research cannot change it.
Keep the dependency tree small when repository code, standard facilities or the
platform meet the need. This is standing discussion guidance, not a request to
reopen already locked phase context.

Owner direction, 2026-11-19: review every CI and Release/CD run while active and
after completion for queue time, critical path, job durations, cancelled work and
low-effort ways to shorten feedback and iteration time. Preserve required test,
fault and release coverage. Put cheap relevant consistency checks before long
work, record when evidence supports no safe improvement, and require measured
results across runs before changing workflow structure, caches or deadlines.
Use GitHub `act` for compatible local preflight when it improves feedback, while
keeping Docker/local results distinct from hosted platform, permissions, OIDC,
artifact and release evidence.

Owner reaffirmation, 2026-11-11: use upstream scoping to keep milestone and later
phase work directionally correct and economical. Retain the high-signal result,
source identity, confidence, alternatives and revisit triggers in the existing
project second brain; no new memory service or dependency is requested. The
[successor research](research/next-milestone/SUMMARY.md) applies the fan-out brief
through relevant consumer, hardware, provenance and operations lenses plus one
adversarial synthesis. Its proposed scope is an input to milestone selection,
not permission to invent a core defect or implement downstream phases early.

Owner direction, 2026-11-13: at each new-milestone boundary, do the saved research
and recommendation work first. Reconcile shipped work, LESSONS, consumer needs,
sibling ownership and current technical evidence; update the recommendation and
rolling roadmap before presenting options. Lead with the recommended scope and
the strongest alternative rather than asking a generic "what do you want to
build next?" When the owner says to follow the recommendation, carry the
ordinary in-scope choices through requirements and roadmap without routine
confirmation. Stop only at a real scope, authority, risk, cost, irreversible
action or owner-only validation boundary. Save high-signal sources/revisions,
evidence class, confidence, contradictions, alternatives and revisit triggers
in the existing `.planning` second brain. Reuse settled research unless new
evidence could change the recommendation. This is an owner workflow preference,
not a reproduced technical lesson for `LESSONS.md`.

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

## Recommendation-first autonomous milestone starts — 2026-11-14

At every new-milestone boundary, present a concrete successor idea and its
strongest alternative, then refresh this rolling roadmap before a scope choice.
The owner wants selected milestones to run in gsd-autonomous style through
dependent local planning, implementation, verification, review and closeout
without routine prompts. Preserve material public API and compatibility
decisions, host/device action, identity-bound consent and irreversible changes.
The owner has preauthorized ordinary Git publication, PR merge and the CI/release
automation they trigger at natural milestone delivery points. Preserve all
other exact stops until new evidence or authority resolves them.

## Early publication and cheap preflight — 2026-11-14

Publish at natural delivery points, release early and often, and finish each
milestone with `main` CI green. Before a long suite or process, run applicable
cheap consistency checks first. Review reproduced lessons and recent bottlenecks
for low-effort sanity checks or fixes; investigate a failed check before retrying
it. Keep CI/runtime evidence and any measured efficiency finding with the run.
When the work is complete and all required CI checks are green, squash-merge its
PR without asking for another confirmation. Keep an explicit hold, safety stop,
or unresolved gate in place unless the owner directly clears it.

## Feature returns and compatibility evidence — 2026-11-16

The owner asked whether Muhtari is reaching diminishing returns from adding
features. At the next `$gsd-new-milestone`, answer from the current shipped state
and evidence: distinguish diminishing returns from project completion, inspect
the compatibility and consumer gaps, and recommend a concrete successor with
its strongest alternative. Reconcile and refresh the [rolling roadmap](PRODUCT-ROADMAP.md)
before presenting the scope choice. Reuse its current evidence and recommendation
as the starting point; update them only where new evidence changes the ranking.
