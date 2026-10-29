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

`frame.sample_renders` carries the `built` label: it needs the sample the pinned 7800basic builds.

## input

`input.directions` checks both keypads:2864-tick recovery, DDR, ghosts,
all12 keys, contention, D7/D6 and HALT/JAM. `input.pa7_edge` checks motion
connector cycles, exact flag-clear boundaries, four-axis extreme saturation/
cancellation and JAM/HALT. Independent vectors: `hardware/controllers.md`.

`diag.controllers` judges `expected.csv`:19 records each for motion0/light0/
analog0;27 for analog1/motion1..3;26 each for SNES motion4a..c and Mega
motion5a..c. Fresh groups reuse the three owned ROMs and27-record cap.
Independent equations, schedules, adapter tails/pairs and normal pins are
in `hardware/controllers.md`. Both ports and every control execute.

Library, actual movie2 runner and loaded module compare all12 native frames,
raw transitions and exact PCM/counts after numeric identity/count/IDs/CRC and
values pass. WASM omits module; native/public pixels and stereo copies are
checked. Real judges reject wrong values, swapped axes/families/pairs,
damaged tails/identification, omissions, duplicates and missing/truncated
reports, including CRC-valid faults. Motion/light frame5 omits submission;
adapters submit absolute state. Runner tests cover4095 masks and EOF release
on both ports; module tests cover mappings/providers and frozen SAVE_RAM.

`input.*` checks what a game reads from the controllers and the console switches. Each case builds a 48 KiB linear
cartridge in memory from its own 6502 code and loads it through `muhtari_load`. Every frame the program waits for
MSTAT's VBLANK, reads SWCHA, SWCHB and INPT0-INPT5 once into a ring in RAM, and paints each byte as its own band of 28
lines, so the picture shows every value read. The input comes from `muhtari-movie 1` rows read by the runner's own
`runner/movie.c`, so the path from a movie's spelling to the register is the runner's.
Every expected byte comes from `docs/hardware/controllers.md` and the cartridge notes it cites (sections 3.1 to 3.3),
in a comment beside the row.

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

`basic.<name>` plays one 7800basic sample for 600 frames and checks them against a pin: one test for each row of
`tests/built/expected.txt`, the sample coming from that row's `built.<name>` fixture. The ROM is loaded with
`muhtari_load` and fast-booted, with no input. `muhtari-test-chain` (`tests/built/chain.c`) hashes every frame into
a chain. These pins use explicitly unplugged ports, keeping the no-EEPROM topology though metadata now attaches serial
storage by default. Enabled devices and default attachment run through the real RIOT path in
`input.savekey.protocol` and `input.savekey.pins`; the ROM bytes and frame pins are unchanged:

    h_0 = 32 zero bytes
    h_i = SHA-256(h_(i-1) || frame_i), i = 1 .. 600

Here frame_i is frame i's 320 x 242 native colour bytes. Frame 1 is the partial frame after the hand-off. The
chain covers frame order, so two swapped frames fail like one changed frame. `tests/built/frames.cmake`
compares h_600 with the row's sixth column, `frame`.

The file holds a `# behaviour_revision N` line. Each check fails unless N equals the library's behaviour revision:
a pin holds only at the revision it was written under. `basic.revision.self_test` gives the check a copy whose
line says 0 and expects it to fail, naming the behaviour revision.

When a check fails with `frame chain drift`, find the first frame that differs. Run
`muhtari-run --frames 600 --hash-frame N FILE` on the old and new cores and bisect on N.

### Writing a pin

Pins are written only by the core under test, never by hand:

    cmake -DCHAIN=build/ci/tests/muhtari-test-chain -DROM=build/ci/built/<name>/<name>.bas.a78 \
          -DNAME=<name> -DEXPECTED=tests/built/expected.txt -DWRITE=ON -P tests/built/frames.cmake

`-DWRITE=ON` fills a `-` column. If the column already holds a pin at the file's revision, it refuses
(`refusing to rewrite a pin at behaviour revision N`), unless the core gives the same chain.

An intended change that moves a pin goes one of two ways:

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

`basic.coverage.self_test` appends `kangaroo 7800basic/simple` to a copy of the file; `simple` never uses kangaroo
mode, so that claim must fail.

### openbios

`built.openbios` builds the 7800 OpenBIOS, whose source is CC0, into `build/<preset>/built/openbios/openbios.bin`, the
BIOS image the BIOS-path tests boot (fixture `built_openbios`). The source is the `7800openbios-asm` row of `pins.txt`.
It includes two binaries of an in-BIOS game that is not CC0, and they are never fetched: `tests/built/openbios.cmake`
replaces each `incbin` line with the same number of `$FF` bytes, fails unless each line occurs exactly once, assembles
with the 7800basic package's dasm, and checks the image's SHA-256,
`46b5bc2b716a20a1c40e2b920365c29cbab1594d01da94f4567b240181365cb2`, on every leg. `-DFILL=00` builds the `$00` variant.
`built.openbios.self_test` doubles one `incbin` line in a copy of the source and expects the build to fail naming it.

The image is never committed or shipped. Both fills' digests are rows of `tests/release/firmware.txt`, so
`release.firmware_scan` rejects a release zip holding either, and CI uploads nothing from `build/*/built`.

## bios

`bios.*` boots through a BIOS (`tests/boot/test_boot.c`). The cases that need OpenBIOS take the image `built.openbios`
builds and the `simple` sample `built.simple` builds, so they run on the ci and wasm32 inventories, which list both. K is the hand-off frame (`docs/embedding.md`, Booting with a BIOS).

- `bios.handoff_simple`: after power-on and RES, PC is the BIOS's reset vector, S is `$FD`, I is set, INPTCTRL and
  CTRL are `$00`, and frame 1 is 117,174 ticks. OpenBIOS then hands `simple` off with K from 300 to 340; it is 323.
  The window comes from OpenBIOS's source: its Fuji shows for 10 half-second ticks of 32 frames, 320 frames.
- `bios.k_window`: the window's predicate passes 300 and 340 and fails 299 and 341.
- `bios.k_first_fetch`: a 4 KiB BIOS and a cartridge built in memory hand control back and forth, each return to
  the cartridge after EXT rises again; K stays at the first.
- `bios.load_aliases`: RAM/EEPROM/HSC BIOS reloads and failed-load preservation.
- `bios.splash_skip`: Pause held on frames 1 to 3 gives K below 30. OpenBIOS first reads SWCHB in frame 3, measured
  with the lag counter, which counts no input read before it.
- `bios.rejects_cart`: `simple` with bit 0 of `$FFF8` cleared fails OpenBIOS's region check, so the BIOS takes its
  2600 path; that frame and the next return `MUHTARI_STATUS_ERR_BIOS_REJECTED_CART` with no samples, a power cycle
  rejects it again in the same frame, and a load of the unchanged image hands off in the window.

The runner's side is in `tests/runner/tests.cmake`: `runner.bios.rejected` exits 77 on the input cart, whose `$FFF8`
is 0; `runner.bios.invalid` exits 1 on a 1,000-byte BIOS; `runner.movie.boot_bios`, `bios_mismatch`,
`boot_bios_no_file` and `boot_conflict` check a movie's `boot` and `bios-sha256` keys.

## title

`tests/compat/ledger.csv` is the compatibility ledger: one header line, then one row per ROM with the columns of
`.planning/preparation/CONFORMANCE.md` section 5, sorted by `id`, then `predicate`. No field holds a comma, quote or
newline. The oracle columns are `-`. `muhtari-test-predicate ROM PREDICATE [--bios FILE]` (`tests/compat/predicate.c`)
evaluates a row's predicate, prints `pass` or `fail <reason>`, and exits 0, 1, or 2 on a usage or load error. A
frame's hash is the SHA-256 of its colour bytes, 320 x 242 (NTSC) or 320 x 292 (PAL), as `muhtari-run --hash-frame`
prints it.

- `title@F=H`, on the fast boot: frame F hashes to H, the title the sample shows by then.
- `title@F=H`, with `--bios`: the hand-off frame K comes by frame 600 and lies in [300, 340], and some frame from K
  to K + 299 hashes to H. K is the frame of the first cartridge opcode fetch after INPTCTRL's EXT goes from 0 to 1
  (`docs/embedding.md`, Booting with a BIOS). OpenBIOS shows its Fuji for about 320 frames, so the 300 frames count
  from K.
- `boots@F`, the weak predicate: in the F frames from the hand-off (frame 1 on the fast boot, K with `--bios`),
  DMA ran, at least two distinct frames appeared and the CPU did not jam.

BIOS-dependent titles fall back to `boots@F;why=<reason>`. Five samples seed
`rand` from INTIM, which differs between boot paths (`why=rng-seeded-from-intim`).
`hiscoretable` draws after frame 262, so uses frame 300. `backuptest` and
`languagetest` draw nothing and are skipped, as is `banksets`: the pinned recipe does not assemble it.
Local diagnostic `mailbox@12` rows record owned-template CRC evidence;
[compatibility](compatibility.md) gives their scope.

`title.simple.fast` and `title.simple.bios` evaluate `simple`'s row on both paths, reading its hash from the ledger
at configure time. Every row of tier `pr` is evaluated on both paths by the `library` preset (ledger, below), in CI
on every pull request, and locally by the pin recipe with `-DBIOS`.

**Pinning.** `--pin` prints `title@F=<hash>` for the fast boot's frame F and fails with `single colour` when every
byte of that frame is the same, so a blank screen is never pinned. `title.pin.self_test` runs it on the blank linear
seed cartridge and expects that failure. To pin a new ROM, add its row with predicate `title@120=-` and status
`unknown` (the `id` is `7800basic-0.41/<sample>` or `diag/<rom>`), then run

```sh
cmake -DPREDICATE=build/ci/tests/muhtari-test-predicate -DLEDGER=tests/compat/ledger.csv \
  -DBUILT=build/ci/built -DDIAG=tests/diag -DBIOS=build/ci/built/openbios/openbios.bin \
  -DCOMMIT=<12 hex> -DDATE=<YYYY-MM-DD> -P tests/compat/pin.cmake
```

It fills each `title@F=-` row, sets each `unknown` row's status from both paths, and fails by name when an
evaluated row's status disagrees. A title not yet on `main` is unreleased: when an intended core change moves it,
reset its predicate to `title@F=-` and run the recipe in the same commit. Never type a hash by hand. The recipe
evaluates only `title@` and `boots@` rows and copies `mailbox@` rows, and rows whose id has no `/`, untouched;
`-DCHECK=<file>` fails when the written ledger differs from it (`title.pin.order`).


## ledger

**Columns.** `id` is the stable key: `7800basic-0.41/<sample>`, `diag/<rom>`, or a MAME short name. `title` is the
display name. `sha1_raw` and `sha256_raw` hash the image without its A78 header, `sha1_a78` and `sha256_a78` the
headered file (`-` without one). `board`, `region` and `peripherals` (`+`-joined, `-` for none) say what the ROM
needs. `predicate` is how the row is judged (title, above). `evidence_commit` is the commit last evaluated and
`evidence` the first 16 hex digits of the title hash, or `-`. `oracle` and `checked` record an outside emulator's
agreement and its date; both are `-` for now.

An id without `/` is a MAME short name. `tools/softlist_ledger.cmake` writes those rows; neither hand nor the pin
recipe edits them: `cmake -DXML=build/pins/mame/a7800.xml -DLEDGER=tests/compat/ledger.csv -DCARTDB=src/cartdb.c
-DOUT=tests/compat/ledger.csv -P tools/softlist_ledger.cmake`. Their `board` and `peripherals` repeat the list's
declarations; `any` in `region` means the list names both systems or none.

**Statuses.** `pass` and `fail` are the last evaluation on both paths. `unknown` (shown as not evaluated) was never evaluated.
`unsupported` needs a board or peripheral the core refuses, and names it. `skipped` is deliberately not run, with
the reason in the predicate, and is never in a CI tier. `infrastructure_error` means the harness broke.

**Tiers.** `pr` rows are judged on every pull request by the `library` preset.
`local` rows are not judged by it; `diag.bankset`, `diag.expansion` and `diag.pal` run the four `mailbox@` rows and compare
the mailbox CRC with the row's evidence.

**The library preset.** It finds each `pr` row's file by its SHA-1 in `MUHTARI_ROM_DIR`, then `build/ci/built`, then `tests/diag`; an empty file or one above the image
maximum is skipped before hashing. `MUHTARI_BIOS` names the BIOS image, by default
`build/ci/built/openbios/openbios.bin`. Configure fails when the directory, the BIOS image or any row's file is
missing. Links and unreadable entries are skipped, never followed, and the missing-file error says how many were, so
copy or hard-link dumps in rather than symlink them. A bracket or semicolon in the directory name, or an unmatched
bracket or a semicolon in a file name, also fails configure with a fixed message. After the `ci` preset's built tests
have run:

```sh
MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library
```

- `library.<id>`, the id's slashes as dots, runs `muhtari-test-predicate ROM PREDICATE --bios FILE --id ID --result
  FILE`. It evaluates the fast boot, then the BIOS path, and writes one line `id,predicate,status,evidence`. The
  status is `pass` only when both paths pass. It exits 0 once the line is written, because the ratchet judges it.
- `library.results` joins the lines into `build/library/results.csv`, sorted by id, then predicate. It fails unless
  every row wrote one.
- `library.ratchet` (`tests/cmake/compat_ratchet.cmake`) fails when the results and the ledger's `pr` rows differ in
  either direction, by status or evidence.
- With `MUHTARI_LEDGER_BASE` set to a commit at configure time, a row (id, predicate) that was `pass` in that
  commit's ledger must still be `pass`, and `pr` if it was. A changed predicate or a removed row counts as a fall. The exception is a
  line `<id> <predicate> <expiry YYYY-MM-DD> <reason>` in `tests/compat/allow.txt` whose expiry is between today and
  60 days on. `unknown` to anything and `fail` to `pass` need no line. A base whose tree has no ledger protects
  nothing. A base missing from the clone fails.
- `library.ratchet.self_test` flips the first row's status and expects that row named.
- `library.compat_doc` runs after the ratchet and fails when `tools/compat_doc.cmake`'s output differs from
  `docs/compatibility.md`.

**In CI.** The `library` job (`docs/testing.md`, CI) builds the samples and OpenBIOS, then runs the preset with the
pull request's base commit, and is part of `CI required`. `policy.compat_doc_fresh` also checks the page on every
`ci` and `wasm32` lane. After a ledger change, regenerate the page:

```sh
cmake -DLEDGER=tests/compat/ledger.csv -DEXPECTED=tests/diag/expected.csv \
  -DINVENTORY=tests/inventory/ci.txt -DOUT=docs/compatibility.md -P tools/compat_doc.cmake
```

**Your own dumps.** Optional. First build the samples and OpenBIOS under
`build/ci/built` with `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -L '^built$'`, then:

```sh
MUHTARI_ROM_DIR=<dump dir> [MUHTARI_BIOS=<file>] cmake --workflow --preset library
ctest --test-dir build/library -R '^library[.]local$' -V | grep -E 'local: [0-9]+ of [0-9]+ present'
```

`library.local` identifies each file through the core's cartridge list, judges each present softlist row on both
boot paths, and prints `local: P of N present, X pass, Y fail, Z absent, commit=... tree=...` and
`local: U files unread (links or unreadable)`. Links are never followed and no file is named, so copy or hard-link
dumps in rather than symlink them. A bracket or semicolon in the directory name, or an unmatched bracket or a semicolon
in a file name, stops the run with a fixed message.
`build/library/local.csv` never enters `results.csv` or the ledger; a fail there is data, not a test failure.

## boot to play

`runner.boot_to_play` plays one game from boot through the real runner: the 7800basic `spacephysics` sample
`built.spacephysics` builds (fixture `built_spacephysics`), fast-booted. `tests/runner/boot_to_play.cmake` writes four
movies into `build/<preset>/tests/boot-to-play/`, each releasing every control until frame F = 121, the frame after
the sample's `title@120`, then holding one line for H = 60 frames:

| Run | Held from frame F |
|-----|-------------------|
| `none` | nothing |
| `input` | Up and Left on port 1 |
| `port2` | Up and Left on port 2 |
| `mirror` | Up and Right on port 1 |

It runs `muhtari-run` on each with `--hash-frame` on every frame and `--dump-frame` at frames 120, 140, 160 and 180,
then `muhtari-test-boot-to-play` (`tests/runner/boot_to_play.c`) judges `input` against `none`, decoding the dumps
with the reader `runner.png` uses (`tests/runner/png_read.c`). It prints `mask <bits>: <details>`, one bit per failed
predicate:

- bit 0, (a): the first frame whose hash differs from `none`'s lies in [F, F + 1]. The game draws thrust on the frame
  after the input whatever the start frame, so the bound is one frame, and a latch one frame late fails it.
- bit 1, (b): pixels differ from `none`'s at frames 140, 160 and 180, and their centroid moves at least 4 px along one
  axis from 140 to 180.
- bit 2, (c): the picture is not frozen; the hashes change after frame 160.

(b) and (c) judge a response, so a run with no frame from F on that differs from `none`'s fails (a) alone. The test
also requires `input`'s centroid to move up and `mirror`'s to move down, which catches swapped or merged left and
right bits.

**The controls** were found by running the game, never by reading its source (`PROVENANCE.md`, section 3, lists the
samples as unknown). With each direction and button held alone from frame 121 and frames 150 and 180 viewed: Up thrusts
and moves the ship right from frame 122; Left and Right turn it every eighth frame, 4 to 11 frames after the press;
button 2 fires; Down, button 1 and every input on port 2 change nothing. Up and Left together thrust while turning,
so the ship climbs, and Up and Right is its mirror.

**Negative controls.** Each judges the recorded runs through `tests/cmake/expect_fail.cmake` and must exit 1 with this
output:

| Test | Judged | Output |
|------|--------|--------|
| `runner.boot_to_play.no_input` | `none` as the input run | `mask 1` |
| `runner.boot_to_play.port2` | `port2` | `mask 1` |
| `runner.boot_to_play.mirrored` | `input` as its own mirror | `sign did not flip` |
| `runner.boot_to_play.static_mutant` | `input` with its frame 120 dump recoloured and repeated at 140, 160 and 180 | `mask 2` |
| `runner.boot_to_play.late_mutant` | `input` with its hashes from frame F moved one frame later | `mask 1` |

The mutants only transform recorded output. This test sets input only between frames; `prop.input_at_vblank` checks when the console applies it.

## diag

PAL: [diagnostic tests](pal-tests.md) judge startup plus 600 full frames before
native/raw/PCM equality across all hosts. The local-tier `diag/pal` ledger row
records `mailbox@601`/CRC, exercised by `diag.pal`, separately from the library
preset's NTSC title/BIOS predicates. See [model limits](quality.md).

`diag.*` runs owned MIT 6502 ROMs reporting numbers, never verdicts.
`tests/diag/<rom>.s` includes `lib.s` (registers, mailbox, CRC, own hex glyphs),
with its `.cfg`, signed `.a78` and manifest row. The bytes can run from a flash
cartridge. [MARIA expectations](hardware/maria.md) and
[board ROMs](hardware/cartridge.md) derive the measurements:

| ROM | Measures |
|-----|----------|
| `tests/diag/dma.a78` | DMA cost by header, width, mode, terminator, hole and overrun, as INTIM after one burst |
| `tests/diag/cpu.a78` | the 2 ticks a slow access adds, as INTIM after 64 loads ended by a WSYNC |
| `tests/diag/dli.a78` | the DLI's NMI on zones 0, 1 and 2 and with the CPU parked in WSYNC, MARIA's WSYNC release, a store right after a WSYNC, and MSTAT's two edges by line and poll |
| `tests/diag/linear16.a78`, `bank6.a78`, `sg144.a78` | the bytes the linear, bank-6 and 144K boards map at $4000, $8000 and $C000, before and after bank writes |
| `tests/diag/modes.a78` | five zones in 160B, 320B, 320D, kangaroo mode and 2-byte characters on a plain SuperGame board; the byte at $4000 and the DLIs in one frame |
| `tests/diag/activision.a78` | 27 records, mailbox ID 10: both tagged 8 KiB halves for all eight banks, four fixed windows, endpoints and hotspot neighbors; all executing code is fixed at $E000-$FFFF |
| `tests/diag/absolute.a78` | 21 records, mailbox ID 11: bank-zero startup, D0/D1 selection and precedence, retain/high-bit policies, fixed ROM and nonselector writes |
| `tests/diag/fractalus.a78` | 24 records, mailbox ID 12: initial zero RAM, adjacent-page aliases, upper mirrors, distinct physical locations, multiple patterns and fixed ROM |
| `tests/diag/bankset.a78` | 17 CPU records, ID 20; bank sweeps, RAM/redirects and MARIA graphics |

[BankSet](bankset-tests.md) and [expansion](expansion-tests.md) expectations precede host comparison.

The retail diagnostics use protocol 1, at most 27 records and independent fixture tags. Decoder edges, mirrors and
startup are compatibility/profile policies. `diag.retail` runs all three through the library, actual runner
(`--frames 600 --diagnostic`) and loaded module, plus the persistent Absolute tracer. Its judge checks magic,
version, status, identity, CRC, count, record order, flags and values. The runner reports the 256-byte mailbox after
a bounded normal run. This proves the selected semantics, not retail gameplay or PAL.

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

The results screen shows one row per record, `NN VVVV` in hex, and the CRC on the last row, so a photo can be
transcribed and checked.

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
derived in `docs/hardware/maria.md` and committed before the ROM that reports it first runs (`git log` shows the
order); only a `fit` value may be read off the emulator.

**`stats.csv`**, `rom,test,ordinary,last,objects`, records each DMA test, and each DLI test that reaches a burst, a
second time. On the frame where a record appears, the driver finds the first line from 17 to 258 that ran DMA and
requires the core's statistics for it to show the row's cost for its kind, ordinary or last, and its objects; `-`
means a line of that kind must not appear. It prints `stats agree` or `stats disagree` for each.

**Rebuilding.** `diag.rebuild` (label `built`) assembles each `tests/diag/*.s` that has a `.cfg`, and Bruce Clark's
`tests/cpu/decimal/clark.s` as printed and as its 65C02 control, with the pinned `dasm.wasm -f3`, `7800sign.wasm -w`
and `7800header.wasm` under wasmtime (`tests/diag/assemble.cmake`), and requires each result's SHA-256 to equal its
manifest row's.

Sample and diagnostic recipes share Wasmtime's compiled-module cache at `build/pins/wasmtime-cache`, configured per
output outside each fresh fixture directory. Every source, assemble, sign and header stage still runs; only pinned
tool compilation is reused, and no home cache or fixture bytes are cached.

After a source edit, derive and commit any new expected value first, then write the new bytes with

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
| `diag.retail` | all three owned retail-board diagnostics through three actual deliverables, plus the persistent tracer and report/protocol fault controls |
| `diag.bankset`, `diag.bankset_tracer` | complete geometry/RAM matrix and canonical three-host case |
| `diag.rebuild` | the committed diagnostics and Clark's two builds rebuilt from source, byte for byte |
| `diag.rebuild.self_test` | a manifest copy with one digit of `dma.a78`'s digest changed must fail with `tests/diag/dma.a78: SHA-256 drift` |
| `diag.expected.self_test` | an `expected.csv` copy whose `null` value is 242 must fail on that record |

## prop

`prop.*` checks properties of the run loop over generated cases (`tests/prop/test_prop.c`, harness
`tests/prop.h`). Each case fast-boots a cartridge drawn from a pool and runs at most 8 frames. The pool is the run
fuzz target's 29 seed inputs and 48 KiB of random bytes from the case's stream, each made a cartridge as that target
does (see fuzz), and the built `simple` and `eightysix` samples. Two runs are equal when every frame's status,
audio count and picture SHA-256 match, and so does the state digest: SHA-256 over the instance with its pointers
cleared, then the cartridge's ROM and RAM.

Each property selects Activision, Absolute and Fractalus in its first three cases;
later cases draw from the pool. Switched banks and written
physical RAM enter the comparisons. PCM joins video, status and pointer-cleared
deep state, including resumed execution after refused loads and allocation failure.


| Test | Holds for every case |
|------|----------------------|
| `prop.split_run` | Frames 1 to k drawn at a wider pitch, then `muhtari_get_info`, `muhtari_get_counters` and `muhtari_get_memory`, then frames k+1 to N, equal one sequence of N frames, for k from 0 to N |
| `prop.peek_pure` | Peeking all 65,536 addresses through `muhtari__peek` and `muhtari__bus_peek` after frame k changes neither the digest nor any later frame, and a second pass reads the same bytes |
| `prop.instances_isolated` | Two instances on different cartridges, loaded, given random input and run alternately call by call, equal each run alone |
| `prop.input_at_vblank` | One to three `muhtari_set_input` calls after frame k, k at least 1, change neither the digest without the pending copy nor SWCHA, SWCHB and INPT0-5 as `muhtari__bus_peek` reads them; the next frame applies the last call, and it and the frame after equal those of an instance given only that call |

`prop.input_at_vblank` cannot test a call made during a frame: `muhtari_run_frame` returns before any other call.
k starts at 1 because the first frame after a fast-boot load starts at the hand-off, which latches
nothing.

**Cases.** A pull request runs 100 cases per property from a fixed seed on every lane that builds the samples. The
advisory nightly `prop` job runs 10,000 per property with the workflow run's ID as the seed. Both come from the test
program's environment, `MUHTARI_PROP_CASES` and `MUHTARI_PROP_SEED` (decimal); the core reads no environment. Case
c's stream depends only on the seed and c.

**A failure** is retried with its size, the frame bound, halved until the case passes, then printed as

    split_run: failed: seed 20261020 case 41 size 2
    rerun: MUHTARI_PROP_SEED=20261020 MUHTARI_PROP_CASES=42

Rerun it with those two variables set before `ctest --test-dir build/ci -R '^prop\.split_run$'
--output-on-failure`; the smallest failing size bounds the frames to inspect.

## fuzz

`fuzz.regress.<target>` replays a fuzz target's invariants on every lane over its seeds and its committed
regressions; `fuzz.regress.<target>.self_test` gives the replay no input and must fail with `no corpus inputs`. The
`fuzz` lane runs libFuzzer for 60 s per target with `-timeout=2`, under clang-21 with AddressSanitizer and
UndefinedBehaviorSanitizer (`docs/testing.md`, Lanes).

| Target | Input | Invariants |
|--------|-------|------------|
| `loader` (`tests/fuzz/loader.c`) | any image up to the size cap, `-max_len=1048705` | two probes agree; the load returns probe's status; a refusal allocates nothing and changes nothing; destroy frees everything |
| `run` (`tests/fuzz/run.c`) | explicit retail A78 inputs select fixed-geometry builders; other bytes are a 49,152-byte linear image; `-max_len=131200` | two instances, loaded and run 2 frames call by call, agree in board, load status, frame status, audio count, PCM and picture; each frame returns at the first instruction boundary at or after its deadline and less than four lines past it |

`tests/fuzz/run_cart.h` adjusts the footer bytes of ordinary bytes (headerless 48 KiB, linear) for acceptance and
clears accidental A78 magic; explicit Activision, Absolute and Fractalus A78 declarations use their exact geometry
and fixed-window reset vectors. The all-$00, JAM, all-$FF, thirteen splitmix64 and empty seeds persist; eleven
explicit retail/malformed/A8 inputs and one accidental-header regression make 29 run seeds. The loader has 31,
including isolated identified AM/OM inputs, unsupported combinations and geometry boundaries. Normalization checks
derive expected private bytes independently and preserve the supplied buffer and raw identity. Refusals compare
deep live ROM/RAM and resumed audible changing controls.

**Adding a regression.** The nightly `fuzz` job uploads crashes as `fuzz-crash` from `build/fuzz/fuzz/crash/`
(loader) and `build/fuzz/fuzz/run/crash/` (run). Minimise one with `muhtari-fuzz-<target> -minimize_crash=1
-runs=10000 <file>`, commit it under `tests/fuzz/<target>/regress/` with its row in `tests/roms/manifest.txt`, and fix
the core in the same pull request. Reconfigure to list it; `fuzz.regress.<target>` then replays it.

Boot-to-play also requests whole-run audio evidence. Its judge requires the exact frame/sample count, 48,000/1
rate, behavior revision and synthesis identity, and a PCM digest differing from SHA-256 over two zero bytes per
sample. A muted control substitutes that zero digest, keeps valid picture/input evidence and must fail only the
audio predicate (mask 8). The image, timing, wrong-port and mirror controls remain required. This proves
non-silence and delivery, not a listening comparison or calibration.

`audio.samples` pins eight built 7800basic sound programs in `tests/audio/expected.txt`: soundtest, soundeffects,
trackertest, eightysix, pokey, rmtramcompressiondemo, rmtramdemo and rmtromdemo. Each run completes 600 fast-boot
frames with both port1 ProLine buttons held for frames1–60 and released for61–600. The release starts rmtromdemo's
song; the press starts soundtest's generic-fire effect. Soundeffects and soundtest are byte-identical in the pinned
package; both names go through the existing fixture recipe.

Rows record name, frame count, mono sample count, rate, behavior revision, synthesis identity, raw digest and PCM
digest. Pins are regression evidence, backed by documentary chip tests and alias-sensitive spectral checks. The
archive driver compares observed, unobserved and discarded output, repeats the first frame after reset, requires
non-silence and zero pre-clamp clips, and bounds delivered/pre-clamp peak to 29,204 (−1 dBFS). Wrong-pin controls
must fail. The runner reproduces every row through a matching movie. No ROM or audio bytes are committed.

For an intentional pin update, first run the production spectral/controls, TIA, POKEY and IRQ checks, then
`muhtari-test-audio-samples --measure ROM NAME unused` from the build's tests directory. Review identity, counts,
raw changes and clipping audits before copying the row. A PCM change alone never justifies replacing raw or video
evidence. The platform hashes file also holds soundtest and pokey's seven audio fields under name prefixes, keeping
the card/PNG rows and Unix newlines.

It appends ten rows each under `activision.`, `absolute.` and `fractalus.`: frame 1, frame 600, all seven audio
fields and a fixed numeric mailbox report. The runner must identify the expected board and the driver must accept
the complete mailbox before a row is written. These diagnostics are silent, so the empty raw-transition digest is
expected; PCM bytes and counts still participate. Controls reject absent fixtures, wrong board or ROM identity,
missing/extra reports and each omitted frame/audio field. All 17 older
test-card/PNG and soundtest/POKEY rows keep their byte meanings.

`battery.first.` and `battery.second.` append eleven rows each: frame 1/120, the seven audio fields,
mailbox and complete raw-save SHA-256. Two fresh runner processes share only their private save directory.
The driver first requires independent HSC and EEPROM counts 1/1 then 2/2; its media judge checks all 34,816 bytes,
untouched FF bytes included. Only then may either process contribute hashes.
The 47 retained observations keep their order and values; five behavior annotation rows advance from 3 to 4 under
the version policy. The ordered schema rejects missing or duplicate rows, even when every lane repeats the fault.

Pull requests require exactly six identical expanded native hash files and compile PowerPC. Nightly and release
workflows also execute the full native/PowerPC inventories and compare a same-commit pair; hosted equality is
checked there and merge-dependent results are confirmed at ship. WASM compares with Linux x64 and stays advisory.
