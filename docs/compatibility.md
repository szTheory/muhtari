# Compatibility

Generated from `tests/compat/ledger.csv` by `tools/compat_doc.cmake`; do not edit it by hand.
`docs/emulation-tests.md`, ledger, says what each column and status means and how a row is judged.

Board support includes linear, SuperGame, Activision, Absolute, Fractalus and BankSet.
Owned board diagnostics verify selected banking and physical RAM semantics; they do not promote an
untested retail title to a passing gameplay result. The owned PAL diagnostic verifies raster timing,
native video and sustained audio across the library, runner and libretro core; it does not establish
PAL retail playability or hardware calibration. Its local-tier mailbox record names the executed
diagnostic predicate, separately from the NTSC title/BIOS predicates evaluated by the library preset.
[PAL evidence and limits](pal-tests.md) describe the numeric checks and confidence.
Local mailbox@12 rows record representative owned BankSet32/no-RAM and page-zero
XBoard/XM diagnostics executed by diag.bankset and diag.expansion. The same tests
also cover all fourteen BankSet geometries, eight XBoard banks, both XM windows
and sixteen pages, gate lifetimes, and selected PAL/dual-chip cases. These are
model-conformance observations, not whole-library gameplay accounting. The historical
7800basic banksets sample remains unexecuted because its pinned recipe does not
assemble; board support does not promote its status or change old passing predicates.
Metadata inclusion names capability, not gameplay acceptance. XM is a selected
RAM/POKEY/HSC implementation profile with medium physical confidence; no YM2151,
Covox/ADPCM, Souper/BupChip, internal XM firmware or full-XM services are available.
[Cartridge behavior](hardware/cartridge.md) describes identified-only Activision AM normalization,
original image identity, physical 2 KiB Fractalus RAM and selected compatibility/profile policies.
[Host memory maps](libretro.md#achievements) expose fixed ROM and only the documented RAM aliases;
banked/HALT RAM, expansion-overlapped cartridge windows, XM HSC windows and Fractalus
mirrors above $4FFF remain descriptor holes. Physical spans have separate lifetimes.

The ledger has 49 rows. 8 are judged by the weak predicate `boots@F` (the game booted, drew and
kept running), not by a pinned title screen.

| Status | Rows |
|---|---|
| pass | 46 |
| skipped | 3 |

| Board | Rows |
|---|---|
| absolute | 1 |
| activision | 1 |
| bankset | 2 |
| fractalus | 1 |
| linear | 34 |
| linear+xboard | 1 |
| linear+xm | 1 |
| supergame | 1 |
| supergame+bank6_at_4000 | 1 |
| supergame+ram | 5 |
| supergame+rom_at_4000 | 1 |

| Region | Rows |
|---|---|
| NTSC | 48 |
| PAL | 1 |

| Id | Title | Board | Region | Peripherals | Predicate | Status |
|---|---|---|---|---|---|---|
| `7800basic-0.41/adventurer` | adventurer | linear | NTSC | - | title | pass |
| `7800basic-0.41/atarivoxspeech` | atarivoxspeech | linear | NTSC | - | title | pass |
| `7800basic-0.41/backuptest` | backuptest | linear | NTSC | - | boots (no-main-loop-jams) | skipped |
| `7800basic-0.41/banksets` | banksets | bankset | NTSC | - | boots (recipe-does-not-assemble) | skipped |
| `7800basic-0.41/banner` | banner | linear | NTSC | - | title | pass |
| `7800basic-0.41/boxcollision` | boxcollision | linear | NTSC | - | title | pass |
| `7800basic-0.41/colorfade` | colorfade | linear | NTSC | - | title | pass |
| `7800basic-0.41/doublebuffer` | doublebuffer | supergame+ram | NTSC | - | boots (rng-seeded-from-intim) | pass |
| `7800basic-0.41/drivingcontrol` | drivingcontrol | linear | NTSC | - | title | pass |
| `7800basic-0.41/eightysix` | eightysix | supergame+ram | NTSC | pokey450 | boots (rng-seeded-from-intim) | pass |
| `7800basic-0.41/fontcheck` | fontcheck | linear | NTSC | - | title | pass |
| `7800basic-0.41/helloworld` | helloworld | linear | NTSC | - | title | pass |
| `7800basic-0.41/hiscoretable` | hiscoretable | linear | NTSC | savekey | boots (rng-seeded-from-intim) | pass |
| `7800basic-0.41/keypadcontrol` | keypadcontrol | linear | NTSC | - | title | pass |
| `7800basic-0.41/languagetest` | languagetest | linear | NTSC | - | boots (draws-nothing) | skipped |
| `7800basic-0.41/mega7800controllers` | mega7800controllers | linear | NTSC | - | title | pass |
| `7800basic-0.41/mousedemo` | mousedemo | linear | NTSC | - | title | pass |
| `7800basic-0.41/multibuttoncontrollers` | multibuttoncontrollers | linear | NTSC | - | title | pass |
| `7800basic-0.41/multisprite` | multisprite | linear | NTSC | - | boots (rng-seeded-from-intim) | pass |
| `7800basic-0.41/onehundredandone` | onehundredandone | supergame+ram | NTSC | - | boots (rng-seeded-from-intim) | pass |
| `7800basic-0.41/plotmapfile` | plotmapfile | linear | NTSC | - | title | pass |
| `7800basic-0.41/pokey` | pokey | linear | NTSC | pokey450 | title | pass |
| `7800basic-0.41/ramcharmap` | ramcharmap | linear | NTSC | - | title | pass |
| `7800basic-0.41/rmtramcompressiondemo` | rmtramcompressiondemo | supergame+ram | NTSC | pokey450 | title | pass |
| `7800basic-0.41/rmtramdemo` | rmtramdemo | supergame+ram | NTSC | pokey450 | title | pass |
| `7800basic-0.41/rmtromdemo` | rmtromdemo | linear | NTSC | pokey450 | title | pass |
| `7800basic-0.41/rotationalcontrols` | rotationalcontrols | linear | NTSC | - | title | pass |
| `7800basic-0.41/screensafe` | screensafe | linear | NTSC | - | title | pass |
| `7800basic-0.41/simple` | simple | linear | NTSC | - | title | pass |
| `7800basic-0.41/smasteroids` | smasteroids | linear | NTSC | - | title | pass |
| `7800basic-0.41/snescontrollers` | snescontrollers | linear | NTSC | - | title | pass |
| `7800basic-0.41/soundtest` | soundtest | linear | NTSC | - | title | pass |
| `7800basic-0.41/spacephysics` | spacephysics | linear | NTSC | - | title | pass |
| `7800basic-0.41/trackertest` | trackertest | linear | NTSC | - | title | pass |
| `7800basic-0.41/underthehood` | underthehood | linear | NTSC | - | title | pass |
| `diag/absolute` | muhtari absolute diagnostic | absolute | NTSC | - | title | pass |
| `diag/activision` | muhtari activision diagnostic | activision | NTSC | - | title | pass |
| `diag/bank6` | muhtari bank6 diagnostic | supergame+bank6_at_4000 | NTSC | - | title | pass |
| `diag/bankset` | muhtari bankset diagnostic | bankset | NTSC | - | mailbox (bankset32n) | pass |
| `diag/cpu` | muhtari cpu diagnostic | linear | NTSC | - | title | pass |
| `diag/dli` | muhtari dli diagnostic | linear | NTSC | - | title | pass |
| `diag/dma` | muhtari dma diagnostic | linear | NTSC | - | title | pass |
| `diag/fractalus` | muhtari fractalus diagnostic | fractalus | NTSC | - | title | pass |
| `diag/linear16` | muhtari linear16 diagnostic | linear | NTSC | - | title | pass |
| `diag/modes` | muhtari modes diagnostic | supergame | NTSC | - | title | pass |
| `diag/pal` | muhtari PAL diagnostic | linear | PAL | pokey450 | mailbox | pass |
| `diag/sg144` | muhtari sg144 diagnostic | supergame+rom_at_4000 | NTSC | - | title | pass |
| `diag/xboard` | muhtari xboard diagnostic | linear+xboard | NTSC | pokey450+hsc | mailbox (expansion-x0) | pass |
| `diag/xm` | muhtari xm diagnostic | linear+xm | NTSC | pokey450+hsc | mailbox (expansion-m0) | pass |
