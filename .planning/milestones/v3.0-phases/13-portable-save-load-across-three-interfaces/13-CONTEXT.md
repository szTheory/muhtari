# Phase 13: Portable save/load across three interfaces - Context

**Gathered:** 2026-11-01
**Status:** Ready for planning

<domain>
## Phase Boundary

Deliver STATE-01, STATE-02, STATE-03, STATE-04, STATE-05, STATE-06 and FRONT-01:
one complete, portable, bounded
library state contract, exposed through the runner and libretro serialization
callbacks. Restore the same compatible machine point and continue identically;
refuse bad or incompatible bytes before changing the live machine.

The dependency is the shipped v2.0 baseline, package v0.10.0,
API 7 / ABI 1 / behavior revision 6. Phase 12 remains halted at accepted 0/4;
its failed evidence, blocked plans and spent hosted allowances remain unchanged.
This discussion authorizes no Phase 12 repair, publication or new host attempt.
Actual RetroArch save/load, rewind and run-ahead evidence remains Phase 14.
No requirement is completed by discussion, source inspection or recommendations.

The owner requested consideration of all relevant decisions through specialist
fan-out and explicitly delegated adoption of coherent recommendations. Three
read-only advisor reviews covered state architecture/security, host/API/progress
semantics, and independent evidence/portability/resource cost. Recommendations
below are agent-selected under that instruction, not invented interview answers.
One adversarial synthesis resolves their interactions; further expansion stops
when it would not change a decision.

</domain>

<decisions>
## Implementation Decisions

### Complete portable machine point

- **D-01 (STATE-01/04):** Keep DEC.26/35: one library-owned typed visitor, explicit
  byte-wise little-endian fields, checked SIZE/WRITE/VALIDATE/APPLY passes, and an
  exact size fixed for a successfully loaded machine until its next successful
  content load or destruction. Different installed topologies may have different
  sizes. Size is zero without loaded content. Save/restore occurs between public
  calls and neither advances emulated time nor allocates after content load.
  Use stable offsets, canonical unused/reserved bytes and an uncompressed format;
  no struct dump, pointer values, native widths, compression dependency or schema
  framework. The planner owns exact bytes, offsets and maximum size calculations.
- **D-02 (STATE-01/03):** Capture everything needed to reproduce the next public
  observation: CPU/bus/chip/board state; volatile and expansion RAM; boot status;
  regional timing; partial interrupt/HALT work; lazy-device clocks/deadlines;
  MARIA working memory/cursors; oscillator, mixer, PCM carry and pending audio;
  controller electrical/timed state; active and pending core-owned input, including
  once-only motion; emulated battery bytes; and unfinished EEPROM protocol,
  buffered page and busy work. Preserve observable counters where they affect the
  public continuation contract. Build the field/invariant inventory from current
  owners and prove it; this list alone is not that inventory.
- **D-03 (STATE-01/02):** Exclude immutable media bytes, allocator/callback state,
  borrowed spans, derived pointers, host output destinations, staging storage,
  filesystem paths and host persistence metadata. Reconstruct derived references
  from the already installed machine. Core-owned pending input is included even
  though old preparation prose and an internal comment describe it as host input;
  the frontend callback, movie cursor and future input source remain host-owned.
  Reconcile those stale descriptions in implementation. No sub-frame capture API,
  external movie cursor or full recording protocol is selected.

### Compatibility, refusal and resource safety

- **D-04 (STATE-02/03):** Require the same supported state-format version and
  behavior revision, effective cartridge bytes and machine-affecting configuration:
  resolved board/options/RAM, region, sound, controller and battery attachment,
  expansion, boot mode, and applicable BIOS/HSC firmware or explicit stub/absence.
  Bind immutable bytes with SHA-256, reusing the existing owned implementation
  through a small private shared owner. Bind effective choices rather than paths,
  titles, ambiguous Auto requests or package/build timestamps. Do not serialize
  ROM/firmware into the state or require a particular operating system or frontend.
  Exact identity encoding and equivalent-media normalization belong to planning.
- **D-05 (STATE-02/04):** Keep corruption detection distinct from identity and
  trust. A CRC/checksum is not authentication, and matching SHA-256 identities do
  not make payload fields trustworthy. Validate the complete envelope, expected
  length/topology, versions, reserved values, scalar ranges, counts/indices and
  cross-field clock/deadline relationships before any live mutation. APPLY must
  be infallible after validation. Hostile lengths never select allocations or
  unbounded loops. Validate against installed topology, not declarations supplied
  by the state. Test checksum-repaired hostile payloads, not only random corruption.
- **D-06 (STATE-02/04):** Refusal preserves subsequent execution, pending input,
  battery bytes/generation, installed media/topology and public span identity.
  Save errors preserve the caller's output buffer. Define input/output overlap
  behavior explicitly: either support it through bounded staging or reject it
  before mutation; never allow an apply pass to overwrite bytes still being read.
  Use existing instance ownership and the least necessary preallocated scratch.
  Exact tick caps, safe arithmetic and scratch/stack/work costs must follow from
  current producer invariants and measured complete work under ENGINEERING §11;
  do not invent a numeric quota, enlarge existing limits or tune timeouts by guess.
- **D-07 (STATE-01/02, FRONT-01):** Keep state-format, API, ABI, behavior and raw
  battery-layout versions separate. Additive public functions/size-tagged fields
  follow the existing API increment and ABI 1 conventions; append statuses without
  renumbering. Keep the battery layout unchanged unless its bytes actually change.
  Version existing execution behavior only when its contract changes. Refuse
  unsupported format/behavior instead of implementing migration. No cross-version
  compatibility, netplay or universal future state promise is made.

### Emulated progress and host durability

- **D-08 (STATE-05):** State rollback includes emulated battery bytes and pending
  programming, but never restores the old host dirty token. Compare restored
  persistent bytes with the live bytes: a changed successful restore advances the
  live generation once under its existing unsigned equality-token semantics;
  equal bytes preserve the token. Refusal preserves both. Restoring pending work
  without changed committed bytes does not itself mark bytes dirty; later actual
  programming uses the existing commit path. Preserve the stable raw battery span.
- **D-09 (STATE-05/06):** Library serialization never reads or writes durable
  files. Runner and frontend retain persistence ownership. A successful explicit
  load intentionally restores older emulated progress and may subsequently cause
  a committed host save to replace newer battery bytes; document that consequence.
  Refused loads and failed state-file replacement must not overwrite prior files.
  Speculative branches must never cause durable writes: test the library/runner
  ownership boundary here, and leave actual RetroArch speculative-mode proof to
  Phase 14. Do not equate a live SRAM pointer or dirty notification with disk I/O,
  or promise that an arbitrary frontend cannot flush speculative memory. Recovery
  history and preserving newer save revisions in Playstead remain separate work.

### Library, runner and libretro consumers

- **D-10 (STATE-01/02, FRONT-01):** Add a small public size/save/load surface using
  caller-owned buffers and the existing status convention; document behavior in
  `docs/embedding.md`, keeping the header terse. Library loading consumes one exact
  canonical state, with no ignored payload extension. A save accepts sufficient
  output capacity, writes the defined bytes and leaves excess caller capacity
  outside the state. Specify NULL, short-buffer, no-content and failed-content-load
  behavior and prove it through the existing lifecycle tests.
- **D-11 (STATE-06):** Runner state load/save/hash is required by the accepted
  requirement; older research calling it optional is superseded. Reuse existing
  argument, exit-status, digest and checked battery-file mechanisms. Load content,
  effective options and any configured initial battery first, then validate/apply
  the optional state before execution. Define save points by completed frame calls
  in this invocation, including a zero-frame point; do not use ambiguous host time
  or an implicit frame number from an unrelated movie. SHA-256 state hashing covers the
  canonical bytes, excluding dirty metadata, paths and wrapper data. Fresh-process
  continuation must consume the intended remaining input sequence explicitly.
  Select exact option names and incompatible-mode rules against the current parser
  during planning; reject ambiguity rather than inventing a continuation protocol.
- **D-12 (STATE-06):** Bound state reads by the installed expected size and detect
  short input, trailing data and I/O failure before restore. Check all write/close
  and replacement results; reuse the existing sibling-temporary checked replacement
  strategy rather than truncating the destination up front. Give invalid arguments,
  state refusal and file failure useful diagnostics and existing nonzero status
  categories (failure 1, usage 2); use diagnostics to distinguish incompatible or
  malformed state from file failure. Do not repurpose unsupported-game exit 77 or
  introduce a dedicated state-refusal exit without a concrete consumer need.
  Preserve existing target files on refusal/failure and exercise actual file
  failure controls. No new state file envelope, compression or sidecar is needed.
- **D-13 (FRONT-01):** Libretro delegates to the same library bytes; it gets no
  second serializer or host-file I/O. Report a stable nonzero size only for loaded
  content with supported serialization; unloaded/failed-load paths are safe and
  expose no stale content. Accept larger serialize capacity as the official API
  requires; write the canonical prefix and leave excess capacity unchanged.
  Restore consumes the exact state length: official guidance provides no equivalent
  oversized-unserialize promise. Prove short/extra/null input refusal. Keep the same
  validated portable format for ordinary and run-ahead contexts; do not skip
  validation because a frontend requests fast states. The pinned MIT header's
  rollback-netplay context 3 requires big-endian integers; when explicitly requested,
  refuse that unsupported context rather than silently emitting the LE format or
  adding a second codec. An unavailable context query follows ordinary behavior.
  Update `.info` and its existing claim guard with proven callback portability and
  determinism; do not announce rewind/run-ahead or actual RetroArch acceptance here.

### Independent evidence and proportional execution cost

- **D-14 (STATE-03/04):** Pair split-run equality with an independent owned
  semantic observation (mailbox, pins/register values, expected pixels or numeric
  audio/timing), so a field omitted by both save and load cannot pass by agreement.
  Compare resumed state bytes, frames, raw transition audio and PCM against an
  uninterrupted machine; cover all supported topology/region classes and targeted
  pending input/device/audio/storage boundaries. Use a coverage matrix with a
  reason for each representative combination rather than an unmeasured Cartesian
  explosion. Unsupported features remain unsupported and unmeasured hardware fits
  retain their confidence labels.
- **D-15 (STATE-02/04/05):** Extend existing unit, allocator, seeded property/fuzz,
  runner-file and loaded-module tests. Include truncation/extra bytes, wrong
  versions/media/firmware/effective config, checksum-valid invalid fields and
  overflowing or inconsistent clocks/deadlines/lengths; after rejection compare
  next execution and host-visible metadata, not just reserialized bytes. Prove
  changed/equal/refused battery restore and pending write replay. Mutations must
  demonstrably exercise their intended fault. Reuse visitor-derived field bounds
  only where they reduce duplicated knowledge; an auto-generated bounds sweep
  cannot replace cross-field invariants or an independent semantic oracle.
- **D-16 (STATE-03):** Require exact-source executed native/big-endian state-byte
  and continuation equality for Phase 13 acceptance, preserving all existing lane
  labels. Current PR `CI required` consumes a compile-only `be32` job; required
  `hash-equality` compares six native lanes. Executed PowerPC equality lives in
  `.github/workflows/be-equality.yml`, called by nightly and release workflows;
  nightly is advisory and release equality gates publication. Planning must name
  how the existing executed path supplies the necessary source-bound receipt.
  Compile success is not execution, nightly is not a required PR gate, and future
  release execution cannot be claimed in advance. No lane promotion or structural
  workflow redesign is selected by this discussion.
- **D-17:** Add meaningful behavior checks to `cmake --workflow --preset ci` through
  existing registrations/inventories; extend existing equality/fuzz consumers.
  Keep generated state bytes ephemeral and use only admitted owned/permissive
  inputs. Retain no post-load core allocation, bounded work, required correctness,
  packaging/provenance checks and advisory distinctions. Measure completed work
  where resource choices need evidence. No generic audit framework, dependency,
  new recurring actual-host case or unrelated CI tune-up is selected. Preserve the
  existing two review-fix rounds and one gap round limit.

### Agent's Discretion

Research/planning owns exact public declarations/status mapping, canonical layout,
field/invariant inventory, checksum coverage, identity normalization, pointer/alias
strategy, tick bounds, scratch placement, CLI spelling/frame-input interactions,
test fixtures and measured costs within these decisions. Reconcile stale source
comments and affected preparation/user docs alongside implementation. A concrete
new conflict with an existing required contract is a blocker to resolve, not an
optional seed; do not ask the owner to choose routine technical details.

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read relevant sections before planning or implementing.**

### Scope and intent
- `AGENTS.md` — provenance, instance/resource ownership, phase/branch and one-step rules.
- `.planning/README.md`, `.planning/INTENT.md` — gamer outcomes, memory and dependency posture.
- `.planning/PROJECT.md`, `.planning/REQUIREMENTS.md`, `.planning/ROADMAP.md`,
  `.planning/STATE.md` — shipped baseline, seven pending requirements and Phase 12 stop.
- `.planning/research/fan-out/prompt.txt` — owner-requested role lenses, alternatives,
  primary-source confidence, adversarial synthesis and diminishing returns.
- `.planning/phases/12-player-ready-delivery-and-playstead-bridge/12-CONTEXT.md`
  — host ownership and initial RetroArch-managed state UI; no renewed host authority.

### State architecture and host contracts
- `.planning/preparation/README.md` — reference index.
- `.planning/preparation/DECISIONS.md` — especially DEC.26/35 and version separation.
- `.planning/preparation/ARCHITECTURE.md` §4/6/7 — API and visitor spine; old 64-byte
  CRC-only header and pending-input exclusion must be reconciled with this context.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` §1/4 — pinned libretro and runner
  contracts; illustrative old CLI/layout does not override accepted requirements.
- `.planning/research/ARCHITECTURE.md`, `.planning/research/PITFALLS.md`,
  `.planning/research/STACK.md` — current-state inventory leads, identity, transactional
  load and host ownership; older optional-runner and Phase 14 ordering prose yields
  to the current requirements/roadmap.
- `.planning/seeds/SEED-002-states-and-movies.md` — state seed; broad movie/continuation
  ambitions are not Phase 13 scope or demonstrated portability claims.
- `docs/embedding.md`, `docs/runner.md`, `docs/libretro.md` — public behavior owners.
- `libretro/libretro.h` and `PROVENANCE.md` §8 — pinned MIT API, especially serialization.
- `ASSET_POLICY.md`, `PROVENANCE.md` — admitted sources and ephemeral state artifacts.

### Evidence and bounded work
- `.planning/LESSONS.md` — independent oracles, borrowed-input staging, exact
  inventories, complete-work timing and bounded workflow recovery.
- `.planning/preparation/ENGINEERING.md` §11 and
  `.planning/preparation/GSD-HANDOFF.md` §3–3.1 — ownership review and proportional gates.
- `docs/testing.md`, `docs/emulation-tests.md`, `docs/maintaining.md` — current consumers.
- `.github/workflows/ci.yml`, `.github/workflows/be-equality.yml`,
  `.github/workflows/nightly.yml`, `.github/workflows/release.yml` — actual required,
  advisory and publication consumers; recheck before using a receipt.

### External primary references reviewed
- [Libretro serialization guidance](https://docs.libretro.com/development/cores/developing-cores/)
  — buffer sizing and stable offsets; source-backed API guidance, not local host proof.
- [NIST CRC glossary](https://csrc.nist.gov/glossary/term/cyclic_redundancy_check)
  — error detection is distinct from authentication and strong media identity.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `src/internal.h` and `src/instance.c` — installed topology, mutable owners, private
  storage, effective media selection and live battery generation/span contract.
- `src/cpu.c`, `src/bus.c`, `src/riot.c`, `src/maria.c`, `src/tia.c`, `src/pokey.c`,
  `src/synth.c`, `src/ports.c`, `src/cart.c`, `src/boot.c` — fields and producer
  invariants; state traversal must not become a second hardware implementation.
- `runner/sha256.c`, `runner/sha256.h`, `runner/crc32.c`, `runner/crc32.h` — existing
  owned digest helpers; SHA-256 currently links into runner/libretro, not the core.
- `runner/main.c`, `runner/movie.c`, `runner/exit_status.h` — parsing, immutable
  replay, checked battery I/O and documented exits to reuse.
- `tests/prop.h`, `tests/prop/test_prop.c`, `tests/alloc.h`, `tests/fuzz/` — seeded
  properties, allocator observation and existing bounded replay/fuzz machinery.
- `tests/runner/test_save.c`, `tests/runner/test_runner.c`, `tests/libretro/host.c`,
  `tests/diag/`, `tests/input/` — file failures, loaded-module semantics and owned oracles.
- `tests/cmake/lane_equality.cmake`, `tests/inventory/ci.txt` — existing equality and
  execution inventories; extend their producers rather than adding parallel ledgers.

### Established Patterns and Integration Points
- No state module/API exists yet; current libretro serialization size is zero.
  Add one library format owner, then delegate from both hosts.
- Extend `include/muhtari.h` additively and register implementation/tests through
  current CMake owners. Preserve all three deliverables and public buffer lifetimes.
- Core owns emulated bytes; hosts own files, callbacks, input sources and lifecycle.
  The same bytes serve all three interfaces without another container format.
- Use independent expected behavior before cross-lane agreement. Record command,
  exit status and run URL for later execution evidence; no committed logs.

</code_context>

<specifics>
## Specific Ideas

The owner's explicit brief selects all relevant areas and delegates adoption of
coherent recommendations, including meaningful alternatives and footguns. Another
small understood permissive copy can be better than another dependency; here the
repository already owns the necessary digest and test mechanisms. Keep research
confidence separate from reproduced results. No new technical lesson is declared
reproduced by this read-only discussion, and no lesson export was requested.

</specifics>

<deferred>
## Deferred Ideas

Actual RetroArch save/load, rewind and run-ahead qualification remains Phase 14.
Cross-version migration, compressed/extensible state schemas, full movies/TAS/AI,
netplay, browser delivery, native Playstead hosting/Continue/history and revision
recovery retain existing product triggers. Revisit only for a demonstrated consumer
or measured cost, rather than widening the state foundation now.

### Reviewed Todos (not folded)

The matcher returned generic keyword overlaps; none adds a state requirement:
- `.planning/todos/pending/2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md`
  — existing CI/publisher dispositions remain outside the state foundation.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md`
  — preserve historical resolutions and remaining optional hardware seeds.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md`
  — retain current filter/ROM/tooling dispositions.
- `.planning/todos/pending/2026-10-29-keep-the-phase-11-first-review-info-findings.md`
  — optional compatibility wording/local-driver work does not become state scope.
- `.planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md`
  — no advisory-lane promotion without independently qualifying evidence.

</deferred>

---

*Phase: 13-portable-save-load-across-three-interfaces*
*Context gathered: 2026-11-01*
