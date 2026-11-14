---
title: "Historical v6.0 successor synthesis: YM2151 homebrew audio"
summary: "Preserved rationale for selecting v6.0; the current successor recommendation and rolling horizons are in v7-SOUPER.md and PRODUCT-ROADMAP.md."
read_when: "Understanding why v6.0 was selected, or reviewing historical evidence before v7.0."
updated: 2026-11-13
---

> **Historical v6.0 record.** The recommendation below was adopted for the
> shipped YM2151 milestone and is no longer the current successor proposal.
> The owner-supplied LibFuturo feedback and the 2026-11-13 Souper fan-out
> supersede its “next” recommendations. Read [v7-SOUPER](v7-SOUPER.md) and
> the active [rolling roadmap](../../PRODUCT-ROADMAP.md) for current scope.

# Recommendation and status

Recommend **v6.0 — YM2151 homebrew sound**, limited to one documented YM profile
within the existing selected/autodetected XM topology. Preserve the public mono
PCM interface, existing C library/runner/libretro deliverables and portable
artifact. The [scope proposal](PROPOSAL.md) defines the outcome, exclusions,
candidate phase boundaries and meaningful acceptance. This is upstream scope,
not an approved requirement set, a live milestone switch or a phase plan.

Why now: the current loader expressly refuses a declared YM2151 sound device,
and first-party homebrew material establishes a use for the music capability.
This is a concrete missing feature. Research did not find a reproduced current
accuracy defect, a ready lawful gameplay oracle, or a demonstrated new core/API/
package need from LibFuturo. Avoid inventing one to occupy the next milestone.

Three bounded independent specialist passes covered accuracy/oracles,
homebrew hardware/provenance, and consumer/product ownership. The orchestrator
covered shipping/evidence/resource operations and owns this synthesis. One
separate adversarial pass checks the combined decision; its final disposition
and corrections are in [ADVERSARIAL](ADVERSARIAL.md). Further fan-out is warranted
only if a source contradiction could change product scope or acceptance.

**Adversarial disposition: modify, then recommend.** Adopt one XM topology only,
keep public mono PCM, include timer/status and declared IRQ behavior, and make
behavior/native-state compatibility explicit. A currently marketed
[Block'em Sock'em cartridge](https://store.atariage.com/products/blockem-sockem-atari-7800)
advertises Yamaha audio, supporting present feature value without proving its
exact header/map or Muhtari playback. Do not claim that title is a supported
profile without a lawful scenario and further evidence. The final proposed
shape is **two phases: proportional tune-up, then one complete feature phase**,
keeping device, state, runner/libretro/Wasm and required acceptance together.
This avoids shipping a chip-only intermediate or deferring behavior tests.

## Evidence register — keep claims distinct

| Finding | Kind / confidence | Source and consequence |
|---|---|---|
| v0.13.1 is published; public-byte checks, latest install and current-main CI closed | Verified retained receipts; high | [Final ship receipt](../../milestones/v5.0-SHIP.md#final-publication-and-current-main-closure--reconciled-2026-11-11). Do not restart shipping recovery. Failed v0.13.0 remains separate. |
| YM metadata is parsed but executable load refuses before allocation | Direct current source/test contract; high | `src/instance.c:356`, `src/cart.c`, `expansion_sound_refusals` in `tests/core/test_core.c`; [HOMEBREW](HOMEBREW.md). No new test was run here. |
| Pac-Man Collection 40th's developer identifies XM/Yamaha music and a TIA fallback | First-party intended use; medium | [Developer thread](https://forums.atariage.com/topic/304268-pac-man-collection-40th-anniversary-edition/). This establishes a music use, not game dependence, playability or permission to distribute its bytes. |
| YM chip digital behavior and a permissive OPM seed are documented | Chip manual and audited license; high for their respective claims | [Yamaha manual](https://map.grauw.nl/resources/sound/yamaha_ym2151_synthesis.pdf), [PROVENANCE](../../../PROVENANCE.md), ymfm pin `81aec25ccbb9`. Seed parity proves port equivalence, not independent hardware accuracy. |
| Physical XM map/clock/IRQ/mix evidence is incomplete or conflicting | Curated programming references and implementation comparison; medium/low | [HOMEBREW profile boundary](HOMEBREW.md#exact-supportedrefused-scope), [audio preparation](../../preparation/A7800-AUDIO.md). Choose and label a defensible digital profile before code; do not imply calibrated analog/hardware behavior. |
| HALT, DLI, POKEY and controller contracts include uncertainties but no identified reproduced defect | Source/contract inspection; high for evidence absence, not silicon correctness | [ACCURACY](ACCURACY.md). The owned DLI diagnostic is a conditional discriminator; a physical-console run has no new authority. |
| 201 pinned titles are unevaluated; title screens and diagnostics do not prove gameplay | Generated compatibility/ledger evidence; high | [Compatibility](../../../docs/compatibility.md), `tests/compat/ledger.csv`. Unknown rows are not failures or candidates for mass promotion. |
| LibFuturo implements native/browser `lf_*` consumers over the C core | Pinned and dirty local source inspection; high within recorded snapshot | [CONSUMERS](CONSUMERS.md). Current browser PAL work is underway; source commits and planning text do not establish completed acceptance. |
| LibFuturo has no demonstrated new upstream API/package requirement here | Bounded inference; medium | Its native PAL/snapshot and browser profiles use existing Muhtari API 8 pins. Revisit on an exact consumer reproducer, not a stale summary or guessed API need. |
| Atari 2600 belongs to twentysixhundo | Owner decision; binding | [INTENT](../../INTENT.md), DEC.03 and retired SEED-006. Shared tooling/lessons remain possible. |

## Coherent choices and rejected alternatives

| Decision | Recommendation | Strongest alternative / revisit condition |
|---|---|---|
| Next Muhtari outcome | Add one declared YM/XM audio profile with owned diagnostic acceptance | A specific accuracy/gameplay correction wins if a lawful discriminating scenario and independent counterexample arrive. |
| Hardware breadth | One YM device/map/enable/clock model, existing XM owners only | Souper/BupChip is broader and lacks a ready lawful music/game acceptance path; revisit when its firmware/protocol/fixture rights are resolved. Covox/ADPCM/full XM remain deferred. |
| Core code/reuse | Audit and specialize the already selected BSD-3 OPM seed to C17; retain notices and update duty | Runtime C++ or a package/DSP abstraction adds portability and maintenance scope without present necessity. Reconsider only on measured feasibility evidence. |
| Correctness oracle | Independent chip numeric expectations plus separately labeled exact-seed synthesis parity | RTL/emulator agreement alone cannot establish physical XM timing or analog gain. Hardware evidence is optional separately authorized work, not hidden owner UAT. |
| Audio shape | Keep core mono and current duplicated-mono libretro output | Stereo public API is a separate product/compatibility decision, not an incidental consequence of a stereo chip. |
| State/API compatibility | Decide the actual new device/state schema consequences before implementation; preserve refusal and existing content behavior | Do not assume current behavior/native-format versions or LibFuturo exact-build envelopes migrate. No new shared ABI is selected. |
| Browser/player work | LibFuturo owns interface/adapter/browser qualification; Playstead or product host owns adoption/player UX | Another browser host or optional interface project inside Muhtari duplicates implemented routes. Revisit only on a concrete unsupported upstream contract. |
| Delivery | Small measured tune-up, useful vertical feature slice and current package/native/Wasm checks | Another general CI/observability/measurement milestone would delay a concrete outcome; [OPERATIONS](OPERATIONS.md) explains cost and limits. |

## Acceptance principles carried downstream

- An owned workload must be refused by old source, load through the newly
  supported topology, observe numeric status/timer/IRQ behavior, and produce
  non-silent bounded audio through actual public consumers. Do not make retail
  game assets or a new host permission the acceptance dependency.
- Exercise complete device state, reset, partial-frame carry, isolation, hostile
  legal writes, buffer bounds and refusal without live-machine/battery mutation.
  State and mixer integration belong to the feature, not an optional later tail.
- Preserve TIA/POKEY-only outputs and all current required checks. Measure new
  YM work/headroom and memory/state growth with representative workloads; the
  existing test-card Ir gate is insufficient evidence of its cost.
- Keep exact-source package transport and clean extracted/installed-consumer
  checks. Reuse current CMake/sh, native matrix and portable producer; no new
  dependency, CI topology or generic evidence system is justified.
- Keep chip-model/port-parity claims separate from game compatibility, physical
  audibility, calibrated accuracy, broad browser support and host adoption.

## Rolling horizons

**Immediate:** choose the proposed YM scope and source-qualified profile; keep
the required tune-up short and driven by already demonstrated faults. Shipping
execution is finished. No successor phase is selected until milestone approval.

**Next:** ship the one YM capability through current deliverables. Independently,
LibFuturo advances PAL browser/profile qualification with its own source/ref and
consumer gates; adoption is host/Playstead work, not Muhtari's phase boundary.

**Mid:** promote a measured 7800 discrepancy or lawful game scene when evidence
arrives; consider another missing homebrew device one at a time. Qualify actual
cost before promising a low-power platform. Do not turn an evidence gap into a
bug claim or reopen denied host scopes.

**Long:** focused developer observations, recording/TAS/AI and wider ports follow
concrete users; interface evolution belongs to implemented LibFuturo work.
Atari 2600 is excluded from Muhtari. Cross-project mechanisms remain hypotheses
in the receiving project until locally reproduced.

## Durable memory and authority

The planning index points here; the rolling roadmap and prompt-coverage ledger
carry the recommendation and owner corrections. [LESSONS](../../LESSONS.md)
keeps locally reproduced mechanisms separate from incoming LibFuturo hypotheses.
The earlier [cross-project packet](../../continue.md) is context; this synthesis
and the proposal supersede its provisional recommendation after owner-requested
fan-out. Reports retain source pins/paths, confidence, unknowns and flip conditions.

No new test, build, app, ROM acquisition, hardware run, CI dispatch or sibling
mutation was performed. Research used local source/retained evidence and bounded
online reading. The denied Phase 12 diagnostic-app scope, spent Phase 12/14
host/remote attempts, expired windows and exact no-retry limits remain intact.
No command with `--auto` or `--chain` is permitted. Research notes do not renew
allowances or authorize implementation, publication or a new phase.
