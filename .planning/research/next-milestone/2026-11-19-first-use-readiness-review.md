---
title: "Successor review: first-use readiness"
summary: "Re-ranks the next milestone around current owner direction, user jobs and concrete onboarding/documentation friction after v11.0 shipped."
read_when: "At the v11.0 close and when evaluating user-facing documentation or onboarding work."
updated: 2026-11-19
---

# Successor review — first-use readiness

**Status:** Recommended and selected for v12.0 under the owner's direction to
follow the scoped recommendation and run the milestone through `$gsd-autonomous`.

## Recommendation

Recommend **v12.0 — First-use readiness**. Help the three primary users move
from a current release artifact to a first useful result: players load a game in
RetroArch, runner users execute a cartridge or the built-in test card, and C
integrators build a small host against the library package. Refresh the user
persona/JTBD map against the shipped v11.0 product and current evidence. Keep
instructions short at entry points and leave complete hardware/API detail in
reference docs.

Treat small source and documentation debt as routine maturity work. For this
milestone, scope fixes to debt that blocks, confuses or contradicts these first
use paths. When code is touched, remove redundant narration and generic
comments, retain comments that explain intent or non-obvious behavior, and keep
hardware claims sourced. This does not authorize a broad aesthetic rewrite.

## Evidence from the current product

| Finding | Evidence class / confidence | Consequence |
|---|---|---|
| v11.0 shipped as stable v0.18.0, and PR #84 archived phases 28–29 and refreshed the successor recommendation. | Release/project receipts; high | Start from merged main at `cd7ce9e792b452647e0d438ee6d188ff778c4641`, not the older root checkout. The archive PR is [#84](https://github.com/szTheory/muhtari/pull/84). |
| The [README](../../../README.md) repeats the project description, links Phase 25 to `.planning/phases/`, and the RetroArch quick start ends at the test card without a Load Content step. | Direct source inspection at `cd7ce9e`; high | Replace duplicated and stale material with the shortest path to loading a user's own lawful image and clear next links. |
| [docs/status.md](../../../docs/status.md) has the same stale Phase 25 link; current archive evidence lives under `.planning/milestones/v9.0-phases/`. | Direct source inspection at `cd7ce9e`; high | Repair current public-facing links and verify all local links touched by the milestone. |
| [docs/libretro.md](../../../docs/libretro.md) and [docs/runner.md](../../../docs/runner.md) contain detailed reference information, while [docs/embedding.md](../../../docs/embedding.md) explains package targets and API contracts with snippets. | Direct source inspection at `cd7ce9e`; high | Preserve the reference material; add concise step-by-step getting-started paths rather than compressing technical contracts. |
| The [2026-10-29 onboarding lesson](../../LESSONS.md#local-player-onboarding-2026-10-29) says a README ending at the test card leaves the player's next action unclear; the current README still lacks Load Content instructions. | Prior reproduced local onboarding evidence plus current source inspection; high for the missing step | Complete the player's journey with core and own-media load steps, optional BIOS distinction and relevant control notes. No recurring owner UAT is created. |
| The [ecosystem and persona research](../../preparation/A7800-ECOSYSTEM-AND-USERS.md#3-personas-and-jobs-to-be-done) dates from the initial product roadmap; its jobs and horizons mostly describe v1.0–v6.0, before the current three-deliverable and v11.0 product. | Dated internal research; high for age, medium for which needs changed | Update its persona/JTBD section and mark user claims by evidence class/date. Keep future personas directional and sibling-owned work separate. |
| The compatibility ledger has 201 unevaluated title rows; the successor review found no exact rights-cleared title plus independent gameplay result. | Current ledger and saved review; high | Do not promote broad compatibility work or reuse protected media for onboarding. Keep the replay scenario conditional. |

## User jobs to preserve

1. **Player:** install the released libretro core in a supported RetroArch
   setup, load a lawfully obtained 7800 image, understand the first screen and
   controls, and know that BIOS is optional under fast boot.
2. **Runner user / homebrew developer:** use a release or local build to probe
   and run their own image, understand deterministic outputs and refusal
   statuses, and find the detailed movie/state/trace reference when needed.
3. **C host integrator:** download the library package, link the published
   CMake or pkg-config interface, provide the required allocator and output
   buffers, and run one frame with a known result.

These are the core product jobs for the milestone, not claims of fresh user
interviews. Existing speedrunner, TAS, ROM-hacker and AI-agent needs remain in
the planning research, but are not all first-use targets. Keep LibFuturo,
Playstead and `twentysixhundo` ownership boundaries explicit.

### Evidence distinctions for the persona refresh

- The player job has one recorded, voluntary owner observation from a local
  RetroArch session, plus earlier reproduced onboarding evidence. Those support
  the existence of a player path, not general compatibility, representative
  demand or all-platform success.
- Homebrew/runner needs and the C host-integrator job are inferred from the
  current released CLI and library interfaces and the dated ecosystem research;
  this review records no direct interview or adoption evidence for either job.
- The persona refresh should link each claim to its dated source, label direct
  observation versus source-backed need versus inference, and preserve the
  confidence and revisit trigger. Do not upgrade a product persona into a
  validated customer segment without new evidence.

## Comparison and decision

| Path | Value | Risk / cost | Decision |
|---|---|---|---|
| Do nothing | Avoids changing stable docs and onboarding behavior. | Leaves a missing player step, broken release-archive links and stale persona horizons. | Reject: direct source evidence and owner direction justify a bounded correction. |
| Fix only the broken README links | Cheaply removes inaccurate references. | Does not help a new player load a game, a runner user find a first workflow, or an integrator use the package. | Too small for the stated maturity goal. |
| **First-use readiness** | Improves first success across all three deliverables and refreshes the evidence behind product priorities. | Requires careful limits so clearer docs do not overstate compatibility or host support. | **Recommend and proceed.** Use existing release/package routes and generated/legal fixtures. |
| Finite gameplay replay | Adds a player-visible scenario regression signal using existing movie support. | No exact lawful fixture and independent expected gameplay result is in hand. | Strongest product alternative after its evidence gate closes. |
| Windows ARM64 CI profiling | Could shorten developer feedback. | Current runs show a long stage but no per-test attribution or demonstrated safe change. | Keep later; review future runs and do not predetermine a workflow change. |

## Rolling horizon check

- **Immediate:** v12.0 first-use readiness, selected after reconciling the
  shipped release, existing onboarding observations, current docs and the
  owner's maturity direction.
- **Next:** one finite gameplay replay only after exact fixture rights and an
  independent expected gameplay result are recorded.
- **Mid:** measured accuracy or qualified hardware only after a source-backed
  question, lawful discriminating fixture and bounded cost are available.
- **Long:** debugging, scripting, AI/TAS and ports remain tied to demonstrated
  users, sibling ownership and measured device budgets. No new evidence changes
  those gates in this review.

## Scope and acceptance boundaries

- Refresh the existing persona/JTBD material using current Muhtari deliverables,
  recorded user observations and dated project/consumer evidence. Label
  inference; do not describe untested games as failures.
- Make the README lead to a real game-loading path, not stop at the test card.
  Cover platform package choice, required media source, optional BIOS, useful
  controls and honest support limits.
- Give `muhtari-run` users an understandable first command for the test card and
  their own cartridge, with expected output and a route to full reference docs.
- Give C integrators one complete minimal example built against the installed
  package, with allocator, buffer sizing, one frame and expected status/output.
- Repair stale local archive links and audit claims/commands affected by those
  journeys against current code and published package contents.
- Reuse existing package-consumer and CI checks where they protect a recurring
  example. Add no general prose linter without a specific repeatable fault and
  pass/fault coverage.
- Keep ROMs, BIOS images, saves, screenshots and personal paths out of Git.
  Use the built-in test card or existing generated/manifest-approved fixtures.
- Add no host UI, device code, API/ABI/state change, new dependency, or
  universal compatibility claim. Preserve the denied Phase 12 request, spent
  Phase 14 host scope, GameDrive NO-GO and sibling ownership.

## Confidence and revisit triggers

Confidence is high that there are stale links and an incomplete README player
path, medium-high that these are the best immediate user improvements, and
medium for the freshness of each persona because the ecosystem survey is dated
and no new interviews are claimed. Revisit if current user reports identify a
more blocking journey, a package changes before the docs are completed, or
source inspection shows the apparent link/path defects have already been
corrected. Promote gameplay replay when rights and an independent outcome are
documented; revisit Windows profiling only when comparable data attributes
repeatable cost and identifies a coverage-preserving option.
