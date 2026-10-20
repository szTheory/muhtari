# Emulation tests

The tests that check the emulated hardware, one section per CTest label. Each says what its cases cover and where
their expected values come from. Building, running and the other checks are in `docs/testing.md`.

Run one label with `ctest --test-dir build/ci -L <label> --output-on-failure`.

## maria

`maria.dma_line.*` runs MARIA's DMA burst with no CPU. Each case writes a DLL and display lists into system RAM, sets
MARIA's registers and calls the burst directly, then checks the HALTRST tick, the line's statistics, the line-RAM
cells and the feature mask. Every expected value is worked out from the constants in `docs/hardware/maria.md` before
the case first ran, with the arithmetic in a comment beside it.

| Case | Covers |
|------|--------|
| `empty_dl` | a terminator-only DL: T0 + 9 on an ordinary line, T0 + 15 on a last line, and no cell written |
| `headers` | a 4-byte and a 5-byte header with one graphics byte: 8 and 10 ticks |
| `widths` | widths 1, 8, 31 and 32 at 3 ticks a byte |
| `indirect` | 1-byte and 2-byte characters at 6 and 9 ticks, and the cells each writes |
| `holes` | H16 and H8 holes, the OFFSET-added test, a hole mid-object, and the indirect hole slot |
| `terminators` | `(b1 & 0x5F) == 0` ends the list; $40, $60, $C0 and $E0 are 5-byte headers that load WM |
| `last_line` | 6 more ticks on a zone's last line, the next entry read and its DLI |
| `abort` | an overrun dropped at LRC: the +3 and +9 tails, the objects kept and the partial one's cells |
| `line_ram` | the cells for 160A, 160B, 320A, 320B, 320C and 320D, kangaroo mode, hpos wrap and WM inheritance |
| `stats` | cost, bytes, headers and objects for a mixed DL |
| `feature_mask` | each feature bit sets on the first line that uses it and stays set |

## frame

`frame.*` runs whole frames through the run loop with SALLY, the bus and MARIA together. Each case but
`frame.sample_renders` builds a 48 KiB linear cartridge in memory from a few bytes of its own 6502 code, loads it
through the internal load and the fast-boot hand-off, and either runs frames or acts as the CPU on the bus to reach
an exact tick. Expected ticks come
from the beam in `docs/hardware/bus.md` and the line events in `docs/hardware/maria.md`.

| Case | Covers |
|------|--------|
| `frame.jam_ends` | a jammed CPU still ends every frame at VBLANK start, within one 4-tick read |
| `frame.dma_off_ends` | with DMA off every line 17-258 says so and every frame ends on time |
| `frame.hold_split_wsync_across_dma` | a TIA WSYNC held across t 28 and t 42 still runs the line's burst and moves MSTAT |
| `frame.halt_c1_write_visible` | a store in the HALT window's c1 is fetched by that line's burst; one in c2 is not |
| `frame.maria_wsync_releases_at_lrc` | MARIA's WSYNC holds until the next t 0, wire-ANDed with the TIA's |
| `frame.mstat_edges` | MSTAT's VBLANK sets at line 258, t 42 and clears at line 16, t 42 |
| `frame.handoff_state` | the hand-off's CPU, INPTCTRL, MARIA and beam state |
| `frame.first_frame_partial` | frame 1 is 8,190 ticks and 54 samples, with rows 0-223 written $00 |
| `frame.dma_blocked_flag` | a refused HALT request marks the line DMA blocked, at zero cost |
| `frame.dli_queued` | a DLI on zone 1's entry enters the NMI handler on line 32, zone 0's last, and the line says the edge was queued |
| `frame.null_video_same_state` | ten frames with and without video leave the same line RAM, statistics, CPU and RAM |
| `frame.sample_renders` | 7800basic's `simple` sample, built by `built.simple`: two instances give the same 120 frames, and frame 120 holds more than one colour and is not the test card |

`frame.sample_renders` carries the `built` label, because it needs the sample the pinned 7800basic builds.

## input

`input.*` checks what a game reads from the controllers and the console switches. Each case builds a 48 KiB linear
cartridge in memory from its own 6502 code and loads it through `muhtari_load`. Every frame the program waits for
MSTAT's VBLANK, reads SWCHA, SWCHB and INPT0-INPT5 once into a ring in RAM, and then paints each byte as its own band of
28 lines, so the picture shows every value read. The input comes from rows written as `muhtari-movie 1` text and read by
the runner's own `runner/movie.c`, so the path from a movie's spelling to the register is the one the runner takes.
Every expected byte comes from `docs/hardware/controllers.md` and the cartridge notes it cites, section 3.1 to section
3.3, in a comment beside the row.

| Case | Covers |
|------|--------|
| `input.directions` | each direction on each port clears its own SWCHA bit and changes the frame's picture |
| `input.proline_two_button` | buttons 1 and 2 on INPT0-INPT3 with INPT4 and INPT5 at 1; one port in each mode |
| `input.proline_one_button` | either button pulls INPT4 or INPT5 to 0 |
| `input.joystick_fire` | a 2600 stick's fire on INPT4 or INPT5 in both modes, and the `io_hazard` count in two-button mode |
| `input.ctlswb_no_swchb_write` | CTLSWB $14 with no SWCHB write stays one-button, since ORB powers on $FF |
| `input.tia_en_disables_two_button` | INPTCTRL's TIA-EN keeps both ports one-button with PB2 and PB4 driven 0 |
| `input.switches` | Reset, Select, Pause and both difficulty switches on SWCHB |
| `input.unplugged` | an unplugged port reads released whatever the host presses |
| `input.pa7_edge` | a Right press on port 0 sets PA7's flag at the VBLANK start that applies it, and its release does not |
| `input.lag_even_frames` | a program reading on even frames only gives 30 lag frames in 60 |
| `input.lag_dma_and_peek` | DMA fetching from $0280 and a peek of SWCHA or INPT4 leave every frame a lag frame |

## basic

`basic.<name>` plays one 7800basic sample for 600 frames and checks them against a pin. There is one test for each
row of `tests/built/expected.txt`, and the sample comes from that row's `built.<name>` fixture. The ROM is loaded with
`muhtari_load` and fast-booted, with no input. `muhtari-test-chain` (`tests/built/chain.c`) hashes every frame into
a chain:

    h_0 = 32 zero bytes
    h_i = SHA-256(h_(i-1) || frame_i), i = 1 .. 600

Here frame_i is frame i's 320 x 242 native colour bytes. Frame 1 is the partial frame after the hand-off. Because the
chain covers frame order, two swapped frames fail as surely as one changed frame. `tests/built/frames.cmake`
compares h_600 with the row's sixth column, `frame`.

The file holds a `# behaviour_revision N` line. Each check fails unless N equals the library's behaviour revision,
so a pin holds only at the revision it was written under. `basic.revision.self_test` gives the check a copy whose
line says 0 and expects it to fail, naming the behaviour revision.

When a check fails with `frame chain drift`, find the first frame that differs. Run
`muhtari-run --frames 600 --hash-frame N FILE` on the old and new cores and bisect on N.

### Writing a pin

Pins are only ever written by the core under test, never by hand:

    cmake -DCHAIN=build/ci/tests/muhtari-test-chain -DROM=build/ci/built/<name>/<name>.bas.a78 \
          -DNAME=<name> -DEXPECTED=tests/built/expected.txt -DWRITE=ON -P tests/built/frames.cmake

`-DWRITE=ON` fills a `-` column. If the column already holds a pin at the file's revision, it refuses
(`refusing to rewrite a pin at behaviour revision N`), unless the core gives the same chain.

An intended change that moves a pin is handled in one of two ways:

- **Unreleased pin.** A pin is unreleased while `git show origin/main:tests/built/expected.txt` has `-` or no row in
  its place. Reset the column to `-`, run `-DWRITE=ON`, and record the row, the cause and the old and new chains.
  The revision does not change.
- **Released pin.** Bump the library's behaviour revision first. `-DWRITE=ON` then sees a core revision above the
  file's: it rewrites the row and the revision line and resets every other row's pin to `-`. Run it once more for
  each of those rows.

A drift with no intended cause is a bug to fix, never a new pin.

### coverage

`basic.coverage` shows that the samples and our diagnostic ROMs together exercise every board and MARIA feature the
emulator claims to run. `tests/built/coverage.txt` has one line per feature, `feature rom...`, where a ROM is
`7800basic/<sample>` or `diag/<name>`:

| Features | Judged from |
|----------|-------------|
| `board-linear16`, `board-linear32`, `board-linear48`, `board-supergame`, `board-supergame-ram`, `board-bank6`, `board-sg144` | `muhtari_probe`'s board, options, ROM size and RAM size |
| `rm0`, `rm2`, `rm3`, `write-mode`, `kangaroo`, `holey8`, `holey16`, `indirect1`, `indirect2`, `dli` | the core's sticky feature mask after 600 fast-boot frames (`docs/hardware/maria.md`, Statistics) |

`muhtari-test-coverage` (`tests/built/coverage.c`) runs each named ROM once and prints one line per claim with the
board, sizes and mask it saw. A claim fails when the ROM does not show the feature, and a feature with no ROM fails
too. A ROM's source or name never counts.

To add a claim, run `build/ci/tests/muhtari-test-coverage FILE build/ci/built tests/diag` on a scratch file that
names the ROM under the feature, read its mask, then add the ROM to the feature's line. A sample a line names becomes
a fixture `basic.coverage` requires.

`basic.coverage.self_test` appends `kangaroo 7800basic/simple` to a copy of the file. `simple` never uses kangaroo
mode, so the run must fail on that claim.

### openbios

`built.openbios` builds the 7800 OpenBIOS, whose source is CC0, into `build/<preset>/built/openbios/openbios.bin`, the
BIOS image the BIOS-path tests boot (fixture `built_openbios`). The source is the `7800openbios-asm` row of `pins.txt`.
It includes two binaries of an in-BIOS game that is not CC0, and they are never fetched: `tests/built/openbios.cmake`
replaces each `incbin` line with the same number of `$FF` bytes, fails unless each line occurs exactly once, assembles
with the 7800basic package's dasm, and checks the image's SHA-256,
`46b5bc2b716a20a1c40e2b920365c29cbab1594d01da94f4567b240181365cb2`, on every leg. `-DFILL=00` builds the `$00` variant.
`built.openbios.self_test` doubles one `incbin` line in a copy of the source and expects the build to fail naming it.

The image is never committed or shipped. Both fills' digests are rows of `tests/release/firmware.txt`, so
`release.firmware_scan` rejects a release zip that holds either, and CI uploads nothing from `build/*/built`.


## diag

`diag.*` runs this repository's own diagnostic ROMs, which measure the hardware the way a program on the console
can and report numbers, never verdicts. Each `tests/diag/<rom>.s` is MIT 6502 code with an SPDX line. It includes
`tests/diag/lib.s` (the registers, the mailbox, its CRC and a results screen in our own hex glyphs) and is committed
beside its `<rom>.cfg` and the signed `<rom>.a78` built from them, with a row in `tests/roms/manifest.txt`. The same
bytes run on hardware from a flash cartridge. `docs/hardware/maria.md`, Diagnostic expected values, and
`docs/hardware/cartridge.md`, The board ROMs, say how each ROM measures and derive every value it reports:

| ROM | Measures |
|-----|----------|
| `tests/diag/dma.a78` | DMA cost by header, width, mode, terminator, hole and overrun, as INTIM after one burst |
| `tests/diag/cpu.a78` | the 2 ticks a slow access adds, as INTIM after 64 loads ended by a WSYNC |
| `tests/diag/dli.a78` | the DLI's NMI on zones 0, 1 and 2 and with the CPU parked in WSYNC, MARIA's WSYNC release, a store right after a WSYNC, and MSTAT's two edges by line and poll |
| `tests/diag/linear16.a78`, `bank6.a78`, `sg144.a78` | the bytes the linear, bank-6 and 144K boards map at $4000, $8000 and $C000, before and after bank writes |
| `tests/diag/modes.a78` | five zones in 160B, 320B, 320D, kangaroo mode and 2-byte characters on a plain SuperGame board; the byte at $4000 and the DLIs in one frame |

A diagnostic that defines `diag_nmi` gets it as its NMI vector; the others return from NMI at once.

**The mailbox** is RAM at $2200-$22FF:

| Address | Content |
|---------|---------|
| $2200-$2203 | magic `M7DG` |
| $2204 | version 1 |
| $2205 | status: $80 running, $00 done, written last |
| $2206, $2207 | records written; the ROM's id |
| $2208-$2209 | CRC-16 of $2210-$22FF, low byte first: polynomial $1021, initial $FFFF, no reflection, no final XOR |
| $2210 + 4n | record n: test id n + 1, flags (bit 0 valid), a 16-bit value, low byte first |

The results screen shows one row per record, `NN VVVV` in hex, and the CRC on the last row, so a photographed screen
can be transcribed and checked.

**The driver.** `muhtari-test-diag ROM EXPECTED [STATS]` (`tests/diag/driver.c`) loads the ROM with `muhtari_load`,
fast-boots it, and reads the mailbox at the end of each frame, the VBLANK start, through `muhtari__peek`, which
changes no state. It stops when the magic is there and the status is $00, and fails as `hang` after 600 frames. It
then checks the version and the CRC and judges record n against the n-th row of `tests/diag/expected.csv` for the
ROM, which it names from the file name. A missing or extra record, a wrong id or an unset valid bit fails. It prints
one line per record and exits 0 when all pass, 1 when one fails, and 2 on a usage or file error. No verdict lives in
the ROM or the runner.

**`expected.csv`** has the columns `rom,test,value,source`. The value is a number or an inclusive `lo-hi` range,
where the hardware leaves a choice the ROM cannot see. The source is `hw` (a hardware measurement), `guide` (the
Software Guide or a datasheet), `rtl` (the PLA re-simulation) or `fit` (a fitted value). A `hw` or `guide` value is
derived in `docs/hardware/maria.md` and committed before the ROM that reports it first runs; only a `fit` value may
be read off the emulator. `git log` shows the order.

**`stats.csv`**, `rom,test,ordinary,last,objects`, records each DMA test, and each DLI test that reaches a burst, a
second time. On the frame where a record
appears, the driver finds the first line from 17 to 258 that ran DMA and requires the core's statistics for it to
show the row's cost for its kind, ordinary or last, and its objects; `-` means a line of that kind must not appear.
It prints `stats agree` or `stats disagree` for each.

**Rebuilding.** `diag.rebuild` (label `built`) assembles each `tests/diag/*.s` that has a `.cfg`, and Bruce Clark's
`tests/cpu/decimal/clark.s` as printed and as its 65C02 control, with the pinned `dasm.wasm -f3`, `7800sign.wasm -w`
and `7800header.wasm` under wasmtime (`tests/diag/assemble.cmake`), and requires each result's SHA-256 to equal its
manifest row's. After a source edit, derive and commit any new expected value first, then write the new bytes with

    cmake -DWASMTIME=build/pins/wasmtime/wasmtime -DPKG=build/pins/7800basic -DSRC=tests/diag/<rom>.s \
          -DCFG=tests/diag/<rom>.cfg -DOUT=build/diag -DMANIFEST=tests/roms/manifest.txt \
          -DKEEP=tests/diag/<rom>.a78 -P tests/diag/assemble.cmake

commit the source, and then the `.a78` with its manifest row's digest and pin set to that commit.

| Test | Covers |
|------|--------|
| `diag.dma` | the `dma` ROM's 27 records against `expected.csv`, each DMA record also against `stats.csv` |
| `diag.cpu` | the `cpu` ROM's 4 records against `expected.csv` |
| `diag.dli` | the `dli` ROM's 11 records against `expected.csv`, three DLI records also against `stats.csv` |
| `diag.linear16`, `diag.bank6`, `diag.sg144` | the board ROMs' 4, 12 and 12 records against `expected.csv` |
| `diag.modes` | the `modes` ROM's 2 records against `expected.csv` |
| `diag.rebuild` | the committed diagnostics and Clark's two builds rebuilt from source, byte for byte |
| `diag.rebuild.self_test` | a manifest copy with one digit of `dma.a78`'s digest changed must fail with `tests/diag/dma.a78: SHA-256 drift` |
| `diag.expected.self_test` | an `expected.csv` copy whose `null` value is 242 must fail on that record |
