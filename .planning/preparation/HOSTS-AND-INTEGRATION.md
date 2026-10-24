---
title: "Hosts and integration: libretro, RetroAchievements, RetroArch on Mac, runner + Playstead, automation, WASM"
summary: "What every host needs from muhtari (libretro adapter, rcheevos memory map, RetroArch on Apple Silicon, muhtari-run and Playstead's protocol, scripting, WASM) and which milestone each piece lands in."
read_when: "Building the libretro adapter or .info, the runner CLI, saves, the RetroArch e2e job, a Playstead pin, scripting/Gym/TAS surfaces, or a web build."
updated: 2026-10-24
---

# Hosts and integration

Markers: (tested) run here on 2026-10-15, (derived), (untested), (unverified). "Lnnn" = line of libretro.h at RetroArch v1.22.2 [HI.01]. Milestones: v1 P1–P4, v2 library breadth, v3 states/movies, v4 accuracy + XM, v5 tools/AI, v6 ports.

## Where to look
| Question | Section |
|---|---|
| Exports, geometry, fps, aspect, options, firmware, input, saves, serialize, `.info`, buildbot | 1 |
| rcheevos 7800 map, descriptors to expose, RA hash, hardcore, cheat offsets | 2 |
| RetroArch install, low-latency settings, bounded hosted e2e | 3 |
| `muhtari-run` exit table, options, hashes, Playstead pin + continuation mapping | 4 |
| Inspection/hooks, Python vs Lua, Gym env, movies, lag, debugger | 5 |
| Web build | 6 |
| Feature → host or core | 7 |

## 1. libretro contract
**Exports.** All 25 entry points (L7511–7840) in nesturbator's 352-line adapter shape [HI.22]: one instance `retro_init`→`retro_deinit`, statics reset in deinit, no game → test card (`SET_SUPPORT_NO_GAME` 18), XRGB8888 (`SET_PIXEL_FORMAT` 10) plus RGB565 when `USE_RGB565` is defined (the PSP template passes it [HI.09]), mono → stereo batch audio, ROM copied at load. 7800 changes: `retro_get_region` 0/1 from the loaded region; `retro_reset` = power cycle (battery media kept), because the console Reset is a game-read switch (RetroPad X). **Rec:** power cycle. **Alt:** hold the Reset switch for 2 frames. **Would change if:** users expect RetroArch's Restart to keep RAM.

| AV field | NTSC | PAL | Derivation |
|---|---|---|---|
| base w × h | 320 × 242 | 320 × 292 | rows 16–257 / 16–307 (design spine); BizHawk's gun Y axis is 1–242 (T3) [HI.12] |
| max w × h | 320 × 293 | 320 × 293 | covers 242/243 and 292/293 (A7800-SYSTEM settles) (derived) |
| fps | 39375000/656711 = 59.95788 | 3546894/71051 = 49.92040 | (315/44 MHz)/(454·263); 7,093,788 Hz/(454·313) (derived, arithmetic tested) |
| sample_rate | 48000 | 48000 | 800.56 / 961.53 samples per frame → 1,024-sample buffer (derived) |
| aspect_ratio | 960/847 ≈ 1.1334 | ≈ 1.1393 | pixel 6:7 = 12.2727 MHz square rate ÷ 7.1591 MHz ÷ 2 lines; PAL 14.75 ÷ 7.0938 ÷ 2 = 1.0396 (derived) |

**Rec:** pixel-correct aspect, computed in the adapter (its only floating point). **Alt:** 4:3 fill (a 320-px line spans 44.7 µs of the ~52.7 µs active line, so 4:3 stretches it 17.6 %) (derived). **Would change if:** the owner prefers fill; RetroArch can override per user anyway.

**Core options** (`SET_CORE_OPTIONS_V2` 67, v1 fallback via `GET_CORE_OPTIONS_VERSION` 52; L2319 asks for few):

| Key | Values (default first) | In state | Lands |
|---|---|---|---|
| `muhtari_region` | auto (header/hash list), ntsc, pal | yes | v1 P3 |
| `muhtari_boot` | fast (HLE hand-off), bios (firmware missing → fast + OSD) | yes | v1 P3 |
| `muhtari_palette` | region default; more once licences are clear | no (adapter) | v1 P3 |
| `muhtari_left_difficulty`, `muhtari_right_difficulty` | B, A (power-on position) | yes | v1 P3 |
| `muhtari_artifacts` | off, on (host composite filter, 320 modes) | no | v4 |

Port devices are `SET_CONTROLLER_INFO` (35) subclasses, not options: ProLine (default), 1-button joystick (= "two-button mode" off), light gun, paddles, none. **Rec:** these five. **Alt:** ProSystem's set (colour depth, audio filter, dual stick; no emulation options) [HI.06]. **Would change if:** rcheevos reviewers reject `region`; else add `{"muhtari_region","pal"}` to rcheevos' disallowed list as for bsnes/fceumm/mesen/snes9x [HI.04].

**Firmware** (from `GET_SYSTEM_DIRECTORY` 9): `firmware0 = "7800 BIOS (U).rom"` (4,096 B, md5 0763f1ffb006ddbe32e52d497ee848ae), `firmware1 = "7800 BIOS (E).rom"` (16,384 B, md5 397bb566584be7b9764e7a68974c4263), both `opt = "true"`; names/md5 from libretro `System.dat`; ProSystem's `.info` lists only (U) [HI.02][HI.03]. **Rec:** accept any 4 KB / 16 KB image (OpenBIOS CC0 exercises the same path in CI), log its SHA-256 and whether it matches. **Alt:** refuse other md5s. **Would change if:** a game depends on a BIOS revision. An HSC ROM entry appears only if cartridge research picks firmware over HLE.

**Input** (`SET_INPUT_DESCRIPTORS` 11) — RetroPad layout = ProSystem's [HI.06]:

| RetroPad | 7800 | Notes |
|---|---|---|
| D-pad, B, A | stick, button 1 (left), button 2 (right) | one-button mode merges both on INPT4 (core pin model) |
| Select, Start, X (port 1) | Select, Pause, Reset | momentary SWCHB bits |
| L, R (port 1) | left / right difficulty toggle | adapter turns press edges into absolute positions in `muhtari_input`; OSD message; BizHawk also toggles [HI.12] |
| Light gun (v2) | `RETRO_DEVICE_LIGHTGUN` 4: SCREEN_X/Y 13/14 (±0x7FFF, 0 = centre) → column 0–319, row 0–241; TRIGGER 2; IS_OFFSCREEN 15 / RELOAD 16 → no light | L238–420 |
| Paddles (v2) | `RETRO_DEVICE_ANALOG` 5: left X → paddle A, right X → paddle B, B/A fire | L260, L389–393 |

**Rec:** ProSystem layout (remaps carry over). **Alt:** Reset on L3. **Would change if:** players report accidental resets.

**Saves.** `RETRO_MEMORY_SAVE_RAM` (0) = battery span, fixed from load to unload; RetroArch writes `.srm` on unload and at `autosave_interval` [HI.05]; the adapter does no file I/O. Layout, raw: HSC SRAM 2,048 B (BizHawk's SaveRam is the HSC SRAM [HI.12]); SaveKey 24LC256 32,768 B, blank $FF (Stella's device-file format [HI.23]); both present → HSC then SaveKey. **Rec:** one per-game raw `.srm` (v2). **Alt:** shared per-device files via `GET_SAVE_DIRECTORY` 31, as Stella's `nvram/savekey_eeprom.dat` (real hardware: one HSC/SaveKey across games). **Would change if:** users want cross-game SaveKey sharing or Playstead adds device-scoped saves.

**Serialize.** Size constant between load and unload (L7699), 0 without content; no `SET_SERIALIZATION_QUIRKS` (44); byte-wise little-endian; savestate context ignored although `ROLLBACK_NETPLAY` (L5707) asks for big-endian. Run-ahead (same or second instance) and rewind need only speed + determinism; netplay compares content, core version and states [HI.22] → byte-identical states on six platforms + BE qemu. XM's 128 KB RAM dominates size (derived). **Rec:** LE always. **Alt:** BE in context 3. **Would change if:** a frontend really exchanges context-3 states across machines.

**`.info`.** `display_name "Atari - 7800 (muhtari)"`, `corename "muhtari"`, `systemname "Atari 7800"`, `systemid "atari_7800"`, `database "Atari - 7800"`, `manufacturer "Atari"`, `supported_extensions "a78|bin"`, `license "MIT"`, `needs_fullpath`/`hw_render`/`cheats`/`disk_control`/`load_subsystem` false, `display_version` generated from `version.txt` (ProSystem: `serialized`, `memory_descriptors true`, `core_options false` [HI.02]). Each claim maps to its proving tests at configure time (nesturbator pattern [HI.22]):

| Claim → true | Lands | Guard test |
|---|---|---|
| `supports_no_game` | v1 P1 | in-process `libretro.host`, NULL game → test-card hash |
| `input_descriptors`, `memory_descriptors` | v1 P3 | descriptor ids valid; vendored rcheevos resolver over our map (§2) |
| `core_options` + `core_options_version "2.0"` | v1 P3 | key list equals the one sent to rcheevos review |
| `libretro_saves` | v2 | in-process `.srm` round trip + hosted case 2 (§3) |
| `savestate`; `savestate_features` basic → serialized → deterministic | v3 | size constant; save+load every frame for 600 frames = straight run; six-platform state hashes |
| `is_experimental "false"` | v2 | compatibility-ledger pass count ≥ threshold |

RetroArch disables features above the declared level (since 1.10.1), and only reads the level when the `.info` sits beside the core [HI.22].

**Buildbot (v6).** ci-templates `96f603e` carry CMake variants for every modern target incl. console statics (libnx, vita, psp, ctr, wii, wiiu, ngc, ps2, orbis, djgpp, dingux, emscripten): `-DLIBRETRO_STATIC=ON -DLIBRETRO_SUFFIX=_<platform>`, target `${CORENAME}_libretro`, artefact `${CORENAME}_libretro_<platform>.a` (`.bc` for emscripten) linked into RetroArch, so every global symbol needs the `muhtari_`/`retro_` prefix [HI.09]. The Linux image is Ubuntu 16.04 + GCC 9 + CMake 3.31.7 [HI.10] → C17 and a CMake 3.25 floor fit. Templates with no CMake twin: psl1ght (PS3), ctr-legacy, ios9, MSVC 2005/2010 (msys2). The `.info` PR goes to libretro-core-info (same blob as libretro-super `dist/info`) [HI.02]. **Rec:** CMake-only. **Alt:** add `Makefile.libretro`. **Would change if:** PS3 or old MSVC builds are wanted, or buildbot maintainers ask.

## 2. RetroAchievements
rcheevos console 51 models the 7800 as the 64 KB CPU bus; the table is byte-identical in v10.0.0, v11.0.0, v12.0.0 and `116d018` (tested); RetroArch 1.22.2 bundles v12.1.0 [HI.04][HI.05]. With descriptors, rcheevos uses only them; without, it reads `SYSTEM_RAM` as a flat buffer from $0000 and `SAVE_RAM` at $4000 (`rc_libretro.c` L679–770) [HI.04].

| rcheevos region | Type | muhtari descriptor (`SET_MEMORY_MAPS` 36) |
|---|---|---|
| $0000–$17FF Hardware Interface | HARDWARE_CONTROLLER | $0040–$00FF → RAM+$0840, $0140–$01FF → RAM+$0940 (shadows [HI.20]); $0480–$04FF → RIOT RAM; $1000–$17FF → HSC SRAM when attached; TIA/MARIA/RIOT registers unmapped (read as 0) |
| $1800–$27FF System RAM | SYSTEM_RAM | RAM+0, 4,096 B, flag SYSTEM_RAM |
| $2800–$2FFF Mirrored RAM | VIRTUAL_RAM | RAM+$0800 ($2000–$27FF repeats at $2800 [HI.20]) |
| $3000–$37FF, $3800–$3FFF "Mirrored RAM" | VIRTUAL_RAM | unmapped: the Software Guide lists $3000+ as external-device space, not RAM (conflict) |
| $4000–$7FFF Cartridge RAM | SAVE_RAM | unbanked cart RAM only (SuperGame 16 KB); banked XM/bankset RAM unmapped — descriptors cannot express banks (L3700s) |
| $8000–$FFFF Cartridge ROM | READONLY | linear ROM and SuperGame's fixed bank, `RETRO_MEMDESC_CONST` |

Mirrors share one `ptr` with different `offset` ("overlapping descriptors … same ptr") [HI.01]; call `SET_SUPPORT_ACHIEVEMENTS` (42) true in `retro_load_game` before the first `retro_run`; also expose `SYSTEM_RAM` (2) and `SAVE_RAM` (0) as L1508–1530 asks.
- **Do sets assume ProSystem's map?** ProSystem exposes descriptors (`memory_descriptors = "true"`) and BizHawk feeds RA its whole "System Bus" peek for the 7800 [HI.02][HI.12] → sets address the CPU bus, not ProSystem internals (derived). Residual risk: register addresses, $3000–$3FFF, banked RAM (Q2).
- **Cheat offsets.** All 94 addresses in the 41 libretro-database 7800 `.cht` files (RetroArch handler) lie in $18BF–$27B5 (tested) → under ProSystem a cheat offset equals the CPU address (derived). If RetroArch's cheat engine reads `SYSTEM_RAM` (Q1), expose `SYSTEM_RAM` as a $2800-byte block whose top 4 KB is the RAM (6 KB dead, zero run cost) (derived).
- **Game hash** = MD5 of the file minus 128 bytes when bytes 1–9 are "ATARI7800" (`rc_hash_rom.c` L23–28) [HI.04] → `--probe` prints it.
- **Hardcore** is enforced by RetroArch (no state loading, rewind, slow motion, frame advance; memory editors, scripting and input playback banned) [HI.11]. RA lists ProSystem and A7800Hawk for the 7800; unlisted cores "may be excluded" from hardcore and leaderboards [HI.06][HI.11] → ask RA in v6 once the buildbot ships muhtari.

**Rec:** descriptors above + rcheevos' own resolver as the CTest oracle (MIT, pinned). **Alt:** one flat 64 KB `SYSTEM_RAM` shadow as ProSystem appears to do. **Would change if:** Q1 shows RetroArch cheats need offset = address everywhere, or an RA set reads registers.

## 3. RetroArch on Apple Silicon
- **Install:** `brew install --cask retroarch-metal` (universal 1.22.2 DMG from buildbot stable; the cask's sha256 is `no_check`, so Homebrew verifies nothing) or the DMG; the `retroarch` cask is the x86_64 DMG; the App Store build is sandboxed [HI.07][HI.22]. 1.22.2 (2025-11-20) is still the newest release (tested). Pin: `RetroArch_Metal.dmg` 232,801,022 B, SHA-256 `81b79121ba26d539064ae13b4d0419a120c3d165afbe656cf5f5412b15fdb434`, equal to nesturbator's CI pin (tested) [HI.08][HI.22].
- **Core + `.info`:** `~/Library/Application Support/RetroArch/{cores,info}`; `curl -fsSL URL | tar -xf - -C ~/Library/Application\ Support/RetroArch cores info`; delete `info/core_info.cache` after an `.info` change; curl downloads carry no quarantine; the linker's ad-hoc signature suffices [HI.22].

| Low-latency setting (cfg key) | Value for 7800 | Why (all untested with muhtari) |
|---|---|---|
| video driver | Vulkan via MoltenVK ("default when available" in the Metal build since 1.15.0 [HI.05]) or Metal | max swapchain images is a Vulkan/D3D knob (Metal: unverified) |
| `video_max_swapchain_images` | 2 | default 3 → one frame less queued [HI.05] |
| `video_hard_sync` | n/a | OpenGL only |
| `video_frame_delay` / auto | auto (refactored in 1.20) or ~10 ms | a muhtari frame ≈ 1 ms if ~1,000 fps (derived); manual delay is blocked in RA hardcore since 1.10.3 |
| `video_swap_interval` | auto | ProMotion 120 Hz support since 1.20.0 [HI.05] → each 59.958 Hz frame shown twice |
| `vrr_runloop_enable` | on only with an Adaptive-Sync external display | paces exactly 59.958 Hz |
| `audio_latency`, `audio_rate_control_delta` | 32–48 ms (default 64); 0.005 | 59.958 vs 60 Hz = 0.070 %, inside DRC's ±0.5 % (derived) |
| `run_ahead_frames` / `preemptive_frames_enable` | 1, preemptive preferred (reruns only on input change, 1.15.0) | needs `savestate_features "deterministic"` (v3) |

**Rec:** preemptive frames 1 + auto frame delay + Vulkan swapchain 2. **Alt:** run-ahead 1 in second-instance mode (hides audio glitches after a state load) [HI.06]. **Would change if:** RetroArch's latency statistics show stalls at muhtari's frame cost, or the 7800 game in question has 0 internal lag frames (frame-advance count per [HI.06]).

**Bounded hosted e2e** (RetroArch aborts in the agent sandbox → hosted only [HI.22]). One macos-15 arm64 job: fetch the pinned DMG, check SHA-256, `hdiutil attach`, `ditto`; isolated config (`config_save_on_exit=false`, `audio_driver=null`, `video_gpu_screenshot=false`, `pause_nonactive=false`, all dirs inside the job); `RetroArch -L CORE --max-frames=N --max-frames-ss --max-frames-ss-path=shot.png -c test.cfg ROM` [HI.22]. Case 1 (v1 P1 test card; v1 P3 an own diagnostic ROM): screenshot = runner `--dump-frame N` after the same palette, byte-exact. Case 2 (v2): an own cc65 ROM increments a counter in HSC/SaveKey at boot and draws it; run 1 (no `.srm`) shows 1, run 2 shows 2; compare both shots and the `.srm` bytes with the runner. Nothing else: no UDP commands, no frame-exact progression, no RetroArch rewind/run-ahead/netplay (prove those in-process). Budget ≤ 300 test lines, timeout 2× measured; nesturbator's grew to 7,706 lines over 53 commits [HI.22]. **Rec:** two cases. **Alt:** in-process only. **Would change if:** a RetroArch update breaks loading unnoticed (add a version check, not a case).

## 4. Runner `muhtari-run` and Playstead

Implemented runner saves, HSC firmware selection, load-time save-device declarations, raw layout/import
rules and normal-exit replacement have graduated to `docs/runner.md`, Battery files. The runner owns all
file I/O; the core owns only the stable raw span and generation. Cadence/on-demand controls remain pending
the following implementation slice; the horizon tables below are recommendations, not live capability claims.

Exit statuses: one X-macro table, printed by `--help`, README equality test [HI.22]: **0** done (also SIGTERM/SIGINT: finish frame, flush, exit) · **1** failure (I/O, corrupt or hostile image, failure after fixture access) · **2** usage · **3** hash mismatch (`--expect`) · **4** `.sav` size mismatch · **5** state rejected · **77** capability refusal (2600 image, unsupported board/device, BIOS required but absent). 3 and 77 become live (reserved in nesturbator).

| Option | Meaning | Lands |
|---|---|---|
| `[--rom] FILE`, `--frames N`, `--hash-frame N`, `--dump-frame N:FILE`, `--bench N` | none → test card; SHA-256 of native pixels; PNG via host palette; fps + p50/p90/p99/p99.9/max µs | v1 P1 |
| `--probe` | A78 v3/v4 fields, board, region, devices, save media, hashes below; no emulation; exit 0/77/1 | v1 P2 |
| `--boot fast\|bios`, `--bios FILE`, `--region auto\|ntsc\|pal` | BIOS bytes are an input; SHA-256 printed | v1 P3 |
| `--port1/--port2 DEV` | auto (header) · proline · joystick · none (v1 P3); lightgun · paddles · keypad · savekey · atarivox (v2+) | v1 P3 / v2 |
| `--expect FILE`, `--capabilities` | compare printed hashes; JSON, writes nothing | v1 P3 |
| `--hash-audio` | transition-stream + PCM SHA-256 | v1 P4 |
| `--save-dir DIR`, `--save-interval F`, SIGUSR1 | `DIR/<rom basename>.sav`, temp → fsync → rename | v2 |
| `--movie FILE` | replays per-frame input; prints the lag count | v1 P3 |
| `--record FILE`, `--state-at-frame N:FILE`, `--load-state FILE`, `--hash-state N` | movie recording and states | v3 |
| `--trace`, `--symbols`, `--dl-dump`, `--dma-csv`, `--lua` | §5 | v5 |

No window, config file, environment, stdin or network; invalid combinations exit 2 [HI.22]. `--probe` prints `sha1`, `sha256` (file as given), `sha1_headerless`, `sha256_headerless` (header removed by the rcheevos rule), `md5_headerless` (RA hash), `crc32` (RetroArch scanner). The libretro 7800 DAT holds 227 headerless `.bin` (commercial) and 110 headered `.a78` (aftermarket) entries, so both forms occur (tested) [HI.03].

| Playstead pin field [HI.21] | `muhtari-run` value | Lands |
|---|---|---|
| system | `a7800` — a new frozen id in Playstead (`gba gb gbc nes snes md psx` today), one-way | v2 |
| emulator, version, download_url, sha256 | macos-arm64 runner zip from the attested release; digest from SHA256SUMS | v1 P1 |
| launch.executable_relative_path | `bin/muhtari-run`; Playstead's installer only mounts a DMG and copies one `.app` → Playstead needs a zip/CLI path | Playstead |
| argument_template | `["--save-dir","{saveDir}","--rom","{romPath}"]` (+ `--movie/--frames/--hash-frame` to verify) | v2 |
| config_injection | `cli_flags`: save_directory `--save-dir {path}`; bios_path `--boot bios --bios {path}`; controller_mapping n/a (headless) | v1 P3 / v2 |
| save_contract | glob `{saveDir}/{romBaseName}.sav`; media `hsc_2k` 2,048 B, `savekey_32k` 32,768 B; `size_is_medium_identity` true; binding `rom_sha256` = headerless SHA-256 | v2 |
| flush triggers; on_demand_flush_supported | within `--save-interval` frames of a change, SIGUSR1, SIGTERM/SIGINT, exit; true | v2 |
| worst_case_loss_seconds | interval ÷ 59.958 (60 frames ≈ 1.0 s) on SIGKILL/crash; 0 on SIGTERM (flush inside Playstead's 2 s grace before SIGKILL) | v2 |
| exit_detection | clean 0/exit (also after SIGTERM); crash 6 or 11 uncaughtSignal; killed 9 | v2 |

**Rec:** SIGTERM → flush → 0. **Alt:** a dedicated "stopped by signal" status. **Would change if:** Playstead must tell host-stop from completion. The runner is headless: play in Playstead goes through the pinned RetroArch DMG + core zip, which needs a two-artifact pin (Playstead-side).

Continuation protocol `local-continuation-adapter-v1` [HI.21]: an owner-local adapter script wraps the runner (muhtari ships only the runner); each action gets one token, a fresh private dir, `PATH=/usr/bin:/bin`, no network (the runner has no socket code — a symbol-allowlist test proves it, v1).

| Action | Runner use | Facts asserted | Lands |
|---|---|---|---|
| qualify | `--capabilities` (dir stays empty) | initial_run, safe_exit, fresh_process, continue, repeatable_visible_oracle, oracle_agreement, repeat_count 2–1000 | v3 |
| initial-run | `--rom F --movie A --save-dir persistent-save --frames N` | save_transition (empty → populated), safe_exit (0, no temp files) | v3 (saves v2) |
| continue | per oracle run: copy the save dir, fresh process, `--movie B --frames M --hash-frame M` | fresh_process, continue_complete, oracle_agreement (≥ 2 equal hashes) | v3 |

Exits 0 pass / 77 refusal before fixture access / 1 failure after access match the runner table. This closes mGBA's three gaps: no on-demand flush, SIGTERM ≈ crash, no deterministic replay [HI.21].

## 5. Scripting and automation (YAGNI order)
| # | Surface | Design | Lands |
|---|---|---|---|
| 1 | Inspection | `muhtari_peek` (bus view without RIOT flag clears, hotspot bank switches or lag credit); CPU/MARIA/line-stats getters; property test: peeking all 64 KB leaves the state hash unchanged | v1–v3 |
| 2 | Hooks | instruction, bus-access, line and frame observers; loop compiled twice, traced copy chosen at run entry; `muhtari_run(budget, &stop)` for breakpoints; Cachegrind proves hooks-off Ir unchanged | v5 |
| 3 | Python | ctypes over the C API (stdlib; releases the GIL per call → threaded envs); wheels carry the release library | v5 |
| 4 | Gymnasium env | below | v5 |
| 5 | Lua in the runner | `--lua FILE` on the hook API; Lua is MIT; 5.4.9 matches BizHawk (lua54) and MAME (5.4.8) though their APIs differ; upstream is at 5.5.1 [HI.16] | on demand |

**Rec:** Python first. **Alt:** Lua first (no packaging, runs on consoles). **Would change if:** homebrew devs or TASers ask for in-emulator scripts.

**Gym env** (ALE/Gymnasium conventions [HI.14]): `obs_type` native (242×320 uint8 MARIA colours; the luma nibble is a free grayscale) · rgb (242×320×3) · ram (4,096 B + cart RAM); actions: full = 9 directions × {none, b1, b2, both} = 36, minimal per game; console switches stay outside the action set (`reset()` presses them; `mode`/`difficulty` set Select presses and A/B switches); `frameskip` 4 and `repeat_action_probability` 0.25 (the v5 defaults) live in the wrapper, drawn from its seeded `np_random`, never in the core; `reset(seed)` seeds the wrapper RNG and the core's RAM-fill profile; `step` → (obs, reward, terminated, truncated, info) from a per-game RAM spec file; vector env = N instances on threads, next-step autoreset. Throughput target ≥ 2,000 frames/s/core with video conversion off — hypothesis: ~2× nesturbator's per-cycle NES at 855–1,100 frames/s/core [HI.22], ~half of ALE-in-EnvPool at ≈3,500–6,300 frames/s/core (derived from [HI.15]); measure before claiming. **Rec:** native obs by default. **Alt:** rgb (ALE parity). **Would change if:** agent pipelines need RGB parity.

**Movies.** **Rec:** own versioned text format (v3): header (headerless ROM SHA-256, region, boot + BIOS SHA-256, port devices, RAM fill + seed, behaviour revision, start = power-on | state hash | SRAM hash) and one line per frame with absolute switch positions; BK2 import in v5 — a zip of `Header.txt`, `Input Log.txt`, `SyncSettings.json`, `Core.bin`, `MovieSaveRam.bin`; A78 mnemonics U D L R 1 2 and P r s p l r b; difficulty as toggles; gun axes 1–320 × 1–242 [HI.12][HI.13]. **Alt:** BK2 as the native format. **Would change if:** TASVideos (7800 = BizHawk only today [HI.13]) accepts muhtari. Format interop ≠ sync: BizHawk's timing differs, so imported inputs re-time (derived).

**Lag frame:** a frame with no CPU read of SWCHA/SWCHB ($280/$282 + mirrors) or INPT0–5 (+ mirrors); peeks and MARIA DMA excluded. A7800Hawk clears its lag flag on RIOT port reads (its TIA file, under `TIA_Sound/`, was not opened) [HI.12]; INPT counts here because two-button, paddles and light gun read there (derived).

**Sub-frame input. Rec:** per-frame input applied at VBLANK start until a TAS or game is shown to need more; `muhtari_input` stays size-tagged so a tick-stamped queue can be appended. **Alt:** the queue from v3. **Would change if:** a TAS, a hardware-capture replay or a game shown to sample input mid-frame needs it.

**Debugger/observability** (runner-side, no GUI, v5): instruction trace with tick + beam row/column; DLL/DL dump at frame N; per-line DMA CSV/overlay PNG from the always-kept line stats (≈ a7800's `a7800dev` view [HI.18]); memory dump; symbols from dasm `-s` dump / `-l` listing (7800basic emits dasm output) and cc65 ld65 `-Ln` VICE labels or `--dbgfile` [HI.19]. Precedent: MAME has break/watch/register/exception points, `trace` with `noloop`, `gvblank` [HI.17]; A7800Hawk has memory domains, System Bus read/write callbacks, register get/set, CPU trace, no stepping [HI.12].

## 6. WASM (SEED-008; the hash lane may come in phase 1)
**Rec:** Emscripten build of the core + a ~150-line JS harness (canvas 320×242, AudioWorklet at 48 kHz, keyboard + Gamepad API); content = test card, own MIT diagnostic ROMs, OpenBIOS (CC0) for the BIOS path; plus a WASI build of `muhtari-run` under Node or wasmtime as a seventh hash-equality lane. The core already satisfies the needs: no threads (no SharedArrayBuffer → no COOP/COEP headers, any static host), no files (bytes in, caller buffers out), allocator hook, no mutable globals; check 32-bit `size_t` in state-size math. **Alt:** RetroArch's web player via `emscripten-static-cmake` (statically linked into GPL-3 RetroArch, output `${CORENAME}_libretro.{wasm,js}` [HI.09]). **Would change if:** the owner wants shaders and menus on the web.

## 7. Feature → host or core
| Feature | Owner | Core provides |
|---|---|---|
| Palettes, colour kill display | host helper / adapter (outside hashes) | native MARIA colour per pixel |
| CRT shaders, overlays, bezels, screenshots, recording | RetroArch | nothing |
| Composite artifact filter (Tower Toppler) | host helper in C, opt-in | 320-wide native pixels |
| Cheats | RetroArch cheat engine | memory maps (§2); `retro_cheat_set` no-op |
| Patches (IPS/BPS/UPS) | RetroArch softpatching (`need_fullpath` false) | load copies bytes |
| Rewind, run-ahead, netplay | RetroArch | fixed-size deterministic LE state |
| Input mapping, remaps, turbo | RetroArch; adapter maps RetroPad → device | electrical pin model per port |
| Battery files | host writes `.srm` / runner writes `.sav` | battery span + save generation |
| Achievements | RetroArch (rcheevos) | descriptors |
| Region detect, fast boot, HSC/SaveKey/POKEY/XM | core | — |
| Lag counter, line stats, trace | core (zero cost off) | inspection + hooks |

## Open questions
1. RetroArch's cheat engine: descriptors or `SYSTEM_RAM`? 10-minute local check with one libretro-database `.cht`.
2. Do 7800 RA sets read registers, $3000–$3FFF or banked RAM? One scan of all set definitions with the owner's RA web API key.
3. RetroArch DMG on SIGTERM: does it flush `.srm`, and with what status (Playstead's play route)?
4. Which driver does the Metal build pick by default on Apple Silicon, and does Metal honour the swapchain setting?
5. HSC: HLE or firmware file (decides `firmware2`)? 242 vs 243 rows (A7800-SYSTEM)?
6. Playstead: zip/CLI install support and two-artifact pins (RetroArch + core).

## Sources
| ID | Tier | Source (URL) | Pin or access date | Supports |
|---|---|---|---|---|
| HI.01 | T1 | github.com/libretro/RetroArch `libretro-common/include/libretro.h` | `69a4f0ea1e8a` (v1.22.2) | entry points, env ids, devices, descriptors, contexts |
| HI.02 | T1 | github.com/libretro/libretro-core-info `prosystem_libretro.info`; libretro-super `dist/info` (same blob) | `5a74858ab2f7`; `f0308e8b8765` | ProSystem claims, BIOS, info location |
| HI.03 | T1 | github.com/libretro/libretro-database `dat/System.dat` L74–76, `metadat/no-intro` + `headered/Atari - 7800.dat`, `cht/Atari - 7800/` | `12c21cf79f1e` | BIOS names/md5, DAT mix, cheat addresses (bucketing tested) |
| HI.04 | T1 | github.com/RetroAchievements/rcheevos `src/runtime/rc_consoleinfo.c` L339–351, `src/libretro/rc_libretro.c` L28–200, L497–770, `src/hash/rc_hash_rom.c` L23–28 (MIT) | `116d01847075`; tags v10.0.0–v12.0.0 | map, resolver, disallowed options, hash |
| HI.05 | T1 | RetroArch `CHANGES.md` (1.10.3 hardcore frame delay, 1.15.0 MoltenVK default + preemptive, 1.20.0 ProMotion + auto frame delay), `retroarch.cfg` L189–331, `docs/retroarch.6` | `69a4f0ea1e8a` | settings, defaults, CLI |
| HI.06 | T1 | docs.libretro.com `library/prosystem/`, `guides/runahead/`, `guides/memorymonitoring/` | accessed 2026-10-15 | ProSystem controls/options, run-ahead, support caveat |
| HI.07 | T2 | formulae.brew.sh/api/cask/retroarch-metal.json, retroarch.json | accessed 2026-10-15 | casks, `no_check` |
| HI.08 | T1 | Local measurement: DMG download + `shasum -a 256`; GitHub releases listing | 2026-10-15 | DMG digest, newest release (tested) |
| HI.09 | T1 | git.libretro.com/libretro-infrastructure/ci-templates (file list, `*-static-cmake.yml`, `linux-cmake.yml`) | `96f603ee450e` | buildbot contract |
| HI.10 | T1 | git.libretro.com/libretro-infrastructure/libretro-build-amd64-ubuntu `Dockerfile.xenial-gcc9` | `869fce5ac223` | GCC 9, CMake 3.31.7 |
| HI.11 | T1 | docs.retroachievements.org `general/emulator-support-and-issues.html`, `general/hardcore-compliance-requirements.html` | accessed 2026-10-15 | listed cores, hardcore rules |
| HI.12 | T1/T3 | github.com/TASEmulators/BizHawk A7800Hawk `*Controllers.cs`, `*ControllerDeck.cs`, `.ISaveRam`, `.IMemoryDomains`, `.IInputPollable`, `.IEmulator`, `M6532.cs` L25–60, `.IDebuggable`; client `RetroAchievements.Memory.cs` L328–331, `BinaryStateLump.cs`, `HeaderKeys.cs`, `Bk2MnemonicLookup.cs` (MIT per LICENSE) | `c9de9a8bff0f` | devices, SaveRam, lag, RA bus, BK2 |
| HI.13 | T1 | tasvideos.org/Bizhawk/BK2Format, tasvideos.org/EmulatorResources | accessed 2026-10-15 | BK2, 7800 = BizHawk |
| HI.14 | T1 | ale.farama.org/environments/, /vector-environment/; gymnasium.farama.org/api/env/ | accessed 2026-10-15 | obs/action/frameskip/sticky/seed API |
| HI.15 | T1 | github.com/sail-sg/envpool README; arxiv.org/abs/2206.10558 | accessed 2026-10-15 | Atari frames/s (per-core division derived) |
| HI.16 | T1 | lua.org/license.html, /download.html, /ftp/; MAME `3rdparty/lua/src/lua.h`; BizHawk `Assets/dll/lua54.dll` | accessed; MAME `dddf4cd1faee`; `c9de9a8bff0f` | Lua MIT, versions |
| HI.17 | T1 | docs.mamedev.org/debugger/index.html, execution.html | accessed 2026-10-15 | MAME debugger |
| HI.18 | T1 | github.com/7800-devtools/a7800 `README.md` | `7a2afdc1ea08` | DMA-usage targets, peripherals |
| HI.19 | T1 | github.com/dasm-assembler/dasm `docs/dasm.1` L29–31, `docs/dasm.txt` L110; cc65.github.io/doc/ld65.html | `c361b8201320`; accessed | symbol/list/label files |
| HI.20 | T2 | 7800.8bitdev.org "7800 Software Guide", memory map appendix | rev 1320 | shadows, $2800 mirror, $3000+ external |
| HI.21 | T1 | playstead `playstead-mac/Playstead/Adapter/{AdapterPin.json,AdapterHost.swift L83–106,AdapterInstaller.swift L306–335}`, `docs/CONTINUATION-PROTOCOL.md`, `scripts/ci/continuation_protocol.py` L24–150, `Design/SystemAccent.swift` | `012163f047c3` | pin fields, grace, installer, protocol, ids |
| HI.22 | T1 | nesturbator `.planning/preparation/LIBRETRO-AND-RUNNER.md`, `runner/exit_status.h`, `libretro/libretro.c`, `.github/workflows/ci.yml` L193–286, `tests/retroarch/`, `README.md` L1090 | `3200aa868371` | adapter, runner, DMG pin, e2e size, speed |
| HI.23 | T1 | stella-emu.github.io/docs/index.html (AtariVox/SaveKey EEPROM files) | accessed 2026-10-15 | per-device `$FF`-initialised files |
