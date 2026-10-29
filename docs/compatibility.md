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

The ledger has 253 rows. 212 are judged by the weak predicate `boots@F` (the game booted, drew and
kept running), not by a pinned title screen.

| Status | Rows |
|---|---|
| pass | 46 |
| skipped | 6 |
| unknown | 201 |

| Board | Rows |
|---|---|
| - | 3 |
| absolute | 3 |
| activision | 4 |
| bankset | 2 |
| fractalus | 2 |
| linear | 158 |
| linear+xboard | 1 |
| linear+xm | 1 |
| supergame | 47 |
| supergame+bank6_at_4000 | 1 |
| supergame+ram | 17 |
| supergame+rom_at_4000 | 14 |

| Region | Rows |
|---|---|
| NTSC | 149 |
| PAL | 64 |
| any | 40 |

| Id | Title | Board | Region | Peripherals | Predicate | Status |
|---|---|---|---|---|---|---|
| `1on1` | One On One (PAL) | linear | PAL | - | boots | unknown |
| `1on1u` | One On One (NTSC) | linear | NTSC | - | boots | unknown |
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
| `aceoface` | Ace of Aces (PAL) | supergame | PAL | - | boots | unknown |
| `aceofaceu` | Ace of Aces (NTSC) | supergame | NTSC | - | boots | unknown |
| `aerial` | Aerial | linear | any | - | boots | unknown |
| `alienbrg` | Alien Brigade (PAL) | supergame+rom_at_4000 | PAL | lightgun | boots | unknown |
| `alienbrgu` | Alien Brigade (NTSC) | supergame+rom_at_4000 | NTSC | lightgun | boots | unknown |
| `antiair` | AntiAir | linear | any | - | boots | unknown |
| `armora2` | Armor Attack II | linear | any | - | boots | unknown |
| `ascend` | Ascend | linear | any | - | boots | unknown |
| `astdelux` | Asteroids Deluxe (PAL) | linear | PAL | - | boots | unknown |
| `astdeluxu` | Asteroids Deluxe (NTSC) | linear | NTSC | - | boots | unknown |
| `asteroid` | Asteroids (NTSC) | linear | NTSC | - | boots | unknown |
| `astrob` | Astro Blaster (NTSC) | linear | NTSC | - | boots | unknown |
| `astrof` | Astro Fighter (NTSC) | linear | NTSC | - | boots | unknown |
| `awass` | Awass | linear | any | - | boots | unknown |
| `ballblaz` | Ballblazer (PAL) | linear | PAL | pokey4000 | boots | unknown |
| `ballblazu` | Ballblazer (NTSC) | linear | NTSC | pokey4000 | boots | unknown |
| `barnblst` | Barnyard Blaster (PAL) | supergame | PAL | lightgun | boots | unknown |
| `barnblstu` | Barnyard Blaster (NTSC) | supergame | NTSC | lightgun | boots | unknown |
| `baseball` | Baseball (PAL) | linear | PAL | - | boots | unknown |
| `baseballu` | Pete Rose Baseball (NTSC) | linear | NTSC | - | boots | unknown |
| `battlot` | Battlot | linear | any | - | boots | unknown |
| `beefdrop` | Beef Drop (NTSC /  added POKEY Chip) | linear | NTSC | pokey4000 | boots | unknown |
| `bentley` | Bentley Bear's Crystal Quest (NTSC) | supergame+rom_at_4000 | NTSC | pokey450 | boots | unknown |
| `bonq` | b*nQ (NTSC) | linear | NTSC | - | boots | unknown |
| `bootskell` | Bootskell | linear | any | - | boots | unknown |
| `bsktbrwl` | Basketbrawl (PAL) | supergame | PAL | - | boots | unknown |
| `bsktbrwlu` | Basketbrawl (NTSC) | supergame | NTSC | - | boots | unknown |
| `cacorm` | Cacorm | linear | any | - | boots | unknown |
| `cavit` | Cavit | linear | any | - | boots | unknown |
| `centiped` | Centipede (PAL) | linear | PAL | - | boots | unknown |
| `centipedu` | Centipede (NTSC) | linear | NTSC | - | boots | unknown |
| `choplift` | Choplifter! (PAL) | linear | PAL | - | boots | unknown |
| `chopliftu` | Choplifter! (NTSC) | linear | NTSC | - | boots | unknown |
| `commando` | Commando (PAL) | supergame | PAL | pokey4000 | boots | unknown |
| `commandou` | Commando (NTSC) | supergame | NTSC | pokey4000 | boots | unknown |
| `cracked` | Crack'ed (PAL) | supergame | PAL | - | boots | unknown |
| `crackedu` | Crack'ed (NTSC) | supergame | NTSC | - | boots | unknown |
| `cracky` | Cracky | linear | any | - | boots | unknown |
| `crazyotto` | Crazy Otto (PAL /  Ms. Pac-Man hack) | linear | PAL | - | boots | unknown |
| `crazyottou` | Crazy Otto (NTSC /  Ms. Pac-Man hack) | linear | NTSC | - | boots | unknown |
| `crossbow` | Crossbow (PAL) | supergame+rom_at_4000 | PAL | lightgun | boots | unknown |
| `crossbowu` | Crossbow (NTSC) | supergame+rom_at_4000 | NTSC | lightgun | boots | unknown |
| `darkcham` | Dark Chambers (PAL) | supergame | PAL | - | boots | unknown |
| `darkchamu` | Dark Chambers (NTSC) | supergame | NTSC | - | boots | unknown |
| `ddragon` | Double Dragon (PAL) | activision | PAL | - | boots | unknown |
| `ddragonu` | Double Dragon (NTSC) | activision | NTSC | - | boots | unknown |
| `devcard` | 7800 Dev. Card | linear | any | pokey4000 | boots | unknown |
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
| `diagtest` | 7800 Pro System Diagnostic Test (v1.0) | linear | any | - | boots | unknown |
| `diagtestp` | 7800 Pro System Diagnostic Test (prototype) | linear | any | - | boots | unknown |
| `digdug` | Dig Dug (PAL) | linear | PAL | - | boots | unknown |
| `digdugu` | Dig Dug (NTSC) | linear | NTSC | - | boots | unknown |
| `dkong` | Donkey Kong (PAL) | linear | PAL | - | boots | unknown |
| `dkongjr` | Donkey Kong Jr. (PAL) | linear | PAL | - | boots | unknown |
| `dkongjru` | Donkey Kong Jr. (NTSC) | linear | NTSC | - | boots | unknown |
| `dkongu` | Donkey Kong (NTSC) | linear | NTSC | - | boots | unknown |
| `dkongxm` | Donkey Kong (PAL /  demo /  XM enhanced /  V1.2) | supergame+rom_at_4000 | PAL | - | boots | unknown |
| `dkongxmcc` | Donkey Kong (PAL/NTSC /  demo /  XM enhanced /  older /  for CC2 board) | supergame+rom_at_4000 | any | - | boots | unknown |
| `dkongxmo` | Donkey Kong (PAL /  demo /  XM enhanced /  older) | supergame+rom_at_4000 | PAL | - | boots | unknown |
| `dkongxmou` | Donkey Kong (NTSC /  demo /  XM enhanced /  older) | supergame+rom_at_4000 | NTSC | - | boots | unknown |
| `dkongxmps` | Donkey Kong (PAL /  demo /  XM enhanced /  V1.2 /  for ProSystem Emulator) | supergame+rom_at_4000 | PAL | - | boots | unknown |
| `dkongxmu` | Donkey Kong (NTSC /  demo /  XM enhanced /  V1.2) | supergame+rom_at_4000 | NTSC | - | boots | unknown |
| `dkongxmups` | Donkey Kong (NTSC /  demo /  XM enhanced /  V1.2 /  for ProSystem Emulator) | supergame+rom_at_4000 | NTSC | - | boots | unknown |
| `dsrtflcn` | Desert Falcon (PAL) | linear | PAL | - | boots | unknown |
| `dsrtflcna` | Desert Falcon (NTSC /  alternate) | linear | NTSC | - | boots | unknown |
| `dsrtflcnu` | Desert Falcon (NTSC) | linear | NTSC | - | boots | unknown |
| `f18` | F-18 Hornet (PAL) | absolute | PAL | - | boots | unknown |
| `f18u` | F-18 Hornet (NTSC) | absolute | NTSC | - | boots | unknown |
| `failsafe` | FailSafe (PAL) | linear | PAL | - | boots | unknown |
| `failsafeu` | FailSafe (NTSC) | linear | NTSC | - | boots | unknown |
| `fatalrun` | Fatal Run (PAL) | supergame | PAL | - | boots | unknown |
| `fatalrunu` | Fatal Run (NTSC) | supergame | NTSC | - | boots | unknown |
| `fightn` | Fight Night (PAL) | supergame | PAL | - | boots | unknown |
| `fightnu` | Fight Night (NTSC) | supergame | NTSC | - | boots | unknown |
| `foodf` | Food Fight (PAL) | linear | PAL | - | boots | unknown |
| `foodfu` | Food Fight (NTSC) | linear | NTSC | - | boots | unknown |
| `foodfup` | Food Fight (NTSC /  prototype) | linear | NTSC | - | boots | unknown |
| `fractal` | Rescue on Fractalus (NTSC /  prototype) | fractalus | NTSC | - | boots | unknown |
| `froggie` | Froggie | linear | any | pokey4000 | boots | unknown |
| `galaga` | Galaga (PAL) | linear | PAL | - | boots | unknown |
| `galagau` | Galaga (NTSC) | linear | NTSC | - | boots | unknown |
| `gatop1` | GATO (NTSC /  prototype /  198801xx) | linear | NTSC | - | boots | unknown |
| `gatop2` | GATO (NTSC /  prototype /  198712xx) | linear | NTSC | - | boots | unknown |
| `gatop3` | GATO (NTSC /  prototype /  198711xx) | linear | NTSC | - | boots | unknown |
| `gatop4` | GATO (NTSC /  prototype /  19870727) | linear | NTSC | - | boots | unknown |
| `gatop5` | GATO (NTSC /  prototype /  19870623) | linear | NTSC | - | boots | unknown |
| `gatop6` | GATO (NTSC /  prototype /  19870611) | linear | NTSC | - | boots | unknown |
| `gatop7` | GATO (NTSC /  prototype /  19870523) | linear | NTSC | - | boots | unknown |
| `gatop8` | GATO (NTSC /  prototype /  19870424) | linear | NTSC | - | boots | unknown |
| `guntus` | Guntus | linear | any | - | boots | unknown |
| `hangly` | Hangly-Man (NTSC) | linear | NTSC | - | boots | unknown |
| `hattrick` | Hat Trick (PAL) | linear | PAL | - | boots | unknown |
| `hattricku` | Hat Trick (NTSC) | linear | NTSC | - | boots | unknown |
| `hiscore` | High Score Cartridge | - | any | hsc | boots (expansion-device) | skipped |
| `hopman` | Hopman | linear | any | - | boots | unknown |
| `ikari` | Ikari Warriors (PAL) | supergame | PAL | - | boots | unknown |
| `ikariu` | Ikari Warriors (NTSC) | supergame | NTSC | - | boots | unknown |
| `impetus` | Impetus | linear | any | - | boots | unknown |
| `impmiss` | Impossible Mission (PAL) | supergame+ram | PAL | - | boots | unknown |
| `impmissbf` | Impossible Mission (NTSC /  bug-fix) | supergame+ram | NTSC | - | boots | unknown |
| `impmissu` | Impossible Mission (NTSC) | supergame+ram | NTSC | - | boots | unknown |
| `jinks` | Jinks (PAL) | supergame+ram | PAL | - | boots | unknown |
| `jinksu` | Jinks (NTSC) | supergame+ram | NTSC | - | boots | unknown |
| `joust` | Joust (PAL) | linear | PAL | - | boots | unknown |
| `joustu` | Joust (NTSC) | linear | NTSC | - | boots | unknown |
| `jrpacman` | Jr. Pac-Man (NTSC) | linear | NTSC | - | boots | unknown |
| `karateka` | Karateka (PAL) | supergame | PAL | - | boots | unknown |
| `karatekau` | Karateka (NTSC) | linear | NTSC | - | boots | unknown |
| `kcmunch` | K.C. Munchkin! | linear | any | - | boots | unknown |
| `klax` | Klax (NTSC /  prototype) | supergame | NTSC | - | boots | unknown |
| `kungfum` | Kung-Fu Master (PAL) | linear | PAL | - | boots | unknown |
| `kungfumu` | Kung-Fu Master (NTSC) | linear | NTSC | - | boots | unknown |
| `lift` | Lift | linear | any | - | boots | unknown |
| `mario` | Mario Bros. (PAL) | linear | PAL | - | boots | unknown |
| `mariou` | Mario Bros. (NTSC) | linear | NTSC | - | boots | unknown |
| `matmania` | Mat Mania Challenge (PAL) | supergame | PAL | - | boots | unknown |
| `matmaniau` | Mat Mania Challenge (NTSC) | supergame | NTSC | - | boots | unknown |
| `mazy` | Mazy | linear | any | - | boots | unknown |
| `mazy2` | Mazy 2 | linear | any | - | boots | unknown |
| `mean18` | Mean 18 (PAL) | supergame | PAL | - | boots | unknown |
| `mean18u` | Mean 18 (NTSC) | supergame | NTSC | - | boots | unknown |
| `meltdown` | Meltdown (PAL) | supergame | PAL | lightgun | boots | unknown |
| `meltdownu` | Meltdown (NTSC) | supergame | NTSC | lightgun | boots | unknown |
| `meteorsh` | Meteor Shower (PAL) | linear | PAL | - | boots | unknown |
| `meteorshu` | Meteor Shower (NTSC) | linear | NTSC | - | boots | unknown |
| `mia` | Missing In Action (prototype) | supergame+rom_at_4000 | any | - | boots | unknown |
| `midnight` | Midnight Mutants (PAL) | supergame | PAL | - | boots | unknown |
| `midnightu` | Midnight Mutants (NTSC) | supergame | NTSC | - | boots | unknown |
| `mieyen` | Mieyen | linear | any | - | boots | unknown |
| `mooncrst` | Moon Cresta (PAL) | linear | PAL | - | boots | unknown |
| `mooncrstu` | Moon Cresta (NTSC) | linear | NTSC | - | boots | unknown |
| `mpsycho` | Motor Psycho (PAL) | supergame | PAL | - | boots | unknown |
| `mpsychou` | Motor Psycho (NTSC) | supergame | NTSC | - | boots | unknown |
| `mspacatk` | Ms. Pac-Attack (NTSC) | linear | NTSC | - | boots | unknown |
| `mspacman` | Ms. Pac-Man (PAL) | linear | PAL | - | boots | unknown |
| `mspacmanu` | Ms. Pac-Man (NTSC) | linear | NTSC | - | boots | unknown |
| `neuras` | Neuras | linear | any | - | boots | unknown |
| `ninjaglf` | Ninja Golf (PAL) | supergame | PAL | - | boots | unknown |
| `ninjaglfu` | Ninja Golf (NTSC) | supergame | NTSC | - | boots | unknown |
| `osotos` | Osotos | linear | any | - | boots | unknown |
| `paccoll` | Pac-Man Collection! (PAL) | linear | PAL | - | boots | unknown |
| `paccollu` | Pac-Man Collection! (NTSC) | linear | NTSC | - | boots | unknown |
| `pacman` | Pac-Man (NTSC) | linear | NTSC | - | boots | unknown |
| `pacplus` | Pac-Man Plus (NTSC) | linear | NTSC | - | boots | unknown |
| `pacpolux` | Pac-Pollux (NTSC) | linear | NTSC | - | boots | unknown |
| `pitfight` | Pit Fighter (prototype) | supergame | any | - | boots | unknown |
| `planetsm` | Planet Smashers (PAL) | supergame | PAL | - | boots | unknown |
| `planetsmu` | Planet Smashers (NTSC) | supergame | NTSC | - | boots | unknown |
| `plutos` | Plutos (prototype) | supergame+ram | any | - | boots | unknown |
| `polepos2` | Pole Position II (PAL) | linear | PAL | - | boots | unknown |
| `polepos2p` | Pole Position II (prototype) | linear | any | - | boots | unknown |
| `polepos2u` | Pole Position II (NTSC) | linear | NTSC | - | boots | unknown |
| `prowres` | Title Match Pro Wrestling (PAL) | linear | PAL | - | boots | unknown |
| `prowresu` | Title Match Pro Wrestling (NTSC) | linear | NTSC | - | boots | unknown |
| `rampage` | Rampage (NTSC) | activision | NTSC | - | boots | unknown |
| `rampart` | Rampart (prototype) | supergame | any | - | boots | unknown |
| `readriot` | Read RIOT | linear | any | - | boots | unknown |
| `ripoff` | Rip-Off (PAL) | linear | PAL | - | boots | unknown |
| `ripoffu` | Rip-Off (NTSC) | linear | NTSC | - | boots | unknown |
| `robotron` | Robotron: 2084 (NTSC) | linear | NTSC | - | boots | unknown |
| `rsbaseb` | RealSports Baseball (NTSC) | supergame | NTSC | - | boots | unknown |
| `ruptus` | Ruptus | linear | any | - | boots | unknown |
| `santasim` | Santa Simon (NTSC) | linear | NTSC | - | boots | unknown |
| `scramble` | Scramble (NTSC) | linear | NTSC | - | boots | unknown |
| `scrapdog` | Scrapyard Dog (PAL) | supergame | PAL | - | boots | unknown |
| `scrapdogu` | Scrapyard Dog (NTSC) | supergame | NTSC | - | boots | unknown |
| `sentinel` | Sentinel (PAL) | supergame | PAL | lightgun | boots | unknown |
| `sentinelu` | Sentinel (NTSC) | supergame | NTSC | lightgun | boots | unknown |
| `sirius` | Sirius (prototype) | supergame+ram | any | - | boots | unknown |
| `spacduel` | Space Duel (PAL) | linear | PAL | - | boots | unknown |
| `spacduelu` | Space Duel (NTSC) | linear | NTSC | - | boots | unknown |
| `spaceinv` | Space Invader (PAL) | linear | PAL | - | boots | unknown |
| `spaceinvu` | Space Invader (NTSC) | linear | NTSC | - | boots | unknown |
| `starwars` | Star Wars 7800 (PAL /  hack) | linear | PAL | - | boots | unknown |
| `starwarsu` | Star Wars 7800 (NTSC /  hack) | linear | NTSC | - | boots | unknown |
| `sumgames` | Summer Games (NTSC) | supergame+ram | NTSC | - | boots | unknown |
| `superpac` | Super Pac-Man (PAL) | linear | PAL | - | boots | unknown |
| `superpacu` | Super Pac-Man (NTSC) | linear | NTSC | - | boots | unknown |
| `suphuey` | Super Huey (PAL) | linear | PAL | - | boots | unknown |
| `suphueyu` | Super Huey (NTSC) | linear | NTSC | - | boots | unknown |
| `supskate` | Super Skateboardin' (PAL) | linear | PAL | - | boots | unknown |
| `supskateu` | Super Skateboardin' (NTSC) | linear | NTSC | - | boots | unknown |
| `svellas` | Svellas | linear | any | - | boots | unknown |
| `tank` | Tank Command (NTSC) | supergame | NTSC | - | boots | unknown |
| `tdown` | Touchdown Football (NTSC) | supergame | NTSC | - | boots | unknown |
| `tomcat` | Tomcat - The F-14 Fighter Simulator (PAL) | linear | PAL | - | boots | unknown |
| `tomcatu` | Tomcat - The F-14 Fighter Simulator (NTSC) | linear | NTSC | - | boots | unknown |
| `towertop` | Tower Toppler (PAL) | supergame+ram | PAL | - | boots | unknown |
| `towertopu` | Tower Toppler (NTSC) | supergame+ram | NTSC | - | boots | unknown |
| `ultrapac` | Ultra Pac-Man (NTSC) | linear | NTSC | - | boots | unknown |
| `wasp` | Wasp! | linear | any | - | boots | unknown |
| `waterski` | Water Ski (NTSC) | supergame | NTSC | - | boots | unknown |
| `wingames` | Winter Games (NTSC) | supergame+ram | NTSC | - | boots | unknown |
| `wingameso` | Winter Games (NTSC /  older?) | supergame+ram | NTSC | - | boots | unknown |
| `worm` | Worm! | linear | any | - | boots | unknown |
| `xboard` | XBoarD Expansion | - | any | xboard | boots (expansion-device) | skipped |
| `xenophob` | Xenophobe (PAL) | supergame | PAL | - | boots | unknown |
| `xenophobu` | Xenophobe (NTSC) | supergame | NTSC | - | boots | unknown |
| `xevious` | Xevious (PAL) | linear | PAL | - | boots | unknown |
| `xeviousu` | Xevious (NTSC) | linear | NTSC | - | boots | unknown |
| `xeviousup` | Xevious (NTSC /  prototype) | linear | NTSC | - | boots | unknown |
| `xm` | XM Expansion Module | - | any | xm | boots (expansion-device) | skipped |
| `yewdow` | Yewdow | linear | any | - | boots | unknown |
