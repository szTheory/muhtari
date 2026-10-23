# Phase 6: Retail cartridge boards - Context

**Gathered:** 2026-10-23
**Status:** Reconciled 2026-10-23; comparability prerequisite06-10 ready, board plans incomplete

<domain>
## Phase Boundary

Deliver BOARD-01–03: Activision banking with equivalent mapped behavior for identified AM/OM dumps,
Absolute fixed/switched windows and initial banks, and Fractalus physical RAM aliases. Exercise the
boards through the library, runner and libretro, retaining previous frame/audio predicates and bounded,
transactional loading. This step captures decisions; it implements no board and completes no requirement.

The owner explicitly selected all relevant decision areas, requested parallel research and adversarial
tradeoffs, and delegated adoption of the synthesized recommendations. Decisions below are agent-selected
under that instruction, not invented interview answers. Prefer existing code and permissive seeds over
dependencies. Preserve settled project choices and stop after this discussion step.

PAL execution, saves, controllers, BankSet, XBoard/XM and Souper remain in their assigned later phases or
milestones. Recognizing a PAL image is separate from executing it; retain the current region refusal.
</domain>

<decisions>
## Implementation Decisions

### Activision layout and identity

- **D-01:** Use OM order, matching the bundled softlist, as the installed Activision representation.
  Preserve which lookup pass matched. Only an identified half-swapped Activision match authorizes swapping
  each 16 KiB bank's 8 KiB halves. Normalize the new private allocation before replacing the loaded cart;
  never modify caller bytes, normalize twice or add a second runtime mapper for the other dump order.
- **D-02:** Retain hashes of the supplied full image and headerless bytes under their current runner meanings,
  including the existing treatment of trailers. Normalization does not redefine raw identity, movie identity
  or RetroAchievements identity. Expose identified half-swap orientation through an additive descriptive bit
  in the existing probe flags, and report it through existing-style runner probe output. No new hash database,
  layout override API/CLI, public peek, hash-filled struct extension or dependency is needed.
- **D-03:** An unknown image explicitly declaring Activision uses documented assumed OM order, with an
  existing-style warning that this is assumed rather than detected. Do not guess from filename, reset vector,
  footer plausibility or trial execution. An unknown headerless image retains the existing probe fallback;
  its size alone does not identify an Activision board. Modified AM images without a hash match must be
  converted externally to the documented convention. Keep hash → validated v4 → resolved v3 precedence,
  controller/region rules and raw header diagnostics.

### Board behavior and evidence confidence

- **D-04:** Activision accepts exactly 128 KiB. With OM bytes, each 8 KiB CPU window begins at the following
  image offset. Bank selection starts at zero. The switched pair is reversed within its 16 KiB OM bank;
  treating it as one contiguous 16 KiB span is incorrect.

  | CPU window | OM image offset |
  |---|---|
  | $4000–$5FFF | $18000 |
  | $6000–$7FFF | $1A000 |
  | $8000–$9FFF | $1C000 |
  | $A000–$BFFF | bank × $4000 + $2000 |
  | $C000–$DFFF | bank × $4000 |
  | $E000–$FFFF | $1E000 |

  CPU writes at $FF80–$FF8F select address bits A2–A0; written data is ignored. Reads and peeks do not
  select banks. Do not broaden this to every $E000–$FFFF write on the basis of one emulator.
- **D-05:** Absolute accepts exactly 64 KiB. $4000–$7FFF selects image bank 0 or 1; $8000–$FFFF holds the
  fixed upper 32 KiB. Start at bank zero. Only a CPU write at $8000 selects: D0 selects zero; otherwise D1
  selects one; otherwise retain. Thus values 0/1/2/3 mean retain/0/1/0. Ignore higher data bits. Do not adopt
  the alternative any-A15-write decoder without new evidence.
- **D-06:** Fractalus accepts exactly 32 KiB ROM at $8000–$FFFF and allocates exactly 2 KiB physical RAM.
  For $4000–$7FFF use physical index `((addr & 0x0E00) >> 1) | (addr & 0x00FF)` for reads and writes.
  Adjacent 256-byte pages alias; all 2,048 physical bytes have eight aliases across the window. A8 and the
  upper mirror bits are disconnected; an ordinary `ram_size - 1` mask on the uncompressed address is wrong.
  The v4 name “8K EXRAM/A8” does not establish physical 8 KiB capacity. Research must explicitly reconcile
  accepted v4 A8 declarations with this board and leave undocumented combinations unsupported rather than
  silently treating every A8 combination as a supported SuperGame variant.
- **D-07:** CPU reads, MARIA fetches and peeks share these memory maps; ordinary reads have no banking side
  effects. Preserve established audio decoding priority. Keep bank zero and zero-filled volatile RAM as the
  deterministic power-on profile; restore them on load and power cycle. Console RESET input remains distinct
  from a power cycle; existing `retro_reset` remains a power cycle. Fractalus RAM is not battery storage.
- **D-08:** Label uncertain hardware edges. The guide establishes core maps but explicitly leaves extended
  hotspots and unusual Absolute write values uncertain. Activision $FF88–$FF8F decoding, Absolute precedence,
  Fractalus mirrors above $4FFF and deterministic latch/SRAM startup are selected compatibility/profile
  policies supported by the preparation's implementation evidence, not new cartridge measurements. Use named,
  cited constants and independently derived vectors. Later hardware evidence can reopen them under DEC.11;
  no owner-console run or new accuracy oracle blocks this phase.

### Loading and host visibility

- **D-09:** Add exact geometries to probe/load validation before allocation or live-state changes. Preserve
  existing truncated/invalid/unsupported status distinctions and size-mismatch warnings; reject unsupported
  combinations explicitly. Failed validation allocates nothing. Allocation failure retains the previous
  cart. Normalize only within the accepted allocation. Retain one ROM-plus-physical-RAM allocation, bounded
  indices, no allocations after load, integer-only state and instance isolation. Keep the existing board
  switch rather than introducing a mapper framework or pointer table. Preserve API struct sizes and ABI;
  apply the documented version policy to the additive flags/warning and behavior revision during implementation.
- **D-10:** libretro fixed ROM descriptors must equal mapped CPU bytes for both Activision orders. Existing
  descriptors point at original content, so translate their 8 KiB offsets using retained input orientation
  or use a shared private normalized representation; never point OM offsets into AM data. Map Absolute fixed
  upper ROM and Fractalus fixed ROM. Leave switched ROM windows unmapped, as with existing banked spans.
- **D-11:** Replace the false contiguous Fractalus RAM descriptor. Prefer sixteen truthful 256-byte aliases
  covering $4000–$4FFF, backed by the physical 2 KiB, with higher mirrors explicitly unmapped in host descriptors.
  Confirm the pinned rcheevos resolver retains all intended regions below its 32-region cap, including fixed
  ROM. If that cannot fit, omit Fractalus RAM descriptors and document the limitation; retain the existing
  physical cartridge-RAM memory span. Core bus aliases still cover the entire window. Do not claim complete
  achievement visibility, grow the resolver or add a dependency. Tests must require intended mappings, not
  merely accept either NULL or a correct byte.

### Automated acceptance and adversarial cases

- **D-12:** Extend existing plain-C cart tests with independent address vectors: all banks, both halves,
  window endpoints, selector boundaries and neighbors, ignored address/data bits, initial state, ROM immutability,
  CPU/MARIA/peek agreement and reload/power cycle. Exercise every Fractalus window address against all 2,048
  physical bytes using multiple patterns; verify both intended aliases and distinct physical locations.
  Do not compute expected values by calling the production mapper.
- **D-13:** Add one owned executable diagnostic per board using existing assembly, manifest, mailbox, numeric
  expectation and bounded-driver conventions. Activision runs selector code from a fixed window. Derive
  expected records from the selected map before pinning outputs. Run the fixtures through library, real runner
  and in-process libretro paths, checking actual mailbox values and bounded execution rather than only a
  screenshot or non-crash. Identified AM/OM pairs must have equal mapped reads, diagnostic records and frame/audio
  behavior while preserving distinct raw identities and unchanged input buffers. Use the existing test-only
  database seam; never insert synthetic hashes into production tables. Reuse existing hosted RetroArch cases.
- **D-14:** Malformed-size and hostile-bank tests must actually select every new board. Extend deterministic
  corpus/property coverage and existing fuzz targets; the current execution fuzzer generates only linear images,
  and the loader fuzzer's raw-copy assertion must gain an independent expected normalization. Keep bounded case
  counts and existing nightly duration. Test refusals on an already loaded, switched board with written RAM:
  compare deep ROM/RAM bytes, registers and subsequent execution against a control, not only pointer-containing
  instance structures. Include allocator failure, no allocation during frames/power cycles/bus traffic, and
  interleaved instances compared with independent controls.
- **D-15:** All behavior tests run under the existing CI workflow/presets and exact inventories. Extend native
  frame/audio equality inputs for the owned diagnostics and preserve applicable existing portability lanes;
  their required/advisory status does not change. Preserve old compatibility predicates and pins; investigate
  any changed old result rather than blanket re-pinning. Diagnostic success proves board semantics, not retail
  playability or PAL completion. Keep untested retail ledger rows honest. Publish behavior prose in hardware,
  embedding/runner/libretro/testing docs as appropriate, with a short user-facing README and terse public header.

### Current review and comparability prerequisites

- **D-16 (accepted 2026-10-23):** The owner accepted the proportional-gates recommendation and authorized its
  bounded migration. ENGINEERING §11 now governs architecture review and concrete check contracts; maintenance
  LOC ceilings, line allocations, inventories, grant admission and complete disposable fit proofs are retired.
  No replacement score or numerical maintenance quota applies. The N654/C15941 redesign proposal is superseded,
  not approved. Preserve the historical measurements and halted01 evidence; they establish no board completion.
- **D-17 (accepted 2026-10-23):** Preserve authenticated ordinary quality observations, raw trusted-base policy,
  SHA/source/configuration identity, comparable compiler/platform and built samples, reasons and measured guards.
  Before new board fixture admission, implement and test only the narrow trusted-manifest comparability relation
  in06-10; current/base join equality stays intact. Every trusted old row stays byte-identical, only unique added
  rows for the three named owned board fixtures are allowed, and later bases protect those rows as old rows.
  Normal legal fixture admission/rebuild remains authoritative. No allocation prototype or line-fit proof is
  a prerequisite; a concrete contract failure still stops affected work under AGENTS rule7.

### Historical allocation decisions — superseded 2026-10-23

- **Historical D-16:** Adopt a fixed maximum **192 net counted lines** solely for BOARD-01–03 integration and its controls.
  Measured starting count **B = 15,287** at `617dfa762ddb649839b925be5aa582180066fa78`; assumed savings **S = 0**;
  fixed allowance **N = 192**; maximum **C = 15,479**. This is an allocation estimate, not demonstrated fit.

  | Allocation | Maximum lines | Fault prevented |
  |---|---:|---|
  | Semantic/diagnostic/fuzz registrations | 48 | New boards exist but their evidence does not execute |
  | Runner/libretro invocation contracts | 40 | Host acceptance silently tests another board or no output |
  | Manifest, inventories and claim glue | 32 | Missing fixtures/tests or unsupported coverage claims |
  | Narrow permanent-allocation admission and controls | 40 | Candidate policy self-authorizes unrelated loosening |
  | Integration reserve | 32 | Required counted integration omitted from estimates |

  Count complete affected enforcement files, comments/blanks, controls, inventory and embedded glue under
  ENGINEERING §11. Product C tests and owned assembly follow its existing classification; relocation does not
  create savings. Redistribution is visible within N, unused allowance retires and final actual tightens C.
  No new lane, generic framework, allowance language or automatic rebaseline is authorized.
- **Historical D-17:** Planning must establish a narrow, trusted-base admission path for this explicit new scope inside
  D-16. The current quality judge demonstrates initial adoption and dated exceptions, not a permanent increase
  against an existing ceiling; `INITIAL_GATE_CEILING` is not a blanket escape. Preserve rejection of unrelated
  policy loosening, expiry/missing evidence controls and measured-value checks. Do not finance permanent growth
  through renewing temporary exceptions. If the fixed construction cannot fit, record the concrete failing
  construction and stop affected expansion; another allocation requires new evidence and an explicit decision.
### Current independent package prerequisite

- **D-18:** The independent `archive_bytes` ceiling is 399,937 and has no headroom established for new code.
  Research/planning must identify a bounded package-size measurement and a lawful acceptance path before
  implementation depends on raising it. D-16 authorizes no byte increase, no candidate self-rebaseline and no
  weakened package guard. A necessary permanent byte allocation must be separately fixed with attributable
  measurements and explicit scope; do not invent an amount during this discussion. Preserve independent
  performance and compatibility guards; the armed performance workload remains the test card.

### Agent's Discretion

Internal helper names and file splits, additive flag/warning names, independent diagnostic patterns and IDs,
and plan ordering are delegated within these contracts. Research should resolve factual, fixture-comparability and package
prerequisites before writing executable tasks; it must not treat an estimate as a successful experiment.
</decisions>

<canonical_refs>
## Canonical References

Downstream agents must read relevant sections before planning or implementing.

### Scope and settled contracts
- `AGENTS.md` — licensing, test/documentation, boundedness, review, one-step and branch rules.
- `.planning/ROADMAP.md` — Phase 6 goal and all four success criteria.
- `.planning/REQUIREMENTS.md` — BOARD-01–03 and later-phase boundaries.
- `.planning/PROJECT.md` — deliverables, dependency preference and deterministic core.
- `.planning/preparation/README.md` and `.planning/preparation/DECISIONS.md` — index and settled choices,
  particularly DEC.09–11, 15–20, 26, 31, 33, 38, 40, 42–44, 51–56.
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` §§1–2,7 — layout, header policy, board maps and bounds.
- `.planning/preparation/ARCHITECTURE.md` §§3–6 — instance ownership, loader and public contracts.
- `.planning/preparation/CONFORMANCE.md` §§4–7 — diagnostic mailbox, legal fixtures, ledger and lane rules.
- `.planning/preparation/ENGINEERING.md` §§3,8,11 — checks, performance and current architecture review.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` §§1–4 — maps, achievement and runner contracts.
- `PROVENANCE.md` and `ASSET_POLICY.md` — per-file licensing and fixture admission.
- `.planning/phases/05-pipeline-tune-up/05-CONTEXT.md` D-09–13,D-19,D-35 and
  `.planning/phases/05-pipeline-tune-up/05-VERIFICATION.md` — current lanes and historical accepted scope; current review policy is ENGINEERING §11.
- `.planning/milestones/v1.0-phases/03-games-play-in-retroarch/03-CONTEXT.md` D-23–27 and
  `.planning/milestones/v1.0-phases/04-sound/04-CONTEXT.md` D-05,D-21–24 — current host/diagnostic/audio invariants.
- `docs/hardware/cartridge.md`, `docs/hardware/bus.md`, `docs/embedding.md`, `docs/runner.md`,
  `docs/libretro.md`, `docs/emulation-tests.md`, `docs/testing.md`, `docs/quality.md` — shipped contracts.

### Code and evidence entry points
- `src/cart.c`, `src/internal.h`, `src/instance.c`, `src/bus.c`, `src/cartdb.h`, `src/cartdb.c`.
- `include/muhtari.h`, `runner/main.c`, `runner/movie.c`, `libretro/libretro.c`.
- `tests/cart/test_cart.c`, `tests/cart/images.c`, `tests/cart/test_cartdb.c` — synthetic table seam and image builders.
- `tests/diag/driver.c`, `tests/diag/tests.cmake`, `tests/diag/assemble.cmake`, `tests/diag/expected.csv`,
  `tests/roms/manifest.txt` — existing executable-fixture pipeline.
- `tests/fuzz/loader.c`, `tests/fuzz/run_cart.h`, `tests/fuzz/seeds.c`, `tests/prop/test_prop.c`,
  `tests/core/test_core.c`, `tests/alloc.h` — safety/lifecycle coverage to extend.
- `tests/libretro/host.c`, `tests/libretro/rcheevos.c`, `tests/runner/test_runner.c`, `tests/compat/ledger.csv`.
- `quality.json`, `tests/cmake/quality.cmake`,
  `tests/cmake/quality_join.cmake`, `tests/policy/quality.cmake`, `.github/workflows/ci.yml`,
  `.github/workflows/nightly.yml`, `pins.txt` — authenticated observations and pinned tooling.

### External evidence, rechecked 2026-10-23
- [Bankswitching guide, revision 391](https://7800.8bitdev.org/index.php?title=ATARI_7800_BANKSWITCHING_GUIDE&oldid=391)
  — documented core maps and explicit limits on hotspot knowledge.
- [A78 header specification](https://7800.8bitdev.org/index.php/A78_Header_Specification)
  — no AM/OM layout discriminator; A8 naming does not prove physical RAM capacity.
- [MAME BSD-3 board file at dddf4cd1faee](https://github.com/mamedev/mame/blob/dddf4cd1faeeca6a6dac26ecbc2da2da4242b740/src/devices/bus/a7800/rom.cpp)
  — OM offsets and Absolute policy; per-file permissive evidence, not hardware measurement or a new code seed.
- [Proper-order correction, MAME PR 10686](https://github.com/mamedev/mame/pull/10686)
  — correction commit `0f164f496f2e5c681712c60c3a95fc3b2ebcbb0d`; AM/OM history.
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` source table CART.20,22,24,27,29,40
  — existing pin and license boundaries for implementation cross-checks. Do not open prohibited source.
- [Maintenance digest issue 22](https://github.com/szTheory/muhtari/issues/22)
  — read during discussion; runner-image notices add no cartridge requirement and cost totals remain qualified.
</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `cart_lookup` already tries direct, slice and Activision half-swap hashes, but discards orientation.
- `cart_install` already validates before one allocation; retained metadata can drive copy-time normalization.
- `cart_ram_at`, the board switch and reset path provide small local seams; Fractalus needs its own address index.
- Existing diagnostic builders, expected CSV, mailbox driver, counting allocator and test-only database avoid new tools.

### Established Patterns
- Size-tagged unchanged public structs, append-only status/flag conventions, deep instance-owned state.
- Independent semantic expectations plus executable diagnostics; exact inventories and named negative controls.
- Original bytes remain host content; private installed bytes serve emulation. Hash pins are regression evidence.

### Integration Points
- `cart_fits`, `cart_refused`, v3/v4 parsing and install must agree on newly accepted boards.
- libretro currently derives ROM descriptors from original content and maps unbanked RAM contiguously; both
  assumptions require board-aware treatment here.
- Loader fuzz assumes raw-copy equality; run-fuzz, isolation and no-allocation cases do not currently cover these boards.
- Existing registrations and exact inventories must retain required execution under the current D-16 review contract.
</code_context>

<specifics>
## Specific Ideas

Review covered hardware, C architecture, security, player/ROM-hack expectations, host/achievement interoperability,
test validity, CI cost, licensing and maintenance. Further fan-out stopped when it produced no new material choice.
No UI or graphics redesign is relevant. The principal traps are reversed switched halves in OM, a false contiguous
Fractalus mask, normalized offsets into original host bytes, superficial refusal snapshots and unexercised fuzz boards.
</specifics>

<deferred>
## Deferred Ideas

Hardware measurements and oracle expansion remain in the accuracy milestone; layout override tools need an actual
consumer beyond this phase. Saves, PAL and expansion boards retain their roadmap phases. No new hosted RetroArch case.

### Reviewed Todos (not folded)
- `.planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md`
  — substantive run-loop items already closed; residual test seeds do not become board requirements.
- `.planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md`
  — separate qualifying main-run history, not earned by adding board tests.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md`
  — surviving generator/firmware-scan tooling seeds remain deferred; no generator rewrite is needed.
- `.planning/todos/pending/2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md`
  — unrelated release/CI follow-up; Phase 5's accepted safety work remains authoritative.
</deferred>

---

*Phase: 06-retail-cartridge-boards*
*Context gathered: 2026-10-23*
