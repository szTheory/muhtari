# Expansion diagnostic

`diag.expansion` uses one owned MIT 48 KiB upper-code template. Its source/config
are committed before the signed/headered bytes; the manifest, narrow admission
function and image builder authenticate the exact pin/digest. Derived headers
and startup inputs exist only under the build directory. Rebuilds use the pinned
assembler/sign/header recipe. No firmware or save media is committed.

The fixture initializes an empty protocol-1 mailbox, writes 19 measured records
with ordered IDs and computes its CRC-16 at runtime. The bound remains 27 records.
`expected.csv` contains literal expectations derived from these owned sentinels:

| Observation | XBoard bank p | XM CPU pages p and 15-p |
|---|---|---|
| Initial/disabled ROM endpoints | 49 | 49 |
| $4000/$5FFF | 64+p / 128+p | 64+p / 128+p |
| $6000/$7FFF | 96+p / 160+p | 64+(15-p) / 128+(15-p) |
| Alias/absent-control probe | 1080 | 64+p |
| Restored HSC endpoints | 165 / 90 | 165 / 90 |
| Supplied synthetic firmware marker | 90 | 90 |
| Same/changed/hidden-write HSC | 165 / 60 / 60 | 165 / 60 / 60 |
| POKEY timer IRQST | 254 | 254 |

XBoard's sixteen control aliases select all eight banks twice. Their measured
sum is 2*(64+65+66+67+68+69+70+71)=1080. XM's absent $0471/$0472 and unsupported
$0470 bits leave the selected data unchanged. Both profiles disable/reveal and
re-enable underlying ROM/RAM. The inexpensive cartridge/bus tests separately
cover individual gate bits, neighbors, hostile selectors and exact electrical
edges; this diagnostic adds actual instructions and DMA through real hosts.

All eight XBoard banks and all sixteen XM pages execute. Each XM group selects
CPU pages p and 15-p, with unequal MARIA pages p XOR 8 and p XOR 7. Groups put the
DLL in either $4100 or $6100. Thus every page is selected in both CPU windows and
both MARIA windows, with both non-HALT and HALT DLL reads. Every page/bank owns
distinct endpoint bytes and a DLL choosing an ordinary-RAM display list whose
horizontal position encodes its page number.

Each DLL has 256 two-line zones, enough for PAL. Its first five-byte object
header explicitly selects write mode 0: four-byte headers retain earlier write
mode, including one encountered when DMA first starts before VBE. OFFSET1 at VBE
loads the CPU DLL page without HALT. DMA18's OFFSET0 graphics play on raster19,
native row3. The next DLL entry is read during HALT; its OFFSET0 graphics play
on native row5. The independent judge expands each endpoint's four two-bit
pixels through literal palette bytes 0/22/39/56, doubled horizontally, starting
at 4*DLL-page + 16*object. It checks all 320 pixels on both selected rows. CRC
completion is required by call11; call12 is judged after that boundary. These
coordinates follow the documented line-RAM pipeline, rather than a screenshot.

The library API, actual runner process and dynamically loaded libretro module
each execute twelve bounded calls. The library restores the physical battery
span before execution; the runner imports through `--save-dir`; the module
restores through `RETRO_MEMORY_SAVE_RAM` before its first `retro_run`. Synthetic
4096-byte HSC firmware is generated under build and discovered by the real
module's system-directory path. XM initially hides both HSC windows. The ROM's
hidden store cannot replace restored A5/C3/5A bytes; later visible reads prove
the restore. Exactly one changed visible byte advances generation to1. Same-value
and hidden stores plus subsequent gate toggles preserve all other bytes, token,
layout1, pointer, length, attachment and firmware identity. Each frame checks
those physical spans. A console Reset packet on call10 retains the device state;
library power cycle and module `retro_reset` subsequently clear volatile RAM and
controls while retaining storage identity, bytes and generation. The runner's
export is checked byte-for-byte against the independently defined final save.

Representative NTSC/PAL groups compare equal-cycle enabled, disabled and disabled
INIT-store runs. Each executes the same tone/timer instructions. The installed
$450 POKEY continues to sound and assert its declared IRQ while decode is off;
the ignored SKCTL store cannot reset it. Cases cover a declared single $450,
the same installed chip without a sound declaration, and $450+$440. Raw tuples
are captured with absolute tick, source and four-bit DAC code: primary channel0
is source2, secondary channel0 is source6. Nonzero transitions must use levels
15 and9 respectively, with ordered ticks/sources. Timer IRQST FE, IRQ level,
topology and audible PCM are independently checked. Sample counts use the
documented rational clock lattice: NTSC first8190 then119402 ticks at88/13125;
PAL first141960 then142102 at4000/591149, carrying each remainder.

Only after numeric and native-page judgment do all twelve native-frame hashes,
canonical raw tuple hash, PCM hash and sample counts compare across hosts. The
runner's actual call12 PNG is parsed and judged independently, including native
height, row filters, checksums and palette conversion. Gate/control equality
also compares the actual raw/PCM/count observations. The ordinary sustained PAL
diagnostic remains unchanged; no 600-frame workload is multiplied here.

The real mailbox consumer rejects missing/truncated/trailing reports, missing,
extra, duplicate and reordered records, identity/version/flags/CRC faults,
reserved bytes and CRC-valid wrong values. Semantic controls include wrong CPU
page, leaked hidden HSC and reset timer status. A wrong-MARIA-page executable
fails both native and runner PNG judges; an enabled INIT store fails the real
tone/timer consumer. Altered template identity and forged raw observations fail.
Negative actual loads name YM2151, Covox, ADPCM, Souper and conflicting $4000/
$0800 sound owners; the runner returns77 and the module refuses with a named log.

This proves the selected emulator model, not new hardware measurements. XM
RAM/POKEY/HSC and gating retain medium physical confidence; its primary developer
specification remains inaccessible. ROF aliases, internal BIOS/top-slot, flash,
PIA, flat48KiB, protection/locks, PAL firmware selection and full-XM services are
absent. Unsupported controls are absence probes, never invented detection success.
