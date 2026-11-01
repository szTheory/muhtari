# Phase 13 independent continuation matrix

This matrix records the owned source used by `core.state.topologies` and its
cross-checks with the existing state-field and battery cases. Images, BIOS
and HSC vectors in the topology case are generated in process memory; no ROM,
BIOS or firmware bytes are tracked. Each row loads two separate instances,
plants distinct expected mailbox and storage bytes in the source, snapshots
to the independently loaded peer, and checks the resumed video, PCM output,
audio counters, frame counters and canonical bytes. Memory-span addresses and
lengths are checked across restore.

## Core topology rows

| Row | Board / region / boot | Sound and memory owners | Input or pending boundary | Independent observation |
|---|---|---|---|---|
| `linear-ntsc-legacy` | Linear, NTSC, fast boot | POKEY at `$0440` and `$0450`; no expansion | Pending legacy joystick and console SELECT | `$1840` RAM mailbox, POKEY wiring, then `SWCHA=$EF` and `SWCHB=$3D` after the pending packet applies; untouched MARIA border pixel is color 0 |
| `supergame-pal-lightgun` | SuperGame, PAL, generated BIOS handoff | POKEY, synthetic 4 KiB BIOS | Pending lightgun plus Mega7800; scheduled light sample; future bus HALT and NMI deadlines | Input aim `(77,41)`, expected sample raster 53/x77 and next eligible sample tick; Mega scan state; separately checked HALT/NMI values |
| `activision-pokey` | Activision, NTSC, fast boot | POKEY at `$4000` | Pending paddle at positions 0 and 32768 | After the short handoff frame, paddle pin 0 is charged and pin 1 is not |
| `absolute-keypad` | Absolute, PAL, fast boot | POKEY at `$0440` | Pending keypad `0xA5`; keypad recovery deadline | Active 12-bit key matrix and released INPT4 value |
| `fractalus-motion` | Fractalus, NTSC, fast boot | POKEY at `$0800`; board RAM | Pending ST mouse movement `(13,-7)` | Both axis residuals and deadlines drain once; no movement repeats in the next frame |
| `bankset-xboard` | BankSet, NTSC, fast boot | XBoard RAM and POKEY at `$0440/$0450` | Pending SNES2Atari packet, then a one-bit serial scan | Cart RAM endpoints, expansion RAM endpoints, adapter packet, serial position and pin value |
| `bankset-xm-savekey-port2` | BankSet, PAL, fast boot | XM RAM, HSC stub mode, SaveKey on port 2 | Pending joystick | Cart, expansion and battery endpoints; EEPROM replay is separately exercised by `core.state.battery` |
| `bankset-hsc-stub-savekey-port1` | BankSet, NTSC, fast boot | HSC stub and SaveKey on port 1 | No conflicting legacy controller override | Cart and battery storage endpoints; both EEPROM owner ports are represented |
| `linear-bios` | Linear, PAL, generated BIOS handoff | No expansion | Pending Amiga mouse movement | Pending device values and once-only motion drain after the BIOS-started machine runs |
| `bankset-hsc-firmware` | BankSet, NTSC, fast boot | Synthetic unknown HSC firmware and SaveKey on port 2 | Pending keypad | Cart, expansion and battery endpoints plus keypad state |
| `linear-proline` | Linear, NTSC, fast boot | POKEY at `$0440/$0450` | Pending ProLine buttons | Pending packet plus resumed video/audio/counters |
| `linear-driving` | Linear, NTSC, fast boot | POKEY at `$0440/$0450` | Pending driving input with bounded X displacement | Pending packet and drained motion residual/deadline |
| `linear-trakball` | Linear, NTSC, fast boot | POKEY at `$0440/$0450` | Pending trak-ball input with bounded X/Y displacement | Pending packet and drained motion residual/deadline |

The rows are representative, not a Cartesian product. The same BankSet
builder exposes both expansion banks and POKEY windows; XM rows supply HSC
and EEPROM together, while a separate XBoard row covers the other expansion.
The battery case already drives a real SaveKey program, saves a pending page,
restores it into another machine, and verifies the commit and generation
behavior, so the matrix does not duplicate serial packet traffic.

Accepted board classes are Linear, SuperGame, Activision, Absolute, Fractalus
and BankSet. SOUPER remains a refused board in the current profile and is not
claimed as a continuation class. ProLine, joystick, lightgun/XG-1, paddles,
keypad, driving, trak-ball, ST mouse, Amiga mouse and both serial adapter
families have representative input rows. Legacy and extended packet paths
remain separate.

## Independent state and fault controls

Every row expects a fixed value at system RAM `$1840`, and checks first/last
cart RAM, expansion RAM and battery bytes when those spans exist. The
SuperGame path also checks the serialized pending bus deadlines. The audio
case seeds a valid two-entry PCM ring window (`-1234`, `2345`) and verifies
those values appear at the start of the public audio buffer after restore;
this is a test-owned queue state, not an emulated game trace. A queued TIA
raw-level transition runs through the mixer before the full resumed raw/PCM
outputs and final canonical bytes are compared. A deliberate RAM-mailbox
corruption makes the independent oracle return false, proving it detects a
shared omission.

`core.state.fields` supplies checksum-repaired domain and cross-field
refusals, including event indexes, device packet fields, pending deadlines,
PCM limits and ring canonicalization. `core.state.battery` supplies the
changed/equal/refused battery generations, pending EEPROM programming,
repeated restore, observer isolation and stable spans. These checks complement
the generated continuation rows; they do not replace the independent
mailbox, pin or pixel observations above.

The test preserves the current MARIA event cursor and continues through
actual frame execution after restore. The explicit bus HALT/NMI values are
scheduled through the bus owners to test that pending deadline boundary; this
does not claim a snapshot taken in the middle of a MARIA burst. The public API
currently resumes only at caller-selected frame boundaries.

## Confidence and unresolved edges

The generated images establish software state and deterministic observations,
not physical hardware equivalence. HSC stub is limited to direct SRAM access;
the synthetic firmware row is unknown firmware, with no claim of OpenHSC
compatibility. XM physical semantics and HSC window gating retain the
medium-confidence labels in `docs/hardware/cartridge.md`. PAL SuperGame BIOS
startup is generated in memory and is not a pinned OpenBIOS run. MARIA timing
and controller values keep their source/confidence labels in
`tests/diag/expected.csv` and the corresponding hardware documentation.

The automatic STATE-02 shape classification and the flagged edge-probe row
remain unresolved. This matrix does not infer a new interpretation, run the
Phase 12 workflow, or claim hosted, RetroArch, physical-ROM or cross-platform
evidence.

## Local verification

The exact-source test executable was compiled directly with the project C17
warning flags against `build/ci/libmuhtari.a`. The `topologies`, `fields` and
`battery` cases each exited 0. The initial PCM continuation test was RED:
source save succeeded but the independent peer returned
`MUHTARI_STATUS_ERR_INVALID_STATE` for a nonempty saved FIFO. After the
validator used the candidate's ring cursor/count, the focused cases passed.
No full CI configuration was repeated; the required pinned build assets are
unavailable locally, as recorded by the phase execution constraints.
