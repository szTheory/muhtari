---
phase: 20-complete-ym2151-homebrew-sound
researched: 2026-11-11
confidence: medium
---

# Phase 20 Research — Complete YM2151 homebrew sound

## Summary

Reuse the completed independent profile, state-compatibility/adversarial and
integration research listed below. Their questions have terminal answers; this
consolidation makes those source-grounded findings available to the normal phase
planner without repeating the upstream survey. It is research, not an execution
plan or an implementation/performance pass.

One qualified digital XM/YM profile is feasible in principle with a specialized
integer C17 OPM port at the approved BSD-3 pin. Existing owners can accommodate
its timing, audio and state without a new scheduler or public layout. Preserve
non-YM results/format-1 envelopes; use a distinct YM format-2 envelope with
explicit global-latest version reporting. The exact C port, numeric gain,
semantic state validation, independent equivalence and operating cost still
require implementation evidence. A prototype or chip-only result cannot satisfy
this feature phase.

<user_constraints>
## Implementation Decisions

### Qualified digital cartridge profile

- **D-01:** Accept a declared YM device only within the existing explicitly selected
  or header-autodetected XM topology. A plain XM selection without a YM declaration
  retains its existing behavior. Unsupported YM topologies, Covox/ADPCM, Souper
  and invalid combinations continue refusing before allocation/installation or
  live-machine/battery mutation. Preserve existing cartridge and expansion owners.
- **D-02:** Use YM address/data ports at `$0460/$0461`, status at `$0461`, gated
  by D7 of the existing exact `$0470` whole-byte XM latch. Clearing D7 removes
  register access without separately resetting the chip. Preserve current HSC,
  POKEY and RAM controls at bits 3–6 and page registers at `$0478/$047C`; add no
  aliases or separate enable latch. `$84` is an enable example, not equality.
  The old MAME exact-byte alternative and physical-XM evidence limits remain
  explicit in the profile evidence.
- **D-03:** Advance one YM clock per two MARIA ticks, independent of CPU slow
  cycles and HALT. NTSC is exactly 315,000,000/88 Hz; PAL is 3,546,894 Hz.
  Retain integer phase/carry and exact event ordering; never truncate NTSC into
  an integer-Hz scheduling rate. Normal output is one step per 64 YM clocks.
  This is a declared digital fit, not measured physical XM timing.
- **D-04:** Implement address/data, busy/status, timers/flags and the chip IRQ
  request. Route YM IRQ only through accepted valid A78 v4 source bit 2 into
  the current bus sampling/deadline owner. No v3/headerless or invalid-v4 IRQ
  inference. Source-independent Yamaha numeric checks precede seed parity;
  establish exact busy and simultaneous-write/timer ordering from qualified
  chip sources before freezing expectations.

### Seed, audio and state compatibility

- **D-05:** Specialize only the approved BSD-3 ymfm OPM path at
  `81aec25ccbb98f4873a255f7551ac4dadac59b4a` into fixed-width integer C17.
  Keep exact seed/license provenance, full notices in every distribution and
  explicit update duty. No production C++ runtime/package or unrelated chip
  port. The approved original seed may be built as a separately labeled
  port-equivalence oracle; it does not establish physical XM accuracy.
- **D-06:** Keep public API 8 and ABI 1, existing size-tagged prefixes and
  signatures, mono signed PCM, and duplicated-mono libretro output. Existing
  declaration/selection fields suffice. Extend private YM observation capacity
  and source handling with an explicit signed-sample encoding; retain every
  old-source canonical 11-byte record and no-YM observation/PCM result.
- **D-07:** Adopt the independently reviewed additive state policy: preserve
  unchanged non-YM native state bytes, identity and format 1/behavior 6 envelope;
  use a distinct format 2/behavior 7 envelope and YM profile identity only for
  YM machines. Reuse the topology-aware visitor. Global version reporting is
  latest supported behavior 7/format 2; document actual per-machine envelopes.
  Do not globally append zero YM identity fields or widen non-YM serialized
  arrays. No general migration framework or LibFuturo exact-build-envelope
  migration is promised.
- **D-08:** Prove two-way non-YM state continuation and same-point byte identity
  using a genuine producer/consumer built at kickoff `fef5691`, covering both
  regions and meaningful RAM/expansion/battery topologies proportionately.
  Keep legacy identity vectors. Update global-version consumers separately
  from legacy-envelope expectations: runner revision metadata changes and
  existing revision-6 movie warnings are expected version metadata. No old
  PCM, transition, frame or native-state digest is casually rebaselined.
- **D-09:** Capture and semantically validate every YM register, operator,
  envelope, noise/LFO, timer/busy, clock, output and audio-carry field required
  for exact continuation. Derived caches may be rebuilt only with a proven
  deterministic reconstruction. Complete validation precedes mutation; preserve
  machine/battery/output-buffer ownership on refusals and all existing reset,
  isolation, no-post-load-allocation and bounded-call contracts.

### Evidence, bounds and delivery

- **D-10:** Pick a documented deterministic integer YM-to-mono gain/rounding
  rule only after deriving its complete numeric bounds. Conditional gain for
  installed YM may preserve old no-YM values. Recompute mixed/delta, impulse,
  integrator, DC-blocker, clipping and visitor bounds plus pending/carry
  capacity; do not assume spare headroom or widen unrelated global limits.
  Physical analog gain/listening remains unmeasured and outside acceptance.
- **D-11:** Own a lawful deterministic generated diagnostic/register workload
  whose old source refuses the YM request and whose new execution produces
  independently checked non-silent PCM. Cover manual-specified status/timer/IRQ
  numbers, complex synthesis parity to the exact seed, all channel algorithms,
  noise/LFO/feedback and representative legal/hostile traffic proportionately.
  Numeric expectations precede hash equality; equal ports are not hardware truth.
- **D-12:** Exercise the same workload through actual public C, runner and
  loaded-libretro paths, then actual portable Wasm and clean extracted/installed
  consumers. Preserve callback/batch/fallback, buffer-tail, invalid-request,
  package identity/notice/integrity and representative pass/fault controls.
  Extend existing fixtures/inventories and current CMake/sh mechanisms rather
  than creating a second scheduler, host or generic evidence framework.
- **D-13:** Measure ordinary and worst-legal native/Wasm YM workloads, state and
  instance extents, integer headroom and structural event/carry bounds. Include
  existing CPU/interrupt overshoot in call-work bounds. Source-derived event
  ceilings and C-line estimates are not performance measurements. Establish a
  real consumer/objective and comparable measurements before a new numerical
  resource limit; preserve existing unrelated bounds and required controls.
- **D-14:** Required final local gates include the existing native CI workflow
  and hygiene, meaningful focused/unit/integration/refusal checks, genuine
  exact-seed parity, old/new state evidence, emitted portable artifact and
  package consumers. Run background by default and avoid duplicate full runs
  after prose-only changes. Diagnose failures before bounded retries. Use
  existing qualified FIFO helpers where useful; Phase 19's shell-heavy hygiene
  needed an outside-sandbox execution boundary to meet its unchanged deadline.
- **D-15:** Reuse current source/package identities, notices, CI topology,
  release integrity and required/advisory membership. Document the supported
  request, gain and clock/IRQ fit and limits in existing user/developer docs.
  Make no retail-game compatibility, physical audibility/calibration, broad
  browser/device or handheld power claim from owned diagnostics/seed parity.
- **D-16:** Continue all runnable plans and required local review/fixes and
  verification under autonomous authorization, within at most two review-fix
  rounds and one gap round. No new owner-only check is selected. Preserve
  original Phase 12/14 denials, spent host/remote attempts, expired windows and
  nine v3.0 gaps. Publication, merge, push, new remote dispatch and materially
  new host/identity actions remain separate boundaries. Never use forbidden
  entrypoints, arguments containing `--auto`/`--chain`, or hygiene bypasses.

### Agent's Discretion

Choose private C module seams, owned fixture implementation, exact parity
corpus, integer representation/rounding and measured digital attenuation within
these contracts. Use current owners and the smallest sufficient recurring checks.
Qualify unresolved implementation arithmetic and concrete acceptance early;
research already-resolved profile/compatibility questions only if new evidence
can overturn the choice. A demonstrated need to break old state/API/outputs or
expand hardware/authority is a real decision boundary, not routine discretion.
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|---|---|---|
| YM-01 | A host can load one explicitly documented YM2151 profile within selected/autodetected XM topology; map, enable, clock, busy/status, timers and established IRQ routing follow a source-qualified digital model. Unsupported topologies/devices refuse before allocation or live-machine mutation. | See the source-qualified profile, state policy/review, integration engineering note and validation design below. |
| YM-02 | A homebrew register workload produces independently checked non-silent FM output from an integer, instance-owned, bounded C17 specialization of the approved BSD-3 OPM seed, with exact-pin synthesis parity separately identified from hardware accuracy and all notices/update duties retained. | See the source-qualified profile, state policy/review, integration engineering note and validation design below. |
| YM-03 | A host receives deterministic bounded mono PCM and canonical audio observations from the qualified YM workload at supported rates/regions, including partial-frame carry and maximum legal mix; TIA/POKEY-only outputs and existing duplicated-mono libretro output remain unchanged. | See the source-qualified profile, state policy/review, integration engineering note and validation design below. |
| YM-04 | A host can reset, unload/reload and run isolated YM instances without stale state, post-load allocation, unbounded work or writes outside borrowed output buffers. | See the source-qualified profile, state policy/review, integration engineering note and validation design below. |
| YM-05 | A host can save/restore all YM state mid-stream and obtain identical subsequent audio; corrupt, unsupported or semantically invalid state preserves the live machine and committed battery bytes. Concrete API/behavior/native-format compatibility policy is documented before implementation. | See the source-qualified profile, state policy/review, integration engineering note and validation design below. |
| YM-06 | The same owned YM diagnostic executes through the actual public C library, headless runner and loaded libretro core, with independent numeric expectations before cross-route hashes and retained callback/batch/fallback/refusal controls. | See the source-qualified profile, state policy/review, integration engineering note and validation design below. |
| YM-07 | The portable libretro artifact and clean extracted/installed package consumers reproduce the owned YM workload's native observations, with package identity, notices and existing integrity/fault controls preserved. | See the source-qualified profile, state policy/review, integration engineering note and validation design below. |
| YM-08 | Integrators can identify the supported request, exact profile and evidence limits in existing docs, and maintainers have representative ordinary/worst-legal YM native/Wasm cost, integer headroom, memory/state extent and justified finite bounds using current mechanisms. | See the source-qualified profile, state policy/review, integration engineering note and validation design below. |
</phase_requirements>

## Standard Stack and Reuse

C17, fixed-width integers, existing CMake/CTest and POSIX sh, current instance and
cartridge allocation model. Approved seed: ymfm OPM at full pin
81aec25ccbb98f4873a255f7551ac4dadac59b4a, minimal LICENSE, ymfm_opm.cpp/h,
ymfm_fm.ipp/h and ymfm.h only. Production remains C; exact-seed C++ is a
separately labeled parity oracle, not a production dependency. Retain BSD-3
notices, exact provenance and update duty. No unrelated emulator source, ROM,
firmware, scheduler, test framework or new package is justified.

The seed and source qualification are documented in
[profile evidence](../../research/ym2151-profile/EVIDENCE.md) and
[integration engineering](../../research/ym2151-integration/ENGINEERING.md).
Only those listed seed files are approved. No local seed directory is assumed
available; if fetching is needed, use ignored build/v6-ym-seed/ and qualify the
exact pin/license/identity before use. Research did not compile the seed.

## Architecture and Current Ownership

The engineering note identifies actual current source locations and widths:
instance/cart own eligibility, gates and port access; bus owns due events and
IRQ sampling; synth owns integer mono mix and canonical observation; state
owns one topology-aware transactional visitor. Runner/libretro/build/package
consumers already exist. Extend those owners; avoid forwarding abstractions or
parallel state/migration engines.

The seed has eight channels and thirty-two operators, normal generation every
64 YM clocks, i.e. 128 MARIA ticks in this profile. Current timing/carry proof
includes a 20,730-tick CPU/interrupt overshoot window: illustrative full-window
ceilings are 1,095 NTSC and 1,273 PAL generated output steps, not runtime
measurements. Confirm these structural bounds against the integrated call
schedule, deadline alignment and legal write traffic. Current observer raw
caches/carry records use uint8 levels even though the callback/raw hash field is
uint16; widening the callback alone does not preserve YM signed sample bits.
See the engineering note for source references and operation-count qualifiers.

## Validation Design

Separate three evidence classes: independent Yamaha chip numbers, faithful C
translation to exact-seed output, and actual deliverable/package execution.
None establishes physical XM clock/wiring/gain or retail-game compatibility.

Independent chip timer periods are 64*(1024-A) and 1024*(256-B) YM clocks; use
minimum and longer periods plus flags/enable/reset and exact edge ordering.
Independently qualify busy timing against the manual before treating the
seed's 32*prescale clocks as a hardware expectation. Model the v4-declared IRQ
route at the existing bus deadline, gated only by valid accepted bit 2. Test
D7 gating and coexistence with current XM controls without new alias behavior.

An owned generated diagnostic must actually write through CPU bus ports, emit
non-silent numeric PCM and run via public C, runner, loaded-libretro and portable
Node/package consumers. Use current callback/batch/fallback and source-identity
controls. Exact-seed synthesis parity should cover all algorithms, envelopes,
feedback, LFO/noise, both channel outputs, legal writes and reset/continuation
proportionately; source-derived hashes alone cannot substitute for independent
numeric checks. Keep no-YM golden vectors and legacy identity bytes unchanged.

The [state policy](../../research/ym2151-state/POLICY.md) and its
[adversarial review](../../research/ym2151-state/REVIEW.md) identify concrete
version consumers and failure boundaries. Build a genuine old producer/consumer
from kickoff fef5691; verify same-point non-YM bytes and continuation in BOTH
directions, rather than producing a legacy fixture with new code. Sample both
regions and meaningful RAM/expansion/battery topologies. New YM state needs full
mid-stream continuation, semantic-invalid/corrupt/topology/identity refusal,
live-machine and battery no-mutation, stable spans and observer-free save/load.
Unknown future versions and old-core rejection must be safe; do not assume a
specific status when existing length checks precede revision checks.

Exercise partial-frame carry, reset/power cycle/unload/reload, isolation,
post-load allocation, output tails, jammed/hostile legal traffic and maximum
mix. Runtime benchmarks must measure actual ordinary and worst-legal workloads
on available native and Wasm consumers, identify source/toolchain/workload and
separate observed cost from structural bounds or untested lower-power targets.

## Main Implementation Risks

- Signed right shifts and signed left shifts in the C++ seed cannot be blindly
  translated to C17. Guard zero/width shifts, widen bounded products and define
  rounding/narrowing while preserving DAC quantization parity.
- Existing mixer and visitor assume nonnegative source levels. Choose a stated
  digital coefficient/representation and prove mixed/delta/kernel/integrator/DC
  and clipping bounds before adoption. Preserve all non-YM gain and state bytes.
- Existing ten-source same-tick/pending/carry proof needs a new YM-qualified
  bound, including possible coincident TIA/POKEY/CPU/YM transitions and overshoot.
- Preserve format-1 identity: do not append even a zero YM tag for non-YM, nor
  unconditionally serialize enlarged source arrays or new device fields.
- Global revision 7/format 2 metadata differs from non-YM envelope 6/1. Update
  current reporting assertions without rebaselining old behavior/identity vectors.
- Source equality, exact-seed equality, actual package bytes and native/Wasm
  consumer equality are distinct gates; producer-workspace success does not
  prove clean-extracted package transport or all other platforms.

## Open Implementation Questions and Discretion

Digital gain/rounding and final numeric/state/instance extent are not yet measured
and must be qualified during implementation under the context constraints.
Choose the smallest private C module/API and representative acceptance corpus
that supplies the promised complete feature. No owner scope choice remains for
the selected profile or additive compatibility policy; return only if new
facts require breaking those contracts, extra hardware/authority, or a materially
new resource/risk consequence. Do not invent resource ceilings from estimates.

## Verification Mechanisms and Scope

Reuse cmake --workflow --preset ci, qualified local FIFO scheduling if needed,
existing hygiene, current pinned scripts/emscripten.sh route and package/install
consumers. Extend exact configure-time inventories and quality fixture identities
where tests or fixtures change; run affected reduced configurations locally
before expensive acceptance. Preserve required pass/fault coverage and controls.
Phase 19 established the shell-heavy hygiene execution boundary: sandboxed tree
test timed out at its unchanged 60 seconds, the identical outside-sandbox case
passed in 37.52 seconds, and complete outside-sandbox hygiene passed. Do not
repeat the same failing environment or widen test deadlines without cause.

No build/test/host or performance experiment was run by these researchers.
This report is source evidence for planning. Original denied/spent Phase 12/14
scopes, nine v3 gaps and separate publication/merge/remote authority remain intact.

## Sources

- [Selected phase context](20-CONTEXT.md), current requirements, INTENT and fan-out brief.
- [Exact profile and primary source links](../../research/ym2151-profile/EVIDENCE.md).
- [Compatibility source review](../../research/ym2151-state/REVIEW.md).
- [State policy recommendation](../../research/ym2151-state/POLICY.md).
- [Current-owner engineering and pinned seed analysis](../../research/ym2151-integration/ENGINEERING.md).
- PROVENANCE.md, preparation/ARCHITECTURE.md, A7800-AUDIO.md, ENGINEERING.md section 11,
  relevant LESSONS.md and current source/test consumers identified in those notes.
