# Cartridge images

`src/cart.c` identifies a cartridge image for `muhtari_probe` (`docs/embedding.md`): it says whether the bytes are a
7800 cartridge, which board and devices it needs, or why it is refused. It reads the A78 header when there is one,
looks the ROM bytes up in two lists generated from MAME's CC0 software lists (`src/cartdb.c`), and otherwise falls
back to the test the 7800 BIOS itself applies.

Sources:

- **The A78 header specification**, 7800.8bitdev.org, "A78 Header Specification", revision 1120, with its older
  revisions for the version history.
- **The cartridge notes**, `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` sections 1, 2 and 7, which
  gather the header fields, how MAME, a7800, MiSTer, A7800DS and BizHawk read them, the boards, the BIOS footer
  checks and the bounds for hostile input, each with its own source.
- **The software lists**, MAME `hash/a7800.xml` and `hash/a2600.xml` (CC0) at the commit `src/cartdb.c` names.

## The A78 header

An A78 file is a 128-byte header and the ROM after it. Bytes 1-9 hold `ATARI7800`; the 7 bytes after it are padding,
zeros in the specification and spaces in some tools, and only the 9 bytes are compared. Multi-byte fields are
big-endian.

| Offset | Size | Field | Read as |
|---|---|---|---|
| 0 | 1 | version | 1 to 4; above 4 is read as 4 |
| 1 | 16 | magic | `ATARI7800` and padding |
| 17 | 32 | title | up to the first NUL, or all 32 bytes when there is none |
| 49 | 4 | ROM size | the bytes after the header the cartridge maps |
| 53 | 2 | cart type | the board bits below |
| 55, 56 | 1 + 1 | port 1, port 2 | controller codes 0 to 12 |
| 57 | 1 | TV | bit 0 PAL; bits 1 and 2 composite artifacts and multi-region; bits 3-7 reserved |
| 58 | 1 | save | bit 0 High Score Cartridge, bit 1 SaveKey or AtariVox; bit 2 an old way to say the HSC; bits 3-7 reserved |
| 59 | 4 | reserved | zero |
| 63 | 1 | XM | non-zero when the XM expansion is wanted; 1 is the value written |
| 64 | 1 | mapper | version 4: 0 linear, 1 SuperGame, 2 Activision, 3 Absolute, 4 Souper |
| 65 | 1 | mapper options | version 4: the device at $4000, and bit 7 bankset |
| 66 | 2 | audio | version 4: POKEY placement in bits 0-2, bit 3 YM2151, bit 4 Covox |
| 68 | 2 | IRQ | version 4: interrupt sources in bits 0-2 |
| 70 | 30 | reserved | zero |
| 100 | 28 | `ACTUAL CART DATA STARTS HERE` | not read |

Below version 4, bytes 64-69 are reserved too.

### Cart type

| Bit | Meaning | Bit | Meaning |
|---|---|---|---|
| 0 | POKEY at $4000 | 8 | Activision banking |
| 1 | SuperGame banking | 9 | Absolute banking |
| 2 | 16 KiB RAM at $4000 | 10 | POKEY at $0440 |
| 3 | ROM at $4000, the extra first bank of SG9 | 11 | YM2151 at $0460 |
| 4 | bank 6 at $4000 | 12 | Souper |
| 5 | banked RAM, two 16 KiB banks | 13 | bankset |
| 6 | POKEY at $0450 | 14 | halt-banked RAM |
| 7 | Fractalus mirror RAM, 2 KiB at $4000 | 15 | POKEY at $0800 |

Some bits cannot hold together. They are resolved in MAME's order, and `MUHTARI_CART_WARN_CART_TYPE_CONFLICT` says
one was:

1. More than one of Fractalus, Absolute, Activision, bankset and Souper is a conflict; the last in that list wins.
2. Activision or Absolute clears bits 0-7.
3. ROM at $4000, bank 6 or banked RAM without SuperGame sets SuperGame.
4. POKEY at $4000 is dropped beside RAM, bank 6 or banked RAM at $4000. Beside ROM at $4000 it stays, as a
   write-only POKEY, which is no conflict.
5. RAM at $4000 is dropped beside ROM at $4000, bank 6 or banked RAM.
6. ROM at $4000 is dropped beside bank 6 or banked RAM.
7. Bank 6 is dropped beside banked RAM.

### The v4 block

A version 4 header's block at bytes 64-69 replaces the cart type's board, RAM and sound when it validates and its
board fits the body (see Geometry). It validates when the mapper is at most 4, the audio word uses only bits 0-5
with a POKEY code of at most 5, the IRQ word only bits 0-2, and the options name a device the mapper has with every
other option bit clear:

| Device code | Linear (bits 0-1) | SuperGame (bits 0-2) | Read as |
|---|---|---|---|
| 1 | 16K RAM | 16K RAM | 16 KiB RAM at $4000 |
| 2 | 8K EXRAM/A8 | 8K EXRAM/A8 | the Fractalus mirror RAM |
| 3 | 32K EXRAM/M2 | 32K EXRAM/M2 | halt-banked RAM |
| 4 | | EXROM | ROM at $4000 |
| 5 | | EXFIX | bank 6 at $4000 |
| 6 | | 32K EXRAM/X2 | banked RAM |

The specification names these options without saying what they do; the readings of A8, M2 and X2 are the cartridge
notes' (section 1.1) and are unverified. Both A8 and M2 name boards this library refuses, so a wrong reading can only
refuse an image. The audio POKEY codes are 1 at $0440, 2 at $0450, 3 at both, 4 at $0800 and 5 at $4000. The other
mappers take no device. Otherwise the cart type applies and `MUHTARI_CART_WARN_HEADER_V4_IGNORED` is set. Ports, TV,
save and XM always come from bytes 55-63.

### Warnings

A defect the probe accepts sets a bit in `warnings`:

| Bit | Set when |
|---|---|
| `HEADER_V4_IGNORED` | a version 4 block does not validate, or its board does not fit the body |
| `HEADER_CORRECTED` | a list entry matches the ROM bytes and its board, RAM or sound differs from the header's |
| `VERSION_ABOVE_4` | the version byte is above 4 |
| `SIZE_FIELD_MISMATCH` | the ROM size differs from the bytes after the header |
| `TITLE_NONPRINTABLE` | a title byte before the first NUL is outside $20-$7E; it is shown as `?` |
| `UNKNOWN_PORT` | a controller code is above 12; that port is reported as none |
| `LEGACY_SAVE_BIT` | save bit 2 is set; it is read as the HSC |
| `XM_VALUE` | the XM byte is neither 0 nor 1; it still asks for the XM |
| `RESERVED_NONZERO` | a reserved byte or bit is set |
| `CART_TYPE_CONFLICT` | cart-type bits conflict, resolved as above |

The title keeps at most 32 bytes, drops trailing spaces and is NUL-terminated in the 36-byte field. Magic padded with
spaces is common, the 7800basic tools write it, and sets nothing. Region is PAL when TV bit 0 is set, NTSC otherwise.

## Probe order

1. The arguments, the size tag and the size cap, before any byte is hashed.
2. **The A78 magic.** A file that ends inside the header, or right after it, is truncated, except that a ROM size of
   0 is invalid. The header is parsed, and the cartridge maps min(ROM size, bytes after the header). Those bytes
   are looked up in the 7800 list as the Hash list section describes; a match to an entry that is not a cartridge (the HSC ROM, the XM, the
   XBoard) is invalid, and any other match replaces the board, RAM and sound and makes the source the hash, never
   the ports, nor the region unless TV bit 2 (multi-region) leaves the header naming none (see Hash list). Then a
   refused board is reported, and the geometry is checked: a body that fails it is truncated when the ROM size is
   larger than the bytes present, and invalid otherwise. A short body that fits its board loads as the bytes present.
3. **The 7800 list**, for an image without the magic, by the whole image, as for a header. The entry gives the board, RAM,
   sound and region (PAL for a PAL-only entry, else NTSC), and port 1 is a light gun for a light-gun entry, ProLine
   otherwise; port 2 is ProLine. The image must fit the entry's board geometry or is invalid.
4. **The 2600 list**, by the first 8 bytes of the same SHA-1: a 2600 image.
5. **The footer test** below: a headerless 7800 image, linear up to 48 KiB and SuperGame above, which must fit that
   board's geometry or is invalid.
6. **The 2600 sizes**: a multiple of 2 KiB up to 64 KiB, 10,495 bytes, or a multiple of 8,448 bytes (Supercharger
   loads): a 2600 image.
7. Anything else is not a cartridge image.

A 2600 image reports only its kind, source, size and, for a list match, the match flag.

## Hash list

`src/cartdb.c` holds two lists generated from MAME's CC0 software lists, which `src/cartdb.h` describes:

- **The 7800 list.** One key per ROM chip, the chip's full SHA-1, sorted, pointing at an entry. An entry gives the
  board, board options, RAM, sound, its regions and a light-gun flag. A multi-chip entry also names a layout: the
  slice sizes of its image in address order, and which chip each slice holds (one chip may fill two slices).
- **The 2600 list.** The first 8 bytes of the SHA-1 of every 2600 ROM, sorted. Eight bytes keep the list at 12.8 KB
  instead of 32 KB. A false hit only refuses an image, and forging one needs a second preimage of 64 bits. The 7800
  keys stay full width, because a match overrides the header.

A body is looked up in a fixed number of SHA-1 passes, each over at most the body:

1. the whole body, matched against single-chip entries;
2. on a miss, each slice of every layout whose sizes add up to the body, matched when the slices are one entry's
   chips in the layout's order;
3. on a miss at exactly 128 KiB, the body again with each 16 KiB bank's 8 KiB halves swapped, fed to SHA-1 in that
   order without a copy. Activision boards come dumped both ways (cartridge notes, section 1.4), and only an
   Activision entry counts for this pass.

The slice and swap hashes are computed at run time, not listed: SHA-1 does not compose, so a whole-image key cannot
be derived from per-chip keys, nor a swapped key from a straight one.

A match overrides the header's board, board options, RAM and sound, sets `MUHTARI_CART_HASH_MATCH`, makes the
source `MUHTARI_SOURCE_HASH`, and sets `MUHTARI_CART_WARN_HEADER_CORRECTED` when the header said otherwise. It never
changes the controllers. The region comes from the header, from the entry when the header sets TV bit 2
(multi-region) and so names none, and otherwise NTSC. A headerless match takes the entry's region (PAL for a
PAL-only entry) and puts a light gun on port 1 for a light-gun entry, ProLine elsewhere.

Entries that are not cartridges stay in the list, so an image matching one is refused as invalid: the High Score
Cartridge ROM, listed both alone and inside the XM (its one duplicate key resolves to the first entry), and any XM or
XBoard entry. The test program links `tests/cart/test_cartdb.c` in place of the bundled lists, with keys for images
built in memory, and `cart.identify.bundled_has_no_test_entries` fails if any of its keys appears in `src/cartdb.c`.

## The footer test

The NTSC 7800 BIOS starts a cartridge in 7800 mode only when its last 8 bytes, $FFF8-$FFFF, pass three checks; it
falls back to 2600 mode otherwise (cartridge notes, section 1.5). The probe applies them with the region bit ignored:

- $FFF8's high nibble is $F.
- $FFF9's low nibble is 3 or 7, and its high nibble, the page where the signed area starts, is at least $4 and at or
  below the reset vector's page.
- The reset vector at $FFFC-$FFFD is neither $0000 nor $FFFF.

## Geometry

The body sizes each board this library runs accepts:

| Board | Body sizes |
|---|---|
| Linear | a multiple of 4 KiB from 4 KiB to 48 KiB, or exactly 64 KiB, of which the top 48 KiB is mapped |
| SuperGame | a multiple of 16 KiB from 32 KiB (2 banks) to 1 MiB |
| SuperGame with ROM at $4000 (SG9) | (2^n + 1) x 16 KiB up to 1 MiB: 48, 80, 144, 272 or 528 KiB |

The evidence is MAME's 7800 list, which gives these ROM sizes per slot:

| Slot | Sizes (entries) |
|---|---|
| linear (`a78_rom`) | 16 KiB (27), 32 KiB (57), 48 KiB (24), 64 KiB (11) |
| linear with POKEY (`a78_pokey`) | 16 KiB (1), 32 KiB (4) |
| SuperGame (`a78_sg`, `a78_sg_pokey`) | 64 KiB (5), 128 KiB (41) |
| SuperGame with RAM (`a78_sg_ram`) | 64 KiB (2), 128 KiB (10) |
| SG9 (`a78_sg9`, `a78_p450_ta`) | 144 KiB (13) |

Every listed size fits. The list's smallest linear body is 16 KiB; the table also accepts 4 KiB steps down to 4 KiB,
so small homebrew loads. A rule too loose only loads more short images as the bytes present, while one too strict
refuses a working game. SuperGame banks are 16 KiB, and the SG9 rule generalises the 144 KiB board to 2^n + 1 banks
as MAME and MiSTer do (cartridge notes, sections 2.1 and 7). A size outside the table is refused: as truncated when
the header claims more bytes than the file holds, as invalid otherwise.

## Boards

`MUHTARI_STATUS_OK` means linear or the SuperGame family: SuperGame, with RAM, SG9, bank 6 at $4000 and banked RAM.
Activision, Absolute, Fractalus, bankset, halt-banked RAM and Souper are reported with
`MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD`. XM, YM2151 and Covox are reported in `devices` and `sound` and never refused.

The boards are one `switch` in `src/cart.c`, so the core keeps no table of function pointers. MARIA's fetches see the
same bytes as the CPU on every board here.

### Linear

The ROM ends at $FFFF: a 16 KiB ROM starts at $C000, a 48 KiB one at $4000. Below its start nothing drives the bus, and
a read returns the open-bus byte (`docs/hardware/bus.md`, Open bus). A write changes nothing. A linear header that asks
for RAM gets it at $4000-$7FFF, in place of any ROM there (cartridge notes, section 2.1).

### SuperGame

The ROM is banks of 16 KiB. $C000-$FFFF holds the last bank. $8000-$BFFF holds the bank the register selects, and a
write anywhere in $8000-$BFFF sets the register. At power-on and after a power cycle the register is 0. $4000-$7FFF
depends on the board:

| Board | $4000-$7FFF | $8000-$BFFF after writing v |
|-------|-------------|-----------------------------|
| SuperGame | the second-to-last bank | bank v mod banks |
| with RAM | 16 KiB of RAM | bank v mod banks |
| bank 6 | bank 6 mod banks | bank v mod banks |
| SG9, 2^n + 1 banks | bank 0 | bank 1 + (v mod 2^n) |
| banked RAM | RAM half (v >> 5) & 1 of 32 KiB | bank v mod banks |

Sources: cartridge notes, section 2.1; the 144 KiB board is 9 banks and generalises to 2^n + 1. The board ROMs below
read each layout.

**Hostile input.** A bank number never indexes the ROM raw: every one is reduced modulo the bank count, so a write of
any value selects a bank that exists. A RAM index is masked with `ram_size - 1`, a power of two, so no header can
make a read or write leave the RAM (cartridge notes, section 7). A list entry for an 8 KiB-RAM game gives 8 KiB, which
the mask mirrors over $4000-$7FFF.

## Loading

`muhtari_load` identifies the image exactly as `muhtari_probe` does and returns the same status. A refusal returns
before anything is allocated and leaves the instance unchanged. An accepted image's mapped bytes, `rom_size` of them
from after the header when there is one, are copied into one allocation through the instance's allocator, which
replaces any cartridge loaded before; `muhtari_destroy` frees it. A board with RAM gets its ROM followed by its
cartridge RAM, `ram_size` bytes, in the same single allocation, so a load allocates once and running allocates
nothing. The RAM starts zeroed, and a power cycle zeroes it again.

## The board ROMs

Three diagnostic ROMs, built like the others (`docs/emulation-tests.md`, diag), read the bytes each board puts at
fixed addresses and report them through the mailbox and on the results screen. Each bank of each ROM starts with its
own number, so a read of a window's first byte names the bank in it. Every value below follows from the board rules of
cartridge notes, section 2.1, and is in `tests/diag/expected.csv` before the ROM first runs. The source is `guide`.
Record n has test id n and is the n-th row for its ROM.

| ROM | Mailbox id | Board | Records, in order |
|-----|------------|-------|-------------------|
| `tests/diag/linear16.s` | 4 | linear, 16 KiB at $C000 | 4: `c000`, `at_4000`, `at_8000`, `at_bfff` |
| `tests/diag/bank6.s` | 5 | SuperGame, 8 banks, bank 6 at $4000 | 12: `power_on`, `at_4000`, `at_c000`, `sel_0` to `sel_7`, `sel_11` |
| `tests/diag/sg144.s` | 6 | SuperGame 144K, 9 banks, bank 0 at $4000 | 12: `power_on`, `at_4000`, `at_c000`, `sel_0` to `sel_7`, `sel_9` |

The `modes` ROM is a plain SuperGame board of 4 banks; its `at_4000` reads bank 2, the second-to-last
(`docs/hardware/maria.md`, The modes ROM).

**linear16.** The 16 KiB ROM ends at $FFFF, so it starts at $C000, whose byte is $16: `c000` = 22. Nothing drives
$4000-$BFFF. A read there returns the open-bus byte (`docs/hardware/bus.md`, Open bus), and the last byte on the bus
before `LDA abs`'s read is the operand's high byte. So `LDA $4000` gives `at_4000` = $40 = 64, `LDA $8000` gives
`at_8000` = $80 = 128 and `LDA $BFFF` gives `at_bfff` = $BF = 191. A ROM mirrored there would give $16 or a code
byte instead.

**bank6.** 128 KiB is 8 banks. Before any write the bank register is 0, so `LDA $8000` reads bank 0: `power_on` = 0.
With the bank-6 option $4000 holds bank 6, `at_4000` = 6, and $C000 holds the last bank, `at_c000` = 7. Each test
then writes v to $8000 and reads $8000, which shows bank v mod 8: `sel_0` to `sel_7` give 0 to 7, and `sel_11`,
v = 11, gives 3.

**sg144.** 144 KiB is 9 = 2^3 + 1 banks. Bank 0 is fixed at $4000, `at_4000` = 0, and bank 8 at $C000, `at_c000` =
8. $8000 shows bank 1 + (r mod 8) for the register r. The register is 0 at power-on, so `power_on` = 1. Writing v
gives 1 + (v mod 8): `sel_0` to `sel_7` give 1 to 8, and `sel_9` gives 2.
