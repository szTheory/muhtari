# YM2151 profile evidence

**Research date:** 2026-11-11
**Scope:** Source qualification for one YM2151 model inside Muhtari's selected/autodetected XM topology. This is evidence for a later phase decision, not an implementation plan.
**Confidence:** HIGH for YM2151 chip-level busy, timer, and IRQ behavior; MEDIUM for the recommended digital XM map/enable profile; LOW for physical XM clock, wiring, and analog level.

## Recommendation

Use one explicitly named **digital XM YM2151 profile**:

- Decode the YM address/data pair at `$0460/$0461`; `$0461` reads YM status, including busy on D7. Use the existing exact `$0470` XM control latch. Gate register access on control bit 7, and preserve the existing bit 3 HSC, bit 4 POKEY, and bits 5/6 RAM meanings. A write to `$0470` that clears bit 7 disables YM register access; it does not represent a separate YM reset. Do not add a YM latch at `$0471/$0472` or aliases.
- Treat the documented `$84` write as a supported enable example, not an equality comparator. The same latch is a whole-byte control register, so later writes must retain bit 7 whenever YM access should remain enabled. This permits existing XM controls to coexist.
- Advance YM at exactly one YM clock per two MARIA master ticks, independent of CPU slow cycles and HALT. NTSC is the rational rate 315,000,000/88 Hz (about 3,579,545.4545 Hz); PAL is 3,546,894 Hz. Derive it from the tick cadence, not an integer-truncated NTSC Hz value. This is a declared digital timing fit based on the project tick model and MAME's 2x slot-clock implementation; it is not evidence that a physical XM board uses those exact clocks.
- Route YM timer IRQ only when a valid A78 v4 header explicitly enables IRQ source bit 2. Normalize the YM's active-low timer request into the existing bus IRQ owner and deadline. v3 and headerless content has no YM IRQ declaration in this profile.
- Keep public PCM mono. Fold the YM's two internal channels into Muhtari's existing integer mono mix without claiming physical level calibration.

The official XM developer material is still unavailable, so this is a reproducible emulator profile, not a statement of measured XM wiring. No physical clock, IRQ, resistor, or game-compatibility claim follows from it.

## Why this enable interpretation fits the selected scope

The 7800 developer wiki documents `$0470` as XM XCTRL1, says the `$84` example exposes YM and cartridge ROM, and advises changing the value when other hardware must remain present. It places the chip at `$0460/$0461`. MiSTer's facts-only cart decoder identifies XCTRL1 bit 7 as the YM enable, while bit 2 is associated with BIOS/top-slot selection. Together these support treating bit 7 as the YM register-decode control while leaving Muhtari's established bits 3–6 available. Sources: [YM2151 Programming, wiki revision 734](https://7800.8bitdev.org/index.php?title=YM2151_Programming&oldid=734), [MiSTer Atari7800 `rtl/cart.sv`, pin `0dc8ad2e3ff7`](https://github.com/MiSTer-devel/Atari7800_MiSTer/blob/0dc8ad2e3ff7/rtl/cart.sv), and `.planning/preparation/A7800-AUDIO.md` §6 / AUD.03, AUD.09, AUD.17.

There is a real alternate implementation: MAME's BSD-3 `rom.cpp` and `xboard.cpp` at `dddf4cd1faee` use an exact `$84` control value and a YM clock at twice the slot clock. That is implementation precedent, not primary XM wiring evidence. An exact-byte comparison would disable YM after any later control write that sets existing HSC, POKEY, or RAM controls, because Muhtari stores the entire `$0470` byte and uses its bit 3–6 fields independently. It therefore cannot preserve the selected XM subset while those controls are active. Bit-7 decode avoids this conflict and agrees with MiSTer's more recent facts-only model. It remains a consciously selected compatibility fit because the older MAME behavior conflicts.

### Qualified XM implementation sources

| Exact source | Permitted use and audit | What it supports |
|---|---|---|
| [MAME `rom.cpp`](https://github.com/mamedev/mame/blob/dddf4cd1faee/src/devices/bus/a7800/rom.cpp) and [`xboard.cpp`](https://github.com/mamedev/mame/blob/dddf4cd1faee/src/devices/bus/a7800/xboard.cpp), pin `dddf4cd1faee` | Each file is BSD-3 under the project audit in [`PROVENANCE.md`](../../../PROVENANCE.md#2-upstream-audit-license-and-file-headers-read-at-the-pin-2026-10-15); use behavior facts only for this decision. | Older exact `$84` enable and 2x-slot-clock precedent; conflicts with the selected independent bit-7 gate. |
| [a7800 `xm.cpp`](https://github.com/7800-devtools/a7800/blob/7a2afdc1ea08/src/devices/bus/a7800/xm.cpp), pin `7a2afdc1ea08` | BSD-3 file within an overall GPL repository; project audit permits only the individually marked file. No unrelated files opened. | Existing XM RAM/HSC/POKEY subset reference; project already uses it for that profile. |
| [MiSTer `rtl/cart.sv`](https://github.com/MiSTer-devel/Atari7800_MiSTer/blob/0dc8ad2e3ff7/rtl/cart.sv), pin `0dc8ad2e3ff7` | Facts only: third-party edits are recorded in [`PROVENANCE.md`](../../../PROVENANCE.md#2-upstream-audit-license-and-file-headers-read-at-the-pin-2026-10-15). | YM control bit 7, bit 2 top-slot/BIOS control, and clock enable tied to CPU phase enables. |
| [7800 developer wiki, YM programming](https://7800.8bitdev.org/index.php?title=YM2151_Programming&oldid=734) and [detection](https://7800.8bitdev.org/index.php?title=YM2151_Detection&oldid=869) | T2 facts only; site has no declared license, per [`PROVENANCE.md`](../../../PROVENANCE.md#2-upstream-audit-license-and-file-headers-read-at-the-pin-2026-10-15). | Register protocol, `$84` recipe and timer-status diagnostic, not copied prose or code. |

These three implementation sources disagree about some behavior. Their role here is to bound viable digital contracts, not to vote on physical truth. The project already records the exact pins and per-file license decisions; no GPL/LGPL/unlicensed emulator source was opened.

Use the same `$0470` latch already owned by the XM subset. Muhtari currently stores exact writes to `$0470`, stores CPU/MARIA page values at `$0478`/`$047C`, and leaves `$0471`/`$0472` and neighboring aliases without an effect. No permitted source describes a separate YM enable latch. Creating one would be an unsupported new mapping. Current behavior and owners are documented in [cartridge hardware](../../../docs/hardware/cartridge.md#xm-rampokeyhsc-subset) and visible in [`src/cart.c`](../../../src/cart.c#L926), [`src/cart.c`](../../../src/cart.c#L939), and [`src/cart.c`](../../../src/cart.c#L1172).

## Options compared

| Option | Evidence and cost | Disposition |
|---|---|---|
| Do nothing | Keeps current fail-before-allocation YM refusal and avoids new sound/state behavior; the selected homebrew capability remains unavailable. Current refusal is in [`src/instance.c`](../../../src/instance.c#L351). | Safe fallback only if the documented digital profile or approved C17 port proves infeasible. It is not needed to resolve this profile. |
| One documented digital profile | Bit-7 access gate on the existing exact `$0470` latch; fixed regional MARIA-tick/2 YM clock; v4-declared IRQ only; current XM bits 3–6 preserved. Uses the developer register recipe plus two implementation precedents, while stating the physical gap. | **Recommend.** Fits the selected one-XM profile and gives a testable bus/audio contract without implying hardware measurement. |
| Physical-calibrated model | Requires a reliable XM schematic or measured board evidence for control decode, clock source, interrupt route, and audio network/level. No such qualified XM evidence was found in the supplied references; the original developer page could not be retrieved. | Not supported by present evidence and explicitly outside the claim. Reopen only if a permitted schematic or measurement becomes available. |

## Chip behavior and interrupt ownership

Yamaha's datasheet identifies YM2151 IRQ as open-drain and active low when either timer overflows; it gives 3.58 MHz as the standard clock and a 3.0–4.0 MHz operating range. Its application manual describes the busy flag, timer A/B periods, timer flags, and timer IRQ enable/reset behavior. The programming reference places the busy bit at status D7 and provides a Timer-B detection workload. Sources: [Yamaha YM2151 datasheet](https://bitsavers.org/components/yamaha/YM2151_199112.pdf), [Yamaha YM2151 application manual](https://map.grauw.nl/resources/sound/yamaha_ym2151_synthesis.pdf), [YM2151 Detection, wiki revision 869](https://7800.8bitdev.org/index.php?title=YM2151_Detection&oldid=869).

The A78 v4 header specification assigns IRQ bit 2 to YM2151. That provides a narrow, explicit digital route. Muhtari currently accepts effective IRQ bits 0–1 only in a validated v4 block and samples POKEY requests in the existing bus-cycle IRQ pipeline. Extend the owning route only for valid v4 bit 2; keep YM IRQ disabled for v3/headerless metadata and ignored v4 blocks. The header definition is [A78 Header Specification, revision 1120](https://7800.8bitdev.org/index.php?title=A78_Header_Specification&oldid=1120); current owners are [`src/cart.c`](../../../src/cart.c#L876) and [`src/bus.c`](../../../src/bus.c#L343).

The Yamaha source establishes the chip pin's electrical polarity and timer cause, not that a particular XM board connects it to the console's IRQ line. The v4-gated route is therefore a documented software profile. Keep the IRQ claim to the declared A78 wiring contract, with the existing bus pipeline owning observation/deadline behavior; do not route YM IRQ unconditionally.

## Clock and audio limits

The project time base defines the MARIA master tick as 315,000,000/44 Hz (about 7,159,090.9091 Hz) for NTSC and 7,093,788 Hz for PAL. The candidate YM cadence is one YM clock per two MARIA ticks: NTSC 315,000,000/88 Hz (about 3,579,545.4545 Hz), PAL 3,546,894 Hz. MAME's audited BSD-3 implementation derives YM clock as 2x the slot clock, matching this cadence in the modeled timeline. The Yamaha datasheet's nominal 3.58 MHz is compatible with the NTSC candidate; both regional rates are inside its listed operating range. MiSTer's facts-only model instead advances the YM on CPU phase enables. No permitted source establishes which clock or slow-cycle/HALT behavior a physical XM board uses. The rates above are thus a deterministic profile choice, not recovered hardware truth. The earlier proposal's `crystal/2` shorthand is ambiguous: this recommendation means MARIA-tick/2, or original master-oscillator/4. See `.planning/preparation/A7800-AUDIO.md` §1, §6 and AUD.03/AUD.09.

The YM2151 produces separate left and right DAC streams, while Muhtari's public audio output remains mono. Preserve the public shape and combine those channels inside the existing deterministic integer mixer. The board's YM gain and stereo-to-mono electrical network are not calibrated by the cited material. Do not claim physical loudness balance or calibrated analog output. The official datasheet's stereo outputs do not determine the XM resistor network.

## Approved seed, exact files, and maintenance

`PROVENANCE.md` §4 approves the BSD-3 ymfm OPM seed at pin `81aec25ccbb98f4873a255f7551ac4dadac59b4a` (project short pin `81aec25ccbb9`) and requires a C++-to-C17, OPM-only, integer specialization. The minimal upstream dependency set for that OPM path is:

| Upstream file at the pinned revision | Role |
|---|---|
| [`src/ymfm_opm.cpp`](https://github.com/aaronsgiles/ymfm/blob/81aec25ccbb98f4873a255f7551ac4dadac59b4a/src/ymfm_opm.cpp) | OPM implementation and inclusion of the FM implementation |
| [`src/ymfm_opm.h`](https://github.com/aaronsgiles/ymfm/blob/81aec25ccbb98f4873a255f7551ac4dadac59b4a/src/ymfm_opm.h) | OPM interface; requires the common and FM headers |
| [`src/ymfm_fm.ipp`](https://github.com/aaronsgiles/ymfm/blob/81aec25ccbb98f4873a255f7551ac4dadac59b4a/src/ymfm_fm.ipp) | Shared FM implementation templates required by OPM |
| [`src/ymfm_fm.h`](https://github.com/aaronsgiles/ymfm/blob/81aec25ccbb98f4873a255f7551ac4dadac59b4a/src/ymfm_fm.h) | Shared FM declarations required by OPM |
| [`src/ymfm.h`](https://github.com/aaronsgiles/ymfm/blob/81aec25ccbb98f4873a255f7551ac4dadac59b4a/src/ymfm.h) | Common ymfm types and state support |

The pin's [`LICENSE`](https://github.com/aaronsgiles/ymfm/blob/81aec25ccbb98f4873a255f7551ac4dadac59b4a/LICENSE) and those source-file headers are BSD-3-Clause. Keep the attribution/SPDX treatment and full license text in `THIRD-PARTY-NOTICES.md` per [PROVENANCE.md](../../../PROVENANCE.md#4-strangler-fig-seeds), including every distributed library, runner, and libretro archive. Do not import other chip modules or a C++ runtime/package. Treat the original pinned OPM as a test oracle for port equivalence, not as independent proof of XM hardware behavior.

The preparation inventory counts 3,482 C++ lines across the needed OPM/FM/common sources and estimates a specialized C port at 1,500–2,000 C lines; that estimate is untested. The upstream implementation uses C++ facilities, so production use requires a genuine C17 port with all state instance-owned, fixed-width integer arithmetic, deterministic output, and bounded per-call work. Project guidance records the runtime arithmetic as integer/fixed-point. The port is plausible within existing ownership rules but not validated until a later implementation measures its size, speed, state and cross-platform determinism. Maintenance duty is to retain the exact source pin/notices and use its equivalence corpus when modifying or updating the specialized port; revisit replacement only if measured size or speed demands it.

## Evidence classes and remaining flip condition

| Claim | Evidence class / confidence | Limit |
|---|---|---|
| `$0460/$0461`, busy status D7, `$0470` enable recipe | 7800 developer wiki, T2 / MEDIUM | Curated programming documentation, not the original XM specification. |
| D7 as independent YM enable and exact `$0470`-byte alternate | MiSTer cart decoder and MAME/a7800 source, T3 / MEDIUM | Emulator/RTL behavior; the qualified MiSTer file is facts-only. MAME's exact `$84` behavior conflicts with the selected D7 profile. |
| Chip busy/timer/IRQ electrical semantics and nominal clock | Yamaha chip documentation, T1 / HIGH for chip semantics | Does not establish how XM connected the chip. |
| v4 IRQ source bit 2 | 7800 A78 header specification, T2 / MEDIUM | Explicit metadata contract; not a measurement of a specific cartridge. |
| Fixed regional MARIA-tick/2 clock | Project rational tick model plus MAME precedent, T3/derived / MEDIUM as a declared digital profile | NTSC must retain the 315,000,000/88 cadence rather than a truncated integer Hz constant. Not physical XM timing; MiSTer uses a different enable schedule. |
| Mono gain / board level | No qualifying XM analog evidence / LOW | No calibration or hardware listening claim. |
| Exact seed licensing and scope | Pinned ymfm tree, file headers, license, and project audit / HIGH | The C17 specialization itself and its performance remain unvalidated. |

The official XM developer page linked by the 7800 wiki could not be fetched during source review; the project preparation records its earlier DNS failure/archive rate limit. This is no observation from that page, not evidence that it lacks a specification. A trusted original XM schematic/specification or reproducible hardware measurement that requires a different enable condition, clock, IRQ path, or audio network is the flip trigger. In particular, evidence that D2 must also gate YM access would require revisiting the current digital profile; until then, bit 7 is the selected and disclosed model.

No ROM/firmware was acquired or opened, no prohibited emulator source was consulted, and no build, test, host run, or implementation was performed for this evidence note.
