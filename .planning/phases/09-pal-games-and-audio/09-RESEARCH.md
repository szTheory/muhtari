# Phase 09: PAL games and audio - Research

**Researched:** 2026-10-25
**Domain:** Atari 7800 PAL scheduling, deterministic audio and three-host geometry
**Confidence:** MEDIUM

<user_constraints>
## User Constraints (from CONTEXT.md)

Copied verbatim below: all nineteen locked decisions, delegated discretion and deferred ideas. These are governing inputs, not new research claims. [VERIFIED: .planning/phases/09-pal-games-and-audio/09-CONTEXT.md, Implementation Decisions and Deferred Ideas]

DATA_6A1F9C34_START

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

DATA_6A1F9C34_END
</user_constraints>

## Project Constraints (from AGENTS.md)

The following are binding directives, carried from the project instructions, not proposed new gates. [VERIFIED: AGENTS.md:13-86]

- Reuse only permissive code with required provenance/notices; never open GPL/LGPL/unlicensed emulator source. Reference emulators are released binaries. Firmware/game/save bytes enter only through admitted fixtures.
- Deliver the library, headless runner and libretro core; device I/O and windows belong to hosts.
- Keep integer-only deterministic emulation, instance-owned state, bounded calls and zero allocations after load; use fixed-width types, plain control flow, small modules and namespaced symbols.
- Every behavior change needs a CI-registered test and matching documentation. Hardware comments describe hardware and cite its source; no planning IDs in source, test names or user docs. README/public header stay terse; maintainer and behavior prose belongs in docs.
- Verification is automation-first. Agent owns judgment, image inspection and actual RetroArch use. Ask the owner only for identity-bound consent or a run on their hardware.
- Extend existing CMake/POSIX-sh mechanisms; changed checks name a concrete fault and prove representative pass/fault behavior through the consumer. Apply ENGINEERING §11 architecture/resource-bound review; two code-review fix rounds and one gap round maximum.
- Keep required safety, product, provenance and release contracts. Retired maintenance quotas and the retired compressed archive cutoff must not return; authenticated current/base package measurements and integrity remain required.
- One GSD step per session; no auto/chained/profiling commands or hygiene bypasses. Branch before committing, use the required noreply identity and hooks; ship/merge remains a separate step.
- Evidence is command, exit status and run URL, never a committed log. SUMMARY coverage must use the prescribed block-style entries and requirements-completed field. Inbound lessons remain hypotheses until reproduced.

The Claude pointer repeats the prohibition on profiling. No project-local skill files were discovered and agent_skills is empty. Workflow settings explicitly say "nyquist_validation": false and "security_enforcement": false; therefore this artifact omits the optional Validation Architecture and ASVS Security Domain sections. Existing buffer, bounded-execution and provenance tests remain required. [VERIFIED: .claude/CLAUDE.md:1-4; .planning/config.json:24-63]

<phase_requirements>
## Phase Requirements

DATA_9F3D6A2C_START

| ID | Description | Research Support |
|---|---|---|
| PAL-01 | PAL content and an explicit PAL override use 313-line timing, PAL MSTAT/DMA/frame boundaries and documented fast/BIOS boot hand-off rather than NTSC defaults. | Shared selected timing, transactional load, scheduler/statistics bounds, separate fast/BIOS startup oracles. |
| PAL-02 | Library, runner and libretro expose the correct PAL geometry, rational frame rate, palette and aspect information. | Existing info layout, post-load allocation, pinned warm-PAL table, load-stable libretro options and actual host tests. |
| PAL-03 | Sustained PAL runs keep transition timestamps and PCM sample carry consistent with the PAL clock, with cross-platform frame/audio hashes. | Independent carry and signed-mass arithmetic, bounded FIFO, 600 full-frame numeric diagnostic followed by existing equality lanes. |

DATA_9F3D6A2C_END

Descriptions quoted verbatim. [VERIFIED: .planning/REQUIREMENTS.md:45-47]
</phase_requirements>

## Summary

Use one effective region committed with successful content installation and extend the existing owners. The public region choices already exist: "MUHTARI_REGION_NTSC = 1, MUHTARI_REGION_PAL = 2". The current loader resolves an override before rejecting every non-NTSC candidate; info, frame execution, synthesis, MARIA storage, ports and both hosts then assume NTSC. Removing that refusal alone would expose incomplete timing and memory bounds. [VERIFIED: include/muhtari.h:127; src/instance.c:133-158,300-369; src/internal.h:10-60,298-318; src/frame.c:43-107]

The exact arithmetic is feasible with the existing kernel, gain, integer widths and bounded synthesis architecture. An independent C17 probe linked directly with the current synthesis source enumerated all PAL interpolation phases and output alignments; the current gain retains its one-dB headroom. PAL needs a larger FIFO and active-region geometry everywhere, but no dense phase table, package or replacement scheduler. These are implementation recommendations supported by the probe E1 below and locked D-03/D-07/D-12/D-13. [VERIFIED: arithmetic probe E1; src/synth.c:156-170,179-219,393-419]

No genuinely new hardware evidence overturning the locked decisions was found. Keep the PAL handoff and optical/paddle parameters labelled fits, the EEPROM duration labelled compatibility policy, and the retained PAL mixer labelled unverified production applicability. Do not equate source agreement or cross-platform equality with hardware measurement. [VERIFIED: 09-CONTEXT.md, D-04–06,D-14–16,D-19]

**Primary recommendation:** Prove narrow evidence admission first, then ship a PAL execution tracer through the existing owners with independently derived arithmetic and all three consumer contracts, preserving every NTSC predicate.

## Architectural Responsibility Map

This is a recommended ownership map based on existing module responsibilities. [VERIFIED: src/instance.c:133-193,300-369; src/frame.c:22-132; src/boot.c:43-108; libretro/libretro.c:1168-1213]

| Capability | Primary tier | Secondary tier | Rationale |
|---|---|---|---|
| Effective region and transactional lifecycle | Core instance/load | Probe metadata | Probe describes content; instance commits the selected console model only after successful installation. |
| Frame/line/MSTAT/DMA/boot | Core scheduler and MARIA/boot | CPU/bus | Preserve current event/data ordering and bounded instruction overshoot. |
| Sample phase, raw ordering, PCM carry | Core synthesis and chip clocks | Frame owner | Logical half-open horizons differ from hardware catch-up. |
| Keypad/paddle/EEPROM elapsed time; native light | Core ports | Bus and MARIA emission | Physical-time conversions use the selected clock; native tick fits remain tick fits. |
| Palette, PNG and float AV conversion | Host helper / runner / adapter | Core info | Native pixels and hashes remain independent of RGB conversion. |
| Numeric judgment and equality | Existing C/CTest diagnostic pipeline | Existing CI lanes | Compare independent numbers before using hashes as regressions. |

## Standard Stack

Reuse the existing stack; no dependency installation or registry package is recommended. [VERIFIED: 09-CONTEXT.md, phase boundary,D-09,D-13,D-18; CMakeLists.txt:10-18]

| Component | Version / identity | Use |
|---|---|---|
| Core language | "CMAKE_C_STANDARD 17", "CMAKE_C_EXTENSIONS OFF" | Existing fixed-width integer implementation. [VERIFIED: CMakeLists.txt:16-18] |
| Build/test workflow | Presets require "major": 4, "minor": 3; local CMake 4.4.3 | Existing CMake/CTest, exact inventories and scripts. [VERIFIED: CMakePresets.json:1-9; environment probe E3] |
| Owned diagnostic toolchain | Existing pinned WASM tools; local wasmtime 48.0.6 | Rebuild owned assembly diagnostics and stripped OpenBIOS; no native assembler required. [VERIFIED: tests/built/tests.cmake:7-17; pins.txt:10-20; environment probe E3] |
| PCM reconstruction | "exact-q23-16x64-dc20" | Preserve kernel/blocker/gain while selecting phase rational. [VERIFIED: docs/hardware/audio.md:3-7,19-27,49-59] |
| PAL RGB seed | Approved per-file BSD-3 a7800 pin 7a2afdc1ea08fc331b16b750d8c1f02d4ef62fc8 | Add warm PAL only with existing notices. [CITED: https://raw.githubusercontent.com/7800-devtools/a7800/7a2afdc1ea08fc331b16b750d8c1f02d4ef62fc8/src/mame/drivers/a7800.cpp] |

**Installation:** none. Package Legitimacy Audit is inapplicable because this phase installs no external packages. Preserve existing pins and approved seed policy; do not upgrade tools as PAL work. [VERIFIED: 09-CONTEXT.md, D-09,D-18,D-19]

## Architecture Patterns

### System architecture diagram

The diagram is a recommended conceptual flow; existing owners and locked contracts are its source. [VERIFIED: src/instance.c:300-369; src/frame.c:22-132; 09-CONTEXT.md,D-01,D-03,D-12]

~~~mermaid
flowchart LR
  A[Content plus explicit load choice] --> P[Existing probe metadata]
  P --> V{Validate whole candidate}
  V -->|refused| O[Prior library state preserved]
  V -->|accepted| R[Commit effective console region]
  R --> B{Boot choice}
  B --> F[Selected deterministic fast handoff]
  B --> BIOS[Supplied firmware executes real reset]
  F --> S[Existing CPU and deadline dispatcher]
  BIOS --> S
  S --> M[MARIA events and native emission]
  S --> C[TIA lattice and actual cartridge clocks]
  S --> D[Timed controller and EEPROM progress]
  C --> Y[Merge transitions and bounded synthesis]
  M --> I[Info and native rows]
  I --> H[Runner / libretro]
  Y --> H
  H --> U[Host palette, PNG, AV and audio callbacks]
~~~

### Component responsibilities and exact current seams

All entries below are recommendations about the named existing source owner, not instructions to create a general clock subsystem.

| Owner | Current evidence opened this session | Required change |
|---|---|---|
| Instance/load and profile | "uint32_t region = media->region != 0u ? media->region : info.region;" followed by "region != MUHTARI_REGION_NTSC". [VERIFIED: src/instance.c:325-330] | Retain existing probe precedence. Add effective instance region or compact value profile; commit after cart-install success, before boot. Do not mutate descriptive probe metadata to make overrides appear to be header facts. |
| Profile representation | Existing fields are "ane_lxa_constant", "riot_orb_power_on", "ram_fill", "tia_phase", "pokey_init". [VERIFIED: src/internal.h:213-224] | Keep the selected timing separate from these power-on variations or append a clearly owned field. A small value-returning helper/switch is sufficient; avoid a pointer registry or general rational-clock engine. |
| Public info and buffers | "uint32_t width, height", "fps_num, fps_den", "par_num, par_den", "sample_rate_num, sample_rate_den", "max_audio_samples"; frame contains pitch and pointers but no video allocation size. [VERIFIED: include/muhtari.h:55-71] | Populate selected geometry/rationals and exact region maximum. Requery after load; retain all released sizes and validation/error ordering. Do not claim allocation-size validation. |
| Frame owner | "elapsed * MUHTARI__SAMPLES_PER_PERIOD", latch comparisons, clearing loop and deadline increment are NTSC-specific. [VERIFIED: src/frame.c:43-83,103-107] | Select all consumers coherently, including input-latch conditions and partial-frame clearing; validation still precedes latching, callbacks and execution. |
| MARIA storage and dispatch | "stats[MUHTARI__LINES_NTSC]" and "MUHTARI__LINES_NTSC 263u". "MARIA_FIRST_DMA_LINE 17u", "MARIA_VBE_LINE 16u", "MARIA_VBLANK_LINE 258u". [VERIFIED: src/internal.h:277-307; src/maria.c:394-396] | Grow physical stats storage to PAL maximum; bound every stats access by effective lines; select terminal line, visible height and frame wrap. Audit request_dma's direct indexing as well as guarded writes. |
| MARIA event owner | "return line >= MARIA_FIRST_DMA_LINE && line <= MARIA_VBLANK_LINE;" and first future event is armed by event_on/arm_after. [VERIFIED: src/maria.c:503-530] | Preserve the inclusive current request convention through PAL line 308, not a newly interpreted guide coordinate. Preserve t0/t28/t42/t43 scheduling, pixel emission and post-fetch ordering. |
| Synthesis | "MUHTARI__SAMPLES_PER_PERIOD 88u", "MUHTARI__TICKS_PER_PERIOD 13125u", "MUHTARI__MAX_AUDIO_SAMPLES 801u"; hard-coded arithmetic also exists in interpolation, advance and render. [VERIFIED: src/internal.h:18-21; src/synth.c:156-170,206-219,403-407] | Carry selected numerator/denominator through every stage, including reset, count arithmetic and assertions. Keep NTSC operation ordering and rounding identical. |
| TIA/raw observer | "TIA_LINE_TICKS 456u"; raw ordering is stable ascending source with half-open carry. [VERIFIED: src/tia.c:5-7; src/synth.c:299-320,343-359] | Never replace the TIA lattice with the MARIA line period; preserve absolute catch-up before resetting fast-boot output epoch. |
| Timed ports | "2864u" keypad deadlines; "resistance * 187747u"; busy start uses "MUHTARI__SAVEKEY_BUSY_TICKS". [VERIFIED: src/ports.c:84-103,139-152,398-406,732-746] | Select physical-time conversions at all creation/reprojection/reset sites. Preserve charge progress, deadline ordering and EEPROM reset policy. |
| Light emission | Native aim is capped by "MUHTARI__HEIGHT_NTSC" and delayed sampling wraps by "MUHTARI__TICKS_PER_FRAME_NTSC". [VERIFIED: src/ports.c:218-237,647-655; src/maria.c:453-478] | Extend validation, aperture clipping, rendering and next-frame rearm together, including NULL video. |
| Runner and movie | "HEIGHT 242u", "AUDIO_CAPACITY 801u"; buffers allocate before load; PAL movie key is refused. [VERIFIED: runner/main.c:60-62,1230-1239,1265-1278; runner/movie.c:265-274] | Move effective output allocation after successful load/info, while registering raw observation before load. Use queried extents for native hashes/PNG; accept PAL movie/CLI with existing conflict rules; add opt-in info report. |
| Adapter | "LR_HEIGHT 242u", "LR_MAX_HEIGHT 293u", "LR_AUDIO_CAPACITY 801u"; AV/region and callback dimensions are fixed. [VERIFIED: libretro/libretro.c:62-67,464-477,951-960,1476-1479] | Query/cache successful-load info and effective region; size buffers accordingly; keep max height 293, actual PAL height 292; reset cache/palette on every release/failure. |

**Effective-region host seam:** the public info layout has geometry and rates but no region field. Keep the ABI intact: the runner/adapter can retain the resolved successful load choice plus probe result, or reuse a narrow private effective-region accessor already within their private-core integration. Do not derive region from BIOS size or filename, and do not introduce a public struct tail solely to print region. This is a recommended implementation choice under D-01/D-02/D-10/D-11. [VERIFIED: include/muhtari.h:55-71; src/instance.c:325-330]

### Backwards compatibility and lifecycle

The declaration baseline is "MUHTARI_API_VERSION 6", ABI "1u" and behavior revision "5u". Numeric status "MUHTARI_STATUS_ERR_UNSUPPORTED_REGION = 12" remains. The v3 client explicitly tests PAL refusal, while the released embedding guide qualifies that limit with "until PAL timing is emulated". Change that temporary expectation deliberately; preserve layout/numeric checks and the frozen legacy input behavior. Update API coverage only if declarations change; decide behavior revision according to actual policy, then narrowly update annotations without changing prior frame/raw/PCM digest predicates. [VERIFIED: include/muhtari.h:12,29; src/instance.c:12-13; tests/api/v3.c:31-35,79-96; docs/embedding.md:230-235]

The probe already implements explicit-header region, hash lookup for multiregion and headerless content, dual-region NTSC preference, and NTSC footer fallback. Relevant defining expressions are "(out->a78_tv & 1u) != 0u", "(out->a78_tv & 0x04u) != 0u", and the PAL-only flags comparison in cart_entry_region. Preserve them; use existing test database substitution for controlled hash-list cases. [VERIFIED: src/cart.c:446-451,569-592,651-695; tests/runner/tests.cmake:1149-1163]

Test rejected PAL/NTSC candidates and allocation failures against prior content, selected region, clocks, pending inputs and battery bytes/generation. Power cycle retains region/attachments/committed bytes, settles only due EEPROM programming, and discards unfinished protocol as before. The library load is transactional; the adapter currently releases before load, so its failed load must expose clean NTSC fallback rather than old-session state. [VERIFIED: src/instance.c:173-192,315-369; docs/hardware/controllers.md:135-147; libretro/libretro.c:295-310,1398-1418]

## Exact Timing, Arithmetic and Bounds

### Numeric oracle ledger

These are locked policy/fact values or arithmetic derived from them, not values obtained by asking a new production profile getter. Keep literal test expectations independent of production profile definitions. [VERIFIED: 09-CONTEXT.md,D-03–08,D-12–15,D-18; arithmetic probes E1,E4]

| Quantity | Independent expected value / derivation | Confidence qualification |
|---|---|---|
| PAL ticks/line, lines/frame, ticks/frame | 454; 313; 454×313 = 142102 | Selected documented clock/timing model. |
| PAL master ticks/second | 7093788 | Locked board-label model; not newly measured here. |
| MSTAT clear / set | 16×454+42 = 7306; 308×454+42 = 139874 | Existing software coordinates; test edge−1/edge/edge+1. |
| VBE / final DMA request | 16×454+43 = 7307; 308×454+28 = 139860 | Preserve existing event convention; no request on line 309. |
| Full-frame wrap | 142102 | Last visible pixel x319 occurs at t454 and must retain prior raster ownership before LRC swap. |
| PAL fast handoff | 308×454+184 = 140016 | Explicit fit; zero undocumented DMA residue. |
| Fast first future deadline / elapsed | 139874+142102 = 281976; 281976−140016 = 141960 | Earlier same-origin boundary is past; strict-future selection is essential. |
| BIOS first deadline / elapsed | 139874 / 139874 | Real RES is part of this interval. |
| Fast first PCM count / remainder | floor(141960×4000/591149) = 960; remainder 336960 | First frame is not a full frame. |
| BIOS first PCM count / remainder | 946; remainder 269046 | From zero output remainder. |
| Full-frame PCM increment | 568408000 / 591149 = 961 plus remainder 313811 | Subsequent frames yield 961 or 962, never a fixed 960. |
| Full carry recurrence | 591149 frames; 568408000 samples; remainder 0 | Cheap arithmetic sweep only. |
| TIA / frame recurrence | 456/gcd(456,142102) = 228 frames | 600 full frames exceed two recurrences. |
| 600 full frames from zero remainder | 576918 samples; remainder 301218 | If including fast first interval plus 600 full frames, total is 577879 samples; choose and name the interval explicitly. |
| PAL native picture / packed bytes | 320×292 / 93440 | Native rows 0..291 are rasters 16..307. |
| PAL FPS / PAR / display aspect | 3546894/71051; 1843750/1773447; 147500000/129461631 | PAR is the selected square-pixel convention, not a CRT measurement. |

The Software Guide independently confirms the total PAL raster count and its raster-16/display-end description; it does not remove the documented endpoint/coordinate ambiguity. Retain the selected 292-row convention and translate guide coordinates explicitly. TI's original decoder manual specifies the 14.75 MHz PAL square-pixel reference; the 7800 PAR remains a derivation from that convention and the selected console clock. [CITED: https://7800.8bitdev.org/index.php/7800_Software_Guide] [CITED: https://www.mouser.com/catalog/specsheets/tvp5010.pdf, table 2–1]

### Boot and scheduler proof obligations

Implement a strictly future boundary calculation at fast handoff; do not simply substitute PAL's VBLANK line into the old deadline assignment. Existing fast boot sets "m->frame_deadline = m->maria.origin + MUHTARI__VBLANK_START" and then resets output origin through cart reset, while catching TIA up first. PAL starts after that same-origin boundary. Preserve the CPU/cartridge clock epoch and actual handoff recording semantics. [VERIFIED: src/boot.c:58-83; src/bus.c:354-364; 09-CONTEXT.md,D-05]

Check the PAL snapshot before execution: reset-vector PC, S/X16, A60, Y0, P2C, INPTCTRL16 with two counted gate writes, CTRL60, DPP2707, the two specified FF RAM ranges, RIOT RAM fill, and zero undocumented MARIA DMA residue. These hexadecimal values are the locked D-04 policy verbatim, not a claim that the whole snapshot was measured. Supplied BIOS tests instead exercise actual reset/instructions with both accepted firmware geometries and both console regions. The existing BIOS validator accepts "4096u" and "MUHTARI__BIOS_SIZE_MAX" (16384u); mapping is independent of region. [VERIFIED: 09-CONTEXT.md,D-04,D-06; src/instance.c:161-169; src/boot.c:86-108,116-133]

The overshoot proof survives region selection because it depends on per-access/line tick lengths rather than total lines. Recheck the actual held-cycle and cutoff owners in the implementation review: RDY at most456 ticks, at most two454-tick DMA lines,12 alignment/tail ticks,6 access ticks,8 opcode plus7 interrupt accesses: (456+908+12+6)×15 = 20730 ticks. A longer PAL frame increases normal work, not this instruction-boundary tail. New tests must exercise JAM, DMA-off, pathological DMA, WSYNC/HALT and interrupt entry; measured maxima do not replace this proof. [VERIFIED: src/internal.h:23-43; src/bus.c:245-280; docs/hardware/audio.md:61-74; src/cpu.c:607-638,1499-1507]

### Synthesis phase and FIFO

Use reduced a/d = 4000/591149 for PAL; preserve a/d = 88/13125 for NTSC. Keep split quotient/remainder elapsed-count arithmetic for long uint64 epochs, rather than multiplying an unbounded absolute tick by a. Local frame accumulation is safe: (d−1)+142102×4000 = 568999148 < 2^30. Interpolation numerator is (d−1)×64 = 37833472 < 2^26; compute signed coefficient-difference products in int64 before division. A sample interval spans at most ceil(591149/4000)=148 master ticks, so retaining the existing conservative150 kernel-add limit remains safe. [VERIFIED: arithmetic probes E1,E4; src/synth.c:156-170,206-219,403-407; src/internal.h:27-43]

Future PCM carry is ceil(20730×4000/591149) = 141; maximum logical current interval is962 samples; allocate at least1103 FIFO cells and962 audio-work cells. NTSC remains bounded by801 current/139 future. Preserve the separation between hardware tick and logical horizon, including pending source events exactly at the deadline. Retain observer offset uint16 storage:20730 fits; raw event count uses the same ten-change-per-tick bound, independent of clock rate. Do not allocate591149 coefficient rows. [VERIFIED: arithmetic probe E1; src/synth.c:258-275,299-319,393-419; src/internal.h:45-60,440-444]

### Complete signed-mass and width proof route

Extend the existing summation-by-parts method. Its present test allocates "cumulative[13125][16]" and enumerates88 alignments; directly scaling that array for PAL would be unnecessary. Stream the coefficients/cumulative response for each event tick, or use a compact bounded memoization only if measured cost warrants it. Enumerate all591149 interpolation phases for normalization/Cmax, then4000 consecutive output alignments for signed masses. For sample n and event tick t, q=floor(a t/d), phase=(a t)%d; S(n,t) is the coefficient prefix at tap n−q, with0/Q outside support. Sum positive and negative parts of S(n,t)−S(n,t+1). [VERIFIED: tests/core/test_core.c:285-345; arithmetic probe E1]

Independent probe results using the current kernel and existing midpoint/rounding rule:

| Bound | PAL result | Consequence |
|---|---|---|
| Q23 row sum | 8388608 at every phase | Exact DC normalization. |
| Maximum corrected coefficient magnitude | 7283993 | NTSC's7283992 assertion cannot be copied. |
| Maximum positive mass P | 10865492 | Enumerated over4000 output alignments. |
| Maximum negative magnitude N | 2476884 | P−N=8388608. |
| P+N | 13342376 | Existing NTSC mass13342402 is slightly larger. |
| Complete bound at M18360, including1 PCM LSB | 29203.225608825684 | Below existing conservative29203.6 limit and −1dBFS threshold29203.6194898. |
| Ring-cell conservative bound | 16×150×18360×7283993 = 320961867552000 | <2^49; ring remains int64. |
| Integrated magnitude | P×18360 <2^38 | Q23→Q30×128 remains <2^45. |

[VERIFIED: arithmetic probe E1; comparison proof and thresholds read in docs/hardware/audio.md:109-160; current NTSC literals "7283992", "10865505", "2476897", "13342402" in tests/core/test_core.c:349-369]

Keep M18360, Q30 pole1070934451, kernel64×16 and blocker arithmetic. Independently reconstruct each extremizing0/M stream through the real ring/order path at all alignments, asserting zero clips and the computed integrator endpoints. Recheck max mixed current/products and blocker quotient/remainder bounds as the current test does; retain the existing formula, exact-frequency spectral judge and rejected bypass control. Parameterize physical frequencies and capture chunking for PAL rather than just changing expectations to production output. Probe E1 establishes interpolation feasibility, not the completed production port or every analog/physical model. [VERIFIED: src/synth.c:143-175,179-195,229-250; tests/core/test_core.c:332-369,384-463; docs/hardware/audio.md:170-225]

## Device Conversion and Lower Rows

Use the selected clock only for physical-time models. Literal PAL deadlines are ceil(7093788×400/1000000)=2838 ticks for keypad recovery and ceil(7093788×5/1000)=35469 ticks for the selected EEPROM programming duration. Microchip specifies5ms as a maximum, not a measured constant; selecting it is existing compatibility policy. Preserve unpolled progress, busy NACK and visibility/generation changes only at completion. Test deadline−1/exact/+1 through actual access ticks and reset ordering. [VERIFIED: arithmetic probe E4; 09-CONTEXT.md,D-14; src/ports.c:84-103,363-406; docs/hardware/controllers.md:135-147] [CITED: https://ww1.microchip.com/downloads/aemDocuments/documents/MPD/ProductDocuments/DataSheets/24AA256-24LC256-24FC256-256K-I2C-Serial-EEPROM-DS20001203.pdf, §§6–7]

For paddles preserve the existing resistance discretization, then scale the unrounded fitted duration:

    R = 10000 + floor(position * 1000000 / 65535)
    PAL_ticks = ceil(R * 187747 * 6502639 / (1000000 * 6562500))

The current defining expression is "10000u + (uint64_t)position * 1000000u / 65535u" and its NTSC duration is "(resistance * 187747u + 999999u) / 1000000u". Apply the PAL ratio before the final ceil. At maximum R1010000 the numerator is1233059473976330000 <2^61; denominator6562500000000, and rounding addition still fits uint64. Example expectations: position0→1861 ticks,32767→94877 ticks,65535→187895 ticks. Test changed-pot materialization without resetting charge action, both ports/pots, dump/release and stalled/JAM progression. [VERIFIED: src/ports.c:98-123,732-746; arithmetic probe E4; 09-CONTEXT.md,D-14]

Keep motion cadence "PORTS_MOTION_CADENCE_TICKS 454u" unchanged. Light D/W tick fits also stay unchanged. Extend gun aim acceptance to PAL y291/y292 boundary and NTSC y241/y242 boundary, aperture clipping, rearm across selected frame period, raster307/x319 emission, delayed pulse spanning frame end, color kill, dark/blanked/offscreen and NULL video. The PAL host palette must never feed sensor brightness. [VERIFIED: src/ports.c:13-16,207-241,647-655; src/maria.c:453-478; 09-CONTEXT.md,D-14,D-15]

## Host Geometry and Palette

The legacy palette helpers must continue returning warm NTSC. Add a region-aware internal helper for the two approved warm tables; verify every one of256 PAL RGB entries against the pinned seed and RGB565 bit truncation separately. The approved upstream file begins "license:BSD-3-Clause" and contains "pal_hue0_warm" through "pal_hue15_warm"; no other emulator file is needed. Update existing provenance/notices rather than adding an importer or runtime palette dependency. [VERIFIED: host/palette.c:8-13,97-107; PROVENANCE.md:46-76] [CITED: https://raw.githubusercontent.com/7800-devtools/a7800/7a2afdc1ea08fc331b16b750d8c1f02d4ef62fc8/src/mame/drivers/a7800.cpp]

Runner allocation, hash extents and PNG height must come from effective post-load info. A packed PAL picture is93440 bytes; a padded caller needs accessible extent (height−1)×pitch+width. At allowed maximum pitch65536, this extent is19071296 bytes, safely within32-bit size_t. Canary tests should surround the exact accessible extent and each row's padding; test partial first-frame clearing and NULL output as well. No frame byte-count field exists to detect caller underallocation. [VERIFIED: include/muhtari.h:55-71; src/internal.h:116-119; arithmetic from D-08]

Add opt-in effective metadata output without changing ordinary transcripts or descriptive probe identity. Include region, dimensions, FPS numerator/denominator, PAR numerator/denominator and palette; key spelling is discretionary. Preserve movie-header precedence and explicit CLI conflict rejection. Since movie decoding happens before content load, gun row validation must use the session's explicit/effective region at the point it is known; avoid retaining a242-row parser limit or accepting PAL rows into an effective NTSC session. [VERIFIED: 09-CONTEXT.md,D-10,D-15; runner/main.c:1265-1278; runner/movie.c:265-274; docs/runner.md:263-267]

libretro should cache selected successful-load geometry/region and palette resolution, convert rationals only at the float host boundary, report RETRO_REGION_PAL for PAL, and use292 callback rows while retaining293 maximum. Load-stable region options require reload; changing them mid-run must not change geometry, ticks or info. Palette auto follows effective region while explicit palettes remain presentation choices. The frontend queries AV after successful load, and both AV-update callbacks are restricted to retro_run. No callback inside load and no live region-switch machinery are needed. [VERIFIED: libretro/libretro.c:464-477,925-960,1476-1479; libretro/libretro.h:1330-1365,1533-1554; 09-CONTEXT.md,D-11] [CITED: https://docs.libretro.com/development/cores/developing-cores/#retro_get_system_av_info]

In the same loaded module test NTSC→PAL→unload→no-content NTSC card→NTSC, failed load after PAL, deinit/reinit, both pixel formats, callbacks absent, batch/per-sample stereo duplication, override both directions and reset. Reuse the current loaded-module/mock host and test-only raw/native observation seams; do not make another frontend harness. [VERIFIED: libretro/libretro.c:415-436,1451-1473; tests/libretro/host.c; 09-CONTEXT.md,D-17,D-19]

## Independent Diagnostics, Registration and Quality Admission

### Admission must precede fixture-dependent acceptance

The trusted-fixture function admits only exact newly owned paths and removes those rows before comparing every remaining byte against the independently obtained base manifest. Current regex is "^tests/diag/(activision|absolute|fractalus|battery|controller-light|controller-analog|controller-motion)[.]a78$" and the accepted row shape includes "this-repo", "MIT",64 hex digest and "-". Add only the chosen owned PAL diagnostic name(s), not a wildcard for all future files. Old row order/content/digests remain byte-identical. [VERIFIED: tests/cmake/quality.cmake:115-156]

A behavior-annotation migration is independently constrained to unique exact2→3,3→4 or4→5 transitions, with all other bytes equal. If PAL changes the behavior revision, add only the needed successor transition, prove skipped/downgraded/duplicate/unrelated changes fail, and retain all numeric/digest bytes. Do not assume changing the behavior annotation lets old hash pins float. [VERIFIED: tests/cmake/quality.cmake:159-189]

Exercise the actual producer and unchanged join with a real event base, source SHA, build configuration and current/base artifacts. The producer reads the manifest and sample baseline with git show from the event base and recursively runs that base using the current producer script; the join requires equal environments and authenticated measured archive evidence per native lane. A unit pass of the helper alone does not establish candidate/base comparability. Preserve controls for old-row edit/deletion, unrelated/duplicate/malformed additions, forged identities, failed producer, missing/mixed lane artifacts, wrong source/base SHA and required package evidence. [VERIFIED: tests/cmake/quality_evidence.cmake:25-56,130-170; tests/cmake/quality_join.cmake:8-58]

### Owned numeric PAL diagnostic

Reuse the current mailbox/CRC numeric model; write one compact owned PAL fixture first and add another only if a concrete stimulus cannot fit it. Judge timer/raster observations independently before hashes. Existing driver constants include "FRAME_CAP 600u", "ROWS_MAX 64u", "MB 0x2200u", "LAST_DMA_LINE 258u". Its old controller captures use NTSC-specific picture/PCM arrays, so a PAL case must use queried dimensions/capacity or a scoped PAL capture rather than silently inheriting those arrays. Do not broaden unrelated existing diagnostics. [VERIFIED: tests/diag/driver.c:40-52,604-606,793; 09-CONTEXT.md,D-17,D-18]

A600-frame cap that includes the partial first frame is not600 full frames. Settle the initial interval separately, then execute600 full frames for sustained evidence, and state the audio oracle's initial remainder. Capture actual transition ticks/source order, native lower rows and exact PCM counts in library, actual runner and loaded module, not merely mailboxes simulated by host helpers. Verify all numeric predicates before appending PAL frame/raw/PCM equality rows. [VERIFIED: arithmetic probe E4; 09-CONTEXT.md,D-17,D-18; tests/diag/tests.cmake:1-123]

Retain assembly reproducibility with the existing pinned WASM assembler/header/signing sequence; KEEP creates an admitted fixture only after source is ready. The script constructs "set(result ${name}.a78)" with a config and creates the working directory with "file(MAKE_DIRECTORY ${dir})". Pin owned source identity, register the manifest row, add the honest PAL ledger predicate and regenerate compatibility docs only after the behavior passes. [VERIFIED: tests/diag/assemble.cmake:1-17,34-49,79-83; tests/diag/tests.cmake:301-305; 09-CONTEXT.md,D-18,D-19]

### Register tests with current mechanisms

Use muhtari_add_test in each current subsystem registration file and update the applicable exact inventories. The registration contract is "NAME;TIMEOUT;COST" plus "COMMAND;LABELS"; inventory is "name<TAB>labels" and configure fails on any missing/extra entry. Preserve portable labels and CMAKE_CROSSCOMPILING_EMULATOR for child binaries; do not self-skip missing fixtures or accidentally run native host binaries in a guest lane. [VERIFIED: cmake/MuhtariTests.cmake:1-33; tests/diag/tests.cmake:313-327; docs/testing.md:46-61]

Recommended new behavior cases (names are proposals, not existing registered values):

| Proposed case | Independent fault it detects | Existing owner |
|---|---|---|
| frame.pal_boundaries | Wrong MSTAT edges, lost final DMA, next-line DMA, wrap/stat out-of-bounds | Existing frame/MARIA C tests |
| core.pal_boot | Past first deadline, wrong snapshot, copied NTSC DMA residue or firmware-size region inference | Existing boot/core C tests |
| core.pal_lifecycle | Early region mutation, short-tail default drift, failed allocation/load mutation | Existing API/lifecycle C tests |
| audio.pal_arithmetic | Wrong rational, rounding, coefficient mass or undersized FIFO | Existing synthesis-bound/contracts tests |
| audio.pal_frames | Wrong first/full counts, ownership/source ordering, insufficient-capacity mutation, NULL divergence | Existing audio contracts |
| input.pal_time | Rounded-NTSC paddle conversion, wrong keypad/EEPROM deadline, polling-dependent progress | Existing input/EEPROM C tests |
| input.pal_light | Missing lower rows, wrong wrap, RGB-dependent sensor or NULL-video divergence | Existing input/MARIA emission tests |
| runner.pal | Fixed NTSC allocation/hash/PNG, incorrect info or movie conflicts | Actual runner CMake/movie tests |
| libretro.host.pal | Wrong AV/region/palette/callback height, stale PAL cache after release/failure | Loaded host mock in both formats |
| diag.pal | Agreeing hosts hiding wrong numbers and truncated sustained workloads | Existing driver/assembly/judge/equality pipeline |

These are implementation recommendations under D-17/D-18. Measure each complete expanded workload before freezing TIMEOUT/COST; in particular measure the complete quality controls,600-frame multi-host diagnostic and signed-mass extremizer workload. Keep focused JUnit outputs separate from full-inventory artifacts. [VERIFIED: .planning/preparation/ENGINEERING.md:269-275; .planning/research/workflow-recovery/prompt.txt]

### Acceptance lanes and finite stopping rule

Use existing six-native equality and full native/PowerPC nightly/release paths; applicable WASM comparisons remain advisory under their current authority. Current required aggregate explicitly lists its needs without wasm32, while nightly property10000 remains continue-on-error outside the report aggregate. Do not promote those lanes or demand their unrelated remediation for PAL. Keep the two hosted RetroArch cases unchanged; in-process host tests prove PAL semantics. [VERIFIED: .github/workflows/ci.yml:282-357,695-706; .github/workflows/nightly.yml:180-211; 09-CONTEXT.md,D-18,D-19]

Retain authenticated current/base sums of exactly three native component ZIPs and release members/notices/asset checks. The join explicitly says "report only" for archive_bytes and requires a measured value; DEC.61 removes only the old cutoff. Keep the armed performance contract: "MUHTARI_PERF_ARMED: \"true\"". Package size growth is a comparable observation, not permission to reinstate a quota or repeat hosted runs until an attractive number appears. [VERIFIED: tests/cmake/quality_evidence.cmake:130-142; tests/cmake/quality_join.cmake:48-62; .github/workflows/ci.yml:27-30; .planning/preparation/ENGINEERING.md:277-281]

Stop phase acceptance after independent numeric contracts, NTSC regressions, bounded/null/noalloc/lifecycle tests, exact-source required CI, applicable existing equality, actual frontend checks and authenticated package integrity/measurements pass. Resolve current required contract violations; seed optional improvements. Use at most the existing two review-fix rounds and one gap round, with another gap only on new evidence. [VERIFIED: AGENTS.md:26-55; 09-CONTEXT.md,D-19]

## Suggested Vertical Plan Decomposition

This sequence is recommended because the shared core/host/diagnostic/evidence owners overlap; keep plans sequential. It is a decomposition, not authority to execute during planning. [VERIFIED: 09-CONTEXT.md, Agent's Discretion; .planning/config.json, "parallelization": false]

1. **Prove PAL fixture comparability.** Extend only the named owned PAL admission and any actually needed metadata transition; real producer/join pass and representative identity failures. Do this before a new fixture becomes a required baseline.
2. **Complete the safe PAL execution tracer.** Selected effective region, scheduler/storage, fast/BIOS deadlines, info and minimum synthesis rational/FIFO bounds must land together before PAL is accepted publicly. Include direct numeric tests, narrow v3 expectation migration, buffer/latch/transactionality and NTSC regressions; document behavior with this change.
3. **Run the tracer in the runner.** Post-load buffers/hash extent/PNG, PAL CLI/movie conflicts, opt-in effective info, actual binary tests; preserve default transcripts and observation-before-load setup.
4. **Run it in loaded libretro.** Region/palette options, post-load AV/callbacks and both formats, reset/reload/failure/no-content sequence; preserve two hosted cases and append-only API.
5. **Complete physical-time and lower-raster behavior.** Keypad/paddle/EEPROM conversions, native light emission/wrap, lifecycle/noalloc and hostile stalls; judge independent deadline/raster numbers through existing C tests.
6. **Certify audio and sustained owned evidence.** Full arithmetic carry sweep, all PAL coefficient phases/output alignments, actual extremizers and spectral controls,600 full diagnostic frames through three hosts; pin hashes only after numeric judgment.
7. **Accept portability and released claims.** Add honest PAL ledger/info claims and exact equality rows/inventories; full local workflow, unchanged required/advisory lane authority, exact-source required hosted evidence and current/base package integrity/measurements. Final docs/source review respects finite rounds.

Each substantial slice should measure real candidate/base packages through the existing producer as prescribed by current policy, with final six-native acceptance. Do not make a slice consist solely of speculative tooling or research; each acceptance-oriented slice must show its concrete product/evidence behavior. [VERIFIED: 09-CONTEXT.md,D-19; .planning/preparation/ENGINEERING.md:258-281]

## Don't Hand-Roll

These recommendations follow locked choices and existing mechanisms. [VERIFIED: 09-CONTEXT.md,D-03,D-09,D-13,D-17–19]

| Problem | Do not build | Use instead |
|---|---|---|
| PAL timing | Second run loop or general clock framework | Selected values in existing instance/scheduler owners |
| PCM retiming | Dense denominator-sized filter table, another resampler, per-frame rounding | Existing64×16 kernel/interpolation/blocker and rational carry |
| Palette | Hue rotation or new palette generator/package | Exact approved warm PAL seed and existing host helper |
| Numeric/portability evidence | New oracle framework or hash-only judgment | Existing owned mailbox/CRC driver, C tests, actual runner/module and equality |
| Fixture admission | Global wildcard or rewritten baseline | Named additive relation preserving raw base identity |
| Frontend acceptance | Third hosted scenario or new interaction automation | Existing loaded module tests plus two hosted cases |

## Common Pitfalls

- **PAL acceptance before all bounds change:**313-line events can index263-line storage and962 samples can overflow801-cell work arrays. Grow physical storage and check every direct index; prove FIFO/count arithmetic before enabling load. [VERIFIED: src/internal.h:21,307,444; src/maria.c:560-576; 09-CONTEXT.md,D-03,D-13]
- **Fast first deadline already passed:**139874 is behind140016; scheduling it can produce unsigned elapsed underflow or a zero-work first call. Assert strict future deadline281976 and elapsed141960. [VERIFIED: src/boot.c:77-80; arithmetic oracle ledger]
- **Plausible50Hz/960 output:** selected full-frame fraction gives961/962. First fast partial960 is not evidence for a full-frame constant. [VERIFIED: arithmetic probes E1,E4]
- **PAL interpolation copied from NTSC literals:** every literal denominator and correction midpoint must follow the selected rational; PAL Cmax differs by one. Preserve NTSC operation ordering. [VERIFIED: src/synth.c:156-170; arithmetic probe E1]
- **Physical time conflated with native ticks:** scale unrounded paddle fit; convert keypad/EEPROM; retain454-tick motion and light fits. Exercise all reset/reprojection paths. [VERIFIED: src/ports.c:13-16,84-152,732-746; 09-CONTEXT.md,D-14]
- **Host equality hides wrong geometry/numbers:** independently check native lower rows, AV rationals, every palette entry, CRC mailbox counts and source timestamps before hashes. [VERIFIED: 09-CONTEXT.md,D-07,D-09,D-17]
- **Lifetime mismatch:** failed library load preserves content; failed adapter load leaves fallback. Mid-session region option must not retime reset or run. [VERIFIED: src/instance.c:315-369; libretro/libretro.c:295-310,1398-1418; 09-CONTEXT.md,D-11]
- **Incomplete sustained run:** count600 full frames after separately judged startup; preserve its nonzero remainder when applicable. Never render591149 frames merely to test an integer carry recurrence. [VERIFIED: arithmetic probes E1,E4; 09-CONTEXT.md,D-18]
- **Evidence false economy:** helper-only admission tests, partial reports or passing candidate-only package measurements cannot establish authenticated current/base comparability. Reuse actual producer/join and retain fault controls. [VERIFIED: tests/cmake/quality_evidence.cmake:25-56,148-170; tests/cmake/quality_join.cmake:8-58]

## Code Examples

The following is an implementation skeleton for the independently specified arithmetic, not an existing production helper. Constants are quoted in the numeric ledger: "4000", "591149", "142102", "568408000". Use the selected NTSC pair for the historical path. [VERIFIED: arithmetic probe E1; 09-CONTEXT.md,D-12,D-18]

~~~c
/* Bounded single interval; a/d chosen by successful load. */
uint32_t acc = remainder + elapsed_ticks * a;
uint32_t count = acc / d;
uint32_t next_remainder = acc % d;
/* Reject insufficient capacity here, before committing any state. */

/* Long output epochs: avoid multiplying the whole epoch by a. */
uint64_t samples = (elapsed / d) * a + (elapsed % d) * a / d;
~~~

For the mass proof retain production interpolation semantics, including row64 as shifted row0, signed nearest/ties-away rounding, then exact Q23 correction at tap7 or8 using the existing floor-half threshold. Derive an independent reference expression and compare production output against it at every phase; then generate positive/negative extremizer streams, not expected values from the production profile accessor. [VERIFIED: src/synth.c:146-170; tests/core/test_core.c:285-345]

For test registration use existing muhtari_add_test entries in the current owner, with proposed behavior names, portable label and measured timeout. Do not copy generated test lists or bypass inventories. The exact current signature is "muhtari_add_test(NAME n COMMAND cmd... LABELS l... TIMEOUT seconds [COST c])". [VERIFIED: cmake/MuhtariTests.cmake:11-25]

## Existing Commands and Research Evidence

These commands are concrete existing commands; PAL-specific proposed tests above do not yet exist. Run the complete workflow after implementation, not just these focused checks. [VERIFIED: docs/testing.md:5-26; current CTest discovery E2]

| Purpose | Command |
|---|---|
| Complete local product/test/package gate | cmake --workflow --preset ci |
| Focused existing timing/audio/host checks | ctest --test-dir build/ci --output-on-failure -R '^(core.audio.silence|audio.frames|audio.observer|maria.dma_line.stats|libretro.host.xrgb8888|libretro.host.rgb565|runner.movie_parse|policy.quality.controls)$' |
| Existing diagnostic checks | ctest --test-dir build/ci --output-on-failure -R '^diag[.]' |
| Existing API/layout clients | ctest --test-dir build/ci --output-on-failure -R '^abi[.]api_' |
| Rebuild admitted owned fixtures | ctest --test-dir build/ci --output-on-failure -R '^diag[.]rebuild' |
| Discover exact test registration | ctest --test-dir build/ci -N |

Research evidence only, all URLs N/A-local:

- **E1:** cc -O2 -std=c17 -Isrc -Iinclude /tmp/muhtari-pal-math.c src/synth.c build/ci/libmuhtari.a -o /tmp/muhtari-pal-math && /tmp/muhtari-pal-math — exit0. Independent temporary C17 equations use the current kernel as input, enumerate591149 phases/4000 alignments and the complete carry recurrence. Results are the numeric bounds above; no production edit was made. Reproduce durably by implementing the described streaming reference and extremizer checks in the existing tests; the temporary probe is not a new repository tool.
- **E2:** focused CTest command shown above — exit0,8/8 existing tests,9.18s. This is a baseline on the existing build, not proof that PAL is implemented.
- **E3:** cmake --version; cc --version; node --version; build/pins/wasmtime/wasmtime --version; file-availability tests for the pinned assembler and OpenBIOS inputs — exit0 for the successful probes. Observed CMake4.4.3, Apple Clang21.0.0, Node26.10.0, wasmtime48.0.6; pinned inputs present.
- **E4:** node -e with BigInt arithmetic for first intervals, full frames,600 frames, recurrence and maximum paddle numerator — exit0. Equations and resulting literals are recorded above.
- **E5:** curl -fLsS --max-time30 for the exact approved palette file and pinned OpenBIOS README — exit0 each. The readme distinguishes CC0 BIOS from its non-CC0 included payload; use the existing stripped build only. [CITED: https://raw.githubusercontent.com/7800-devtools/7800OpenBIOS/b8bc745a8744ae3e0d24cc151e63f669f6a87c24/README.txt]

No new public CI run was requested for this read-only research stage. Hosted exact-source implementation acceptance remains execution work.

## Environment Availability

Observed dependencies, not assumptions about future hosted acceptance. [VERIFIED: environment probe E3; tests/built/tests.cmake:7-17]

| Dependency | Available | Version / fallback |
|---|---|---|
| CMake, C compiler | Yes |4.4.3 / Apple Clang21.0.0; native local build present |
| Node arithmetic/GSD seams | Yes |26.10.0 |
| Existing pinned WASM assembler/header tools | Yes | Use existing pinned package; no native cl65/ca65 needed |
| Wasmtime | Yes |48.0.6, existing pin |
| Pinned OpenBIOS input/build | Yes | Existing stripped recipe |
| Loaded libretro test modules | Yes | Focused XRGB8888/RGB565 tests passed |
| Hosted native/PowerPC and advisory WASM execution | Not run for this artifact | Use existing workflow/lane authority during execution |

Missing blocking dependency: none found for the planned local implementation path. Context7 MCP and ctx7 CLI were unavailable; official documentation and the vendored MIT API header supplied the fallback. Graphify is disabled; its status command supplied no graph context. These are lookup observations, not compatibility claims about a missing version. [VERIFIED: init/graph/provider probes this session]

## State of the Art

This phase evolves existing project capability, not its technology stack. [VERIFIED: 09-CONTEXT.md,D-01–03,D-12–13]

| Current released behavior | Planned behavior | Compatibility consequence |
|---|---|---|
| PAL metadata recognized, execution temporarily refused | Existing callers may load PAL | Deliberate v3 behavioral expectation migration; unchanged layout/status12 |
| NTSC-only info/host output | Selected PAL geometry and exact rational rate | Requery info after load; no live timing switch |
| NTSC-only phase/FIFO proof | Both rationals with independently proved bounds | Same kernel/blocker/gain and preserved NTSC digests |
| NTSC palette helper | Legacy helper preserved plus selected warm PAL helper | Native hashes/sensing unchanged |

No newly discovered deprecated API or dependency replacement is needed. The preliminary PAL schematic conflict is already resolved as a qualification in D-16; the sheet was not successfully refetched by the web tool in this session, so this artifact does not add a new schematic observation or upgrade its confidence. [VERIFIED: 09-CONTEXT.md,D-16; web fetch observation]

## Assumptions Log

| # | Claim | Source / section | Risk / disposition |
|---|---|---|---|
| A1 | Retaining85:342 for PAL is a compatibility assumption, not a measured production PAL mix. [ASSUMED] | Locked D-16; synthesis/headroom | Analog balance may differ. Already explicitly accepted in CONTEXT; no repeated owner confirmation. Reopen only with the production evidence specified there. |

The fitted boot, paddle and optical values are documented selected policies, not new training-derived factual assumptions. No additional unverified package, hardware constant or performance target is promoted to a locked choice.

## Open Questions — RESOLVED

Resolved dispositions assign the existing policies and execution obligations below; they do not assert future implementation, workload or hosted acceptance results.

1. **Production PAL mixer/boot/optics calibration — RESOLVED (deferred):** intentionally deferred; no hardware run blocks this phase. Keep confidence labels and exact reopen triggers from D-05/D-16. No calibration implementation enters these plans. [VERIFIED: 09-CONTEXT.md,D-05,D-16,Deferred Ideas]
2. **Behavior/API revision annotation — RESOLVED (actual-change policy):** determine from the actual declaration/hash change under the existing policy, assigned to09-01 Task3 and09-06 Task1 if a transition is required. If advancing, enumerate all annotation consumers and make only the narrow allowed transition; preserve NTSC output predicates. No wholesale rebaseline is authorized. [VERIFIED: 09-CONTEXT.md,D-02,D-19; tests/cmake/quality.cmake:159-189; tests/cmake/hashes.cmake:15-19]
3. **Changed test execution cost — RESOLVED (assigned to execution):**09-01/05 measure the complete signed-mass/extremizer workload,09-07 the actual600-full-frame host workload,09-06 the producer/join controls and09-08 the applicable portability runs before choosing new timeouts. Research proves arithmetic feasibility, not hosted operating cost; complete workload measurement remains an execution obligation. [VERIFIED: .planning/preparation/ENGINEERING.md:269-275; 09-CONTEXT.md,D-19]

## Sources

### Primary/current repository evidence

All source citations above refer to files opened this session, with verbatim discrete values quoted beside their claims. Hardware policies are cited to the locked context rather than misrepresented as new measurements. Principal source owners: instance, boot, frame, internal state, cart probe, bus/MARIA/ports/TIA/synth, public header, runner/movie/palette/libretro, numeric diagnostics and quality producer/join. [VERIFIED: source reads recorded in this session]

### External primary references consulted

- [Atari Software Guide transcription](https://7800.8bitdev.org/index.php/7800_Software_Guide): PAL raster total, display description, MSTAT. Coordinate convention remains project-selected.
- [Maintained A78 format specification](https://7800.8bitdev.org/index.php/A78_Header_Specification): TV byte57, PAL/multiregion flags.
- [Official libretro lifecycle](https://docs.libretro.com/development/cores/developing-cores/#retro_get_system_av_info): after-load AV query; vendored API header cross-checks callback restrictions.
- [TI TVP5010 manual](https://www.mouser.com/catalog/specsheets/tvp5010.pdf): original14.75MHz square-pixel PAL reference, table2–1.
- [Microchip DS20001203Y](https://ww1.microchip.com/downloads/aemDocuments/documents/MPD/ProductDocuments/DataSheets/24AA256-24LC256-24FC256-256K-I2C-Serial-EEPROM-DS20001203.pdf):5ms maximum/write busy ACK behavior.
- [Pinned approved BSD-3 palette file](https://raw.githubusercontent.com/7800-devtools/a7800/7a2afdc1ea08fc331b16b750d8c1f02d4ef62fc8/src/mame/drivers/a7800.cpp): file header and warm PAL table only.
- [Pinned OpenBIOS README](https://raw.githubusercontent.com/7800-devtools/7800OpenBIOS/b8bc745a8744ae3e0d24cc151e63f669f6a87c24/README.txt): BIOS/payload license boundary.

### Lookup seam and confidence

The research-plan seam selected websearch for guide/palette and context7 for libretro. Context7 was unavailable; official docs/vendored header were used as fallback. classify-confidence returned MEDIUM for verified websearch/context7 and LOW for generic webfetch/codebase provider labels; these labels do not turn fetched T4 claims into measurements. Three source digests were stored with the seam-returned MEDIUM tier. Direct source reads and reproducible arithmetic have explicit provenance above; overall research confidence remains MEDIUM because hardware fits and production mix applicability remain qualified. No package-registry existence was used as legitimacy evidence. [VERIFIED: research-plan/classify-confidence/research-store calls this session]

## Metadata

**Confidence:** MEDIUM.
- Stack: existing pinned tools and C17 source verified; no package choice.
- Architecture: existing owners and direct consumers opened; implementation remains to be tested.
- Arithmetic: independent finite reference evaluated successfully; production/extremizer/host checks remain execution obligations.
- Hardware: locked fitted/measured distinctions retained; no newly verified production PAL analog model.

**Research date:**2026-10-25.
**Valid until:** stable hardware facts remain current unless contradictory evidence appears; recheck code seams after intervening source changes. No implementation or PAL acceptance is claimed by this artifact.
