# Phase 9: PAL games and audio - Context

**Gathered:** 2026-10-25
**Status:** Ready for planning

<domain>
## Phase Boundary

Deliver PAL-01–03: PAL detection/override, 313-line execution and fast/BIOS boot,
matching video/palette/aspect/rational-rate information through the library,
runner and libretro, and sustained deterministic raw audio and PCM carry.
Preserve NTSC output unless new evidence justifies an explicit behavior revision.
This discussion implements no behavior and completes no requirement.

The owner selected all relevant areas, requested specialist fan-out with primary
sources and an adversarial pass, and delegated adoption of coherent recommendations.
Three read-only reviews covered region/scheduler/boot, video/hosts and audio/device
time/evidence. Decisions below are agent-selected under that instruction, not
invented interview answers. Cross-area review found lower PAL light-gun rows,
fixed-size DMA statistics, a past first-boot deadline and NTSC-only synthesis bounds.
Expansion stopped when it produced no material new choice.

Carry forward three deliverables, integer-only instance state, bounded calls,
transactional validation, no allocation after load, permissive provenance,
automation-first verification and finite review/gap limits. Reuse existing owners
and small owned helpers; no new dependency or general clock framework is selected.
Stop after this discussion; planning and implementation are separate steps.
</domain>

<decisions>
## Implementation Decisions

### Region selection, compatibility and lifecycle

- **D-01:** Keep the probe's current metadata resolution. Explicit load override
  wins; otherwise use the probe's explicit-header, applicable hash-list and existing
  fallback rules. Preserve multiregion/headerless handling and NTSC preference for
  dual-region entries. Never infer console region from firmware size, filename or
  trial execution. Commit one effective region only with a successful load; rejected
  candidates and allocation failure preserve prior region, content, battery and
  execution. Power cycle retains region; successful reload may replace it. No-content
  operation remains the historical NTSC test card and silence.
- **D-02:** Enable PAL for existing callers using the existing region enum and media
  field. The PAL refusal in `tests/api/v3.c` implements a documented temporary limit
  ("until PAL timing is emulated"); deliberately migrate that behavioral expectation
  to success, preserving layouts, size minima, symbols and numeric status 12. Do not
  make PAL depend on struct size or invent a legacy refusal route. Test omitted
  optional tails against explicit defaults. Retain ABI 1 and apply the established
  API/behavior revision policy to actual declaration/hash changes; version annotation
  changes need narrow evidence, never wholesale hash rebaselining.

### Scheduler, MARIA and boot

- **D-03:** Extend the current scheduler with selected NTSC/PAL timing, not a second
  run loop. PAL uses 7,093,788 master ticks/second, 454 ticks/line, 313 lines and
  142,102 ticks/full frame. Preserve the existing software-coordinate convention:
  MSTAT sets at line 308,t42 and clears at line 16,t42; VBE remains line 16,t43 and
  existing DMA-request scheduling extends through line 308. Display rows end at
  native row 291. Grow physical DMA-statistics storage to cover 313 lines and keep
  indexing bounded by the active region. Merely widening event bounds around the
  current 263-element array is invalid. Preserve per-fetch DMA, pixel emission,
  data-tick ordering, HALT/RDY and absolute-deadline overshoot behavior.
- **D-04:** Select a documented deterministic PAL fast-boot profile: reset-vector PC,
  S=X=$16, A=$60, Y=0, P=$2C, INPTCTRL=$16 with the existing two-write gate, CTRL=$60,
  DPP=$2707, and $FF at $2000–$203F/$2100–$213F. RIOT RAM retains `ram_fill`; no BIOS
  loader or splash bytes are reconstructed. PAL DMA internals are unmeasured: start
  their undocumented residue deterministically at zero rather than transplanting
  NTSC's measured DL/DLL/OFFSET snapshot.
- **D-05:** Use line 308,t184 as the explicit fitted PAL fast-handoff default, inside
  preparation C10's inferred beam range. This is policy, not a hardware measurement.
  Its tick is 140,016; the first strictly future VBLANK start is 281,976, yielding
  141,960 elapsed ticks. The same-origin boundary 139,874 is already past and must
  never become the deadline. Preserve absolute-lattice TIA catch-up and the released
  cartridge/output epoch rules. Retain the legacy versus extended first-input latch
  distinction and clear all 292 output rows when a first frame is partial. A legal
  PAL BIOS/hardware trace demonstrating a startup dependency can reopen this fit;
  no owner hardware run blocks the phase.
- **D-06:** Supplied BIOS boot executes real reset and instructions at the selected
  console region, independently of the fast snapshot. Its first PAL deadline is
  139,874 ticks. Preserve actual first-cartridge-opcode handoff timestamps and sticky
  BIOS refusal. Firmware size controls mapping, not region: existing accepted 4/16 KiB
  images can execute with either explicit region and may naturally reject content.
  Use owned in-memory synthetic firmware for precise mapping/RES/rejection vectors
  and the existing pinned, stripped OpenBIOS build for public integration. OpenBIOS
  has different S/D/INPTCTRL/display behavior; do not demand Atari PAL handoff parity
  or copy prohibited Atari bytes to make it appear so.

### Native video, palette and host contracts

- **D-07:** Report PAL 320×292, FPS 3546894/71051 and nominal pixel aspect
  1843750/1773447 through the existing `muhtari_info`. The PAR follows preparation's
  square-pixel convention, 14,750,000/(2×7,093,788); display aspect is
  147500000/129461631, approximately 1.139333707. Preserve NTSC's exact values.
  libretro converts these rationals at its floating-point host boundary; the core
  remains integer-only. Keep its existing maximum height 293 while actual PAL
  callbacks contain 292 rows. Do not round the frame rate to 50 Hz or force 4:3.
- **D-08:** Callers requery info after successful load and provide the resulting
  output storage: 93,440 packed PAL bytes, or accessible extent
  `(height - 1) * pitch + width`. Preserve pitch limits and argument/error ordering.
  The released frame struct has no allocation-byte count; do not claim it can detect
  an undersized allocation or enlarge the ABI just for that claim. Check writer
  bounds with outer canaries and row padding, partial-frame clearing and NULL video.
  Failed validation executes nothing and reports zero audio samples as documented.
- **D-09:** Extend the approved host palette seed with only the warm PAL table from
  the pinned BSD-3 a7800 file. Keep the existing palette helper's warm-NTSC behavior
  for old callers and use a region-aware helper internally. Verify all 256 RGB values
  against the pinned seed, plus RGB565 truncation; PAL is not just rotated NTSC hues.
  Update existing provenance/notices. Palettes remain outside native hashes and
  native light sensing. No new palette package, generator, crop, filter or colour
  temperature family is needed.
- **D-10:** Runner allocation, native hash extent, PNG dimensions and movie-region
  acceptance follow effective post-load info. Add a narrow opt-in `--info` key/value
  report for effective region, width/height, FPS numerator/denominator, PAR
  numerator/denominator and palette. Keep existing default transcripts and descriptive
  `--probe` identity unchanged. No new media container, image rescaling or schema
  framework. Explicit CLI/movie region conflicts follow current precedence and
  validation contracts rather than silently replaying a different session.
- **D-11:** libretro region options `auto/ntsc/pal` are load-stable and reload-only;
  edits during a loaded session do not change emulated time or AV geometry. Auto
  palette follows effective region, with explicit warm NTSC/PAL choices retained as
  host presentation choices. Reset cached AV/palette on release and neutral fallback.
  The frontend queries AV after successful load; do not invoke SET_SYSTEM_AV_INFO
  or SET_GEOMETRY inside load (the API restricts them to retro_run). No live-switch
  machinery is needed. Failed library loads preserve their prior state; the existing
  adapter releases before loading, so its failed load instead leaves clean fallback
  metadata, without stale PAL pointers. Do not add adapter transactionality by accident.

### Audio, physical time and finite work

- **D-12:** Keep TIA's independent 456-tick lattice, source ordering, POKEY's actual
  stretched φ2 edges through HALT, and raw half-open timestamp ownership. Parameterize
  the existing sample carry and synthesis phase arithmetic for PAL's reduced
  4000/591149 samples/tick at 48 kHz. Full PAL frames return 961 or 962 samples, not
  960. Handle actual elapsed first frames and instruction overshoot without rebasing
  epochs or rounding each frame separately. NULL PCM/video outputs still advance the
  same hardware/carry. Reject insufficient audio capacity before state or callbacks
  change; test exact capacity and one sample short.
- **D-13:** Recompute fixed sample/FIFO capacity and PAL arithmetic bounds before
  enabling execution. The existing 20,730-tick overshoot bound requires up to 141 PAL
  future samples; 962 current plus 141 future requires 1,103 cells under that proof.
  Verify the bound against the actual owner and all first-frame paths rather than
  keeping NTSC's 801/139 assertions. Reprove interpolation, signed coefficient mass,
  accumulator widths, carry and headroom over PAL's phase alignments; use the existing
  kernel, blocker and gain/quality controls. A denominator of 591,149 does not justify
  allocating a dense denominator-sized filter table. Preserve synthesis cost bounded
  per sample, no allocation after load and hostile-ROM run bounds.
- **D-14:** Convert elapsed physical-time device models using the selected clock,
  with explicit rounding: keypad 400µs rounds up to 2,838 PAL ticks; the already
  selected EEPROM 5ms model rounds up to 35,469 PAL ticks. Five milliseconds remains
  a compatibility duration, not a measured EEPROM latency. Scale the unrounded fitted
  paddle duration by PAL/NTSC clock ratio 6502639/6562500, using bounded uint64_t math;
  do not scale an already rounded NTSC deadline. Preserve motion's documented
  454-tick native-line cadence and light D/W tick fits; they are compatibility policies,
  not physical-time constants to convert blindly. Document confidence and test
  deadline−1/exact/+1, changed pots, HALT/JAM and unpolled progress.
- **D-15:** Extend native light/aim validation, emission and delayed-response wrapping
  to the effective height/frame period. PAL y291 is valid and y292 invalid; NTSC y241
  remains valid and y242 invalid. Cover raster 307, x319, delayed frame crossing,
  colour kill, dark/bright/blanked and off-screen controls, and identical sensor behavior
  with NULL video. Host RGB palettes never determine light. Preserve committed battery
  bytes, attachment and generations across region-appropriate power cycles and failed
  loads; existing unfinished-protocol reset policy still applies.
- **D-16:** Retain current 85:342 mixer weights as an explicitly unverified PAL
  compatibility assumption. New evidence is Atari's preliminary PAL rev2 schematic:
  equal 6.8kΩ input resistors suggest 25:38 current weights, but production applicability
  is uncertain and the sheet also has an inconsistent crystal label. Do not claim the
  retained model is a measured PAL analog mix. A released production schematic or
  reliable board-resistor measurement can reopen it with fresh mix/headroom checks.
  This evidence does not reopen the settled PAL clock or select an analog-model redesign.

### Independent evidence and stopping rules

- **D-17:** Extend plain C tests and owned numeric diagnostics before pinning hashes.
  Independently judge MSTAT boundary−1/at/+1, final DMA and absent next-line DMA,
  line/frame wrap, boot snapshots/deadlines, exact sample totals, transition ticks and
  source order, and lower PAL rows. Exercise library, actual runner and loaded libretro
  in both pixel formats; test NTSC→PAL→unload→NTSC card→NTSC in the same module,
  overrides both directions, power cycle, rejected load and no-allocation. Derive
  expectations from documented facts/policy, never production profile getters alone.
- **D-18:** Use a cheap full 591,149-frame arithmetic carry sweep (568,408,000 samples,
  zero final remainder from zero) plus a bounded 600-full-frame actual owned diagnostic.
  The actual run spans over two 228-frame TIA/frame-phase recurrences; separately
  check first-frame offsets and carry. Compare native/raw-audio/PCM outputs only after
  numeric expectations pass. Reuse existing native cross-platform equality and
  applicable portability lanes with unchanged required/advisory authority. Extend
  trusted fixture admission narrowly for owned PAL rows, proving real producer/join
  pass/fault behavior and preserving old identities. No 591,149-frame rendering run
  or new evidence framework is justified.
- **D-19:** Preserve existing NTSC diagnostic/hash predicates. If a discovered required
  hardware correction changes them, name the source, exact affected outputs and
  permitted behavior-version transition. Equal hashes alone do not establish correct
  PAL timing. Keep current two hosted RetroArch cases; in-process host tests prove PAL
  semantics. Preserve authenticated current/base three-component package measurements,
  package members/notices/asset checks and the armed +1% test-card performance guard.
  DEC.61's retired archive cutoff and retired maintenance-line quotas do not return.
  Measure complete changed workloads before choosing timeouts; no unrelated advisory
  lane promotion, audit expansion or repeated hosted probing just for another size report.

### Agent's Discretion

Internal profile representation and helper names, test/diagnostic names, plan ordering,
report key spelling and arithmetic decomposition are delegated within these contracts.
Research verifies source facts, exact bounds and independent vectors before freezing
implementation expectations. Fits remain labelled; new evidence can revise them.
No further owner interview is needed for routine engineering choices. This discussion
does not claim future PAL tests, quality proofs or hosted acceptance have passed.
</decisions>

<canonical_refs>
## Canonical References

Read the relevant sections before planning or implementing.

### Scope, settled choices and evidence policy

- `AGENTS.md`, `.planning/PROJECT.md`, `.planning/REQUIREMENTS.md` PAL-01–03,
  `.planning/ROADMAP.md` Phase 9 — scope and step boundary.
- `.planning/preparation/README.md`, `.planning/preparation/DECISIONS.md` — index;
  especially DEC.04,09–11,15–19,22–30,33,37–38,40,43–47,51–58,61.
- `.planning/preparation/A7800-SYSTEM.md` §§1–2,7–8 — clocks, coordinates, firmware
  facts and qualified fast handoff.
- `.planning/preparation/A7800-MARIA.md` §§4,6–8,
  `.planning/preparation/A7800-AUDIO.md` §§1–5,8,
  `.planning/preparation/ARCHITECTURE.md` §§2–7 — native video and time/audio owners.
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` §§1,3–4,7,
  `.planning/preparation/HOSTS-AND-INTEGRATION.md` §§1–4 — region metadata, peripherals,
  host contracts and bounded frontend acceptance.
- `.planning/preparation/CONFORMANCE.md` §§4–7,
  `.planning/preparation/ENGINEERING.md` §§3–4,8,11,
  `.planning/preparation/GSD-HANDOFF.md` §3.1,
  `.planning/research/workflow-recovery/prompt.txt`, `.planning/LESSONS.md`,
  `PROVENANCE.md`, `ASSET_POLICY.md` — independent/legal evidence, architecture review,
  bounded synthesis and current package policy.
- `.planning/phases/06-retail-cartridge-boards/06-CONTEXT.md`,
  `.planning/phases/07-battery-saves/07-CONTEXT.md`,
  `.planning/phases/08-controllers-at-the-pins/08-CONTEXT.md` — prior lifecycle,
  battery, input and native-emission decisions; historical quotas are superseded by
  current DEC.10/61.
- `docs/embedding.md`, `docs/runner.md`, `docs/libretro.md`, `docs/testing.md`,
  `docs/emulation-tests.md`, `docs/quality.md`, `docs/hardware/bus.md`,
  `docs/hardware/maria.md`, `docs/hardware/audio.md`, `docs/hardware/tia.md`,
  `docs/hardware/controllers.md` — released contracts to update with behavior.

### Existing code and automation

- `include/muhtari.h`, `src/internal.h`, `src/instance.c`, `src/boot.c`, `src/frame.c`,
  `src/bus.c`, `src/maria.c`, `src/synth.c`, `src/tia.c`, `src/pokey.c`, `src/ports.c`,
  `src/cart.c`, `host/palette.c`, `host/palette.h`.
- `runner/main.c`, `runner/movie.c`, `runner/png.c`, `libretro/libretro.c`,
  `libretro/libretro.h`, `libretro/muhtari_libretro.info.in`.
- `tests/api/v3.c`, `tests/api/v4-layout.c`, `tests/core/test_core.c`,
  `tests/audio/tests.cmake`, `tests/audio/test_contracts.c`,
  `tests/audio/test_spectral.c`, `tests/audio/test_stress.c`,
  `tests/input/test_input.c`, `tests/runner/test_runner.c`,
  `tests/runner/test_movie.c`, `tests/libretro/host.c`, `tests/alloc.h`.
- `tests/diag/driver.c`, `tests/diag/assemble.cmake`, `tests/diag/tests.cmake`,
  `tests/diag/expected.csv`, `tests/roms/manifest.txt`, `tests/compat/ledger.csv`,
  `tests/cmake/quality.cmake`, `tests/cmake/quality_join.cmake`,
  `tests/cmake/lane_equality.cmake`, `tests/retroarch/run_retroarch.cmake`,
  `quality.json`, `pins.txt`, `.github/workflows/ci.yml`,
  `.github/workflows/nightly.yml` — existing admission, identities, judges and lanes.

### External evidence reviewed 2026-10-25

- [Atari 7800 Software Guide](https://7800.8bitdev.org/index.php/7800_Software_Guide)
  — primary guide transcription; PAL rasters/display and access timing. Translate
  document coordinates into the code's established convention explicitly.
- [A78 specification](https://7800.8bitdev.org/index.php/A78_Header_Specification)
  — maintained format specification for PAL/multiregion flags.
- [Pinned OpenBIOS README](https://raw.githubusercontent.com/7800-devtools/7800OpenBIOS/b8bc745a8744ae3e0d24cc151e63f669f6a87c24/README.txt)
  — CC0 BIOS and separate non-CC0 payload; reuse the existing stripped build only.
- [Pinned warm PAL palette source](https://github.com/7800-devtools/a7800/blob/7a2afdc1ea08fc331b16b750d8c1f02d4ef62fc8/src/mame/drivers/a7800.cpp#L805)
  — approved per-file BSD-3 seed, not permission to open other emulator source.
- [TI TVP5010 manual, Table 2–1](https://www.mouser.com/catalog/specsheets/tvp5010.pdf#page=20)
  — original TI square-pixel PAL reference; the chosen 7800 PAR is derived policy.
- [Official libretro lifecycle](https://docs.libretro.com/development/cores/developing-cores/#retro_get_system_av_info)
  and [MIT libretro API header](https://github.com/libretro/libretro-common/blob/master/include/libretro.h)
  — after-load AV query and legal AV-update callback timing; retain the vendored header.
- [Altirra Hardware Reference Manual](https://www.virtualdub.org/downloads/Altirra%20Hardware%20Reference%20Manual.pdf)
  — primary hardware measurements for POKEY; not Altirra emulator source.
- [Microchip 24LC256 datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/MPD/ProductDocuments/DataSheets/24AA256-24LC256-24FC256-256K-I2C-Serial-EEPROM-DS20001203.pdf)
  — write-cycle maximum and busy NACK, distinct from the selected duration.
- [Atari preliminary PAL schematic C025964 rev2](https://www.atariage.com/7800/archives/schematics_pal/images/Schematic_Atari7800_PAL_LR_2000.jpg)
  — conflicting mixer evidence, with production applicability explicitly unresolved.

GPL/LGPL and unlicensed emulator source remain excluded. No new external code beyond
the already approved palette seed is selected.
</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets

- Probe metadata, the public info/media layouts, transactional loader and current
  power-cycle path already provide region/lifecycle seams.
- The one deadline dispatcher, lazy chip clocks, per-fetch MARIA scheduling and
  integer synthesis can carry a selected profile without another scheduler.
- Existing palette/PNG helpers, movie validation, loaded-module host mock, numeric
  diagnostic pipeline, seeded safety tests and authenticated equality avoid new tools.

### Established Patterns

- Validate before mutation/allocation, fixed-width bounded instance state and stable
  caller ownership. Native pixels and raw transitions precede host conversion.
- Independent numeric observations precede cross-platform hash comparison. Exact
  inventories, source identity and meaningful fault controls remain authoritative.

### Integration Points

- `src/instance.c` currently refuses PAL; frame, MARIA, boot, synthesis and stats then
  assume NTSC. Change acceptance only when those consumers agree.
- `src/ports.c` and native emission in `src/maria.c` contain NTSC dimensions/periods;
  physical-time device fits and line/tick compatibility fits need different treatment.
- Runner and libretro retain NTSC buffer/height/palette assumptions even though the
  public info structure can already express PAL. Audit direct consumers together.
- The API v3 PAL-refusal fixture is temporary capability evidence, unlike frozen
  structure layouts and the intentionally frozen legacy controller setter.
</code_context>

<specifics>
## Specific Ideas

Relevant lenses were hardware confidence, C/ABI/buffer safety, rendering causality,
audio reconstruction, player/host expectations, deterministic replay, provenance,
evidence validity and operating cost. Unrelated UI/framework/distributed-system
choices add no value. Principal footguns are a past PAL first deadline, 263-line
storage behind 313-line scheduling, plausible-but-wrong 50Hz/960-sample output,
rounded paddle conversion, omitted lower light-gun rows and stale host metadata.
</specifics>

<deferred>
## Deferred Ideas

Production-calibrated PAL mixer/boot/optics, live region switching, general sample
rates, additional presentation options and new hosted frontend cases remain later
work requiring new evidence or a concrete consumer. Expansion boards stay Phase 10;
whole-library accounting stays Phase 11. No oracle, save-state or recording expansion.

### Reviewed Todos (not folded)

- `.planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md`
  — promotion needs its own qualifying history; PAL does not change lane authority.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md`
  — residual tooling seeds add no PAL requirement; preserve completed corrections.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md`
  — substantive run-loop contracts remain binding; optional residual test seeds do
  not become an extra audit or gap round.
- `.planning/todos/pending/2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md`
  — unrelated release follow-up; preserve accepted Phase 5 controls.
</deferred>

---

*Phase: 09-pal-games-and-audio*
*Context gathered: 2026-10-25*
