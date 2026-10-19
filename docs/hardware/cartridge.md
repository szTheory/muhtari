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
   0 is invalid. The header is parsed, and the cartridge maps min(ROM size, bytes after the header). The SHA-1 of
   those bytes is looked up in the 7800 list; a match to an entry that is not a cartridge (the HSC ROM, the XM, the
   XBoard) is invalid, and any other match replaces the board, RAM and sound, never the ports or region. Then a
   refused board is reported, and the geometry is checked: a body that fails it is truncated when the ROM size is
   larger than the bytes present, and invalid otherwise. A short body that fits its board loads as the bytes present.
3. **The 7800 list**, for an image without the magic, by the SHA-1 of the whole image. The entry gives the board, RAM,
   sound and region (PAL for a PAL-only entry, else NTSC), and port 1 is a light gun for a light-gun entry, ProLine
   otherwise; port 2 is ProLine.
4. **The 2600 list**, by the first 8 bytes of the same SHA-1: a 2600 image.
5. **The footer test** below: a headerless 7800 image, linear up to 48 KiB and SuperGame above, which must fit that
   board's geometry or is invalid.
6. **The 2600 sizes**: a multiple of 2 KiB up to 64 KiB, 10,495 bytes, or a multiple of 8,448 bytes (Supercharger
   loads): a 2600 image.
7. Anything else is not a cartridge image.

A 2600 image reports only its kind, source, size and, for a list match, the match flag.

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
