# Phase 04: Sound - Research

**Researched:** 2026-10-20
**Domain:** targeted recovery of actual-clock POKEY synthesis
**Confidence:** HIGH for the bounded local candidate measurements; production integration remains unverified.

<user_constraints>
## User Constraints (from CONTEXT.md)

The following is copied verbatim from the current context. It records adopted constraints, including historical statements that the dated amendments supersede; those statements are not fresh certification.

DATA_W6p9R3mK_START
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
- **2026-10-20 actual-chip amendment to D-08:** The six retained wholly slow AUDF1 failures in
  `04-06-SUMMARY.md` supersede the earlier feasibility conclusion for the fixed 32-phase table. Fresh discussion
  reproduction of `ctest --test-dir build/ci -R '^audio.spectral$' --output-on-failure --no-tests=error`
  exits 8, with worst peak -78.28741 dBFS. Reopen phase-table resolution first. Research 64 then 128 phases,
  keeping 16 taps, Q23, compact support, Kaiser-beta8 shape, current weights, gain, blocker and judge fixed
  during comparison. Those tables occupy 4/8 KiB rather than 2 KiB; the 16-cell ring and coefficient applications
  per merged event can remain unchanged. Select the smallest candidate meeting all acceptance evidence;
  neither candidate is certified by this discussion. Historical Q23/128 success on the old 132-case matrix
  is only a lead. The prior prohibition on changing phase count is superseded by this evidence amendment.
  Preserve the approved seed provenance and integer event/ring architecture; introduce no dependency.
  If both candidates fail, diagnose coefficient/interpolation rounding separately before considering longer
  support or a different window. Wider arithmetic, support or engine changes need measured cause and an
  explicit inventory of invalidated proofs, not an open-ended redesign. The agent may make these technical
  choices under the owner's delegation; no additional owner approval or listening test is required.
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

### Recovery from the actual slow-clock failure

- **D-25:** Preserve all eight partially implemented files identified in `04-06-SUMMARY.md`. Keep completed
  plans 04-01 through 04-05 as history; revalidate their affected synthesis evidence rather than repeating their
  implementation. The next planning step must explicitly amend synthesis work and resume **04-06 Task 2**.
  Its SUMMARY has `status: blocked`; file existence and its completed date do not establish completion.
  Plans 04-07 onward stay blocked until Task 2 is verified and its partial summary is reconciled. Do not
  discard the six failures, revert documented bus timing, or move unmet SND-05 acceptance to a later phase.
- **D-26:** Compare candidates at the existing M=18,361 and unchanged hardware current model/Q12 gain4096.
  A lower output level cannot count as spectral improvement. Recompute the full signed step/blocker bound,
  rounding allowance and intermediate widths from final coefficients over all 13,125 phases and 88 output
  alignments. The existing bound has little slack; do not reuse its L1 or M numerically. If a new bound needs
  a different M solely for headroom, document that derivation separately and require spectral success both
  at the common comparison gain and final production gain. Preserve at least 1 dB headroom, zero clipping,
  the 85:342 current ratio and physical-only masks. A clamp is not a proof.
- **D-27:** Preserve the 132-case production matrix, all 132 aliased controls, all 72 actual-chip cases and
  independent arithmetic-schedule PCM comparisons. Before freezing a candidate, perform bounded offline
  exploration of legal mixed 4/6-tick clock patterns, independently selected origins and periodic high-pass
  narrow pulses (including the existing fast/slow/mixed delay conventions). Retain compact representative
  worst cases in the existing tests. Physical sidebands must come from the actual periodic raw schedule;
  an average clock frequency must not exempt aliases. No exhaustive permanent configuration matrix or new
  test framework is required. Stop expanding the search when the established requirements and these directly
  relevant checks pass; remaining uncertainty needs new evidence, not further speculative fan-out.
- **D-28:** Independently refine candidate worst spectral peaks at their actual frequencies before accepting
  a near-threshold result: an FFT-bin maximum can understate a windowed sinusoid's amplitude. Keep the
  existing capture, settling, window, full-scale definition and -80 dB threshold. This is one-off candidate
  evidence within D-20, not a new repository tooling gate or an invented stronger threshold. Report measured
  margin and the limits of the finite tested matrix. Ultrasonic int16 silence is valid; establish that raw
  DAC transitions reach synthesis so disconnected output cannot masquerade as rejection. A future permanent
  refinement of the analyzer is a tooling seed, not an additional phase requirement.
- **D-29:** Establish reproducible owned coefficient generation using the documented formula and convergence
  checks; do not depend on a transient scratch path. Keep it within existing generator/documentation patterns
  and avoid a new runtime or CI package. Update the synthesis identity when coefficients change. Recheck
  endpoint shifting, exact row sums, ring widths, work and carry bounds, adverse transitions, partition/NULL/
  reset behavior and eventual platform PCM parity. Phase-only changes retain support; support changes must
  revisit every hardcoded ring size/mask, latency/tail and occupancy claim. Existing performance checks apply;
  do not add an automatic quality mode or an unmeasured fallback.
- **D-30:** Preserve hardware event timestamps, bus/CPU semantics, raw source IDs and transition serialization,
  API/ABI, 48 kHz cadence and logical frame boundaries. Synthesis changes may change PCM, never justify a raw
  transition or video re-pin. Keep the already planned mapping/revision migration and host/platform work.
  `04-RESEARCH.md` conclusions claiming no unresolved feasibility prerequisite are stale for actual slow
  clocks; targeted research must reconcile them before planning resumes. No replacement has been measured
  or implemented in this discussion, and no SND requirement or plan count advances.

### Agent's Discretion

Internal names, module splits, bounded carry representation, exact tie-order machinery, spectral harness details,
and CLI labels may be chosen during research/planning within these contracts. Research must prove spectral and
headroom claims; this discussion does not certify them. No speculative public API or dependency is authorized
merely by an illustrative architecture signature.

### Folded Todos

- `.planning/todos/pending/2026-10-19-re-pin-the-pokey-samples-when-pokey-lands.md`: directly triggered by mapping
  $0450; its migration is D-23. Keep the todo until execution produces evidence.

## Deferred Ideas

PAL, XM/YM2151/COVOX and other audio devices, physical mix calibration, hardware/RTL referees, public hooks,
arbitrary rates, WAV export, save states and broad diagnostics retain their existing milestone placement.

### Reviewed Todos (not folded)

- Release-rehearsal filtering, wasm32 promotion, arming the performance gate, Dependabot/publish review seeds,
  and nightly/cartdb review seeds stay in their existing separate work; keyword matches do not make them sound
  requirements. The prior phase already folded its CPU/run-loop review todo; it is not a new Phase 4 gap.
DATA_W6p9R3mK_END
</user_constraints>

## Summary

Recommend the smallest passing phase-density candidate: **16 taps, 64 phases, Q23, unchanged compact sinc/Kaiser-beta8 support, 48 kHz, one existing blocker**, with proposed synthesis identity `exact-q23-16x64-dc20`. Fresh isolated builds link a regenerated synthesizer against the current production chip objects and exercise the existing 132 production schedules, 132 negative controls and 72 actual-chip cases. Both 64 and 128 phases pass these finite matrices after continuous-frequency peak refinement. The 64-phase candidate also passes 84 bounded mixed-clock/high-pass cases. This is candidate evidence, not certification of an edited production tree. [VERIFIED: candidate commands in Evidence; src/synth.c:1-240; tests/audio/test_spectral.c:1-508]

Compare at the unchanged **M=18,361** first. The independently re-enumerated signed reconstruction bound for 64 phases requires **M=18,360** for the conservative one-dB complete-path proof; this one-LSB adjustment is required by headroom, not used to obtain spectral acceptance. The candidate passes again at that final level, including negative controls and mixed-clock exploration. Keep the current weights and cartridge gain. Resume **04-06 Task 2**, preserve its eight dirty files and six failing vectors, and require final production verification before 04-07 onward. No owner action is required for these delegated technical choices. [VERIFIED: bound/candidate commands in Evidence; locked D-25–D-30 above]

The previous RESEARCH statements that production audio did not exist, Q23/32 removed all feasibility prerequisites, and fast/equivalent schedules established slow-clock feasibility are superseded. The Q15 failure and Q23/32 equivalent-schedule success remain historical observations; the retained actual slow-clock failure invalidates their extrapolation. Current production still fails six slow AUDF1 cases; this session reproduced that failure. No requirement or plan completion advances. [VERIFIED: ctest command in Evidence; 04-06-SUMMARY.md:75-108; src/synth.c:1-114]

## Architectural Responsibility Map

| Capability | Primary owner | Secondary owner | Rationale |
|---|---|---|---|
| Legal raw DAC timestamps and values | Existing TIA/POKEY/bus model | Documentary vectors | Synthesis recovery must preserve hardware events |
| Tick-phase reconstruction and gain | Integer synthesizer | Owned coefficient generator | Density changes an interpolation table, not the clocks |
| Spectral classification and offline refinement | Existing test-only analyzer | Bounded exploratory captures | No floating code enters the shipped core |
| Frame/carry/discard/reset contracts | Existing core/frame integration | Current contract tests | Revalidate affected output history without repeating implementation |
| Runner, hosts and platform pins | Existing later phase slices | Existing CI | Their work stays ordered after Task 2 |

These ownership assignments follow the adopted context and inspected module boundaries; they are planning recommendations, not new public interfaces. [VERIFIED: locked D-01–D-30; src/synth.c:1-240; tests/audio/test_spectral.c:1-508]

<phase_requirements>
## Phase Requirements

The descriptions below quote the current requirements; recovery changes none of them. [VERIFIED: .planning/REQUIREMENTS.md:100-110]

| ID | Description | Research support |
|---|---|---|
| SND-01 | `ctest -L tia` steps every AUDC setting through the period and bit pattern A7800-AUDIO tabulates, on the TIA's own 456-tick lattice, with TIA writes ungated by INPTCTRL. | Preserve completed TIA implementation and rerun affected synthesis tests |
| SND-02 | `ctest -L pokey` checks POKEY at $4000, $0450 and $0800 as the header or hash list places it: 16-bit joins, 1.79 MHz mode, two-tone with the high-pass filter, the polynomial counters, RANDOM, SKCTL initialisation, and its clock stretching on slow cycles. | Resume retained actual-clock work; downstream expansion/mapping remain required |
| SND-03 | `muhtari-run --hash-audio` prints the transition-stream and PCM hashes for the 7800basic sound and POKEY samples, identical on every platform, and no sample clips. | Recomputed widths/headroom; keep later observation/platform slices |
| SND-04 | The libretro batch and single-sample paths deliver stereo-duplicated PCM equal to the runner's, and `runner.boot_to_play` also requires non-silent audio. | Keep later host parity slices; recovery does not certify them |
| SND-05 | A test renders reference tones through the synthesiser and shows the largest non-harmonic peak below 16 kHz is under −80 dB. | Measured passing 64-phase route, fixed judge, final production acceptance still required |
</phase_requirements>

## Project Constraints (from AGENTS.md)

Apply all of these as locked constraints: permissive seeds with provenance only; never open GPL/LGPL/unlicensed emulator source; library/runner/libretro only; no device/window code; no unapproved ROM/BIOS/save bytes or personal identifiers; GitHub noreply commits and hygiene hooks; deterministic integer-only instance state and bounded calls; every behavior change gets an existing-CI test and documentation; short user README/header with behavior prose in docs; automation-first verification with owner questions only for identity-bound consent or own hardware; CMake/POSIX-sh policy gates under the code budget; no more than two review fix rounds and one evidence-backed gap round; one GSD step then stop without chaining/bypasses; branch before commits and follow ship-time PR/CI/merge/release checks; evidence is commands, exits and hosted URLs rather than committed logs; lessons remain hypotheses until reproduced; summaries need requirements-completed plus block coverage entries; small modules, fixed-width integers, prefixed names, cited hardware comments, no planning IDs in code/test/user docs, and the remove/reuse/standard-library/platform/minimum-code ladder. [VERIFIED: AGENTS.md:1-76]

This research owns only this document. It did not alter the eight retained partial implementation files or commit. The current partial files are quoted verbatim from the observed status: `docs/hardware/pokey.md`, `src/boot.c`, `src/bus.c`, `src/instance.c`, `src/internal.h`, `src/pokey.c`, `tests/audio/test_pokey.c`, `tests/audio/test_spectral.c`. [VERIFIED: git status --short, exit 0; opened files and 04-06-SUMMARY.md:1-43]

## Standard Stack

Use the existing C17/core/CTest architecture and test-only standard math library. This environment reports CMake 4.4.3, Apple clang 21.0.0 and Python 3.14.4; Python was used only for isolated scratch experiments, not proposed as a new CI/runtime dependency. No external package installation is required, so a package-legitimacy registry audit is inapplicable. Project skill directories were absent at both inspected locations; configured researcher agent skills were empty. Graph context is disabled. Nyquist and security artifacts are explicitly disabled; do not introduce them. [VERIFIED: local version commands, agent-skills query and graphify status, exits 0; .planning/config.json:1-100]

The approved MIT seed remains the architecture/provenance pin quoted in context, `9f9386c661f6`; retain its required attribution. The regenerated table is owned mathematical output, not copied emulator expression. Hardware behavior remains the adopted documentary model, with physical startup, STIMER polarity and cartridge loading confidence-qualified. [VERIFIED: PROVENANCE.md:20-75; docs/hardware/pokey.md:1-60; locked D-07–D-08]

## Architecture Patterns

Keep the event/ring architecture. Current source-of-truth declarations are `MUHTARI__SYNTH_TAPS 16u`, `MUHTARI__SYNTH_PHASES 32u`, `MUHTARI__SYNTH_Q23 INT64_C(8388608)`, `MUHTARI__SYNTH_Q30 INT64_C(1073741824)`, `MUHTARI__SYNTH_MAX_LEVEL 18361`, and `extern const int32_t muhtari__synth_kernel[32][16];`. Proposed changes are the phase count, corresponding dimensions/endpoint comparison, generated rows, final bound-derived level and identity. [VERIFIED: src/internal.h:27-31,99-104; verbatim current declarations quoted here]

The production endpoint currently uses `row < 31u` and otherwise the zero-left/shifted-row-zero endpoint. Replace that hardcoded terminal row with the phase-count-derived comparison. Audit declaration, definition, all dimensional assertions, table-size prose, phase-endpoint tests, synthesis identity and numeric bound expectations together. Keep `impulse[16]`, the `& 15u` ring mask and 16 applications per merged event: phase-only changes do not lengthen support. [VERIFIED: src/synth.c:16-114,127-133,200-205; src/internal.h:49-60]

The authoritative bus convention remains fast/held rise s+2, fall/data s+4 and completion s+4; slow rise s+2, fall/data s+5 and completion s+6. High-pass captures commit at the third subsequent actual half edge, giving the tested fast/slow/mixed delays 6/9/7 ticks. Keep fast-boot tick108984/phi2zero and BIOS tickzero/phi2zero; do not synthesize skipped BIOS cycles. These are adopted compatibility conventions, not new physical measurements. [VERIFIED: docs/hardware/pokey.md:9-60; tests/audio/test_pokey.c:20-129; 04-06-SUMMARY.md:55-73]

Primary author documentation treats table density, support length and coefficient precision as separate error controls. Increasing density is therefore an evidence-directed local intervention; that documentation does not promise that a specific phase count meets this project's threshold. [CITED: https://www.dsprelated.com/freebooks/pasp/Implementation.html, Choice of Table Size and Word Lengths; MEDIUM from classify-confidence seam]

## Candidate measurements

The isolated build replaces only the synthesizer table/count/terminal comparison; it links the current built production library for actual chip behavior. It keeps capture length 65,536, settling 12,000, the existing periodic four-term Blackman-Harris window, 48 kHz, physical-only masks, nonlinear currents, Q12 gain4096, blocker arithmetic and signed-int16 conversion. The exploratory judge refines each largest unmasked FFT peak over ±0.6 bin using 28 ternary iterations of the continuous windowed DFT. Frequencies reported by the inherited diagnostic remain bin centers; dB results below use the refined amplitude. This is bounded one-off investigation, not a permanent analyzer gate. [VERIFIED: /tmp candidate source/run commands in Evidence; production measurement conventions in tests/audio/test_spectral.c:13-15,181-252,420-508]

| Candidate | Common M | Existing production cases | Actual cases | Negative controls | Worst TIA dBc | Worst equivalent POKEY dBFS |
|---|---:|---:|---:|---:|---:|---:|
| 64 phases | 18361 | 132 pass | 72 pass | 132 rejected | -89.80738 | -94.97775 |
| 128 phases | 18361 | 132 pass | 72 pass | 132 rejected | -89.91416 | -102.05614 |
| 64 phases final gain | 18360 | 132 pass | 72 pass | 132 rejected | -89.93812 | -94.97775 |

All 72 actual-chip settled int16 captures quantize to exact silence for both phase-density candidates, yielding negative infinity dBFS, not the diagnostic accumulator's initial sentinel -1000. The actual square captures retain chip-fire assertions and independent one-channel arithmetic-schedule PCM equality. The additional mixed/HP probe explicitly requires over 1,000 raw source changes and nonzero kernel applications, so disconnected output cannot masquerade as rejection. The measured TIA acceptance margin at common comparison gain is 9.80738 dB. These finite-case results establish a concrete recovery route; they do not prove every legal register stream is spectrally bounded. [VERIFIED: candidate production outputs; mixed exploration source/assertions and outputs; Evidence]

### Bounded mixed 4/6-clock and narrow-pulse exploration

Seven periodic clock words were tested, with low-order bit selecting the first cycle and 1 denoting slow: `1/2, 1/3, 3/3, 1/4, 3/4, 5/4, 7/4` (word/length). Use independently selected origins 1,29,661; two chips with one active channel each. The direct-square portion uses AUDF0/1/3, producing 63 cases. The high-pass portion uses the existing legal equal-period AUDF2 convention, with the high timer initially AUDF0 and changed to2 at the fourth fall, producing 21 cases. This yields 84 additional actual-model cases with no changed clocks or gain. Mixed square output is compared chunk-for-chunk to an independent arithmetic reference: first fire at falling-cycle N+8, subsequent fire every N+4 falling cycles. HP pulses are produced by the retained real three-half-edge pipeline. [VERIFIED: /tmp/muhtari_recovery/explore.py and generated mixed.c, commands in Evidence]

The complete settled raw period follows the actual repeating clock word and timer sequence. For these deliberately short patterns, square periods span lcm(word length,2(N+4)) cycles and HP periods span lcm(word length,12) cycles. Every resulting physical fundamental exceeds16kHz (maximum square period is224 ticks); every physical harmonic therefore lies outside the judged band. No average-frequency or folded-harmonic exemption is needed. Worst common-gain case is word3/length3, AUDF1, origin29, at -90.99157514 dBFS (bin center15358.15429688Hz); refined amplitude was used. HP cases all quantize to exact settled silence despite verified continuing raw transitions. Retain compact representative worst mixed and HP vectors in current tests; do not commit this whole exploratory matrix as a new framework. [VERIFIED: enumerated period arithmetic and 84-case output in Evidence]

## Full-path signed headroom and intermediate widths

Recomputed through the candidate coefficient function: all13,125 tick phases have exact sum8,388,608; all88 output alignments satisfy summation-by-parts weight sum8,388,608. For each alignment the probe constructs both extremizing bounded0/M sequences and sends them through the actual candidate event/ring path, checking integrated Q23 against the predicted value and zero clipping (176 extremizers). [VERIFIED: bound.c and bound commands in Evidence; existing analogous production test tests/core/test_core.c:285-365]

| Phases | Maximum coefficient magnitude | Positive mass P | Negative mass N | L1 numerator P+N | Derived M |
|---|---:|---:|---:|---:|---:|
| 64 | 7283992 | 10865505 | 2476897 | 13342402 | 18360 |
| 128 | 7283992 | 10865661 | 2477053 | 13342714 | 18359 |

For64 phases L1=1.590538263320923. The unchanged blocker has one positive and one negative impulse mass, so its conservative peak is L1*M plus rounding allowance. Retain one full PCM LSB for feedback truncation accumulation and final rounding. Thus M=floor((32767*10^(-1/20)-1)*8388608/13342402)=18360. At M18361, this bound plus oneLSB is29204.873052835464, exceeding the one-dB limit29203.619489828441; this is why the old M cannot be retained for the headroom proof. At M18360 the bound is29203.282514572144, below that limit. The fixed85:342 current ratio and Q12 gain4096 do not change. [VERIFIED: bound arithmetic, production blocker docs/hardware/audio.md:29-37,88-136; locked D-26]

The old numeric L1, masses and maximum-level proof are invalidated. Cmax remains7,283,992 in this enumeration, but retain the width proof independently: 16*150*18360*Cmax <2^49 for a pending ring cell; temporary integrator-plus-cell <2^50; P*M <2^38; Q23-to-Q30 conversion times128 <2^45; input difference plus feedback <2^47; quotient/remainder multiplication <2^60. The maximum mixed current and gain intermediate remain below2^34 and2^49. The 64-phase phase numerator is at most13124*64=839936 (<2^20), fitting uint32. Update numerical assertions from final coefficients, not by weakening expected bounds. [VERIFIED: candidate bound output; src/synth.c:99-114,116-142,180-205; docs/hardware/audio.md:114-136; integer arithmetic derived from quoted final inputs]

Support remains16 samples, at most150 merged ticks per sample and16 coefficient applications per merged event. The instruction/bus carry proof is structurally unaffected: current declarations quote `MUHTARI__SYNTH_PCM_CAPACITY (801u + MUHTARI__SYNTH_CARRY_SAMPLES)`, with the retained overshoot constants 456,2*454,12,6 and8+7 accesses giving20,730ticks and139carry samples. Revalidate ring/order/FIFO/carry/partition/NULL/reset tests after integration; no support/clock/frame amendment is justified by this evidence. [VERIFIED: src/internal.h:23-60; docs/hardware/audio.md:38-55; locked D-09,D-17,D-29]

## Reproducible owned coefficient generation

Do not depend on a transient scratch filename. Add a small owned maintainer generator within the existing C/test-only-math conventions, committed with its formula and deterministic output format. A standalone C17 generator using math.h and an owned I0 series needs no package or runtime dependency. It is an implementation task, not an existing repository file. Generate candidate rows independently at Simpson128,256,512; require exact equality after quantization/correction; emit the selected checked-in int32 table. Runtime and regular CI consume fixed integer data and validate invariants; no new mandatory policy gate is needed. [VERIFIED: no package installation in this probe; docs/hardware/audio.md:18-29 and existing test-only math in tests/audio/test_spectral.c:1-15; proposed implementation recommendation under D-29]

Use the documented integrand:
h(x)=sinc(x)*I0(8*sqrt(1-(x/7.5)^2))/I0(8) inside[-7.5,7.5].
For phase p of P and tap j, integrate over intersection of[j-7-p/P-0.5,j-7-p/P+0.5] with support.
Use true endpoint integrand values, not artificial zero. Quantize each integral by floor(area*8388608+0.5); then correct tap7 for p<P/2, otherwise tap8, so sum is8388608. Define the virtual phaseP as phase0 shifted right by one tap with left zero; row0 final cell iszero. Runtime interpolation preserves signed nearest/ties-away difference rounding and exact row correction using the existing phase threshold. [VERIFIED: docs/hardware/audio.md:18-33; src/synth.c:89-114; fresh scratch generator128/256/512 convergence]

The fresh generator's rows coincide at all three subdivision counts. To compare generators independently of C whitespace, hash phase-major/tap-major signed-int32 values serialized little-endian:64-phase SHA256 `8fdfc75effc7565eff4996b71ad966f4c0d327909ede230d052dc58bc239d66e`;128-phase `998e669a3aba82ccf2494ca22600ef06751a6921482e93cbeed468318af4f514`. Table sizes are4096/8192bytes. Treat these as candidate data fingerprints; the committed generator must reproduce selected output and convergence before it replaces production rows. [VERIFIED: generation and packed coefficient hash commands, exits0]

## Invalidated and retained proofs

| Evidence | Disposition | Required action |
|---|---|---|
| Q15 seed nine equivalent-schedule failures | Historical measurement retained, not rerun | Do not restore Q15 |
| Q23/32 original132 schedule success | Historical scope valid; fresh current run still reports those132 passing | Keep all132; do not extrapolate to actual slow clocks |
| Fixed32 actual slow AUDF1 quality | Refuted by six retained/currently reproduced failures | Replace density under adopted amendment |
| Existing32 coefficient masses/L1/M | Invalid for new rows | Substitute fresh exhaustive coefficient/alignment proof |
| Existing maximum coefficient and width powers | Cmax revalidated in candidate; width claims need final source check | Recompute numeric products and assert final bounds |
| TIA model/lattice, clocks, raw source IDs | No evidence invalidates them | Preserve behavior; run existing vectors |
| 16tap support, ring/work and carry structure | Unchanged mathematically by density | Recheck final implementation dimensions and contract tests |
| PCM hashes | Expected to change with synthesis | Freeze only after final production/platform verification |
| Video/raw-transition hashes | Synthesis recovery gives no cause to change them | Investigate any drift; never blanket repin |
| Host/platform parity | Not established by local candidate | Complete existing downstream slices |

[VERIFIED: reproduced baseline and candidate commands; opened current source and 04-06-SUMMARY.md; locked D-25–D-30]

## Precise planning amendments and resume boundary

1. Keep completed04-01–04-05 history. Amend04-06 Task2 and its declared file scope to include the table definition/declaration/count/endpoint, owned generator, numeric headroom assertions and audio documentation. Do not reexecute Task1 or discard its retained Task2 GREEN attempt.
2. First preserve/reproduce the committed six-failure RED evidence, then implement the selected64-phase rows and corrected endpoint dimensions. Compare at M18361 before the separately justified finalM18360. Keep132+132+72 checks and independent PCM comparisons; never replace them with synthetic-only captures.
3. Reproduce owned coefficient convergence/fingerprint, exact row/endpoint invariants, all13125 phases/all88alignments, adverse production sequences, full signed blocker bound and intermediate widths. Update identity and hardware/preparation prose together; never inherit the32-phase numeric proof.
4. Retain compact representatives from the bounded mixed/HP search and independently chosen origins in existing test coverage. Preserve actual half-edge schedules and document physical period/sidebands. One-off true-frequency refinement supplies candidate evidence; a permanent analyzer improvement is a tooling seed.
5. Revalidate affected core audio, TIA, POKEY, frame/partition/NULL/reset/carry tests with the final production objects; run fullCI and existing hygiene. Measure bounded hostile workload/performance using the current conventions. No automatic quality mode or new fallback.
6. Reconcile04-06-SUMMARY only after Task2 GREEN evidence passes. Only then unblock04-07 onward. Keep the existing expansion, maps/IRQ, transition/PCM hashing, revision/pin migration, hosts and platform evidence. Do not mark allSND requirements complete just because synthesis recovers.

These are executable tasks inside the authorized phase recovery; no additional owner choice/checkpoint is needed for candidate selection or the bound-derived one-LSB scale correction. [VERIFIED: locked D-25–D-30; current 04-06-PLAN.md:1-140 and 04-06-SUMMARY.md]

## Don't Hand-Roll

Use the current event/ring/blocker, nonlinear mix, plain-C tests, owned FFT, runner SHA256, fixture recipes and CI/platform lanes. Build only the missing owned coefficient-generation recipe and minimal recovery vectors. Do not introduce a resampler dependency, larger support, new filter, public profile, device code, copied excluded source, exhaustive permanent configuration matrix or policy machinery. [VERIFIED: locked context D-08–D-30; PROVENANCE.md; inspected synth/test seams]

## Common Pitfalls

- A table-only edit misses the hardcoded terminal row and declaration dimensions; audit all count-dependent sites.
- Reusing old L1 silently invalidates one-dB headroom even though the delta is small.
- Reducing gain before comparison can disguise poor rejection; compare at18361 and test final18360 independently.
- An FFT-bin peak understates off-bin sinusoid amplitude; use continuous-frequency one-off refinement.
- Average mixed-clock frequencies produce incorrect masks; derive period from the actual raw schedule.
- Exact ultrasonic int16 silence is legitimate only with proven continuing raw events and synthesis activity.
- Summary existence is not completion; downstream plans remain gated by actual Task2 GREEN.
- The isolated linker override is not production CI/platform certification.

[VERIFIED: observed source/probe outcomes and locked recovery constraints]

## Code Examples

Proposed implementation pattern, deliberately expressed without inventing an existing API:

```text
generate rows at 128, 256 and 512 subdivisions
assert corrected integer tables are identical
emit selected fixed integer rows and matching phase-count dimension
for each integer event phase:
    interpolate adjacent rows, including shifted endpoint
    correct row sum exactly
for each output alignment:
    compute signed step-difference weights
    realize both extremizing level schedules through production ring
derive final gain from signed blocker bound plus rounding allowance
compare fixed common gain, then validate final gain
```

This is a task algorithm, not a claim that a generator or new signature exists. [VERIFIED: candidate algorithms above; existing coefficient/bound mechanisms inspected]

## Evidence and limits

Commands were run in this research session; scratch logs/results remain outside the repository and are not deliverables.

| Command | Exit | What it establishes |
|---|---:|---|
| `ctest --test-dir build/ci -R '^audio.spectral$' --output-on-failure --no-tests=error` | 8 | Current production retains exactly six actual slow AUDF1 failures, worst-78.28741dBFS |
| `python3 /tmp/muhtari_recovery/run.py` | 0 | Generates64 then128, checks128/256/512 convergence, compiles isolated candidate objects, runs132+72 production and132controls at commonM18361 |
| `/tmp/muhtari_recovery/64/bound` | 0 | All13125 phases/88alignments and176 candidate event/ring extremizers; fresh64 masses and M |
| `/tmp/muhtari_recovery/128/bound` | 0 | Same enumeration for128; larger table and lower headroom scale |
| `python3 /tmp/muhtari_recovery/explore.py` | 0 | 84mixed/HP cases at commonM18361, independent square PCM reference, continuing raw changes/kernel activity |
| `/tmp/muhtari_recovery/64-final/spectral production` | 0 | FinalM18360 candidate132+72 cases pass |
| `/tmp/muhtari_recovery/64-final/spectral controls` | 0 | FinalM18360 rejects all132aliased controls |
| `/tmp/muhtari_recovery/64-final/mixed` | 0 | FinalM18360 mixed/HP checks pass |
| `git diff --check` | 0 | Research patch whitespace |

**Hosted run URL:** none; no hosted run was started. No new fullCI, frame-contract, platform-parity or performance certification is claimed. Scratch commands explain what was measured; the planned committed generator and existing production checks provide the durable reproduction path. No candidate is certified for release by this research. [VERIFIED: command outputs this session]

## Environment Availability

CMake4.4.3, Apple clang21.0.0, Python3.14.4 and the current built production library were available. No new package or network service is required for execution. The primary DSP author page was opened via web fallback after the research seam selected an unavailable Jina provider; its digest was cached with the confidence seam's MEDIUM tier. Physical/RTL referees remain the later milestone; their absence does not block this phase's adopted documentary model or create owner UAT. [VERIFIED: probes/query/open/store commands, exits0; locked D-07]

## Assumptions Log

No unverified training-knowledge assumption is promoted to a locked implementation decision. Outstanding production reproduction, generator ownership, performance and eventual platform parity are explicit executable proof obligations. Finite matrices do not certify arbitrary hostile register streams spectrally; bounded work still must hold for those streams. [VERIFIED: scope of measured experiments and locked D-09,D-12,D-21,D-29]

## Open Questions

No owner-blocking decision remains. The implementation must reproduce the64-phase route and proofs through final production objects; if that contradicts the isolated evidence, retain the failing case and diagnose coefficients/interpolation first. Any support/engine/arithmetic widening requires new measured cause and a new inventory of invalidated proofs under D-08. Do not create an owner approval or listening-test blocker. [VERIFIED: locked actual-chip D-08 amendment and D-25–D-30]

## Sources

- Local primary evidence: current context, requirements/state/roadmap/config; AGENTS; preparation index/decisions/audio and provenance; current synthesis/internal/chip/test/audio documentation; partial04-06 summary/plan. Source values are quoted beside discrete-value claims above.
- Candidate primary measurements: regenerated coefficient convergence, isolated production-object captures, signed step enumeration/extremizers and bounded mixed/HP captures described with commands above.
- Primary author DSP reference: [Implementation: table size and word lengths](https://www.dsprelated.com/freebooks/pasp/Implementation.html), opened2026-10-20; separates interpolation density from coefficient precision/support. Documentary confidence MEDIUM; project measurements carry the candidate conclusion.

## Metadata

**Standard stack:** HIGH, inspected existing C/test seams.
**Architecture:** HIGH for current code and adopted phase scope; physical chip confidence remains qualified.
**Candidate feasibility:** HIGH for the stated local matrices and arithmetic, not universal or final production certification.
**Research date:**2026-10-20. **Validity:** until context, coefficient arithmetic or final production source changes; reproduce acceptance before freezing PCM pins.
