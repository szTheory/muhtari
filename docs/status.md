# Project status

Muhtari is an Atari 7800 emulator core, not a desktop emulator app. It provides a C library, a headless command-line
runner and a libretro core. RetroArch or another host provides the window and audio and input devices.

## Available now

The latest published package is v0.14.0. It includes the selected XM/YM2151 digital sound profile.

Souper mapper support is implemented, tested and merged into the main source, but is not in a published package yet.
The earlier release draft remains tied to source that failed a Wasm32 test; the corrected source passed main CI. The
current milestone is to publish the corrected source through a fresh feature release. See the
[Phase 25 release receipt](../.planning/phases/25-publish-the-corrected-souper-mapper/25-RELEASE.md) for the exact
source and its release evidence.

## Compatibility

Compatibility claims are tied to specific evidence. In the current pinned list of 204 titles, none has a verified
pass: 201 have not been tested and 3 are skipped. Passing diagnostic tests show particular hardware-model behavior;
they do not establish that a retail game works from start to finish. See the [compatibility ledger](compatibility.md)
for the evidence and its limits.

## Still missing

- GameDrive/Covox streamed ADPCM and BupChip/BupBoop audio have not been implemented. GameDrive's public profile
  and an independent lawful audio oracle are not complete enough to implement safely.
- Broad title compatibility is not established. Many titles in the pinned list are still untested.
- Nine older Playstead and frontend integration requirements remain unresolved, including actual host launch,
  input, save/load, rewind and run-ahead evidence. These are not proved by the library's in-process tests.
- Full OpenHSC support and AtariVox speech are not available. Muhtari intentionally has no built-in window,
  audio device or input device.

## What counts as done

There is no honest universal “all Atari 7800 games work” finish line today. A scoped emulator feature is done when
its behavior has a source-qualified profile, deterministic and bounded checks cover the supported library, runner
and libretro paths, unsupported cases are refused clearly, and the exact release artifact passes the existing
CI and installation checks.

Project-wide game compatibility needs a finite title list and evidence for every promised title. The current
204-entry list does not meet that bar. Until that target is selected and verified, describe Muhtari as an active
emulator project rather than a finished, universally compatible emulator.

For the current release milestone and its exact completion checks, see the [project roadmap](../.planning/ROADMAP.md).
