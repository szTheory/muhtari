# Phase 7: Battery saves across hosts - Context

**Gathered:** 2026-10-23
**Status:** Ready for planning

<domain>
## Phase Boundary

Deliver SAVE-01–06: HSC SRAM and optional supplied firmware, SaveKey/AtariVox EEPROM,
a stable battery API, safe runner persistence and real RetroArch persistence across
fresh launches. The core owns emulated hardware and bytes; hosts own files.
This discussion completes no save requirement and implements no behavior.

The owner selected all relevant areas, requested parallel research across relevant
roles with an adversarial pass, and delegated adoption of coherent recommendations.
The decisions below are agent-selected under that instruction, not invented interview
answers. Three read-only reviews covered hardware/API, runner safety and frontend
acceptance. Further expansion stopped when it yielded no material new decision.

Carry forward permissive provenance, three deliverables, integer-only instance state,
bounded calls, no allocation after load, automation-first verification, proportional
gates and finite review/gap limits. Prefer existing mechanisms and small owned code
over dependencies. Stop after this discussion step; planning is a separate command.
Full OpenHSC, speech, save states, other controllers, PAL execution and expansion boards
retain their existing later scope.
</domain>

<decisions>
## Implementation Decisions

### HSC and firmware

- **D-01:** Attach HSC from resolved cartridge metadata or an explicit load-time override.
  Map its physical 2,048-byte SRAM at $1000–$17FF and 4,096-byte ROM at $3000–$3FFF
  through the existing decode paths. CPU reads/writes, MARIA fetches and side-effect-free
  peeks must agree where the hardware decode permits access. Preserve BIOS/INPTCTRL and
  existing cartridge/audio priority; do not add unconditional aliases through disabled
  decode. SRAM starts FF on a new load and is never formatted by the emulator.
- **D-02:** Optional user HSC firmware must be exactly 4,096 bytes and copied at load.
  Identify the canonical SHA-1 `a3af676991391a6dd716c79022d4947206b78164` using the
  existing SHA-1 implementation. Accept an explicitly supplied unknown image with a
  visible identity warning; absent C6/FE signature bytes at offsets $900/$904 also warn.
  This interprets DEC.60's hash check as identification, consistent with preparation
  §7's signature warning and existing optional-BIOS policy, rather than a canonical-ROM
  allowlist. It permits owned synthetic firmware tests without special production hashes.
  HSC has no reset vectors: do not reuse BIOS reset-vector validation or accept 16 KiB.
  Invalid size, unreadable requested media and inconsistent pointer/length pairs fail
  before replacing any live state. A supplied HSC ROM implies attachment; an explicit
  disable conflicting with that request is an error.
- **D-03:** Without firmware, expose only the documented C6/FE signature with deterministic
  inert ROM fill. State clearly that this supports compatible direct-SRAM software and
  supplies none of the seven firmware routines, screens or formatting. No firmware
  traps, copied instructions, bundled Atari bytes, vector success emulation or OpenHSC
  implementation. Runner gains an explicit HSC-ROM path; libretro discovers optional
  `highscre.bin` in its system directory only when HSC is attached. Missing optional
  firmware selects the limited stub with a visible notice; present unreadable/malformed
  firmware fails loading. Firmware bytes never enter saves or release archives.

### SaveKey and AtariVox hardware

- **D-04:** Own a small bounded pin-level 24LC256 model, shared by SaveKey and AtariVox's
  EEPROM-only behavior. Observe effective SCL/SDA after both RIOT ORA and DDRA writes
  at their actual bus data tick; released inputs pull high and either device may pull
  SDA low. Port 2 uses SWCHA SDA bit2/SCL bit3; support the corresponding port-1 wiring.
  Device-driven SDA changes must not generate host START/STOP events. Reads/peeks observe
  pins without inventing clock edges or protocol progress. Do not seed from excluded
  EEPROM RTL or add an I2C package.
- **D-05:** Implement address-select ACK/NAK, repeated START, two address bytes with A15
  ignored, random/current/sequential reads, master ACK versus final NAK, 64-byte page
  wrap and 32 KiB sequential wrap. Incomplete traffic and unsupported control addresses
  recover through documented STOP/START behavior with bounded state. Buffer writes;
  an address-only transaction does not start programming. STOP with data starts a named
  fixed 5 ms emulated-time busy interval; NAK while busy. Microchip specifies a maximum,
  so this duration is a deterministic compatibility model, not measured hardware timing.
  Publish changed persistent bytes only when programming completes. A same-value write
  still becomes busy but changes no generation. End-of-frame catch-up commits due writes
  without requiring software to poll; stalled CPUs and DMA cannot prevent time advancing.
  Power cycle first settles any already-due completion, then discards incomplete/pending
  programming and resets the protocol clock while retaining committed data. An exit or
  host flush never advances emulated time to finish a write. Test and document these edges.

### Battery API, lifecycle and attachment

- **D-06:** Battery layout version 1 is raw HSC alone (2,048), EEPROM alone (32,768), or
  HSC followed by EEPROM (34,816 bytes); absent media is NULL/0. No header, checksums,
  host-endian fields, filename identity or firmware bytes inside this span. All new
  public surface is additive and size-tagged; old media structs get zero/default tails
  and old linked clients retain behavior. Expose battery memory, layout version and a
  change generation without growing existing caller requirements. Apply the established
  API/ABI/behavior version policy during implementation.
- **D-07:** Resolve storage attachment at successful load and freeze pointer, length,
  layout and device placement until the next successful load or destroy. At most one
  EEPROM is supported, on either port; reject dual attachments transactionally rather
  than sharing an array or inventing layout 2. Explicit pre-load overrides win over
  resolved controller declarations; a SaveKey hint with no explicit location defaults
  to port 2. Explicit non-save device selection can suppress that default. Distinguish
  auto from explicit unplugged selection using existing conventions. HSC and EEPROM
  may coexist. Document precedence and test conflicting declarations/overrides.
- **D-08:** Preserve existing per-frame joystick/input semantics. Adding, removing or
  moving battery-bearing attachments through a per-frame input or late frontend callback
  must not resize/reinterpret SAVE_RAM; require reload and report the limitation visibly.
  Auto or the already attached save device remains valid. Avoid a general controller
  configuration redesign in this phase. Libretro must establish provisioning before
  publishing SAVE_RAM; first retro_run is too late. Do not assume controller setters
  necessarily ran before retro_load_game.
- **D-09:** Hosts may restore raw battery bytes after successful load and before the
  first emulated frame; these writes establish the initial clean baseline. Generation
  tracks actual committed emulated byte changes thereafter, including HSC CPU stores
  and completed EEPROM programming, and never read traffic, same-value stores or aborted
  transactions. Direct arbitrary host mutation after that import window is outside this
  contract; do not add shadow copies or per-query scans to discover it. Use a 64-bit
  equality token with defined unsigned wrap. A power cycle retains bytes and generation,
  so resetting hardware does not erase unflushed dirty evidence. A successful new load
  creates a new baseline; a refused load preserves bytes, topology, pointers, generation
  and subsequent execution. Boot setup currently runs no cartridge instructions before
  the first frame; retain and test the safe restore window. Peeks/getters remain observers.

### Runner persistence

- **D-10:** `--save-dir DIR` explicitly opts into persistence and requires a cartridge and
  an existing directory. Without it, create/read no save files. With no battery, it is a
  no-op after option validation. Use `<rom basename without its final extension>.sav`:
  only the final component matters, and an initial dot alone is not an extension.
  Preserve this established per-game naming; document one writer per save path and
  separate directories for same-basename games/concurrent sessions. No content-hash
  suffix, shared device file, locking service, migration container or sidecar identity.
- **D-11:** Read at most span-size plus one into FF-filled staging before applying data.
  A missing save is blank; zero/short files are padded FF; exact-size files are copied;
  oversize returns the reserved size-mismatch exit 4. Other directory/open/read/close or
  non-regular-file errors return 1 before frames. Missing means absence of that file,
  not an inaccessible/missing save directory. Arm persistence only after import succeeds;
  failure cleanup must never replace a rejected save with blank or partial memory.
  Import/missing blank is clean: do not create/rewrite a file until emulated bytes change.
  Raw contents are untrusted RAM, never a structure to parse or repair.
- **D-12:** `--save-interval F` is a positive bounded frame count, requires `--save-dir`,
  and defaults to 60 frames. On each cadence, flush only a changed generation; a change
  waits at most F subsequently completed frames. On-demand and orderly-exit flushing use
  the same dirty path. Preserve POSIX SIGUSR1 on-demand and SIGINT/SIGTERM orderly stops;
  on Windows support console CTRL_BREAK as on-demand and CTRL_C as stop using the native
  callback, with appropriate inter-thread flag synchronization. Handlers only record
  requests; all core access and I/O occur in the frame loop. Repeated/coalesced requests
  must work, including a request arriving during a flush. Force termination has no final
  flush guarantee. Document platform-specific capability honestly, without a new IPC protocol.
- **D-13:** A save transaction exclusively creates its own sibling temporary file,
  checks full write, stdio flush, file sync and close, then replaces the destination
  using the platform's same-filesystem replacement operation. Use POSIX rename and a
  narrow Windows replacement seam; MoveFileEx with replace/write-through and no copy
  fallback is preferred over ReplaceFile's documented destructive failure cases.
  Never unlink/truncate the destination first, copy over it or reuse another process's
  temporary. Bound collision attempts and remove only this attempt's temporary.
  Update last-persisted generation only after success. Stop visibly with exit 1 on
  flush failure, retain dirty state and do not silently reflush in cleanup. Tests must
  prove prior bytes survive each pre-replacement failure through the actual consumer.
  State the replacement commit point and tested local-filesystem guarantees; do not
  promise universal power-loss durability or unchanged old bytes after a successful
  replacement followed by a later error. No post-replacement directory-sync requirement
  or durability framework is added here.
- **D-14:** Finish the current bounded frame before servicing stop/flush requests.
  Final flush occurs before destroying valid loaded state, including after an unrelated
  output failure following valid frames; preserve the original failure exit. Never
  persist after failed import, failed load or BIOS rejection. Do not advance an EEPROM
  busy deadline merely to save. Successful normal/INT/TERM completion returns 0 only
  if required flushing succeeds. Reuse the exit table and capabilities/probe conventions;
  add exit 4 when its behavior becomes live. No new general host protocol or movie format.

### Frontend interoperability and evidence

- **D-15:** libretro exposes the exact core battery span as RETRO_MEMORY_SAVE_RAM with
  stable pointer/size, power-cycle retention and NULL/0 when absent or unloaded; the
  frontend owns .srm I/O. Late options requiring different storage take effect on reload.
  Descriptors expose actual HSC SRAM at $1000–$17FF and installed ROM/stub at $3000–$3FFF,
  with correct RAM/CONST flags; absent-HSC holes remain holes. EEPROM is serial hardware,
  not a fabricated flat CPU descriptor. Preserve existing cartridge/RAM descriptor truths
  and resolver limits. Test every intended HSC mapped byte with the pinned rcheevos
  resolver and prove SRAM aliases the battery span. Add optional firmware and libretro_saves
  claims only with their actual in-process and hosted proofs; save states stay unavailable.
- **D-16:** Keep exactly two logical hosted RetroArch cases: existing DMA screenshot
  equivalence, and the save round trip. Remove the redundant hosted no-content smoke
  while retaining its existing in-process coverage. The new owned diagnostic attaches
  both media, increments separate counters through CPU-visible HSC and real I2C traffic,
  waits until EEPROM programming completes, reads back and displays both counters.
  Two fresh frontend processes sharing only designated save storage must show 1 then 2.
  Compare independent expected counter pixels and each complete 34,816-byte .srm snapshot
  against the corresponding fresh runner .sav result. Matching black/wrong screenshots,
  preseeded counters or one unchanged medium cannot pass. Fresh per-launch directories
  must not delete the shared save directory; the current helper always removes RA_DIR.
  Consolidate within the existing 300-test-line budget and measured timeout policy;
  tests/retroarch currently totals 246 physical lines, not unlimited spare capacity.
- **D-17:** Detailed semantics belong in existing plain-C/in-process/runner tests, legal
  owned diagnostic machinery, deterministic property tests and bounded I2C fuzz replay
  plus existing nightly fuzz. Cover every layout, firmware validation/stub limit, pin
  edges/DDR changes, ACK phases, wraps, interrupted transactions, busy boundaries,
  same-value writes, generation, reset/import/load failures, allocator failures,
  instance isolation and no allocation after load. Inject write/flush/sync/close/replace
  faults through a narrow test seam, not production failure flags or a general I/O
  framework. Verify exact saved bytes and actual fresh-process execution, not merely
  successful exits. Preserve old predicates/pins and native/PowerPC equality evidence;
  extend existing lanes/inventories without promoting advisory lanes or adding cases.
- **D-18:** Before fixture-dependent acceptance, extend the existing trusted-manifest
  comparison narrowly for named owned Phase 7 diagnostics with real candidate/base
  producer/join evidence. Its current allowlist admits only Phase 6 board diagnostics.
  Every trusted old row remains byte-identical; reject modified/deleted/duplicate or
  unrelated rows, preserve raw trusted identity, and keep ordinary admission/provenance
  checks. This authorizes scoped fixture admission, not arbitrary fixture comparability.
  The independent three-component archive ceiling remains 399,937 bytes on every native
  lane. Measure early and at final six-native acceptance; no size increase, automatic
  rebaseline, new maintenance quota or weakened performance/compatibility guard is granted.
  Apply ENGINEERING §11 architecture review to the ownership boundaries and checks.
  Graduate behavior prose into hardware/embedding/runner/libretro/testing docs together;
  keep README short and the public header terse.

### Agent's Discretion

Exact additive API field/function names, internal storage placement, source splits,
diagnostic names, fixture count beyond the single combined hosted fixture, fault-injection
seams and task ordering are delegated within these contracts. Research must resolve
factual contradictions and demonstrate fixture/package prerequisites before relying on
them. This discussion does not claim those future experiments have passed.
</decisions>

<canonical_refs>
## Canonical References

Downstream agents must read relevant sections before planning or implementing.

### Scope and settled contracts
- `AGENTS.md`, `.planning/PROJECT.md`, `.planning/REQUIREMENTS.md` SAVE-01–06 and
  `.planning/ROADMAP.md` Phase 7 — scope, three deliverables, acceptance and step boundary.
- `.planning/preparation/README.md`, `.planning/preparation/DECISIONS.md` — index;
  DEC.09–11,15–20,22,26,32–33,40,43–45,51–57,60 remain binding.
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` §§3–4,7 and source table
  CART.06,10,28,38,40 — HSC/EEPROM, wiring, limits and excluded source.
- `.planning/preparation/ARCHITECTURE.md` §§3–7 — API/lifecycle/ownership. Clarify its
  BIOS/HSC validation conflation and import/generation wording as D-02/D-09 specify.
- `.planning/preparation/A7800-SYSTEM.md` §§4–5,7–8 — decode, RIOT and boot boundaries.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` §§1–4 — layout, descriptors, saves,
  exit statuses and two hosted cases. Early future-feature prose is not shipped behavior.
- `.planning/preparation/CONFORMANCE.md` §§4–7, `.planning/preparation/ENGINEERING.md`
  §§3,8,11, `PROVENANCE.md`, `ASSET_POLICY.md` and `.planning/LESSONS.md` — diagnostics,
  legal bytes, current proportional gates and local-versus-inbound lessons.
- `.planning/phases/06-retail-cartridge-boards/06-CONTEXT.md` current D-16–18 and
  `.planning/phases/06-retail-cartridge-boards/06-VERIFICATION.md` — accepted bounds and
  authenticated fixture precedent; historical maintenance allocations are superseded.
- `.planning/phases/05-pipeline-tune-up/05-CONTEXT.md` D-07–13 and
  `.planning/milestones/v1.0-phases/04-sound/04-CONTEXT.md` D-02–05 — retained lanes,
  evidence authorities, bus data ticks and boot/audio invariants.
- `docs/embedding.md`, `docs/runner.md`, `docs/libretro.md`, `docs/hardware/bus.md`,
  `docs/hardware/riot.md`, `docs/hardware/controllers.md`, `docs/hardware/cartridge.md`,
  `docs/testing.md`, `docs/emulation-tests.md`, `docs/quality.md` — existing public contracts.

### Code and automation entry points
- `include/muhtari.h`, `src/internal.h`, `src/instance.c`, `src/ports.c`, `src/riot.c`,
  `src/bus.c`, `src/cart.c`, `src/boot.c`, `src/frame.c`, `src/sha1.c`, `src/sha1.h`.
- `runner/main.c`, `runner/stop.c`, `runner/stop.h`, `runner/movie.c`,
  `runner/exit_status.h`, `libretro/libretro.c`, `libretro/muhtari_libretro.info.in`.
- `tests/core/test_core.c`, `tests/input/test_input.c`, `tests/cart/test_cart.c`,
  `tests/alloc.h`, `tests/runner/test_runner.c`, `tests/runner/test_audio.c`,
  `tests/runner/tests.cmake`, `tests/libretro/host.c`, `tests/libretro/rcheevos.c`,
  `tests/libretro/tests.cmake` — lifecycle, actual-main testing, resolver and claims.
- `tests/diag/driver.c`, `tests/diag/assemble.cmake`, `tests/diag/tests.cmake`,
  `tests/diag/expected.csv`, `tests/roms/manifest.txt`, `tests/retroarch/run_retroarch.cmake`,
  `tests/retroarch/test.cfg.in`, `tests/retroarch/compare.c`, `tests/retroarch/bmp.c`.
- `tests/fuzz/tests.cmake`, `tests/prop/test_prop.c`, `tests/cmake/lane_equality.cmake`,
  `tests/cmake/quality.cmake`, `tests/cmake/quality_join.cmake`, `tests/policy/quality.cmake`,
  `quality.json`, `.github/workflows/ci.yml`, `.github/workflows/nightly.yml`, `pins.txt`.

### External evidence reviewed 2026-10-23
- [Microchip DS20001203Y](https://ww1.microchip.com/downloads/aemDocuments/documents/MPD/ProductDocuments/DataSheets/24AA256-24LC256-24FC256-256K-I2C-Serial-EEPROM-DS20001203.pdf)
  §§2,4–8 — manufacturer protocol facts; resolves CART.38's previously unfetched facts.
- [HSC technical reference](https://7800.8bitdev.org/index.php/7800_HSC_Technical_Reference)
  — map, signatures, routines and firmware identity; documentation only, not its unlicensed disassembly.
- [libretro API header](https://raw.githubusercontent.com/libretro/libretro-common/master/include/libretro.h)
  and [core lifecycle documentation](https://docs.libretro.com/development/cores/developing-cores/)
  — host lifecycle and stable memory; local pinned header remains build authority.
- [POSIX rename](https://pubs.opengroup.org/onlinepubs/000095399/functions/rename.html)
  and [directory durability rationale](https://pubs.opengroup.org/onlinepubs/9799919799/xrat/V4_xbd_chap01.html)
  — replacement versus durability; no universal crash-safety inference.
- [Microsoft MoveFileEx](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexw),
  [ReplaceFile](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-replacefilew),
  [_commit](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/commit?view=msvc-170),
  [console handlers](https://learn.microsoft.com/en-us/windows/console/handlerroutine) and
  [console control generation](https://learn.microsoft.com/en-us/windows/console/generateconsolectrlevent)
  — platform replacement/control boundaries, including destructive ReplaceFile errors.
- [Apple sigaction](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/sigaction.2.html)
  — persistent POSIX request handlers.
- [Maintenance digest issue 22](https://github.com/szTheory/muhtari/issues/22) — reviewed;
  runner-image notices add no battery requirement and do not promote advisory lanes.
</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `muhtari_span`, size-tagged struct handling and linked old-API smoke conventions support additions.
- Probe metadata already identifies HSC/EEPROM/controller hints; actual hardware is absent.
- Existing allocator counters, synthetic images, owned assembly/mailboxes, runner-main test
  wrappers, rcheevos resolver and bounded fuzz machinery cover the needed evidence.
- Runner already handles stop requests at frame boundaries; extend control flags without I/O in handlers.

### Established Patterns
- Validate before the committing cartridge install/free-old step; power cycle resets volatile hardware.
- Short module ownership, plain dispatch, fixed-size state, integer ticks and no new dependency.
- Independent numeric expectations, actual host invocations, exact inventories and honest capability claims.

### Integration Points
- Public API4 currently lacks battery memory/generation; media contains only cartridge and BIOS.
- `ports.c` currently applies controller overrides per frame; EEPROM provisioning needs a load seam.
- RIOT ORA/DDRA transitions and the actual slow-cycle data tick drive serial hardware.
- libretro currently exposes only SYSTEM_RAM, marks saves false and leaves HSC address holes.
- Hosted helper deletes its entire launch directory; isolate persistent save storage explicitly.
- Trusted-manifest comparison permits only three named Phase 6 fixtures; package ceiling stays independent.
</code_context>

<specifics>
## Specific Ideas

Review considered players' progress, homebrew compatibility, C ABI and lifecycle, hardware
timing, storage failure modes, Windows/POSIX control behavior, frontend callback ordering,
achievement maps, fixture legality, test independence, CI cost and maintenance. The critical
traps are formatting imported SRAM, counting buffered writes as committed, resetting dirty
generation, growing SAVE_RAM late, cleanup overwriting failed imports, destructive replace
fallbacks and a screenshot-only persistence oracle. No UI or graphics redesign is needed.
</specifics>

<deferred>
## Deferred Ideas

Shared per-device/cross-game media, automatic same-basename migration/locking, dual EEPROM
layouts, full OpenHSC, speech, external IPC/continuation protocols and universal power-loss
durability are not added. Hardware timing measurements remain in the accuracy milestone.

### Reviewed Todos (not folded)
- `.planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md`
  — run-loop contracts already closed; residual test seeds are not save requirements.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md`
  — surviving generator/firmware-scan seeds are outside this change; retained guards still apply.
- `.planning/todos/pending/2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md`
  — unrelated CI follow-up; do not reopen accepted release safety.
- `.planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md`
  — separate qualifying history, not earned by battery support.
</deferred>

---

*Phase: 07-battery-saves*
*Context gathered: 2026-10-23*
