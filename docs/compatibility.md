# Compatibility

Generated from `tests/compat/ledger.csv` by `tools/compat_doc.cmake`; do not edit it by hand.
[`docs/emulation-tests.md`](emulation-tests.md#ledger) says what each column and status means and how a
row is judged.

Evidence covers the rows listed; there is no claim of universal title compatibility. The pinned list's
board, region and peripheral columns repeat its declarations: `src/cartdb.c` recognising an image is
metadata, not playability. Owned diagnostics show model conformance against documented sequences, never
gameplay. Detail behind the claims: [cartridge behavior](hardware/cartridge.md),
[PAL evidence and limits](pal-tests.md), [BankSet](bankset-tests.md) and [expansion](expansion-tests.md)
tests.

204 of 204 pinned-list entries accounted for; 0 pass, 0 fail, 0 unsupported, 201 not evaluated, 3 skipped. Not evaluated means untested, not failing.

## Status by tier

Pinned CC0 list: 204 entries

| Status | pr | nightly | local | total |
|---|---|---|---|---|
| pass | 0 | 0 | 0 | 0 |
| fail | 0 | 0 | 0 | 0 |
| not evaluated | 0 | 0 | 201 | 201 |
| unsupported | 0 | 0 | 0 | 0 |
| skipped | 0 | 0 | 3 | 3 |

Owned samples and diagnostics: 49 rows

| Status | pr | nightly | local | total |
|---|---|---|---|---|
| pass | 42 | 0 | 4 | 46 |
| fail | 0 | 0 | 0 | 0 |
| not evaluated | 0 | 0 | 0 | 0 |
| unsupported | 0 | 0 | 0 | 0 |
| skipped | 0 | 0 | 3 | 3 |

## Evidence kinds

Pinned list rows:

| Evidence kind | Rows |
|---|---|
| pass | 0 |
| fail | 0 |
| not evaluated | 201 |
| skipped (reason) | 3 |
| unsupported (reason) | 0 |

Owned rows (a diagnostic is never counted as gameplay):

| Evidence kind | Rows |
|---|---|
| public gameplay | 27 |
| public boot only | 5 |
| model conformance | 14 |
| fail | 0 |
| not evaluated | 0 |
| skipped (reason) | 3 |
| unsupported (reason) | 0 |

## Claims

Each claim names its evidence class and the tests that exercise it. owned-oracle: expected values taken
from documents, hardware or RTL. model-only: mostly fitted values, so the model follows its documented
sequences without hardware calibration. public gameplay and public boot only: ledger pass rows of public
samples. none: no evidence. No real HSC ROM is exercised and PAL is not retail-verified. A class
describes the evidence, not a promise about any title.

| Kind | Claim | Evidence class | Backing tests |
|---|---|---|---|
| board | absolute | model-only | diag.retail |
| board | activision | owned-oracle | diag.retail |
| board | bank6_at_4000 | owned-oracle | diag.bank6 |
| board | bankset | owned-oracle | diag.bankset, diag.bankset_tracer |
| board | fractalus | owned-oracle | diag.retail |
| board | linear | owned-oracle | diag.linear16, title.simple.fast |
| board | ram | public gameplay | none |
| board | rom_at_4000 | owned-oracle | diag.sg144 |
| board | supergame | owned-oracle | diag.sg144, title.simple.fast |
| board | xboard | owned-oracle | diag.expansion |
| board | xm | owned-oracle | diag.expansion |
| controller | SNES2Atari and Mega7800 | model-only | diag.controllers |
| controller | XG-1 light gun | model-only | input.lightgun, diag.controllers |
| controller | driving | model-only | diag.controllers |
| controller | paddles and keypad | model-only | diag.controllers |
| controller | trak-ball and ST/Amiga motion | model-only | diag.controllers |
| peripheral | hsc | owned-oracle | core.hsc.decode, diag.battery |
| peripheral | lightgun | model-only | input.lightgun, diag.controllers |
| peripheral | pokey4000 | none | none |
| peripheral | pokey450 | public gameplay | none |
| peripheral | savekey | owned-oracle | input.savekey.protocol, libretro.host.battery |
| peripheral | xboard | owned-oracle | diag.expansion |
| peripheral | xm | owned-oracle | diag.expansion |
| region | NTSC | public gameplay | title.simple.fast |
| region | PAL | owned-oracle | diag.pal, runner.region.pal, title.pal.fast |
| region | any | none | none |

## Owned rows

| Id | Title | Board | Region | Peripherals | Predicate kind | Status |
|---|---|---|---|---|---|---|
| `7800basic-0.41/adventurer` | adventurer | linear | NTSC | - | title | pass |
| `7800basic-0.41/atarivoxspeech` | atarivoxspeech | linear | NTSC | - | title | pass |
| `7800basic-0.41/backuptest` | backuptest | linear | NTSC | - | boots | skipped (no-main-loop-jams) |
| `7800basic-0.41/banksets` | banksets | bankset | NTSC | - | boots | skipped (recipe-does-not-assemble) |
| `7800basic-0.41/banner` | banner | linear | NTSC | - | title | pass |
| `7800basic-0.41/boxcollision` | boxcollision | linear | NTSC | - | title | pass |
| `7800basic-0.41/colorfade` | colorfade | linear | NTSC | - | title | pass |
| `7800basic-0.41/doublebuffer` | doublebuffer | supergame+ram | NTSC | - | boots | pass |
| `7800basic-0.41/drivingcontrol` | drivingcontrol | linear | NTSC | - | title | pass |
| `7800basic-0.41/eightysix` | eightysix | supergame+ram | NTSC | pokey450 | boots | pass |
| `7800basic-0.41/fontcheck` | fontcheck | linear | NTSC | - | title | pass |
| `7800basic-0.41/helloworld` | helloworld | linear | NTSC | - | title | pass |
| `7800basic-0.41/hiscoretable` | hiscoretable | linear | NTSC | savekey | boots | pass |
| `7800basic-0.41/keypadcontrol` | keypadcontrol | linear | NTSC | - | title | pass |
| `7800basic-0.41/languagetest` | languagetest | linear | NTSC | - | boots | skipped (draws-nothing) |
| `7800basic-0.41/mega7800controllers` | mega7800controllers | linear | NTSC | - | title | pass |
| `7800basic-0.41/mousedemo` | mousedemo | linear | NTSC | - | title | pass |
| `7800basic-0.41/multibuttoncontrollers` | multibuttoncontrollers | linear | NTSC | - | title | pass |
| `7800basic-0.41/multisprite` | multisprite | linear | NTSC | - | boots | pass |
| `7800basic-0.41/onehundredandone` | onehundredandone | supergame+ram | NTSC | - | boots | pass |
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
| `diag/bankset` | muhtari bankset diagnostic | bankset | NTSC | - | mailbox | pass |
| `diag/cpu` | muhtari cpu diagnostic | linear | NTSC | - | title | pass |
| `diag/dli` | muhtari dli diagnostic | linear | NTSC | - | title | pass |
| `diag/dma` | muhtari dma diagnostic | linear | NTSC | - | title | pass |
| `diag/fractalus` | muhtari fractalus diagnostic | fractalus | NTSC | - | title | pass |
| `diag/linear16` | muhtari linear16 diagnostic | linear | NTSC | - | title | pass |
| `diag/modes` | muhtari modes diagnostic | supergame | NTSC | - | title | pass |
| `diag/pal` | muhtari PAL diagnostic | linear | PAL | pokey450 | mailbox | pass |
| `diag/sg144` | muhtari sg144 diagnostic | supergame+rom_at_4000 | NTSC | - | title | pass |
| `diag/xboard` | muhtari xboard diagnostic | linear+xboard | NTSC | pokey450+hsc | mailbox | pass |
| `diag/xm` | muhtari xm diagnostic | linear+xm | NTSC | pokey450+hsc | mailbox | pass |

## Pinned list rows

| Id | Title | Board | Region | Peripherals | Predicate kind | Status |
|---|---|---|---|---|---|---|
| `1on1` | One On One (PAL) | linear | PAL | - | boots | not evaluated |
| `1on1u` | One On One (NTSC) | linear | NTSC | - | boots | not evaluated |
| `aceoface` | Ace of Aces (PAL) | supergame | PAL | - | boots | not evaluated |
| `aceofaceu` | Ace of Aces (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `aerial` | Aerial | linear | any | - | boots | not evaluated |
| `alienbrg` | Alien Brigade (PAL) | supergame+rom_at_4000 | PAL | lightgun | boots | not evaluated |
| `alienbrgu` | Alien Brigade (NTSC) | supergame+rom_at_4000 | NTSC | lightgun | boots | not evaluated |
| `antiair` | AntiAir | linear | any | - | boots | not evaluated |
| `armora2` | Armor Attack II | linear | any | - | boots | not evaluated |
| `ascend` | Ascend | linear | any | - | boots | not evaluated |
| `astdelux` | Asteroids Deluxe (PAL) | linear | PAL | - | boots | not evaluated |
| `astdeluxu` | Asteroids Deluxe (NTSC) | linear | NTSC | - | boots | not evaluated |
| `asteroid` | Asteroids (NTSC) | linear | NTSC | - | boots | not evaluated |
| `astrob` | Astro Blaster (NTSC) | linear | NTSC | - | boots | not evaluated |
| `astrof` | Astro Fighter (NTSC) | linear | NTSC | - | boots | not evaluated |
| `awass` | Awass | linear | any | - | boots | not evaluated |
| `ballblaz` | Ballblazer (PAL) | linear | PAL | pokey4000 | boots | not evaluated |
| `ballblazu` | Ballblazer (NTSC) | linear | NTSC | pokey4000 | boots | not evaluated |
| `barnblst` | Barnyard Blaster (PAL) | supergame | PAL | lightgun | boots | not evaluated |
| `barnblstu` | Barnyard Blaster (NTSC) | supergame | NTSC | lightgun | boots | not evaluated |
| `baseball` | Baseball (PAL) | linear | PAL | - | boots | not evaluated |
| `baseballu` | Pete Rose Baseball (NTSC) | linear | NTSC | - | boots | not evaluated |
| `battlot` | Battlot | linear | any | - | boots | not evaluated |
| `beefdrop` | Beef Drop (NTSC /  added POKEY Chip) | linear | NTSC | pokey4000 | boots | not evaluated |
| `bentley` | Bentley Bear's Crystal Quest (NTSC) | supergame+rom_at_4000 | NTSC | pokey450 | boots | not evaluated |
| `bonq` | b*nQ (NTSC) | linear | NTSC | - | boots | not evaluated |
| `bootskell` | Bootskell | linear | any | - | boots | not evaluated |
| `bsktbrwl` | Basketbrawl (PAL) | supergame | PAL | - | boots | not evaluated |
| `bsktbrwlu` | Basketbrawl (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `cacorm` | Cacorm | linear | any | - | boots | not evaluated |
| `cavit` | Cavit | linear | any | - | boots | not evaluated |
| `centiped` | Centipede (PAL) | linear | PAL | - | boots | not evaluated |
| `centipedu` | Centipede (NTSC) | linear | NTSC | - | boots | not evaluated |
| `choplift` | Choplifter! (PAL) | linear | PAL | - | boots | not evaluated |
| `chopliftu` | Choplifter! (NTSC) | linear | NTSC | - | boots | not evaluated |
| `commando` | Commando (PAL) | supergame | PAL | pokey4000 | boots | not evaluated |
| `commandou` | Commando (NTSC) | supergame | NTSC | pokey4000 | boots | not evaluated |
| `cracked` | Crack'ed (PAL) | supergame | PAL | - | boots | not evaluated |
| `crackedu` | Crack'ed (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `cracky` | Cracky | linear | any | - | boots | not evaluated |
| `crazyotto` | Crazy Otto (PAL /  Ms. Pac-Man hack) | linear | PAL | - | boots | not evaluated |
| `crazyottou` | Crazy Otto (NTSC /  Ms. Pac-Man hack) | linear | NTSC | - | boots | not evaluated |
| `crossbow` | Crossbow (PAL) | supergame+rom_at_4000 | PAL | lightgun | boots | not evaluated |
| `crossbowu` | Crossbow (NTSC) | supergame+rom_at_4000 | NTSC | lightgun | boots | not evaluated |
| `darkcham` | Dark Chambers (PAL) | supergame | PAL | - | boots | not evaluated |
| `darkchamu` | Dark Chambers (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `ddragon` | Double Dragon (PAL) | activision | PAL | - | boots | not evaluated |
| `ddragonu` | Double Dragon (NTSC) | activision | NTSC | - | boots | not evaluated |
| `devcard` | 7800 Dev. Card | linear | any | pokey4000 | boots | not evaluated |
| `diagtest` | 7800 Pro System Diagnostic Test (v1.0) | linear | any | - | boots | not evaluated |
| `diagtestp` | 7800 Pro System Diagnostic Test (prototype) | linear | any | - | boots | not evaluated |
| `digdug` | Dig Dug (PAL) | linear | PAL | - | boots | not evaluated |
| `digdugu` | Dig Dug (NTSC) | linear | NTSC | - | boots | not evaluated |
| `dkong` | Donkey Kong (PAL) | linear | PAL | - | boots | not evaluated |
| `dkongjr` | Donkey Kong Jr. (PAL) | linear | PAL | - | boots | not evaluated |
| `dkongjru` | Donkey Kong Jr. (NTSC) | linear | NTSC | - | boots | not evaluated |
| `dkongu` | Donkey Kong (NTSC) | linear | NTSC | - | boots | not evaluated |
| `dkongxm` | Donkey Kong (PAL /  demo /  XM enhanced /  V1.2) | supergame+rom_at_4000 | PAL | - | boots | not evaluated |
| `dkongxmcc` | Donkey Kong (PAL/NTSC /  demo /  XM enhanced /  older /  for CC2 board) | supergame+rom_at_4000 | any | - | boots | not evaluated |
| `dkongxmo` | Donkey Kong (PAL /  demo /  XM enhanced /  older) | supergame+rom_at_4000 | PAL | - | boots | not evaluated |
| `dkongxmou` | Donkey Kong (NTSC /  demo /  XM enhanced /  older) | supergame+rom_at_4000 | NTSC | - | boots | not evaluated |
| `dkongxmps` | Donkey Kong (PAL /  demo /  XM enhanced /  V1.2 /  for ProSystem Emulator) | supergame+rom_at_4000 | PAL | - | boots | not evaluated |
| `dkongxmu` | Donkey Kong (NTSC /  demo /  XM enhanced /  V1.2) | supergame+rom_at_4000 | NTSC | - | boots | not evaluated |
| `dkongxmups` | Donkey Kong (NTSC /  demo /  XM enhanced /  V1.2 /  for ProSystem Emulator) | supergame+rom_at_4000 | NTSC | - | boots | not evaluated |
| `dsrtflcn` | Desert Falcon (PAL) | linear | PAL | - | boots | not evaluated |
| `dsrtflcna` | Desert Falcon (NTSC /  alternate) | linear | NTSC | - | boots | not evaluated |
| `dsrtflcnu` | Desert Falcon (NTSC) | linear | NTSC | - | boots | not evaluated |
| `f18` | F-18 Hornet (PAL) | absolute | PAL | - | boots | not evaluated |
| `f18u` | F-18 Hornet (NTSC) | absolute | NTSC | - | boots | not evaluated |
| `failsafe` | FailSafe (PAL) | linear | PAL | - | boots | not evaluated |
| `failsafeu` | FailSafe (NTSC) | linear | NTSC | - | boots | not evaluated |
| `fatalrun` | Fatal Run (PAL) | supergame | PAL | - | boots | not evaluated |
| `fatalrunu` | Fatal Run (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `fightn` | Fight Night (PAL) | supergame | PAL | - | boots | not evaluated |
| `fightnu` | Fight Night (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `foodf` | Food Fight (PAL) | linear | PAL | - | boots | not evaluated |
| `foodfu` | Food Fight (NTSC) | linear | NTSC | - | boots | not evaluated |
| `foodfup` | Food Fight (NTSC /  prototype) | linear | NTSC | - | boots | not evaluated |
| `fractal` | Rescue on Fractalus (NTSC /  prototype) | fractalus | NTSC | - | boots | not evaluated |
| `froggie` | Froggie | linear | any | pokey4000 | boots | not evaluated |
| `galaga` | Galaga (PAL) | linear | PAL | - | boots | not evaluated |
| `galagau` | Galaga (NTSC) | linear | NTSC | - | boots | not evaluated |
| `gatop1` | GATO (NTSC /  prototype /  198801xx) | linear | NTSC | - | boots | not evaluated |
| `gatop2` | GATO (NTSC /  prototype /  198712xx) | linear | NTSC | - | boots | not evaluated |
| `gatop3` | GATO (NTSC /  prototype /  198711xx) | linear | NTSC | - | boots | not evaluated |
| `gatop4` | GATO (NTSC /  prototype /  19870727) | linear | NTSC | - | boots | not evaluated |
| `gatop5` | GATO (NTSC /  prototype /  19870623) | linear | NTSC | - | boots | not evaluated |
| `gatop6` | GATO (NTSC /  prototype /  19870611) | linear | NTSC | - | boots | not evaluated |
| `gatop7` | GATO (NTSC /  prototype /  19870523) | linear | NTSC | - | boots | not evaluated |
| `gatop8` | GATO (NTSC /  prototype /  19870424) | linear | NTSC | - | boots | not evaluated |
| `guntus` | Guntus | linear | any | - | boots | not evaluated |
| `hangly` | Hangly-Man (NTSC) | linear | NTSC | - | boots | not evaluated |
| `hattrick` | Hat Trick (PAL) | linear | PAL | - | boots | not evaluated |
| `hattricku` | Hat Trick (NTSC) | linear | NTSC | - | boots | not evaluated |
| `hiscore` | High Score Cartridge | - | any | hsc | boots | skipped (expansion-device) |
| `hopman` | Hopman | linear | any | - | boots | not evaluated |
| `ikari` | Ikari Warriors (PAL) | supergame | PAL | - | boots | not evaluated |
| `ikariu` | Ikari Warriors (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `impetus` | Impetus | linear | any | - | boots | not evaluated |
| `impmiss` | Impossible Mission (PAL) | supergame+ram | PAL | - | boots | not evaluated |
| `impmissbf` | Impossible Mission (NTSC /  bug-fix) | supergame+ram | NTSC | - | boots | not evaluated |
| `impmissu` | Impossible Mission (NTSC) | supergame+ram | NTSC | - | boots | not evaluated |
| `jinks` | Jinks (PAL) | supergame+ram | PAL | - | boots | not evaluated |
| `jinksu` | Jinks (NTSC) | supergame+ram | NTSC | - | boots | not evaluated |
| `joust` | Joust (PAL) | linear | PAL | - | boots | not evaluated |
| `joustu` | Joust (NTSC) | linear | NTSC | - | boots | not evaluated |
| `jrpacman` | Jr. Pac-Man (NTSC) | linear | NTSC | - | boots | not evaluated |
| `karateka` | Karateka (PAL) | supergame | PAL | - | boots | not evaluated |
| `karatekau` | Karateka (NTSC) | linear | NTSC | - | boots | not evaluated |
| `kcmunch` | K.C. Munchkin! | linear | any | - | boots | not evaluated |
| `klax` | Klax (NTSC /  prototype) | supergame | NTSC | - | boots | not evaluated |
| `kungfum` | Kung-Fu Master (PAL) | linear | PAL | - | boots | not evaluated |
| `kungfumu` | Kung-Fu Master (NTSC) | linear | NTSC | - | boots | not evaluated |
| `lift` | Lift | linear | any | - | boots | not evaluated |
| `mario` | Mario Bros. (PAL) | linear | PAL | - | boots | not evaluated |
| `mariou` | Mario Bros. (NTSC) | linear | NTSC | - | boots | not evaluated |
| `matmania` | Mat Mania Challenge (PAL) | supergame | PAL | - | boots | not evaluated |
| `matmaniau` | Mat Mania Challenge (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `mazy` | Mazy | linear | any | - | boots | not evaluated |
| `mazy2` | Mazy 2 | linear | any | - | boots | not evaluated |
| `mean18` | Mean 18 (PAL) | supergame | PAL | - | boots | not evaluated |
| `mean18u` | Mean 18 (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `meltdown` | Meltdown (PAL) | supergame | PAL | lightgun | boots | not evaluated |
| `meltdownu` | Meltdown (NTSC) | supergame | NTSC | lightgun | boots | not evaluated |
| `meteorsh` | Meteor Shower (PAL) | linear | PAL | - | boots | not evaluated |
| `meteorshu` | Meteor Shower (NTSC) | linear | NTSC | - | boots | not evaluated |
| `mia` | Missing In Action (prototype) | supergame+rom_at_4000 | any | - | boots | not evaluated |
| `midnight` | Midnight Mutants (PAL) | supergame | PAL | - | boots | not evaluated |
| `midnightu` | Midnight Mutants (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `mieyen` | Mieyen | linear | any | - | boots | not evaluated |
| `mooncrst` | Moon Cresta (PAL) | linear | PAL | - | boots | not evaluated |
| `mooncrstu` | Moon Cresta (NTSC) | linear | NTSC | - | boots | not evaluated |
| `mpsycho` | Motor Psycho (PAL) | supergame | PAL | - | boots | not evaluated |
| `mpsychou` | Motor Psycho (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `mspacatk` | Ms. Pac-Attack (NTSC) | linear | NTSC | - | boots | not evaluated |
| `mspacman` | Ms. Pac-Man (PAL) | linear | PAL | - | boots | not evaluated |
| `mspacmanu` | Ms. Pac-Man (NTSC) | linear | NTSC | - | boots | not evaluated |
| `neuras` | Neuras | linear | any | - | boots | not evaluated |
| `ninjaglf` | Ninja Golf (PAL) | supergame | PAL | - | boots | not evaluated |
| `ninjaglfu` | Ninja Golf (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `osotos` | Osotos | linear | any | - | boots | not evaluated |
| `paccoll` | Pac-Man Collection! (PAL) | linear | PAL | - | boots | not evaluated |
| `paccollu` | Pac-Man Collection! (NTSC) | linear | NTSC | - | boots | not evaluated |
| `pacman` | Pac-Man (NTSC) | linear | NTSC | - | boots | not evaluated |
| `pacplus` | Pac-Man Plus (NTSC) | linear | NTSC | - | boots | not evaluated |
| `pacpolux` | Pac-Pollux (NTSC) | linear | NTSC | - | boots | not evaluated |
| `pitfight` | Pit Fighter (prototype) | supergame | any | - | boots | not evaluated |
| `planetsm` | Planet Smashers (PAL) | supergame | PAL | - | boots | not evaluated |
| `planetsmu` | Planet Smashers (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `plutos` | Plutos (prototype) | supergame+ram | any | - | boots | not evaluated |
| `polepos2` | Pole Position II (PAL) | linear | PAL | - | boots | not evaluated |
| `polepos2p` | Pole Position II (prototype) | linear | any | - | boots | not evaluated |
| `polepos2u` | Pole Position II (NTSC) | linear | NTSC | - | boots | not evaluated |
| `prowres` | Title Match Pro Wrestling (PAL) | linear | PAL | - | boots | not evaluated |
| `prowresu` | Title Match Pro Wrestling (NTSC) | linear | NTSC | - | boots | not evaluated |
| `rampage` | Rampage (NTSC) | activision | NTSC | - | boots | not evaluated |
| `rampart` | Rampart (prototype) | supergame | any | - | boots | not evaluated |
| `readriot` | Read RIOT | linear | any | - | boots | not evaluated |
| `ripoff` | Rip-Off (PAL) | linear | PAL | - | boots | not evaluated |
| `ripoffu` | Rip-Off (NTSC) | linear | NTSC | - | boots | not evaluated |
| `robotron` | Robotron: 2084 (NTSC) | linear | NTSC | - | boots | not evaluated |
| `rsbaseb` | RealSports Baseball (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `ruptus` | Ruptus | linear | any | - | boots | not evaluated |
| `santasim` | Santa Simon (NTSC) | linear | NTSC | - | boots | not evaluated |
| `scramble` | Scramble (NTSC) | linear | NTSC | - | boots | not evaluated |
| `scrapdog` | Scrapyard Dog (PAL) | supergame | PAL | - | boots | not evaluated |
| `scrapdogu` | Scrapyard Dog (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `sentinel` | Sentinel (PAL) | supergame | PAL | lightgun | boots | not evaluated |
| `sentinelu` | Sentinel (NTSC) | supergame | NTSC | lightgun | boots | not evaluated |
| `sirius` | Sirius (prototype) | supergame+ram | any | - | boots | not evaluated |
| `spacduel` | Space Duel (PAL) | linear | PAL | - | boots | not evaluated |
| `spacduelu` | Space Duel (NTSC) | linear | NTSC | - | boots | not evaluated |
| `spaceinv` | Space Invader (PAL) | linear | PAL | - | boots | not evaluated |
| `spaceinvu` | Space Invader (NTSC) | linear | NTSC | - | boots | not evaluated |
| `starwars` | Star Wars 7800 (PAL /  hack) | linear | PAL | - | boots | not evaluated |
| `starwarsu` | Star Wars 7800 (NTSC /  hack) | linear | NTSC | - | boots | not evaluated |
| `sumgames` | Summer Games (NTSC) | supergame+ram | NTSC | - | boots | not evaluated |
| `superpac` | Super Pac-Man (PAL) | linear | PAL | - | boots | not evaluated |
| `superpacu` | Super Pac-Man (NTSC) | linear | NTSC | - | boots | not evaluated |
| `suphuey` | Super Huey (PAL) | linear | PAL | - | boots | not evaluated |
| `suphueyu` | Super Huey (NTSC) | linear | NTSC | - | boots | not evaluated |
| `supskate` | Super Skateboardin' (PAL) | linear | PAL | - | boots | not evaluated |
| `supskateu` | Super Skateboardin' (NTSC) | linear | NTSC | - | boots | not evaluated |
| `svellas` | Svellas | linear | any | - | boots | not evaluated |
| `tank` | Tank Command (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `tdown` | Touchdown Football (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `tomcat` | Tomcat - The F-14 Fighter Simulator (PAL) | linear | PAL | - | boots | not evaluated |
| `tomcatu` | Tomcat - The F-14 Fighter Simulator (NTSC) | linear | NTSC | - | boots | not evaluated |
| `towertop` | Tower Toppler (PAL) | supergame+ram | PAL | - | boots | not evaluated |
| `towertopu` | Tower Toppler (NTSC) | supergame+ram | NTSC | - | boots | not evaluated |
| `ultrapac` | Ultra Pac-Man (NTSC) | linear | NTSC | - | boots | not evaluated |
| `wasp` | Wasp! | linear | any | - | boots | not evaluated |
| `waterski` | Water Ski (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `wingames` | Winter Games (NTSC) | supergame+ram | NTSC | - | boots | not evaluated |
| `wingameso` | Winter Games (NTSC /  older?) | supergame+ram | NTSC | - | boots | not evaluated |
| `worm` | Worm! | linear | any | - | boots | not evaluated |
| `xboard` | XBoarD Expansion | - | any | xboard | boots | skipped (expansion-device) |
| `xenophob` | Xenophobe (PAL) | supergame | PAL | - | boots | not evaluated |
| `xenophobu` | Xenophobe (NTSC) | supergame | NTSC | - | boots | not evaluated |
| `xevious` | Xevious (PAL) | linear | PAL | - | boots | not evaluated |
| `xeviousu` | Xevious (NTSC) | linear | NTSC | - | boots | not evaluated |
| `xeviousup` | Xevious (NTSC /  prototype) | linear | NTSC | - | boots | not evaluated |
| `xm` | XM Expansion Module | - | any | xm | boots | skipped (expansion-device) |
| `yewdow` | Yewdow | linear | any | - | boots | not evaluated |
