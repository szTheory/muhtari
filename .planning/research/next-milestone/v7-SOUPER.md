---
title: "v7.0 successor recommendation: Souper mapper for homebrew developers"
summary: "Decision record for promoting mapper-only Souper support, with source confidence, ownership boundaries, scope limits and conditions that would change the recommendation."
read_when: "At v7.0 requirements/phase discussion, before changing the Souper board, or when newer source evidence changes its profile."
updated: 2026-11-13
---

# v7.0 recommendation — Souper mapper support

## Recommendation

Select **v7.0 — Souper homebrew mapper support**. Muhtari already recognizes the
v3 cart-type bit and v4 mapper declaration but refuses to load the board. The
original mapper author says Souper was released so developers can build games
using or extending the design. This is a core-owned capability with a useful
developer outcome; it is not a reproduced bug or proof of broad player demand.

Limit the claim to **conformance with one published mapper-only Souper profile**.
Do not claim that Rikki & Vikki works, that BupChip audio is present, or that the
profile has been calibrated against a physical cartridge. Keep the C17 core,
runner, libretro core and portable build on the existing three-deliverable path.
No new package dependency is needed; write the minimum core behavior from the
public mapping facts in Muhtari's own words and tests.

The selected candidate uses the 512 KiB ROM / 32 KiB EXRAM profile, mapper
registers $8000–$8007, the separately mapped CPU and MARIA views, character
remapping, EXRAM segmentation, MARIA fetch trapping, reset and save-state
behavior. A generated diagnostic must carry literal expected bytes authored
from the public mapping description, separate from production implementation
logic. It should drive real CPU and MARIA accesses through the production core.
It must include malformed/unsupported negative cases and continuation through
state save/load. The exact reset register defaults and initial EXRAM contents
must be stated as model policy before implementation; the repository's settled
zero-fill profile is the default unless a stronger Souper source establishes a
different value.

The existing `PROVENANCE.md` audits MiSTer `rtl/souper.v` at `0dc8ad2e3ff7` as
Zlib but marks it read-only, not as a §4 seed. **This recommendation does not
copy or port that file.** The mapping contract is recorded from the public
developer and GameDrive code-example references below, paraphrased in Muhtari's
own words. Keep the established no-GPL/LGPL/unlicensed-source rule. If an
implementation later needs permissive upstream expression, add an explicit
§4 seed row, notice, replacement trigger and parity proof before use; the
current recommendation has no such need.

## Decision comparison

| Candidate | Value | Cost / risk | Decision |
|---|---|---|---|
| No new Muhtari scope | No compatibility or state change; correct if only direct user requests qualify | Continues refusing a published mapper that its author intentionally made available to new homebrew developers | Viable fallback, but weaker than the concrete developer capability |
| **Souper mapper only** | Enables a specific public homebrew-development profile with a generated, lawful diagnostic | New CPU/MARIA mapping, HALT-sensitive reads, 32 KiB state, loader geometry and snapshot/version consequences | **Recommend for v7.0**, with model-conformance language and hard exclusions |
| Full Souper plus BupChip | Would approach the feature set used by Rikki & Vikki | Separate processor/synth, unavailable or unqualified firmware/audio model, larger bounded-cost and state problem | Defer; not required to deliver mapper development capability |
| Accuracy/DLI correction | Could improve a known title if a discriminating counterexample exists | No reproduced Muhtari mismatch or ready independent gameplay oracle; DLI question needs hardware evidence | Revisit when a lawful counterexample or authorized measurement exists |
| LibFuturo PAL/browser work | Advances a downstream consumer toward PAL browser coverage | Belongs to LibFuturo; its current hosted/Linux limits do not establish a Muhtari API defect | Leave with the sibling; no Muhtari code requirement demonstrated |

## Mapper model and acceptance boundary

The public GameDrive code example describes the baseline CPU map as 16 KiB
switchable ROM at $8000–$BFFF, fixed final ROM at $C000–$FFFF, and EXRAM at
$4000–$7FFF. Its segmented EXRAM mode divides that window into a fixed 8 KiB
region plus separately selected 4 KiB windows at $6000 and $7000. It assigns
independent mode bits to EXRAM segmentation, character remapping and MARIA-fetch
trapping. With MARIA-fetch trapping, MARIA's upper ROM window exposes EXRAM;
with character remapping, the MARIA view exposes a fixed ROM window and two
2 KiB character selections. This is three independent controls, not a set of
mutually exclusive S/C/E modes as the older preparation summary simplified it.

The profile's acceptance should cover:

- Correct A78 v3 bit-12 and v4 mapper-4 identification, using existing v4
  precedence and exact geometry validation.
- Bank selection, register writes and defined aliases; character A/B selection;
  flat/segmented EXRAM windows; and read/write distinctions.
- Separate CPU and MARIA fetch results, including MARIA DMA under the existing
  tick-aware HALT path and the documented fetch-trap transition.
- Deterministic reset, EXRAM fill, power-cycle and transactional save/load
  continuation, with every new mutable field in the instance and serialized
  state.
- Generated pattern ROM/EXRAM and literal expected values for each mapping
  boundary, plus malformed size/header and unsupported-profile refusals before
  allocation or mutation.
- Public C, runner, loaded-libretro and existing portable consumer paths, as
  applicable to changed behavior; retain existing non-Souper hashes and
  refusal controls.
- User and hardware documentation stating exactly what the mapper profile
  supports and which audio/image forms remain unsupported.

The pinned MiSTer RTL resets mapper register latches to zero and the public
GameDrive code example describes the reset memory view. This is a cross-check
between two software descriptions, not physical measurement. Use the project's
settled zero-filled RAM profile for EXRAM and label it emulator policy; physical
EXRAM power-on contents remain unestablished. TailChao's released mapper page
timed out during this research; the visible code example and author's published
HALT explanation are the scoped software model, not a complete hardware
specification. The preparation note's older Rikki & Vikki image-size statement
also conflicts with TailChao's own 4-Mbit game plus 2-Mbit music description.
Do not infer the game's exact A78 trailing-payload layout or accept it as a
fixture.

The route written on the mapper's $8007 register is the BupChip command path.
This milestone does not synthesize, queue or expose BupChip audio. The docs and
tests must make that limit visible. Do not accept an image whose mapper/audio
payload layout cannot be parsed under a source-qualified rule. Do not broaden
the profile to SUBPAR/SUBLET flash behavior or other mapper variants.

State compatibility is a phase-discussion decision before code: add all mapper
registers and EXRAM to the native snapshot; test restore continuation and
transactional rejection; then apply DEC.26's separate behavior/state-version
policy. Preserve existing non-Souper state behavior where the format permits,
and do not claim cross-build migration. No public API/ABI bump is implied if
public structures remain unchanged. A release/package version is not selected
by the planning milestone.

## Evidence register

| Claim | Evidence class / confidence | Source and limit |
|---|---|---|
| Muhtari recognizes the Souper header declaration but currently refuses load | Direct code and refusal tests; high for current behavior | `src/cart.c`, `src/instance.c`, `tests/core/test_core.c`, `docs/hardware/cartridge.md` |
| The v3 bit and v4 mapper value identify Souper | Curated header reference; medium/high for header interpretation | [A78 Header Specification, revision 1120](https://7800.8bitdev.org/index.php/A78_Header_Specification?oldid=1120); the wiki has no project-license grant, so use facts only |
| Souper was deliberately published for other developers to build on | Original developer's own statement; high for intent, not demand | [TailChao's Rikki & Vikki post](https://forums.atariage.com/topic/285896-rikki-vikki/), 2018-12-11, edited 2023-09-14. It identifies Souper as the mapper and says the author released it for derivative development |
| BupChip is separate audio, not part of the memory mapper acceptance | Original developer's own statement; high for the named product | Same post: BupChip generates music; game data and music data are distinct. The author says no standalone raw game ROM is sold, and does not support environments outside Windows/BupSystem |
| Detailed RAM/ROM windows and mapper registers/modes are publicly described | Community technical example; medium, not physical measurement | [Eagle's Souper mapper example](https://forums.atariage.com/topic/353011-homebrew-code-examples/#findComment-5292592), posted 2023-08-02. Per the local tier policy, a forum example is T4; corroborate its map with the original author's own notes before calling it a hardware fact |
| HALT/MARIA bus transition deserves time-aware coverage | Original developer explanation, with explicit uncertainty; medium for chosen model, not silicon truth | [TailChao's Sally/Maria memory discussion](https://forums.atariage.com/topic/331505-sally-and-marias-memory-dance/), 2022-02-19: describes the second falling PHI2 edge and explicitly calls parts of the explanation an elaborate guess pending die-level evidence |
| The MiSTer `souper.v` file is Zlib at a known pin and its register reset/decode agrees with the public map; it is not an approved seed today | Read-only source cross-check plus provenance record; high for that pinned implementation, not hardware accuracy | [PROVENANCE.md §2/§4](../../../PROVENANCE.md); pin `0dc8ad2e3ff7`; checked register reset/decode; no code or table copied |
| No demonstrated Muhtari need arises from LibFuturo's current PAL/browser work | Owner-supplied sibling feedback plus prior read-only inspection; high within those snapshots | No Muhtari API/package requirement; PAL/Linux and hosted qualification remain LibFuturo work. Firefox failure is inconclusive and no retry is authorized |
| No current player-visible accuracy defect is demonstrated | Existing successor research; high for evidence absence, not for silicon accuracy | [ACCURACY](ACCURACY.md); DLI remains conditional on a lawful discriminating trace or separately authorized hardware observation |

## Fan-out and adversarial synthesis

Independent consumer, hardware and roadmap reviews now converge on mapper-only
Souper as the strongest Muhtari-owned successor **if the promise is scoped to
homebrew development rather than Rikki & Vikki compatibility**. The adversarial
review's strongest reason to defer is the absence of broad demand plus reliance
on one community-published map and no physical board measurement. The answer is
to constrain the public claim to that pinned software model and require an
independent, generated expected-value diagnostic. If an original-author source
or coherent reset/EXRAM policy cannot support that gate, defer before
requirements rather than widening the scope.

The fan-out is complete at diminishing returns: more Atari 7800 product listings
do not show Souper demand; sibling PAL work does not show a core/API gap; and
additional emulator implementations risk weakening the project's source policy
without changing acceptance. Do not copy GPL/LGPL/unlicensed emulator code as a
cross-check.

## Rolling roadmap recommendation

- **Immediate — v7.0:** one mapper-only Souper profile with generated
  CPU/MARIA diagnostics, state continuation, honest refusal boundaries and the
  recurring measured tune-up first.
- **Near term — next:** promote one specific gameplay/accuracy correction only
  after a lawful scenario, independent expected result and evidence that the
  fault belongs to Muhtari. Otherwise keep this horizon open rather than
  inventing another feature.
- **Mid term:** revisit broader 7800 homebrew hardware (BupChip, composite
  filtering or other boards) only after its source/firmware/license profile,
  fixture strategy and bounded runtime costs are known. DLI or timing work needs
  a real discriminator; no measurement-only phase.
- **Long term:** tools/TAS/AI and low-power ports follow demonstrated users and
  measured device budgets. Browser consumers and PAL browser qualification stay
  in LibFuturo; Atari 2600 stays in `twentysixhundo`.

**Revisit triggers:** a direct request for a different title or feature; a
lawful exact Souper game image with permission and a usable game-level oracle;
an original-author specification resolving the remaining reset or payload
questions; a physical Souper trace; a reproduced title-blocking defect; or a
concrete Muhtari API/portability failure from a sibling. A later pass on another
source or platform does not reopen earlier denied/spent host or remote scopes.

**No implementation, build, test, host run, sibling operation or publication was
performed during this research.**
