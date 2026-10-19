# Phase 3: Games play in RetroArch - Context

**Gathered:** 2026-10-19
**Status:** Ready for planning

<domain>
## Phase Boundary

Phase 3 makes cartridges run. It adds:

- **MARIA.** Registers, DMA as one burst per line with per-fetch ticks (DEC.23), line RAM and pixel output, the DLI,
  MARIA's WSYNC at $24 and MSTAT.
- **The run loop.** `muhtari_run_frame` runs SALLY from VBLANK start to VBLANK start (DEC.25), bounded for any ROM
  (DEC.33), and ends a frame even when the CPU is jammed or DMA is off.
- **Boards.** Linear 16–48 KB and SuperGame, with RAM, bank 6 and 144 KB variants.
- **Boot.** The fast-boot hand-off by default (DEC.37), and `--boot bios` through the CC0 OpenBIOS built in CI
  (DEC.18).
- **Input.** A two-button ProLine stick and a one-button stick on both ports, the console switches, and lag frames
  (DEC.32).
- **Public API version 3.** Load, power cycle, input, memory spans and counters (D-01).
- **Runner and libretro.** `muhtari-run --movie`, `--boot`, `--bios`, `--region` and `--port1/2`, the libretro
  RetroPad layout, BIOS names, core options and rcheevos memory descriptors.
- **Evidence.** The 7800basic samples and the project's own diagnostic ROMs with frame-hash pins on every platform,
  `ctest -L diag`, `-L input`, `-L prop`, `fuzz.regress.run`, `runner.boot_to_play`, the hosted RetroArch job on a
  diagnostic ROM, and the compatibility ledger with a generated `docs/compatibility.md`.

Requirements GAME-01 to GAME-06. Merging publishes 0.3.0.

Not in this phase:
- Phase 4: TIA and POKEY sound. Audio stays silent, and the `audio` pin columns stay `-`.
- v2.0 (LIB-*): PAL games (LIB-04; PAL is refused in phase 3), the other peripherals and boards, the HSC, SaveKey
  and battery.
- v3.0: save states, movie recording and Playstead's continuation protocol.
- v4.0: the MiSTer RTL referee and owner-console hardware sessions that settle the fitted timings (DEC.41).

</domain>

<decisions>
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

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**

### Phase scope
- `.planning/ROADMAP.md` (Phase 3): the goal and the five success criteria, with criterion 1 amended per D-14.
- `.planning/REQUIREMENTS.md`: GAME-01 to GAME-06 (GAME-01 amended per D-14, GAME-03 clarified per D-16).
- `.planning/PROJECT.md`: the constraints.
- `.planning/preparation/DECISIONS.md`: DEC.18, 19, 22–27, 31–37, 40–47, 51, 55–58, and the open questions ($2800,
  242 vs 243 rows, HALT latency, ORB).
- `.planning/phases/02-the-cpu-matches-the-public-vectors/02-CONTEXT.md`:
  - D-01 and D-02: the probe API and statuses;
  - the bus entry points for MARIA;
  - the deferred list.

### Hardware and design
- `.planning/preparation/A7800-MARIA.md`:
  - §3.3: first pixel;
  - §4: DMA costs, holes, terminators, abort tails;
  - §6: video geometry;
  - §10: modelling choices.
- `.planning/preparation/A7800-SYSTEM.md`:
  - C4–C10: slow cycles, RDY, NMI, $2800, ORB, the hand-off;
  - §8: the profile.
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md`:
  - §2.1–2.3: linear and SuperGame boards;
  - §3.1–3.4: controllers, two-button mode, console switches.
- `.planning/preparation/ARCHITECTURE.md`:
  - §2–§3: time, the dispatcher, modules;
  - §4: API rules;
  - §5: I/O contracts;
  - §6: the profile (to be amended per D-12);
  - §11: build order;
  - §12: changes from the spine;
  - §13: open conflicts.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md`:
  - §1–2: the libretro mapping, BIOS names, rcheevos;
  - §4: runner options;
  - §5: input and sub-frame input.
- `docs/hardware/bus.md`, `docs/hardware/sally.md`, `docs/hardware/riot.md`, `docs/hardware/cartridge.md`.

### Conformance, engineering and CI
- `.planning/preparation/CONFORMANCE.md`:
  - §2(a)–(b): 7800basic and OpenBIOS builds;
  - §4: the diagnostics and the mailbox;
  - §5: the ledger;
  - §6: CI tiers;
  - §7: fixture admission.
- `.planning/preparation/ENGINEERING.md`:
  - §3: tests, fuzzing, the property harness;
  - §4: CI;
  - §8: the Ir gate and zero allocations;
  - §12: the gate-code budget.
- `ASSET_POLICY.md`, `PROVENANCE.md` (§3 sample licences, §4 seeds, §6 firmware).

### Rules
- `AGENTS.md`: rules 1–11 and Style.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md`
- `.planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md`

### Code
- `include/muhtari.h`, `src/internal.h`, `src/bus.c`, `src/tia.c`, `src/riot.c`, `src/cart.c`, `src/frame.c`,
  `src/instance.c`, `src/testcard.c`
- `runner/main.c`, `runner/exit_status.h`, `libretro/libretro.c`, `libretro/muhtari_libretro.info.in`
- `tests/built/{build.cmake,tests.cmake,expected.txt}`, `tests/cpu/decimal/`, `tests/fuzz/`,
  `tests/retroarch/run_retroarch.cmake`, `tests/libretro/`, `tests/runner/test_png.c`, `tests/release/firmware.txt`,
  `tests/roms/manifest.txt`
- `pins.txt`, `scripts/fetch-pin.sh`, `.github/workflows/ci.yml`, `nightly.yml`
- `docs/embedding.md`, `docs/runner.md`, `docs/testing.md`, `docs/libretro.md`

### External sources (cite by name and pin)
- **7800basic v0.41 @03aebb85206f53046425c19a236f65ae80d9f498** (`7800basic-0.41-wasm.tar.gz`): `dasm.wasm`,
  `7800sign.wasm`, `7800header.wasm`, `samples/`. Its compiler is GPL-2, used only as a tool.
- **7800-devtools/7800OpenBIOS @b8bc745a8744ae3e0d24cc151e63f669f6a87c24** (CC0): `7800openbios.asm`.
- **RetroAchievements/rcheevos v12.1.0 @6755915f2bdf6c11c83b3e0e68c4623a39c0dabe** (MIT): `src/rc_libretro.c`,
  `src/rcheevos/consoleinfo.c` (console 51).
- **szTheory/nesturbator** (MIT, same owner): `runner/movie.c`.
- **libretro docs:** the ProSystem core page (layout and BIOS names only; the core's source is GPL and is not opened).
- **wasmtime v48.0.6**.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `muhtari__cart_load` and the probe in `src/cart.c` become `muhtari_load`'s first half. The loader already
  validates before it allocates.
- `muhtari__bus_request_halt` and `muhtari__bus_raise_nmi` in `src/bus.c` are MARIA's entry points (D-06).
- TIA WSYNC in `src/tia.c`, and the RIOT ports in `src/riot.c`, where the controllers attach.
- `tests/built/build.cmake` is the wasmtime/dasm recipe for the samples, the diagnostics and OpenBIOS.
- `tests/fuzz/loader.c`, `seeds.c` and `tests.cmake` are the pattern for `fuzz/run.c`.
- `tests/runner/test_png.c`, and the `--hash-frame` and `--dump-frame` runner options, serve boot-to-play.
- `tests/retroarch/run_retroarch.cmake` and its compare, for D-27.
- `tests/cmake/firmware_scan.cmake` with `tests/release/firmware.txt`, for OpenBIOS.
- `host/status_text.c`, for the new statuses' messages.

### Established Patterns
- Size-tagged structs; append-only statuses; validate, then apply; zero allocations after load.
- An exact test inventory per preset, negative controls, and no self-skip.
- Link-time substitution and `src/internal.h` access for tests.
- Pins in `pins.txt` fetched by `scripts/fetch-pin.sh`, with configure stopping when one is missing.
- Report-only for 10 runs on `main` before a lane gates.

### Integration Points
- `src/maria.c` (new) and `src/frame.c` (rewritten) under `muhtari_run_frame`.
- `include/muhtari.h` (API 3), `docs/embedding.md`, `tests/api/v3.c`.
- `runner/main.c` and `runner/movie.{c,h}`.
- `libretro/libretro.c` and its `.info`.
- `ci.yml`: the samples and OpenBIOS on every leg, plus the `library` job. `nightly.yml`: prop and `fuzz.run`.
- `tests/compat/`, `tools/compat_doc.cmake`, `docs/compatibility.md`.
- Line counts today: `src/` about 5,535 lines (`cpu.c` 1,474, `cartdb.c` 2,047, `cart.c` 756), `runner/main.c` 566,
  `libretro/libretro.c` 314.

</code_context>

<specifics>
## Specific Ideas

- **Gate-code budget is tight.** `tests/cmake`, `scripts`, hygiene and policy together are about 3,000 lines against
  about 5,500 in `src/`. Whatever counts as gate code under ENGINEERING §12, phase 3 adds little to it.
- **The coverage evidence comes from the core.** The feature mask reports what MARIA actually did, not what a sample's
  source declares.
- **The diagnostics measure numbers.** Verdicts stay outside the ROM, so the same bytes can later run on a flash cart
  (DEC.40).
- **Watch for:**
  - expected values copied from the emulator (D-09);
  - the on-screen hex needing a working MARIA, so at first bring-up the driver trusts only the mailbox;
  - the rcheevos 32-region cap.

</specifics>

<deferred>
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

</deferred>

---

*Phase: 03-games-play-in-retroarch*
*Context gathered: 2026-10-19*
