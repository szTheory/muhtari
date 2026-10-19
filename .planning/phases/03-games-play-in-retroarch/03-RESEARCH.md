# Phase 3: Games play in RetroArch - Research

**Researched:** 2026-10-19
**Domain:** MARIA DMA and video, the run loop, cartridge boards, boot, input, libretro and rcheevos, plus the test and CI evidence for all of these (C17 core, CMake `-P`, GitHub Actions)
**Confidence:** HIGH for the repository facts and pins (read or reproduced this session). MEDIUM for the integration design, which is derived from the code and the prep files. LOW for the CI time estimates, which are extrapolated and must be measured.

## Summary

Phase 3 builds on a core that runs a CPU but nothing else yet. `src/bus.c` has no cartridge path: every address outside RAM, the TIA, MARIA's register window and the RIOT decodes to `DEV_OPEN`. `src/frame.c` only paints the test card. `muhtari__cart_load` copies the ROM, but nothing maps it. Before the first sample can run, the phase must add four things to the bus: a cartridge and BIOS device in `decode()`; MARIA's line events in the deadline dispatcher; MARIA's registers at $20–$3F; and a run loop that ends a frame at line 258, t 42. The existing HALT and NMI entry points (`muhtari__bus_request_halt` and `muhtari__bus_raise_nmi`) are the right seams. Two bus details need care. First, `hold()` advances a whole RDY or HALT hold in one jump, so it must stop at line-event deadlines. Second, the DMA burst must run after SALLY's two latency cycles (c0 and c1), not at the t 28 request.

Every external input was reproduced this session on the pinned tools:
- The own recipe builds 34 of the 35 `sizes.ref` samples byte-identically, and `simple` matches its committed pin. `banksets` fails to assemble with the current recipe, and its board is refused in phase 3 anyway.
- OpenBIOS at `b8bc745a` with $FF fill gives `46b5bc2b…`, and with $00 fill gives `1ee4c780…ad4f`, using the package's `dasm.wasm`.
- The rcheevos v12.1.0 tag resolves to `6755915f…`, and its GitHub archive has a SHA-256 of `c8bb9f66…`.

Three findings contradict the locked text and must be handled in the plans:
- OpenBIOS never *locks* INPTCTRL on the 7800 path (it writes `$06`), so D-16's hand-off frame K must key on the first cartridge fetch with EXT = 1.
- The `pokey` digest the recipe produces is not the one CONFORMANCE §2(a) recorded.
- AGENTS rule 7's gate-code budget is already exceeded under every definition measured.

**Primary recommendation:** Land the bus/MARIA/run-loop tracer first (D-04 steps 1–2) as one plan with a hand-built DL unit test, then `simple` via fast boot. Before that plan executes, fix three things in writing: the T0/HALT fit (T0 = /HALT fall + 6 ticks reproduces the measured 16/24), the hand-off-frame convention, and the hold-splitting rule. Everything later (samples, diagnostics, SuperGame, input, BIOS, libretro, ledger) widens from that path.

<user_constraints>
## User Constraints (from CONTEXT.md)

The decisions below are copied verbatim from `03-CONTEXT.md` `<decisions>` and `<deferred>`.

## Implementation Decisions

All prior choices in `.planning/preparation/DECISIONS.md` and in phases 1 and 2's CONTEXT files stand.

As in phases 1 and 2, each open area was researched by an advisor agent working from the owner's fan-out brief. Four
agents ran in parallel at calibration tier `minimal_decisive`. Their results were reconciled across areas and are
locked here. Two points went to the owner:
- the public API, because it adds to a released library;
- GAME-01's wording, because new evidence shows it cannot pass as written.

The owner chose the recommended option for both.

### The public API, version 3 (GAME-03, GAME-04, GAME-05)

- **D-01:** Consumer-minimal API version 3 (owner's choice). Every addition has a phase 3 consumer: the libretro
  adapter or the runner. Everything else stays internal and is reached by tests through `src/internal.h`, as
  `muhtari__cart_load` was in phase 2.
  - **Versions.** `MUHTARI_API_VERSION` becomes 3. ABI 1 is unchanged. Behaviour revision 1 is unchanged too, because
    with no content loaded every frame and audio hash stays the same.
  - **The header comment.** Fix the stale `uint32_t api; /* 1 */` in `muhtari_versions` here. That folds IN-01 of
    the phase 2 seeds todo; an `/* MUHTARI_API_VERSION */` comment cannot drift.
  - **The API test.** `tests/api/v3.c` touches every new name, so `abi.symbols` and `abi.api_version` cover them.
  - **Reversibility:** costly. From 0.3.0 this is published API. Pre-1.0 semver allows a break (DEC.14), but every
    embedder would have to change.

- **D-02:** The header delta. The planner may adjust names, but not the shape.
  ```c
  #define MUHTARI_API_VERSION 3

  /* appended to the status enum */
      MUHTARI_STATUS_ERR_BIOS_REJECTED_CART = 10, /* run_frame: the BIOS took its 2600 path; sticky until load or power_cycle */
      MUHTARI_STATUS_ERR_INVALID_BIOS = 11,       /* not 4,096 or 16,384 bytes, or its reset vector points outside it */
      MUHTARI_STATUS_ERR_UNSUPPORTED_REGION = 12, /* resolves to PAL, until PAL lands (LIB-04) */
      MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE = 13  /* an explicit device this version does not model */

  /* appended to the port enum, outside the A78 byte range */
      MUHTARI_PORT_UNPLUGGED = 256

  typedef struct muhtari_media {
      uint32_t size;
      uint32_t region;      /* 0: the cartridge's (header, hash, then NTSC); MUHTARI_REGION_* */
      const void *image;    /* copied at load */
      size_t image_size;
      const void *bios;     /* NULL: fast boot; copied at load */
      size_t bios_size;
  } muhtari_media;

  enum { MUHTARI_BUTTON_UP = 1u, MUHTARI_BUTTON_DOWN = 2u, MUHTARI_BUTTON_LEFT = 4u,
         MUHTARI_BUTTON_RIGHT = 8u, MUHTARI_BUTTON_1 = 16u, MUHTARI_BUTTON_2 = 32u };
  enum { MUHTARI_SWITCH_RESET = 1u, MUHTARI_SWITCH_SELECT = 2u, MUHTARI_SWITCH_PAUSE = 4u,
         MUHTARI_SWITCH_LEFT_DIFFICULTY_A = 8u, MUHTARI_SWITCH_RIGHT_DIFFICULTY_A = 16u };

  typedef struct muhtari_input {
      uint32_t size;
      uint32_t device[2];   /* 0: the cartridge's; MUHTARI_PORT_PROLINE, _JOYSTICK, _UNPLUGGED */
      uint32_t buttons[2];  /* MUHTARI_BUTTON_*, pressed = 1 */
      uint32_t switches;    /* Reset, Select, Pause held = 1; a difficulty bit set = A, clear = B */
  } muhtari_input;

  enum { MUHTARI_MEMORY_SYSTEM_RAM = 1, MUHTARI_MEMORY_RIOT_RAM = 2, MUHTARI_MEMORY_CART_RAM = 3 };

  typedef struct muhtari_span {
      uint32_t size;
      uint8_t *data;        /* stable from load to destroy; NULL when absent */
      size_t bytes;
  } muhtari_span;

  typedef struct muhtari_counters {
      uint32_t size;
      uint64_t frames;      /* since load or power cycle */
      uint64_t lag_frames;  /* frames with no CPU read of SWCHA, SWCHB or INPT0-5 */
  } muhtari_counters;

  muhtari_status muhtari_load(muhtari *m, const muhtari_media *media);
  muhtari_status muhtari_power_cycle(muhtari *m);
  muhtari_status muhtari_set_input(muhtari *m, const muhtari_input *in);
  muhtari_status muhtari_get_memory(muhtari *m, uint32_t kind, muhtari_span *out);
  muhtari_status muhtari_get_counters(const muhtari *m, muhtari_counters *out);
  ```

- **D-03:** Behaviour, written in `docs/embedding.md`, not the header:
  - **Load.**
    - `load` validates everything before it applies anything: the probe status, the BIOS size and vector, and the
      region. A failed load keeps the previous content.
    - A successful load replaces any loaded content and power-cycles the machine.
    - There is no `muhtari_unload`: `destroy` plus `create` returns to the test card.
    - The BIOS lives in a fixed 16 KiB array in the instance. Only the ROM copy is allocated, so
      `core.no_alloc_after_load` holds.
  - **Region.** PAL is refused with `ERR_UNSUPPORTED_REGION` until LIB-04, so pinned NTSC hashes cannot change
    silently later. `get_info` reflects the loaded content, and embedders read it after load.
  - **Devices.**
    - A device resolved from the header (`device == 0`) that phase 3 does not model is wired as ProLine. This is
      documented, and changes with a behaviour-revision bump when that device lands.
    - An explicit unmodelled device in `set_input` returns `ERR_UNSUPPORTED_DEVICE` and changes nothing.
    - The device sits in the per-frame input because RetroArch calls `retro_set_controller_port_device` after
      `retro_load_game`.
  - **Input bits.** Unknown button or switch bits return `ERR_ARGUMENT`. Opposing directions pass through at pin
    level.
  - **Input timing.** The input is latched and applied at the next VBLANK start (DEC.25). Pending input is not machine
    state.
  - **`get_memory`.**
    - It is non-const, because hosts write through the span (cheats, rcheevos).
    - It is the one deliberate exception to the "no pointers into core state" struct rule, because libretro memory
      maps need live pointers. The docs say so.
    - An absent kind returns OK with an empty span. An unknown kind returns `ERR_ARGUMENT`.
    - `CART_RAM` is the unbanked SuperGame RAM only.
  - **BIOS digests.** The core does not report them. The runner and the adapter hash the bytes they pass in and log
    the SHA-256.
  - **Internal only (reached through `src/internal.h`):**
    - the DEC.36 profile, including `cpu_phase` (D-12) and `riot_orb_power_on`;
    - the per-line DMA statistics (D-09);
    - a side-effect-free peek, for the prop tests and the input tests;
    - `io_hazard` and the other counters.

### MARIA, the run loop and the diagnostic ROMs (GAME-01, GAME-02)

- **D-04:** Tracer order (ARCHITECTURE §11):
  1. `maria.dma_line` unit tests: a hand-built DLL and DL in RAM, `maria__dma_line()` called directly, no CPU. They
     assert the HALTRST tick, the line-RAM bytes and the statistics against A7800-MARIA §4.2's closed form (T0+9+D, or
     +15+D on a zone's last line; abort tails +3/+9).
  2. The 7800basic `simple` sample through fast boot, with its frame pin.
  3. The diagnostic ROMs.
  4. SuperGame.
  5. Input, the BIOS path, libretro and the ledger.

- **D-05:** Module shape.
  - **`src/maria.c`:**
    - the registers;
    - the line-event table (t 0/28/42/43, A7800-MARIA §4.1);
    - `maria__dma_line(m, t0, lrc, *stats)`: one burst, each fetch at its own tick through the cart read, with
      fetches at or after LRC dropped (DEC.23);
    - span rendering before each $20–$3F write and at t 0;
    - the per-line statistics in the instance (about 10 bytes × 263 lines, DEC.34).
  - **`src/frame.c`:**
    - It loops `cpu_step` until the tick reaches the VBLANK-start deadline (119,402 ticks).
    - Line events fire from the tick count, not from DMA, so a ROM with DMA off still ends its frame.
    - A jammed CPU idles to the next deadline. Every step advances at least 4 ticks: that is the termination argument
      for DEC.33.
  - **Boards** dispatch by a `switch` in `cart.c` (DEC.31).

- **D-06:** The NMI and HALT contract. This folds the todo
  `2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md`.
  - **NMI (WR-05).** MARIA raises one DLI edge per zone and never retries, so it needs only "queued".
    - Narrow `muhtari__bus_raise_nmi` in `src/internal.h` and `docs/hardware/bus.md` to: it returns 1 when the edge
      is queued for latching and 0 when the edge is ignored. A queued edge can still be dropped by `latch_nmi`, when
      an earlier latched edge has not been taken or MEN was cleared.
    - No drop counter, because nothing would read it.
    - MARIA stores the return as a `dli_queued` bit in the line statistics.
    - Add an `nmi.one_edge_at_a_time` sub-case.
  - **HALT (IN-04).** The docs name both reasons for a 0: the gate is shut, or a window is already in flight. MARIA
    treats 0 as "no burst this line" and sets a `dma_blocked` statistics flag. The planner checks this rule against
    bus.md's gate semantics.
  - **JAM (IN-01).** Add `frame.jam_ends` and `frame.dma_off_ends`. Trim the JAM sentence in
    `docs/hardware/sally.md` to what `frame.jam_ends` shows.
  - **Seeds (rule 7).** WR-02 (set `poll_phi2` in `cpu.jam.step_reads_ffff`) and IN-05 are test code. Fold them in
    only if the plan touching those tests does so for free.

- **D-07:** Diagnostic ROMs: written, assembled and kept.
  - **Toolchain.** dasm from the pinned 7800basic 0.41 package under wasmtime 48.0.6: `dasm.wasm -f3`, then
    `7800sign.wasm -w`, then `7800header.wasm`, the same stages as `tests/built/build.cmake`. No new tool.
    - The alternative, ca65 at a pinned cc65 commit (CONFORMANCE §4), is used only if dasm's macros make the DMA
      matrix unmanageable.
  - **Sources.**
    - Our own code, MIT, with an SPDX line in each `.s`.
    - Our own 16 hex glyphs; no BIOS or cc65 font.
    - Linear at $C000 unless the ROM tests a board.
  - **Committed bytes.** `tests/diag/*.a78` with their `.s` and `.cfg` sources. The bytes are committed because
    hardware runs and the nightly a7800 differential need them (CONFORMANCE §7 rule 6). Each gets a
    `tests/roms/manifest.txt` row (`this-repo`, `MIT`).
  - **Rebuild and compare.** `diag.rebuild`, in a CMake `-P` script `tests/diag/assemble.cmake`, runs under the
    `built` label on all six legs.
    - It assembles each source and compares the result with its manifest SHA-256.
    - It covers `tests/cpu/decimal/clark.s` too, which closes phase 2's pending "rebuild-and-compare joins the
      diagnostics lane".
    - Its negative control is one changed digest, which must fail by name.
    - Fault prevented: a source edit or tool drift leaving the committed bytes stale.
  - **Mailbox.** Exactly CONFORMANCE §4's layout:
    - $2200: magic `M7DG`;
    - $2204: version 1;
    - $2205: status ($80 running, $00 done; $81 reserved);
    - $2206: count;
    - $2207: ROM id;
    - $2208: CRC-16 of $2210–$22FF;
    - records at $2210+4n: test id, flags, a 16-bit LE value.
  - **Reading it.** An in-process ctest driver, `muhtari-test-diag`, labelled `diag`.
    - It fast-boots and reads the mailbox at each VBLANK start.
    - It stops when the status is $00, and fails as "hang" at 600 frames.
    - It checks the CRC, then judges each record against `tests/diag/expected.csv`
      (`rom,test,value|lo-hi,source`, where source is `hw`, `guide`, `rtl` or `fit`).
    - It also checks the per-line statistics, so each quantity is recorded twice and the two must agree.
    - Its negative control is a CSV with one changed value.
    - It is not a runner flag: verdict logic stays out of the shipped runner.

- **D-08:** The diagnostic set, with CONFORMANCE §4 numbering.
  - **`dma`:**
    - #1: cost by NULL DL; 4- and 5-byte headers; widths 1, 8 and 32; direct against indirect 1- and 2-byte; last
      line against other lines. Reported as INTIM values and loop counts.
    - #5: terminator variants.
    - #6: holes.
    - #9: overrun, reported as objects completed as objects are added.
  - **`cpu`:** #2, the slow-access cost (INTIM after 64 × `LDA` from $00, $0480, $0280 and $1800 with DMA off).
    #3 (RIOT edges) is optional.
  - **`dli`:**
    - #4: DLI arrival against WSYNC;
    - the WSYNC release;
    - the store timing of RDY after a write;
    - #8: MSTAT edges.
  - **Feature-gap ROMs (D-14):**
    - `modes`: 160B, 320B and 320D (read mode 2), kangaroo mode, 2-byte indirect;
    - one ROM each for a 16 KB linear board, SuperGame bank 6 and SuperGame 144 KB. Each shows its bank numbers on
      screen and in the mailbox.
    - The planner may merge the board ROMs if one image can show all three layouts.
  - **Deferred to v4.0 or the owner's hardware:**
    - #7 (width 0 = 32 bytes, unverified);
    - #10 (picture only; the samples and `modes` cover it);
    - #11 (INPTCTRL; phase 2's bus tests cover it);
    - the raster-numbered BACKGRND-split ROM.

- **D-09:** Expected values and the ARCHITECTURE §13 conflicts.
  - **Write expected values before the first run.** Each value is derived from the cited constants before the
    emulator first runs the ROM, and the derivation goes in a new `docs/hardware/maria.md`. Only `fit` rows may ever
    be copied from the emulator's output. Fault prevented: the CSV becoming a snapshot of itself.
  - **Gating rows (`hw`/`guide`), settled now:**
    - header 8/10, graphics byte 3, indirect 6/9 (App. 3);
    - empty DL 16/24 (measured);
    - direct hole = header + 3;
    - the terminator rule `!(v & 0x5F)`;
    - +2 ticks per slow access, on the data cycle only;
    - 113 timer ticks per WSYNC line;
    - overrun aborts at LRC with partial objects.
  - **Fitted rows (`rtl`/`fit`).** Each keeps §13's current recommendation and carries its v4.0 flip predicate:
    - 242/292 rows;
    - `MARIA_FIRST_PIXEL_T 135`;
    - the HALT latency and handshake in `halt_window()`;
    - DLI→NMI per MiSTer (the 2nd φ1 after /HALT rises);
    - RDY after a write by the NMOS rule;
    - indirect hole 6/9;
    - holey DMA draws nothing.
  - **$2800–$2FFF.** The CPU bus stays undecoded (§13 C8). Only the rcheevos descriptor maps it (D-21).
  - **RIOT ORB at power-on.** It stays $FF by default (§13). An input-test case pins CTLSWB $14 with no SWCHB write
    staying one-button.

- **D-10:** The per-line statistics gain a sticky feature mask, the evidence for D-14. Its bits: RM0, RM2, RM3, write
  mode, kangaroo, holey 8, holey 16, indirect 1-byte, indirect 2-byte, DLI fired, `dli_queued`, `dma_blocked`. The
  board comes from the probe.

- **D-11:** One translation unit is not revisited. Phase 2's measured gap was 0.31 %, under the 5 % trigger.

- **D-12:** `cpu_phase` is a constant derived from a cited source, with the RTL simulation in v4.0.
  - **Derivation.** The fast-boot hand-off puts the first cartridge fetch at row 239, column 436 (A7800-SYSTEM C10,
    measured). That is line 240 at t = 24, and 24 mod 4 = 0, so the default is `cpu_phase = 0`.
  - **Labelled fitted.** ARCHITECTURE §6's row changes from "extracted by RTL simulation in P3" to "derived from C10;
    RTL check in v4.0 (DEC.41)". The OpenBIOS path uses the same default, also fitted.
  - **Why not simulate now.** A Verilator simulation would be a new dependency built on MiSTer's facts-only
    top-level files, and would only refine a value already bounded to {0, 2}.

### Samples, OpenBIOS, predicates, properties and the ledger (GAME-01, GAME-03, GAME-06)

- **D-13:** Build all 35 7800basic 0.41 samples. Phase 2 built `simple`; 34 more come in.
  - **Tests.** `tests/built/tests.cmake` loops over the names in `tests/built/expected.txt`. Each sample gets:
    - `built.<name>` (FIXTURES_SETUP `built_<name>`, labels `built;portable`);
    - `basic.<name>` (labels `basic;portable`), the 600-frame fast-boot chain.
  - **Negative control.** The one `built.samples.self_test` stays.
  - **No subset.** Building them all leaves no subset to choose or defend, and the `pokey` and `rmt` samples feed
    phase 4.

- **D-14:** Close the GAME-01 coverage gap with our own ROMs (owner's choice).
  - **The evidence.** A grep of the pinned package's `.bas` directives shows what the 35 samples use:
    - bitmap modes: only 160A, 320A and 320C, so no read mode 2 and no kangaroo mode;
    - ROM sizes: only 32k, 48k, 128kRAM and `banksets`, so no 16 KB linear, no SuperGame bank 6 and no 144 KB.
    - They do cover DLIs, indirect character mode, and zone heights 8 and 16.
  - **The wording fix.** GAME-01 and roadmap criterion 1 now say the samples **and the project's diagnostic ROMs**
    together cover the list. This commit edits both.
  - **The coverage test.** `basic.coverage` reads `tests/built/coverage.txt` (feature → the ROMs claimed to exercise
    it). It fails if any claimed ROM's observed feature mask (D-10) lacks the feature, or if a required feature has no
    ROM. Fault prevented: coverage claimed that nothing exercises.

- **D-15:** Frame pins live in `tests/built/expected.txt`.
  - **The chain.** The `frame` column (`-` today) becomes the SHA-256 chain `h_i = SHA256(h_{i-1} ‖ frame_i)` over
    frames 1–600 on fast boot. `muhtari-run --hash-frame` finds the first differing frame.
  - **Audio.** The `audio` column stays `-` until phase 4.
  - **Revision guard.** The file carries `# behaviour_revision N`.
    - The check fails unless N equals the library's behaviour revision.
    - `-DWRITE=ON` refuses unless the core's revision is greater than N, and then rewrites the hashes and N.
    - Fault prevented: an unintended hash change pinned silently.
    - During phase 3 the pins are first written at revision 1. The first change to a released pin bumps the revision.
  - **Every leg** matches the committed pins, so `hash-equality` needs no change.
  - **asan.** It runs `basic` in full. If that is measured too slow, `basic` leaves the asan inventory rather than
    gaining a shorter pin.

- **D-16:** The title-screen predicate (GAME-03).
  - **One ledger row per ROM:** `title@120=H`, where H is the fast-boot frame-120 hash. The pin writer rejects a frame
    of a single colour. Fault prevented: pinning a blank screen.
  - **Fast path:** the hash at frame 120 equals H.
  - **BIOS path.**
    - The runner reports the hand-off frame K: the first fetch from cartridge space after the BIOS locks INPTCTRL.
    - The row passes if K is in [300, 340] and H appears within the 300 frames from K.
  - **New evidence on timing.** The OpenBIOS splash shows the Fuji for about 320 frames (`DISPLAYTIME = 10`
    half-second ticks, `FrameCounter32`). "Within 300 frames" can therefore only be met from the cartridge hand-off,
    and GAME-03's wording is clarified to say so in this commit.
  - **Splash skip.** One extra test holds Pause during frames 1–3 under the BIOS and asserts K < 30.
  - **Fallback.** A sample whose BIOS-path sequence differs (state the BIOS did not clear, RNG) falls back to
    `boots@120`, with the reason in its row. The generated doc counts weak rows.

- **D-17:** OpenBIOS is built in CI.
  - **Pin.** A `pins.txt` row `7800openbios-asm` at
    `b8bc745a8744ae3e0d24cc151e63f669f6a87c24` (raw `7800openbios.asm`, SHA-256
    `ff04da42db52480cf4cd5e11d415a6c55a54b72ab24af9b461cd4dff4e422109`), fetched by `scripts/fetch-pin.sh`. KiloParsec
    is never fetched.
  - **Build.** `tests/built/openbios.cmake` replaces the two `incbin` lines with `ds.b 8192,$FF` and `ds.b 896,$FF`
    (it fails unless each matches exactly once), then assembles with the package's `dasm.wasm`. `built.openbios`
    (FIXTURES_SETUP `built_openbios`) checks SHA-256
    `46b5bc2b716a20a1c40e2b920365c29cbab1594d01da94f4567b240181365cb2` on every leg. The advisor reproduced that
    digest on macOS arm64 with the pinned wasmtime, matching CONFORMANCE §2(b).
  - **Kept out of releases.** Add the `$FF`-fill and `$00`-fill digests to `tests/release/firmware.txt`, which its
    line 12 already anticipates, so the firmware scan rejects it. `build/*/built` is never uploaded as an artifact.

- **D-18:** Property and fuzz tests (DEC.44).
  - **Harness.** `tests/prop.h`, about 60 lines: splitmix64, the seed and case printed on failure, a size-halving
    shrink.
  - **Properties:** `prop.split_run`, `prop.peek_pure`, `prop.instances_isolated` and `prop.input_at_vblank`.
    - Each case picks a ROM from: the fuzz seed images, a random-bytes cart, `simple` and `eightysix`.
    - Each case runs at most 8 frames.
  - **Case counts.**
    - Pull request: 100 cases from a fixed seed on all six legs.
    - Nightly: `MUHTARI_PROP_CASES=10000` and `MUHTARI_PROP_SEED=$GITHUB_RUN_ID` on ubuntu x64, under the same test
      names.
  - **`tests/fuzz/run.c`** mirrors `loader.c`.
    - The input is padded or truncated to 49,152 bytes and run as a headerless 48 KB linear cart, fast-booted for 2
      frames.
    - Invariants: every call returns within its bound, two instances agree, and there are no sanitizer findings.
    - Seeds: all $00, all $02 (JAM), all $FF, and 13 splitmix images.
    - Tests: `fuzz.regress.run` and its self test on all legs; libFuzzer for 60 s nightly (`-max_len=49152
      -timeout=2`).

- **D-19:** The ledger, kept minimal (rule 7).
  - **`tests/compat/ledger.csv`** uses the CONFORMANCE §5 schema, with the sample rows and the diagnostic rows.
    Oracle columns stay `-`.
  - **CI.** One ubuntu x64 job runs the `library` preset with `MUHTARI_ROM_DIR=build/ci/built`. It runs
    `library.<id>` for each built file whose SHA-1 is in the ledger and writes `build/library/results.csv`.
  - **`tests/cmake/compat_ratchet.cmake`** (about 80 lines) makes two checks:
    - fresh results must equal the ledger in both directions;
    - a pass→non-pass change against `git show $BASE:tests/compat/ledger.csv` needs a line in
      `tests/compat/allow.txt` dated within 60 days.
    - It reads the base ledger with `git show`, not a second checkout.
  - **`tools/compat_doc.cmake`** (about 60 lines) generates `docs/compatibility.md`, and `policy.compat_doc_fresh`
    regenerates and compares.
  - **Personal dumps.** The owner's run over personal dumps stays optional and local, never a verification item.

- **D-20:** CI tiers.
  - **Every pull request, six legs:**
    - the 35 samples and OpenBIOS built;
    - about 21,000 fast-boot frames;
    - 400 property cases;
    - `diag`, `input` and `fuzz.regress.*`.
  - **Every pull request, one Linux job:** `library`, the ratchet and doc freshness.
  - **be32:** the same `portable` set per pull request while it stays within 5 minutes; otherwise nightly (DEC.46).
  - **wasm32:** report-only, until its own todo promotes it.
  - **Nightly:** 10,000 property cases and `fuzz.run` for 60 s.
  - **New lanes:** report-only for their first 10 runs on `main`, then a timeout of twice the slowest measured run
    (CONFORMANCE §6).
  - **Gate-code budget.** Measure it before planning. ENGINEERING §12 counts policy and hygiene scripts against
    `src/` lines, and rule 7's 10 % applies from this phase. The ratchet and the doc generator count toward it. Test
    drivers and fixture recipes do not.

### Movies, input, boot-to-play, libretro and RetroArch (GAME-04, GAME-05)

- **D-21:** The movie format is `muhtari-movie 1`, owned by the runner.
  - **The parser.** `runner/movie.{c,h}` is a pure `parse(bytes, n, out, err)`, so a v3 `fuzz.movie` is about 15
    lines. It is seeded from nesturbator's validate-before-run reader (MIT, same owner).
  - **Grammar.**
    ```
    muhtari-movie 1
    # comments and blank lines: header only
    rom-sha256 <64 hex, headerless>   optional; a mismatch exits 1 naming both
    port1 proline                     required: proline | joystick | none (never auto)
    port2 joystick                    required
    region ntsc                       optional; pal is refused by name until LIB-04
    boot fast                         optional: fast | bios (bios needs bios-sha256)
    behaviour 1                       optional; a mismatch prints one warning, replay continues
    |...R1.|.....|...BB|
    |U.L..2|U...1|RS.AB|
    ```
  - **Fields.** Each field is positional: character i is the letter or `.`.
    - ProLine is `UDLR12`, joystick is `UDLR1`, and `none` is empty.
    - The console field is `RSP` followed by left and right difficulty as absolute `A`/`B`, so a v3 movie can start
      from a state.
    - Frame k is line k after the header, applied at VBLANK start. There is no RLE.
  - **Forward compatibility.** Unknown or duplicate keys and values outside the set are refused by name. Line 1's
    version changes only for grammar breaks; v3 adds keys.
  - **Precedence.**
    - The header wins over the command line. A conflicting `--port1/--port2/--region/--boot` exits 2.
    - `--frames` defaults to the movie's length. Past the end, every button is released and the last difficulty
      positions are kept.
  - **Lag output.** stdout prints `lag_frames N` after the hash lines.
  - **Hostile input.** The whole file is validated before frame 1.
    - Limits: at most 32 MiB, 1,000,000 frames, 64-byte lines and 32 header lines.
    - Printable ASCII only. LF or CRLF; a bare CR, NUL, BOM or byte ≥ $80 is refused.
    - A failure exits 1 with `muhtari-run: FILE:LINE:COL: <what>, expected <what>`.
    - `tests/runner/test_movie.c` checks about 20 bad inputs, each with its exact position.
    - `runner/exit_status.h`'s failure row gains "movie is malformed".

- **D-22:** The runner's other options:
  - `--boot fast|bios`;
  - `--bios FILE` (required with `--boot bios`; exit 2 if missing);
  - `--region auto|ntsc` (`pal` is refused);
  - `--port1/--port2 auto|proline|joystick|none`.
  - **Exit statuses.** `BIOS_REJECTED_CART`, `UNSUPPORTED_REGION` and `UNSUPPORTED_DEVICE` exit 77; `INVALID_BIOS`
    exits 1. Each enters `exit_status.h` when the code first returns it.
  - **Docs.** `docs/runner.md` gains the options and the movie grammar.

- **D-23:** `ctest -L input` runs in-process on a cart that the test assembles.
  - **The cart.** `tests/input/test_input.c` builds an A78-headered 48 KB linear image in memory: our own 6502 bytes,
    generated at run time and never committed.
    - Each frame it waits for MSTAT VBLANK, reads SWCHA, SWCHB and INPT0–5 once, and stores them in a RAM ring.
    - Its rows are written in movie syntax and parsed by `runner/movie.c` linked into the test, which proves the path
      from spelling to register.
  - **Rows.** Expected values cite A7800-CARTRIDGES-AND-PERIPHERALS §3.1–3.3 and a new `docs/hardware/controllers.md`.
    - every direction on both ports;
    - ProLine in two-button mode;
    - ProLine in one-button mode;
    - a 2600 stick's fire in both modes, including the `io_hazard` count;
    - CTLSWB $14 with no SWCHB write;
    - INPTCTRL TIA-EN disabling two-button mode;
    - Reset, Select, Pause and both difficulty switches.
  - **Lag.** A variant reads only on even frames, so lag = N/2.
    - DMA pointed at $0280 still counts as lag, and so does a peek.
    - `runner.movie.lag` checks `lag_frames 30` over 60 frames through the CLI.
  - **Hardware runs.** A hardware-runnable build of the loop joins the diagnostic set only when owner-console runs
    arrive (v4.0).

- **D-24:** `ctest -R runner.boot_to_play`.
  - **The game.** `spacephysics` (7800basic 0.41, 32k). Among the samples it is the only one that reads joy0 and fire
    and plays sound effects, so phase 4's "not silent" check attaches to the same test. Its controls are measured by
    running it, not by reading its source (PROVENANCE §3 marks the samples "unknown").
  - **The driver.** `tests/runner/boot_to_play.cmake` generates the movies and runs the real `muhtari-run` with
    `--hash-frame` every frame and `--dump-frame` at four frames. `boot_to_play.c` judges with `test_png.c`. F is the
    first input frame, after the title predicate.
  - **Predicates:**
    - (a) the first frame hash that differs from the no-input run lies in [F, F+2];
    - (b) the centroid of the pixels that differ from the no-input run moves at least 4 px across the hold window;
    - (c) the picture is not frozen.
  - **Negative controls**, each returning its exact failure mask:
    - no input (fails a);
    - the same input on port 2 (fails a);
    - the mirrored direction, whose shift sign must flip, which catches swapped SWCHA bits;
    - two pure mutants: a recoloured but static picture (fails b) and a latch one frame late (fails a).
  - **Input at a non-VBLANK tick** is covered by `prop.input_at_vblank` and is not duplicated here.

- **D-25:** The libretro core.
  - **RetroPad layout = ProSystem** (DEC.55):
    - D-pad → the stick, B → button 1, A → button 2, on both ports;
    - port 1 only: Select → Select, Start → Pause, X → Reset;
    - port 1 L and R → left and right difficulty. Press edges become absolute positions, with an OSD message.
    - On a joystick device, both B and A fire button 1.
  - **Devices.** `RETRO_DEVICE_JOYPAD` "Auto (cartridge header)", subclass ProLine, subclass Joystick, plus
    `RETRO_DEVICE_NONE`.
  - **Reset.** `retro_reset` → `muhtari_power_cycle`.
  - **BIOS.** `7800 BIOS (U).rom` and `7800 BIOS (E).rom`, both optional; the adapter logs the SHA-256.
  - **`.info` fields:** `firmware_count 2`, plus `memory_descriptors`, `input_descriptors` and `core_options`.
  - **Core options** (v2 with a v1 fallback): `muhtari_boot` fast|bios, `muhtari_palette`, and
    `muhtari_left_difficulty` and `muhtari_right_difficulty` B|A.
    - The difficulty options give the position at load and after Restart. An option changed mid-game sets the
      position, and L/R edges flip it.
    - The region option waits for LIB-04.
  - **Memory descriptors** (`SET_MEMORY_MAPS`, with `SET_SUPPORT_ACHIEVEMENTS` true):
    - $0040–$00FF → RAM+$0840 and $0140–$01FF → RAM+$0940;
    - $0480–$04FF → RIOT RAM;
    - $1800–$27FF → RAM+0 (`SYSTEM_RAM`);
    - $2800–$2FFF → RAM+$0800, following rcheevos' table while the bus stays undecoded. One test pins each, and
      `docs/libretro.md` says why;
    - unbanked cart RAM at $4000;
    - fixed ROM as `CONST`;
    - registers, $3000–$3FFF and banked spans unmapped.

- **D-26:** `libretro.rcheevos` uses rcheevos' own resolver, fetched by pin, never vendored.
  - **Pin.** A `pins.txt` row `rcheevos 12.1.0` (commit `6755915f2bdf6c11c83b3e0e68c4623a39c0dabe`, the version
    RetroArch 1.22.2 bundles). The planner re-checks the archive URL and SHA-256 the advisor reported. It is fetched
    by `scripts/fetch-pin.sh`, and configure stops if it is missing.
  - **Build.** Compile `src/rc_libretro.c`, `src/rcheevos/consoleinfo.c` and `src/rc_compat.c` with warnings off,
    plus a two-function stub. The advisor's spike linked these against our `libretro/libretro.h` and resolved $1800 →
    RAM+0, $2800 → RAM+$800 and $3000 → NULL.
  - **The test.** Call `rc_libretro_memory_init(..., 51)` on the core's real map for a linear and a SuperGame cart.
    - Assert `count < 32`, since overflow silently drops regions.
    - Assert `total_size == 65536`.
    - For every address with a pointer, its byte equals the internal peek; $2800–$2FFF equals RAM+$800.
    - $3000 and the registers are NULL.
  - **No notice.** Nothing ships.

- **D-27:** The hosted RetroArch job reuses phase 1's `retroarch` job.
  - **Change.** `tests/retroarch/run_retroarch.cmake` gains an optional `-DROM=`, about 4 lines. It boots the
    committed `tests/diag/dma.a78`, which prints its results in on-screen hex, with an empty system directory, so fast
    boot is proven without firmware.
  - **Comparison.** The screenshot at frame 120 is compared with the runner's
    `--dump-frame 120:runner.png --hash-frame 120`, plus one `grep` of that hash against the diagnostic's pinned
    value, so two test cards cannot pass. The existing compare and its negative control stay.
  - **Budget.** At most 15 more lines under DEC.56's 300; the plan measures the total.

### Reconciled across areas
- **Movie header against the API.** The movie fixes the devices per movie (D-21); the API carries a device per frame
  (D-02). The runner writes the movie's devices into every frame's input.
- **PAL.** It is refused everywhere in phase 3: the API status, `--region`, the movie's `region` key, and no libretro
  region option.
- **No public peek or profile.** The prop tests, the input tests and the rcheevos test use internal accessors, so
  nothing is published only for tests.
- **The RetroArch ROM.** It uses the committed diagnostic ROM directly, so no artifact passes between jobs.
- **Seven committed MIT ROMs at most:** `dma`, `cpu`, `dli`, `modes` and up to three board ROMs. The input cart is
  generated at test time.

### Claude's Discretion
- The diagnostics' internal layout, macros and record ids, within the mailbox contract.
- Merging the three board ROMs into fewer images.
- The exact names of the internal accessors, and the order of plans within the tracer order (D-04).
- Whether WR-02 and IN-05 are folded in, if they are free (D-06).

### Folded Todos
- **Close the phase 2 re-review findings when MARIA and the run loop land** (core). WR-05, IN-04 and IN-01 go into
  D-06; WR-02 and IN-05 stay seeds.
- **Close the phase 2 review seeds on the nightly filter and the ROM tables** (ci), in part:
  - IN-01 (the stale API comment) goes into D-01, because phase 3 edits the header;
  - WR-01 (nightly.yml's `paths` filter) goes into whichever plan adds the nightly prop and `fuzz.run` jobs, because
    that plan edits the same filter. The new inputs join it too.
  - IN-02 and IN-03 stay in the todo; phase 3 does not touch those files.


### Deferred Ideas (OUT OF SCOPE)
## Deferred Ideas

- **Profile in `muhtari_media`** (RAM fill including SEEDED, `cpu_phase`, `riot_orb_power_on`, `tia_phase`, the
  ANE/LXA constant, POKEY power-on). Trigger: the first non-test consumer, such as the v3 movie header, runner flags
  for owner-console diagnostics, or a Gym binding that wants RAM randomisation.
- **`get_line_stats`, `get_maria`, `get_cpu` and a public `peek`.** Trigger: runner `--dma-csv`, `--dl-dump` or
  `--trace` (v5), or a debugger embedder.
- **More counters** (`io_hazard`, overrun lines, POKEY init writes). Trigger: the ledger or the lint reads them.
- **Further work by milestone:**
  - PAL: LIB-04.
  - Other devices and the `axes` field: LIB-02.
  - Battery memory and `save_generation`: LIB-03.
  - States, with no size reserved now: STATE-01.
  - A sub-frame input queue: when a TAS or a game needs it.
  - `muhtari_unload`: when an embedder needs it.
- **Diagnostics for v4.0 or the owner's hardware:** #7, #10, #11 and the raster-numbered ROM. A Verilator simulation
  for `cpu_phase`, the HALT latency and DLI timing.
- **Later in the CI and ledger work:** the a7800 nightly differential (oracle columns), JSON `--probe` with
  `--capabilities`, and a hardware-runnable build of the input loop.

### Reviewed Todos (not folded)
- **Arm the instructions-per-frame gate** (ci). Possible now that phase 2 has merged; it gets its own `ci:` pull
  request.
- **Close the re-review's findings on the Dependabot gate and the publish checks** (ci). Its own `ci:` pull request.
- **Limit release rehearsals to pushes that change release inputs** (ci). Revisit if rehearsals become a nuisance.
- **Promote the wasm32 lane into CI required** (ci). Due after 10 green `wasm32` runs on `main`; it gets its own
  `ci:` pull request.
- **Phase 2 review seeds IN-02 and IN-03** (cartdb reload sizes, firmware scan slices). Untouched files; they stay
  in their todo.

</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| GAME-01 | `ctest -L basic` builds the pinned 7800basic samples, checks each ROM's SHA-256, runs each for 600 frames with every frame hash pinned on every platform; with the diagnostic ROMs they cover linear 16–48 KB, SuperGame (RAM, bank 6, 144 KB), every read mode, kangaroo, holey and indirect DMA, DLIs | The 35 sample names and candidate digests are in §Sample Inventory. Probe results: 25 linear 32 KB, 2 linear 48 KB, 5 SuperGame 128 KB + 16 KB RAM, `banksets` refused. So 16 KB linear, bank 6 and 144 KB need the board ROMs (D-14). Chain cost estimate in §CI Cost. |
| GAME-02 | `ctest -L diag`: DMA costs, overrun abort, DLI tick, WSYNC release, slow-access cost equal A7800-MARIA/SYSTEM, as do the per-line statistics | Cost constants and closed form in §MARIA DMA Constants. The T0 fit reproducing 16/24 is in §Pattern 2. Toolchain invocation is in §Diagnostic Toolchain. |
| GAME-03 | Fast boot hand-off; `--boot bios` through CC0 OpenBIOS built in CI; title predicate within 300 frames on both paths, counted from hand-off | The OpenBIOS recipe and digests were reproduced. OpenBIOS hand-off code was read: `bit MSTAT` wait, then INPTCTRL `$06`, then `jmp ($FFFC)`. Splash skip is on SWCHB bit 3 (Pause). The K-definition correction is in §Pitfall 5. |
| GAME-04 | `--movie` drives ProLine and 2600 sticks on both ports and every switch; `ctest -L input` matches A7800-CARTRIDGES-AND-PERIPHERALS §3; lag counted | Pin model in §Pattern 5. Existing `riot_port()` and the PA7 edge detector must see device pins (§Pitfall 7). The nesturbator movie seed is binary and path-based, so the D-21 text grammar is new code. |
| GAME-05 | `runner.boot_to_play`; rcheevos resolver; hosted RetroArch screenshot of a diagnostic ROM | rcheevos pin verified. Console 51 table quoted verbatim. Compile list and the 32-region cap verified. Current `run_retroarch.cmake` shape read (54 lines). |
| GAME-06 | `fuzz.regress.run`, `-L prop` (100/PR, 10,000 nightly), `library` preset regenerating `docs/compatibility.md` | Fuzz and nightly patterns read (`tests/fuzz/tests.cmake`, `nightly.yml`). The inventory mechanism (`cmake/MuhtariTests.cmake`) forces every new test name into the inventory files. The library preset needs its own inventory handling (§Pitfall 11). |
</phase_requirements>

## Project Constraints (from CLAUDE.md / AGENTS.md)

- **Rule 1:** Never open GPL, LGPL or unlicensed emulator source. Reference emulators run only as released binaries. 7800basic's `7800basic.sh` and `7800bas.bat` (GPL-2 scripts, present in `build/pins/7800basic/`) were not read in this session, and the plans must not read them either.
- **Rule 2:** Three deliverables only. No window, audio-device or input-device code.
- **Rule 3:** No ROM, BIOS or save bytes except `tests/roms/manifest.txt` rows. OpenBIOS and the samples stay in `build/`. No personal paths or e-mail addresses. Commits use the noreply identity, and `scripts/hygiene.sh` gates them.
- **Rule 4:** Integer-only, all state in the instance, identical hashes on every platform, every call bounded in emulated time even for a jammed or hostile ROM.
- **Rule 5:** Every behaviour change ships with a test run by `cmake --workflow --preset ci`. The README stays short, docs go in `docs/`, and the header stays terse.
- **Rule 6:** Automation first; the owner verifies nothing by default. The agent reads screenshots and runs RetroArch.
- **Rule 7:** Repository-check code is CMake `-P` or POSIX sh and, from phase 3, stays under a tenth of `src/` lines. Every gate names the fault it prevents. A phase gets at most two review fix rounds and one gap round. Review findings about test or tooling code become seeds. See §Gate-Code Budget: the budget is already exceeded.
- **Rule 8:** One GSD step per session. Never use `--auto` or `--chain`, and never set the `MUHTARI_ALLOW_*` bypasses.
- **Rule 9:** Work on branch `phase/03-games-play-in-retroarch` and merge by PR. `gh pr merge --auto --squash` runs in a shell, never inside GSD command text.
- **Rule 10:** Evidence is a command, an exit status and a run URL, never a committed log.
- **Style:** Small modules, plain control flow, fixed-width types. Public names use `muhtari_`, internal names `muhtari__<module>_`. Hardware comments cite a source. No planning IDs in code, test names or user docs. Follow the ponytail ladder (remove the need, reuse, stdlib, platform, then minimal new code). A permissive copy-paste is preferred over a new dependency.
- **GSD summaries:** each plan SUMMARY has `requirements-completed:` plus a block-style `coverage:` list (id, description, requirement, verification{kind, ref, status}, human_judgment). No flow style and no inline `#` comments in that block.
- **Memory (owner):** `query commit` drops the Co-Authored-By trailer, so commit with git directly. A review fix round makes verification stale, so run execute-phase before ship. Check `.planning/config.json` for model-key drift after GSD steps.

## Architectural Responsibility Map

The tiers here are this project's own (core / runner / adapter / test harness / CI), not web tiers.

| Capability | Primary Tier | Secondary Tier | Rationale |
|------------|-------------|----------------|-----------|
| MARIA registers, DMA burst, line RAM, spans, DLI, MSTAT, per-line stats | core `src/maria.c` | `src/bus.c` (line events, HALT, NMI, RDY) | DEC.22/23: time advances only in bus.c, and MARIA computes per-line |
| Frame loop VBLANK→VBLANK, jam/DMA-off bound | core `src/frame.c` | bus.c deadline | DEC.25/33 |
| Cartridge and BIOS mapping, board switch | core `src/cart.c` (`switch` on board) | bus.c `decode()` gains a cart device | DEC.31; the bus currently returns `DEV_OPEN` for all cart space |
| Fast-boot hand-off, BIOS identification, 2600-path refusal | core (`boot` code in `instance.c` or a new `boot.c`) | bus.c (INPTCTRL observation) | ARCHITECTURE §3/§9 |
| Controller pins, switches, lag | core (`ports` code; RIOT and TIA read paths) | — | DEC.32: pin level |
| Movie grammar, `--boot/--bios/--region/--port` | runner `runner/movie.{c,h}`, `runner/main.c` | — | D-21: verdicts and file I/O stay out of the core |
| RetroPad mapping, options, BIOS lookup, memory maps | adapter `libretro/libretro.c` | host helpers | DEC.55 |
| Diagnostic verdicts, prop harness, rcheevos resolver test | test harness (`tests/diag`, `tests/prop.h`, `tests/libretro`) | `src/internal.h` accessors | D-01: nothing published only for tests |
| Sample and OpenBIOS builds, ROM pins, ledger ratchet, doc generation | CMake `-P` recipes and checks | GitHub Actions jobs | DEC.19/42 |

## Current Codebase Facts (read this session)

These are the seams Phase 3 attaches to. Each is quoted verbatim.

- **The bus has no cartridge device yet.** `src/bus.c:12`: `enum bus_device { DEV_OPEN, DEV_RAM, DEV_MARIA, DEV_TIA, DEV_RIOT };` The `decode()` function returns `DEV_OPEN` for `$0400-$17FF` outside the RIOT RAM and for `$2800-$FFFF`, and with MEN = 0 for every address with A12 set. `muhtari__bus_read` returns `m->bus.open_bus` for `DEV_OPEN`. [VERIFIED: src/bus.c:12, 33-56, 199-213]
- **Cycle lengths.** `src/bus.c:16-19`: `#define BUS_FAST_TICKS 4u`, `#define BUS_FAST_DATA 4u`, `#define BUS_SLOW_TICKS 6u`, `#define BUS_SLOW_DATA 5u`. HALT latency `src/bus.c:81`: `#define HALT_RUN_CYCLES 2u`. [VERIFIED: src/bus.c]
- **HALT and NMI entry points.** `src/bus.c:248`: `int muhtari__bus_request_halt(muhtari *m, uint64_t at_tick, uint64_t until_tick)`. It returns 0 when `b->halt_gate_writes < 2u || b->halt_stage != HALT_NONE`. `muhtari__bus_raise_nmi(muhtari *m, uint64_t at_tick)` returns 0 when `!men(m) || m->bus.nmi_pending != 0u`. [VERIFIED: src/bus.c:248-273]
- **`hold()` advances a whole hold in one step.** `src/bus.c:123` `static void hold(muhtari *m, uint64_t n)` adds `n * BUS_FAST_TICKS` to `ticks` in one go. The RDY path at `src/bus.c:163` (`if (is_read && b->rdy_release != 0u) { hold(m, cycles_until(b->ticks, b->rdy_release)); ...`) jumps straight to the release. Nothing in between fires. [VERIFIED: src/bus.c:123-136, 163-166]
- **TIA WSYNC.** `src/tia.c:7` `#define TIA_LINE_TICKS 456u`; `src/tia.c:10` `#define TIA_WSYNC 0x02u`. A write sets `m->bus.rdy_release`. MARIA's WSYNC at $24 must combine with it as a wire-AND, so the release is the later of the two. [VERIFIED: src/tia.c]
- **Frame constants.** `src/internal.h:13` `#define MUHTARI__HEIGHT_NTSC 242u`; `src/internal.h:18` `#define MUHTARI__TICKS_PER_FRAME_NTSC 119402u`; `src/internal.h:101` `#define MUHTARI__RAM_SIZE 0x1000u`. INPTCTRL bits `src/internal.h:93-97`: `LOCK 0x01u`, `MEN 0x02u`, `EXT 0x04u`, `TIA_EN 0x08u`, `BITS 0x0Fu`. [VERIFIED]
- **Versions.** `include/muhtari.h:12` `#define MUHTARI_API_VERSION 2`; `include/muhtari.h:26` `MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD = 9` (the last status); `include/muhtari.h:45` `uint32_t api;                /* 1 */` (the stale comment D-01 fixes); `src/instance.c:11` `#define MUHTARI__BEHAVIOUR_REVISION 1u`; `src/instance.c:90` `m->profile.riot_orb_power_on = 0xFFu;`. [VERIFIED]
- **Boards the loader accepts.** `src/cart.c:437-441`: `return (b->board != MUHTARI_BOARD_LINEAR && b->board != MUHTARI_BOARD_SUPERGAME) || (b->options & MUHTARI_BOARD_HALT_BANKED_RAM) != 0u;` returns true for a refused board. So linear and SuperGame, including the banked-RAM option without halt banking, load; bankset is refused. [VERIFIED]
- **There is no CPU reset sequence.** `cpu.c` has no RES path (grep for `reset` finds only JAM comments). `muhtari__power_on` zeroes the CPU and bus. The fast-boot hand-off sets the registers directly. The BIOS path needs a RES entry: PC from ($FFFC), S = $FD, I = 1 (A7800-SYSTEM §8). [VERIFIED: src/cpu.c, src/instance.c:97-105]
- **The test inventory is exact.** `cmake/MuhtariTests.cmake` makes configure fail unless the registered tests and labels equal `tests/inventory/<MUHTARI_INVENTORY>.txt`. Every new test name must be added to `ci.txt`, and to `wasm32.txt`/`nofp.txt` where those presets register it. [VERIFIED: cmake/MuhtariTests.cmake:1-80]
- **Size today.** `src/` has 5,535 lines (3,488 without the generated `src/cartdb.c`). `runner/main.c` has 566, `libretro/libretro.c` 314. [VERIFIED: `cat src/*.c src/*.h | wc -l`, exit 0]

## Standard Stack

No new library enters the core, runner or adapter. Everything below is a pinned tool or a test-only fetch.

### Core tools (pinned, already in `pins.txt`)
| Tool | Version | Purpose | Status |
|------|---------|---------|--------|
| wasmtime | 48.0.6 | runs `dasm.wasm`, `7800sign.wasm`, `7800header.wasm` and the 7800basic stages on all six legs | [VERIFIED: pins.txt; local `wasmtime --version` → `wasmtime 48.0.6 (72bc45bd1 2026-10-14)`] |
| 7800basic wasm package | 0.41 (`62ef0a56…a7fe7`) | sample builds; dasm for diagnostics and OpenBIOS | [VERIFIED: pins.txt] |

### New pins (test and CI only; never shipped)
| Pin row | Value | Verified |
|---------|-------|----------|
| `7800openbios-asm` | `https://raw.githubusercontent.com/7800-devtools/7800OpenBIOS/b8bc745a8744ae3e0d24cc151e63f669f6a87c24/7800openbios.asm`, SHA-256 `ff04da42db52480cf4cd5e11d415a6c55a54b72ab24af9b461cd4dff4e422109` (1,498 lines) | [VERIFIED: curl + shasum this session] |
| `rcheevos` | `12.1.0 https://github.com/RetroAchievements/rcheevos/archive/6755915f2bdf6c11c83b3e0e68c4623a39c0dabe.tar.gz c8bb9f66239f1b6c1f66417d26b341ce284ef2475acdc4f6fce0d04ac1678c55` | [VERIFIED: `gh api repos/RetroAchievements/rcheevos/git/ref/tags/v12.1.0` → object sha `6755915f2bdf6c11c83b3e0e68c4623a39c0dabe`; release has **no assets**, so the codeload archive is the only tarball; shasum this session] |

`fetch-pin.sh` already handles `.tar.gz` by stripping the top directory, and a raw file by copying it. [VERIFIED: scripts/fetch-pin.sh:52-66]

The GitHub archive bytes are generated on demand. GitHub has kept them stable since 2023, but that is a policy, not a guarantee [ASSUMED]. If a digest ever drifts, `fetch-pin.sh` fails loudly, and the fallback is one raw-file pin per rcheevos source and header.

### Seeds (copy, not dependency)
| Seed | Use | Finding |
|------|-----|---------|
| nesturbator `runner/movie.{c,h}` @`9f9386c661f6` (MIT, same owner) | D-21 parser | Its interface is binary and reads from a path: `int nesturbator_movie_read(const char *path, nesturbator_movie *movie);` with `uint8_t *masks; /* two uint8_t button masks per frame */`. Only the validate-before-run pattern carries over. The D-21 text grammar and pure `parse(bytes, n, out, err)` are new code. Keep the commit-body seed line (PROVENANCE §5a) only if code is actually copied. [VERIFIED: gh api contents at 9f9386c661f6] |

**Installation (CI steps, each build leg plus asan and be32):**
```bash
scripts/fetch-pin.sh 7800openbios-asm build/pins/openbios
scripts/fetch-pin.sh rcheevos build/pins/rcheevos   # every leg that configures the libretro tests
```

## Package Legitimacy Audit

There are no npm, PyPI or crates packages. The three external artifacts are fetched by exact commit or tag and SHA-256:

| Artifact | Registry | Source Repo | Verdict | Disposition |
|----------|----------|-------------|---------|-------------|
| rcheevos @6755915f (v12.1.0) | GitHub archive | github.com/RetroAchievements/rcheevos (MIT, LICENSE read) | OK (tag and digest verified) | Approved, test-only, never vendored (D-26) |
| 7800OpenBIOS `7800openbios.asm` @b8bc745a | GitHub raw | github.com/7800-devtools/7800OpenBIOS (CC0) | OK (digest matches CONTEXT) | Approved, CI-built only, never committed (DEC.18) |
| 7800basic 0.41 wasm | GitHub release (already pinned) | github.com/7800-devtools/7800basic | OK | Already approved in phase 2 |

**Packages removed:** none. **Flagged:** none. The `gsd-tools package-legitimacy` seam does not apply: there is no package-manager install.

## Sample Inventory (reproduced this session)

The 35 names come from `samples/sizes.ref` at the 7800basic tag (`gh api repos/7800-devtools/7800basic/contents/samples/sizes.ref?ref=03aebb85…`). The package's `samples/` directory also holds `soundeffects`, `splitmodedemobottom`, `splitmodedemotop` and `samplegfx`, which are not in `sizes.ref`. `languagetest/` has 3 `.bas` files, and `languagetest.bas` is the one built. [VERIFIED]

All were built on macOS arm64 with wasmtime 48.0.6 using a shell transcription of `tests/built/build.cmake`'s six stages. Build time for all 35: **7.45 s wall** (M5 Pro). The probe results come from `build/ci/runner/muhtari-run --probe`.

| Sample | .a78 SHA-256 | .bin SHA-256 | Probe |
|---|---|---|---|
| adventurer | 7dd09ae006f4514b0a41f31134dcf6a908c6bd891ee59093a3211460171e855d | e6915fccdf374074708e6025aa885bfd6ba1ba26b31e9f2f0454232fe68eee71 | linear 32K |
| atarivoxspeech | 32e8ba92d7729cf6b6646fbd1d1306a033b79118da54d487a09025f09e193986 | 8af0d20a3b4fa36468ac94cf4c5c51fbefd1c90ea3b5d8c9c6328486b889a9a9 | linear 32K |
| backuptest | 0e2372eb76f2befee28b453713eed70d7ed54b53b83ba9b254b4e8fadd04a8eb | cf0f21cad12ad5e6d84bc0053801780f0d036912b4760fbaa31a10dd80caabbb | linear 32K |
| banksets | **assembly fails** (`banksets.bas.asm (1812): error: Unknown Mnemonic 'lda #>bset_demotext'`) | — | bankset board, refused in phase 3 |
| banner | 556ba917df400482bf96491d23f62252a2640af4d0856fe133f6daa890ac328a | 17dba9493bd16ecdc163fd1ed16404668031948bfc072ebab75c3f71d338e471 | linear 32K |
| boxcollision | 43a6e3fd06abc00d16b013963f3e348b791463cd06fe47de015e23bc6a9c2827 | c11b58f1f93c51b80585335e8ef9600342025f8c763d1346e23575e08cd8c42e | linear 32K |
| colorfade | fc71161b13fefc9b72e7ab264554c5707e8edd3d4d20c7accdb3cd19c2e7b3ac | 766c4b5dcb80e18269db9c83b787d0133759d3591d49c243419c3e116dd962ac | linear 32K |
| doublebuffer | 7e539ce2cec96d036b065dbc4bcf5a145e5ff6c9bd5bd24d1fe43f5d4b1821ea | 452bf198c3b44ebfcf4c6f8e8b357236df70cb341978094b0f39f746e8e19aff | SuperGame 128K + 16K RAM |
| drivingcontrol | 8e3a6110d9744b14bb69c3cb961400bac60a644bb48041e94c742c93276ab35a | e10d5331900db7ab9a3fd9331956bec8708261bcb4f0cc84d25c09a254440106 | linear 32K |
| eightysix | 0f7fa150369eedab473b02679afec9cb169b10da53253745135c9ced2d299c06 | e7775921a91bf08f2f86f086fce3725c61f50f32d3eddee17baaac343e8c3e63 | SuperGame 128K + 16K RAM, pokey_450 |
| fontcheck | b8af60ce3b0afccfceedd5ee57a496491bb6d9181204af85689925a87a99383a | 2a7ab1f2bf4069ab94baa5f8a0e937a7fc73d2ae142625a070f4ba23d3c3f103 | linear 32K |
| helloworld | cd80c5e7b3558c80e8cad2aeee0b15b1757034baa727c575a85ec57e5497b3f0 | 553bb161f4fa99adb9e524b243f8ea8d6a8a342174841f3be09b0a60013e2dae | linear 32K |
| hiscoretable | c728619d520c16bf6e352173a0ae6b56e65d9f74c74b18a8f28f71ce87b0169a | 18eaa34d26f507301d8b3eefd4a392bdfd070378d83f3f07f6f91fdf0475c028 | linear 32K, devices=savekey |
| keypadcontrol | 3bffeb92dad31cbea27e556efa83e29963bb9ec651bc92593b60a6544f3c6a47 | 4a0b930e385b66b77ef6a1bf49e69e79b33e9fadbdf5f0c72d633f485f11eeaf | linear 32K |
| languagetest | cd3bbc1462bc161e8a5e1f256b2a3ae6af77bf51db56ac23d68c724842c84841 | e76c7d3351fbdaa9ab936c9ab0cb506f9035bd3c2ff16cfe028af06b6fee5ad1 | linear 32K |
| mega7800controllers | b7cc2b68934dbc2393aedd0197378d8ae22f9139bf80b9068c04f64032437d93 | d1998d346723628ff18931712091eb46ad9748545d361c26d9bb7c19968fde8c | linear 32K |
| mousedemo | 14c6a8113c9a4a6c6df810ce4c8d35e985a3470b78b61d6f6ee2cee9feb9ff70 | 5e3a1d9174e6b7ade35736bc825c73c60438432cbc9ab77e2a0d24303610eb5e | linear 32K |
| multibuttoncontrollers | 7e2364860a5cf98e9dc344291f47244963adea7af8767e431d5257828d7228e4 | 4a76e3180a846d55d1763c3cd4f2e0d6e7e40ec10df5569cd9621006b6bc5f3d | linear 32K |
| multisprite | 7cde901d2c198d9b94b1f852e7947172d5fccd895418b2b04501e1cf42c07b19 | 374e4436e7c26d25fe442d8f0c4cbbcfda739974647125676d85f4be3f9541b2 | linear 32K |
| onehundredandone | 3102e6f0f6c6f22c530a3fe0b5db70493dc185a113576423920d3a04d49f0dc4 | 8e97c0ab4517e7a4ca0dd08156d52c55f69ddc6bbeafda5eec09e1b4e4d371d0 | SuperGame 128K + 16K RAM |
| plotmapfile | 01f067376bed15d9d73c2a6433dcbbe3dd8bb712432927f864f06f0da14702a6 | 2c76cdfe01b373268ff0b277f695fd796a0a593375350180259d3b3f44f60b12 | linear 32K |
| pokey | a96b1dcea2659a57d3cadee7348469a837bbe1714d8bab78af0c07653d4cdd25 | de0180b203b4402d391d86672a21a48a36ecc52c5970378044279542739ab5a2 | linear 32K, pokey_450 |
| ramcharmap | 7ab16ae69fc365870782554c2088d03fb078beb54b1c6ffb8865bd9bc3d3ede2 | 9d4dbb9918e7cd95afb6d0182f2bd15b44cd1429c108b876d6cdbfafc93ec7e5 | linear 32K |
| rmtramcompressiondemo | bf6816a294e9d7070f824009f077d576293f750e4e7eff4fe59f1a99c755af79 | 478ccc8f2675ac61f7bcac77b923f74b3ac05c112e51ec20d7c66489064e359a | SuperGame 128K + 16K RAM, pokey_450 |
| rmtramdemo | fc65955dd6329f849dfd55376ae1bda426bf1ee59504f3d05e89acb67d1603c0 | d862e6a2498a3da0d0731b559b058f43404757f36cd55e003ad0167f65b0d85e | SuperGame 128K + 16K RAM, pokey_450 |
| rmtromdemo | eccd1a409f8a9d4b2f68011ccb91bf3bac7ef75be9ee14dd6b54b26c7f564374 | 575732d595a9ba023006991dcc2edfbc4876be222e4ec65a114f2ea0d40e76a0 | linear 48K, pokey_450 |
| rotationalcontrols | 26ef6102db792e268c020b9080c3d22cf17087af9fdbf3ed9181c0c6228384dd | 6ed79e0fc2822fdf1719be7e1338f4f21d8d84d1a98b7b95d68769e60cde5f45 | linear 32K |
| screensafe | 0fcf6b197acd1e8aa3afa642d5cd013913a42e301038f842e0b1534acc55c78f | 0314b8484d4db6b31f67bc5003dbc6ce47179afeb1fad18d658457e3f7128596 | linear 32K |
| simple | d09da3ed27fe82e0a4dd2821ff2cb9bde26c9531c59890788acb1cfdc4004ecf | 03a8b7be5d4cd122382582a4f41a94e3104c92191ecdf0a664f93e6802e46b29 | linear 32K (equals the committed `tests/built/expected.txt:5`) |
| smasteroids | 6a75c14fd8e759b64f1faf6c0449d15a6ac3e816aa53cb505fc81e24704d8aa0 | ea064a0e2f00b0b2a0e45402277a2b8e812d413a19fa906d88b1af490536b717 | linear 48K |
| snescontrollers | 897babfe63cbbc13a5121d4ae2d6d3140ad6fdfb8ea581c2391ad5d45ab3d46d | 3321c7447471ea24871e88d5d876ea639d2eba8ff179dfebcf6ab80daa1d45da | linear 32K |
| soundtest | 06588f0777035b9ea9dc67146b3bd0721218b6ee8b270201682216c8d31a44d3 | 9a882db967ebd12fb48f4faed1f07e18bd34aeb5489bdde641b038b8d39a5902 | linear 32K |
| spacephysics | eaea2c6a5bac8c56662f99d9693545f59429f300b2585f7b1c40ec0ea357cc4a | aee8f40a24b4a41473041ca65f112f29564648e1d47614559e4c434d43577c47 | linear 32K |
| trackertest | 407f3a50b19d7badf2d0831b1c201b09dac06485e7e38cb8a0c4344ef0f4c6cc | feb6de8b9b902fed27974e24455259ce09022da969edc4f3372d4ee2ac93ab0c | linear 32K |
| underthehood | ade6d222f15644bf020f818c5a3139a6e5216ba33c1c16e798b340e73b35b100 | fbe85ac916bfc09e6f7cbf8a1b9adf820a8d31847ea49876e122e1eef9a5c4c3 | linear 32K |

All 34 built samples probe with port1 = port2 = proline (rc 0). These digests are candidates. The `built.<name>` tests on all six legs are the authority, and `build.cmake` is the recipe, not this transcription. [VERIFIED: local build, exit 0 per stage, `simple` equal to the committed pin]

**Discrepancies the planner must resolve:**
1. **`banksets`** does not assemble with the six-stage recipe. Its board (bankset, `MUHTARI_BOARD_BANKSET = 6`) is also refused by `cart_refused`. D-13 says "build all 35". Recommendation: register `built.banksets` only if a tool's usage output shows the missing stage (`banksetsymbols.wasm` is in the package, and its usage text may name it). Otherwise expected.txt lists 34, and the ledger gets one `unsupported` row for `7800basic-0.41/banksets` naming "bankset board (LIB-*)". Either way, no `basic.banksets` chain in phase 3: the load returns `ERR_UNSUPPORTED_BOARD`.
2. **`pokey`** builds to `a96b1dce…`, not CONFORMANCE §2(a)'s `290bbd3a…` (built with wasmtime 49.0.1 and possibly 7800basic's own script, which this project never reads). `simple` matches both. Cross-host identity is the requirement (DEC.19), so pin what `build.cmake` produces on all six legs. Record the divergence in `tests/built/expected.txt`'s comment so nobody "fixes" the pin back to CONFORMANCE's value. [VERIFIED]
3. **POKEY-at-$0450 samples** (`eightysix`, `pokey`, `rmt*`): phase 3 has no POKEY, so reads of $0450–$045F return open bus. Their 600-frame pins will change in phase 4 (RANDOM and timers can steer game logic), which needs a behaviour-revision bump there. Say this in the phase 4 seed, not here.

## MARIA DMA Constants (with sources)

These come from A7800-MARIA §4.2/§4.8 ("all derived†, cross-checked against [MAR.14]" Software Guide rev 1320 App. 3). They should be defined in `src/maria.c` as named constants, each with a citation. [CITED: .planning/preparation/A7800-MARIA.md §4.8, quoted]

> Recommended: `MARIA_LINE_TICKS 454` [MAR.09][MAR.20] · `MARIA_DMA_REQ_T 28` [MAR.14][MAR.21] · `MARIA_DMA_ABORT_T 454` (= t 0 of the next line) [MAR.06] · `MARIA_PLA_START 2` · `MARIA_DL_BYTE 2` · `MARIA_GFX_BYTE 3` · `MARIA_CHARMAP 3` · `MARIA_TERMINATOR 8` · `MARIA_TAIL_OTHER 1` · `MARIA_TAIL_LAST 7` · `MARIA_VBE_LEN 7` · `MARIA_ABORT_TAIL_OTHER 3` · `MARIA_ABORT_TAIL_LAST 9` · hole = one slot of the object's kind · `MARIA_DLI_EXTRA 0` (all derived†, cross-checked against [MAR.14]) · `MARIA_FIRST_PIXEL_T 135` [MAR.09] · HALT handshake: SYSTEM, fitted to the empty-DL 16/24 [MAR.18].

- Header 4-byte / 5-byte: **8 / 10** ticks. Indirect character, 1 byte / 2 bytes: **6 / 9** (the map read is 3 with no OFFSET, plus 3 per graphics byte). [CITED: A7800-MARIA §4.2 table]
- Closed form: "HALTRST comes at T0 + 9 + D on an ordinary line and T0 + 15 + D on a last line, where D is the sum of the header, graphics, character and hole costs." The terminator's 8 ticks are inside the 9/15. The arithmetic: the terminator's four DL bytes are addressed at T0+2, +4, +6, +8, then a tail of 1 (or 7) gives 9 (or 15). [CITED: A7800-MARIA §4.2; arithmetic derived here]
- Decode: a header is 4 bytes when `(b1 & 0x1F) != 0`. Otherwise it is 5 bytes when `b1 & 0x40`. Otherwise it is the terminator, `(b1 & 0x5F) == 0`. Width = 32 − W, and a 5-byte header with W = 0 means 32. WM is loaded only by 5-byte headers and persists. IND applies only to its own header. [CITED: A7800-MARIA §2.2]
- Line timeline: t 0 LRC (swap, clear, MARIA WSYNC release, abort deadline), t 28 HALT request on lines 17–258 when DM = 10, t 42 MSTAT edge on lines 16/258, t 43 VBE on line 16 (7 ticks), pixels t 135–454. [CITED: A7800-MARIA §4.1; ARCHITECTURE §2]
- Frame: 263 × 454 = 119,402 ticks. VBLANK start (the frame boundary) is line 258, t 42. [VERIFIED: src/internal.h:18 `119402u`; CITED: ARCHITECTURE §2]
- Fast-boot hand-off: "row 239, col 436 … 8,190 ticks before VBLANK". That is line 240, t 24, and the check holds: 18 × 454 + (42 − 24) = 8,190. [CITED: A7800-SYSTEM C10; arithmetic verified]
- Diagnostic gating values (D-09): header 8/10, graphics 3, indirect 6/9, empty DL 16/24 (measured), direct hole = header + 3, terminator `!(v & 0x5F)`, +2 ticks per slow access (data cycle only), 113 timer ticks per WSYNC line, overrun aborts at LRC with partial objects.

## Architecture Patterns

### System Architecture Diagram

```
 muhtari_set_input ──► pending input (not state) ──┐ latched at VBLANK start
                                                   ▼
 muhtari_run_frame ─► frame.c: loop { cpu_step } until ticks ≥ next VBLANK start (line 258 t 42)
                          │            (jammed CPU: each step reads $FFFF, ≥ 4 ticks → terminates)
                          ▼
                  cpu.c ── bus_read / bus_write (one call = one 4/6-tick cycle)
                          ▼
                  bus.c  begin(): ticks + data ≥ next_deadline ?  ──► events():
                          │                                         ├─ MARIA line events (t 0, 28, 42, 43) ─► maria.c
                          │                                         │     t 0: span flush, LRC swap/clear, MARIA RDY release
                          │                                         │     t 28: request HALT (DMA line, DM=10, gate open)
                          │                                         │     t 42: MSTAT edge (lines 16/258) / frame end check
                          │                                         ├─ HALT reaches c2 ─► maria__dma_line(T0) burst:
                          │                                         │     every fetch at its tick via cart read(by_maria)
                          │                                         │     → HALTRST, stats, DLI ─► raise_nmi
                          │                                         ├─ RDY holds (TIA $02, MARIA $24) — split at line events
                          │                                         └─ NMI latch
                          ▼ decode()
          RAM │ TIA (INPT0-5 ← ports) │ RIOT (SWCHA/SWCHB ← ports, PA7 edge) │ MARIA regs ($20-$3F: span flush before write; MSTAT read)
          CART/BIOS: cart.c switch(board): linear | SuperGame(+RAM, bank6, SG9)  ;  BIOS image while EXT = 0
                          ▼
                  maria.c spans ─► io->video rows (raster 16-257 → rows 0-241), native byte; NULL video skips render
```

### Recommended file layout (additions only)
```
src/maria.c        registers, line events, dma_line burst, line RAM ×2, spans, MSTAT, DLI, line stats + feature mask
src/frame.c        rewritten: argument checks (kept), input latch, cpu_step loop to deadline, audio count from ticks
src/cart.c         + cart read/write/peek with switch(board): linear, SuperGame family; BIOS mapping helper
src/bus.c          + DEV_CART decode, MARIA read/write dispatch, line-event deadlines in rearm(), hold() splitting, MARIA WSYNC
src/ports.c        (or inside tia.c/riot.c if small) ProLine / joystick / none pins, switches, lag flag
src/boot.c         (optional; may live in instance.c) fast-boot hand-off, BIOS validation, 2600-path detection
src/instance.c     + muhtari_load, muhtari_power_cycle, set_input, get_memory, get_counters
include/muhtari.h  API 3 delta exactly per D-02
tests/maria/       maria.dma_line unit tests (hand-built DLL/DL, no CPU)
tests/frame/       frame.jam_ends, frame.dma_off_ends, nmi.one_edge_at_a_time (or in tests/bus)
tests/diag/        *.s, *.cfg, *.a78 (committed, MIT), assemble.cmake, expected.csv, driver.c (muhtari-test-diag)
tests/built/       expected.txt (+ behaviour_revision line, frame chains), coverage.txt, openbios.cmake, tests.cmake loop
tests/input/       test_input.c (in-memory cart), lag variant
tests/prop.h, tests/prop/  four properties
tests/fuzz/run.c   run target + regress
tests/compat/      ledger.csv, allow.txt ; tests/cmake/compat_ratchet.cmake ; tools/compat_doc.cmake ; docs/compatibility.md
runner/movie.{c,h} ; tests/runner/test_movie.c ; tests/runner/boot_to_play.{cmake,c}
docs/hardware/maria.md, docs/hardware/controllers.md, docs/libretro.md (+ embedding.md, runner.md, testing.md updates)
```

### Pattern 1: line events from the tick count, not from DMA
**What:** `maria.c` exposes the tick of its next line event. `bus.c`'s `rearm()` takes the minimum of that tick and the existing HALT/NMI/RDY deadlines. `events()` fires every MARIA event whose tick is at or before the cycle's data tick, in tick order, before the HALT/RDY/NMI logic for that cycle.
**Why:** a ROM with DMA off, or a jammed CPU, still crosses t 42 of line 258, so the frame ends (D-05; DEC.33).
**Hold splitting (required change):** `hold(m, n)` must advance in pieces that stop at each line-event deadline, so a CPU parked in TIA WSYNC or under HALT still sees LRC, MSTAT edges and the next line's t 28. A WSYNC that holds across t 28 is the DLI and WSYNC diagnostics' main case (#4). [VERIFIED current behaviour: src/bus.c:123-136]

### Pattern 2: the DMA burst runs at c2, with T0 = /HALT fall + 6
**What:** at t 28 MARIA asks for HALT. The bus lets c0 and c1 run (`HALT_RUN_CYCLES 2u`). When the bus reaches c2, it calls `maria__dma_line(m, T0, lrc, &stats)` with T0 = the /HALT fall tick + 6. The returned HALTRST sets `halt_until`, and the CPU loses `m = ceil((HALTRST − fall)/4)` cycles (the existing `cycles_until`).
**Why the burst runs at c2, not at the t 28 request:** c0 and c1 may write RAM that the DL fetch reads. Running the burst at the request would read pre-write bytes. With T0 = fall + 6, the first DMA address tick (T0 + 2 = fall + 8) is exactly the end of a fast c1. [derived]
**Fit check (derived, must go into `docs/hardware/maria.md` before the first diagnostic run):**
- Empty DL, ordinary line: HALTRST = T0 + 9 = fall + 15, so m = ceil(15/4) = **4 cycles = 16 ticks**. This matches RevEng's measured 16. [CITED: A7800-MARIA §4.3]
- Empty DL, last line: T0 + 15 = fall + 21, so m = **6 cycles = 24 ticks**. This matches the measured 24.

Measuring T0 from the /HALT fall rather than from t 28 makes both totals independent of the 0–3 tick alignment. The "≈ 6-tick handshake" (A7800-MARIA §4.3) is then a named, fitted constant in one place.
**Consequence:** `muhtari__bus_request_halt(at, until)` currently requires `until` at request time. The planner either (a) adds an internal bus→MARIA call at c2 that supplies `until`, or (b) keeps the signature and calls the burst from the t 28 event, giving up the c0/c1 ordering. Recommend (a). Its unit test: a `STA` to the DL in c1 is visible to that line's fetch. [ASSUMED design; the planner may adjust per D-05 "planner may adjust names"]

### Pattern 3: one burst, per-fetch ticks, abort at LRC
`maria__dma_line` walks the DL. For each fetch it computes the address tick from the §4.2 schedule. A fetch whose address tick is at or after LRC (the next t 0) is dropped. After an abort, HALTRST = LRC + 3 (ordinary line) or LRC + 9 (last line, where the next DLL entry and its DLI still happen). Cart-space fetches go through the board read with `by_maria = 1`. TIA, RIOT and MARIA addresses return the floating bus (the previous fetch's byte) with no side effects. RAM reads directly. Each fetch updates `open_bus`, as on the real bus (C7: "DMA fetches join with MARIA"). [CITED: A7800-MARIA §4.6-4.7, §10.A; ARCHITECTURE §2]

### Pattern 4: spans from line RAM
Render from the last rendered tick up to `now` before every write to $20–$3F and at t 0, taking RM, colour kill and border from the registers at that moment. 160-px cells are written twice, colour kill is `& $0F`, rows are rasters 16–257, and rasters 16–17 are always BACKGRND. When `io->video == NULL`, skip the pixel writes but keep line-RAM state identical, so a NULL-video frame and a video frame leave the same machine state (`prop.split_run` relies on this). [CITED: A7800-MARIA §3, §10.B; ARCHITECTURE §5]

### Pattern 5: ports at pin level
At the access tick, the RIOT port A read returns `(ora & ddra) | (pins & ~ddra)`, where the device pins replace today's "every input pin reads high". The TIA's INPT0–5 drivers take the device pins. Two-button mode on a port = its PB2/PB4 bit is an output (DDR = 1), ORB bit = 0, and INPTCTRL TIA-EN = 0. In two-button mode a ProLine's right button reads on INPT0/2 and its left on INPT1/3 (1 = pressed), and INPT4/5 read 1. In one-button mode either button pulls INPT4/5 to 0. A 2600 stick's fire in two-button mode still shows on INPT4/5 and counts `io_hazard`. The Pause button is SWCHB D3, the same bit OpenBIOS's splash skip reads (`lda SWCHB / and #%00001000`). [VERIFIED: src/riot.c:98-116 current read; CITED: A7800-CARTRIDGES-AND-PERIPHERALS §3.1-3.3; VERIFIED: 7800openbios.asm:958-960]

### Pattern 6: the SuperGame family in one `switch`
| Board | Map | Source |
|---|---|---|
| Linear ≤ 48K | ROM ends at $FFFF | CART §2.1 |
| SuperGame | a write to $8000–$BFFF selects bank `value & (banks−1)` at $8000; bank 0 at power-on; last bank fixed at $C000; second-to-last at $4000 | CART §2.1 |
| + RAM (b2) | 16K RAM at $4000–$7FFF | CART §2.1 |
| + bank 6 (b4) | bank 6 fixed at $4000 | CART §2.1 |
| SG9 / 144K (b3) | bank 0 at $4000; $8000 = 1 + (v & 7); $C000 = bank 8; generalises to 2ⁿ+1 | CART §2.1 |

`bank = value mod banks, never a raw index; RAM index & (size − 1)` (ARCHITECTURE §6, hostile input). [CITED]

### Anti-Patterns to Avoid
- **A const table of board function pointers.** It lands in `.data.rel.ro` under PIC and fails `abi.globals` (DEC.31). Use `switch`.
- **Adding +17 for a DLI, or a 426/430-tick cap.** Both were superseded (ARCHITECTURE §12).
- **Hashing an internal frame buffer.** The core writes straight into the caller's rows. A 77,440-byte buffer in the instance would also enlarge future states.
- **Recording the HALT gate's denial as "no DMA" silently.** D-06 requires the `dma_blocked` statistics flag.
- **Expected diagnostic values copied from the emulator's output** (D-09). Derive them in `docs/hardware/maria.md` first; only `fit` rows may be copied.

## Don't Hand-Roll

| Problem | Don't Build | Use Instead | Why |
|---------|-------------|-------------|-----|
| An rcheevos address-space check | a re-implementation of console 51's map | `rc_libretro_memory_init(..., 51)` from the pin | D-26; the resolver's own merging and 32-region cap are what RetroArch runs |
| SHA-256 / PNG / CRC32 in tests and runner | new copies | `runner/sha256.c`, `runner/png.c`, `runner/crc32.c`, `tests/runner/test_png.c` | already in tree |
| An expected-failure harness | new negative-control plumbing | `tests/cmake/expect_fail.cmake` (19 lines) | used by 11 test files |
| Sample, diagnostic and OpenBIOS assembly | a new toolchain | `tests/built/build.cmake`'s wasmtime stage pattern (`stage()` function) | one recipe on six OSes |
| Downloads | ad-hoc curl | `scripts/fetch-pin.sh NAME DEST` + a `pins.txt` row | verifies SHA-256 before unpacking |
| Firmware leak checks | new scan | `tests/cmake/firmware_scan.cmake` + two rows in `tests/release/firmware.txt` | its comment already reserves them |

## Diagnostic Toolchain (D-07)

`tests/built/build.cmake` runs every tool as `${WASMTIME} run --dir=. "--dir=${pkg}::/b" ${pkg}/<tool>.wasm …` in a fresh per-name directory. Each stage goes through `stage(name … COMMAND …)`, which fails naming the stage and every exit status. [VERIFIED: tests/built/build.cmake:38-63]

For a diagnostic `.s`, the three stages are:
```
dasm.wasm <rom>.s -f3 -o<rom>.bin            # add -I/b/includes only if a 7800basic include is used (avoid: own code only)
7800sign.wasm -w <rom>.bin
7800header.wasm -o -f <rom>.cfg <rom>.bin     # writes <rom>.a78 from the .cfg answers
```
- The `-p20` and `7800filter.wasm` pipe in the sample recipe are 7800basic-specific and not needed for hand-written dasm.
- Reproduced this session: `dasm.wasm ob.asm -f3 -oob.bin` gives exit 0 and "Complete. (0)" for OpenBIOS. The flag form `-o<file>` (no space) is the one that works. [VERIFIED]
- **`tests/diag/assemble.cmake`** reuses the `stage()` shape. It assembles each `tests/diag/*.s`, plus `tests/cpu/decimal/clark.s` with its existing flags, and compares each output with its `tests/roms/manifest.txt` SHA-256 (field 5).
- **Negative control:** one changed digest must fail by name, through `expect_fail.cmake`.
- **Rule 7:** `assemble.cmake` is a fixture recipe and does not count toward the gate-code budget (D-20).

## OpenBIOS (D-17), reproduced

- The source has two `incbin` lines: `7800openbios.asm:164` `     incbin "kiloparsec.bl1"` and `:1466` `     incbin "kiloparsec.bl2"`. Each occurs exactly once. [VERIFIED]
- Replacing them with `ds.b 8192,$FF` and `ds.b 896,$FF` and assembling with `dasm.wasm … -f3` gives 16,384 bytes, SHA-256 **`46b5bc2b716a20a1c40e2b920365c29cbab1594d01da94f4567b240181365cb2`**. With `$00` fill it gives **`1ee4c780397c923447b245fcd75e3a276956a0fd130049d0a85041d058c3ad4f`**. These are the two `tests/release/firmware.txt` rows, as `sha256 <hex> <name>`. Assembly takes under 0.01 s. [VERIFIED]
- The SHA-1s, if firmware.txt prefers SHA-1 like its other rows, are `824815e80873313bcfa6f899d9b627a44d2984d8` ($FF) and `ec7ffa61f9c554a548dc2072ce1322defad6d46d` ($00). [VERIFIED]
- Vectors: bytes $3FF8– are `ff f7 37 fb 00 f8 64 fb`, i.e. $FFF8/$FFF9 = $FF/$F7, NMI $FB37, RESET $F800, IRQ $FB64. Code is at `ORG $C000`, a 16 KiB image mapped over $8000–$FFFF (mirrored twice) while EXT = 0. [VERIFIED: xxd; 7800openbios.asm:157]
- **7800 hand-off** (7800openbios.asm:1110-1128): `lda #$60 / sta CTRL`, then `bit MSTAT / bpl` (wait for VBLANK start), then `ldx #$06 / stx INPTCTRL`, `ldx #$FF / txs`, `cld`, `jmp ($FFFC)`. INPTCTRL `$06` = MEN | EXT, **LOCK clear**. [VERIFIED]
- **2600 path** (7800openbios.asm:1425-1444): ends with `lda #$FD / sta $08 / jmp ($FFFC)`. $08 is a TIA address, so the unlocked latch takes `$FD & $0F = $0D` = LOCK | EXT | TIA-EN with MEN clear. That is the BIOS_REJECTED_CART trigger: "INPTCTRL becomes locked with MEN = 0". [VERIFIED]
- **Splash:** `DISPLAYTIME = 10 ; length of fuji display in half-second ticks`; `FrameCounter32` increments every 32 NMI frames (`and #31`), so about 320 frames. The skip is `lda SWCHB / and #%00001000 / bne CheckFrameCounter / jmp LeaveLoader`, i.e. Pause held (SWCHB D3 = 0). [VERIFIED: 7800openbios.asm:19, 958-965, 1351-1356]

## Code Examples

### API 3 delta (verbatim shape from D-02; names adjustable, shape not)
See the locked D-02 block above. The status values continue the enum after `MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD = 9` (include/muhtari.h:26). Add `_Static_assert` layout rows in `src/internal.h` for each new size-tagged struct, matching the existing pattern (e.g. `_Static_assert(sizeof(muhtari_info) == 40, "info size");`). Where a struct holds a pointer and a `size_t` (`muhtari_media`, `muhtari_span`), the offsets depend on `MUHTARI__P`, as for `muhtari_frame`. [VERIFIED pattern: src/internal.h]

### rcheevos test skeleton (names from the pinned headers)
```c
/* rc_libretro.h at 6755915f: RC_LIBRETRO_MAX_MEMORY_REGIONS 32 (src/rc_libretro.h:39) */
rc_libretro_memory_regions_t regions;
memset(&regions, 0, sizeof regions);
int ok = rc_libretro_memory_init(&regions, &mmap /* from the adapter's SET_MEMORY_MAPS */,
                                 get_core_memory_info_stub, 51 /* RC_CONSOLE_ATARI_7800 */);
/* assert ok, regions.count < 32, regions.total_size == 65536,
   rc_libretro_memory_find(&regions, 0x2800) == ram + 0x800, rc_libretro_memory_find(&regions, 0x3000) == NULL */
```
Signatures verbatim from `src/rc_libretro.h:56-60`: `int rc_libretro_memory_init(rc_libretro_memory_regions_t* regions, const struct retro_memory_map* mmap, rc_libretro_get_core_memory_info_func get_core_memory_info, uint32_t console_id);` and `uint8_t* rc_libretro_memory_find(const rc_libretro_memory_regions_t* regions, uint32_t address);`. The struct (`src/rc_libretro.h:40-46`) has fields `data[RC_LIBRETRO_MAX_MEMORY_REGIONS]`, `size[…]`, `total_size`, `count`. [VERIFIED]

Console 51's table, verbatim from `src/rcheevos/consoleinfo.c:341-349`:
```
{ 0x000000U, 0x0017FFU, 0x000000U, RC_MEMORY_TYPE_HARDWARE_CONTROLLER, "Hardware Interface" },
{ 0x001800U, 0x0027FFU, 0x001800U, RC_MEMORY_TYPE_SYSTEM_RAM, "System RAM" },
{ 0x002800U, 0x002FFFU, 0x002800U, RC_MEMORY_TYPE_VIRTUAL_RAM, "Mirrored RAM" },
{ 0x003000U, 0x0037FFU, 0x003000U, RC_MEMORY_TYPE_VIRTUAL_RAM, "Mirrored RAM" },
{ 0x003800U, 0x003FFFU, 0x003800U, RC_MEMORY_TYPE_VIRTUAL_RAM, "Mirrored RAM" },
{ 0x004000U, 0x007FFFU, 0x004000U, RC_MEMORY_TYPE_SAVE_RAM, "Cartridge RAM" },
{ 0x008000U, 0x00FFFFU, 0x008000U, RC_MEMORY_TYPE_READONLY, "Cartridge ROM" }
```
rcheevos also calls $3000–$3FFF "Mirrored RAM". D-25 leaves that range unmapped, and the D-26 test pins $3000 → NULL. The docs should say this choice is deliberate.

**Compile notes:**
- `rc_libretro.c` includes `rc_libretro.h` (which has `#include <libretro.h>`, angle brackets, so add `libretro/` to the include path), `rc_consoles.h`, `rc_compat.h` and `rhash/rc_hash_internal.h` (→ `rc_hash.h`, `md5.h`).
- The only rhash symbol referenced is `rc_hash_iterator_error` (`src/rc_libretro.c:758`). That is the stub need the advisor's spike found.
- Include dirs: `<pin>/include`, `<pin>/src`, `<pin>/src/rhash`, `libretro/`. [VERIFIED: grep this session]
- Compile the three files with warnings off on GCC, Clang and MSVC. Use per-target options; the project's warning flags come from `muhtari_warnings()`, so don't call it on this target.

## Common Pitfalls

### Pitfall 1: line events skipped during holds
**What goes wrong:** a CPU in TIA WSYNC across t 28, or a long HALT, makes `hold()` jump past LRC, MSTAT and the next line's DMA request. Lines are lost, and the frame boundary is missed by a line.
**How to avoid:** split holds at the next MARIA event (Pattern 1). Unit test: STA WSYNC at t 20 with `tia_phase` placing the release after t 28. The line's DMA must still run, and MSTAT must still flip.

### Pitfall 2: the DMA burst before c0/c1
See Pattern 2. **Warning sign:** a `STA` into a DL entry in the instruction straddling t 28 shows one line late.

### Pitfall 3: the first frame after load
The fast-boot hand-off is at line 240, t 24, which is 8,190 ticks before the first VBLANK start. With DEC.25 framing, either:
- **(a)** frame 1 is an 8,190-tick partial frame (about 54 audio samples, `(8190·88)/13125`, rows 0–223 not scanned since load), or
- **(b)** the first `run_frame` runs 127,592 ticks. Option (b) breaks `max_audio_samples = 801`, which is published API.

**Recommendation:** (a). Rows not scanned since load or power cycle are written as $00, deterministic and documented in `docs/embedding.md`. The audio count becomes `(rem + elapsed_ticks·88)/13125` instead of the full-frame constant, so `frame.c`'s arithmetic generalises from `MUHTARI__TICKS_PER_FRAME_NTSC` to elapsed ticks. title@120 is then counted from this frame 1.

The BIOS path has the same issue from power-on: MARIA's beam position at power-on (row 0? [ASSUMED]) decides frame 1's length. Pick it explicitly and document it.

**What MARIA reads on hand-off lines 240–257:** with RAM zero-filled, DPP $1F84 points at zeros. The rest of that frame shows whatever those zero DLL entries fetch (DL at $0000 means TIA-space reads, i.e. floating-bus bytes). The result is deterministic and pinned, but it is not the BIOS's Fuji. That is acceptable, but state it in `docs/hardware/maria.md`. [ASSUMED behaviour; derived from C10 values]

### Pitfall 4: CTRL $00 at BIOS power-on is DMA test mode
CTRL powers on $00 (A7800-SYSTEM §8), i.e. DM = 00. Choice 3.B treats DM 00/01 as DMA off and counts it. Make sure the "DMA test mode" stats flag and counter don't fire on every BIOS-path frame before the BIOS writes $7F. Otherwise the ledger and coverage see noise. [CITED: A7800-MARIA §3.2, Choice 3.B]

### Pitfall 5: D-16's "after the BIOS locks INPTCTRL" is wrong for OpenBIOS
OpenBIOS writes `$06` (no LOCK) before `jmp ($FFFC)`, and the NTSC BIOS leaves `$16`, also unlocked (A7800-SYSTEM §5). **Define K as** the frame of the first opcode fetch from cartridge space after INPTCTRL EXT goes 0→1. In OpenBIOS's case that is the `jmp ($FFFC)` target fetch. The runner's K report and the ledger rows must use this definition. Report it as a wording fix in the plan that implements it. This is new evidence from the source, not a reopened decision. [VERIFIED: 7800openbios.asm:1124-1128]

### Pitfall 6: the splash-skip test window
OpenBIOS reads SWCHB D3 only in its per-frame loop after its RAM clear and copies. If that loop's first iteration falls after frame 3, D-16's "hold Pause during frames 1–3 … K < 30" fails for a harness reason. Measure the first-check frame when the BIOS path first runs. If it is later than 3, hold Pause for frames 1–N with N measured and stated in the test comment. [ASSUMED risk; code read, timing not run]

### Pitfall 7: input must reach the PA7 edge detector
`riot.c` computes PA7 (`SWCHA` b7 = port 0 right) through its edge detector (`src/riot.c:81`: `return (uint8_t)((((r->ora & r->ddra) | (uint8_t)~r->ddra) >> 7) & 1u);`). Device pins must feed this too. Input applied at VBLANK start that changes PA7 must update `pa7_detect`, and set `pa7_flag` on the active edge, at that tick. [VERIFIED: src/riot.c:81]

### Pitfall 8: lag counted by a peek or by DMA
A lag frame is one with no **CPU** read of SWCHA/SWCHB ($280/$282 + mirrors) or INPT0–5 (+ mirrors). DMA reads of TIA and RIOT space float with no side effect, and a peek has none either, so neither may clear the lag flag (D-23 tests both). Put the flag in the CPU-read path of `bus.c`, not in `riot_read`/`tia_read`, which a peek might share.

### Pitfall 9: per-frame SHA-256 cost on slow legs
Native SHA-256 of 600 frames of 77,440 bytes took 0.145 s (0.215 s total minus 0.071 s without hashing) on an M5 Pro, about 320 MB/s. 21,000 chained frames ≈ 1.6 GB, about 5 s native. `docs/testing.md` measures be32 (`qemu-ppc`) at 10.7× and wasmtime at 2.5×, which gives about 55 s of hashing alone on be32, before emulation (CPU about 30k cycles/frame plus up to 242 bursts). The be32 job currently takes 45 s inside a 5-minute limit, so it is at risk. Measure after the `simple` tracer and decide per D-20/DEC.46 (be32 moves nightly if it exceeds 5 minutes). [VERIFIED: local timing, exit 0; extrapolation ASSUMED]

### Pitfall 10: rcheevos on every configuring leg
`configure stops if it is missing` (D-26). Every job that configures with tests registered must fetch it: six build legs, asan, be32, wasm32 (if the libretro tests are registered there; `wasm32.txt` excludes libretro tests, so check before fetching), plus the library job. Forgetting asan or be32 fails configure, not a test. [VERIFIED: CI job list in .github/workflows/ci.yml]

### Pitfall 11: the library preset and the exact inventory
The inventory check (`cmake/MuhtariTests.cmake`) needs a fixed `tests/inventory/<preset>.txt`. `library.<id>` tests are registered per built file present. Give the `library` preset its own inventory generated from the ledger rows whose tier is `pr`. Configure then fails if a built file is missing (no skip), which matches DEC.43. [VERIFIED mechanism; design ASSUMED]

### Pitfall 12: the ratchet's base needs history
`git show $BASE:tests/compat/ledger.csv` (D-19) needs the base commit in the clone. `actions/checkout` defaults to depth 1, so the library job needs `fetch-depth: 0`, or a `git fetch --depth=1 origin $BASE_SHA` step. CONFORMANCE §5 also suggests running main's ratchet script so a PR cannot loosen its own judge. D-19 is locked to `git show`; note that it reads the base *ledger*, not the base *script*. [CITED: CONFORMANCE §5]

### Pitfall 13: nightly path filter
`nightly.yml`'s `pull_request.paths` lists `tests/fuzz/**` and the CPU/cart files. New inputs (`src/maria.c`, `src/frame.c`, `src/bus.c`, `tests/prop*`, `tests/fuzz/run.c`) must join it, and WR-01 is folded into that plan. [VERIFIED: .github/workflows/nightly.yml:18-37]

## Gate-Code Budget (rule 7) — measured

| Count | Lines | Command (exit 0) |
|---|---|---|
| `src/*.c src/*.h` | **5,535** | `cat src/*.c src/*.h \| wc -l` |
| … without generated `src/cartdb.c` (2,047) | **3,488** | |
| All `tests/cmake/*.cmake` + `scripts/*.sh` | **2,586** (1,955 + 631) | `wc -l tests/cmake/*.cmake scripts/*.sh` |
| Narrow: hygiene/policy-registered `tests/cmake` scripts (api_names 50, budgets 31, ci_required 90, dependabot 56, firmware_scan 225, no_float 59, no_skip 38, notices 57, pins 49, rom_hash_coverage 69, single_version 49, timeouts 34) + `scripts/{hygiene,check-title,classify-changes,apply-repo-settings}.sh` (177+25+27+152) | **1,188** | per-file `wc -l` this session; phase 2's SUMMARY counted 1,332 with 16 scripts |
| Budget at 10 % | **554** (or 349 without cartdb) | |

**Finding:** the budget is already exceeded by 2.1× (narrowest definition, cartdb counted) up to 7.4× (broadest, cartdb excluded). Phase 3 adds about 140 gate lines (the D-19 ratchet ~80 and doc generator ~60) and perhaps 2,000–3,000 `src/` lines. The narrow ratio then lands near 15–18 %, still over. Rule 7 cannot be met by this phase's plans alone, and this is not a locked decision the planner can override.
**Recommendation:**
1. One plan task records the tally (command + numbers) and the definition used.
2. Fold `tools/compat_doc.cmake` into the ratchet script where possible, or keep each under its stated size.
3. Route the shortfall to the owner as a scoping question at ship (rule 6 lets the agent decide most things, but rule 7 is the owner's rule). Name the candidate cuts: `firmware_scan.cmake` 225, `apply-repo-settings.sh` 152 and `perf_compare.cmake` 126 are the largest. Record a seed for the v1.0 tune-up (ENGINEERING §12).

## CI Cost Estimate

| Item | Basis | Estimate |
|---|---|---|
| 35 sample builds per leg | 7.45 s total on M5 Pro (measured). Phase 2's `simple` alone took 2.44 s on the slowest leg (macOS x64, run 39195320937). | ~35–90 s per leg serial; ctest runs them in parallel (`"jobs": ""`) |
| 21,000 fast-boot frames + chain | hashing ~5 s native (measured basis); emulation unmeasured | ~10–30 s native; be32 ×10.7, asan ×3 |
| OpenBIOS build | < 0.01 s | negligible |
| Current slowest build leg | 84 s (ci.yml comment, run 39197902638); job timeout 5 min | the margin is the risk for be32 and asan |

Measure after the tracer plan. Timeouts follow the "2 × slowest of N runs with run ID" comment convention.

## State of the Art (project-local)

| Old (design spine / prep) | Current | Impact |
|---|---|---|
| DLI +17 ticks; 426/430 cap | 0 ticks; abort at LRC, tails +3/+9 | `maria.c` has no cap constant |
| `cpu_phase` "extracted by RTL simulation in P3" | derived from C10 = 0, fitted, RTL in v4.0 (D-12) | amend ARCHITECTURE §6 row |
| VBE merged with line 16's DMA | own 7-tick fetch at line 16 t 43; content from raster 18 | rows 0–1 always BACKGRND |

## Assumptions Log

| # | Claim | Section | Risk if Wrong |
|---|-------|---------|---------------|
| A1 | T0 = /HALT fall + 6 reproduces 16/24 for every alignment | Pattern 2 | diag `dma` gating rows fail; refit changes the behaviour revision |
| A2 | Running the burst at c2 (internal bus→MARIA call) is the right seam | Pattern 2 | c0/c1 writes are seen one line late |
| A3 | Frame 1 after load = 8,190-tick partial frame with unscanned rows $00 | Pitfall 3 | title@120 pins shift if changed later |
| A4 | Power-on beam position on the BIOS path (row 0) | Pitfall 3 | BIOS-path K shifts by a frame |
| A5 | OpenBIOS reaches its SWCHB check within frames 1–3 | Pitfall 6 | splash-skip test needs a longer hold |
| A6 | be32 stays within 5 minutes with 21,000 chained frames | Pitfall 9 | `basic` moves nightly on be32 (DEC.46) |
| A7 | GitHub codeload archive bytes stay stable for the rcheevos pin | Standard Stack | the fetch fails loudly; switch to raw-file pins |
| A8 | The `banksets` recipe gap is a missing tool stage | Sample Inventory | 34 samples + an unsupported row |
| A9 | Library-preset inventory derived from ledger rows | Pitfall 11 | inventory check conflicts |

## Open Questions

1. **Rule 7 budget.** It cannot be met as worded. Owner scoping is needed at ship (see the Gate-Code Budget section). The plans should proceed and record the numbers.
2. **`banksets`.** Is it built, or listed 34 + 1 unsupported? Recommend 34 + an unsupported ledger row, unless a tool's usage output reveals the stage.
3. **Hand-off frame convention (A3/A4).** Lock this in the tracer plan, document it in `docs/embedding.md`, then pin.
4. **BIOS mapping under MEN = 0.** With MEN = 0 (the 2600 decode), A12 = 1 is "cartridge", and the BIOS must answer there while EXT = 0, or power-on cannot read $FFFC. Recommendation: while EXT = 0, every A15 = 1 address returns the BIOS image (mirrored); with MEN = 0, A12 = 1 maps to the BIOS's top 4 KiB as the 2600 decode sees it (A7800-SYSTEM §4: "with the BIOS mapped the cart sees A12, A14 and A15 low"; MiSTer mirrors over $8000–$FFFF). Verify with OpenBIOS booting `simple`. [ASSUMED]
5. **Cart RAM storage.** D-03 says "Only the ROM copy is allocated". Put the SuperGame 16 KiB RAM in the same allocation as the ROM (ROM ‖ RAM), so `get_memory(CART_RAM)` is a stable pointer and `core.no_alloc_after_load` holds. Alternatively use a fixed 16 KiB array in the instance; that costs every instance 16 KiB. Recommend the joint allocation.

## Environment Availability

| Dependency | Required By | Available (local) | Version | Fallback |
|------------|------------|-----------|---------|----------|
| wasmtime | sample, diag, OpenBIOS builds | ✓ (`build/pins/wasmtime`) | 48.0.6 | — |
| 7800basic package | same | ✓ (`build/pins/7800basic`) | 0.41 | — |
| CMake | everything | ✓ | 4.4.3 | — |
| C compiler | core | ✓ | Apple clang 21.0.0 | — |
| RetroArch | hosted job | ✓ locally (`/Applications/RetroArch.app`); CI uses the pinned DMG | CI 1.22.2 | — |
| qemu-ppc, clang-21 sanitizer runtime | be32, fuzz | ✗ locally; installed by CI apt steps | — | run in CI or Docker (`docker` present) |
| Network to github.com raw/codeload | pins | ✓ | — | — |

## Validation Architecture

`workflow.nyquist_validation` is `false` in `.planning/config.json`, but the orchestrator asked for this map, so it is included.

### Test Framework
| Property | Value |
|----------|-------|
| Framework | CTest via `muhtari_add_test` (cmake/MuhtariTests.cmake), exact inventories in `tests/inventory/*.txt` |
| Quick run | `ctest --preset ci -L <label>` after `cmake --build --preset ci` |
| Full suite | `cmake --workflow --preset ci` (`workflow.test_command`) |

### Success criterion → tests
| Criterion | Req | Behaviour | Test (label / name) | Exists? |
|---|---|---|---|---|
| 1 | GAME-01 | each sample builds byte-identically | `built.<name>` (`built;portable`), `built.samples.self_test` | only `built.samples` + self_test today; Wave 0 loop |
| 1 | GAME-01 | 600-frame chain equals pin on every leg; revision guard | `basic.<name>` (`basic;portable`); revision check in the same script; `hash-equality` | ❌ |
| 1 | GAME-01 | coverage of modes and boards from observed feature masks | `basic.coverage` | ❌ |
| 1 | GAME-02 | DMA costs, overrun, DLI tick, WSYNC release, slow access vs expected.csv + per-line stats | `ctest -L diag` (`diag.<rom>` via `muhtari-test-diag`), `diag.expected.self_test` (one changed value), `diag.rebuild` (`built`) + its self test | ❌ |
| 1 | GAME-02 | burst closed form, HALTRST, line RAM, stats | `maria.dma_line*` unit tests | ❌ (Wave 0 tracer) |
| 1 | — | jam and DMA-off frames end; one NMI edge | `frame.jam_ends`, `frame.dma_off_ends`, `nmi.one_edge_at_a_time` | ❌ |
| 2 | GAME-03 | title@120 fast; K∈[300,340] and H within 300 frames of K on BIOS; splash skip K < 30 | `basic.<name>` title row check (or `title.<name>`), `built.openbios` (`built`), `bios.splash_skip` | ❌ |
| 3 | GAME-04 | register values per controller mode and switch; lag | `ctest -L input` (`input.*`), `runner.movie.lag` | ❌ |
| 3 | GAME-04 | movie grammar, ~20 bad inputs with positions | `runner.movie_parse` (test_movie.c) | ❌ |
| 3 | GAME-05 | boot-to-play predicates a/b/c + 4 negative controls with exact masks | `runner.boot_to_play` | ❌ |
| 4 | GAME-05 | rcheevos resolver over the core's real maps (linear + SuperGame) | `libretro.rcheevos` | ❌ |
| 4 | GAME-05 | RetroArch screenshot of `tests/diag/dma.a78` at frame 120 equals the runner's, plus a hash grep | `retroarch` CI job (not ctest) | job exists; `-DROM=` ❌ |
| 5 | GAME-06 | fuzz replay + self test on all legs; nightly 60 s | `fuzz.regress.run`, `fuzz.regress.run.self_test`; nightly `fuzz.run` | ❌ |
| 5 | GAME-06 | four properties, 100 cases/PR, 10,000 nightly | `ctest -L prop` (`prop.split_run`, `prop.peek_pure`, `prop.instances_isolated`, `prop.input_at_vblank`) | ❌ |
| 5 | GAME-06 | ledger equals fresh results; ratchet; doc fresh | `library` job: `library.<id>`, `compat_ratchet.cmake`; `policy.compat_doc_fresh` | ❌ |
| 5 | — | release on merge | release-please on the squash merge; confirm the release PR is green (rule 9) | existing |
| — | API | every new name compiled and exported | `abi.api_v3` (tests/api/v3.c), `abi.symbols`, `abi.api_version` | v3 ❌ |

### Sampling Rate
- **Per task commit:** `ctest --preset ci -L <touched label>`.
- **Per plan:** `cmake --workflow --preset ci`, plus `asan` for core changes.
- **Phase gate:** CI green on all legs, with run URLs recorded (rule 10).

### Wave 0 Gaps
- [ ] `tests/maria/test_maria.c` + `tests/maria/tests.cmake`: hand-built DLL/DL harness (no CPU).
- [ ] An internal accessor block in `src/internal.h` for line statistics, the peek and the profile (D-01 internal only).
- [ ] `tests/prop.h` (≈60 lines).
- [ ] The `tests/built/tests.cmake` loop over expected.txt rows (replaces the single-`simple` registration).
- [ ] Inventory rows in `tests/inventory/ci.txt`, `wasm32.txt`, `nofp.txt` for every new test.

## Suggested Tracer-First Plan Order

This follows D-04 and the granularity "coarse" setting. The planner may merge or split plans.
1. **MARIA burst unit tests + `docs/hardware/maria.md` derivations.** `maria.c` registers, schedule constants, `dma_line`, line RAM and stats; `maria.dma_line*` tests against §4.2's closed form. All expected values are written before any ROM runs.
2. **Tracer to `simple`.** Bus cart device (linear), MARIA register dispatch, line events plus hold splitting, burst at c2, MARIA WSYNC, DLI to NMI, `frame.c` loop, fast-boot hand-off, API 3 header with `tests/api/v3.c` (load and power_cycle real; set_input latching), runner loads a file, `basic.simple` chain pinned at `# behaviour_revision 1`; `frame.jam_ends`, `frame.dma_off_ends`, `nmi.one_edge_at_a_time`; bus.md, sally.md and embedding.md edits (D-06).
3. **The other 34 samples.** `built.<name>` loop and pins, `basic.<name>` chains; measure CI time and decide asan/be32.
4. **Diagnostics.** `assemble.cmake` and `diag.rebuild` (with clark.s), `dma`/`cpu`/`dli` ROMs, `muhtari-test-diag`, `expected.csv`.
5. **SuperGame family + `modes` + board ROMs.** Feature mask, `coverage.txt`, `basic.coverage`.
6. **Input.** Ports, switches, lag, `tests/input`, `docs/hardware/controllers.md`; runner `movie.{c,h}`, `--movie/--port1/--port2`, `test_movie.c`, `runner.movie.lag`.
7. **BIOS path.** BIOS mapping and validation, RES, the 2600-path refusal, `openbios.cmake`/`built.openbios`, firmware.txt rows, `--boot/--bios/--region`, the K report, title predicates on both paths, splash skip.
8. **libretro.** RetroPad layout, devices, options v2/v1, BIOS names and SHA-256 log, memory maps; the rcheevos pin and `libretro.rcheevos`; `.info` fields; the RetroArch job `-DROM=` (≤ 15 lines).
9. **`runner.boot_to_play`** (spacephysics), with its negative controls.
10. **Prop and fuzz.** `tests/prop.h` and the four properties; `tests/fuzz/run.c` and regress; nightly jobs plus the path filter (WR-01).
11. **Ledger.** `ledger.csv`, the library preset and job, `compat_ratchet.cmake`, `compat_doc.cmake`, `docs/compatibility.md`, `policy.compat_doc_fresh`.
12. **Close.** Gate-code tally, docs budgets, then ship the PR (0.3.0 via release-please).

## Security Domain

`security_enforcement` is `false` in `.planning/config.json`, so this section is omitted. Hostile-input bounds (ROM, BIOS size and vector, movie limits, fuzz) are covered under Pitfalls and in D-03/D-18/D-21.

## Sources

### Primary (HIGH)
- Repository files read this session: `src/{bus,cpu,frame,instance,internal.h,tia,riot,cart}.c/h`, `include/muhtari.h`, `tests/built/{build.cmake,tests.cmake,expected.txt}`, `pins.txt`, `scripts/fetch-pin.sh`, `cmake/MuhtariTests.cmake`, `.github/workflows/{ci,nightly}.yml`, `tests/retroarch/run_retroarch.cmake`, `tests/fuzz/tests.cmake`, `docs/hardware/bus.md`.
- `.planning/preparation/{A7800-MARIA,A7800-SYSTEM,ARCHITECTURE,CONFORMANCE,A7800-CARTRIDGES-AND-PERIPHERALS,DECISIONS}.md`.
- 7800OpenBIOS `7800openbios.asm` @b8bc745a (CC0), fetched and assembled.
- rcheevos @6755915f (MIT): `src/rc_libretro.{c,h}`, `src/rcheevos/consoleinfo.c`, `include/rc_consoles.h`.
- 7800basic `samples/sizes.ref` @03aebb85 (data list); 34 samples built locally with the pinned tools.
- nesturbator `runner/movie.h` @9f9386c661f6.

### Secondary / Tertiary
- None from web search. GitHub archive stability is an assumption (A7).

## Metadata

**Confidence breakdown:**
- Stack and pins: HIGH. Every digest was reproduced.
- Architecture: MEDIUM. The seams were read; the T0 fit and the c2 burst are derived.
- Pitfalls: HIGH for those found in code; MEDIUM for the timing estimates.

**Research date:** 2026-10-19
**Valid until:** 2026-11-18 (pins are frozen; the CI-time estimates go stale after the first measured run)
