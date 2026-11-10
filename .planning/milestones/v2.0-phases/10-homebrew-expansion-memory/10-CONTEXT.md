# Phase 10: Homebrew and expansion memory - Context

**Gathered:** 2026-10-25
**Status:** Ready for planning

<domain>
## Phase Boundary

Deliver BOARD-04 and BOARD-05: checked BankSet CPU/MARIA ROM and RAM views at
both HALT transitions, plus distinct XBoard and XM RAM/POKEY/HSC profiles,
through the library, runner and libretro. No behavior is implemented and no
requirement is completed by this discussion.

The owner selected all four areas, requested relevant specialist fan-out,
primary-source research and an adversarial synthesis, and delegated adoption of
coherent recommendations. These decisions are agent-selected under that instruction,
not invented interview answers. Three read-only reviews covered BankSet/bus timing,
expansion hardware, and API/hosts/evidence. One interaction pass resolved RAM
declarations, slow-cycle causality, HSC provisioning and sound ownership. Further
expansion stopped when it would not change a choice.

Carry forward integer-only instance state, bounded calls, no allocation after
load, transactional refusal, permissive provenance, three deliverables,
automation-first verification and finite review/gap limits. Use current owners
and small understood helpers; no dependency or general bus/plugin framework is
selected. YM2151, Souper/BupChip, Covox/ADPCM, full XM service hardware and full
OpenHSC remain outside this phase. Whole-library accounting remains Phase 11.
Stop after discussion; planning is a separate command.
</domain>

<decisions>
## Implementation Decisions

### Selection, capability reporting and lifecycle

- **D-01:** Append one load-time expansion selector with Auto, Off, XBoard and XM.
  Zero and an omitted/incomplete optional field mean Auto. Explicit selection
  wins; otherwise use the probe's existing resolved `MUHTARI_DEVICE_XM`, then Off.
  Preserve the current interpretation of any nonzero A78 byte 63 as an XM request,
  including its warning for values other than 1. XBoard needs explicit selection;
  no distinct header encoding was established. Never infer an expansion from a
  filename, firmware size or trial execution, or accept a database-marked module/
  firmware image as a game cartridge. Capture the effective selection only on
  successful load; power cycle retains it and resets volatile hardware controls.
- **D-02:** Keep raw probe identity and resolved cartridge requests distinct from
  effective attachment. Expose the latter through additive size-tagged information,
  runner information and libretro logging. Use matching `--expansion
  auto|off|xboard|xm` and a reload-only libretro option. Say "XM RAM/POKEY/HSC
  subset" and name unavailable features. Off may intentionally suppress the XM
  request, but must not erase independently declared unsupported audio or imply
  the game is verified playable. Probe remains descriptive; executable load refuses
  declared YM2151, Covox/ADPCM and other unimplemented devices with the existing
  appropriate unsupported status, runner exit 77 and a named frontend failure.
  Retain Souper's board refusal. No silent soundless-success compatibility path.
- **D-03:** Preserve old layouts, size minima, numeric constants/statuses and ABI 1.
  Apply the established API/behavior revision policy to the actual additive
  declarations and changed execution semantics. Newly supported hardware must
  work for old zero-tail callers; explicitly changing the old silent acceptance
  of unsupported sound is a documented behavior correction. Judge optional-field
  presence from the complete field end and test sizes before, inside and at that
  end against explicit defaults. Validate geometry, effective attachment, sound
  ownership and conflicts before live-state replacement or allocation for a
  refused configuration. Allocation failure and invalid candidates preserve
  content, execution, firmware, public spans, battery bytes/topology and generation.
  Retain safe staging of borrowed firmware before its backing memory can be freed
  or cleared. Expansion RAM is instance-owned volatile memory, allocated at load
  through the existing allocator; checked size arithmetic and the physical 128 KiB
  capacity establish its bounds, not a new proxy quota.

### BankSet geometry and memory ownership

- **D-04:** Accept equal CPU-then-MARIA ROM halves of 32, 48, 52, 128, 256 or
  512 KiB; the total remains at most the existing 1 MiB ROM limit. Linear halves
  end at $FFFF; switched halves derive bank counts and fixed windows from one
  half, never the combined body. Keep one shared bank register, initially zero.
  The wiki documents through 128 KiB per set; 256/512 are supported producer/
  emulator extensions with explicit confidence limits, not measured cartridge
  capacities. Reject odd, truncated, unsupported or inconsistent geometries
  without fallback that silently discards a valid unsupported v4 declaration.
- **D-05:** Preserve declarations rather than inventing RAM from the BankSet bit.
  Omitted RAM means none; normal 16 KiB means shared CPU/MARIA RAM; HALT RAM
  means two physical 16 KiB halves. The supported matrix is:

  | ROM KiB per set | No RAM | Shared 16 KiB | HALT-selected 32 KiB |
  |---|---|---|---|
  | 32 | Yes | Emulator extension | Yes |
  | 48 or 52 | Yes | Refuse | Refuse |
  | 128 | Yes | Emulator extension | Yes |
  | 256 or 512 | Size extension | Size/shared-RAM extension | Size extension |

  Shared RAM is a deliberate emulator extension supported by distinct metadata
  and emulator documentation; the BankSet hardware wiki describes HALT RAM.
  Do not claim these are identical physical boards. Retain refusal of unsupported
  A8/X2/EXROM/EXFIX combinations rather than inferring them from this matrix.
  In HALT RAM, CPU writes $4000–$7FFF update the CPU half and writes
  $C000–$FFFF update the MARIA half; CPU ROM reads at the latter addresses remain
  ROM. MARIA reads its half at $4000–$7FFF during the effective HALT view.
- **D-06:** Model the electrically selected view: SALLY until the second falling
  phi2 edge after HALT assertion, then MARIA until HALT rises. Derive these edges
  from the actual stretched phase sequence. A `by_maria` flag alone is insufficient:
  MARIA has VBE/display-list helper reads while HALT is high. Existing `halt_stage`
  alone is also insufficient because it is cleared before the batched burst runs.
  Preserve caller identity and access time where needed, with one instance-owned
  authority for the effective view. No mutable global DMA flag. Debug observation
  has a documented logical CPU view and no clocking, bank changes or register side
  effects; actual CPU/DMA transfers obey their electrical timing.
- **D-07:** Preserve causal memory ordering in the current batched MARIA design.
  A bus-level counterexample is slow c0 RIOT access followed by fast c1 cartridge
  write: the latter commits at HALT-fall+10, while the current retroactive first
  DMA read is timestamped fall+8. A timestamp argument cannot undo an applied
  write. This is a code-derived bus-level counterexample, not an executed ROM or
  hardware measurement. Prefer a narrowly justified burst handoff that starts
  memory reads only after the CPU relinquishes the bus and preceding effects
  commit; preserve the all-fast fit, line deadline, DMA abort and existing edge
  owners. Document any changed slow-edge timing as a derived compatibility fit.
  Research must verify the minimum coherent correction before planning it.
  Move to chronological individual fetch scheduling only if a supported path
  requires real interleaving that batching cannot represent. Do not introduce
  write history/rollback, or broadly refit unrelated SALLY/MARIA timings.
- **D-08:** Resolve overlaps explicitly. BankSet $4000 ROM or RAM remains readable
  beside a write-only POKEY there; redirects and mapper writes must not be silently
  swallowed by an unrelated decoder. A 52 KiB half occupies $3000–$FFFF and cannot
  coexist with effectively provisioned HSC in this supported profile. Refuse that
  combination before allocation, including XM Auto-HSC; explicit HSC Off can
  remove the conflict. Expansion overlays otherwise have their documented priority
  over underlying cartridge windows; disabling an overlay reveals the cartridge.
  Preserve MEN/BIOS priority and test boundaries rather than inventing aliases.

### Distinct XBoard and XM profiles

- **D-09:** Implement XBoard separately: 128 KiB RAM in eight 16 KiB banks;
  $0470–$047F are control aliases, bits 0–2 choose the bank, bit 3 enables RAM
  at $4000–$7FFF, bit 4 gates POKEY CPU register decode at $0450–$045F. Controls
  reset to zero. Disabling or changing a bank retains stored RAM. Retain existing
  deterministic power-on RAM fill and distinguish power cycle from the console
  Reset switch. The optional second XBoard POKEY at $0460 is unverified and is
  not part of the supported profile.
- **D-10:** Implement the XM game-facing subset from the audited XM implementation,
  not the older XBoard-style XM paragraph in preparation audio §6. XM uses exact
  $0470, $0478 and $047C writes: $0470 bits 5/6 enable $4000–$5FFF and
  $6000–$7FFF; low/high nibbles of $0478 select their CPU pages, and $047C supplies
  the corresponding MARIA pages. There are sixteen physical 8 KiB pages, with
  bounded offsets through $1FFFF. Bit 3 gates HSC, bit 4 gates $0450 POKEY decode.
  Controls reset to zero. Use the same effective HALT view as BankSet, including
  non-HALT MARIA reads; no independent, inconsistent DMA flag. Disabled windows
  pass through to the cartridge and ordinary cartridge bank writes still work
  outside an enabled overlay.
- **D-11:** State the limits precisely: this is an implementation-backed compatibility
  profile, with medium confidence for physical XM semantics. The primary XM
  developer specification remains inaccessible. Do not synthesize firmware, sticky
  locks or detection success from emulator comments. XM $0470 bits 0–2/7 and
  unsupported $0471/$0472 controls have no implemented effect in this profile;
  unsupported reads retain the established open-bus behavior. ROF aliases, internal
  XM BIOS/top-slot switching, flash, PIA, flat-48-KiB mode, write protection, sticky
  locks and PAL-specific firmware selection are unavailable. Cartridge pass-through
  remains active; no full-XM service or game claim follows from accepting the subset.
  Do not add a new runtime fault protocol merely because a ROM probes absent hardware.

### HSC, POKEY and host memory

- **D-12:** XM Auto provisions the existing HSC storage at successful load; the
  explicit HSC override wins, including Off. Separate physical provisioning from
  $0470 bit 3, which gates both SRAM and ROM decode. The existing raw battery
  layout 1, pointer, length and attachment stay fixed until reload/destroy.
  Gate writes alone change no bytes/generation; hidden HSC writes are ignored;
  re-enabling reveals retained bytes. Power cycle clears expansion controls but
  retains committed HSC/EEPROM data and generation. Standalone HSC keeps its existing
  behavior. Gating both windows is a source-qualified physical inference; the
  reference's always-on SRAM is explicitly a persistence workaround to avoid.
  Preserve signature-only stub limits and user-supplied firmware validation.
- **D-13:** Resolve effective expansion and HSC consistently in core battery preparation,
  runner setup, libretro optional `highscre.bin` discovery and late-option/storage
  checks. Attaching HSC only inside the core would miss firmware discovery and
  create incorrect reload warnings in existing adapter consumers. Discover/read
  optional firmware only for an effectively attached device; invalid requested
  firmware fails before replacing a live machine. Late hardware option changes
  require reload and never reinterpret SAVE_RAM. Avoid a general configuration
  framework; share or duplicate the small stable resolution rule as appropriate.
- **D-14:** An installed expansion POKEY continues clocking and sounding while its
  register window is disabled. Bit 4 gates CPU decode, not chip existence, reset,
  DAC history, timers, mixer normalization or declared IRQ wiring. This is the
  source-backed chip-select model, not a measured audio-mute circuit. Re-enabling
  exposes continued state. Gate writes must not reset POKEY or synthesis. Retain
  stretched phi2 clocks through HALT and both regional profiles.
- **D-15:** Keep the current two-chip identity/source/IRQ bounds. With expansion
  selected, a declared $0450 means that single gated expansion POKEY; it does not
  instantiate a duplicate. A declared $0440 secondary can coexist. Refuse expansion
  plus separate $4000/$0800 primary sound, requests for independent same-address
  owners, and the unverified $0460 expansion chip in this profile. These are explicit
  support limits, not claims that physical hardware cannot combine them. Do not
  silently drop, rename or merge independent chips, or introduce a third chip.
  A concrete required configuration and evidence-backed owner/IRQ model may reopen
  this boundary with corresponding proofs; hypothetical topology flexibility does not.
- **D-16:** Keep physical storage separate from active CPU windows. Existing
  `MEMORY_CART_RAM` covers physical cartridge RAM, including both BankSet halves;
  expansion RAM has a distinct additive physical memory kind if exposed. It is
  neither cartridge metadata RAM nor battery SAVE_RAM. Do not publish banked or
  conditionally gated windows as permanently mapped pointers. In particular, revise
  the current static-map condition that treats non-`BANKED_RAM` cart RAM as fixed:
  it must also exclude HALT RAM. XM-gated HSC descriptors must remain honest as its
  control changes. Retain stable physical spans and validate CPU-map holes through
  the existing rcheevos resolver. Failed load and unload/reload cannot leave stale
  frontend pointers or effective selections.

### Independent evidence, resources and stopping rules

- **D-17:** Use owned numeric diagnostics with independent expected records and CRC,
  then compare native frames, canonical raw audio and PCM through the real library,
  runner and loaded libretro module. Equal hashes alone do not prove ownership.
  Exercise every supported geometry, shared versus HALT RAM, both HALT edges,
  no-HALT VBE reads, all bank/page values and endpoints, and slow-edge ordering.
  Include actual CPU/MARIA accesses, distinct sentinels, redirected stores, hidden
  writes, disabled overlays and integrated RAM+HSC+POKEY. Separate valid metadata
  admission, executable behavior and result comparison; forged/missing/extra or
  wrong expected observations must fail at the relevant existing consumer.
- **D-18:** Test invalid/partial structs, malformed headers, conflicting geometry/
  devices, failed allocation and borrowed media transactionality; neutral startup,
  power cycle, same-value stores, gate toggles, failed reload and cross-instance
  isolation. Prove HSC restore before execution while initially gated off, later
  visibility and unchanged save identity. Prove POKEY gate-off tone/phase continuity
  against a control and ignored disabled-window writes. Exercise unsupported-register
  and hostile bank/control traffic under finite NTSC/PAL calls, with no allocation
  after load and bounded array/index arithmetic. The slow-c0/fast-c1 synthetic
  sequence is bus-level evidence; do not label it a demonstrated instruction stream.
- **D-19:** Extend existing CTest/CMake registrations, owned fixture admission,
  fuzz/property coverage and native/PowerPC equality. Preserve prior passing
  observations unless specific new evidence justifies a narrowly recorded revision.
  Keep the two existing real-RetroArch cases, not a third hosted harness; semantics
  belong in process/module diagnostics. Preserve the armed test-card performance
  guard, authenticated current/base three-component package observations and all
  existing correctness/provenance/ABI contracts. Measure complete changed workloads
  before setting deadlines through DEC.47; add no replacement archive/LOC quota,
  new registry or advisory-lane promotion. Record commands, exits and actual run URLs,
  without committed execution logs. Discussion evidence is not runtime acceptance.
- **D-20:** Apply ENGINEERING §11's bounded architecture review to the changed ownership,
  encapsulation, locality, state/control flow, independent tests and recurring cost.
  Prefer the existing cartridge switch, bus timing owner, chip models and host
  boundaries. Another small understood permissive copy can be cheaper than a
  dependency, but any reused expression still needs its notice and maintenance
  rationale. No package is needed here. Stop specialist expansion after this
  synthesis. Reopen a choice only for an accessible primary specification,
  contradictory hardware observation, or concrete required fixture/consumer that
  falsifies the profile. Optional tooling remains seeds; existing required contract
  violations remain blocking. Preserve the two-review-fix/one-gap limits.

### Agent's Discretion

Choose private field/helper names, checked allocation layout, small module seams,
fixture organization and precise automated assertions within these decisions.
Research must verify source assumptions and the narrow causal DMA correction before
planning implementation. No routine owner judgment, hardware run or new framework
is required. The owner has delegated adoption of the coherent recommendations.
</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**
Phase 10's roadmap entry contains no separate Canonical refs line; the following
paths supply the accumulated scope, source and consumer references.

### Scope, settled choices and evidence policy

- `AGENTS.md` — deliverables, provenance, automation, finite gates and one GSD step.
- `.planning/PROJECT.md`, `.planning/REQUIREMENTS.md`, `.planning/ROADMAP.md` —
  BOARD-04/05, exclusions and Phase 11 boundary.
- `.planning/preparation/README.md`, `.planning/preparation/DECISIONS.md` — source
  index and settled policies, especially DEC.09–11, 15–20, 22–33, 43–47 and 61.
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` §§1–2,4,7 — metadata,
  BankSet, distinct expansion maps, HSC and hostile input.
- `.planning/preparation/A7800-AUDIO.md` §§4,6,8 — POKEY clocks, historical XM map
  conflict and synthesis bounds; §6's XBoard-style XM map is superseded here.
- `.planning/preparation/A7800-SYSTEM.md` §§3–5 and
  `.planning/preparation/A7800-MARIA.md` §4 — stretched cycles, decode and DMA.
- `.planning/preparation/ARCHITECTURE.md` — time, ownership, instance and API contracts.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` §§1–4 and
  `.planning/preparation/CONFORMANCE.md` §§4–7 — maps, hosts, diagnostics and fixtures.
- `.planning/preparation/ENGINEERING.md` §§3–4,8,11,
  `.planning/preparation/GSD-HANDOFF.md` §§3–3.1,
  `.planning/research/workflow-recovery/prompt.txt` and
  `.planning/research/workflow-recovery/RECOMMENDATION.md` — owner-requested bounded
  specialist synthesis, concrete contracts, measured deadlines and evidence.
- `.planning/phases/07-battery-saves/07-CONTEXT.md`,
  `.planning/phases/08-controllers-at-the-pins/08-CONTEXT.md` and
  `.planning/phases/09-pal-games-and-audio/09-CONTEXT.md` — storage/attachment,
  append-only caller behavior, regional timing and causal evidence precedents.
- `PROVENANCE.md`, `ASSET_POLICY.md`, `tests/roms/manifest.txt` — per-file reuse
  licenses and admissible fixture bytes.

### Code and automation entry points

- `include/muhtari.h`, `src/instance.c`, `src/internal.h` — media/info tails,
  transactional load, private staging, battery and fixed chip state.
- `src/cart.c`, `src/bus.c`, `src/maria.c`, `src/pokey.c`, `src/synth.c` —
  geometry/decode, HALT/phase edges, helper fetches and continuous chip clocks.
- `runner/main.c`, `libretro/libretro.c` — selectors, refusal/status output,
  firmware discovery, storage checks and memory descriptors.
- `docs/embedding.md`, `docs/runner.md`, `docs/libretro.md`,
  `docs/hardware/cartridge.md`, `docs/hardware/bus.md`,
  `docs/hardware/maria.md`, `docs/hardware/pokey.md` — released consumer contracts;
  update historical unsupported/never-refused claims with implementation.
- `tests/cart/test_cart.c`, `tests/cart/images.c`, `tests/bus/test_bus.c`,
  `tests/diag/driver.c`, `tests/diag/expected.csv`, `tests/diag/tests.cmake`,
  `tests/libretro/rcheevos.c`, `tests/api/`, `tests/fuzz/` and
  `tests/roms/manifest.txt` — synthetic media, numeric diagnostics, resolver,
  old callers and admission mechanisms.

### External evidence reviewed 2026-10-25

- [BankSet specification revision 1016](https://7800.8bitdev.org/index.php?title=Bankset_Bankswitching&oldid=1016)
  — HALT selection, documented geometries, redirected RAM and HSC conflict.
- [A78 specification revision 1120](https://7800.8bitdev.org/index.php?title=A78_Header_Specification&oldid=1120)
  — declarations, requested addresses, normal versus HALT RAM and XM hint.
- [7800basic producer guide](https://www.randomterrain.com/7800basic.html)
  — BankSet doubling, larger producer sizes and hardware-combination caveat.
- [A7800DS README at 6c108cd9767a](https://github.com/wavemotion-dave/A7800DS/blob/6c108cd9767a/README.md)
  — documentation only: six per-set sizes and shared versus HALT RAM. No GPL
  emulator source was opened or copied.
- [BSD-3 BankSet source at 7a2afdc1ea08](https://github.com/7800-devtools/a7800/blob/7a2afdc1ea08/src/devices/bus/a7800/bankset.cpp),
  [BSD-3 XM source at that pin](https://github.com/7800-devtools/a7800/blob/7a2afdc1ea08/src/devices/bus/a7800/xm.cpp)
  and [BSD-3 XBoard source at dddf4cd1faee](https://github.com/mamedev/mame/blob/dddf4cd1faee/src/devices/bus/a7800/xboard.cpp)
  — per-file licenses audited by specialists; implementation evidence, not physical
  measurement. XM comments and persistence workarounds are not hardware authority.
- [Official libretro header at RetroArch v1.22.2](https://raw.githubusercontent.com/libretro/RetroArch/v1.22.2/libretro-common/include/libretro.h)
  and [MIT rcheevos resolver at 6755915f2bdf6c11c83b3e0e68c4623a39c0dabe](https://github.com/RetroAchievements/rcheevos/blob/6755915f2bdf6c11c83b3e0e68c4623a39c0dabe/src/rc_libretro.c)
  — static descriptor limits and actual consumer behavior.
- [Original XM developer specification](http://www.7800xm.com/XMdevelopers.htm)
  — inaccessible in this review; not claimed as verified evidence. Its absence
  limits claims for service modes, locks, reset circuitry and full XM hardware.
</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets

- Existing A78 v3/v4 and hash metadata, fixed statuses, synthetic cart builders,
  private allocator and pre-commit media staging.
- One cartridge switch, explicit MARIA caller parameter, timed HALT dispatcher,
  bounded per-fetch MARIA burst, two continuously clocked POKEYs and source IDs.
- HSC/EEPROM raw layout 1, load-stable spans, generation token, checked runner
  persistence and libretro firmware discovery.
- Numeric diagnostic mailbox/CRC, all-host drivers, API/linked-client tests,
  rcheevos resolver, exact inventory and native/PowerPC hash consumers.

### Established Patterns

- Validate a complete candidate before publishing any live attachment; successful
  load freezes topology while control registers alter only volatile decode.
- Bus owns advancing time; observers do not advance hardware. Preserve small
  device owners and independent expected values instead of a new generic framework.
- Required checks retain concrete fault controls, actual source identity and measured
  deadlines; advisory lanes remain advisory and unsupported claims stay explicit.

### Integration Points

- Geometry/refusal and reset in `src/cart.c`; effective HALT view at `src/bus.c`
  and `src/maria.c`, including helper reads and slow edges.
- Selection before battery preparation, allocation and commit in `src/instance.c`;
  complete field-end handling in the public media/info paths.
- Runner flags/info and libretro option/HSC discovery/late-option/map consumers.
- Existing diagnostic fixture admission, three-host checks and cross-platform outputs.
</code_context>

<specifics>
## Specific Ideas

The owner's generic fan-out brief is applied through the project-local bounded
`prompt.txt`: independent relevant roles, meaningful alternatives and cheap
falsifiers, one interaction/adversarial synthesis, then stop at diminishing returns.
Preference: another small understood permissive copy can beat another dependency;
keep the dependency tree small and flat without treating copying as license-free.

Useful falsifiers include unequal CPU/MARIA sentinels at both HALT edges, a VBE
read with HALT high, the slow-c0/fast-c1 bus sequence, gated-off HSC restore followed
by re-enable, uninterrupted tone/phase across POKEY decode disable, and map consumers
walking every exposed byte while banks and gates change. These are planned checks,
not claims that new runtime behavior passed during discussion.
</specifics>

<deferred>
## Deferred Ideas

- Full XM boot/service/flash/PIA/locks, YM2151, Souper/BupChip, Covox/ADPCM,
  additional independently owned sound chips, full OpenHSC and measured hardware
  calibration retain later scope. Required counterexamples may reopen a specific
  decision; broad hypothetical extensibility does not.
- Phase 11 owns whole-library accounting and gameplay claims. Synthetic board
  diagnostics establish only their asserted capabilities, not every title's playability.
- Independent release follow-up: PR 35 merged with green required CI 40246033915,
  but v0.8.0 is still a draft. Publication run 40249027723 attempt 1 failed in
  Windows x64 and macOS x64 quality-observation steps, then quality and the required
  aggregate; publication was skipped. Read-only `gh pr view`, `gh release view`,
  `gh run list` and complete-job `gh run view` queries exited 0. This records the
  failed outcome, not release acceptance or a diagnosed cause:
  https://github.com/szTheory/muhtari/actions/runs/40249027723.
  No release repair or retry is performed in this discussion step.

### Reviewed Todos (not folded)

- Phase 2 nightly/ROM-table seeds: historical trigger coverage and API comment
  fixes already landed; remaining optional parser/scan improvements stay seeds
  unless new work demonstrates a current required-contract defect.
- Phase 2 CPU/HALT re-review seeds: frame termination and queue wording already
  landed; optional old test strengthening remains separate. New HALT-edge evidence
  is required by BOARD-04 and does not blanket-close the historical todo.
- Dependabot/publication review group: retain historical disposition; this phase
  does not add a release/CI cleanup workstream. Existing contract violations still
  block their affected work if reproduced.
- WASM promotion: needs its own qualifying evidence and authority; filename overlap
  and Phase 10 expansion coverage do not authorize advisory-lane promotion.
</deferred>

---

*Phase: 10-homebrew-expansion-memory*
*Context gathered: 2026-10-25*
