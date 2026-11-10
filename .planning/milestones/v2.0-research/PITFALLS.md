# Pitfalls research — v2.0

Researched: 2026-10-21. Sources: preparation references and current implementation; primary EEPROM and input sources linked in STACK.md.

| Failure | Prevention and acceptance | Address during |
|---|---|---|
| A recognized header mistaken for support | Run bus-visible board/device diagnostics, preserve explicit refusals | Each capability |
| Activision AM/OM layouts confused | Normalize only identified geometries/layouts at load; equivalent mapped reads and separate raw identities | Retail boards |
| Fractalus mirror indexes beyond 2K | Mask physical RAM address; sweep aliases and hostile traffic | Retail boards |
| BankSet flips at an approximate DMA boundary | Explicit CPU/MARIA ownership and tests at both HALT edges | Homebrew boards |
| EEPROM immediately commits bytes or ignores page wrap | Buffer through STOP, wrap low six address bits, busy NACK on emulated time, sequential/random read tests | Battery |
| Raw saves get a new header or unstable pointer | Layout version in metadata, fixed span from load to unload; exact-length interop checks | Battery |
| Signature-only HSC advertised as full firmware | Document supported direct-SRAM callers; user ROM path tests; no Atari ROM or disassembly opened | Battery |
| Atomic save failure destroys prior bytes | Temporary-file and replace error tests; signals request flush through normal control flow | Runner persistence |
| Lightgun hits dark pixels or wrong PAL row | Sensor integrates beam geometry, luma and explicit fit parameters; blanking/dark/off-screen controls | Controllers/PAL |
| Keypad/analog ports bypass DDR and TIA latch | Device pin model, register-access timing diagnostics, ghost/RC behavior qualified as fitted | Controllers |
| PAL picture works but audio/frontend stay NTSC | One region descriptor and sustained A/V sample-carry and geometry checks | PAL |
| Whole-library headline becomes unsupported blanket accuracy claim | Every pinned metadata entry has explicit status and evidence; local titles never silently skip or count as public passes | Compatibility |
| Pipeline fixes outgrow emulator | Reuse existing gate outputs, retire overlap with stronger signal named, honor tenth-of-src and bounded reviews | Tune-up |

## Fresh operational evidence

Archive PR18 run 39611531779, macos-x64 job 119261437519: 604/605 regular tests passed; audio.samples timed out at 120.11 s after seven sample audit rows. This is a timeout observation, not proof of flakiness or an incorrect audio hash. The first attempt also required inspection of the BE lane before concluding. Job rerun was refused while the parent run was active; a failed-jobs retry was requested after completion. No timeout was increased without measurements. Run: https://github.com/szTheory/muhtari/actions/runs/39611531779

The timeout correction (bcdc89f) provisionally allows 240 seconds without changing comparisons. Hosted macOS Intel passed all 605 + 6 checks in run 39614595363; audio.samples took 83.66 seconds. This bounds that observed run, not future variance.
