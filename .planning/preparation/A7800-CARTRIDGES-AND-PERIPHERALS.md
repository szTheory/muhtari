---
title: "Atari 7800 cartridges, boards, controllers, saves and hostile inputs"
summary: "A78 header v1–v4, every board scheme with library share, pin-level controller map, HSC/SaveKey saves, commercial library, ROM-hack support and parser bounds for muhtari."
read_when: "Before designing or changing cart.c/board_*.c, the A78/headerless probe, ports.c, saves (HSC, SaveKey, .srm), patch handling, or the loader fuzz targets."
updated: 2026-10-26
---
# Atari 7800 cartridges and peripherals (research R4)

| Question | Section |
|---|---|
| What is at each A78 header offset, and does v4 or v3 win? | 1.1–1.3 |
| How do I identify a headerless dump, a bad header, or a 2600 image? | 1.4–1.6 |
| What hotspots, banks, RAM and POKEY addresses does each board use? | 2.1 |
| How much of the library does each board cover? Which goes in milestone v1 and which in v2? | 2.2–2.3 |
| What do flash carts do that homebrew relies on? | 2.4 |
| How does each controller map to RIOT/TIA pins, and what is the two-button hazard? | 3.1–3.3 |
| What do light-gun games check? What does a keypad need? | 3.4–3.5 |
| HSC and SaveKey: memory map, protocol, ROM status, HLE, file formats | 4 |
| What is in the commercial library, and what are its special cases? | 5 |
| IPS/BPS, hack identity, the BIOS signature | 6 |
| Which parsers handle hostile input, and what bounds do they need? | 7 |

## 1. The A78 header
The header, the probe and the hash lists graduated in phase 2 to `docs/hardware/cartridge.md`, which cites this
section's sources [CART.01][CART.02][CART.20]. The subsections keep their numbers because that page cites them; each
keeps its choice line and the evidence the page does not repeat.

### 1.1 Fields [CART.01][CART.02] (multi-byte fields are big-endian [CART.20])
The field table, version handling and the v4 block are in `docs/hardware/cartridge.md`, The A78 header and The v4
block. Field history: v1 is Dan Boris's A7800 emulator, v2 added the save byte, v3 came in 2021, v4 in January 2024
[CART.02]; MAME and rcheevos compare 9 magic bytes, MiSTer just `ATARI` [CART.20][CART.29][CART.24]; MiSTer also uses
mapper 5 = ACE/SN Cart, which is not in the spec [CART.24]. The spec gives the v4 option names no semantics. Reading
(derived, unverified): A8 = bit 7 Fractalus mirror RAM (A7800DS calls Fractalus "EXRAM/A8" [CART.30]); M2 = bit 14
halt-banked RAM; X2 = bit 5 banked RAM (A7800DS has "SUPERCART_RAMX2"); EXROM = bit 3; EXFIX = bit 4.

### 1.2 Cart-type word (bytes 53–54) [CART.01]
The bit table is in `docs/hardware/cartridge.md`, Cart type. MAME's comments swap Activision and Absolute, but its
loader code follows the spec (0x100 = Activision) [CART.20].

### 1.3 Parsing policy and who reads what
**Readers:** MAME upstream uses bits 0–9 only [CART.20]; a7800 v5.2 bits 0–9 and 13–15 plus byte 63, no v4
[CART.22]; MiSTer bits 0–15, byte 64 only for ACE, and the HSC when byte 58 ≠ 0 [CART.24]; A7800DS reads v4 and falls
back to v3 [CART.30]; BizHawk checks its MD5 database first [CART.27]. The conflict order built is MAME's
`validate_header` [CART.20][CART.22], listed in `docs/hardware/cartridge.md`, Cart type.

**Recommendation** (built): parse v3 always; a version ≥ 4 block that validates and fits the body overrides the v3
board, RAM and sound, else `HEADER_V4_IGNORED`; POKEY@4000 beside ROM@4000 stays write-only, as in the bankset wiki
and MiSTer's `pokey4k_wo` [CART.04][CART.24]. **Strongest alternative:** honour v4 blindly, as the spec says
[CART.01]. **Would change if:** a scan of PROPack headers finds v4 images whose v3 bits are deliberately stale.

### 1.4 Headerless images and hash identity
The lists, the slice and half-swap passes and the override rules are in `docs/hardware/cartridge.md`, Hash list.
Evidence: the CC0 `hash/a7800.xml` has 204 entries with per-chip CRC32 and SHA-1 [CART.18]; libretro's 7800 DAT
(2026.10.07) lists 227 headerless `.bin` and 110 headered `.a78` entries, so both forms must be identified [CART.40];
91 of 114 PROPack v8.17 CRC32s match a softlist CRC directly (tested) [CART.15]. The misses: 9 two-chip entries
(hash each slice); 11 48K games stored on a 64K chip (Desert Falcon, Donkey Kong, DK Jr, Hat Trick, NTSC Mario Bros,
PAL Xevious: the 48K hash needs local dumps); 3 Activision layouts (Double Dragon NTSC and PAL, Rampage), whose
PROPack "(AM)" images swap the 8K halves of every 16K bank against MAME's "(OM)" dumps, which the header cannot tell
apart [CART.03][CART.15][CART.20][CART.22][CART.24][CART.27]. PROPack's Alien Brigade and Crossbow each carry a
Joystick and a Lightgun header over one CRC [CART.15].

**Recommendation** (built): a sorted per-chip SHA-1 table generated from the CC0 XML at build time, with each
multi-chip entry's slice layout; slice and half-swapped hashes computed at run time, since SHA-1 does not compose
(tested while building the probe). A match overrides board, RAM and POKEY (`HEADER_CORRECTED`), never controllers;
region from the header, else the hash, else NTSC. Activision loading now normalizes only an identified
half-swapped match into a private OM copy, retaining raw caller/runner identity. Unknown explicit Activision
assumes OM with a warning; unmatched modified AM needs external conversion. The implemented contract and
source confidence are in `docs/hardware/cartridge.md`, Activision. **Strongest alternative:** trust the header and use the hash only when there is none (MiSTer, a7800).
**Would change if:** the owner's dumps show softlist slot errors.

### 1.5 2600 images → `MUHTARI_STATUS_UNSUPPORTED_2600` (2600 support is planned)
The probe order, the footer test and the 2600 sizes are in `docs/hardware/cartridge.md`, Probe order and The footer
test. Evidence: `hash/a2600.xml` (CC0) has 1592 entries: 4K 935, 8K 373, 2K 133, 16K 121, 32K 16, 12K 6, 64K 5,
10495 B 3 (tested) [CART.19], so 8K to 64K overlap 7800 sizes; the NTSC BIOS footer checks are [CART.16], and the PAL
BIOS checks only the footer and looks for 2600-style mirrors [CART.16].

**Recommendation** (built): magic, then the 7800 list, the 2600 list, the footer test, the 2600 sizes, else
`INVALID_IMAGE`. **Strongest alternative:** size-only rules, such as BizHawk's `len % 1024 == 128` header test
[CART.27]. **Would change if:** the footer test misclassifies anything in the owner's library or the 7800basic sample
builds.

### 1.6 Bad headers in the wild
Commando without POKEY music has a bad header [CART.12]. ProSystem ignored headers and keyed games by MD5, so old
dumps carry stale headers [CART.34]. The hash table covers only the CC0-listed retail set; homebrew and hacks get
header lint (§6).

## 2. Boards
### 2.1 Every scheme (n = softlist entries, of which commercial) [CART.03][CART.18]
| Board (bits / slot) | Map and hotspot | Power-on / fixed | RAM, sound, titles | n |
|---|---|---|---|---|
| Linear 8–48K (`a78_rom`) | ROM ends at $FFFF; images over 48K use the top 48K [CART.20] | — | — | 119 (46) |
| Linear + POKEY (b0) | POKEY's 16 regs mirrored over $4000–$7FFF, read/write | — | Ballblazer | 5 (2) |
| SuperGame (b1) | any write to $8000–$BFFF selects the bank (value & (banks−1)); Planet Smashers uses $BFFF [CART.12] | bank 0 (Legend of Silverpeak needs it); last bank at $C000; second-to-last at $4000. MAME, MiSTer and A7800Hawk agree; Stolberg says some boards float [CART.20][CART.24][CART.27] | a 64K image is 4 banks, mirrored (RealSports Baseball, Tank Command, Water Ski; PAL Karateka uses banks 4–7) | 44 (41) |
| SG + RAM (b2) | 16K RAM at $4000–$7FFF. The 8K-RAM games (Impossible Mission, Jinks, Tower Toppler) run with 16K [CART.20] | — | Summer/Winter Games, Plutos and Sirius use 16K | 12 (10) |
| SG9 / EXROM (b3) | 144K = 9 banks: bank 0 fixed at $4000; $8000 = 1 + (v&7); $C000 = bank 8. Generalises to 2ⁿ+1 banks (272K, 528K) [CART.20][CART.24] | — | Alien Brigade, Crossbow, MIA | 12 (4) |
| SG variants | b4: bank 6 fixed at $4000. b0+b1: SG + POKEY@4000 (Commando, 2 (2)). SG9 + POKEY@450 (`a78_p450_ta`: Bentley Bear, 1 (0)) | — | — | 3 |
| Banked RAM: CPUWIZ VersaBoard / MegaCart+ (b5) | VersaBoard ≤256K, bank = D3–D0; MegaCart+ ≤512K, bank = D4–D0. 2×16K RAM at $4000, RAM bank = D5 of the same write [CART.20]. MiSTer generalises to D7–D5 [CART.24] | bank 0, RAM bank 0 | — | 0 |
| Activision (b8) | 128K; $A000–$DFFF switchable. A write to $FF80–$FF8F sets bank = A2–A0 (a7800/MiSTer); BizHawk decodes $FF80–$FF87, MAME any write to $E000–$FFFF. Double Dragon writes $FF88+ [CART.22][CART.24][CART.27] | bank 0; bank 6 at $4000–$7FFF; bank 7 at $8000–$9FFF and $E000–$FFFF (half order per 1.4) | Double Dragon, Rampage | 3 (3) |
| Absolute (b9) | 64K; $4000–$7FFF = bank 0/1; $8000–$FFFF = image 32–64K. MAME: a write to $8000 with D0 → bank 0, D1 → bank 1. MiSTer: any write with A15 set, bank = D1 [CART.20][CART.24] | bank 0 | F-18 Hornet | 2 (2) |
| Fractalus mirror RAM (b7, "EXRAM/A8") | 32K ROM at $8000; 2K RAM at $4000 with A8 ignored. MiSTer also ignores A12–A13, so the 2K repeats over $4000–$7FFF [CART.03][CART.24] | — | prototype | 1 (0) |
| BankSet (b13; b14 RAM) | Graduated to [cartridge hardware](../../docs/hardware/cartridge.md#bankset): exact six-size/fourteen-geometry matrix, electrical ownership, redirects and overlap rules [CART.04][CART.24][CART.30]. Larger sizes and shared RAM are producer/emulator extensions; slow-edge causal timing is derived, not measured | bank 0 | physical spans and POKEY overlap documented there | 0 |
| Souper (b12 / v4 4) | 512K ROM + 32K RAM. Regs $8000–$8007 repeat every 8 B to $FFFF: 0 bank D4–D0; 1/2 char A/B; 3 mode (S/C/E); 4/5 EXRAM 4K pages at $6000/$7000; 7 BupChip command. In S mode MARIA fetches remap: $8000 → fixed bank, $A000 → 2K char windows, $C000 → EXRAM [CART.25] | RAM at $4000, bank 0, last bank at $C000 | music needs the BupChip ARM soft-synth; the 1 MB A78 is 512K ROM + 512K music [CART.39] | 0 |
| XBoard (internal) | Graduated to [XBoard](../../docs/hardware/cartridge.md#xboard): eight 16 KiB RAM banks, sixteen control aliases and continuously installed gated $0450 POKEY [CART.20]. Optional $0460 chip remains unverified and unsupported | all off | — | 1 |
| XM (pass-through) | Graduated to [XM subset](../../docs/hardware/cartridge.md#xm-rampokeyhsc-subset): distinct exact-address two-window CPU/MARIA pages, gated HSC and POKEY [CART.22]. Medium-confidence implementation-backed profile; primary specification inaccessible. Older shared XBoard/XM decoder [CART.20] is not this map | all 0 | No YM2151, service/flash/lock/PIA/internal firmware or full-XM acceptance | 1 |
| POKEY windows | @450 = $0450–$045F; @440 = $0440–$044F; @800 = $0800–$0FFF, 16-byte mirror [CART.22][CART.24] | stays silent (in reset) until software writes SKCTL. Flash carts release it at boot, and demos relied on that [CART.34] | — | — |
| Other | ACE/SN Cart: MiSTer only; 4K windows + SN76489 at $043F. Covox at $0430–$0433, always on in MiSTer. Minnie (GCC 1730) was never released. "Matchbox": no source found | | | |

### 2.2 Library share (tested: [CART.18] parsed; PROPack central directory and text lists only, no ROM data fetched [CART.15])
| Softlist class | Linear | SG | SG+RAM | SG9 | POKEY lin / SG / p450 | Act | Abs | Other |
|---|---|---|---|---|---|---|---|---|
| All 204 | 119 | 44 | 12 | 12 | 5 / 2 / 1 | 3 | 2 | mram 1, HSC 1, XBoard 1, XM 1 |
| Commercial 110 (61 NTSC, 49 PAL) | 46 | 41 | 10 | 4 | 2 / 2 / 0 | 3 | 2 | — |
| Prototype 18 (+2 diagnostic) | 11 (+2) | 3 | 2 | 1 | — | — | — | mram 1 |
| Homebrew 70 | 60 | 0 | 0 | 7 | 2 (+ devcard) / 0 / 1 | — | — | — |

Linear + SG covers 187/204 (91.7%); adding POKEY gives 195/204. For commercial titles alone: 101/110, then 105 with POKEY, and 110 with Activision and Absolute.

**PROPack v8.17 (2026-01-01), 1309 headered images:** retail NTSC 63, PAL 52; prototypes 68; homebrews 258; hacks 342; demos 454; utilities 72. Homebrew sizes: ≤48K 175, 96K 1, 128/144K 63, 256–528K 19. Filename tags in the 1194 non-retail images give lower bounds: POKEY@4000 ≥103, @450 ≥77, YM2151 ≥35, dual POKEY ≥13, bankset ≥10, Covox ≥8, @800 ≥6, XM-required ≥4, Souper ≥1; 7 images are 1 MB.

The softlist has no bankset, Souper, XM-required, YM2151, @800 or @440 titles. These schemes are homebrew-heavy: PETSCII Robots is the first bankset game, and Rikki & Vikki is Souper [CART.04][CART.39].

### 2.3 Milestone placement
**Recommendation:** **v1** = linear; the SG family (SG, SG+RAM, SG9/EXROM, bank 6, banked RAM, up to 1 MB); POKEY @4000/@450/@440/@800 (phase 4); ProLine and 2600 sticks. **v2** = Activision (with AM/OM normalisation), Absolute, Fractalus, bankset (needs the exact /HALT phase), HSC, SaveKey, light gun, paddles, keypad, driving, trak-ball, mice, SNES2Atari, Mega7800, XBoard/XM (POKEY + RAM + HSC). **v4** = YM2151, Souper/BupChip, Covox/ADPCM, ACE. The implemented boards retain one switch and an instance-owned latch, shared by CPU reads, peeks and MARIA. The earlier ops-table suggestion is superseded by that implementation; future bankset/Souper/XM accesses must retain their caller identity without a global DMA flag [CART.22; `docs/hardware/cartridge.md`]. **Strongest alternative:** move Activision/Absolute into v1 (5 commercial entries, about 60 lines). **Would change if:** phase 3 has slack, or the owner's must-play list includes Double Dragon, Rampage or F-18.

### 2.4 Flash-cart behaviour that homebrew relies on
**Concerto:** ≤512K. Concerto+ adds 256K, 256K + 16K RAM and 272K images with HOKEY@450; HOKEY lacks pots, keyboard and serial. The CEM module adds HSC emulation and a warm-reboot button [CART.13]. **DragonFly:** YM2151 + Covox; mixes two POKEYs onto the one cart audio line [CART.17][CART.34]. **7800GD:** bankset, dual POKEY, YM2151, Covox, BupChip; reportedly maps empty bank 62 as RAM on 1 MB SG+RAM images (T4 snippet, unverified) [CART.35]. **2600+ cart reader:** does not wire /HALT, so it cannot dump bankset carts [CART.34].

**Recommendation:** emulate stock hardware (POKEY silent until SKCTL, no flash-cart extras), with a per-hash compatibility list for demos that relied on flash carts. **Strongest alternative:** a "flash cart" profile option. **Would change if:** homebrew that is silent on stock hardware shows up outside PROPack (RevEng patched the PROPack copies [CART.34]).

## 3. Controllers
### 3.1 Pins → registers (port 0 = left; port 1 in brackets) [CART.08][CART.09][CART.23]
| Pin | Signal | Register | Notes |
|---|---|---|---|
| 1–4 | up, down, left, right | SWCHA b4–b7 [b0–b3] | 0 = closed; RIOT DDR at $281 sets direction |
| 5 | pot (a7800 numbering; not checked against the schematic) | INPT0 [INPT2] | TIA pot input; VBLANK D7 = 1 dumps it to ground |
| 9 | pot | INPT1 [INPT3] | same |
| 6 | trigger | INPT4 [INPT5] | 0 = pressed; VBLANK D6 latches |
| — | two-button enable | SWCHB PB2 [PB4] | driven 0 = two-button mode, which puts +5 V on pin 6 [CART.11]. INPTCTRL TIA-EN (bit 3) disables it [CART.09] |

### 3.2 Devices [CART.09][CART.23][CART.28][CART.11]
| Device (header code) | Pin model | Use |
|---|---|---|
| ProLine (1) | Two-button mode: right → INPT0/2, left → INPT1/3, 1 = pressed; INPT4/5 read 1. One-button mode: either button → INPT4/5 = 0 | MAME, a7800 and MiSTer keep INPT0–3 live in both modes; real one-button readings are unverified |
| 2600 stick (5) | fire pulls pin 6 low | In two-button mode this shorts the driven pin; INPT4 still reads 0. The Software Guide says it "may be harmful"; games should revert to one-button mode when INPT4/5 shows a press [CART.08][CART.09] |
| XG-1 light gun (2) | Trigger switch on pin 1 is closed at rest: SWCHA b4 = 0 idle, 1 when pulled (an 8-bit Atari reads 14 → 15). Light sensor on pin 6 pulls INPT4 low while lit [CART.32][CART.23][CART.27] | needs one-button mode |
| Paddles (3) | pots on INPT0/1 [2/3]; buttons on SWCHA b7/b6 [b3/b2] | homebrew (Crazy Brix, Super Circus); 2600 mode |
| Trak-ball CX22 (4) | pins 1/3 = X/Y direction; pins 2/4 toggle once per step; fire on pin 6 | homebrew |
| Driving (6) | 2-bit Gray code on pins 1–2 (00→10→11→01); fire on pin 6 | homebrew |
| Keypad (7) | rows = SWCHA outputs, active low; columns = INPT0, INPT1, INPT4 [2, 3, 5]; pressed = low | §3.5 |
| ST / Amiga mouse (8/9) | quadrature on ST X pins 1–2 / Y pins 3–4, Amiga X pins 2/4 / Y pins 1/3; left button = pin 6, right = pin 9 | homebrew |
| SaveKey / AtariVox (10) | §4.2. AtariVox adds pin 1 = SpeakJet DATA out (19200 8N1 inverted = 93.2 CPU cycles per bit, derived) and pin 2 = READY in [CART.10] | — |
| SNES2Atari (11, CC0) | either port, one-button mode: pin 1 CLOCK, pin 2 LATCH, pin 6 DATA through 1 kΩ on INPT4/5; 16+1 bits; a full read takes 533 cycles ≈ 447 µs at 1.19 MHz, the source's figure (≈ 298 µs at 1.79 MHz, derived) [CART.11] | homebrew |
| Mega7800 (12) | pins 1–2 driven as select/strobe; pins 3–4 return 2 bits per step × 6 steps (ID, 3-button, 6-button) [CART.28] | homebrew |

**Recommendation:** model ports at pin level. At the access tick each device sees RIOT PA output/DDR, the effective PB2/PB4 levels and the VBLANK dump/latch state, and returns pin levels; TIA and RIOT own the latching. Devices come from header bytes 55/56, and a host override wins. **Strongest alternative:** register-level shortcuts per device, as MAME and a7800 do. **Would change if:** profiling shows port reads above 1% of frame time.

### 3.3 Two-button mode
One On One and Worm! set the DDR without ever writing SWCHB and rely on the default RIOT state [CART.12]. On hardware ORB powers on high ($FF), so CTLSWB = $14 alone leaves the sticks in one-button mode and One On One reads its button on INPT4; an ORB = 0 default broke it in MAME until 0.168 (T4) [CART.41].

**Recommendation:** two-button mode = PB bit is an output (DDR = 1), its ORB = 0, and TIA-EN = 0. When a 2600 stick's fire is pressed in that mode, show it on INPT4 and count it in an `io_hazard` telemetry counter. **Strongest alternative:** a7800's rule, which latches the mode only on SWCHB writes while the DDR bit is set [CART.23]. **Would change if:** One On One or Worm! fails under the pin rule.

### 3.4 Light gun (Alien Brigade, Barnyard Blaster, Crossbow, Meltdown, Sentinel × NTSC/PAL = the 10 softlist entries; MAME: 4 partial, 6 not working) [CART.18]
**What the games check:** they switch DMA off (the screen becomes BACKGRND), then poll INPT4 in a tight loop. Crossbow manages about 16 polls per line, roughly 20-pixel X resolution. Each game hard-codes its own latency offsets. *Ducks Away* (homebrew) instead keeps DMA on and flashes one target at a time [CART.34].

**What emulators assume:** a7800 lights the sensor within 8 px of the beam, with an X offset of 95 units (~56 ticks) [CART.23]. A7800Hawk drops the sensor 64 ticks after the beam passes, for 30 ticks per line over 9 lines [CART.27]. Neither reads pixel brightness, and no source gives a luminance threshold. MiSTer's `lightgun.sv` has an unknown licence and was not opened [CART.40].

The implemented native-emission model has graduated into
[controller hardware](../../docs/hardware/controllers.md#xg-1): D64/W30/L9/T8,
clipped aim aperture and historical delayed pulses. These are named unmeasured
compatibility policies. Independent owned numeric diagnostics establish model
conformance; the earlier zero-offset retail calibration suggestion is superseded.
Physical calibration and broader retail accuracy require later measurements.

### 3.5 Keypad
The implemented source-backed wiring and explicit compatibility policies have
graduated into [controller hardware](../../docs/hardware/controllers.md#keypad).
The Software Guide's400µs settling wait [CART.09] is2864 elapsed master ticks,
not a fixed count of executed CPU cycles. Original field-manual figure7-9
has no isolation diodes; held switches form a connected network. Low-dominant
contention, immediate discharge and per-column timed recovery are derived
policies, not measured analog charge sharing. Reports of three-key behavior
[CART.34] do not establish an arbitrary software key cap. All twelve keys are
accepted; same-column retained charge and fourth-corner ghosts are tested.

## 4. Saves
### 4.1 High Score Cartridge [CART.06]
| Item | Fact |
|---|---|
| Map | SRAM 2K at $1000–$17FF, ROM 4K at $3000–$3FFF; a pass-through cart with no reset vectors |
| Detection | $3900 = $C6 and $3904 = $FE. Old 7800basic checked `$1002 = $68`. Without an HSC, open bus returns $39 (derived) |
| API | 7 JMP vectors at $3FEB–$3FFD: HSCSETRS, CPYFM, CPYTO, ENTER2, STAT, ATRCT, ENTER. Each call needs a 512-byte work area (pages N and N+1, Y = page N) and uses ZP $80–$BC |
| NVRAM | magic $68 $83 $AA $55 $9C at $1002–$1006; owner name at $1008; 69-slot directory (ID1, ID2, difficulty, age; $7F = free); 25-byte tables at $113D + 25·s; scores are 24-bit little-endian binary. The wiki lists 15 ROM bugs, e.g. a lost BCD carry (655,360 → 0) |
| 7800basic | never calls the vectors; it formats and writes the NVRAM itself and needs only the signature bytes [CART.28] |
| ROM | Atari's copyrighted `highscre.bin`, 4096 B: user-supplied only. Identify by MD5 c8a73288ab97226c52602204ab894286 or SHA-1 a3af676991391a6dd716c79022d4947206b78164 (no SHA-256 is published) [CART.06][CART.18][CART.40]. The wiki disassembly has no licence [CART.07]. MiSTer's `rtl/mem4.hex` is the same ROM: never a source |
| XM | same ROM (same SHA-1), enabled by XCTRL1 b3. Has a PAL-HSC register; the community PAL hack only lengthens gap zones [CART.06][CART.22] |

Implemented HSC mapping, supplied firmware validation/identity, signature-only stub and
generation/reset semantics have graduated into [cartridge hardware](../../docs/hardware/cartridge.md#hsc)
and [embedding](../../docs/embedding.md#battery-storage-and-hsc-firmware). The canonical SHA-1
identifies firmware; explicitly supplied unknown bytes are accepted with warnings, not
rejected by an allowlist. The shipped FF/signature stub implements no routines, screens,
formatting or full OpenHSC. A future clean-room OpenHSC remains later scope; the unlicensed
wiki disassembly and copyrighted hex ROM remain excluded.

### 4.2 SaveKey / AtariVox (24LC256, 32 KB) [CART.10][CART.28]
Implemented wiring, ORA/DDRA data-tick observation, ACK/read turnaround, page/read wrap,
bounded packet recovery and completion/reset semantics have graduated into
[controller hardware](../../docs/hardware/controllers.md#serial-eeprom). The manufacturer
protocol facts are verified against DS20001203Y sections 4–8 [CART.38]; its 5 ms maximum
is implemented as a selected fixed 35796-tick NTSC compatibility interval, not a hardware
measurement. One EEPROM is frozen on either port at load; AtariVox speech is not implemented.

**Layout:** page 0 holds `ATARIVOX` plus a TV byte. AtariAge allocates $0000–$3FFF; $4000–$7FFF is the file area, and 7800basic keeps its tables at $6000+ in 32-byte records. **Licence:** MiSTer's `EEPROM_24LC256.sv` is GPL-3 (from NES_MiSTer) and is excluded [CART.40].

### 4.3 File formats and interop
| Data | Elsewhere | muhtari |
|---|---|---|
| HSC SRAM | raw 2048 B: MAME/a7800 NVRAM ($FF when new), BizHawk SaveRAM, A7800DS per-game `.hsc` (formerly a shared `A7800.SRAM`) [CART.20][CART.27][CART.30] | raw 2048 B, same bytes |
| SaveKey EEPROM | raw 32 KB: Stella `nvram/savekey_eeprom.dat` and `atarivox_eeprom.dat` [CART.36]; MiSTer keeps one backup RAM for HSC or SaveKey [CART.24] | raw 32768 B, erased to $FF |
| libretro `.srm` | the ProSystem core has no saves [CART.31], so there is no prior layout | see below |

No retail cart has battery-backed RAM; the exceptions are Harry Dodgson's Monitor cart (8K) [CART.03] and flash-cart features (Concerto "save game" is not working yet [CART.13]).

The implemented core span is layout 1: HSC 2048 B, EEPROM 32768 B, or HSC ‖ EEPROM
34816 B. Hosts restore a clean baseline before the first frame and own persistence;
power cycle retains committed bytes and generation. Host integrations use per-game raw
files, one writer per path, with separate directories for basename collisions. No optional
shared-device file is planned. Frontend/runner acceptance remains with their phase plans.

## 5. Commercial library
There are 58 retail titles: 57 NTSC (59 PROPack files, counting the Joystick/Lightgun pairs) and 49 PAL [CART.15][CART.18].

| Category | Titles |
|---|---|
| POKEY | Ballblazer (32K, @4000), Commando (128K SG, @4000) |
| Activision board | Double Dragon (NTSC/PAL), Rampage (NTSC) |
| Absolute board | F-18 Hornet (NTSC/PAL). Absolute's other titles (Kung-Fu Master, Pete Rose Baseball, Super Skateboardin', Title Match, Tomcat) use standard boards |
| Light gun | 5 titles × 2 regions (§3.4) |
| PAL-only | Sentinel (MAME also has an NTSC dump, which PROPack omits) |
| NTSC-only | Asteroids (PAL consoles have it in the BIOS), Rampage, RealSports Baseball, Robotron, Summer Games, Tank Command, Touchdown Football, Water Ski, Winter Games |
| Size/board differs by region | Choplifter, Dig Dug, Food Fight, Galaga, Joust, Karateka (NTSC 48K linear; PAL 64K SG with banks 4–7 and bank 6 at $4000), Mario Bros, Ms. Pac-Man, Xevious [CART.03][CART.18] |

**Titles known to need special timing or wiring [CART.12][CART.34]:** DMA timing: Choplifter, Xenophobe, Summer Games, Ace of Aces, Karateka, Alien Brigade. DMA start point: Kung-Fu Master, MIA. Floating bus: Choplifter. RIOT power-on defaults: One On One, Worm!. First frame and a double SWCHA read: Pole Position II. Startup race: Klax prototype (hold button 1 during the logo). DL terminator: Ninja Golf, Crossbow. $BFFF hotspot: Planet Smashers. Composite artifacts: Tower Toppler. Not bugs but difficulty-switch behaviour: Super Huey, Water Ski and others.

**Prototypes:** softlist 18 (+2 diagnostic) vs PROPack 68. **Homebrew:** softlist 70 vs PROPack's 258 homebrews, 342 hacks, 454 demos and 72 utilities.

## 6. ROM-hack friendliness
- **IPS:** `PATCH`, then records {u24 BE offset, u16 BE size, data}. Size 0 means RLE {u16 count, u8 value}. Ends with `EOF`. Addresses up to 16 MiB; offset $454F46 collides with the `EOF` marker [CART.33]. A u24 truncation size after `EOF` is an extension (unverified).
- **BPS:** `BPS1`, then varint source, target and metadata sizes. Actions are SourceRead, TargetRead, SourceCopy and TargetCopy, with length = (n>>2)+1 and signed relative copy offsets. Footer: three CRC32s (source, target, patch), little-endian [CART.33].
- **Header in offsets:** an `.a78` patch counts the 128-byte header in its offsets; a `.bin` patch does not. A BPS source CRC exposes the mismatch; IPS cannot.

**Recommendation:** patching belongs in the host. libretro sets `need_fullpath = false`, so RetroArch soft-patches `rom.ips`/`.bps`/`.ups` (and `.ips1`…) [CART.31]. `muhtari-run --patch` uses a separate, fuzzed `patch.c` (IPS + BPS); when a BPS source CRC fails, it retries with the header added or removed. Identity = MD5/SHA-1 of the patched image minus 128 bytes when bytes 1–9 read `ATARI7800`; that is the rcheevos rule, so RetroAchievements hack sets line up [CART.29]. Movies also pin the SHA-256 of the full image, because header bits change behaviour. `--probe` lints: unterminated title, non-zero reserved bytes, bit conflicts, size mismatch, a v4 header with no v3 bits, AM/OM. **Strongest alternative:** IPS/BPS in the core API. **Would change if:** Playstead cannot patch on its own.

**Header tools:** 7800header is interactive and ships with 7800basic and 7800AsmDevKit. `a78_asm_header` has no licence anywhere; I read its README but did not open its source, and it must never be vendored [CART.37].

**BIOS signature:** the NTSC BIOS checks a 120-byte signature at $FF80–$FFF7 that covers from the start page to $FF7F; a failure boots 2600 mode. The PAL BIOS does no crypto check [CART.09][CART.16]. So a hack runs on real NTSC hardware only after re-signing with 7800sign (Bruce Tomlin, LGPL-2, ships with 7800basic) [CART.17][CART.28]. HLE fast boot skips the check.

**Recommendation:** under `--boot bios`, return `BIOS_REJECTED_CART` when the BIOS takes the 2600 path, with the hint "unsigned or modified image: use fast boot or 7800sign". **Strongest alternative:** fall into 2600 mode silently. **Would change if:** 2600 mode lands and real 2600 images go through the BIOS.

## 7. Hostile-input list
Fuzz targets: `muhtari_probe`, board construction with random bus traffic, the four file loaders, the I²C state machine with random pin toggles, IPS/BPS.

| Parser | Size / count bounds | Offsets and invariants |
|---|---|---|
| A78 header | file ≥ 128 B; image ≤ 1 MiB + 128 (Souper with music, 1 MB demos), else `UNSUPPORTED_SIZE` | 9-byte magic; version ≤ 4 or warn; title ≤ 32 B, stop at NUL, map non-printables; length field: reject 0, use min(field, file−128); controller codes ≤ 12, else unknown → ProLine; v4 mapper ≤ 4 (5 = ACE later); options ≤ 3 (linear) / ≤ 6 (SG) |
| Board geometry | linear ≤ 48K (a bankset half may be 52K); SG a multiple of 16K, ≥ 2 banks, SG9 = 2ⁿ+1; Activision 128K; Absolute 64K; Fractalus 32K; Souper ROM ≤ 512K; bankset an even split into 32/48/52/128/256/512K halves | SG bank = value mod banks, never a raw index; ordinary power-of-two RAM uses index & (size−1). Fractalus instead compresses disconnected address bits: `((addr & 0x0E00) >> 1) \| (addr & 0x00FF)`, yielding exactly 2 KiB and eight aliases per byte over $4000–$7FFF. MAME's mram indexes a 2K softlist buffer with `offset & 0xfeff`, up to $3EFF: out of bounds (derived) [CART.20]. Implemented/refused geometries and selected edge policies: `docs/hardware/cartridge.md`. |
| Headerless / hash | ≥ 8 B for the footer test | slice hashes only when the size equals the sum of the chips; the hash table is built from CC0 XML and validated at build time |
| BIOS | exactly 4096 (NTSC) or 16384 (PAL, and the Asteroids prototype) | known hash → info; unknown → allowed with a warning; the reset vector must point inside the BIOS [CART.21] |
| HSC ROM | exactly 4096 | signature at +$900 and +$904 (warn if absent) |
| HSC SRAM / SaveKey file | 2048 / 32768; shorter → pad with $FF; longer → reject | contents are untrusted RAM: copy them, parse nothing |
| IPS | target ≤ cap (1 MiB + 128) | every record's offset+size ≤ cap; RLE count ≥ 1; `EOF` required |
| BPS | varints < 2⁶⁴ with no overflow; sizes ≤ cap | source size and CRC must match; every action stays inside source/target; output fully written; patch CRC checked |

## Sources
| ID | Tier | Source (URL) | Pin or access date | Supports |
|---|---|---|---|---|
| CART.01 | T2 | 7800.8bitdev.org/index.php?title=A78_Header_Specification | rev 1120 | header fields, v4 precedence |
| CART.02 | T2 | same page, older revisions | revs 112, 615, 858, 937, 1113 | version history |
| CART.03 | T2 | …/ATARI_7800_BANKSWITCHING_GUIDE (Stolberg v1.40) | rev 391 | boards, titles, Activision halves |
| CART.04 | T2 | …/Bankset_Bankswitching | rev 1016 | bankset |
| CART.06 | T2 | …/7800_HSC_Technical_Reference | rev 1337 | HSC API, layout, bugs |
| CART.07 | T2 | …/7800_HSC_CART (community disassembly, no licence; header read only) | rev 1333 | ROM provenance |
| CART.08 | T2 | …/Atari_7800_Hardware_Interfacing_Guide_For_Programmers | rev 1215 | SWCHA/SWCHB, two-button setup and hazard |
| CART.09 | T2 | …/7800_Software_Guide | rev 1320 | ports, keypad 400 µs, INPTCTRL, signature space |
| CART.10 | T2 | …/AtariVox_for_the_7800 | rev 134 | pins, I²C, EEPROM map |
| CART.11 | T2 | …/SNES2Atari (CC0 design) | rev 1002 | +5 V on pin 6, protocol |
| CART.12 | T2 | …/Common_Emulator_Development_Issues | rev 1304 | special-case titles |
| CART.13 | T2 | …/Concerto | rev 1307 | flash-cart behaviour |
| CART.14 | T2 | …/Detecting_POKEY, YM2151_Detection, XBoard_Memory_Init | revs 93, 869, 90 | detection |
| CART.15 | T2 | …/Trebors_7800_ROM_PROPack + v8_17 zip (central directory and 8 text lists only) | rev 1293; zip sha1 339c04768867 | counts, CRCs, AM/OM |
| CART.16 | T2 | …/7800_NTSC_BIOS, 7800_PAL_BIOS (comment headers only) | revs 1259, 1285 | footer checks |
| CART.17 | T2 | …/Emulators_and_Tools | rev 1065 | flash carts, 7800sign |
| CART.18 | T3 | github.com/mamedev/mame hash/a7800.xml (CC0) | dddf4cd1faee | softlist counts, HSC SHA-1 |
| CART.19 | T3 | mame hash/a2600.xml (CC0) | dddf4cd1faee | 2600 sizes |
| CART.20 | T3 | mame src/devices/bus/a7800/{a78_slot,rom,cpuwiz,xboard,hiscore}.cpp (BSD-3) | dddf4cd1faee | mappers, validate_header |
| CART.21 | T3 | mame src/mame/atari/a7800.cpp (BSD-3) | dddf4cd1faee | BIOS ROMs, TIA inputs |
| CART.22 | T3 | github.com/7800-devtools/a7800 bus/a7800/{a78_slot,rom,bankset,xm}.cpp. The repo is GPL-2.0+ overall; only files with BSD-3 headers were read | 7a2afdc1ea08 | AM map, bankset, XM |
| CART.23 | T3 | a7800 bus/a7800_ctrl/*.cpp (the fork's own; MAME has no such bus) and mame/drivers/a7800.cpp, all with BSD-3 headers | 7a2afdc1ea08 | pin maps, gun model |
| CART.24 | T3 | github.com/MiSTer-devel/Atari7800_MiSTer rtl/cart.sv, paddles.sv, Atari7800.sv. **Facts only**: cart.sv has third-party edits from the core's CC BY-NC period and is MIT only since 2026-08-30 | 0dc8ad2e3ff7 | mappers, header use |
| CART.25 | T3 | same repo, rtl/souper.v (zlib, Celimli 2015) | 0dc8ad2e3ff7 | Souper |
| CART.27 | T3 | github.com/TASEmulators/BizHawk A7800Hawk/*.cs (no TIA_Sound), Assets/gamedb/gamedb_a7800.txt (MIT) | c9de9a8bff0f | AM map, gun timing |
| CART.28 | T3 | github.com/7800-devtools/7800basic includes/*.asm, i2c7800.inc (CC0); LICENSE.txt | e7c18d2ecdc6 | drivers, HSC use, 7800sign LGPL |
| CART.29 | T1 | github.com/RetroAchievements/rcheevos src/hash/rc_hash_rom.c (MIT) | 116d01847075 | hack identity rule |
| CART.30 | T3 | github.com/wavemotion-dave/A7800DS README (docs only) | 6c108cd9767a | v4 fallback, EXRAM/A8, .hsc |
| CART.31 | T1 | docs.libretro.com/library/prosystem/ and /guides/softpatching/ | accessed 2026-10-15 | no SRAM, soft-patching |
| CART.32 | T2 | atarimagazines.com/v7n8/xg1light.php (ANTIC, 1988) | accessed 2026-10-15 | XG-1 trigger polarity |
| CART.33 | T2 | zerosoft.zophar.net/ips.php; romhacking.net/documents/746 (BPS, not fetched) + vidua.readthedocs.io BPS notes | accessed 2026-10-15 | patch formats |
| CART.34 | T4 | AtariAge threads 268461, 268458, 318275, 293427, 353281, 383566, 355224 (via harvest notes) | accessed 2026-10-15 | gun, keypad, POKEY reset, ProSystem |
| CART.35 | T4 | forums.atariage.com/topic/160424 (search snippet only) | accessed 2026-10-15 | v4 option draft, 7800GD |
| CART.36 | T2 | Stella man page (hexmos mirror) | accessed 2026-10-15 | SaveKey file names |
| CART.37 | T1 | github.com/7800-devtools/a78_asm_header README (repo has no licence) | abc32926402e | tooling, licence status |
| CART.38 | T1 | Microchip [24LC256 datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/MPD/ProductDocuments/DataSheets/24AA256-24LC256-24FC256-256K-I2C-Serial-EEPROM-DS20001203.pdf) | DS20001203Y, 1998–2022, fetched 2026-10-24 | sections 4–8 protocol, 5 ms maximum, page/read wrap |
| CART.39 | T2 | …/Converting_Rikki_&_Vikki_(Steam)_to_an_A78 | accessed 2026-10-15 | Souper music image |
| CART.40 | T1/T2 | coordinator's provenance audit: MiSTer per-file licence history (lightgun.sv, mouse_st.sv and detect2600.sv unknown; EEPROM_24LC256.sv GPL-3; mem4.hex = HSC ROM); HSC hashes; libretro 7800 DAT | audit 2026-10-15; DAT 2026.10.07 | licence limits, DAT counts |
| CART.41 | T4 | mametesters.org/view.php?id=6060 (trebor relaying RevEng, 2015-11-04; fixed in 0.168) | accessed 2026-10-15 | RIOT port B outputs high at power-on; One On One reads INPT4 |
