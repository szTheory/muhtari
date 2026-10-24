---
title: "Architecture: time model, modules, public API, I/O contracts and build order"
summary: "The technical shape of the muhtari core: one MARIA-tick timeline advanced only by bus.c, a per-line DMA burst, lazy devices, the module and seed map, the public C API and its versions, the I/O, determinism, state, observability and boot contracts, the v1.0 build order, and how each research conflict is settled."
read_when: "Before planning or writing core, runner or libretro code; before adding a module, API call, profile field or hash; whenever two research files disagree."
updated: 2026-10-24
---

# muhtari architecture

## Where to look
| Question | Section |
|---|---|
| What ships, and what does the core never do? | 1 |
| What is a tick, what happens at each tick of a line, and who advances time? | 2 |
| Which module owns what, seeded from where, landing when? | 3 |
| Which public functions, struct rules and version numbers? | 4 |
| Video, audio and input formats and hashes | 5 |
| Determinism rules, profile fields, hostile inputs, fuzz targets | 6 |
| Save states, battery media, `.srm` | 7 |
| Line statistics, inspection, hooks, counters | 8 |
| Fast boot, BIOS boot, refusals | 9 |
| Seams kept for later features | 10 |
| v1.0 build order | 11 |
| What the research changed in the design spine; how each research conflict is settled | 12, 13 |

Citations: [ARCH.01] A7800-SYSTEM, [ARCH.02] A7800-MARIA, [ARCH.03] A7800-AUDIO, [ARCH.04] A7800-CARTRIDGES-AND-PERIPHERALS, [ARCH.05] HOSTS-AND-INTEGRATION, [ARCH.06] ENGINEERING, [ARCH.07] CONFORMANCE, [ARCH.08] PROVENANCE, [ARCH.09] design spine; "§n" and "Cn" are that file's sections and choices. Choices read **Rec** — **Alt** (strongest alternative) — **Flip** (would change if). Internal names below drop the `muhtari__<module>_` prefix [ARCH.06 §10].

## 1. Shape
| Deliverable (directory) | What it is | Built from |
|---|---|---|
| Core library (`include/muhtari.h`, `src/`) | one terse public header (≤ 24 KB) and one module per chip (§3); external symbols `muhtari_*`, cross-file internals `muhtari__*` | C17; links nothing beyond `memcpy memmove memset memcmp` [ARCH.06 §2] |
| `muhtari-run` (`runner/`) | headless runner: `--probe`, frame/audio/state hashes, `--dump-frame`, `--bench`; later saves, movies, trace; one exit-status table [ARCH.05 §4] | core + host helpers |
| `muhtari_libretro` (`libretro/`) | the sibling's 352-line adapter shape: one instance, test card without content, XRGB8888 (RGB565 under `USE_RGB565`), mono → stereo batch audio, options, memory maps [ARCH.05 §1–2] | core + host helpers + vendored `libretro.h` |
| Host helpers (`host/`), outside the determinism boundary | `palette.c` maps native bytes to RGB (six a7800 V7 tables, default WARM, plus an MIT generator) [ARCH.02 §7]; `composite.c` (v4) is the opt-in artifact filter over native 320-px frames [ARCH.02 §8]; never hashed | used by runner and adapter |

The core never opens files, reads clocks or the environment, starts threads, uses floating point (the adapter's aspect ratio is the only float, outside the core), allocates after `muhtari_load`, prints, aborts or exits, or keeps mutable globals [ARCH.06 §2, §10]. Bytes come in; caller-owned buffers go out.

## 2. Time and stepping
Coordinates: line n starts at t = 0 = MiSTer column 412 of raster n − 1; t counts ticks inside the line [ARCH.01 §2][ARCH.02 §1].

| Constant | NTSC | PAL | Source |
|---|---|---|---|
| Tick = one MARIA clock (crystal ÷ 2) | 315/44 MHz = 7,159,090.9 Hz | 7,093,788 Hz | [ARCH.01 §1] |
| CPU cycle | 4 ticks, data at the φ2 fall (tick 4); 6 ticks when TIA or RIOT is selected, or on every cycle while MEN = 0, data at tick 5 | same | [ARCH.01 §1, C4] |
| Line | 454 ticks = 113.5 fast cycles; the TIA (clock 2 ticks) counts its own 456-tick line | same | [ARCH.01 §1][ARCH.03 §1–2] |
| Frame | 263 lines = 119,402 ticks; 39,375,000/656,711 Hz | 313 lines = 142,102 ticks; 3,546,894/71,051 Hz | [ARCH.01 §1] |
| HALT requested (DMA lines) | lines 17–258 (= rows 16–257 at column 440); VBE on line 16 | lines 17–308; line 16 | [ARCH.02 §4.1][ARCH.01 §2] |
| MSTAT D7 (VBLANK); frame boundary = VBLANK start | sets at line 258 t 42 = the frame boundary; clears at line 16 t 42 | 308; 16 | [ARCH.01 §2] |

| t | In-line event (one table in `maria.c`, fired by `bus.c`) |
|---|---|
| 0 | line origin: MARIA WSYNC releases RDY; DMA force-stop (LRC); line-RAM buffers swap and the new write buffer clears; span flush; last pixel of raster n − 1 |
| 28 | HBLANK; on a DMA line with DM = 10 and the HALT gate open, MARIA requests HALT → `halt_window()` |
| 42 | HSYNC, raster n begins; MSTAT D7 edge on lines 16 and 258 (PAL 308) |
| 43 | line 16 only: VBE fetch of DPPH/DPPL and the first DLL entry, 7 ticks |
| 135 + x | pixel x (0–319) of raster n; x = 319 lands on the next t 0 |

- **Counters.** One `uint64_t ticks` and one φ2-cycle counter per instance; RIOT and POKEY count φ2, which keeps running through HALT and WSYNC [ARCH.01 §1, §6][ARCH.03 §4]. 454 ≡ 2 (mod 4), so the CPU-clock grid moves 2 ticks against t every line, plus 2 per slow cycle; its parity class is fixed per power-up and is the profile field `cpu_phase` [ARCH.01 §1, §8].
- **One deadline dispatcher.** `bus.c` is the only place time advances. Each `bus_read`/`bus_write` is one CPU cycle of 4 or 6 ticks chosen by address; before its data tick it fires, in tick order, every deadline at or before that tick (the line events above, TIA-WSYNC release, the DLI edge, POKEY IRQs, frame end) from one cached `next_deadline`, so the fast path costs one compare.
- **Lazy devices.** MARIA renders pixel spans from line RAM up to `now` before any write to $20–$3F and at t 0, taking RM, colour kill and border per span [ARCH.02 §10]; TIA audio catches up on TIA writes and at frame end; POKEY catches up on access, before every slow cycle (so the φ2 → tick map stays piecewise linear) and at frame end [ARCH.03 §4]; the RIOT timer, TIA dump/latch and port pins are computed when read.
- **DMA burst.** HALT freezes any cycle, so the burst can start inside a read or a write call. `maria_dma_burst(T0, LRC)` performs every fetch of the line at its exact tick from the PLA-derived schedule [ARCH.02 §4.2]: PLA start 2; DL/DLL byte 2; header 8 / 10; direct graphics byte 3; indirect character 6 / 9; a hole costs one slot of its object's kind and ends the object; terminator 8; tail 1 (other lines) or 7 (a zone's last line, which fetches the next DLL entry). Cart fetches call `cart_read(addr, tick, by_maria = 1)`; TIA, RIOT and MARIA addresses return the open-bus byte with no side effects [ARCH.02 §4.7]. There is no tick budget: a fetch whose address tick reaches LRC (the next t 0) is dropped, and HALTRST follows at LRC + 3 or LRC + 9, the next DLL entry and its DLI still taking place [ARCH.02 §4.6]. The call returns HALTRST, the DLI, the OFFSET/zone update and the line statistics; running the whole line at once is exact because the halted CPU cannot change anything the fetches read [ARCH.02 §10].
- **`halt_window()`** in `bus.c` is the only home of the fitted HALT rule: /HALT falls at the first φ2 fall at or after the request (+0–3 ticks, +0–5 with a slow cycle in flight); SALLY runs the cycle starting there (c0) and the next (c1), freezes c2 mid-cycle for reads and writes alike, and after two release samples completes c2, having lost exactly m cycles (m = whole cycles /HALT stays low); the ≈ 6-tick handshake makes an empty line cost 16 ticks and a zone's last line 24 [ARCH.01 §3][ARCH.02 §4.3]. Constants are named and cited; a refit bumps the behaviour revision. The HALT gate opens after two unlocked writes in the TIA chip-select range, RMW dummy writes included [ARCH.01 §3].
- **RDY** is MARIA WSYNC ($24 and mirrors, low until t 0) wire-ANDed with TIA WSYNC ($02, low until the TIA's own HBLANK, which slides 2 ticks per MARIA line). It holds reads only: the first read after the write waits, then runs at the fast rate; an RMW straddling it completes both writes [ARCH.01 §3, C5].
- **NMI.** A DLI drives NMI low from the second φ1 after /HALT rises, for two cycles, never while MEN = 0 [ARCH.01 C6]. SALLY latches an edge at the end of every φ2 (also under RDY) and polls in φ2 of the last cycle: an edge present by the end of the penultimate cycle is taken after this instruction, a later one after the next; a taken branch without page cross polls early; an NMI before T5 φ2 of BRK or IRQ takes its vector [ARCH.01 §3].
- **Frames and bounds.** A frame runs from one VBLANK start to the next, exactly 119,402 (PAL 142,102) ticks; `set_input` takes effect at its first tick. Nothing a ROM does stops time: WSYNC and HALT advance ticks, a jammed CPU idles to the next deadline, `muhtari_run` (v5) takes a tick budget, and a loaded state's ticks are capped at 2^62 [ARCH.10].

## 3. Modules
| Module | Owns | Interface | Seed or own [ARCH.08 §4] | Lands |
|---|---|---|---|---|
| `cpu.c` | SALLY: registers, 256 NMOS opcodes, NMOS decimal, NMI/IRQ edge sampling (§2), JAM, ANE/LXA constant | `cpu_step()`; memory only through `bus_read`/`bus_write`, one call per cycle | szTheory/nesturbator `src/cpu.c` @`9f9386c661f6` (MIT, same owner; the published main PROVENANCE §4 pins): add decimal and the per-cycle NMI rules, drop 2A03 paths | P2; then the A78 probe, CC0 table and loader fuzz target (moved from P3) |
| `bus.c` | `ticks`, φ2 counter, decode table and mirrors [ARCH.01 §4], 4/6-tick cycles, open-bus byte, INPTCTRL latch and HALT gate, RDY sources, deadline dispatcher, `halt_window()` | `bus_read`/`bus_write(addr)`, `bus_peek(addr)` (no side effects) | nesturbator bus pattern | P2 |
| `riot.c` | 6532 ports, DDRs, timer on φ2, flags, 128 B RAM | `riot_read`/`riot_write(addr)` at the data tick | own (datasheet; `M6532.sv` facts only) | P2 |
| `maria.c` | registers, CTRL half-cycle latch, MSTAT (one cycle stale), DPP/DLL/DL engine, DMA burst and schedule, two line RAMs, pixel spans, DLI, line statistics | `maria_dma_burst(T0, LRC)`, `maria_render_to(tick)`, `maria_read`/`maria_write(reg, tick)` | own (Software Guide, MiSTer facts; MAME and a7800 cross-check only) | P3 |
| `tia.c` | INPT0–5 dump/latch, D7/D6 drivers, TIA WSYNC → RDY, 456-tick line counter; two audio channels | `tia_read`/`tia_write(reg, tick)`, `tia_audio_catch_up(tick)` | own (`TIA.sv` facts only) | P2 WSYNC; P3 inputs; P4 audio |
| `ports.c` | one device per port at pin level: ProLine, 2600 stick, none | `ports_pins(port, tick)` | own | P3; other devices v2 |
| `cart.c` + generated `cartdb.c` | probe (A78 v3/v4, headerless footer test, 2600 classification), SHA-1 table, header lint, board choice and dispatch | `cart_read(addr, tick, by_maria)`, `cart_write(addr, v, tick)`, `cart_peek(addr)` | own; table generated from MAME `hash/a7800.xml` (CC0) [ARCH.08 §7] | P3 |
| `board_*.c` | linear; SuperGame family (SG, +RAM, SG9/EXROM, bank 6, banked RAM, ≤ 1 MiB); POKEY windows; later Activision, Absolute, Fractalus, bankset, XBoard/XM, Souper | called only from `cart.c`'s switch | own | P3; POKEY P4; v2/v4 |
| `pokey.c` (×2) | timers, joins, two-tone, high-pass, polys, RANDOM, init, IRQ only when the v4 header bit is set | `pokey_read`/`pokey_write(reg)`, `pokey_catch_up(phi2)` | own from the Altirra manual; MAME v4.9 read-only cross-check; BSD-3 fallback seed a7800 `pokey.cpp` @`7a2afdc1ea08` | P4 |
| `synth.c` | per-tick merge of level changes, mixer tables, band-limited kernel, DC blocker, output-rate fraction | `synth_level(tick, source, level)`, `synth_render(end_tick, out)` | nesturbator `src/synth.c` | P1 silence; P4 |
| `boot.c` | HLE hand-off, BIOS identification and mapping, detection of the BIOS's 2600 path | `boot_hle()`, `boot_bios_identify(bytes)` | own: facts only, no Atari bytes [ARCH.08 §6] | P3 |
| `state.c` | visitor and 64-byte header (§7) | `state_visit(mode)` | nesturbator `src/state.c` | v3 |
| `instance.c`, `testcard.c`; runner, adapter, CI, hygiene | public API, config and profile, frame driver, counters, test card; §1 | `include/muhtari.h` | szTheory/nesturbator @`9f9386c661f6` | P1 |
| `host/palette.c` | six V7 tables (NTSC/PAL × 25.7°/26.7°/27.7°) + MIT generator | `host/muhtari_host.h` | a7800 `src/mame/drivers/a7800.cpp` L705–1452 (BSD-3) | P1 |
| Later | `hsc.c`, `savekey.c` (v2); `xm.c` (v2, YM v4); `ym2151.c` (v4); runner `patch.c`, movies (v3); `host/composite.c` (v4) | — | ymfm OPM (BSD-3) seeds `ym2151.c`; the rest own (lifting MiSTer's MIT composite structure would need a §4 row first) | v2–v4 |

Boards dispatch through one `switch (board_kind)` in `cart.c`: a const table of function pointers lands in `.data.rel.ro` under PIC and fails `abi.globals` [ARCH.06 §2]. **Rec** the switch — **Alt** pointers stored in the instance at load (they must then be re-derived after a state load, never serialised) — **Flip** Cachegrind shows the switch costing Ir once the board count grows.

## 4. Public C API
```c
muhtari_status muhtari_create(const muhtari_config *cfg, muhtari **out);  /* allocator hook, audio rate num/den, synth mode */
void           muhtari_destroy(muhtari *m);
muhtari_status muhtari_probe(const void *image, size_t n, muhtari_cart_info *out); /* pure: kind, board, devices, region, hashes */
muhtari_status muhtari_load(muhtari *m, const muhtari_media *media); /* ROM, BIOS or NULL (fast boot), region, ports, profile; copied */
muhtari_status muhtari_power_cycle(muhtari *m);                      /* battery kept; the console Reset is a switch in the input */
muhtari_status muhtari_set_input(muhtari *m, const muhtari_input *in); /* latched; takes effect at the next VBLANK start */
muhtari_status muhtari_run_frame(muhtari *m, muhtari_frame *io);     /* VBLANK start → VBLANK start; NULL buffers skip that output */
muhtari_status muhtari_get_info(const muhtari *m, muhtari_info *out); /* 320 × height, fps and rate as num/den, PAR, region */
muhtari_status muhtari_get_memory(muhtari *m, uint32_t kind, muhtari_span *out); /* SYSTEM_RAM, RIOT_RAM, CART_RAM */
muhtari_status muhtari_get_battery(const muhtari *m, muhtari_battery_info *out); /* raw span, layout, committed-change token */
muhtari_status muhtari_get_hsc_rom(const muhtari *m, muhtari_const_span *out); /* installed read-only ROM/stub */
size_t         muhtari_state_size(const muhtari *m);                 /* constant from load to unload */
muhtari_status muhtari_save_state(const muhtari *m, void *buf, size_t n);
muhtari_status muhtari_load_state(muhtari *m, const void *buf, size_t n); /* validated in full before anything is applied */
uint8_t        muhtari_peek(const muhtari *m, uint16_t addr);        /* bus view without side effects */
muhtari_status muhtari_get_cpu(const muhtari *m, muhtari_cpu_view *out);
muhtari_status muhtari_get_maria(const muhtari *m, muhtari_maria_view *out); /* registers, DPP/zone/DL pointers, beam */
muhtari_status muhtari_get_line_stats(const muhtari *m, muhtari_line_stats *out); /* every line of the last frame */
muhtari_status muhtari_get_counters(const muhtari *m, muhtari_counters *out); /* lag frames, io_hazard, overruns … */
muhtari_status muhtari_get_versions(muhtari_versions *out);         /* library, API, ABI, behaviour, state, battery */
muhtari_status muhtari_set_hooks(muhtari *m, const muhtari_hooks *h); /* v5 */
muhtari_status muhtari_run(muhtari *m, const muhtari_budget *b, muhtari_stop *why); /* v5: bounded in ticks */
```
| Rule | Detail (sibling conventions [ARCH.10]) |
|---|---|
| Size tag | every boundary struct starts with `uint32_t size`; smaller than the library's → `MUHTARI_STATUS_ERR_STRUCT_SIZE`; larger accepted only if the extra bytes are zero |
| Fields | fixed-width integers; enums carried as `uint32_t`; no `bool`, bit-fields or pointers into core state; rates as num/den |
| Statuses | append-only, specific codes (`INVALID_IMAGE`, `UNSUPPORTED_SIZE`, `UNSUPPORTED_2600`, `UNSUPPORTED_BOARD`, `BIOS_REJECTED_CART`, `STATE_*` …); warnings are flag bits in `muhtari_cart_info` (`HEADER_V4_IGNORED`, `HEADER_CORRECTED`) [ARCH.04 §1.3–1.4] |
| Errors | validate, then apply: an error changes no state, and a failed `load` keeps the previous content |
| Memory | allocator hook {alloc, free with size, user}; allocation only in `create` and `load`, zero calls afterwards (`core.no_alloc_after_load`) [ARCH.06 §8] |
| Buffers, threads | video, audio and state buffers are caller-owned; ROM and BIOS bytes are copied at load; an instance is single-threaded and instances share nothing |

| Number | Changes when | Read from |
|---|---|---|
| Library `x.y.z` | each release (release-please, `version.txt`) [ARCH.06 §5] | generated `MUHTARI_VERSION_*`, `muhtari_get_versions` |
| `MUHTARI_API_VERSION` (integer) | +1 with every header addition (function, field, status, enum value); never decreases | the macro at compile time; an embedder refuses a runtime value below the macro it compiled with |
| ABI (SOVERSION) | an incompatible signature or layout change; appending to a size-tagged struct is not one | soname, `muhtari_get_versions` |
| Behaviour revision | any frame, audio or state hash changes for the same inputs, with a reason | state and movie headers, `--capabilities`; Playstead records it beside its SHA-256 pin of the runner archive [ARCH.05 §4] and refuses a continuation whose movie or state carries another |
| State format; battery layout | the state byte layout; the battery span layout | state header; `muhtari_info` and `--capabilities` |

## 5. I/O contracts
| Stream | Contract |
|---|---|
| Video | one `uint8_t` per pixel = native MARIA colour (hue b7–4, luma b3–0); 320 × 242 (NTSC) / 292 (PAL) = rasters 16–257 / 16–307; 160-px pixels written twice; colour kill applies `& $0F`; borders not emitted; rasters 16–17 always show BACKGRND; frame hash = SHA-256 over these bytes; PAR 6:7 (NTSC) / 1.0396 (PAL), DAR 960/847 / 1.1393, reported uncropped; no content → test card of all 256 colours [ARCH.02 §6, §10][ARCH.05 §1] |
| Audio out | mono `int16_t` at the configured rate num/den (default 48,000/1); NTSC 48 kHz = 88 samples per 13,125 ticks (800 or 801 per frame), PAL 4,000 per 591,149 ticks (961 or 962); the fraction carries across frames; never padded [ARCH.03 §1, §8] |
| Audio model | TIA ×2 on its own 456-tick lattice (B at 64/288), POKEY ×2 on φ2; owned Q24 node-current tables (TIA 2n/(n+30) at 2.5 V full scale, 31 cells; POKEY saturation, 825 cells; current weights 85:342, Q12 gain 4096). Corrected 2026-10-20: scale uses the complete finite transient/blocker/rounding bound, not a DC-only clipping claim. P/N=10865505/2476897 over Q23, L1=13342402/8388608, M=18360 gives ≥1 dB headroom. See [production proof](../../docs/hardware/audio.md). Historical MIT seed Q15 precision is replaced by regenerated 16-tap/64-phase Q23: 1024 int32 cells, 4096 bytes, sum 8388608, integration Q23→Q30×128; identity `exact-q23-16x64-dc20`, single Q30 blocker R=1070934451. [Dated D-08 amendment](../phases/04-sound/04-RESEARCH.md#feasibility-resolution-explicit-minimal-d-08-revision); [ARCH.03 §2–3, §8] |
| Audio cost, hashes | cost per output sample is bounded: ≤ 150 merged changes (one per tick, derived from 13,125/88) in EXACT mode; BOX_BINS (exact tick integration into 1/8-sample bins, ≤ 8 kernel adds per sample) is the fallback for hostile 1.79 MHz POKEY, added if that ROM costs > 10 % of frame time on the low-power target [ARCH.03 SYN-2]; primary hash = SHA-256 over LE {u64 tick, u8 source, u16 level} per per-channel DAC change (runner hook tap, independent of mixer and rate); PCM hash secondary, keyed by (rate, synth mode) [ARCH.03 HASH] |
| Input | per port a device (v1: PROLINE, JOYSTICK, NONE) from A78 bytes 55/56, a host override wins; `muhtari_input` = per-port buttons and axes + console switches (Reset, Select, Pause momentary; difficulty as absolute A/B positions); applied at VBLANK start; devices return pin levels at the access tick from RIOT PA output/DDR, effective PB2/PB4 and TIA dump/latch; two-button mode iff PB2 (PB4) is an output driving 0 and INPTCTRL TIA-EN = 0; a 2600 stick's fire there still shows on INPT4 and counts `io_hazard`; lag frame = no CPU read of SWCHA/SWCHB ($280/$282 + mirrors) or INPT0–5 (+ mirrors), peeks and DMA excluded [ARCH.04 §3][ARCH.05 §1, §5] |

## 6. Determinism, profiles and safety
Integer-only (`hygiene.no_float`, `nofp` preset, `abi.undefined`); no mutable globals or statics (`abi.globals`); byte-wise little-endian serialisation; no punning, bit-fields or packed structs; a UBSan lane; hashes byte-identical on six platforms plus `be32` (`hash-equality`); every intentional hash change bumps the behaviour revision with a reason [ARCH.06 §2, §4, §12]. Every profile field below lives in the instance, the state and the movie header; BIOS bytes are an input, never a default [ARCH.01 §8]; output options (rate, synth mode, `cart_gain`) change only the PCM hash.

| Profile field | Values (default first) | Notes |
|---|---|---|
| `region` | AUTO (header, then hash, then NTSC), NTSC, PAL | [ARCH.04 §1.4] |
| `ram_fill` | ZERO, SEEDED(u64 seed) | 4 KiB RAM + 128 B RIOT RAM, fixed integer generator [ARCH.01 §8] |
| `cpu_phase` | 0–3: CPU-grid offset against t at MARIA enable | derived from C10 (hand-off at t 24, so 0); RTL check in v4.0 (DEC.41), 2026-10-20 [ARCH.01 §8] |
| `tia_phase` | 0–455: TIA line counter at tick 0 | default 0; shared by TIA WSYNC and TIA audio [ARCH.03 TIA-1] |
| `ane_lxa_constant` | $00, $EE | $EE only in the SingleStepTests lane [ARCH.07 §1] |
| `pokey_power_on` | RUNNING (as if SKCTL = $03 at tick 0), INIT | §13 |
| `riot_orb_power_on` | $FF, $00 | §13; the rest of the RIOT powers up DDRA = DDRB = ORA = $00, timer $FF at 1024T, flags clear [ARCH.01 C9] |

| Hostile input | Bounds (full list [ARCH.04 §7]) |
|---|---|
| A78 or headerless image | file ≥ 128 B (header) or ≥ 8 B (footer test); image ≤ 1 MiB + 128 else `UNSUPPORTED_SIZE`; 9-byte magic; size = min(field, file − 128), 0 rejected; out-of-range codes → defaults + lint |
| Board geometry | bank = value mod banks, never a raw index; RAM index & (size − 1); image size checked against the mapper at load |
| BIOS | exactly 4,096 or 16,384 B; its reset vector must point inside the BIOS |
| HSC ROM | exactly 4,096 B; canonical SHA-1 identifies, unknown supplied images warn and are accepted; missing C6/FE signature bytes warn independently; no reset-vector validation; see docs/embedding.md |
| Battery bytes | HSC 2,048 B, SaveKey 32,768 B; shorter → pad $FF, longer → reject; contents never parsed |
| State | exact size; magic, CRC-32, versions and every field bound checked before any byte is applied; ticks ≤ 2^62 |
| Movie, IPS/BPS | runner only (`patch.c`), every offset bounded by the image cap |
| Fuzz targets | each with a `fuzz.regress.<target>` replay on six legs, plus nightly libFuzzer [ARCH.06 §3]: `fuzz.a78` (probe + load: A78 v3/v4, headerless, 2600), `fuzz.run` (random bytes as a 48 KiB linear cart, fast boot, 2 frames: no crash, UB or timeout); with their features `fuzz.board` (random bus traffic), `fuzz.i2c` (SaveKey pins, v2), `fuzz.state` and `fuzz.movie` (v3), `fuzz.patch` |

## 7. State and saves
- **Visitor.** One function per struct, run as SIZE, WRITE (save) and VALIDATE then APPLY (load) — the sibling's four-pass visitor — plus RECORD, which tests use to emit every field's bounds for an automatic out-of-range rejection sweep; byte-wise LE; `_Static_assert(sizeof …)` tripwires; VALIDATE walks everything, so APPLY cannot fail [ARCH.10].
- **Header**, 64 B: PNG-style 8-byte magic, format version, header size, total size, behaviour revision, board, region, ROM CRC-32, state CRC-32, frame number, ticks, reserved zeros [ARCH.10]. The size is constant from load to unload (libretro serialize) and XM's 128 KB RAM dominates it from v2 [ARCH.05 §1]. The body holds every chip, the profile, the counters, lazy-device catch-up ticks, both line RAMs, the open-bus byte, the HALT-gate count. Not saved: the host's pending input (set before every frame), hooks, buffers.
- **Battery (implemented API 5).** `muhtari_get_battery` returns one raw span, layout 1, attachment/firmware metadata and a uint64 equality token. Pointer/length/placement stay fixed until successful load or destroy; power cycle retains bytes/token. Host restore after load and before the first frame is clean; later direct host mutation is outside contract. Only committed emulated changed bytes increment generation, with unsigned wrap; queries do not scan or advance time. Installed HSC ROM/stub is a separate const view, never save bytes. See [embedding](../../docs/embedding.md#battery-storage-and-hsc-firmware) and [HSC hardware](../../docs/hardware/cartridge.md#hsc). The core does no file I/O [ARCH.05 §1].
- **`.srm` (v2):** raw HSC SRAM 2,048 B, SaveKey 32,768 B (erased $FF), or HSC ‖ SaveKey = 34,816 B; battery layout 1; the runner's `.sav` holds the same bytes [ARCH.04 §4.3].

## 8. Observability
- **Per-line DMA statistics, always kept:** ≈ 10 B × 263 lines per frame — T0, HALTRST, CPU halt cycles, 4-/5-byte header counts, graphics bytes, character maps, holes, flags {overrun with abort tick, DLI, last line, VBE, DMA off, test mode} [ARCH.02 §10]. **Flip** to traced builds only if Cachegrind shows > 1 %.
- **Inspection never changes state:** `peek` reads the bus view without RIOT flag clears, hotspot bank switches, open-bus updates or lag credit. Property test: peeking all 64 KiB leaves the state hash unchanged [ARCH.05 §5].
- **Counters** (saved, reset by power cycle): lag frames, `io_hazard`, DMA overrun lines, DMA test-mode (DM 00/01) lines, POKEY writes while SKCTL holds init (feeds the lint).
- **Hooks (v5):** instruction, bus-access, line and frame observers. `run_loop.inc` is compiled twice (`MUHTARI_TRACE` 0 and 1); the traced copy is chosen once per run call, only when hooks are installed. Proof: with no hooks, Ir/frame stays within 32 of a `MUHTARI_TRACE_LOOP=OFF` build; the size delta goes to `quality.json` [ARCH.06 §8, §10]. Runner outputs on top (v5): `--trace` (instruction, tick, beam row/column), `--dl-dump`, `--dma-csv` and overlay PNG from the line statistics, `--symbols` (dasm, ld65), `--lua` [ARCH.05 §4–5].

## 9. Boot
| State | Fast boot (default; HLE from facts, no Atari bytes) [ARCH.01 C10] | `--boot bios` |
|---|---|---|
| Starts at | the hand-off below, at the first cart opcode fetch | power-on: INPTCTRL $00 (BIOS mapped, MARIA off, unlocked), CTRL $00, CPU after RES (A = X = Y = 0, S = $FD, I = 1), PC = BIOS reset vector |
| CPU | PC = ($FFFC); S = X = $16; A = Y = $00 (PAL: A = $60); P = I + D | set by the BIOS |
| INPTCTRL, HALT gate | $16 (acts as $06, unlocked); two writes counted | BIOS writes |
| MARIA | CTRL $60; DPP $1F84 (PAL $2707); palettes, BACKGRND, CHARBASE 0; NTSC DMA internals DL $2200, DL pointer $2207, zone pointer $1FF3, OFFSET 2, pixel pointer $8400 | set by the BIOS |
| Beam | NTSC row 239, column 436 (8,190 ticks before VBLANK); PAL ≈ row 308, column 125–160 (derived) | where the BIOS hands off (varies with the signature length) |
| TIA, RIOT, RAM | TIA registers 0; RIOT power-on profile, timer running; `ram_fill` everywhere, PAL also $FF at $2000–$203F/$2100–$213F; **no loader in RIOT RAM**, which keeps `ram_fill` [ARCH.08 §6] | whatever the BIOS writes |
| Cart | board power-on state, bank 0 | same |

- **BIOS bytes** are user-supplied only: recognised by digest (NTSC 4 KiB md5 0763f1ff…, PAL 16 KiB md5 397bb566… [ARCH.05 §1]), any other 4/16 KiB image accepted with a warning; mapped over $8000–$FFFF while INPTCTRL EXT = 0. CI covers this path with the CC0 OpenBIOS built from `b8bc745a8744` with KiloParsec replaced by $FF fill (SHA-256 `46b5bc2b…`), never committed or packaged [ARCH.07 §2].
- **Refusals.** An image the probe classifies as 2600 gets `MUHTARI_STATUS_UNSUPPORTED_2600`, "unsupported: 2600 (planned)" [ARCH.04 §1.5]. Under `--boot bios`, when the BIOS takes its 2600 path (it locks INPTCTRL with MEN = 0 [ARCH.01 §5]), the core stops the CPU and `run_frame` returns `MUHTARI_STATUS_BIOS_REJECTED_CART`, "unsigned or modified image: use fast boot or 7800sign", from then on [ARCH.04 §6]. The runner exits 77 for both.

## 10. Seams kept for later
| Later feature | Seam kept now | Lands |
|---|---|---|
| 2600 mode | the probe already classifies 2600 images; INPTCTRL is a full latch (LOCK, MEN, EXT, TIA-EN) and `bus.c`'s table has the MEN = 0 decode (2600 map, every cycle slow, no NMI); `tia.c` owns its 456-tick counter, so video can join it; fast boot writes no loader | after the 7800 library (owner decision) |
| XM, YM2151 | A78 byte 63 parsed; XM is one more board beside the POKEY windows; synth sources are numbered, not fixed; YM clocked from φ2 × 2 until the XM clock is known [ARCH.03 §6] | XM v2; YM v4 (ymfm OPM → C) |
| Light gun | `ports_pins(port, tick)` can ask MARIA for the beam row/column and the luma of the pixel output at that tick (aim window D = 64, W = 30, L = 9 + threshold T) [ARCH.04 §3.4] | v2 |
| Sub-frame input | `muhtari_input` is size-tagged, so a tick-stamped queue appends without an ABI break [ARCH.05 §5] | when a TAS or a game needs it |
| Big-endian, 32-bit, consoles, WASM | byte-wise LE serialisation; `be32` per PR from P1; `size_t` arithmetic checked on ILP32; RGB565 path in the adapter; no threads, files or globals, so an Emscripten core and a WASI runner (a seventh hash lane) need no core change [ARCH.06 §9][ARCH.05 §1, §6] | v6 |

## 11. Build order for v1.0
| Phase | Tracer first (one path through every layer) | Then widen to | Done when |
|---|---|---|---|
| P1 A test frame in RetroArch | test card from `instance.c` → runner `--hash-frame` → libretro → RetroArch screenshot | six platforms + `be32`, asan/nofp/hygiene, presets, attested release | runner and libretro hashes equal on every leg; the screenshot equals `--dump-frame` |
| P2 The CPU matches the vectors | SALLY against the committed SingleStepTests `6502/v1` sample and Bruce Clark's decimal test | bus: 4/6-tick cycles by address, open bus, INPTCTRL and the two-write HALT gate, HALT for reads and writes after the fitted latency, RDY reads only (`INC $24` completes both writes), NMI penultimate-cycle rule; RIOT | full vectors nightly; `ctest -L sally` green |
| P3 Games play in RetroArch | one display list from a unit test → one 7800basic sample via fast boot → DMA costs (own diagnostics vs App. 3/4) → SuperGame → wiki predicates | ports and switches, OpenBIOS path, rcheevos maps, library preset → `docs/compatibility.md` [ARCH.07 §4–5] | sample hashes pinned on every leg; boot-to-play in RetroArch |
| P4 Sound | TIA (lattice unit tests) → POKEY (Altirra-derived vectors) → mixer → one integrated RetroArch session | transition and PCM hashes; 7800basic POKEY and RMT samples; Ballblazer and Commando locally | hashes on every leg; zero clipped samples, peak ≤ −1 dBFS [ARCH.03 §8] |

## 12. Changes from the design spine [ARCH.09]
- A DLI costs 0 ticks: no +17 overhead; NMI timing comes from MARIA's pulse and SALLY's sampling [ARCH.02 §4.5]. No 426/430 cap: DMA aborts at LRC with tails of +3 / +9 [ARCH.02 §4.6].
- A zone's last line costs +6 raw ticks, then CPU-cycle rounding; the 16/24 already contain the 8-tick terminator; a7800's +8 over-charges [ARCH.02 §4.3].
- A hole costs one slot of its object's kind (3/6/9) and ends the object: neither free (MAME) nor "+3 once" [ARCH.02 §4.4].
- NMI: MiSTer's DLI pulse (second φ1 after /HALT rises, two cycles), not "DMA end + 1 tick" [ARCH.01 C6].
- VBE is its own 7-tick fetch at line 16 t 43; content starts on raster 18; rasters 258–259 are fetched and blanked [ARCH.02 §6.1].
- Slow cycle = a 6-tick call with data at tick 5 (MARIA's divider), not aligned to the TIA clock; RDY holds the first read after the write [ARCH.01 C4, C5].
- TIA audio steps on its own 456-tick line (B half-steps 224/232 ticks apart), neither every 228 ticks nor twice per MARIA line; the two-channel mix follows n/(n+30); TIA writes are never gated by INPTCTRL [ARCH.03 §2].
- POKEY is own code from the Altirra manual (spine: an a7800 port); MAME is a read-only cross-check; a7800 `pokey.cpp` is only the BSD-3 fallback [ARCH.08 §4]. It runs on the stretched φ2 and powers up out of init [ARCH.03 §4].
- The mix is an integer node-current sum (cartridge ≈ 2.65× per volt), not fixed per-source scaling [ARCH.03 §3].
- Hand-off beam: row 239, column 436 at the first cart fetch (MiSTer seeds 239/386 50 ticks earlier); S = X = $16, D = 1 [ARCH.01 C10].
- Fast boot writes no RIOT-RAM loader; OpenBIOS is built in CI and never committed or packaged, because it draws the Fuji logo [ARCH.08 §6][ARCH.07 §2].
- MiSTer `TIA.sv`, `M6532.sv`, `cart.sv`, `top.sv` and `Atari7800.sv` are facts only (third-party edits from the CC BY-NC period): nothing is lifted from them [ARCH.08 §2].
- RIOT ORB powers up as $FF by default, not "DDR/output = 0" (§13); $2800–$2FFF is undecoded on the bus [ARCH.01 C8–C9]; DMA reads of TIA, RIOT and MARIA space return the floating bus with no device side effects [ARCH.02 §4.7].
- Palettes come from a7800's Trebor V7 tables (BSD-3), default WARM, not MAME's [ARCH.02 §7]; boards dispatch by `switch`, not a const ops table [ARCH.06 §2]; the light gun adds a luma threshold to the beam window [ARCH.04 §3.4].

## 13. Open conflicts and how each is settled
| Conflict | Positions | Settlement | Local predicate that settles it |
|---|---|---|---|
| RIOT ORB at power-on | [ARCH.01 C9]: ORB acts as $FF — two T4 console reports (BupSystem; RevEng quoted on MAME tester 06060): CTLSWB = $14 without a SWCHB write leaves PB2/PB4 high, which One on One and Worm! need. [ARCH.04 §3.3]: those titles set the DDR without writing SWCHB, "so they rely on ORB = 0" (the datasheet and MAME zero ORB). Both cite the same wiki item, read two ways | **Rec** profile field `riot_orb_power_on`, default $FF: console-specific measurements outweigh a generic datasheet and one reading of a shared T2 line — **Alt** $00 — **Flip** either predicate shows $00 | (a) library rows for One on One and Worm! with a scripted fire press (`movie:…/ram@F`) under each value; (b) owner console: a diagnostic sets CTLSWB = $FF without writing SWCHB and stores SWCHB in the mailbox |
| $2800–$2FFF | [ARCH.01 C8]: undecoded (cart or open bus); A7800Hawk reads 0; the Guide says mirror. [ARCH.05 §2]: rcheevos console 51 calls it mirrored RAM, descriptor VIRTUAL_RAM → RAM + $0800 | **Rec** the CPU decode follows C8; the descriptor follows rcheevos' table, because it maps rcheevos' address space, not the bus; one test pins each — **Alt** decode the mirror on the bus too — **Flip** Summer Games deadlocks on open bus, or the owner's console mirrors page $27 reliably | Summer Games ledger row (`boots@F`, then a RAM predicate); the RA set scan for reads of $2800–$2FFF [ARCH.05 Q2] |
| 242 vs 243 rows | MiSTer's VBLANK decode, MAME `lines − 5`, the Guide's raster prose → 242/292; the Guide's diagram, a7800, A7800Hawk, Trebor → 243/293 [ARCH.02 §6.1] | **Rec** 242/292, height from `get_info`; libretro max 320 × 293, so a flip changes no ABI, only the behaviour revision — **Alt** 243/293 — **Flip** a capture shows a 243rd unblanked raster or content on raster 258 | owner capture of a raster-numbered diagnostic; the a7800 differential crops its 320 × 243 PNG to 242 rows [ARCH.07 §3] |
| First-pixel tick | MiSTer 135; Guide App. 4 and a7800 134; A7800Hawk 133 [ARCH.01 §2][ARCH.02 §3.3] | **Rec** `MARIA_FIRST_PIXEL_T 135` (schematic-derived); it moves only where a mid-line register write lands (1 tick = one 320-px pixel) and, from v2, the light-gun beam; DMA and CPU timing are unaffected — **Alt** 134 — **Flip** the photo below puts the split one pixel off | mid-line BACKGRND-write diagnostic photographed on the owner's console; 7800test tests 6 and 13 locally [ARCH.07 §1, §4] |
| HALT latency and handshake | MiSTer: 2 cycles in, 2 out, fitted to "general_tests" test 15 = 556 (not public); scope: ≥ 1 cycle runs, bus free by the 2nd φ2 fall; MAME and A7800Hawk: 0 [ARCH.01 §3]; handshake ≈ 6 ticks fitted to RevEng's measured 16/24 [ARCH.02 §4.3] | **Rec** MiSTer's rule inside `halt_window()`, the only model meeting both 16/24 and 556 — **Alt** halt at the next cycle boundary (A7800Hawk) — **Flip** a hardware loop count differs | diagnostics #1 (DMA cost by loop count and INTIM) and #4 (DLI vs WSYNC) on the owner's console; test 15 as a unit test once its ROM is obtained [ARCH.07 §4, §8] |
| MiSTer holey DMA at the pin | `0dc8ad2e3ff7` contains #62 (2026-09-11), which removed the hole write gate, so MiSTer now draws a holey object's first byte from the bus; MAME, a7800, A7800Hawk and bsteux's hardware comparison draw nothing [ARCH.02 §4.4, §11] | **Rec** draw nothing and charge one slot of the object's kind; the v4 Verilator oracle builds `DMA.sv` at `8c96e1f7cc24` (pre-#62, already MIT) or lists holes as a documented deviation; report upstream — **Alt** follow the RTL at the pin — **Flip** a hardware photo shows hole bytes drawn | diagnostic #6 (cost by INTIM + picture); Scrapyard Dog sewer-water frame predicate (local) |
| DLI → NMI timing | MiSTer: NMI from the 2nd φ1 after /HALT rises, two cycles wide; Guide and A7800Hawk: 1 tick after DMA shutdown [ARCH.01 C6] | **Rec** MiSTer — **Alt** one tick after shutdown — **Flip** the diagnostic fits the one-tick model | diagnostic #4; Xenophobe zone-0 DLI predicate (local) |
| RDY after a write | NMOS rule: the first read after `STA WSYNC` waits; MiSTer's netlist note: the cycle after a write cannot be held, so the stall lands one cycle later [ARCH.01 C5] | **Rec** NMOS rule — **Alt** one cycle later — **Flip** a WSYNC-then-store diagnostic shows the store 4 ticks earlier | that diagnostic, photographed on the owner's console |
| POKEY power-on | [ARCH.03 POKEY-PWR]: out of init (MiSTer; Beef Drop, Cybernoid II and demos play without writing SKCTL). [ARCH.04 §2.1, §2.4]: stock POKEY silent until SKCTL, flash carts release it at boot. Altirra: indeterminate | **Rec** profile `pokey_power_on` = RUNNING, INIT selectable; probe and runner lint "POKEY written but SKCTL never left init" — **Alt** INIT (MAME, a7800) — **Flip** a real POKEY board (not a flash cart's HOKEY) that never writes SKCTL is silent from cold boot | cold-boot test on the owner's console [ARCH.03 §9] |
| POKEY fallback file | [ARCH.08 §4]: a7800 `pokey.cpp` @`7a2afdc1ea08`; [ARCH.03 §5]: MAME v4.9 upstream is the superset | **Rec** PROVENANCE governs; if the fallback ever triggers, the same PR may move its row to MAME `sound/pokey.cpp` @`dddf4cd1faee` (BSD-3 there [ARCH.08 §2]) — **Alt** keep a7800's — **Flip** n/a | POKEY unit vectors [ARCH.03 §9] |

## Sources
| ID | Tier | Source (URL) | Pin or access date | Supports |
|---|---|---|---|---|
| ARCH.01 | T1 | `.planning/preparation/A7800-SYSTEM.md` (this project's research) | 2026-10-15 | clocks, line timeline, SALLY HALT/RDY/NMI, map, INPTCTRL, RIOT, boot, profile |
| ARCH.02 | T1 | `.planning/preparation/A7800-MARIA.md` | 2026-10-15 | DMA schedule and burst, spans, rows, palettes, holes |
| ARCH.03 | T1 | `.planning/preparation/A7800-AUDIO.md` | 2026-10-15 | TIA lattice, mix, POKEY, synthesis, hashes |
| ARCH.04 | T1 | `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` | 2026-10-15 | header, probe, boards, ports, saves, hostile inputs |
| ARCH.05 | T1 | `.planning/preparation/HOSTS-AND-INTEGRATION.md` | 2026-10-15 | libretro, rcheevos, runner, Playstead, scripting |
| ARCH.06 | T1 | `.planning/preparation/ENGINEERING.md` | 2026-10-15 | C rules, tests, fuzzing, performance, style |
| ARCH.07 | T1 | `.planning/preparation/CONFORMANCE.md` | 2026-10-15 | vectors, diagnostics, oracles, OpenBIOS build |
| ARCH.08 | T1 | `PROVENANCE.md` | 2026-10-15 | seeds, notices, BIOS and loader rules |
| ARCH.09 | T1 | design spine: the hardware-design agent's hypotheses handed to research (not kept in the repository) | 2026-10-15 | the superseded items in §12 |
| ARCH.10 | T1 | nesturbator `src/state.c` L4–6, L29, L46–48 and `include/nesturbator.h` L46–131, github.com/szTheory/nesturbator (MIT, owner's sibling) | `34d68516a15e` | state header, visitor, tick cap, version macros, size tags, allocator |
