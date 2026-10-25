# Compatibility

Generated from `tests/compat/ledger.csv` by `tools/compat_doc.cmake`; do not edit it by hand.
`docs/emulation-tests.md`, ledger, says what each column and status means and how a row is judged.

NTSC board support includes linear, SuperGame, Activision, Absolute and Fractalus.
Owned board diagnostics verify selected banking and physical RAM semantics; they do not promote an
untested retail title to a passing gameplay result. The owned PAL diagnostic verifies raster timing,
native video and sustained audio across the library, runner and libretro core; it does not establish
PAL retail playability or hardware calibration. Its local-tier mailbox record names the executed
diagnostic predicate, separately from the NTSC title/BIOS predicates evaluated by the library preset.
[PAL evidence and limits](pal-tests.md) describe the numeric checks and confidence.
[Cartridge behavior](hardware/cartridge.md) describes identified-only Activision AM normalization,
original image identity, physical 2 KiB Fractalus RAM and selected compatibility/profile policies.
[Host memory maps](libretro.md#achievements) expose fixed ROM and only the documented RAM aliases;
banked windows and Fractalus mirrors above $4FFF remain descriptor holes.

The ledger has 43 rows. 8 are judged by the weak predicate `boots@F` (the game booted, drew and
kept running), not by a pinned title screen.

| Status | Rows |
|---|---|
| pass | 40 |
| skipped | 2 |
| unsupported | 1 |

| Board | Rows |
|---|---|
| banksets | 1 |
| linear | 34 |
| supergame | 1 |
| supergame+bank6_at_4000 | 1 |
| supergame+ram | 5 |
| supergame+rom_at_4000 | 1 |

| Region | Rows |
|---|---|
| NTSC | 42 |
| PAL | 1 |

| Id | Title | Board | Region | Peripherals | Predicate | Status |
|---|---|---|---|---|---|---|
| `7800basic-0.41/adventurer` | adventurer | linear | NTSC | - | title | pass |
| `7800basic-0.41/atarivoxspeech` | atarivoxspeech | linear | NTSC | - | title | pass |
| `7800basic-0.41/backuptest` | backuptest | linear | NTSC | - | boots (no-main-loop-jams) | skipped |
| `7800basic-0.41/banksets` | banksets | banksets | NTSC | - | boots (bankset-board) | unsupported |
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
| `diag/bank6` | muhtari bank6 diagnostic | supergame+bank6_at_4000 | NTSC | - | title | pass |
| `diag/cpu` | muhtari cpu diagnostic | linear | NTSC | - | title | pass |
| `diag/dli` | muhtari dli diagnostic | linear | NTSC | - | title | pass |
| `diag/dma` | muhtari dma diagnostic | linear | NTSC | - | title | pass |
| `diag/linear16` | muhtari linear16 diagnostic | linear | NTSC | - | title | pass |
| `diag/modes` | muhtari modes diagnostic | supergame | NTSC | - | title | pass |
| `diag/sg144` | muhtari sg144 diagnostic | supergame+rom_at_4000 | NTSC | - | title | pass |
| `diag/pal` | muhtari PAL diagnostic | linear | PAL | pokey450 | mailbox | pass |
