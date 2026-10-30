# Phase 8: Controllers at the pins - Context

**Gathered:** 2026-10-24
**Status:** Ready for planning

<domain>
## Phase Boundary

Deliver INPUT-01–05: header defaults and explicit device selection; deterministic
extended host input and runner replay; applicable libretro mappings; XG-1, paddles,
keypad, driving, CX22, ST/Amiga mice, SNES2Atari and Mega7800 at the pins.
The core owns emulated devices; hosts own physical input and presentation.
This discussion implements no controller and completes no requirement.

The owner selected all relevant areas, requested specialist fan-out and adversarial
review, and explicitly delegated adoption of coherent recommendations. These are
agent-selected decisions under that instruction, not invented interview answers.
Three read-only reviews covered host/API/replay, emitted-light/analog/matrix timing,
and quadrature/adapter protocols. A final cross-area pass added lifecycle, unpolled
edge and raster-boundary cases; expansion stopped when it added no material choice.

Carry forward three deliverables, permissive provenance, integer-only instance state,
bounded calls, no allocation after load, automation-first verification and finite
review/gap limits. Prefer existing mechanisms and small owned code over dependencies.
PAL completion stays in Phase 9; expansion boards, AtariVox speech, hardware-calibrated
accuracy, recording and save states retain their existing later scope. Stop after
this discussion; planning is a separate command.
</domain>

<decisions>
## Implementation Decisions

### Input API, selection and lifecycle

- **D-01:** Add a separate size-tagged extended input structure and setter. Preserve
  the released 24-byte `muhtari_input`, old setter, constant values, field masks,
  unsupported-device refusals and legacy automatic ProLine fallback. Frozen
  `tests/api/v3.c` asserts both the size and explicit paddle refusal; do not rewrite
  those expectations to make a changed old API pass. New hosts opt into the extended
  setter, while new frozen/linked-client coverage proves the additive surface.
  Apply the existing API/ABI/behavior version policy. Preserving the old input API
  does not excuse missing TIA hardware: evidence-backed D6/D7 corrections follow
  D-17's narrow regression/version policy.
- **D-02:** In the extended path, resolve explicit per-frame device selection, then
  the successful load's explicit port override, then A78 header defaults. Distinguish
  Auto from explicit unplugged using existing constants; do not rewrite descriptive
  cartridge metadata to report a fallback. Newly supported explicit load overrides
  install the selected device with neutral controls immediately. Extended Auto must
  activate the actual resolved device before the first emulated frame; runner and
  libretro submit an initial extended packet, including when no controls are held.
  The legacy setter retains its released automatic semantics. Validate the entire
  request before replacing pending input, including both ports and resolved-device
  payloads; unknown masks/flags, invalid ranges and nonzero inapplicable fields fail
  transactionally. Missing supported tails have documented neutral defaults.
- **D-03:** Keep one VBLANK-applied packet: the latest accepted submission replaces
  earlier pending submissions. Hold buttons, console switches, keypad keys, paddle
  positions and gun aim until replaced; transfer relative motion exactly once at
  the latch and clear its pending contribution. A frame with no new submission must
  not replay its delta; already accepted finite device motion may finish. State
  units and signs explicitly: motion +X right/+Y down, absolute pot travel independent
  of a game's polling loop, native gun coordinates and a separate off-screen flag.
  Keep trigger separate from illumination and two paddle positions/buttons per port.
  Do not add subframe host events, timestamps, an input queue or recording machinery.
- **D-04:** Nonbattery device changes take effect at VBLANK and reset the replaced
  device's volatile protocol, motion, charge and pending-light state. Repeated packets
  for the same device preserve legitimate in-flight state. Define neutral startup,
  successful reload and power-cycle behavior; console Reset remains an input switch,
  distinct from power cycle. Phase 7's at-most-one EEPROM identity/port, battery
  pointer/length/layout and bytes remain frozen until reload. Reject attempts to
  add, remove, move or replace that attachment before changing either port's pending
  input. Test controller plus EEPROM in both legal placements; controller selection
  cannot reinterpret SAVE_RAM or suppress committed programming.

### Light gun and TIA input timing

- **D-05:** Sense the native MARIA signal at pixel emission, before host palettes,
  scaling, filters or output buffering. Qualify light by native luminosity and the
  aim window, then produce the delayed sensor response. Later palette writes or
  darkness cannot change historical illumination. Bright DMA-disabled BACKGRND is
  still visible and can trigger; actual blanking and off-screen aim cannot create
  illumination. A response already scheduled from a visible sample follows its
  defined pulse duration across later blanking. Preserve the emission raster when
  a delayed response crosses a line/frame boundary, including x=319 at the next line's
  tick zero. Null video buffers must give identical sensor behavior. Geometry-only
  detection and inspection of a completed framebuffer do not meet INPUT-02.
- **D-06:** Start research from the preparation's D=64 master ticks, W=30 ticks and
  L=9 lines as an explicitly unmeasured compatibility model, not a calibration.
  Research must define window anchoring, pulse composition and native-luma threshold
  T, document deterministic defaults and rounding, and derive independent vectors
  before pinning expected outputs. Named cited parameters belong to instance-owned
  emulation behavior; host display palettes cannot change them. No game-specific
  offsets, polling-loop heuristics or claimed zero-error retail calibration without
  measurements. Public owned diagnostics establish the chosen model; private retail
  ROMs and an owner-console run are not prerequisites.
- **D-07:** TIA owns VBLANK D6's INPT4/5 low latches and D7's INPT0–3 dump control;
  controller models supply pin levels. Research must settle the exact enable,
  repeated-write, clear and release truth tables from primary documentation and test
  their interaction with INPTCTRL and RIOT two-button control. Catch up relevant
  timed pin events through the access tick before reading or applying a write under
  the existing bus ordering. Low pulses must latch without CPU polling, including
  under HALT/JAM. Peeks remain observers and must not clock or commit device state.
  Preserve D7-only INPT driving/open-bus bits, existing decode and independent TIA
  audio; do not infer MARIA blanking from a TIA bit without evidence.
- **D-08:** Model paddles with a small integer elapsed-time charge/discharge state
  driven by D7 dump/release and the actual pot setting, not a register shortcut.
  Research must select and label physical or fitted charge endpoints/transfer,
  rounding, discharge and mid-charge setting-change behavior; elapsed charge cannot
  depend on how frequently software polls. Cover both pots/buttons, endpoints,
  repeated dump, one tick before/at/after charge deadlines, frame crossings, changed
  positions and reset. Never scale the physical pot range to a particular game's
  polling window. Host sensitivity is separate from core electrical behavior.
- **D-09:** Model keypad row selection from effective RIOT output/DDR pins and columns
  on INPT0/1/4 or INPT2/3/5. Use the preparation's immediate-low/timed-high pot-column
  model as a labelled derived starting point; digital trigger-column behavior stays
  distinct. The guide's 400 microseconds is elapsed time, approximately 2,864 NTSC
  master ticks rounded upward, not 716 executed CPU bus cycles. Slow cycles and HALT
  must not stretch that physical interval. Research must resolve row polarity, idle
  or multiple driven rows, D7 overlap, key ordering and electrical/retained-charge
  ghosting. Do not present the preparation's reported three-key limit as a universal
  hardware fact or invent diode isolation. Test early/late release and multi-key
  cases against independently documented expectations.

### Motion and adapter protocols

- **D-10:** Use compact fixed-size per-axis motion state and timed pin transitions.
  Driving and mice use device-specific quadrature tables; CX22 uses direction plus
  motion toggles. Define one motion unit, forward/reverse phase, both axes, initial
  phase, buttons and ST/Amiga wiring explicitly. ST documentation labels a positive
  waveform RIGHT/UP, so API +Y down needs a deliberate conversion. Do not reuse a
  single unqualified Gray table for every device or advance phase on reads.
- **D-11:** Bound accepted displacement, residual motion and event work; saturate
  excessive motion with documented discarded excess rather than accumulating an
  unbounded backlog. Latest pending packets replace each other; a latched delta is
  consumed once. Research must justify cadence, saturation and minimum edge spacing
  against emulated-time/operating-cost objectives and comparable measurements under
  ENGINEERING §11, with overflow-safe arithmetic for signed extremes. Preserve phase
  when idle. Device cadence is independent of polling and continues through CPU
  stalls. Chronological PA7 transitions must reach RIOT's edge detector even without
  SWCHA reads; projecting only the final phase can lose an even number of edges.
  Use existing deadline/catch-up mechanisms or prove an equivalent bounded summary;
  no general event framework or per-host-delta unbounded loop.
- **D-12:** Own finite SNES2Atari and Mega7800 state machines observing effective
  ORA/DDRA pin transitions at the bus data tick. Reads/peeks only observe response
  pins; identical writes cannot synthesize edges. Preserve the other port's nibble,
  normal device behavior outside extended scans, and two-button electrical context.
  SNES2Atari wires CLOCK/LATCH/DATA to pins 1/2/6; research must reconcile exact
  latch/shift edges and the documented detection tail, including the seventeenth
  bit. Mega7800 enters scanning through pins 1/2, returns six two-bit observations
  on pins 3/4 and advances through its documented strobe; research must establish
  identification/class/button, completion and restart tables. Do not assume every
  low/low write restarts the scan. Both pinned CC0 drivers explicitly support both
  ports: a port-2 sample is not a hardware restriction. Cover partial/extra clocks,
  DDR-only edges, every returned bit, reset and EEPROM coexistence. Reuse driver
  facts; a code seed requires the existing provenance/notice process.

### Runner and libretro

- **D-13:** Extend existing `--port1`/`--port2` device names and add movie format 2 for
  new typed payloads. Preserve movie 1 grammar, meanings, failures and legacy replay
  behavior. Movie 2 records explicit per-port devices and complete deterministic
  per-frame control state, with relative fields interpreted once; no hotplug record,
  recorder or subframe stream. Preserve whole-file validation and media/behavior
  identity checks before the first frame, strict integer/range/mask validation,
  deterministic line/column faults and CLI/header conflict handling. Reject invalid
  late lines before any emulation or save-side effect.
- **D-14:** Reuse the immutable validated movie bytes and sequentially decode into one
  extended packet, or prove an equally compact bounded representation. Do not
  multiply the million-frame limit by a newly enlarged public structure. Retain
  existing total-file, frame-count and header bounds; derive the version-2 line bound
  from its largest accepted canonical record and measure parser/allocation work.
  No streaming external input, generic parser package or new core allocation path.
- **D-15:** Advertise libretro device subclasses with appropriate base abstractions;
  poll using base IDs. Use absolute LIGHTGUN SCREEN_X/Y and explicit off-screen/reload
  information, never deprecated relative gun coordinates. Convert coordinates using
  bounded integer arithmetic over reported native geometry, including -32768; do
  not hard-code host window dimensions or complete PAL here. Use analog axes for
  absolute pots, relative MOUSE input for mice/CX22 and applicable driving controls,
  RetroPad buttons for adapters, and a documented keypad mapping. Poll motion once
  per frame. Publish complete device-specific mappings/descriptors and refresh them
  on selection; unsupported physical abstractions are documented limitations rather
  than invented device I/O.
- **D-16:** Preserve ProSystem-compatible ProLine/joystick layouts and legacy console
  shortcuts. Extended device buttons must not simultaneously trigger console actions:
  adapter Start/Select/X/shoulders cannot also Pause/Select/Reset/change difficulty.
  Map SNES buttons conventionally and use a documented conventional six-button
  layout for Mega7800. Keep existing difficulty options. For console momentary
  switches unavailable from another selected ordinary port, use narrowly scoped
  documented nonconflicting host controls, preferring unused RetroPad controls or
  a chord whose constituents are withheld from controller input. Research must
  publish a complete reachable mapping before implementation, without a new menu
  system. Unknown frontend IDs fail visibly or follow a documented safe fallback;
  late storage-changing callbacks still require reload and cannot mutate SAVE_RAM.

### Evidence, resource limits and architecture

- **D-17:** Add independent pin/timing vectors and owned executable numeric diagnostics
  for every newly claimed device family. Derive expected records from the documented
  model before hashing output; a successful exit or two equally wrong hosts is not
  a verdict. Exercise library, actual runner replay and in-process libretro mappings,
  including both ports, meaningful control changes and native frame/audio equality.
  Test threshold/deadline boundaries, absent video buffers, unpolled D6/PA7 events,
  palette changes during gun latency, rightmost/last-visible pixels, matrix history,
  opposite/zero/extreme motion, malformed replay, rejected-packet pending-state
  retention, device changes, reload/reset, instance isolation and no allocation after
  load. Extend existing deterministic property/fuzz harnesses with actual new devices
  and bounded cases. Every behavior test belongs to `cmake --workflow --preset ci`
  with exact inventories and current lane authority. Keep old predicates/pins; any
  intentional D6/D7 hardware correction requires specific evidence, a narrow
  behavior-version transition and explicit affected expectations, not blanket repins.
- **D-18:** Keep the existing two hosted RetroArch cases and 300-test-line bound;
  detailed controller semantics belong in in-process/runner tests. Public CI needs
  no private games, firmware or owner hardware. Diagnostic evidence proves the
  selected device model, not universal title playability or measured XG-1 accuracy.
  Preserve lag semantics: only CPU reads of the existing input-register set count,
  not automatic device events, peeks or DMA. Publish truthful capabilities only
  after their executed evidence. Graduate behavior into hardware, embedding, runner,
  libretro and testing docs together; keep README and public header within budgets.
- **D-19:** Reuse the existing ports owner, RIOT/TIA/MARIA seams, plain C tests,
  diagnostics and vendored libretro header. No dependency or generic controller/bus
  abstraction is selected. Apply ENGINEERING §11's ownership, coupling, control-flow
  and test-independence review; retired maintenance-line quotas do not return.
  Extend trusted fixture comparability only for named owned controller diagnostics,
  preserving every trusted old row, uniqueness, raw identity, actual producer/join
  controls and normal provenance admission. Existing admission covers board/save
  fixtures, not arbitrary controller additions. Before relying on fixture-dependent
  acceptance, prove this narrow prerequisite through the real consumer.
- **D-20 (superseded separately on 2026-10-24; DEC.61):** Require attributable
  current/base three-component ZIP measurements on comparable native lanes and
  final six-native acceptance. Retire only the historical 399,937-byte cutoff;
  it was exceeded, not passed. The owner requested and authorized the separate
  evidence-backed purpose review in workflow-recovery/RECOMMENDATION.md.
  Preserve required typed, authenticated archive observations, joined JSON peaks,
  exact three-ZIP content/notices/asset checks, and all runtime/ABI/provenance/test
  and lane-authority contracts. No replacement quota or advisory promotion.
  Preserve the armed test-card +1% performance gate; dispatch repeatability proves
  only repeatability, and controller operating cost remains separately measured.
  Intermediate slices keep local full-ci/package evidence; final08-14 owns the
  complete exact-source hosted matrix. Earlier hosted probes need a concrete
  platform/integration question, not merely another archive-size report.

### Agent's Discretion

Exact additive names, struct layout, internal source splits, keypad/adapter host
button tables, diagnostic count/names and plan ordering are delegated within these
contracts. Research resolves factual ambiguities and chooses justified numeric model
parameters before implementation expectations are frozen. It must distinguish
primary hardware facts, permissive-driver behavior and fitted compatibility policy.
No further owner interview is needed for routine engineering choices. This context
does not claim the future measurements, diagnostics or package acceptance passed.
</decisions>

<canonical_refs>
## Canonical References

Downstream agents must read the relevant sections before planning or implementing.

### Scope and settled contracts

- `AGENTS.md`, `.planning/PROJECT.md`, `.planning/REQUIREMENTS.md` INPUT-01–05 and
  `.planning/ROADMAP.md` Phase 8 — scope, deliverables, acceptance and step boundary.
- `.planning/preparation/README.md`, `.planning/preparation/DECISIONS.md` — index;
  especially DEC.09–11,15–20,22–27,32–33,40,43–46,51–58,60.
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` §§1,3–4,7 and source
  table CART.08–11,23,27–28,32,34,40 — header policy, wiring, models and excluded source.
- `.planning/preparation/ARCHITECTURE.md` §§2–7 and
  `.planning/preparation/A7800-SYSTEM.md` §§4–6 — time, ownership, input and bus ticks.
- `.planning/preparation/A7800-MARIA.md` §§4,6–8 — emitted raster, palette/luma,
  native geometry and the separation from host presentation.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` §§1–5 — host input conventions,
  runner/replay, descriptors, two hosted cases and later scripting boundaries.
- `.planning/preparation/CONFORMANCE.md` §§4–7,
  `.planning/preparation/ENGINEERING.md` §§3,8,11, `PROVENANCE.md`, `ASSET_POLICY.md`
  and `.planning/LESSONS.md` — independent diagnostics, legal bytes and bounded review.
- `.planning/phases/07-battery-saves/07-CONTEXT.md` D-04–09,D-15–18 and
  `.planning/phases/07-battery-saves/07-VERIFICATION.md` — immutable EEPROM topology,
  timed pin seam, battery/host contracts and current acceptance evidence.
- `.planning/phases/06-retail-cartridge-boards/06-CONTEXT.md` current D-16–18 and
  `.planning/phases/05-pipeline-tune-up/05-CONTEXT.md` D-07–13,D-35 — architecture
  review, fixture identity, independent archive limit and retained lane authority.
- `docs/embedding.md`, `docs/runner.md`, `docs/libretro.md`,
  `docs/hardware/controllers.md`, `docs/hardware/riot.md`, `docs/hardware/tia.md`,
  `docs/hardware/maria.md`, `docs/hardware/bus.md`, `docs/hardware/cartridge.md`,
  `docs/testing.md`, `docs/emulation-tests.md`, `docs/quality.md` — shipped contracts.

### Code and automation entry points

- `include/muhtari.h`, `src/internal.h`, `src/ports.c`, `src/riot.c`, `src/tia.c`,
  `src/maria.c`, `src/bus.c`, `src/frame.c`, `src/instance.c`, `src/cart.c`.
- `runner/main.c`, `runner/movie.c`, `runner/movie.h`, `runner/exit_status.h`,
  `libretro/libretro.c`, `libretro/libretro.h`, `libretro/muhtari_libretro.info.in`.
- `tests/api/v1.c`, `tests/api/v2.c`, `tests/api/v3.c`, `tests/api/v4.c`,
  `tests/api/v5.c`, `tests/input/test_input.c`, `tests/input/tests.cmake`,
  `tests/riot/test_riot.c`, `tests/core/test_core.c`, `tests/alloc.h`.
- `tests/runner/test_movie.c`, `tests/runner/test_runner.c`, `tests/runner/tests.cmake`,
  `tests/libretro/host.c`, `tests/libretro/tests.cmake` — actual host consumers.
- `tests/diag/driver.c`, `tests/diag/assemble.cmake`, `tests/diag/tests.cmake`,
  `tests/diag/expected.csv`, `tests/roms/manifest.txt`, `tests/compat/ledger.csv`,
  `tests/prop/test_prop.c`, `tests/fuzz/tests.cmake`,
  `tests/retroarch/run_retroarch.cmake` — semantic evidence and scope boundaries.
- `tests/cmake/quality_join.cmake`, `tests/cmake/quality.cmake`,
  `tests/policy/quality.cmake`, `tests/cmake/lane_equality.cmake`, `quality.json`,
  `.github/workflows/ci.yml`, `.github/workflows/nightly.yml`, `pins.txt` — trusted
  fixture/package observations, equality and current gates.

### External evidence reviewed 2026-10-24

- [Atari 7800 Software Guide](https://7800.8bitdev.org/index.php/7800_Software_Guide)
  — primary guide mirrored by 8BitDev; keypad wait, controller registers and visible
  BACKGRND. Distinguish guide facts from the preparation's derived timing model.
- [Atari TIA input-port guide, section 12](https://www.blarworld.net/stella/12.0%20Input%20Ports.html)
  — primary programmer documentation mirrored online; dump/latch facts, not emulator
  source. Verify the complete truth tables rather than borrowing another console's timing.
- [Original Atari ST engineering hardware specification, section 5.1](https://www.temlib.org/AtariForumWiki/index.php/Engineering_Hardware_Specification)
  — original document transcription; pin-labelled mouse wiring and directional waveform.
- [Atari CX22 field service manual](https://www.atarimania.com/documents/Atari-CX22-Trakball-Field-Service-Manual.pdf)
  — primary source identified; full PDF fetch timed out during discussion. Research
  must inspect the output table before locking polarity; it was not verified here.
- [SNES2Atari original CC0 design/driver](https://7800.8bitdev.org/index.php/SNES2Atari)
  — original wiring, sample scan and licensing; its port-2 example is not a restriction.
- [Pinned CC0 SNES2Atari driver](https://github.com/7800-devtools/7800basic/blob/e7c18d2ecdc6/includes/snes2atari.asm),
  [pinned CC0 Mega7800 driver](https://github.com/7800-devtools/7800basic/blob/e7c18d2ecdc6/includes/mega7800.asm)
  and [licensing overview](https://github.com/7800-devtools/7800basic/blob/e7c18d2ecdc6/LICENSE.txt)
  — both-port driver behavior; implementation evidence, not hardware measurements.
- [Official libretro input API](https://docs.libretro.com/development/input-api/) and
  [upstream MIT API header](https://raw.githubusercontent.com/libretro/libretro-common/master/include/libretro.h)
  — base abstractions, subclass polling and gun/mouse semantics. Reuse the vendored
  header where sufficient; no dependency update is selected.

GPL/LGPL and unlicensed emulator source remain excluded. In particular, the
preparation's unknown-license `lightgun.sv` is not an available seed or reference.
</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets

- `src/ports.c` owns pending/applied input and the timed EEPROM state machine;
  `src/riot.c` already notifies actual-tick ORA/DDRA changes and owns PA7 flags.
- `src/tia.c` supplies D7-only INPT reads with open-bus preservation; it lacks
  the dump/latch behavior needed here. Preserve its independent audio lattice.
- MARIA already renders spans up to writes; use that emission boundary for light
  sensing. Pixel computation currently depends on non-NULL caller rows, which must
  not become a hardware dependency. Last-pixel emission crosses the line origin.
- Existing pure movie validation, host mocks, API clients, numeric mailbox/assembly
  pipeline, manifest and seeded property/fuzz harnesses avoid new tools.

### Established Patterns

- Fixed-width size-tagged additive APIs, validate-before-mutation, instance-owned
  bounded state, VBLANK input and no allocation after load.
- Phase 7 attaches storage before host publication and prohibits later topology
  changes. Device selection has to preserve this stronger lifecycle contract.
- Semantic vectors are independent of output hash pins; existing authenticated
  candidate/base joins and package limits remain authoritative.

### Integration Points

- Extend load-time device validation and modern port resolution together. Current
  `battery_prepare` refuses new explicit devices; legacy automatic fallback is in
  `muhtari__ports_device`. Retain an explicit load choice across extended Auto packets.
- Integrate emission/delayed sensor and quadrature transitions with existing time
  ordering so TIA latches and RIOT PA7 see events even when software does not read.
- Runner currently materializes 24-byte version-1 frames; avoid multiplying that
  allocation by the larger extended packet. Version-1 callers remain intact.
- libretro currently has one static ProSystem input descriptor table; adapter
  buttons collide with console shortcuts unless selection changes the mapping.
</code_context>

<specifics>
## Specific Ideas

Review covered players, embedders, homebrew drivers, C/ABI ownership, rendering
causality, hardware confidence, replay security, accessibility of host controls,
automation validity, portability, licensing, package/performance cost and maintenance.
UI redesign, distributed-system abstractions and unrelated framework choices add no
value here. The main footguns are frozen legacy input, repeated relative motion,
buffer-dependent gun sensing, delayed samples using the wrong raster/palette,
CPU-cycle analog waits, read-clocked adapters, skipped PA7 edges and conflicting
console shortcuts. No new dependency is justified by these responsibilities.
</specifics>

<deferred>
## Deferred Ideas

Hardware-calibrated thresholds/cadence and broader accuracy oracles remain later
work; unmeasured model parameters are documented here. PAL execution, expansion
boards, AtariVox speech, recording, save states and general input tooling retain
their existing roadmap placement. No additional hosted RetroArch case is selected.

### Reviewed Todos (not folded)

- `.planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md`
  — broad keyword match; residual generator/firmware-scan seeds add no controller
  requirement. Preserve already completed nightly/header corrections.
- `.planning/todos/pending/2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md`
  — unrelated CI/release follow-up; accepted Phase 5 work remains authoritative.
- `.planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md`
  — separate lane-promotion history, not earned by controller implementation.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md`
  — substantive run-loop items are already closed; remaining optional test seeds
  do not become INPUT requirements. Existing required contract violations still block.
</deferred>

---

*Phase: 08-controllers-at-the-pins*
*Context gathered: 2026-10-24*
