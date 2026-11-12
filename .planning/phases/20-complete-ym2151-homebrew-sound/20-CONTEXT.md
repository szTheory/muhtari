# Phase 20: Complete YM2151 homebrew sound - Context

**Gathered:** 2026-11-11
**Status:** Ready for planning

<domain>
## Phase Boundary

Deliver one declared XM/YM2151 sound profile as a complete vertical feature:
chip/bus/audio/state, public library, runner, loaded libretro, portable Wasm and
clean package consumers, required tests, documentation and measured operating
cost. Cover YM-01–08 in this phase. Phase 19 has terminal passed verification and
clean review; its final SDK bookkeeping is independent of this context.

The owner selected the saved v6.0 proposal and explicitly authorized autonomous
local follow-through. Relevant source qualification and an adversarial state
policy review are complete. Ordinary in-scope recommendations below are adopted
under that authority, without a routine acceptance prompt. Evidence remains
source-specific; these decisions do not claim an implemented feature.
</domain>

<decisions>
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
</decisions>

<code_context>
## Existing Code Insights

### Reusable Assets

- Existing XM cartridge selector, control/page latches and transactional load.
- Current bus event catch-up/deadline and three-stage IRQ sampling owner.
- Integer synthesis, observer coalescing/carry, mono PCM and runner hash stream.
- One topology-aware little-endian state visitor, identity and no-mutation tests.
- Public/runner/loaded-libretro diagnostics, native inventories, installed and
  extracted package checks, pinned portable producer and offline Node consumers.

### Established Patterns

Per-instance mutable state, immutable tables, no post-load allocation, existing
CMake/sh checks and bounded diagnostic workloads. Test independently derived
numbers before cross-route hashes. Keep canonical source-specific receipts and
failed history; raw logs/ROM/save bytes stay out of commits.

### Integration Points

`src/instance.c`, `cart.c`, `bus.c`, `synth.c`, `internal.h`, `state.c`; existing
runner, libretro, diagnostics/tests, CMake inventories, packaging/notices and
docs. The source-grounded engineering note names actual current widths, loops,
operation ceilings and C17 hazards, rather than relying on legacy design prose.
</code_context>

<specifics>
## Specific Ideas

Use the saved milestone synthesis/proposal, now-selected requirements and these
completed independent findings:

- [Profile/source qualification](../../research/ym2151-profile/EVIDENCE.md).
- [State policy](../../research/ym2151-state/POLICY.md) and
  [adversarial source review](../../research/ym2151-state/REVIEW.md).
- [Integration engineering](../../research/ym2151-integration/ENGINEERING.md).

No further broad ecosystem survey is needed. These are source findings and
recommendations; implementation, parity, timing and runtime evidence remain to
be produced in this phase. Follow the fan-out brief and relevant project lessons.
</specifics>

<deferred>
## Deferred Ideas

Full XM services/firmware, other YM topologies, Souper/BupChip, Covox/ADPCM,
Atari 2600, public stereo, generalized cross-build migration/stable shared ABI,
browser/player UI, LibFuturo adapter changes, Playstead, real-device/physical
listening or calibrated analog proof, and unrelated CI restructuring. No older
host/remote scope is reopened.
</deferred>
