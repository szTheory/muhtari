# Phase 2: The CPU matches the public vectors - Context

**Gathered:** 2026-10-16
**Status:** Ready for planning

<domain>
## Phase Boundary

Phase 2 adds the SALLY CPU, the 7800 bus, the RIOT, the TIA's WSYNC, and cartridge probing and loading.

- **SALLY.** An NMOS 6502 with decimal mode. It must match the SingleStepTests `6502/v1` vectors on final state and
  on every bus cycle: the committed sample on every pull request, the full set nightly.
- **The bus.** It covers:
  - 4- and 6-tick cycles by address, with every cycle slow while MEN = 0;
  - the decode and its mirrors, and the open-bus byte;
  - the INPTCTRL latch and the two-write HALT gate;
  - HALT for reads and writes after the fitted latency, and RDY for reads only;
  - the NMI rules.

  Tests drive HALT and NMI through internal entry points that phase 3's MARIA will call.
- **RIOT (6532).** The timer, flags, ports and direction registers with the profile's power-on values, and its RAM.
- **TIA.** WSYNC on the TIA's own 456-tick line, and its read drivers.
- **Cartridges.** `muhtari_probe` enters the public header (API version 2). `muhtari-run --probe FILE` prints an
  image's identity and hashes. An internal loader validates an image before it allocates anything, and a loader fuzz
  target runs nightly.

Requirements CPU-01 to CPU-05. Merging publishes 0.2.0.

Not in this phase:
- Phase 3:
  - MARIA, its DMA, its WSYNC at $24, MSTAT and the DLI;
  - linear and SuperGame boards actually running;
  - fast boot and OpenBIOS;
  - controllers and console switches;
  - a public `muhtari_load`;
  - `cpu_phase`.
- Phase 4: TIA and POKEY sound.

</domain>

<decisions>
## Implementation Decisions

All prior choices in `.planning/preparation/DECISIONS.md` and phase 1's `01-CONTEXT.md` stand.

As in phase 1, each open area was researched by an advisor agent working from the owner's fan-out brief. Four
agents ran in parallel at calibration tier `minimal_decisive`. Their results were reconciled across areas and are
locked here. One decision adds public API to a library that is already released (D-01 to D-07), so it went to the
owner, who chose "Probe now, load in phase 3".

### Cartridge identity in the public API (CPU-05)

- **D-01:** In phase 2 the header gains `muhtari_probe` and its types, and nothing else.
  - **Versions.** `MUHTARI_API_VERSION` becomes 2. ABI 1 and behaviour revision 1 are unchanged, since no frame or
    audio hash changes.
  - **The loader stays internal.** It is `muhtari__cart_load(muhtari *m, const void *image, size_t n)` in
    `src/internal.h`, and only the tests and the fuzz harness reach it.
  - **Load arrives in phase 3.** `muhtari_load` and `muhtari_media` enter then, designed together with BIOS, region,
    ports and the profile, when a loaded cartridge actually runs. Until then `muhtari_run_frame` still gives the test
    card and silence.
  - **API test.** `tests/api/v2.c` calls `muhtari_probe` and touches every new name and value, so `abi.symbols` and
    `abi.api_version` (phase 1 D-12) cover the addition.
  - **Reversibility:** costly. From 0.2.0 this is published API. Pre-1.0 semver lets a later minor version break it
    (DEC.14), but every embedder would then have to change.

- **D-02:** The header additions. Status names keep version 1's `ERR_` prefix. Zero means "absent" in every enum
  (phase 1 D-10).
  ```c
  #define MUHTARI_API_VERSION 2
  #define MUHTARI_IMAGE_SIZE_MAX 1048704u /* 1 MiB of ROM plus a 128-byte header */

  /* appended to the status enum */
      MUHTARI_STATUS_ERR_INVALID_IMAGE = 5,
      MUHTARI_STATUS_ERR_TRUNCATED_IMAGE = 6,
      MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE = 7,
      MUHTARI_STATUS_ERR_UNSUPPORTED_2600 = 8,
      MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD = 9

  enum { MUHTARI_KIND_7800 = 1, MUHTARI_KIND_2600 = 2 };
  enum { MUHTARI_SOURCE_HEADER = 1, MUHTARI_SOURCE_HASH = 2, MUHTARI_SOURCE_FOOTER = 3,
         MUHTARI_SOURCE_SIZE = 4 };
  enum { MUHTARI_BOARD_LINEAR = 1, MUHTARI_BOARD_SUPERGAME = 2, MUHTARI_BOARD_ACTIVISION = 3,
         MUHTARI_BOARD_ABSOLUTE = 4, MUHTARI_BOARD_FRACTALUS = 5, MUHTARI_BOARD_BANKSET = 6,
         MUHTARI_BOARD_SOUPER = 7 };
  enum { MUHTARI_BOARD_ROM_AT_4000 = 1u, MUHTARI_BOARD_BANK6_AT_4000 = 2u,
         MUHTARI_BOARD_BANKED_RAM = 4u, MUHTARI_BOARD_HALT_BANKED_RAM = 8u };
  enum { MUHTARI_SOUND_POKEY_4000 = 1u, MUHTARI_SOUND_POKEY_450 = 2u, MUHTARI_SOUND_POKEY_440 = 4u,
         MUHTARI_SOUND_POKEY_800 = 8u, MUHTARI_SOUND_YM2151 = 16u, MUHTARI_SOUND_COVOX = 32u };
  enum { MUHTARI_DEVICE_HSC = 1u, MUHTARI_DEVICE_SAVEKEY = 2u, MUHTARI_DEVICE_XM = 4u };
  enum { MUHTARI_PORT_NONE = 0, MUHTARI_PORT_PROLINE = 1, MUHTARI_PORT_LIGHTGUN = 2,
         MUHTARI_PORT_PADDLE = 3, MUHTARI_PORT_TRAKBALL = 4, MUHTARI_PORT_JOYSTICK = 5,
         MUHTARI_PORT_DRIVING = 6, MUHTARI_PORT_KEYPAD = 7, MUHTARI_PORT_ST_MOUSE = 8,
         MUHTARI_PORT_AMIGA_MOUSE = 9, MUHTARI_PORT_ATARIVOX = 10, MUHTARI_PORT_SNES2ATARI = 11,
         MUHTARI_PORT_MEGA7800 = 12 }; /* the A78 byte 55/56 codes */
  enum { MUHTARI_REGION_NTSC = 1, MUHTARI_REGION_PAL = 2 };
  enum { MUHTARI_CART_HEADER = 1u, MUHTARI_CART_HASH_MATCH = 2u };
  enum { MUHTARI_CART_WARN_HEADER_V4_IGNORED = 1u, MUHTARI_CART_WARN_HEADER_CORRECTED = 2u,
         MUHTARI_CART_WARN_VERSION_ABOVE_4 = 4u, MUHTARI_CART_WARN_SIZE_FIELD_MISMATCH = 8u,
         MUHTARI_CART_WARN_TITLE_NONPRINTABLE = 16u, MUHTARI_CART_WARN_UNKNOWN_PORT = 32u,
         MUHTARI_CART_WARN_LEGACY_SAVE_BIT = 64u, MUHTARI_CART_WARN_XM_VALUE = 128u,
         MUHTARI_CART_WARN_RESERVED_NONZERO = 256u, MUHTARI_CART_WARN_CART_TYPE_CONFLICT = 512u };

  typedef struct muhtari_cart_info {
      uint32_t size;
      uint32_t kind, source, flags, warnings;
      uint32_t rom_size;                 /* bytes mapped, header excluded */
      uint32_t board, board_options, ram_size, sound, devices;
      uint32_t port1, port2, region;
      uint32_t a78_version, a78_rom_size, a78_cart_type, a78_port1, a78_port2;
      uint32_t a78_tv, a78_save, a78_xm, a78_mapper, a78_mapper_options, a78_audio, a78_irq;
      uint8_t title[36];                 /* printable ASCII, NUL-terminated, zero-padded */
  } muhtari_cart_info;

  muhtari_status muhtari_probe(const void *image, size_t n, muhtari_cart_info *out);
  ```
  - **Layout.** `src/internal.h` defines `MUHTARI__CART_INFO_SIZE_FIRST` as the end of `title` and pins every offset
    with `_Static_assert`; `sizeof` is 140. There are no `uint64_t`, pointer or `bool` fields.
  - **What stays out:**
    - hashes, pointers and names from the database;
    - the reserved bytes and the `ACTUAL CART DATA` text;
    - 2600 bank schemes;
    - a warning for magic padded with spaces, which is common in the wild: the 7800basic sample has it (tested).

- **D-03:** `muhtari_probe`'s behaviour.
  - **Pure.** It allocates nothing, keeps no state, and gives the same output for the same bytes.
  - **Bounded.** It makes a fixed number of SHA-1 passes over at most `MUHTARI_IMAGE_SIZE_MAX` bytes:
    - the mapped body;
    - each slice of a multi-chip layout whose sizes add up to the body;
    - for a 128 KiB image that misses the table, the body again with each 16 KiB bank's 8 KiB halves swapped, fed to
      SHA-1 in that order without making a copy.
  - **Checks, in order:**
    1. `out` is NULL, or `image` is NULL with `n` > 0: `ERR_ARGUMENT`.
    2. A bad size tag: `ERR_STRUCT_SIZE`.
    3. `n` > `MUHTARI_IMAGE_SIZE_MAX`: `ERR_UNSUPPORTED_SIZE`, before anything is hashed.
    4. DEC.38's order: A78 magic, then the 7800 table, the 2600 table, the footer test and the size, using D-05's
       rules.
  - **Status and output.** Probe returns the status the loader would return. It writes `out` only for OK,
    `ERR_UNSUPPORTED_2600` and `ERR_UNSUPPORTED_BOARD`, so a host can show what was refused. For a 2600 image only
    `kind`, `source`, `rom_size` and `flags` are non-zero. `docs/embedding.md` states this as the one exception to
    "an error writes nothing".
  - **A hash match** follows A7800-CARTRIDGES-AND-PERIPHERALS §1.4:
    - It overrides board, RAM and sound, and sets `HEADER_CORRECTED` when the header said otherwise. It never
      overrides the controllers.
    - Region comes from the header, else from the entry, else NTSC.
    - A headerless image's ports come from the entry's light-gun flag, else ProLine.

- **D-04:** OK means a board in v1.0's loader set: linear, or the SuperGame family (SG, SG with RAM, SG9/EXROM,
  bank 6, banked RAM). Phase 3 runs these.
  - `ERR_UNSUPPORTED_BOARD` covers Activision, Absolute, Fractalus (cart-type bit 7), bankset (bit 13), halt-banked
    RAM (bit 14) and Souper (bit 12 or v4 mapper 4).
  - XM, YM2151 and Covox are reported in `devices` and `sound`, not refused.
  - A board moves from refused to OK in the release that emulates it.

- **D-05:** The failure classes, reconciled between the API research and the fixture research:
  - **`ERR_UNSUPPORTED_SIZE`:** `n` > `MUHTARI_IMAGE_SIZE_MAX`.
  - **`ERR_TRUNCATED_IMAGE`:** either case below.
    - The A78 magic is present, but the file ends inside the 128-byte header or at its end while the size field is
      non-zero.
    - The size field exceeds `n` − 128, and the bytes present fail the board's geometry.
  - **Short but valid bodies load.** When the bytes present do satisfy the geometry, the loader keeps the settled
    rule: it takes min(field, `n` − 128) bytes, as MAME and a7800 do (A7800-CARTRIDGES-AND-PERIPHERALS §7,
    ARCHITECTURE §6), and sets `SIZE_FIELD_MISMATCH`.
    - The fixture research proposed treating any shortfall as truncated. That was not taken: old dumps carry stale
      headers (§1.6), and a size field that counts the header would refuse a working game.
  - **`ERR_INVALID_IMAGE`:**
    - a size field of 0;
    - a headerless file that fails both the footer test and the 2600 sizes;
    - a geometry that fits no board;
    - a hash match to an entry that is not a cartridge (D-24).
  - **An invalid v4 block** is not a refusal. v3 applies and `HEADER_V4_IGNORED` is set (§1.3).
  - **The geometry table** per board comes from §2 and §7. Planning fixes linear's smallest size and step from the
    CC0 list's sizes and the 7800basic samples, and `docs/hardware/cartridge.md` records the table.

- **D-06:** Hashes.
  - **The core** has one hash, SHA-1, in `src/sha1.c`: our own code from FIPS 180-4, about 90 lines. It looks up the
    table over the bytes the cartridge maps, min(field, `n` − 128). The core still links only `memcpy`, `memmove`,
    `memset` and `memcmp`.
  - **The runner.** `muhtari-run --probe` prints eight hashes: `sha1`, `sha256`, `md5` and `crc32` of the file as
    given, and the same four with the suffix `_headerless`.
    - "Headerless" follows rcheevos's 7800 rule exactly. If the file is longer than 128 bytes and bytes 1–9 are
      `ATARI7800`, it drops the first 128 bytes; otherwise it hashes the whole file (rcheevos @1433173220a7,
      `src/rhash/hash_rom.c`, tested).
    - Where the core's mapped bytes differ from that input, `SIZE_FIELD_MISMATCH` says so.
  - **Implementations.** The runner reuses the core's SHA-1 through `src/sha1.h`, so the repository holds one SHA-1.
    It keeps `runner/sha256.c` and `runner/crc32.c`, and adds `runner/md5.c`, written from RFC 1321.
  - **No digest** enters `muhtari_cart_info`. One can be appended later without breaking anything.

- **D-07:** `muhtari-run --probe FILE`.
  - **Arguments.** It takes one file. Any other mode or option given with it exits 2.
  - **Reading.**
    - It reads in chunks into one buffer of `MUHTARI_IMAGE_SIZE_MAX` + 1 bytes, looping on `fread` until it returns
      0. It uses no `fseek` or `ftell`, so there is no `long` overflow on Win64, and pipes work.
    - `ferror` exits 1 ("cannot read").
    - Probe receives the byte count that was read, so the core alone decides `ERR_UNSUPPORTED_SIZE`.
  - **Output.** Standard output gets `key value` lines, where the value is everything after the first space:
    - **Order:**
      1. `kind source board board_options rom_size ram_size sound devices port1 port2 region`;
      2. `title a78_version a78_rom_size a78_cart_type a78_port1 a78_port2 a78_tv a78_save a78_xm a78_mapper
         a78_mapper_options a78_audio a78_irq`;
      3. one `warning <name>` line for each bit set;
      4. `sha1 sha256 md5 crc32 sha1_headerless sha256_headerless md5_headerless crc32_headerless`.
    - **Formats:**
      - enums as lowercase names;
      - bit sets as comma-separated names, or `none`;
      - raw header fields as `0x%02x` (`0x%04x` for the cart-type word and audio);
      - sizes in decimal.
    - **Which lines print:**
      - `title` and the `a78_*` lines only for a headered file;
      - for a 2600 image, `kind`, `source`, `rom_size` and the hashes;
      - for `ERR_INVALID_IMAGE` and `ERR_TRUNCATED_IMAGE`, only the hash lines;
      - for `ERR_UNSUPPORTED_SIZE`, nothing on standard output.
  - **Messages.** A refusal writes `muhtari-run: FILE: <message>` to standard error.
    - The text lives in `host/status_text.{c,h}`, which keeps English out of the core and the ABI. The libretro
      adapter reuses it in phase 3.
    - The messages are:
      - "not a 7800 or 2600 cartridge image";
      - "the image ends before the size its header gives";
      - "larger than 1 MiB plus a 128-byte header";
      - "unsupported: 2600 (planned)", DEC.03's text;
      - "unsupported board: <name>".
  - **Exit statuses.** 0 for OK; 1 for invalid, truncated, oversized or unreadable; 77 for a 2600 image or an
    unsupported board.
    - `runner/exit_status.h` gains `{77, "refused", "the image is a 2600 cartridge or uses a board this version
      does not emulate"}`.
    - Status 1's meaning widens to "a file could not be read or written, an image is malformed, truncated or too
      large, memory ran out or the core failed".
    - `docs/runner.md` mirrors `--help`, checked by `runner.exit.table`.
  - **Why not JSON.** Key-value lines match `--bench` and the frame-hash output. JSON waits for `--capabilities` in
    phase 3, which needs an escaper for hostile titles.

### The vector sample and the nightly full set (CPU-01, CPU-02)

- **D-08:** The committed sample is `tests/vectors/6502-sample.n65v`.
  - **Format.** It uses the N65V v1 format from szTheory/nesturbator @9f9386c661f6, unchanged, and is read at test
    time. It holds 256 chunks, one per opcode 00–ff, each with the first 100 tests: 25,600 in all, about 1.68 MB (to
    be measured).
  - **Layout.** Byte-wise little-endian, with no compression and no checksum (the manifest's SHA-256 covers the
    file):
    - magic `N65V`, version 1, opcode, and a u16 test count;
    - per test, a u16 index;
    - initial and final state: pc u16; s, a, x, y and p u8; a u8 RAM count with (u16 address, u8 value) pairs;
    - a u8 cycle count with (u16 address, u8 value, u8 kind: 0 read, 1 write) triples.
  - **Manifest row** (ASSET_POLICY §2 fields):
    `tests/vectors/6502-sample.n65v  https://github.com/SingleStepTests/65x02  2f6980a2d95757486c7bee24355c360e40e2a224  MIT  <sha256>  singlesteptests`.
    The JSON fixture prefixes in D-10 get rows too.
  - **`.gitattributes`:** `*.n65v binary` and `tests/vectors/fixtures/*.json -text`.
  - **Notice.** The upstream MIT notice is kept as PROVENANCE §5 describes, in a `THIRD-PARTY-NOTICES.md` section
    named `singlesteptests`.
  - **Rejected:** embedding the sample as a C array. C17 has no `#embed`, and a 1.68 MB initializer compiles slowly
    on MSVC.

- **D-09:** The converter and readers are test code. They never enter the core, and they are not repository-check
  code under AGENTS rule 7, because they exercise the core as the harness does.
  - **Files:**
    - `tests/vectors/n65v.{c,h}`: the copied reader. It takes a buffer and a length, checks bounds before every
      field, and requires exact chunk and test counts.
    - `tests/vectors/sst_json.{c,h}`: converts SingleStepTests-shaped JSON to N65V. It accepts any whitespace and
      unsigned decimals without leading zeros, and otherwise exits naming the byte offset.
    - `tests/vectors/vecconv.c`: `vecconv IN.json OUT.n65v XX [--first N]`.
  - **Seeds.** szTheory/nesturbator @9f9386c661f6 `tests/vectors/n65v.{c,h}` and `tools/vecconv/vecconv.c`. These
    are MIT, by the same owner, with attribution per PROVENANCE §5.
  - **Rejected:** CMake `string(JSON)`. It re-parses the document on every lookup and cannot write NUL bytes.

- **D-10:** The per-PR vector tests, in `tests/cpu/tests.cmake`.
  - **Tests:**
    - `cpu.vectors.00` … `cpu.vectors.ff`, one per opcode, labels `vectors;portable`, so JUnit lists every opcode.
    - `cpu.vectors.reader` (crafted buffers).
    - `cpu.vectors.convert.a9`, `cpu.vectors.convert.02` (the compact layout), `cpu.vectors.convert.tail` (a
      truncated prefix) and `cpu.vectors.convert.reject`.
    - `cpu.vectors.self_test`: a fixture with final A flipped, run through `expect_fail.cmake` with the regex
      `a9.json\[0\]: a expected`.
    - `cpu.vectors.pin` (CMake `-P`, label `vectors`). It checks that the per-file list has 256 sorted lines, that
      its SHA-256 equals the `sst-6502-v1` pins row, and that the row's commit equals the manifest's pin.
  - **Fixtures.** Byte prefixes of 6502/v1 `a9.json` and `02.json`, in `tests/vectors/fixtures/` with a README.
  - **The harness**, `tests/cpu/test_vectors.c`, runs as `cpu_vectors FILE XX`. It:
    - reads `.n65v`, or one upstream `.json` file;
    - runs on `tests/cpu/vector_bus.c`: a flat 64 KiB RAM and a 32-entry cycle log, where overflowing the log fails
      the test;
    - sets `ane_lxa_constant` to $EE;
    - loads and compares P raw, B included;
    - clears RAM after a failed test.
  - **Output:**
    - one line per failing test with its first mismatch, such as
      `6502/a9.json[17]: cycle 3 addr expected $1234 got $1235`;
    - the first failure also prints the full expected and actual cycle tables;
    - then `6502/a9: F of N failed`;
    - exit 0, 1, or 2 for a usage error.

- **D-11:** The full set, run nightly.
  - **Pin.**
    - `pins.txt` gets the row
      `sst-6502-v1 2f6980a2d95757486c7bee24355c360e40e2a224 https://github.com/SingleStepTests/65x02.git <SHA-256 of tests/vectors/6502-v1.sha256>`,
      with a comment that `fetch-pin.sh` does not serve it.
    - `tests/vectors/6502-v1.sha256` holds 256 lines of the form `<sha256>  6502/v1/xx.json`.
    - This adjusts phase 1 D-24's "the vector archive": GitHub has no per-directory archive, and the commit's whole
      tree is 5,170,575,743 B over five CPUs (tested).
  - **Fetch.** `tests/cmake/fetch_vectors.cmake` (CMake `-P`):
    - Steps: `git init`; a cone sparse checkout of `6502/v1`;
      `fetch --depth 1 --filter=blob:none origin <sha>` with `GIT_HTTP_LOW_SPEED_LIMIT=1000` and
      `GIT_HTTP_LOW_SPEED_TIME=60`; checkout; `file(SHA256)` of all 256 files; then delete `.git`.
    - It does nothing when every file already verifies.
    - `MUHTARI_VECTORS_DIR` defaults to `${CMAKE_BINARY_DIR}/vectors-full`. A path in the source tree outside
      `build/` is refused.
    - It never runs size, log or diff commands in the blobless tree, since each would fetch blobs lazily.
    - No cache (DEC.47).
    - Rejected: 256 raw URLs. GitHub tightened its unauthenticated raw limits in 2025, and `fetch-pin.sh` sends no
      `--compressed`.
  - **Preset.** An option `MUHTARI_VECTORS_FULL`, and `vectors-full` configure, test and workflow presets that
    inherit `ci` with `MUHTARI_INVENTORY=vectors-full`. In that mode `tests/tests.cmake` includes only the full-tier
    file, the same way the hygiene lane returns early.
  - **Tests**, all labelled `vectors-full`:
    - `cpu.vectors-full.fetch`: `FIXTURES_SETUP vectors_full`, TIMEOUT 600 (provisional).
    - `cpu.vectors-full.sample_match`: regenerates the first 100 tests of each fetched file and byte-compares them
      with the committed sample. With `-DWRITE=ON` it rewrites the sample for a pin bump. TIMEOUT 120.
    - `cpu.vectors-full.00` … `cpu.vectors-full.ff`: TIMEOUT 60 each.
  - **Inventory.** `tests/inventory/vectors-full.txt` has 258 lines. A failed fetch leaves its dependent tests not
    run, and `check_junit` then fails the job. The test preset sets `noTestsAction: error`.
  - **Nightly job.** `nightly.yml` gains a `vectors-full` job on `ubuntu-26.04`.
    - `timeout-minutes: 30  # provisional`. At phase close it becomes the fetch timeout plus twice the slowest of 10
      runs.
    - Its `pull_request` paths are the vector inputs and `src/cpu.c`.
    - A red run reports to the rolling `nightly` issue.

- **D-12:** SALLY matches the pinned vectors with no waiver list.
  - **JAM opcodes** (`02 12 22 32 42 52 62 72 92 b2 d2 f2`):
    - `cpu_step` emits the vectors' 11 reads: the opcode at PC, PC+1, `$FFFF $FFFE $FFFE $FFFF`, then `$FFFF` five
      more times. PC ends at PC+1 (tested on `02.json`). It then sets `jammed`.
    - Idling until the next deadline belongs to the run loop (DEC.33).
    - The harness never steps a jammed CPU. It asserts that `jammed` is set if and only if the opcode is one of the
      twelve.
  - **SHA, SHX, SHY and TAS:** the stored value is the register ANDed with (H + 1). On a page cross, the address's
    high byte becomes that value (checked on `9f.json`).
  - **P** is loaded and compared raw, B included.
  - **ANE ($8B) and LXA ($AB)** use `ane_lxa_constant`: $EE in the vector harness, $00 by default (DEC.36).
    `cpu.unstable.sally_constant` (labels `vectors;portable`) pins the default: `LDA #$00; LDX #$FF; ANE #$FF` and
    `LDA #$00; LXA #$FF` leave $00, as three real SALLYs read (CONFORMANCE §1).

- **D-13:** Bruce Clark's decimal test.
  - **Source.** It comes from the 6502.org decimal-mode tutorial's Appendix B ("Written by Bruce Clark. This code
    is public domain."). It is transcribed into `tests/cpu/decimal/clark.s` with that line and the URL, and never
    taken from Klaus Dormann's GPL copy.
  - **Bytes.**
    - It is assembled once into `clark-6502.bin` and `clark-65c02.bin` (the listing's own `JSR A65C02`/`S65C02`
      substitution).
    - The assembler is the `dasm.wasm` in the pinned 7800basic package, under the pinned wasmtime, so no new pin is
      needed. The command and exit status are recorded.
    - The bytes get manifest rows with `LicenseRef-PublicDomain`.
    - Rebuild-and-compare joins the diagnostics lane in phase 3.
  - **Coverage.** The listing runs as printed, comparing A, C, N, V and Z over all 262,144 cases. The page's edit
    for the 6502 that compares only A and C belongs to the documented-only variant and is not applied.
  - **The harness:**
    - pushes a sentinel return address and sets PC to `TEST`;
    - runs at most 150,000,000 cycles;
    - requires `ERROR` = 0, and prints N1, N2 and C on failure.
  - **Tests.** `cpu.decimal.clark`, and `cpu.decimal.clark.self_test`, where the 65C02 build must give `ERROR` = 1.
    Both are labelled `vectors;portable`, so `ctest -L vectors` covers decimal mode twice.
  - **Klaus Dormann's functional test leaves the plan.** Its pass address is only in its GPL listing, which may not
    be opened. CONFORMANCE §1 and §8 record this. Revisit only if the nightly misses a defect that a sequence test
    would catch.

### Driving SALLY, the bus and RIOT before MARIA (CPU-03, CPU-04)

- **D-14:** How the CPU connects to the bus.
  - **One object, two buses.** `src/cpu.c` is built once as an OBJECT library, `muhtari_cpu`.
    - It goes into `muhtari` through `$<TARGET_OBJECTS:muhtari_cpu>`.
    - It also goes into the CPU test programs, which define their own
      `muhtari__bus_read(muhtari *m, uint16_t addr)` and `muhtari__bus_write(muhtari *m, uint16_t addr, uint8_t v)`.
    - So the object under test is the object that ships. The sibling does the same (szTheory/nesturbator
      @9f9386c661f6, `CMakeLists.txt`).
  - **Shared options.** One CMake function, `muhtari_core_options(target)`, applies the core's options to both
    targets:
    - hidden visibility and PIC;
    - the hardening flags phase 1 settled, and `/GS- /Zl` on MSVC;
    - nofp's `-mgeneral-regs-only`;
    - the warnings.
  - **What cpu.c reaches.**
    - Only those two functions and fields of `struct muhtari`: `profile.ane_lxa_constant`, `bus.phi2` and
      `bus.nmi_phi2`. The CPU programs do not link `muhtari`, so the linker rejects any other reference.
    - A zeroed `struct muhtari` is a valid CPU-test machine: zero means no pending NMI or HALT.
    - cpu.c provides `cpu_step` (exactly one instruction), writable raw registers and the `jammed` flag.
  - **Rejected.**
    - A template compiled twice: tests would run different machine code from what ships.
    - A bus function pointer: an indirect call every cycle, and a `call_indirect` on wasm32.
  - **A one-off check**, recorded as a command and its result. Cachegrind measures Ir per emulated cycle of a RAM
    loop through the shipped `bus.c`, against the same loop with cpu.c and bus.c in one translation unit. If the gap
    exceeds 5 %, the owner decides in phase 3 whether to ship the core as one translation unit.

- **D-15:** What `src/bus.c` owns.
  - **State:**
    - `ticks` (`uint64_t`) and `phi2`;
    - the decode and its mirrors (A7800-SYSTEM §4), including the MEN = 0 decode in which every cycle is slow;
    - the open-bus byte (C7), updated by every CPU read and write;
    - INPTCTRL (§5): LOCK, MEN, EXT and TIA-EN. Every unlocked write in the TIA range also writes INPTCTRL;
    - the HALT gate, `halt_gate_writes`, which saturates at 2. Every unlocked write cycle in the TIA chip-select
      range counts, RMW dummy writes included;
    - RDY and `next_deadline`.
  - **`halt_window()`** implements MiSTer's fitted rule, with its constants named and cited. A refit bumps the
    behaviour revision (DEC.24).
  - **Entry points for phase 3's MARIA, tested now:**
    - `muhtari__bus_request_halt(m, at_tick, until_tick)` has no effect until the gate reaches 2.
    - `muhtari__bus_raise_nmi(m, at_tick)` has no effect while MEN = 0.
    - They are functions rather than fields, so `next_deadline` stays correct.
    - `abi.symbols` already accepts defined `muhtari__*` names (`tests/cmake/abi.cmake`, tested), and hidden
      visibility keeps these out of the libretro module.
  - **RDY's phase 2 source is TIA WSYNC ($02).** `src/tia.c` lands with two things: WSYNC on the TIA's own 456-tick
    line (`tia_phase`), and the TIA read drivers. Those drive D7 or D7–D6 with idle inputs, and D5–D0 come from the
    open-bus byte.
  - **MARIA WSYNC ($24)** needs maria.c's line timeline, so it arrives in phase 3, when an `INC $24` case joins
    `ctest -L sally`.
  - **MARIA's registers ($20–$3F)** also arrive in phase 3. Until then the bus decodes the range as MARIA's, with
    fast cycles, and reads return the open-bus byte.

- **D-16:** NMI.
  - `bus.c` records `m->bus.nmi_phi2`: the φ2 count of the cycle whose φ2 latched the edge, also under RDY.
  - At an instruction's end, cpu.c takes the NMI when `nmi_phi2` falls at or before the instruction's penultimate
    cycle.
  - A taken branch without a page cross compares against its cycle 2, so it polls early.
  - BRK compares against T4. An NMI asserted during BRK's first four cycles takes $FFFA with B set in the pushed P. A
    later one gets the BRK vector, then the NMI.
  - Nothing is added per cycle.
  - Sources: A7800-SYSTEM §3 [SYS.07], and the NESdev wiki's "CPU interrupts" page (accessed 2026-10-16) for the NMOS
    polling rules.

- **D-17:** How tests observe timing.
  - **No trace in shipped code.** DEC.34 keeps hooks in v5's dual-compiled loop, and a pointer-gated trace would
    cost 2–3 Ir per cycle.
  - **Bus tests act as the CPU.** They call `muhtari__bus_read` and `muhtari__bus_write` on an instance and read
    `m->bus.ticks` around each call.
    - Each timing case also checks the value the nearest wrong model would give.
    - Failures print the expected and actual ticks.
  - **CPU rules** run on the flat bus's cycle log.
  - **Test programs** are hand-assembled `const uint8_t[]` arrays with a mnemonic comment on each line. There is no
    in-repo assembler.
  - These are the first tests to include `src/internal.h`.

- **D-18:** `ctest -L sally` (labels `sally;portable`, one case per argv).
  - **Programs:**
    - `tests/bus/tests.cmake`: executable `muhtari-test-bus`, which links `muhtari` and includes `src/`.
    - `tests/cpu/tests.cmake`: executable `muhtari-test-cpu`, the CPU object plus the flat bus.
  - **Cycle length:**
    - `bus.cycle.fast_4_ticks`: $0040, $1800, $2000, $0020 and $8000 each read and write in 4 ticks.
    - `bus.cycle.slow_6_ticks`: $0000, $001F, $0100, $0300, $0280 and $0480 take 6 ticks; $0240 takes 4.
    - `bus.cycle.men_off_all_slow`: every cycle takes 6 ticks while MEN = 0.
  - **HALT:**
    - `bus.halt.gate_two_writes`: a request after 0 or 1 writes changes no tick; after 2 it is taken. Data is
      ignored, mirrors count, and after LOCK writes stop counting.
    - `bus.halt.gate_counts_rmw_dummy`: starting from INPTCTRL $02 with a count of 0, `INC $06` from $1800 writes
      $06 then $07, leaving a count of 2 and INPTCTRL $07. Negative controls: `LDA $06` leaves 0 and `STA $06`
      leaves 1.
    - `bus.halt.latency_two_cycles`: a request at a φ2 fall gives c0 = c1 = 4 ticks and c2 = 4 + 4m.
    - `bus.halt.waits_for_slow_fall`: a request inside a 6-tick cycle starts c0 at that cycle's end.
    - `bus.halt.freezes_writes`: c2 as a write also takes 4 + 4m.
  - **RDY:**
    - `bus.rdy.holds_reads_only`: after `STA $02`, the next read waits for the TIA line's release and then runs in
      4 ticks. A write at the same point is not held.
    - `bus.rdy.rmw_completes_both_writes`: with INPTCTRL locked at $07, `INC $02` takes 4 + 4 + 6 + 6 + 6 ticks,
      both writes land, and the next opcode fetch is held.
  - **NMI:**
    - `bus.nmi.latched_in_cycle`: a request inside cycle k gives `nmi_phi2` = k, and is ignored while MEN = 0.
    - `bus.nmi.last_cycle_after_next`: an NMI raised in instruction A's last cycle pushes a return address after
      instruction B. This runs through the real bus and needs no vector at $FFFA.
    - `cpu.nmi.penultimate_taken_after` and `cpu.nmi.last_cycle_taken_after_next`.
    - `cpu.nmi.branch_no_cross_polls_early`, contrasted with a page-crossing branch and with `LDA zp`.
    - `cpu.nmi.hijacks_brk`: B is set in the pushed P, the return address is BRK + 2, and the vector is $FFFA. An
      edge after T4 gives the BRK vector, then the NMI.

- **D-19:** `ctest -L riot` (labels `riot;portable`), in `tests/riot/tests.cmake`. The executable
  `muhtari-test-riot` runs through the real bus. Timer cases run at 1T, 8T, 64T and 1024T.
  - **Timer:**
    - `riot.timer.reads_n_minus_1`.
    - `riot.timer.flag_at_n_interval_plus_1`: clear at φ2 N·I, set at N·I + 1.
    - `riot.timer.counts_past_underflow`: $FF, $FE, …, one per φ2.
    - `riot.timer.read_in_setting_cycle_keeps_flag` and `riot.timer.access_clears_flag`.
    - `riot.timer.counts_through_halt` and `riot.timer.counts_through_wsync`: the flag still sets at N·I + 1 when
      HALT or RDY fills the time.
    - `riot.timer.slow_cycle_counts_one`.
  - **`riot.pa7.edge_flag`:** PA7 is driven as an output through DDRA and ORA. The edge is falling by default and
    rising after the polarity write. Reading $285 clears the flag.
  - **Ports:** `riot.swcha.ddr` (input pins read high in phase 2) and `riot.swchb.ddr`.
  - **`riot.power_on.defaults`:**
    - DDRA = DDRB = ORA = $00, ORB = $FF, the timer at $FF on 1024T, flags clear.
    - With CTLSWB $14 and no SWCHB write, SWCHB reads $FF. Under `riot_orb_power_on` = $00 it reads $EB.
  - **`riot.ram.window`:** $0480–$04FF and the $0580 mirror, 6 ticks, zero fill.
  - **INPTCTRL and open bus**, also labelled `riot`:
    - `bus.inptctrl.lock`: later writes are ignored, and $16 acts as $06.
    - `bus.open_bus.unmapped`: `LDA $0240` returns $02, the operand's high byte; a read after a write returns the
      written byte.
    - `bus.open_bus.write_only`: reading $01 gives the TIA drivers plus the open-bus low bits, and MARIA addresses
      return the open-bus byte.

- **D-20:** The profile values phase 2 needs.
  - They live in an internal struct inside `struct muhtari`, in `src/internal.h`:
    `struct muhtari__profile { uint8_t ane_lxa_constant; uint8_t riot_orb_power_on; uint8_t ram_fill; uint16_t tia_phase; }`.
  - Defaults are $00, $FF, ZERO and 0, set by `muhtari_create`. `muhtari__power_on(m)` applies them.
  - Tests assign fields and then call `muhtari__power_on`. The vector harness sets $EE.
  - Phase 2 implements ZERO fill only. SEEDED arrives with the public profile in phase 3's load.
  - `cpu_phase` moves to phase 3, because nothing uses it before MARIA gives t an origin. This is sequencing; DEC.36
    stands.

- **D-21:** Hardware facts move into `docs/hardware/`.
  - **Pages:**
    - `docs/hardware/sally.md`;
    - `docs/hardware/bus.md`: clocks, the memory map, slow cycles, open bus, INPTCTRL, the HALT gate, and the RDY
      sources including TIA WSYNC;
    - `docs/hardware/riot.md`;
    - `docs/hardware/cartridge.md`, added when the probe areas were reconciled: the A78 fields, the probe order, the
      footer test, the hash list and the geometry table. Boards join it in phase 3.
  - **Content.** Facts in our own words with their sources: the Software Guide rev 1320, the MOS 6532 datasheet,
    MiSTer @0dc8ad2e3ff7 (facts only), NESdev and the A78 spec. Fitted or unmeasured values are marked, and no
    planning IDs appear. Code comments cite the pages by section, for example `docs/hardware/bus.md, HALT`.
  - **Budget.** `tests/cmake/budgets.cmake` adds a `docs/hardware/*.md` glob at 40,960 B, the budget of every docs
    page (ENGINEERING §11); about 10 KB each is expected. Today it globs only `docs/*.md` (tested).
  - **Preparation files.** A7800-SYSTEM §3–6 and A7800-CARTRIDGES-AND-PERIPHERALS §1 shrink to pointers plus their
    choice lines, as the preparation README says.
  - **Other docs:**
    - `docs/embedding.md` documents `muhtari_probe`;
    - `docs/runner.md` documents `--probe` and exit 77;
    - `docs/testing.md` documents the new labels, presets, nightly jobs, the fuzz target and the built sample.

### Probe evidence and the bundled tables (CPU-05)

- **D-22:** Phase 2 builds 7800basic's `simple` sample.
  - **Output.** It yields the v4-headered `.a78` (32,896 B) and the stripped `.bin` (32,768 B). The `.bin` equals the
    `.a78` without its header (tested). The remaining 34 samples arrive with phase 3.
  - **Where it builds.** On every leg that runs `ci.txt` (the six build legs, asan and be32) and in the wasm32 lane.
  - **The recipe** is our own CMake `-P` script, `tests/built/build.cmake`.
    - It runs each stage as `wasmtime run --dir=. --dir=<pkg>::/b` in a fresh `build/<preset>/built/<name>/`. A
      reused directory picks up a stale `7800.asm` (tested).
    - **Stages:**
      1. preprocess;
      2. `7800basic.wasm -i /b`;
      3. postprocess | optimize (`-O`);
      4. dasm `-I/b/includes -f3 -p20` | 7800filter;
      5. 7800sign `-w`;
      6. 7800header `-o -f a78info.cfg`.

      The bankset branch arrives in phase 3.
    - It checks every stage's exit status and then both SHA-256s, failing with `<name>: .a78 SHA-256 drift`.
    - Mapping the package to `/b` keeps host paths out of the guest, so one recipe serves every OS. 7800basic.sh's
      absolute paths break under MSYS on Windows.
    - The recipe copies no text from 7800basic.sh (GPL-2). Each pin bump compares its output once with
      `sh 7800basic.sh` on Linux, recorded as a command and exit status.
    - It reproduced the upstream bytes from a different, deeper path (tested).
  - **`tests/built/expected.txt`** (ASSET_POLICY §3 columns: name, tool pins, source, `.a78` SHA-256, `.bin` SHA-256,
    frame, audio) gets the row
    `simple  7800basic-wasm@0.41,wasmtime@48.0.6  samples/simple/simple.bas  d09da3ed27fe82e0a4dd2821ff2cb9bde26c9531c59890788acb1cfdc4004ecf  03a8b7be5d4cd122382582a4f41a94e3104c92191ecdf0a664f93e6802e46b29  -  -`.
    Phase 3 appends rows and fills in the frame and audio columns.
  - **Pins.** `pins.txt` adds these rows, with digests from the release API:
    - `7800basic-wasm 0.41 https://github.com/7800-devtools/7800basic/releases/download/v0.41/7800basic-0.41-wasm.tar.gz 62ef0a562541bff3f813c038a4609abc89ab125ebda3a371f57de2821e7a7fe7`;
    - wasmtime 48.0.6 for x86_64-macos (`cf05506b9b118a07fc8892d2acd63d59442d9aca01ea010bb64371b3bdb91c85`);
    - wasmtime 48.0.6 for x86_64-windows, a zip (`89bec9b918a20e2b09f1059e9cf49d2c2f3e66c1d3f23aa65979e5ff582b813a`);
    - wasmtime 48.0.6 for aarch64-windows, a zip (`a2d54979f5819a1e620b78fa3d5e359517e2de45cbf0fc578331d995188302c4`).

    `fetch-pin.sh` gains a `*.zip)` case that extracts with `cmake -E tar xf`, because Git Bash's tar cannot read
    zip files.
  - **`ci.yml`.** Each build leg, asan, be32 and wasm32 run, before the workflow:
    - `scripts/fetch-pin.sh wasmtime-<leg> build/pins/wasmtime`;
    - `scripts/fetch-pin.sh 7800basic-wasm build/pins/7800basic`.

    Configure requires both through `find_program(... REQUIRED)` and names the fetch command; nothing self-skips.
    nofp keeps its own inventory and does not fetch.
  - **Tests.** `built.samples` (`FIXTURES_SETUP`, labels `built;portable`), and `built.samples.self_test`: a wrong
    digest, expecting `simple: .a78 SHA-256 drift`.
  - **Built bytes** never reach artifacts, commits, releases or fuzz seeds (DEC.19).
  - **Rejected:** building once and sharing through the Actions cache (DEC.47). A cache that any pull-request run
    can restore is close to an artifact.
  - **Would change if** a Windows leg cannot reproduce the bytes. The sample tests then move to their own preset on
    the legs that can, with the run URL recorded; they never skip.

- **D-23:** Identification goes through an internal entry point that takes the tables as arguments:
  `muhtari__cart_identify(img, n, <7800 keys and entries>, <2600 prefixes>, out)`.
  - The addresses are computed in code. Bundling them in a const struct of pointers is ruled out: it lands in
    `.data.rel.ro` under PIC and fails `abi.globals` (DEC.31).
  - `muhtari_probe` passes the bundled tables. Tests pass `tests/cart/test_cartdb.c`, which also exercises slices,
    the Activision swap and 2600 hits with synthetic bytes.
  - **Tests:**
    - `cart.identify.test_list`: the sample's headerless SHA-1, `1aa9559b74eab35d4c5443b56bc4dfd7a445ac4a`
      (tested; in neither MAME list), resolves to linear, NTSC, source hash.
    - `cart.identify.bundled_has_no_test_entries`: no SHA-1 from the test list appears in `src/cartdb.c`.
  - **A test-only runner (reconciled).** `muhtari-run-testdb` links the core with `tests/cart/test_cartdb.c` in
    place of `src/cartdb.c`. This is link-time substitution, as D-14 uses for the CPU, and the shipped runner never
    contains a test entry.
    - `runner.probe.sample_hash_match`: `muhtari-run-testdb --probe` on the stripped sample prints `source hash`,
      `board linear` and `region ntsc`, end to end.
    - `runner.probe.sample_headerless`: the shipped runner identifies the same file by its footer (`$FFF8 = $FF`,
      `$FFF9 = $F7`, reset `$F000`; tested).
    - `runner.probe.sample_a78`: the shipped runner on the headered file (v4, linear, NTSC, ProLine on both ports),
      with all eight hashes.
  - **Rejected:** a runner option `--hash-list FILE`. It would add a new parser and a new fuzz surface.

- **D-24:** The CC0 tables.
  - **Generator.**
    - `tools/cartdb.cmake` (CMake `-P`) generates `src/cartdb.c`, which holds data only and is committed. It reads
      MAME `hash/a7800.xml` and `hash/a2600.xml` at dddf4cd1faeeca6a6dac26ecbc2da2da4242b740.
    - The types live in a hand-written `src/cartdb.h`.
    - The generated file opens with
      `/* SPDX-License-Identifier: MIT AND CC0-1.0 — generated by tools/cartdb.cmake from MAME hash/a7800.xml and hash/a2600.xml at dddf4cd1faeeca6a6dac26ecbc2da2da4242b740; do not edit */`.
    - The XML never enters the tree (DEC.20 bundles only the 2600 SHA-1s).
  - **Layout.** Everything is const and pointer-free (DEC.31, `abi.globals`). Each key row sits on one line in a
    fixed `{{0x..,…}` form, so the firmware scan can read it.
    - `cartdb_7800_keys`: `{uint8_t sha1[20]; uint8_t entry; uint8_t part;}` × 214, sorted by (sha1, entry);
      4,708 B.
    - `cartdb_7800_entries` × 204: board, board options, RAM, sound, region and light-gun flags, and part count.
      There are no names, reconciled with D-02: the public struct carries no database name, so names would be dead
      bytes in every core. One can be added later with an API version.
    - `cartdb_7800_layouts`: the distinct slice-size tuples of the 11 multi-chip entries.
    - `cartdb_2600_prefix`: `uint8_t[1601][8]`, sorted; 12,808 B.
      - Eight-byte SHA-1 prefixes keep the list at 12.8 KB instead of 32 KB. A false hit only refuses an image, and
        forging one needs a 2⁶⁴ second preimage.
      - The 7800 keys stay full width, because a match overrides the header.
    - In all, about 19 KB of read-only data in the core. The 2600 list stays in the core, so the library, the
      runner and the libretro core classify an image the same way.
  - **Slices and swaps.** Slices and the Activision half-swap are computed at run time, not generated. SHA-1 does
    not compose, so a whole-image key cannot be derived from per-chip hashes. This is new evidence, and
    A7800-CARTRIDGES-AND-PERIPHERALS §1.4's build-time wording is updated.
  - **Entries that are not cartridges** stay in the table, so probe calls such a file not a cartridge image: the HSC
    ROM (listed as `hiscore` and inside `xm`), and any XM or XBoard entries the generator finds. The one duplicate
    key, the HSC ROM, resolves to its first entry.
  - **Every field is used in phase 2** (D-03): board, options, RAM and sound in the override, plus region and the
    light-gun flag. Phase 3 does not regenerate the table.
  - **Regeneration check.** `policy.cartdb_generated` and its self test join the hygiene inventory; the self test
    changes one byte and expects `src/cartdb.c differs from the generator output`. The hygiene job fetches both XML
    pins first.
  - **Pins.** `pins.txt` rows:
    - `mame-a7800-xml dddf4cd1faee https://raw.githubusercontent.com/mamedev/mame/dddf4cd1faeeca6a6dac26ecbc2da2da4242b740/hash/a7800.xml 5343714eedd715f7f1f080f1e283d5ed56e6f6509ea3eff2f8f7c8c53ee066b9`;
    - `mame-a2600-xml dddf4cd1faee https://raw.githubusercontent.com/mamedev/mame/dddf4cd1faeeca6a6dac26ecbc2da2da4242b740/hash/a2600.xml efe03008c6c2a4aa197eac1242d838913b3ec4eb434d04f2b59bb769a726197c`.
  - **Notice.** `THIRD-PARTY-NOTICES.md` gets a "MAME software lists" section naming `src/cartdb.c`, with the two
    courtesy lines PROVENANCE §7 asks for.

- **D-25:** `tests/cart/images.c` builds every failure class in memory.
  - It is shared by the cart tests, the allocator test, the fuzz seeds, and `muhtari-test-images DIR`, which writes
    the images into the build directory for the runner tests.
  - No image bytes are tracked, so hygiene's A78 rule holds.

  | Class | Image | Expected |
  |---|---|---|
  | 2600 | 4 KiB and 8 KiB, reset $F000, failing footer; plus a 2600 hit in the test list | `ERR_UNSUPPORTED_2600` |
  | Unsupported board | v4 header with mapper 4 (Souper) and cart-type bit 12; it stays unsupported until v4, so phase 3 cannot flip it | `ERR_UNSUPPORTED_BOARD` |
  | Malformed | magic with a size field of 0; bad magic in a 1,000-byte file | `ERR_INVALID_IMAGE` |
  | Truncated | a header-only file with a non-zero size field; a SuperGame header claiming 128 KiB over a 40 KiB body | `ERR_TRUNCATED_IMAGE` |
  | Oversized | cap + 1 = 1,048,705 B | `ERR_UNSUPPORTED_SIZE` |
  | Accepted with warnings | an invalid v4 block (`HEADER_V4_IGNORED`); a size field 128 above a valid linear body (`SIZE_FIELD_MISMATCH`); one header per other warning bit, with a clean header as the negative control | OK |

  Tests:
  - `cart.probe.<class>` and `cart.probe.warnings` (labels `cart;portable`);
  - `runner.probe.<class>`: exit status and standard-error message on the written files.

- **D-26:** The fuzz target is `loader`, the name CPU-05 uses; ENGINEERING §3's `fuzz.a78` is renamed.
  - **`tests/fuzz/loader.c`** (`LLVMFuzzerTestOneInput`):
    1. Probe twice and compare the results (purity).
    2. Compare probe's status with the internal load's.
    3. Load through a counting allocator. A refusal allocates nothing and leaves the instance unchanged.
    4. Destroy.
    5. `abort()` on any broken invariant.
  - **Other files.** `tests/fuzz/replay_main.h` is seeded from szTheory/nesturbator @9f9386c661f6 (MIT, same owner).
    `tests/fuzz/seeds.c` wraps `images.c`.
  - **CTest.** `fuzz.regress.loader` (labels `fuzz;portable`, every platform) runs `muhtari-fuzz-loader-replay`
    over the generated seeds and the committed regressions. `fuzz.regress.loader.self_test` expects
    `no corpus inputs`.
  - **Nightly job `loader-fuzz`** on ubuntu-26.04 with apt clang-21:
    - ASan and UBSan with `-fno-sanitize-recover=all`;
    - flags `-max_total_time=60 -timeout=2 -max_len=1048705 -artifact_prefix=crash/`;
    - corpus directories: a writable new-corpus directory first (never the tree), then the seeds and
      `tests/fuzz/loader/regress`;
    - crashes are uploaded, since they derive only from synthetic seeds;
    - it also runs on pull requests that touch `tests/fuzz/**` or the cart sources;
    - its timeout is provisional.
  - **Why max_len is cap + 1.** libFuzzer cuts every seed longer than max_len, so a max_len of 65,664 would never
    fuzz SuperGame geometry, slices, the swap or the oversize check. This is new evidence, and ENGINEERING §3 is
    updated.
  - **Committed regressions** are `tests/fuzz/loader/regress/<sha1-12>`, each with a manifest row
    `path  this-repo  <commit fuzzed>  MIT  <sha256>  -`.
    - The row exempts the file from hygiene's name, A78-magic and NUL rules.
    - `tests/fuzz/**` stays at or under 512 KiB.
  - **Manifest.** `tests/roms/manifest.txt` is created with its format header. `policy.rom_hash_coverage` and its
    self test (ASSET_POLICY §5) check that:
    - every row's file exists and matches its SHA-256;
    - every file under `tests/fuzz/**/regress` and `tests/roms/**` has a row;
    - the size cap holds.

    The self test names the path that has a wrong SHA-256.

- **D-27:** `cart.load.no_alloc_on_reject` (labels `cart;portable`) reuses the counting allocator in
  `tests/core/test_core.c`. For each refusal class in D-25:
  - the allocation and free counts after the load equal those after `muhtari_create`;
  - the status is that class's own;
  - the instance is byte-identical;
  - with an allocator that fails every call, the status is still that class's, never `ERR_NO_MEMORY`. This shows
    that validation runs before allocation.

  Positive control: an accepted linear image makes exactly one allocation, of `rom_size` bytes.

### Folded todo: the release firmware scan

- **D-28:** `release.firmware_scan` arrives with this first ROM-handling code.
  - **The script**, `tests/cmake/firmware_scan.cmake` (CMake `-P`), hashes every member of every release zip with
    SHA-1 and SHA-256. It extracts members with `cmake -E tar xf`, and hashes each whole, and again without the
    first 128 bytes when `ATARI7800` sits at offset 1.
  - **What it knows:**
    - `tests/release/firmware.txt`: the ASSET_POLICY §4 BIOS and HSC digests, the Supercharger BIOS, and the
      OpenBIOS builds with $FF and $00 fill;
    - the SHA-256s in `tests/built/expected.txt`;
    - the 7800 SHA-1s and 2600 prefixes in `src/cartdb.c`.
  - **Registrations:**
    - `release.firmware_scan` and its self test, in `ci.txt` with label `release`. The self test zips a synthetic
      A78 file, passes a list holding its header-stripped SHA-1, and expects `matches synthetic (header stripped)`.
    - The same script with `-DTRACKED=1`, as `hygiene.firmware_scan` over tracked files, in the hygiene inventory.
  - **Release.** `publish.sh prepare` runs it over the 18 zips before writing `SHA256SUMS`, so the release rehearsal
    exercises it.
  - ASSET_POLICY §5 drops "(untested)" for the scan, and the todo moves to `done/`.

### Reconciled across areas

- **D-29:** Test layout.
  - **Registration files.** `tests/{cpu,bus,riot,cart,built,fuzz}/tests.cmake` join the existing ones. ENGINEERING §3
    asks for one per subsystem, each at most 400 lines.
  - **Names** take the form `<subsystem>.<behaviour>`. The API research's `core.probe.*` names became `cart.*`.
  - **Requirement labels:**
    - `vectors` for CPU-01: `cpu.vectors.*`, `cpu.decimal.*` and `cpu.unstable.*`;
    - `vectors-full` for CPU-02;
    - `sally` for CPU-03;
    - `riot` for CPU-04: `riot.*`, `bus.inptctrl.lock` and `bus.open_bus.*`;
    - `portable` on everything that be32 and wasm32 run.
  - **Inventories.**
    - Every name enters `tests/inventory/ci.txt` and `wasm32.txt`.
    - `vectors-full.txt` is new.
    - The hygiene inventory gains `policy.cartdb_generated`, `policy.rom_hash_coverage` and `hygiene.firmware_scan`,
      with their self tests.
    - A test that cannot run everywhere belongs to another preset (ENGINEERING §3).
  - **be32** gains about 300 processes under qemu. Measure it against the 5-minute per-PR budget (ENGINEERING §9)
    before ship. If it is over budget, be32 moves to nightly per DEC.46 rather than losing tests.

- **D-30:** Reference fixes in the phase's pull request. The preparation README's rule is that a phase which finds a
  file wrong fixes it and its `updated` date in the same pull request.
  - ARCHITECTURE §3 cites the CPU seed at `3200aa868371`, and A7800-SYSTEM [SYS.31] at `e4c75a059d4d`. Both become
    szTheory/nesturbator @9f9386c661f6, which PROVENANCE §4 pins: the published main, as settled in phase 1.
  - ENGINEERING §3: target `loader`, `fuzz.regress.loader`, `-max_len=1048705`.
  - CONFORMANCE §1 and §8: Klaus Dormann's test is dropped, with the reason.
  - ASSET_POLICY:
    - §3: 7800basic tag v0.41 `03aebb85206f53046425c19a236f65ae80d9f498` with its bundled `dasm.wasm` replaces
      `e7c18d2ecdc6` with a separate dasm.
    - §5: as in D-28.
  - A7800-CARTRIDGES-AND-PERIPHERALS §1.4: slice and swap keys are computed at run time.
  - Phase 1 D-24's "the vector archive" is the `sst-6502-v1` row (D-11).

- **D-31:** Every TIMEOUT and `timeout-minutes` above is provisional until measured: twice the slowest of 10 or more
  runs, with the run ID (DEC.47). `workflow.test_gate_timeout` (3) is re-measured once the CPU tests are in the suite.

### Claude's Discretion

- Internal names and layouts beyond those named above: the CPU register struct, the bus fields, the deadline
  mechanism, and the cartridge state inside `struct muhtari`.
- Whether `muhtari_create` or the internal load calls `muhtari__power_on`.
- How the core's OBJECT libraries are split for D-14 and D-23: for example `muhtari_cpu`, plus one OBJECT library
  for everything except `cpu.c` and `cartdb.c`.
- Linear's smallest size and step in the geometry table (D-05).
- Whether `title` keeps the trailing spaces some tools write.
- The wording of `--help` and the docs, within D-07's messages.
- How the one-off Cachegrind check in D-14 is run.

### Folded Todos

- **Add the release firmware scan with the first ROM code.**
  - File: `.planning/todos/pending/2026-10-16-add-the-release-firmware-scan-with-the-first-rom-code.md` (area
    release).
  - Problem: `release.firmware_scan` from ASSET_POLICY §5 waited for the first ROM-handling code or fixture.
  - Phase 2 adds both: the probe and loader, the vector and decimal-test fixtures, and the built sample. The scan
    therefore lands here, as D-28.

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**

### Phase scope
- `.planning/ROADMAP.md` (Phase 2): the goal and the five success criteria.
- `.planning/REQUIREMENTS.md`: CPU-01 to CPU-05, and the preamble's definition of "every platform".
- `.planning/PROJECT.md`: constraints (determinism, efficiency, legal, verification, delivery).
- `.planning/preparation/DECISIONS.md`: DEC.03, 19, 20, 22, 24, 31, 33, 34, 36, 38, 39, 43, 44, 46, 47, 53 and 57,
  and the open questions.
- `.planning/phases/01-a-test-frame-in-retroarch/01-CONTEXT.md`:
  - D-07 to D-12: header growth, size tags, statuses and the abi checks;
  - D-24: pins;
  - D-25: CI gating;
  - D-27: a copy beats a dependency.

### Hardware and design
- `.planning/preparation/A7800-SYSTEM.md`:
  - §1: ticks;
  - §3: SALLY, HALT, RDY, NMI and slow cycles (C4–C6);
  - §4: the memory map, open bus (C7) and $2800 (C8);
  - §5: INPTCTRL;
  - §6: RIOT (C9);
  - §8: the profile.
- `.planning/preparation/ARCHITECTURE.md`:
  - §2: time, the dispatcher, `halt_window()`, RDY and NMI;
  - §3: modules;
  - §4: API rules and version numbers;
  - §6: determinism, profile fields and hostile-input bounds;
  - §10: seams;
  - §11: build order;
  - §13: open conflicts.
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md`:
  - §1: the A78 header, parsing policy, headerless images and hashes, 2600 images and bad headers;
  - §2.1–2.3: boards;
  - §7: hostile input.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md`:
  - §2: RetroAchievements hashing;
  - §4: runner options, exit statuses and probe hashes.

### Conformance, engineering and CI
- `.planning/preparation/CONFORMANCE.md`:
  - §1: the test-ROM inventory, the $EE constant and the sample;
  - §2(a): 7800basic reproducibility;
  - §6: CI tiers, and measuring before gating;
  - §7: admitting fixtures;
  - §8.
- `.planning/preparation/ENGINEERING.md`:
  - §2: C rules and the abi checks;
  - §3: tests, the inventory, negative controls and fuzzing;
  - §4: CI design;
  - §8: the Ir gate and zero allocations;
  - §9: be32;
  - §10–11: style and docs budgets.
- `.planning/preparation/GSD-HANDOFF.md` §3: the per-phase routine.

### Rules and provenance
- `AGENTS.md`: rules 1–11 and Style.
- `PROVENANCE.md`:
  - §4: the CPU seed;
  - §5: notices;
  - §7: the hash list and test vectors.
- `ASSET_POLICY.md` §2–5: manifest fields, built fixtures, firmware digests and checks.
- `.planning/todos/pending/2026-10-16-add-the-release-firmware-scan-with-the-first-rom-code.md`: the folded todo.

### Code
- `include/muhtari.h`, `src/internal.h`, `src/instance.c` (`muhtari__check_size`, `write_out`), `src/frame.c`
- `runner/main.c`, `runner/exit_status.h`, `runner/sha256.{c,h}`, `runner/crc32.{c,h}`, `host/`
- `CMakeLists.txt`, `CMakePresets.json`, `cmake/MuhtariTests.cmake`, `tests/tests.cmake`, `tests/inventory/*`
- `tests/cmake/{abi,budgets,check_junit,expect_fail,pins}.cmake`, `tests/core/test_core.c` (the counting
  allocator), `tests/api/v1.c`
- `pins.txt`, `scripts/fetch-pin.sh`, `scripts/hygiene.sh` (`in_manifest`, the A78 rule), `scripts/publish.sh`
  (`prepare`)
- `.github/workflows/ci.yml`, `nightly.yml`, `release.yml`
- `docs/embedding.md`, `docs/runner.md`, `docs/testing.md`

### External sources (cite by name and pin)
- **szTheory/nesturbator @9f9386c661f6** (MIT, same owner). Its footguns: libFuzzer writing into the committed
  corpus, and a 9 MB committed corpus.
  - `src/cpu.c`, `src/bus.c`, `CMakeLists.txt` and `tests/CMakeLists.txt`.
  - `tests/vectors/n65v.{c,h}` and `tools/vecconv/vecconv.c`.
  - `tests/cpu/{test_vectors.c,vector_bus.c,vector_bus.h,test_cpu_unit.c,test_bus.c}`.
  - `tests/cmake/{fetch_vectors,vectors_sample_match,vectors_full_run}.cmake`.
  - `tests/fuzz/{replay_main.h,rom_loader.c}`, `tests/roms/manifest.txt` and `.github/workflows/nightly.yml`.
  - `.planning/milestones/v1-phases/02-the-cpu-matches-the-public-vectors/02-CONTEXT.md`.
- **szTheory/gabbaboy @a175a39**: `include/gabbaboy/gabbaboy.h` (`gbb_probe_rom`, a pure probe that returns the
  load's verdict).
- **SingleStepTests/65x02 @2f6980a2d95757486c7bee24355c360e40e2a224** (MIT): `6502/README.md`, `LICENSE` and
  `6502/v1/`.
- **rcheevos @1433173220a7eaede6a9ed7a18e94117be1821e0** (MIT): `src/rhash/hash_rom.c` and `src/rhash/hash.c`.
- **MAME** `hash/a7800.xml` and `hash/a2600.xml` @dddf4cd1faeeca6a6dac26ecbc2da2da4242b740 (CC0).
- **7800basic v0.41 @03aebb85206f53046425c19a236f65ae80d9f498**, asset `7800basic-0.41-wasm.tar.gz`. Its compiler
  is GPL-2, used only as a tool.
- **wasmtime v48.0.6** release assets.
- **Hardware and algorithm references:**
  - the MOS 6532 datasheet (http://archive.6502.org/datasheets/mos_6532_riot.pdf);
  - the 6502.org decimal-mode tutorial, Appendix B (http://www.6502.org/tutorials/decimal_mode.html);
  - the NESdev wiki, "CPU interrupts";
  - MiSTer Atari7800 @0dc8ad2e3ff7 (`sally.sv`, `mos6502_ctl.sv`), for facts only;
  - FIPS 180-4 and RFC 1321.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `muhtari__check_size` and `write_out` in `src/instance.c` handle size tags and partial output writes for
  `muhtari_cart_info`.
- The counting allocator in `tests/core/test_core.c`.
- `runner/sha256.c` and `runner/crc32.c`, both nesturbator seeds, for the printed hashes.
- `pins.txt`, `scripts/fetch-pin.sh` and `tests/cmake/pins.cmake` for pinned downloads.
- `tests/cmake/expect_fail.cmake` for every self test; `tests/cmake/check_junit.cmake` for the exact inventory.
- `host/` (PIC, hidden visibility) for `status_text`.
- `tests/cmake/abi.cmake` already accepts defined `muhtari__*` names; `tests/cmake/budgets.cmake` holds the docs
  budget.
- `scripts/hygiene.sh`'s `in_manifest` reads the first field of each manifest row.

### Established Patterns
- Size-tagged structs with `_Static_assert` layouts; append-only statuses; validate, then apply.
- An exact inventory per preset, negative controls and no self-skip.
- CMake `-P` or POSIX sh checks that name the fault they prevent.
- Tables mirrored in the docs and checked (`runner.exit.table`).
- Evidence is a command, an exit status and a run URL.
- New in phase 2:
  - link-time substitution: the CPU object with a flat bus, and the test hash list with a test runner;
  - the first tests that include `src/internal.h`.

### Integration Points
- `CMakeLists.txt`: the core target's options become `muhtari_core_options(target)`.
- `ci.yml`: the legs gain the wasmtime and 7800basic fetch steps.
- `nightly.yml`: gains `vectors-full` and `loader-fuzz`.
- `publish.sh prepare` and the hygiene job gain the firmware scan; the hygiene job also gains the XML fetch.
- `THIRD-PARTY-NOTICES.md`: gains the `singlesteptests` and "MAME software lists" sections.
- `tests/api/v2.c` and `.gitattributes`.

</code_context>

<specifics>
## Specific Ideas

- **The owner's brief** (GSD-HANDOFF §3, restated in this discussion): research every open area through all lenses,
  with primary sources and an adversarial pass, and follow one coherent recommendation per point without asking
  again. Only the public API went to the owner, who chose "Probe now, load in phase 3".
- **Tested during research (2026-10-16):**
  - **7800basic `simple`** under wasmtime 48.0.6 on aarch64-macos:
    - `.a78` `d09da3ed…4ecf` and `.bin` `03a8b7be…6b29`, built in 0.23 s;
    - our own recipe gives the same bytes from a deeper path;
    - a reused directory picks up a stale `7800.asm`.
  - **The stripped sample:**
    - SHA-1 `1aa9559b74eab35d4c5443b56bc4dfd7a445ac4a`, MD5 `948e2565fefcbb635cd8b73ea9b29941`, CRC-32 `55f0312b`;
    - it is in neither MAME list;
    - its header: version 4, magic and title padded with spaces, size field $8000, cart type 0, ports 1/1, the v4
      block all zero.
  - **wasmtime 48.0.6** publishes x86_64-macos, x86_64-windows (zip) and aarch64-windows (zip) assets.
  - **MAME lists:**
    - a7800.xml: 204 entries, 216 ROMs with a size, 214 SHA-1s (213 unique), 11 multi-chip entries;
    - a2600.xml: SHA-256 `efe03008…197c`, 1,601 SHA-1s, none shared with a7800.xml.
  - **SingleStepTests 6502/v1:**
    - 256 files, 1,081,886,574 B;
    - the pin is upstream HEAD (2025-05-10);
    - the JAM read pattern, the SHA formula and raw P were confirmed;
    - both JSON layouts occur: compact `[{` in 02, spaced in 69, 8b and a9;
    - the 100th test ends by 54,474 B in the four files measured.
  - **rcheevos's 7800 rule** (`hash_rom.c` L21–30) and its 64 MiB cap.
  - **The repository:** `abi.symbols` accepts `muhtari__*`; `docs.budgets` globs only `docs/*.md`; no test includes
    `src/internal.h` yet.
- **Facts to verify while planning** (untested or unverified):
  - **7800basic builds:**
    - identical bytes on Windows x64 and arm64 and on macOS x64, including how wasmtime parses `D:\…::/b`;
    - the cold-runner cost per leg, expected at about 13 MB and 1–5 s.
  - **Vectors, generator and tests:**
    - the generator's size and its run time on a2600.xml;
    - the 100th test's offset across all 256 files, the sample's size (about 1.68 MB), and which files use which
      JSON layout;
    - the git transfer (about 190 MB and 90 s);
    - CTest JUnit marking fixture-dependent tests as not run;
    - be32 staying within 5 minutes with the new tests;
    - the decimal test's run time, and dasm accepting the listing.
  - **Linking:**
    - test programs linking hidden `muhtari__` internals from the archive on all six platforms (MSVC included) and
      on wasm32;
    - the OBJECT library's options on the nofp lane;
    - the call overhead (D-14).
  - **Hardware behaviour:**
    - `INC $06` writing $06 then $07 with phase 2's TIA read drivers;
    - the TIA WSYNC release tick relative to `tia_phase`;
    - the 6532's rule for a read in the setting cycle, and PA7 on an output pin (re-read the datasheet);
    - the HALT latency, which is fitted, not measured.
  - **Cartridges and release:**
    - that no real header counts its own 128 bytes in the size field over a body that fails geometry (scan the
      PROPack headers);
    - that XM-flagged homebrew boots without the XM;
    - that `publish.sh prepare` leaves the zips in a state the scan can read.
- `cpu_phase`'s default, MiSTer's hand-off phase, is extracted by RTL simulation in phase 3 (ARCHITECTURE §6).

</specifics>

<deferred>
## Deferred Ideas

- Public `muhtari_load` and `muhtari_media`, with BIOS, region, ports and the profile (phase 3).
- MARIA WSYNC at $24 with an `INC $24` case, MSTAT, and `cpu_phase` (phase 3).
- A database name or a digest field in `muhtari_cart_info`, added with an API version when a host asks.
- A public status-string function, if outside hosts ask.
- `ram_fill` SEEDED, with the public profile.
- JSON output for `--probe`, together with `--capabilities` (phase 3).
- The remaining 34 7800basic samples (phase 3).
- Rebuild-and-compare for the decimal test's binaries (phase 3's diagnostics lane).
- Klaus Dormann's functional test, only if the nightly misses a defect at the level of instruction sequences.
- A runner option `--hash-list FILE`, unless a user needs one.
- Shipping the core as one translation unit, only if D-14's one-off check shows more than 5 %; the owner decides.

### Reviewed Todos (not folded)
- **Arm the instructions-per-frame gate** (ci). It needs two control runs after phase 1's merge, and gets its own
  `ci:` pull request.
- **Close the re-review's findings on the Dependabot gate and the publish checks** (ci). It gets its own `ci:` pull
  request after phase 1's merge. It edits `scripts/publish.sh`, as D-28 does, so whichever lands second rebases.
- **Limit release rehearsals to pushes that change release inputs** (ci). Revisit if rehearsals become a nuisance.
  D-28 touches `publish.sh`, so phase 2's pull request will rehearse.
- **Promote the wasm32 lane into CI required** (ci). Due after 10 green `wasm32` runs on `main`. Phase 2's tests join
  the lane's inventory either way.

</deferred>

---

*Phase: 02-the-cpu-matches-the-public-vectors*
*Context gathered: 2026-10-16*
