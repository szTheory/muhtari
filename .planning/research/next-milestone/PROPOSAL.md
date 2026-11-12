---
title: "Proposed v6.0: YM2151 homebrew sound"
summary: "Upstream scope recommendation, acceptance boundaries and decision gates; not approved requirements or a phase execution plan."
read_when: "Continuing the existing new-milestone discussion or scoping the selected sound-hardware phases."
updated: 2026-11-11
---

# Proposed v6.0 — YM2151 homebrew sound

**Status:** proposed scope for the ongoing new-milestone discussion. No live
milestone switch, requirement reset, phase plan or implementation is authorized
by this document. User direction authorizes upstream fan-out and a coherent
recommendation; the consequential product-scope choice remains reviewable here.

## Recommended outcome

A declared, documented YM2151 cartridge profile that is currently refused can
produce deterministic FM sound through Muhtari's C library, runner and libretro
core, including its portable Wasm artifact. Existing games, POKEY/TIA synthesis,
battery semantics and unsupported-device refusal remain covered.

The observed gap is explicit: `src/instance.c` refuses the YM2151 declaration
before allocation; `expansion_sound_refusals` in `tests/core/test_core.c` protects
that contract. This is a missing capability, not a reproduced retail-game bug.
First-party programming/game evidence and seed feasibility are in
[HOMEBREW](HOMEBREW.md). Adding this device must not imply full XM hardware,
universal YM cartridge support or tested gameplay in named commercial games.

## Scope and exclusions

- One evidence-supported YM2151 mapping/enable/clock profile within the existing
  selected/autodetected XM expansion topology; YM declarations outside that
  qualified topology continue to refuse. Cover address/data
  ports, busy/status, timers and declared interrupt wiring at their owning bus
  deadlines; retain CPU/MARIA ownership and integer regional timing. Reconcile
  clock and control-latch evidence before committing the profile. Unsupported
  topologies continue to refuse before mutation/allocation.
- Specialize the already approved BSD-3 ymfm OPM seed into C17 if phase research
  confirms that route remains feasible. Preserve exact upstream license and
  notices; record the seed pin and update duty. No runtime C++/package dependency,
  second emulation scheduler, DSP framework or unrelated chip port is selected.
- Integrate the new device into the existing bounded audio and lifecycle owners:
  load/reset/run, integer headroom, non-silent PCM, canonical audio observations,
  instance isolation, and complete transactional state capture/restore/refusal.
  Preserve the public core's mono PCM contract; any internal chip left/right
  channels fold into the existing documented integer mix, and libretro retains
  its existing duplicated-mono host output. A new public stereo API is excluded.
- Exercise the actual public library, headless runner and loaded-libretro paths,
  then the existing portable artifact and clean package consumers. Document
  supported requests and limits through existing user/developer docs.
- Exclude Souper/BupChip, Covox/ADPCM, full XM services/firmware, Atari 2600,
  composite/video filters, browser UI, LibFuturo adapter changes, Playstead,
  real-device/physical-audibility proof, stable shared ABI and generalized
  cross-build snapshot migration. No old host allowance is reopened.

## Candidate phase shape — ordering only

1. **Measured milestone tune-up.** Reuse final shipping receipts; correct stale
   documentation and the one weakest demonstrated quality dimension. No new
   release recovery, speculative CI restructuring or broad timeout campaign.
2. **Complete YM2151/XM vertical slice.** Resolve the exact cartridge profile,
   clock, license and compatibility policy; deliver useful chip/bus/audio/state
   behavior through the public C API, runner, libretro and portable artifact
   with independent numeric, port-equivalence and package evidence. Include
   measured operating cost and documentation in this behavior phase. An isolated
   chip engine or a feature with its required tests deferred does not complete it.

These are milestone boundaries, not task/wave designs. Keeping the feature and
its deliverable acceptance together avoids a chip-only release followed by a
validation tail. The roadmapper may adjust scope boundaries with source evidence,
but may not defer behavior tests to another phase. Proposed numbering is Phase
19 tune-up and Phase 20 complete feature after approval; neither exists yet.

## Smallest honest acceptance

1. Own a lawful deterministic register/diagnostic workload that actually uses
   the selected YM profile. The old source refuses it; the implementation loads,
   advances and emits independently checked non-silent sound. Do not require
   copyrighted game assets as the recurring acceptance gate.
2. State exact clock/map/enable and timer/status/IRQ expectations from hardware
   documentation. Use original ymfm as an external port-equivalence oracle for
   complex synthesis where permitted. Those comparisons establish faithful
   porting, not independent hardware accuracy or analog calibration.
3. Check lifecycle, bounded hostile register traffic, partial-frame carry,
   instance isolation, buffer ownership and state continuation. Wrong identity,
   unsupported topology and corrupt/semantic-invalid states must preserve the
   previous machine and committed battery data.
4. Require the same owned workload through all three deliverables, native lanes
   and the portable consumer as applicable. Compare independent numeric results
   before hashes; retain existing PCM stereo/batch/fallback and refusal controls.
5. Measure representative ordinary and worst legal YM work, integer width/
   headroom, memory/state extent and Wasm/native cost using current mechanisms.
   Preserve current required checks. The existing test-card performance gate
   alone does not bound YM synthesis; do not invent a numerical target before
   phase research and measurement establish a defensible budget.
6. State API/behavior/native-format consequences explicitly before implementation.
   A new sound-device snapshot cannot silently inherit current compatibility.
   Preserve or explicitly revise old contracts under an approved policy; no
   forced migration of LibFuturo exact-build envelopes is assumed.

Existing native workflow, focused tests and `scripts/emscripten.sh` are the
mechanisms; detailed inventory and exact commands belong in phase planning.
No acceptance command was run during this upstream research.

## Main tradeoff and fallback

This is substantially more implementation than adding a flag or another package.
The payoff is a concrete new 7800 capability with documented homebrew use.
Clock/mix uncertainty, the OPM port, audio/state integration and ongoing seed
maintenance are real costs. Label the supported digital model and unmeasured
analog mix honestly instead of calling it hardware-calibrated.

The strongest different Muhtari outcome is one measured accuracy/gameplay fix.
It wins if a lawful discriminating scenario and independent counterexample are
provided. Current research finds neither a reproduced mismatch nor a ready
gameplay oracle; see [ACCURACY](ACCURACY.md). Do not create a measurement-only
milestone whose necessary console observation is unauthorized.

If profile evidence, licensing or integer/bounded feasibility fails during
research, stop that scope decision and compare a narrower supported profile or
the alternative with the new evidence. Do not silently expand to full XM or
accept a chip-only implementation as the promised deliverable outcome.

## Next material decision

Recommend choosing this feature milestone rather than another browser/package
slice. The owner should be deciding the product outcome and its new sound/state
compatibility consequences, not routine research or test commands. On selection,
continue the current `$gsd-new-milestone` workflow through its required scope,
requirements and roadmap review; use this research as input instead of asking
the same upstream questions again. Phase implementation remains downstream.
