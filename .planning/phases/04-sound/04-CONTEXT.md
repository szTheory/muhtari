# Phase 4: Sound - Context

**Gathered:** 2026-10-20
**Status:** Ready for planning

<domain>
## Phase Boundary

Games produce TIA and cartridge POKEY sound through an integer band-limited synthesiser. The library supplies
mono PCM, the runner supplies transition and PCM hashes, and libretro supplies duplicated stereo. SND-01 through
SND-05 and Phase 4's five roadmap criteria remain the scope. Merging publishes through the existing pipeline.

PAL, other cartridge audio, XM control, save states, recording, public debugging hooks and audio-device code remain
later work. This session captures decisions only; research and planning are the next GSD step.
</domain>

<decisions>
## Implementation Decisions

The owner requested consideration of all relevant areas, delegated fan-out research, an adversarial pass and
automatic adoption of a coherent recommendation set. Three read-only reviews covered chips/bus, synthesis/mix,
and hosts/evidence. Decisions below reconcile their findings with settled preparation choices. Recommendations
are adopted under that delegation, not presented as answers to an interview that did not occur.

### TIA and POKEY behaviour

- **D-01:** Carry forward DEC.17, 22, 28–30 and 33. Own documented integer C chip models; no new runtime
  dependency. TIA uses its independent 456-tick lattice, A at 0/144 and B at 64/288, never MARIA's line or evenly
  spaced substitutes. Every decoded TIA write reaches it regardless of INPTCTRL. Preserve AUDF wrap behaviour,
  all AUDC patterns, and immediate AUDV level changes described in A7800-AUDIO §2.
- **D-02:** One mutable TIA horizontal epoch drives audio, RSYNC and WSYNC. The existing `tia_phase` seeds it;
  RSYNC must move both schedules together without retroactive events. Keep the existing no-RSYNC WSYNC timing
  and its tests. Register writes take effect at the bus's data tick: a slow write's start+5, rather than its
  cycle completion at start+6. Clarify preparation's ambiguous “cycle end” prose when implementing.
- **D-03:** Deterministic startup: zero TIA audio logic, existing initial phase, POKEY running as SKCTL=$03.
  Preserve an internal alternate-init profile for tests. These are compatibility defaults, not claims that
  physical unreset chips power up deterministically. Advance hardware over the existing boot timeline; do not
  invent BIOS residue or reset lattice phase at cartridge hand-off. A power cycle resets audio; the console's
  Reset input remains a switch read by software.
- **D-04:** POKEY counts actual bus phi2, including HALT/RDY holds and stretched slow cycles. Catch up before
  changing a tick-to-phi2 segment, mapped accesses, interrupt decisions and output flushes. Aggregate cycle
  counts cannot reconstruct transitions across lost slow-cycle segments. Implement documented joins,
  reload/STIMER pipelines, two-tone resynchronisation, high-pass delay, staggered polynomials and RANDOM/init
  semantics. Silent output is not permission to stop hidden counters or RANDOM.
- **D-05:** Model existing header POKEY codes with at most two fixed chip instances: $4000–$7FFF mirrored,
  $0450–$045F, $0800–$0FFF, and $0440–$044F including $440+$450. Both chips mix to mono. Reuse one chip model;
  no broader board or expansion work. Decode through the cartridge path with explicit read/write priority;
  preserve ROM reads on documented write-only overlaps and existing conflict normalization. CPU accesses and
  MARIA fetches must not accidentally share side effects.
- **D-06:** Fulfil the settled v4 IRQ wiring policy, including CPU acceptance. The existing CPU implements NMI,
  but no maskable IRQ path; a POKEY line field alone is insufficient. Add timer IRQST/IRQEN semantics and
  documented NMOS IRQ sampling/entry, retaining NMI priority and existing vectors. Use validated effective v4
  wiring, not raw bits from an invalid v4 block. Wire the @440 identity to its own bit even when it occupies
  instance slot zero. Test masking, acknowledgment, persistent levels, held cycles and NMI interaction.
- **D-07:** Hardware uncertainty remains confidence-qualified: TIA pattern disagreements, POKEY STIMER polarity,
  startup state and physical mix are recorded with sources. Use A7800-AUDIO's documented functional model and
  Altirra manual vectors as the provisional model; emulator agreement is not hardware proof. Real-console and
  Verilated referee work stays in its later milestone, and does not become an owner UAT task here.

### Synthesis, mix and runtime bounds

- **D-08:** Seed the approved sibling's 16-tap/32-phase Q15 kernel, interpolation and integer ring integration;
  retain a single approximately 20 Hz integer DC blocker and omit NES-specific filters. Use the provenance-
  approved seed pin `9f9386c661f6`; reconcile the audio preparation's different `34d68516a15e` reference before
  copying. Copy only needed sections and retain attribution as PROVENANCE.md requires. The sibling's spectral
  result is a hypothesis until measured through this production pipeline.
- **2026-10-20 evidence amendment to D-08:** The literal Q15 coefficient precision is superseded by
  signed Q23 rows for the same 16-tap/32-phase compact-support kernel, corrected interpolation and integer
  ring architecture. The approved seed pin remains `9f9386c661f6`; attribution and the single approximately
  20 Hz integer blocker remain required. Regenerate the 512 coefficients as int32 values (2 KiB), normalize
  each corrected row to 8,388,608, and convert integrated Q23 to Q30 by multiplying 128. Use synthesis identity
  `exact-q23-16x32-dc20`. This narrow change is supported by `04-RESEARCH.md`, “Feasibility resolution:
  explicit minimal D-08 revision”: the unchanged Q15 seed failed 9 of 132 hardware-scaled cases; Q23 passed
  all 132 and every deliberately aliased control was rejected. Worst measured equivalent-schedule results
  were −82.10380 dBc for TIA and −94.71708 dBFS for POKEY, with bound-derived M = 18,361 and
  L1 = 13,341,774 / 8,388,608. Coefficients were identical under Simpson 128/256 refinement. Gain is still
  derived from hardware currents and full-path headroom, never lowered to pass a spectral test. This records
  new evidence rather than changing the historical D-08 text above; reproduction through the production
  tables, actual chip models, frame carry and platform lanes remains mandatory. No tap/phase/support count,
  rate, profile, filter or dependency is added.
- **D-09:** Keep the released public contract: 48,000/1 Hz, mono int16, exact NTSC phase ratio 88/13,125.
  No sample-rate configuration or public synthesis profile is added. At this rate one sample interval spans
  at most 150 master-tick positions; merge mixed-level changes at a tick before kernel work. Bound catch-up,
  event carry, ring storage and integer intermediates explicitly; allocate nothing after load.
- **D-10:** Use owned integer TIA nonlinear and POKEY saturation tables generated from A7800-AUDIO §3's formulas,
  node-current weights and default cartridge gain. Simultaneous raw source changes are mixed together. Actual
  cartridge loading and coupling corners remain unmeasured; add no speculative board or TV filters.
- **D-11:** Correct the preparation's clipping argument: a maximum DC sum and unity kernel-row sum cannot prove
  no clipping when signed lobes ring. Derive scale from a conservative bound on the complete reconstruction
  and DC-blocker path, including rounding, to leave at least 1 dB headroom. Prove intermediate-width safety
  separately. Test maximum hardware combinations, adverse transitions and real built samples. A saturation
  clamp or flat-DC test does not establish zero clipping. Update the affected preparation claim in the same
  implementation change; no need to reopen the analog model.
- **D-12:** Start with exact synthesis only. Measure the hostile high-rate POKEY case and preserve bounded
  per-output-sample work. The documented box-bin performance trigger remains conditional, not an instruction
  to add another profile now. Never switch quality automatically with workload. Any measured fallback must
  preserve raw transition hashes and independently meet SND-05; otherwise record the finding for planning.

### Observation, hashes and frame boundaries

- **D-13:** Use a narrow internal per-instance transition tap for the runner, which already links private core
  inspection. Keep SHA-256 in the runner and public API version 3. NULL disables observation; avoid hashing,
  serialization and event storage for observation when disabled. General hooks and public registration
  contracts stay deferred. The tap must not affect machine state, PCM or frame hashes; callbacks do not reenter
  the core. Register observation before running, and keep its host context separate from power-cycle state.
- **D-14:** Canonical transition records are exactly 11 bytes: LE u64 absolute master tick, u8 source,
  LE u16 raw channel DAC level. No native structs, padding, mixer outputs or unchanged-level notifications.
  Source IDs are fixed: 0/1 TIA channels, 2–5 primary POKEY channels, 6–9 @440 secondary POKEY channels; a lone
  @440 uses its @440 IDs. Other single-chip locations use primary IDs. Serialize equal-tick events in source
  order, retaining meaningful within-source order; do not collapse raw transitions just because synthesis
  merges the tick. Known initial zero levels produce no synthetic change record; later changes use the same
  absolute boot timeline as the bus. Document the source assignment and format before pinning.
- **D-15:** Hash PCM as successive mono signed-int16 representations serialized to two LE bytes each. Hash only
  actual output samples, never capacity or stereo duplication. Keep transition and PCM digests separate.
  Rate, behaviour revision and synthesis identity accompany evidence; changing the mixer does not redefine
  the raw transition stream.
- **D-16:** `--hash-audio` produces two distinctly labelled SHA-256 values over the whole requested run,
  with completed-frame/sample counts and rate. Successful signal stop includes only completed frames. A failed
  run produces no success digest. Keep stdout's existing key/value style and exit-status table. Exact labels
  are the planner's choice and must be documented/tested. No per-frame query syntax, WAV export or new hashing
  package is needed.
- **D-17:** Audio belongs to logical half-open frame intervals [frame_start, frame_deadline), including the
  existing partial first frames. The CPU currently finishes an instruction beyond that deadline. Keep future
  transitions out of the current frame and its hash; use bounded carry to retain them, kernel tails, blocker
  state and fractional phase. Do not solve audio by changing CPU/frame semantics or dropping overshoot.
  Preserve the fast-boot first frame (8,190 ticks, 54 samples), BIOS first interval, and full 800/801 cadence.
- **D-18:** NULL audio skips writes only: chip clocks, synthesis history, count and future PCM advance identically.
  Validate buffer errors before running or invoking observation. Power cycle clears chip/ring/blocker/carry/
  fraction state. No-content output remains the existing silent test card. BIOS refusal is an existing sticky,
  post-execution error; emit no failed-frame PCM/hash result and document that existing exception instead of
  promising rollback of execution. Test ordinary invalid calls as atomic and BIOS refusal separately.

### Evidence and published pins

- **D-19:** Chip unit tests assert documented values, not just self-generated golden hashes. Cover A7800-AUDIO
  §9's lattice, all AUDC settings, AUDF changes, timers, polys, RANDOM, init, slow cycles and level writes;
  include RSYNC, IRQ and address-overlap cases. Use existing plain-C/CTest and fixture licensing conventions.
- **D-20:** Define SND-05's measurement before implementation: largest unintended spectral peak strictly below
  16 kHz, below -80 dBc against an audible intended fundamental; for ultrasonic-only cases with no in-band
  fundamental, below -80 dBFS against an explicitly defined full-scale reference. Pin settling, capture length,
  window, normalization and intended-harmonic masks. Exclude only intended physical harmonics and justified
  leakage lobes; a folded alias never becomes an exempt harmonic. Exercise actual TIA lattice tones and
  POKEY AUDF 0/1/3 through the production kernel/blocker, rather than only FFT-bin-rounded synthetic events.
  A deliberately aliased control must fail. Test-only floating spectral analysis may live outside the integer
  core; reuse the minimal permitted sibling harness without adding an analysis package to runtime or CI.
- **D-21:** Add non-silent sound/POKEY sample transition and PCM pins to existing cross-platform hash comparison,
  including be32 and applicable WASM output. Test observer-on/off equality, partitioned versus continuous
  rendering, discarded frames then audible output, reset and no-content silence. Pins establish regression
  stability, while chip vectors and spectral tests establish their meaning.
- **D-22:** Prove libretro batch preference and single-sample fallback sample-for-sample against core/runner
  mono, with equal L/R, actual counts, partial first frame, missing callbacks and reset/reload. Extend existing
  `runner.boot_to_play` with non-silent audio and a muted negative control. Keep hosted RetroArch within its
  established case and size budget; do not add recording/device automation. The agent handles any one-off
  RetroArch judgment; no owner listening test is required.
- **D-23:** Fold the pending POKEY re-pin todo. In the commit that maps $0450, bump the behaviour revision and
  rewrite the five POKEY samples' frame chains plus four affected `title@120` pins, regenerate compatibility
  documentation and record old/new values and causes in that plan's SUMMARY. Changes in samples without POKEY
  require investigation, not blanket re-pinning. Preserve ABI and API; replace stale numeric behaviour-version
  comments with a reference to the reported value. Reuse one intentional revision migration for this phase's
  released behaviour changes, with explicit reasons and established pin-writing rules.
- **D-24:** Every behaviour change includes a test in the existing CI workflow. Keep proportional gates, no
  self-skips, and existing review-round limits. Evidence is a command, exit status and hosted run URL where
  available; no committed execution logs. Ship-time merge/release checks remain the ship step's job.

### Agent's Discretion

Internal names, module splits, bounded carry representation, exact tie-order machinery, spectral harness details,
and CLI labels may be chosen during research/planning within these contracts. Research must prove spectral and
headroom claims; this discussion does not certify them. No speculative public API or dependency is authorized
merely by an illustrative architecture signature.

### Folded Todos

- `.planning/todos/pending/2026-10-19-re-pin-the-pokey-samples-when-pokey-lands.md`: directly triggered by mapping
  $0450; its migration is D-23. Keep the todo until execution produces evidence.
</decisions>

<canonical_refs>
## Canonical References

Downstream agents must read the relevant sections before planning or implementation.

- `AGENTS.md`: licensing, scope, verification, budgets, branch and single-step rules.
- `.planning/ROADMAP.md` Phase 4 and `.planning/REQUIREMENTS.md` SND-01–SND-05: scope and acceptance.
- `.planning/PROJECT.md`: product constraints and no separate low-accuracy core.
- `.planning/preparation/README.md` and `.planning/preparation/DECISIONS.md`: reference index and settled choices.
- `.planning/preparation/A7800-AUDIO.md` §§1–5, 8–10 and source table: chip models, mix, synthesis and uncertainty.
- `.planning/preparation/ARCHITECTURE.md` §§2–6, 8, 11–12: clocks, API, profiles, frame/hash boundaries and conflicts.
- `.planning/preparation/A7800-SYSTEM.md` §§1–5, 7–8: bus data ticks, holds and boot timing.
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` §§1–2: validated A78 mapping and board priority.
- `.planning/preparation/CONFORMANCE.md` §§1–7: permitted samples, diagnostics, ledger and oracle limits.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` §§1, 3–4: callbacks, hosted RetroArch budget and runner contract.
- `.planning/preparation/ENGINEERING.md` §§2–4, 8–11: tests, budgets, performance, portability and docs.
- `PROVENANCE.md` and `ASSET_POLICY.md`: approved pins, notices, generated tables and fixture restrictions.
- `.planning/phases/01-a-test-frame-in-retroarch/01-CONTEXT.md`: no-content/API and delivery contracts.
- `.planning/phases/02-the-cpu-matches-the-public-vectors/02-CONTEXT.md`: bus, CPU, probe and test contracts.
- `.planning/phases/03-games-play-in-retroarch/03-CONTEXT.md`: partial frames, private consumer policy, released pins.
- `.planning/todos/pending/2026-10-19-re-pin-the-pokey-samples-when-pokey-lands.md`: affected rows/migration.
- `docs/embedding.md`, `docs/runner.md`, `docs/libretro.md`, `docs/emulation-tests.md`, `docs/testing.md`: current
  user-visible behaviour, pin writing and automated checks.
- `docs/hardware/bus.md`, `docs/hardware/sally.md`, `docs/hardware/cartridge.md`: implemented hardware contracts.

External primary references consulted or identified for targeted implementation research:

- [Altirra Hardware Reference Manual](https://www.virtualdub.org/downloads/Altirra%20Hardware%20Reference%20Manual.pdf),
  chapter 5 and appendix E; documentation only, never Altirra source. Use A7800-AUDIO's pinned edition.
- [A78 header specification](https://7800.8bitdev.org/index.php/A78_Header_Specification), maintained format
  specification; reconcile with the pinned revision before changing the parser.
- [Windowed-sinc interpolation](https://www.dsprelated.com/freebooks/pasp/Windowed_Sinc_Interpolation.html),
  [DC blocker](https://www.dsprelated.com/freebooks/filters/DC_Blocker.html), and
  [spectrum-analysis windows](https://www.dsprelated.com/freebooks/sasp/spectrum_analysis_windows.html):
  author's DSP explanations for reconstruction, gain and spectral leakage; no code copying implied.
- Approved nesturbator seed at `9f9386c661f6`, as indexed by `PROVENANCE.md`; another console supplies synthesis
  machinery, never 7800 clock constants or unverified quality claims.
</canonical_refs>

<code_context>
## Existing Code Insights

- `src/tia.c`: value-less write entry point currently handles WSYNC only; extend internally for audio and RSYNC.
- `src/bus.c`: authoritative data ticks and phi2/held-cycle counting; POKEY catch-up and IRQ integration belong
  here, preserving existing slow/fast conventions.
- `src/cpu.c`: NMI exists, maskable IRQ acceptance does not yet exist.
- `src/cart.c`, `src/cartdb.c`: sound/header identification is already parsed; runtime decode must consume it.
- `src/frame.c`: returns zero-filled PCM, carries rational remainder, permits instruction overshoot and partial
  first frames. Replace silence without changing its validated external timing.
- `src/internal.h`, `src/instance.c`, `src/boot.c`: instance-owned storage, power cycle and behaviour revision.
- `include/muhtari.h`: released API 3 already returns fixed-rate mono PCM; no new consumer needs public options.
- `runner/main.c`, `runner/sha256.c`: existing private inspection and streaming SHA-256 can serve audio hashes.
- `libretro/libretro.c`: existing mono-to-stereo, batch preference and sample fallback; extend parity tests.
- `tests/built/expected.txt`, `tests/compat/ledger.csv`, `tests/compat/pin.cmake`: sample pins and migration path.
- `tests/core/`, `tests/libretro/`, `tests/runner/`, `tests/abi/`, `tests/inventory/`, `.github/workflows/ci.yml`:
  reuse actual-object tests, exact inventory, no-float/global gates and platform hash comparison.
</code_context>

<specifics>
## Specific Ideas

The owner's fan-out brief prioritises relevant stakeholder perspectives, primary evidence, adversarial checks,
coherent defaults and smaller, flatter dependencies: another copy and paste beats another dependency. Apply it
to 7800 audio, host embedding, portability, licensing and test validity. Unrelated UI and distributed-system
design do not add useful decisions here. Three independent review passes converged; remaining uncertainties
require implementation measurements, rather than additional speculative fan-out.
</specifics>

<deferred>
## Deferred Ideas

PAL, XM/YM2151/COVOX and other audio devices, physical mix calibration, hardware/RTL referees, public hooks,
arbitrary rates, WAV export, save states and broad diagnostics retain their existing milestone placement.

### Reviewed Todos (not folded)

- Release-rehearsal filtering, wasm32 promotion, arming the performance gate, Dependabot/publish review seeds,
  and nightly/cartdb review seeds stay in their existing separate work; keyword matches do not make them sound
  requirements. The prior phase already folded its CPU/run-loop review todo; it is not a new Phase 4 gap.
</deferred>

---
*Phase: 04-sound*
*Context gathered: 2026-10-20*
