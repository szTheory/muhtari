# Asset policy

Which bytes may be in this repository, its history, its CI artifacts and its release archives: game ROMs, BIOS and other firmware, saves, states, screenshots and test fixtures. Code provenance is in [PROVENANCE.md](PROVENANCE.md); this file covers data. Written on 2026-10-15; updated 2026-10-19. `[AP.NN]` IDs resolve in §8.

| Question | Section |
|---|---|
| Can this file be committed? How is it listed? | 1, 2 |
| What is built in CI instead of committed? | 3 |
| How do I test with my own dumps and BIOS? | 4 |
| What enforces this? | 5 |

## 1. Never
- Bytes of a commercial game in any amount or form: ROM images with or without an A78 header, patches that embed them, disassemblies, and extracted graphics, sound or text.
- Firmware: the Atari 7800 BIOS (NTSC, PAL, prototype), the High Score Cartridge ROM, any part of the loader the BIOS leaves in RIOT RAM, and other cartridge or 2600 firmware such as the Supercharger BIOS. This includes copies inside permissively licensed repositories (MiSTer `rtl/mem4.hex` is the HSC ROM, `rtl/ar.hex` is the Supercharger BIOS, and `rtl/RIOT/M6532.sv` L69–80 is the BIOS loader [AP.03]), and bytes retyped into source code.
- KiloParsec, the all-rights-reserved game inside 7800OpenBIOS [AP.04].
- Battery saves, HSC or SaveKey images, save states, movies, screenshots, recordings, traces or logs made while running a commercial game, a real BIOS or a real HSC ROM.
- Bulk tables from databases that state no licence (No-Intro DATs, emulator header databases, ROM-pack headers). MAME's CC0 `hash/a7800.xml` is the one allowed database [AP.01].
- Atari logos and artwork, including the Fuji; for example, a frame dump of a BIOS splash screen.
- Test ROMs or data whose authors state no licence, or a copyleft or non-commercial one (handled as in §3).

Owning the cartridge or the console changes nothing on this list. A player's own dumps stay on the player's machine.

## 2. Allowed, when listed in `tests/roms/manifest.txt`
- Third-party test ROMs and data under a licence from PROVENANCE.md §1, checked in both the LICENSE and the file's header at the exact pin.
- Our own builds: diagnostic ROMs assembled from this repository's MIT sources (their cc65 runtime adds Zlib), fuzz seeds and generated fixtures.
- Single facts about a game or board (board type, ROM size, a hash), and images or recordings made from committed fixtures or from the core's own test card.

The manifest has one line per file, with tab-separated fields; `#` starts a comment. It uses nesturbator's format plus a `notice` field [AP.07]: `path source pin licence sha256 notice`. The fields are: path from the repository root; source = upstream URL or `this-repo`; pin = 40-hex commit, or release tag plus asset name; licence = SPDX expression; sha256 = 64 lowercase hex; notice = the `THIRD-PARTY-NOTICES.md` section, or `-`. Example: `tests/roms/diag/dma-cost.a78  this-repo  <commit>  MIT AND Zlib  <sha256>  cc65`.
- **Recommendation:** the `notice` field, which ties each third-party file to its notice. **Strongest alternative:** nesturbator's five fields. **Would change if:** no committed fixture ever needs a notice.

## 3. Built in CI, hashes committed
A file whose licence is unstated or restricts redistribution is built or fetched in CI at pinned commits. Only its digests are committed: `tests/built/expected.txt` holds the name, tool and source pins, ROM SHA-256, and frame and audio hashes. Tools are pinned by the SHA-256 of their archives, and every OS must produce byte-identical files. Built files stay in the build directory; they never reach artifacts or releases.

| Fixture | Built or fetched from (pin) | Why not committed |
|---|---|---|
| 7800basic samples | 7800basic tag v0.41 `03aebb85206f53046425c19a236f65ae80d9f498`, its release package with the bundled `dasm.wasm`, run under the pinned wasmtime | sample sources, `rmtplayer.asm` and `macro.h` state no licence [AP.05] |
| OpenBIOS | 7800OpenBIOS `b8bc745a8744`, with `kiloparsec.bl1` replaced by 8,192 fill bytes | CC0 code, but its splash screen draws the Atari Fuji [AP.04] |
| GPL test suites (e.g. Klaus Dormann's) | fetched at a pin; SHA-256 checked | GPL-3 |
- **Recommendation:** build OpenBIOS in CI. **Strongest alternative:** commit the KiloParsec-free binary, which CC0 allows. **Would change if:** the dasm build is not byte-identical on all six platforms.

## 4. Local only: your own dumps and BIOS
- The local run takes `MUHTARI_ROM_DIR` (a directory of your own cartridge dumps) and, optionally, `MUHTARI_BIOS` (a BIOS file) inline on the command line; the command is in [emulation tests, ledger](docs/emulation-tests.md#ledger). Everything it writes stays under the ignored `build/library/`.
- Firmware is identified by digest. The digests are facts and may be committed; the bytes may not [AP.01][AP.02]:

| Dump | Bytes | CRC32 | MD5 | SHA-1 |
|---|---|---|---|---|
| NTSC BIOS (MAME `7800.u7`, libretro `7800 BIOS (U).rom`) | 4,096 | `5d13730c` | `0763f1ffb006ddbe32e52d497ee848ae` | `d9d134bb6b36907c615a594cc7688f7bfcef5b43` |
| PAL BIOS (MAME `c300558-001b.u7`, libretro `7800 BIOS (E).rom`) | 16,384 | `d5b61170` | `397bb566584be7b9764e7a68974c4263` | `5a140136a16d1d83e4ff32a19409ca376a8df874` |
| BIOS with Asteroids (MAME `c300558-001a.u7`) | 16,384 | `a0e10edf` | not published | `14584b1eafe9721804782d4b1ac3a4a7313e455f` |
| HSC ROM (MAME `highscre.bin`) | 4,096 | `9be408d3` | not published | `a3af676991391a6dd716c79022d4947206b78164` |

No SHA-256 is published for these dumps (search on 2026-10-15) [AP.09]. The first maintainer dump that matches both CRC32 and SHA-1 records it here. A bad NTSC dump with wrong final bytes reportedly circulated (unverified) [AP.08], so the runner reports an unknown BIOS digest by name instead of guessing.

## 5. How this is checked
- `scripts/hygiene.sh` runs at pre-commit, at pre-push over all outgoing history, and in CI. Unless the manifest lists the file, it rejects ROM, BIOS and save extensions (`.a78 .a26 .bin .rom .hsc .sav .srm .state .eeprom`, as in `.gitignore`), the A78 magic `ATARI7800` at offset 1 [AP.06], and any file containing a NUL byte. `policy.rom_hash_coverage` ties manifest rows to files.
- `tests/cmake/firmware_scan.cmake` hashes every member of every release zip and every tracked file (SHA-1, SHA-256): whole, and again without the first 128 bytes when an A78 header is present. It fails on any §4 digest or the Supercharger BIOS (`tests/release/firmware.txt`), any built-sample SHA-256 in `tests/built/expected.txt`, and any 7800 ROM SHA-1 or 2600 SHA-1 prefix from MAME's CC0 hash lists in `src/cartdb.c`. `release.firmware_scan` runs it over each build leg's zips, `hygiene.firmware_scan` over the tracked files, and `scripts/publish.sh prepare` over the 18 release zips before `SHA256SUMS`; `release.firmware_scan.self_test` and `hygiene.firmware_scan.self_test` are its negative controls, each a synthetic A78 file.

## 6. Names
"Atari", "7800" and "ProSystem" are used only to say what the core emulates. The project is not affiliated with or endorsed by Atari.

## 7. If something gets in
Remove it and rewrite the branch before pushing; the push hook scans every commit about to leave. If it was already pushed, rewrite the history, force-push, and ask GitHub support to drop cached copies.

## 8. Sources
| ID | Tier | Source (URL) | Pin or access date | Supports |
|---|---|---|---|---|
| AP.01 | T1 | github.com/mamedev/mame `src/mame/atari/a7800.cpp` L1465–1476, `hash/a7800.xml` L1–4 and L2879–2887, `hash/a2600.xml` L16483 | `dddf4cd1faee` | BIOS and HSC digests; CC0 list; Supercharger digest |
| AP.02 | T1 | github.com/libretro/libretro-database `dat/System.dat` L74–76 | `12c21cf79f1e` | MD5s, libretro file names |
| AP.03 | T1 | github.com/MiSTer-devel/Atari7800_MiSTer `rtl/mem4.hex`, `rtl/ar.hex` (decoded and hashed here (tested)), `rtl/RIOT/M6532.sv` L69–80 | `0dc8ad2e3ff7` | firmware inside an MIT repo |
| AP.04 | T1 | github.com/7800-devtools/7800OpenBIOS `README.txt`, `7800openbios.asm` L1–11, L164 | `b8bc745a8744` | KiloParsec terms; Fuji data |
| AP.05 | T1 | github.com/7800-devtools/7800basic `LICENSE.txt`, `includes/*`, `samples/` | tag v0.41 `03aebb85206f` | unstated sample licences |
| AP.06 | T2 | 7800.8bitdev.org "A78 Header Specification" (magic at offsets 1 and 100); MAME `a78_slot.cpp` L490–499 | rev 1120; `dddf4cd1faee` | A78 signature |
| AP.07 | T1 | github.com/szTheory/nesturbator `ASSET_POLICY.md`, `tests/roms/manifest.txt`, `scripts/hygiene.sh` | `9f9386c661f6` | template, manifest format |
| AP.08 | T4 | forums.atariage.com/topic/266712 "anybody dump their NTSC BIOS" (search snippet; page returned 403) | accessed 2026-10-15 (unverified) | bad dump |
| AP.09 | T4 | web search for SHA-256 digests of these dumps (none found) | accessed 2026-10-15 | none published |
