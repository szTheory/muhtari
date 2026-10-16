---
title: "A7800 audio: TIA, board mix, cartridge POKEY, XM, synthesis"
summary: "Clock-exact facts and decisions for 7800 sound: TIA on its own 456-tick line, the 18 kOhm / 6.8 kOhm board mix, POKEY on the stretched cartridge phase 2, the POKEY seed, XM/YM2151 via ymfm, and integer band-limited synthesis with a transition-stream hash."
read_when: "Before designing, implementing or testing TIA audio, pokey.c, synth.c, the mixer, A78 audio/IRQ flags, XM/YM2151, audio hashes or audio ledger predicates."
updated: 2026-10-15
---

# A7800 audio

## Where to look
| Question | § |
|---|---|
| Clock rates, output-rate ratios, samples per frame | 1 |
| TIA audio clock (228 vs 227 ticks), AUDC table, AUDF, AUDV, latency, INPTCTRL gating, power-on | 2 |
| Board audio path, TIA vs cartridge level, integer mix, clipping bound | 3 |
| POKEY addresses, clock (slow cycles, HALT), timers, joins, two-tone, high-pass, polys, RANDOM, init, IRQ, titles | 4 |
| POKEY seed: port MAME vs own from the Altirra manual (v1, v4) | 5 |
| XM, YM2151, ymfm port | 6 |
| BupChip, COVOX, ADPCM, Minnie, SN76489 | 7 |
| Synthesis, hashes, A/V sync, performance bound | 8 |
| Tests, 7800basic samples, hardware diagnostics | 9 |
| Open questions and risks | 10 |

Licence status of the main T3 sources: MiSTer `rtl/TIA.sv`, `rtl/top.sv`, `rtl/cart.sv` carry third-party edits made while the repo was CC BY-NC, so they are **facts only**: no code, table or structure from them may enter muhtari, and none is a seed. `rtl/Pokey/*` and `rtl/bupchip_*` are MIT and author-only. MAME/a7800 `pokey.cpp` is BSD-3 (Oliver, Smith, Buchmueller; a7800 improvements by Mike Saarna; "original info" from Ron Fries' emulator; polynomial info from Perry McFarlane). MAME TIA sound, jt51, Nuked-OPM, Stella, ProSystem and Altirra source stay unopened.

## 1. Time base
| Quantity | NTSC | PAL | Source / derivation |
|---|---|---|---|
| Tick = MARIA clock = crystal/2 | 315 MHz/44 = 7,159,090.91 Hz | 7,093,788 Hz | [AUD.08] |
| TIA oscillator (colour clock) | tick/2 = 3,579,545.45 Hz | 3,546,894 Hz | [AUD.01][AUD.08] |
| TIA line (own H counter) | 57 states × 4 colour clocks = 228 cc = **456 ticks** (MARIA line: 454) | same | [AUD.01] |
| TIA audio clock (mean) | 456/2 = 228 ticks → **31,399.52 Hz** | 31,113.11 Hz | (derived) |
| Cartridge φ2 (POKEY clock), nominal | tick/4 = 1,789,772.73 Hz; 6 ticks per TIA/RIOT cycle | 1,773,447 Hz | [AUD.13][AUD.20] |
| POKEY 64 kHz / 15 kHz bases | φ2/28 = 63,920.45 Hz; φ2/114 = 15,699.76 Hz | 63,337.39 / 15,556.55 Hz | [AUD.06] |
| YM2151 on XM (if 2×φ2), output rate clock/64 | 3,579,545 Hz; 55,930.40 Hz | 3,546,894 Hz; 55,420.22 Hz | [AUD.09]; real XM source (unverified) |
| Frame | 263 × 454 = 119,402 ticks; 39,375,000/656,711 = 59.95788 Hz | 313 × 454 = 142,102; 3,546,894/71,051 = 49.92040 Hz | (derived) |
| 48 kHz, samples per tick | **88/13,125** (48,000 × 44/315 M; gcd 24,000) → 800.562 per frame | 4,000/591,149 → 961.531 per frame | (derived, tested) |
| 44.1 / 96 kHz | 77/12,500; 176/13,125 | 3,675/591,149; 8,000/591,149 | (derived, tested) |

## 2. TIA audio
**Clock.** The TIA runs from its own oscillator and its own 6-bit LFSR horizontal counter, not MARIA's line; hardware shows TIA-WSYNC bars drifting against MARIA lines because the TIA line is longer [AUD.31]. Counter decodes: SHS 4, RHS 8, RCB 12, RHB 16, LRHB 18, CNT 36, END 56 (tested: LFSR stepped here). Audio phase A fires at counts 0 and 18 (SHB, LRHB), phase B at 8 and 36 (RHS, CNT); the output can change only at B, so B events sit at line ticks 64 and 288: spacing alternates **224 / 232 ticks**, mean 228 [AUD.01] (tested). MAME clocks an evenly spaced 228-tick sample (right rate) [AUD.08]; A7800Hawk clocks at counts 113 and 340 of the 454-tick MARIA line = 227 ticks = 31,537.85 Hz, **+7.6 cents sharp** [AUD.12]. An RSYNC ($03) write restarts the counter and shifts the audio phase [AUD.01].

**Channel logic** (facts; simulated here from [AUD.01], tested, and cross-checked against [AUD.16]): a 5-bit up-counter is compared with AUDF at phase A; on a match it restarts at 0 and the shift registers step at the next B, so the period is (AUDF+1) audio clocks. There is no reload latch: lowering AUDF below the running count lets the counter run past 31 and wrap. A switch 20→5 at count 12 gave the next step 26 clocks later; the worst case is 32 clocks ≈ 1.02 ms. A 4-bit and a 5-bit shift register with AUDC-selected feedback form poly4, poly5, poly9 and the dividers.

| AUDC | Output (AUDF = 0, unit = audio clock) | Period | Ones | Agreement with the Fries-derived guide [AUD.16] |
|---|---|---|---|---|
| 0, 11 | constant high once the register fills (from zero: 4 steps for 0, 10 for 11); AUDV writes then give 4-bit PCM | 1 | 1 | yes |
| 1 | poly4 | 15 | 8 | yes, opposite polarity |
| 2 / 3 | poly4 gated by div 31 / by poly5 | 465 | 248 | period yes; patterns unconfirmed |
| 4, 5 | div 2 | 2 | 1 | yes |
| 6, 10 | div 31 (10 = complement of 6, same sound) | 31 | 13 / 18 | yes |
| 7, 9 | poly5 (7 = complement of 9, same sound) | 31 | 15 / 16 | yes |
| 8 | poly9 (both registers chained) | 511 | 256 | yes |
| 12, 13 | div 6 | 6 | 3 | yes |
| 14 | div 93: high 44, low 49 | 93 | 44 | **no**: the guide holds AUDC 6's pattern ×3 (39 ones) |
| 15 | poly5 with div 3 | 93 | 46 | **no**: the guide holds poly5 ×3 (48 ones) |

Frequency = 31,399.52 / ((AUDF+1) × period) Hz, e.g. AUDC 4 / AUDF 0 = 15,699.8 Hz and AUDC 12 / AUDF 31 = 163.5 Hz. The 7800 TIA pitch equals the 2600's (both 3.58 MHz/114).
- **AUDV and latency:** channel level = AUDV × output bit, and a write reaches the pin at once (combinational path), so stamp it at the write cycle's end tick. AUDF acts at the next phase A compare; AUDC acts at the next A (noise feedback) and B (pulse feedback), 0–232 ticks later [AUD.01].
- **Power-on:** the TIA has no reset pin, so its state is indeterminate. An all-zero state is safe: the NOR feedback escapes the all-zero lock-up (tested). Whether the BIOS leaves TIA audio state behind is unknown; Ninja Golf's title music reportedly depends on BIOS residue [AUD.33] (unverified).
- **Two-channel mix:** AUD0 (pin 13) and AUD1 (pin 12) are tied together with a 1 kΩ pull-up R24 [AUD.13]. The summed level follows **n/(n+30)**, n = Σ active volumes (0–30). MiSTer's 31-entry table fits that law within 1.8/32,768 (derived from [AUD.02]; the measurement thread [AUD.34] is unverified), and its developers report A/B listening tests matching hardware, including this "rumble at loud volumes" [AUD.30]. Both channels at 15 are therefore only 1.5× one channel at 15.
- **INPTCTRL:** TIA chip selects come from A7, A12 and MARIA's TIACS decode, not from the INPTCTRL latch [AUD.13]. INPTCTRL decodes the same $00–$1F range, so writes before lock reach both [AUD.31]. MAME forwards every write [AUD.08]; a7800 forwards only when `(INPTCTRL & 3) == 3` [AUD.11], which would also mute 2600 mode.

**TIA-1 clock.** **Recommendation:** step TIA audio on its own 456-tick lattice with half-steps A at line ticks {0, 144} and B at {64, 288}; power-on line phase 0 at tick 0 (a profile constant shared with TIA WSYNC in A7800-SYSTEM). **Strongest alternative:** MAME's evenly spaced 228 ticks (same pitch, simpler). **Would change if:** a logic-analyser capture of the AUD pins shows evenly spaced edges.
**TIA-2 gating.** **Recommendation:** no INPTCTRL gate; every $00–$1F write (and mirrors) reaches the TIA. **Strongest alternative:** a7800's `& 3 == 3` gate. **Would change if:** the §9 lock-write diagnostic stays silent on hardware.
**TIA-3 implementation.** **Recommendation:** own functional model (counter + two shift registers + AUDC feedback, two half-steps) written from public documentation (Stella Programmer's Guide AUDC table, [AUD.16], TIA schematics, H-counter decodes) and tested against the table above. TIA.sv is evidence only. **Strongest alternative:** none is permissive at die level (MAME tiasound GPL; A7800Hawk `TIA_Sound/` excluded). **Would change if:** TIA.sv's third-party edits were relicensed MIT. Then it could become a Verilator oracle.

## 3. Board audio path and mix
- **Schematic C025231-001 (NTSC, 1984-03-23)** [AUD.13]: the TIA node (R24 1 kΩ to +5 V) → C14 0.1 µF → R6 **18 kΩ** → AUDIO node. Cartridge EAUD (J1 pin 18) → C10 0.1 µF → R5 **6.8 kΩ** → the same node, which feeds J2-17 (peripheral AUDIO) and the RF modulator. There is no console-side pull-up on EAUD (the cartridge loads its own POKEY), no board low-pass, and both paths are AC-coupled. The PAL board is unchecked.
- **Weights:** for equal source voltages the cartridge counts 18/6.8 = **2.65×** the TIA [AUD.25]. High-pass corners into a low-impedance node: TIA 1/(2π·18.5 kΩ·0.1 µF) ≈ 86 Hz; cartridge ≈ 204–234 Hz (derived; the node load, i.e. the modulator input, is unverified). RF/TV de-emphasis is outside the console (unverified). MiSTer's author suggested a ≈ 6 kHz low-pass without knowing whether hardware has one [AUD.30].
- **POKEY level:** four DACs drop 0.12 / 0.26 / 0.56 / 1.12 V per volume bit (not 1:2:4:8). The sum saturates: y = 2.171x for x ≤ 0.14, else 2.171(0.14 + (1 − e^(−2.85(x−0.14)))/2.85), with x = Σdrops / 8.24 V and y = 1 ↔ ≈ 3.80 V. This was measured on an 800XL load [AUD.06]; the cartridge pull-up is unverified.
- **Ratio (derived):** TIA full scale ≈ 2.5 V/18.5 kΩ = 135 µA; one POKEY channel at 15 (x = 0.25, y = 0.509) ≈ 1.93 V/6.8 kΩ = 284 µA = **2.1× TIA full scale**; four saturated channels ≈ 558 µA. MiSTer gives one channel ≈ 1.0× and the maximum ≈ 2.0× [AUD.02][AUD.04]. No 7800 mix recording was found; Trebor's hardware recordings are POKEY-only demos [AUD.29]. Tapping before the resistors lets TIA drown POKEY; Commando (TIA + POKEY) is the balance test [AUD.25].

**MIX.** **Recommendation:** an integer node-current mix, `sum = T[n]·wT + Σchips P[raw]·wP (+ later sources)`.
- T (31 entries) and P (825 entries, raw = Σ weighted drops in 10 mV units) are owned tables, generated offline from the formulas above and checked in as integers.
- wT : wP = (2.5 V/18 kΩ) : (3.80 V/6.8 kΩ), with one profile constant `cart_gain` (Q12, default 4096) for later calibration.
- The PCM scale is chosen from the declared audio hardware so the largest possible DC sum maps to ≤ 32,767; after the DC blocker a step can swing ±max, so clipping is impossible by construction.

**Strongest alternative:** MiSTer-style fixed per-source scaling, halved when external audio is present (louder, rare clipping). **Would change if:** a hardware recording of equal-volume TIA and POKEY tones (Utility Cart style, §9) gives a ratio outside 1.5–2.7×; then retune `cart_gain`.

## 4. POKEY on 7800 cartridges
| Item | Facts |
|---|---|
| Windows | **$4000–$7FFF**: 16 registers mirrored; A78 bit 0; Ballblazer, Commando (SuperGame), Beef Drop POKEY edition, Froggie [AUD.09][AUD.10]. **$0450–$045F**: bit 6; Bentley Bear, RMT demos (E.X.O. and A.R.T.I. likely, address unverified). **$0440–$044F**: bit 10, second chip; MiSTer plays $440 left / $450 right; Dragonfly mixes both onto EAUD; HOKEY answers at both [AUD.03][AUD.29]. **$0800–$0FFF**: bit 15; PETSCII Robots, Drone Patrol [AUD.28]. XM: $0450 behind XCTRL ($0470) bit 4. v4 audio code (bits 0–2): 0 none, 1 @440, 2 @450, 3 @450+@440, 4 @800, 5 @4000 [AUD.14]. MiSTer makes the $4000 window write-only when it overlaps RAM or a bankset [AUD.03]. |
| Clock | POKEY φ2 = cartridge pin 32 CLK2 = Sally's φ2 output [AUD.13], so a TIA/RIOT access stretches it to 6 ticks. Hardware: song pitch drops while code polls inputs [AUD.26]. φ2 keeps running through MARIA HALT: RIOT on the same clock counts 113 per line with DMA on or off [AUD.27]. Cycles per frame = 29,850.5 − S/2 for S slow cycles (derived). A 6-cycle zero-page `LDA INPT4 / BPL` loop runs POKEY at 24/26 = **−139 cents**; 300 slow cycles per frame give −8.7 cents. MAME and a7800 use a fixed 1.79 MHz, A7800Hawk ticks every 4 ticks [AUD.08][AUD.12], and MiSTer uses the CPU phase enables [AUD.03]. |
| Timers | 1.79 MHz: N+4 cycles; joined pair (AUDCTL bit 4: 1+2, bit 3: 3+4) at 1.79 MHz: N+7; base clocks: (N+1)×28 or ×114. A joined low timer fires AUDF_high+1 times per high period. AUDF takes effect at the next reload; STIMER reloads everything without pulses or IRQs, with the first reload 3 cycles after the write [AUD.06]. |
| Two-tone (SKCTL bit 3) | Timer 2 always resyncs (resets timers 1 and 2); timer 1 resyncs only for a 1 bit; force break (SKCTL bit 7) selects the 0-bit tone. Resync lands 2 cycles after the reload, so a 1.79 MHz timer 1 driving it runs N+6 (fires at 8, 14, 20). A one-cycle-apart pair resyncs twice (+3) [AUD.06][AUD.04]. E.X.O. sets force break [AUD.32]. MAME 4.9 resets with no delay [AUD.07]. |
| High-pass | ch1 by ch3, ch2 by ch4 (AUDCTL bits 2/1; pairs differ from the joins). A flip-flop samples the low channel at the high channel's clock and XORs it before the volume-only override. The ≈1.5-cycle path delay leaves half-cycle pulses, so cancellation is never complete. When disabled the flip-flop is held at 1, which inverts ch1/2 [AUD.06]. MiSTer reproduces the half phase; MAME samples in the same step [AUD.04][AUD.07]. |
| Polys | XNOR LFSRs at 1.79 MHz shared by all channels: poly4 1+x³+x⁴ (000011101100101), poly5 1+x³+x⁵ (0000011100100010101111011010011), 9-bit 1+x⁴+x⁹, 17-bit 1+x¹²+x¹⁷ (AUDCTL bit 7 selects 9). Bits reach ch1–ch4 one cycle apart. AUDC bit 4 = volume only, bit 5 = square, bit 6 = poly4 vs 9/17, bit 7 = 0 lets poly5 mask clocks [AUD.06]. Neither MAME nor MiSTer staggers the channels [AUD.07][AUD.04]. |
| RANDOM ($xA) | Inverted register bits 15..8, shifting 1 bit per cycle. Reads $FF from 9 cycles after init is entered. In 9-bit mode, `STA SKCTL` (leaving init) then `LDA RANDOM` gives $1F [AUD.06]. MAME's sequence passes Tempest's protection check [AUD.07][AUD.32]. A broken RNG makes Ballblazer lose a music voice [AUD.15]. |
| Init (SKCTL bits 0–1 = 0) | Freezes the 15/64 kHz prescalers, polys, serial and keyboard. Timer counters, outputs, AUDx and AUDCTL are **not** reset, so 1.79 MHz channels keep sounding [AUD.06][AUD.04]; MAME/a7800 stop every channel [AUD.07]. On exit the first 64 kHz tick comes 19 cycles later and the first 15 kHz tick 78 cycles later (IRQ at 24 / 83) [AUD.06]. |
| Power-on | There is no reset pin and the state is indeterminate [AUD.06]. a7800 v5.2 and MAME start in init (silent until SKCTL is written) [AUD.28][AUD.07]. MiSTer (2026) leaves init because Cybernoid II, Beef Drop and the Galaxian demos never write SKCTL yet play on hardware [AUD.04]. Donkey Kong XM writes SKCTL = 0 and still sounds [AUD.33]. |
| IRQ | Timers 1, 2 and 4 have IRQs; timer 3 has none [AUD.04]. /IRQ is cartridge pin 31 [AUD.13]. A78 v4 interrupt bits: 0 = POKEY 1, 1 = POKEY 2 (@440), 2 = YM2151 [AUD.14]. No title is known to use POKEY IRQs (unverified). |
| YM/other titles | 1942 and Pac-Man Collection 40th need the XM YM2151 [AUD.28]; Donkey Kong PK (@$450) [AUD.33]. |

**POKEY-CLK.** **Recommendation:** POKEY runs on the bus's φ2 counter (4 or 6 ticks, HALT cycles included). When a POKEY is mapped, the bus calls `pokey_catch_up` before every slow cycle so the φ2-to-tick map stays piecewise linear. **Strongest alternative:** a fixed tick/4 clock (MAME, A7800Hawk). **Would change if:** a capture of CLK2 during `LDA INPT4` shows no stretch.
**POKEY-PWR.** **Recommendation:** power on as if SKCTL = $03 had been written at tick 0 (MiSTer), with a profile option `pokey_power_on = INIT` and a homebrew lint warning "POKEY written but SKCTL never left init". **Strongest alternative:** start in init (a7800, MAME). **Would change if:** a cold-boot hardware test with a cartridge that never writes SKCTL is silent.
**POKEY-IRQ.** **Recommendation:** wire a POKEY's IRQ to the CPU only when its A78 v4 interrupt bit is set; v3 headers are unwired. **Strongest alternative:** always wired. **Would change if:** a released title needs the IRQ without the header bit.

## 5. POKEY seed
| Behaviour (needed by) | MAME v4.9 (BSD-3) [AUD.07] | Altirra manual, docs [AUD.06] | MiSTer rtl/Pokey, MIT [AUD.04] |
|---|---|---|---|
| N+4 / N+7 at 1.79 MHz, joins (E.X.O., A.R.T.I.) | yes | yes, with cycle figure | yes |
| Two-tone +2 resync, force break (E.X.O.) | reset without delay | yes | yes (delay line) |
| High-pass 1.5-cycle offset (E.X.O., A.R.T.I.) | no | yes | yes |
| Init keeps 1.79 MHz channels running | no | yes | yes |
| RANDOM sequence and init (Ballblazer) | yes (Tempest-checked) | yes ($FF, $1F) | yes |
| Per-channel noise stagger | no | yes | no |
| STIMER output state | ch1/2 on | flip-flops to 1 → ch1/2 off | ch1/2 on (CO12294; open) |
| Analog DAC + saturation | linear or float RC | App. E | integer App. E fit |
| Execution model | per cycle, 512 KiB poly17 table, float paths | none | per-cycle RTL (Verilator-able) |

Both MAME and a7800 carry the a7800 4.7–4.9 fixes; MAME upstream is the superset [AUD.07][AUD.11].
**SEED-v1.** **Recommendation:** an own integer C POKEY written from the Altirra manual. Catch-up steps per φ2 cycle, with an idle fast path to the next prescaler tick; poly4/5/9 are tiny tables and poly17 is a 16 KiB bit table. MAME v4.9 is read-only cross-checking for RANDOM and joins; any lifted line carries the full BSD-3 notice and attribution. **Strongest alternative:** port MAME v4.9 to integer C (the design spine's plan, but upstream rather than a7800), then fix the four deviations above. **Would change if:** by mid-P4 the own model still fails the §9 timer and RANDOM vectors.
**SEED-v4.** **Recommendation:** run Verilated MiSTer `rtl/Pokey` (self-contained module, MIT) nightly on register-write traces recorded from E.X.O., A.R.T.I., Ballblazer and the RMT demos, comparing per-channel DAC edges cycle-for-cycle and allowing only documented deviations; add owner-console recordings. **Strongest alternative:** a7800 v5.2 black-box audio captures (no cycle timing). **Would change if:** Verilator harness effort exceeds 2 days.

## 6. XM expansion and YM2151 (v4.0)
- **Map:** POKEY at $0450 and an optional second POKEY at $0460, both enabled by XCTRL bit 4; XCTRL at $0470, bits 0–2 select the RAM bank and bit 3 enables RAM [AUD.09]. The YM2151 sits at $0460 (address) and $0461 (data); poll bit 7 of $0461 for busy. Writing $84 to $0470 maps the YM and cartridge ROM [AUD.17]. MAME enables the YM only on exactly $84 [AUD.09]; MiSTer uses XCTRL bit 7 or header bit 11 [AUD.03]. Official XM documents are unreachable (unverified) [AUD.35].
- **Clock:** MAME uses 2 × the slot clock = 3,579,545 Hz [AUD.09]; MiSTer clocks jt51 on both CPU phase enables, so it would stretch [AUD.03]. The real XM source is unverified. Detection check: Timer B with CLKB = 252 lasts 1,024 × 4/3,579,545 = 1.144 ms, which is 136.5 iterations of the 15-cycle poll loop, so the expected count is $88 ([AUD.17], derived).
- **ymfm** (BSD-3, @81aec25ccbb9) is integer-only at runtime; its "floating point" is the DAC's 3.10 integer format. OPM uses ymfm.h 566 + ymfm_fm.h 463 + ymfm_fm.ipp 1,592 + ymfm_opm.h 322 + ymfm_opm.cpp 539 = 3,482 C++ lines of templates over register sets. A C port specialised to OPM should be ≈ 1,500–2,000 lines (estimate, untested). Output is one stereo sample per 64 YM clocks (128 ticks at 2×φ2). Nuked-OPM (LGPL) and jt51 (GPL) are excluded [AUD.21].

**YM.** **Recommendation:** in v4, port ymfm OPM to C with the BSD-3 notice; feed one (tick, level) per YM sample; clock it from the φ2 counter ×2 until the XM clock is known. **Strongest alternative:** keep ymfm in C++ behind a C ABI (upstream fixes for free, but every port, including the PSP, DS and Wii lanes, would need C++). **Would change if:** XM photos or a schematic show a separate oscillator; then clock it from ticks/2.

## 7. Other cartridge audio (scope only)
| Device | What it is | Docs / licence | When |
|---|---|---|---|
| BupChip (Souper boards; Rikki & Vikki) | Coprocessor rendering the "CoreTone" soft-synth (samples, instruments and songs in the cartridge) onto EAUD. The game sends commands as $8007 write pairs; MiSTer runs firmware on an emulated ARM7TDMI feeding a 48 kHz PCM FIFO [AUD.05]. Only js7800 plays the converted image [AUD.18]. TIA effects must work without it [AUD.15]. | `souper.v` is zlib (2015); BupChip firmware and docs are not public (unverified) | v4+, high-level player |
| COVOX @$0430 | 8-bit DAC; A78 v4 audio bit 4; MiSTer always decodes $0430–$0433 (L = r1 + r3, R = r0 + r2) [AUD.03][AUD.14] | — | v2 (one event source) |
| ADPCM stream @$0420 | A78 v4 audio bit 5 [AUD.14]; device unidentified (unverified) | — | when documented |
| Minnie (GCC 1730) | Never-produced 3-voice wavetable chip for EAUD (16-bit frequency, 28 kHz, 10-bit) [AUD.19] | GCC specs on the wiki | no |
| SN76489 / YM2413 carts | ACE board SN76489 at $043F [AUD.03]; an SN76489 + YM2413 PCB exists (unverified) | — | later |

## 8. Synthesis, hashes, A/V sync
- **Sibling precedent** [AUD.23]: each change of the mixed level adds delta × a linearly interpolated kernel row to an integer ring buffer. The kernel is 16 taps × 32 phases, Kaiser β 8 over 15 samples, Q15, each row summing to exactly 32,768; a running sum gives the output. Measured: worst non-harmonic component −87 dB below 16 kHz.
- **Retiming:** the kernel is defined in output samples, so it carries over unchanged. Phase accumulator (NTSC, 48 kHz): +88 per tick modulo 13,125; kernel row = acc·32/13,125, remainder = interpolation (acc·32 < 2^19). PAL: +4,000 modulo 591,149 (acc·32 < 2^25). One tick is 1/149.15 sample, and 32 interpolated phases resolve positions finer than a tick.
- **Rates and bound:** TIA ≤ 2 × 31,399.5 = 62,799 changes/s; POKEY ≤ 1 merged change per φ2 cycle = 1.79 M/s per chip (AUDF 0 at 1.79 MHz toggles 447,443/s per channel); YM 55,930/s. Merge all sources per tick before the kernel. Worst case is 28.6 M multiply-adds/s per POKEY (hostile ROM) against about 1 M/s for TIA only.
- **Why band-limit:** test7800's E.X.O. title-music errors were aliasing of 1.79 MHz tones at a 31,469 Hz output rate [AUD.32]. Bentley Bear plays a 32 kHz tone (AUDF 0, AUDC $Ax) that must stay inaudible [AUD.33].
- **A/V:** frames are 800 or 801 samples (NTSC 48 kHz, fraction 0.562 carried) and 961 or 962 (PAL). libretro reports 39,375,000/656,711 fps and 48,000 Hz, and RetroArch's dynamic rate control absorbs the 0.07 % gap to 60 Hz. Never pad samples per frame.

**SYN-1.** **Recommendation:** reuse the sibling kernel with per-tick merging and a single first-order DC blocker (≈ 20 Hz, Q30 integers, coefficients checked in); drop the NES corner filters. RF/TV colouring becomes an optional host filter outside the hashes. **Strongest alternative:** model the board's 86 Hz / ≈ 220 Hz coupling high-passes. **Would change if:** the node load is measured.
**SYN-2.** **Recommendation:** if the hostile 4-channel 1.79 MHz ROM costs more than 10 % of frame time on the low-power bench target, add a PCM-only "box bins" profile for POKEY (exact tick integration into 1/8-sample bins, ≤ 8 kernel adds per sample). The transition hash is unchanged by it. **Strongest alternative:** a 1,024-phase table without interpolation (32 KiB) [AUD.23]. **Would change if:** the measurement shows the dense case stays within budget.
**HASH.** **Recommendation:** the primary hash is SHA-256 over little-endian records {u64 tick, u8 source, u16 level}, one per per-channel DAC change. Sources: TIA0/1, POKEY a/b ch1–4, YM L/R, COVOX. It is independent of mixer tables and output rate and is computed only in the runner via the hook tap (zero cost when off). The PCM hash is secondary, per (rate, profile). **Strongest alternative:** PCM-only hashes (sibling). **Would change if:** hashing costs more than 2 % in the runner. **Before freezing PCM digests:** a synthetic full-scale test once hid 3.04 % clipping [AUD.24]. Run Commando, Ballblazer, E.X.O., A.R.T.I., the RMT demos and TIA-only titles; require 0 clipped samples and a peak ≤ −1 dBFS.

## 9. Tests
| Behaviour | Unit test that proves it |
|---|---|
| TIA clock | B events at line ticks 64 and 288 (224/232 spacing); RSYNC restarts the phase; 1,000 lines = 456,000 ticks |
| TIA AUDC | For AUDC 0–15 at AUDF 0: period and ones from §2; first 40 outputs from the zero state; period × (AUDF+1) for AUDF 1, 5, 31; the AUDF 20→5 wrap (+26 clocks) |
| TIA levels | AUDV write → transition at the write tick; n/(n+30) table endpoints and monotonicity |
| POKEY timers | N+4, N+7, (N+1)×28 and ×114 for N ∈ {0, 1, 255}; Altirra Fig. 8: STIMER at T → IRQST at T+8, preempt by T+4, reloads T+3/T+7, two-tone fires 8/14/20; joined low fires AUDF2+1 times |
| POKEY filter and init | half-cycle high-pass pulse; in init a 1.79 MHz channel keeps toggling while a 64 kHz channel stops; first prescaler ticks at 19/78 cycles |
| POKEY polys and RANDOM | poly4/poly5 pattern strings; per-channel 1-cycle stagger; RANDOM = $FF from 9 cycles into init, $1F after `STA SKCTL`/`LDA RANDOM` in 9-bit mode, and the 9-bit sequence 00 DF EE 16 B9 when read every 114 cycles [AUD.06] |
| POKEY state and clock | STIMER output state; volume-only overrides high-pass and noise; 100 slow cycles delay a 1.79 MHz channel's edges by 200 ticks |
| Mixer | P curve at x = 0.14 → 0.30394 and x = 1 → 1.0000 (±1 LSB); no overflow at the maximum declared hardware |
| Synth | sibling spectral test retimed (TIA AUDC 4 sweep; POKEY AUDF 0/1/3 at 1.79 MHz below −80 dB in band); exact 800/801 cadence over 1,000 frames |
- **7800basic v0.41 samples** [AUD.22] (built in CI; ROM and audio hashes pinned): `soundtest` and `soundeffects` (TIA effects; seed `rand`), `trackertest` (TIA tracker), `pokey` (detection: found at $450 only when the header declares it), `rmtromdemo`, `rmtramdemo` (128 KB RAM) and `rmtramcompressiondemo` (RMT music at $450). `atarivoxspeech` is external speech, not core audio.
- **Local ledger predicates** [AUD.15]:
  - Ballblazer: all four POKEY channels non-silent within 30 s of title music.
  - Commando: TIA plus POKEY.
  - E.X.O. and A.R.T.I.: per-channel traces equal the Verilated MiSTer POKEY on the same writes.
  - Bentley Bear: no in-band energy from the 32 kHz tone.
  - Difficulty-switch audio: Rikki & Vikki TIA level; music in Sentinel, Super Skateboardin' and Ikari Warriors.
  - YM detection count $88 (v4).
- **Hardware diagnostics** (own cc65, owner's console):
  - AUDC 0–15 at AUDF 31, recorded (settles 2/3/14/15).
  - Equal-volume TIA then POKEY tones (mix ratio).
  - CLK2 during polling (stretch).
  - Lock INPTCTRL with $07 written to AUDV0 ($19), then set AUDC0 = 4 and AUDF0 = 31. If the amplitude equals that of an explicit post-lock AUDV0 = 7, the lock write reached the TIA.
  - Cold boot with a cartridge that never writes SKCTL.

## 10. Open questions and risks
- **Open:**
  - TIA:POKEY level (1.0× MiSTer vs 2.1× derived).
  - EAUD and node load, and with them the high-pass corners.
  - AUDC 2/3/14/15 bit patterns (die model vs Fries).
  - POKEY power-on state.
  - STIMER polarity (Altirra vs CO12294).
  - XM YM2151 clock and its level against POKEY.
  - BIOS residue in TIA audio (Ninja Golf).
  - Any POKEY-IRQ titles.
  - Which homebrew uses dual POKEY ($440 + $450).
  - TIA power-on line phase (shared with TIA WSYNC timing).
- **Risks:**
  - Dense 1.79 MHz POKEY cost on low-power targets (SYN-2).
  - Mixer retuning churns PCM digests; the primary transition hash is unaffected.
  - Licence: TIA.sv, top.sv and cart.sv are facts only; the BupChip firmware is unknown; Trebor's recordings and Synthpopalooza's demos carry no licence (oracle use only).
  - The 2026 MiSTer POKEY is new and partly unvalidated (its STIMER comment admits an open point).

## Sources
| ID | Tier | Source (URL) | Pin or access date | Supports |
|---|---|---|---|---|
| AUD.01 | T3 | github.com/MiSTer-devel/Atari7800_MiSTer `rtl/TIA.sv` (facts only) | 0dc8ad2e3ff7 | TIA clocks, H-counter decodes, audio channel logic (simulated, tested) |
| AUD.02 | T3 | same repo `rtl/top.sv` (facts only) | 0dc8ad2e3ff7 | TIA mix table (n/(n+30) fit), source mixing |
| AUD.03 | T3 | same repo `rtl/cart.sv` (facts only) | 0dc8ad2e3ff7 | POKEY/YM/COVOX/XCTRL decode, POKEY and YM clock enables, stereo dual POKEY |
| AUD.04 | T3 | same repo `rtl/Pokey/*.sv` (MIT, author only; from the re-drawn POKEY schematic) | 0dc8ad2e3ff7 | dividers, polys, init, STIMER, reload delay, two-tone delay, HP phase, DAC, power-on out of init |
| AUD.05 | T3 | same repo `rtl/bupchip_*.sv` (MIT, 2026) | 0dc8ad2e3ff7 | BupChip command and PCM interface |
| AUD.06 | T1 | Altirra Hardware Reference Manual, ch. 5 POKEY and App. E — virtualdub.org/downloads | 2026-01-02 edition (PDF sha256 1909927618a7…) | POKEY timing, init, polys, RANDOM, two-tone, HP, DAC and saturation |
| AUD.07 | T3 | github.com/mamedev/mame `src/devices/sound/pokey.cpp` v4.9 (BSD-3) | dddf4cd1faee | MAME POKEY behaviour and deviations; attribution |
| AUD.08 | T3 | MAME `src/mame/atari/a7800.cpp` (BSD-3) | dddf4cd1faee | crystals, TIA at crystal/456, slot clock crystal/8, no TIA gate |
| AUD.09 | T3 | MAME `src/devices/bus/a7800/{rom,xboard}.cpp` (BSD-3) | dddf4cd1faee | $4000 POKEY mirror, XM map, YM at 2× slot clock, $84 enable |
| AUD.10 | T3 | MAME `hash/a7800.xml` (CC0) | dddf4cd1faee | POKEY and XM titles |
| AUD.11 | T3 | github.com/7800-devtools/a7800 `src/mame/drivers/a7800.cpp`, `src/devices/sound/pokey.cpp` (BSD-3) | 7a2afdc1ea08 (2024-11-17) | `(ctrl&3)==3` TIA gate; POKEY 4.7–4.9 |
| AUD.12 | T3 | github.com/TASEmulators/BizHawk A7800Hawk `A7800Hawk.IEmulator.cs`, `Pokey.cs` (MIT) | c9de9a8bff0f | TIA at counts 113/340 of 454; POKEY every 4 ticks |
| AUD.13 | T1 | Atari schematic C025231-001 "7800" NTSC, archive.org/details/7800_NTSC_Schematic_Diagram_1984_U | drawn 1984-03-23; accessed 2026-10-15 | audio network, cartridge pins 18/31/32, TIA chip selects |
| AUD.14 | T2 | 7800.8bitdev.org A78 Header Specification | rev 1120 | audio/interrupt bits, POKEY codes |
| AUD.15 | T2 | 7800.8bitdev.org Common Emulator Development Issues | rev 1304 | Ballblazer, E.X.O., A.R.T.I., Commando, difficulty-switch audio |
| AUD.16 | T2 | 7800.8bitdev.org Atari 2600 VCS Sound Frequency and Waveform Guide (Stolberg 1997) | rev 50 | AUDC periods and waveforms (Fries model) |
| AUD.17 | T2 | 7800.8bitdev.org YM2151 Programming; YM2151 Detection | rev 734; rev 869 | $460/$461/$470, busy flag, detection count |
| AUD.18 | T2 | 7800.8bitdev.org Converting Rikki & Vikki (Steam) to an A78 with Music | rev 1350 | CoreTone, js7800 only |
| AUD.19 | T2 | 7800.8bitdev.org The 7800 Minnie sound chip | rev 1300 | Minnie |
| AUD.20 | T2 | 7800.8bitdev.org Atari 7800 Hardware Interfacing Guide | rev 1215 | Audio In, 1.19 MHz slow access |
| AUD.21 | T3 | github.com/aaronsgiles/ymfm (BSD-3) | 81aec25ccbb9 | OPM sources, sizes, integer-only |
| AUD.22 | T1 | github.com/7800-devtools/7800basic `samples/` | e7c18d2ecdc6 | audio samples and their settings |
| AUD.23 | T1 | nesturbator `src/synth.c`; `.planning/preparation/NES-HARDWARE-CPU-APU.md` §5 (owner's sibling, MIT) | 34d68516a15e | kernel, interpolation, measured quality |
| AUD.24 | T4 | gabbaboy lesson (owner's sibling): PCM digests frozen with 3.04 % clipping | accessed 2026-10-15 | clipping check |
| AUD.25 | T4, measured | forums.atariage.com/topic/316812 (6.8 kΩ / 18 kΩ mix) | accessed 2026-10-15 | mix resistors, balance test |
| AUD.26 | T4, measured | AtariAge topic 86087 (POKEY pitch drops with slow cycles) | accessed 2026-10-15 | φ2 stretch |
| AUD.27 | T4, measured | AtariAge topic 145363 (RIOT 113/line with DMA; SKCTL/RANDOM) | accessed 2026-10-15 | φ2 during HALT |
| AUD.28 | T4 | AtariAge topic 268458 (a7800 changelogs; POKEY init; XM/YM titles) | accessed 2026-10-15 | power-on, titles |
| AUD.29 | T4, measured | AtariAge topic 293427 (hardware recordings; two-tone; Dragonfly, HOKEY) | accessed 2026-10-15 | dual POKEY, oracle recordings |
| AUD.30 | T4 | AtariAge topic 318275 (MiSTer 7800 core: TIA non-linearity, A/B listening, 6 kHz low-pass idea) | accessed 2026-10-15 | TIA mix, filter question |
| AUD.31 | T4, measured | AtariAge topic 318358 (TIA vs MARIA lines; INPTCTRL decode) | accessed 2026-10-15 | TIA own line, no gate |
| AUD.32 | T4 | AtariAge topic 383566 (test7800: RANDOM, two-tone, aliasing) | accessed 2026-10-15 | RANDOM, E.X.O. |
| AUD.33 | T4 | AtariAge topic 285921 (BupSystem: aliasing, Bentley Bear, DK XM, BIOS residue) | accessed 2026-10-15 | ultrasonic tones, SKCTL |
| AUD.34 | T4 | AtariAge topic 271920 "TIA sound abnormalities" | (unverified: HTTP 403) | origin of the TIA mix data |
| AUD.35 | T1 | 7800xm.com XM developer documents | (unverified: DNS failure; archive 429) | XM register map |
