# muhtari

An Atari 7800 ProSystem emulator core in C: a small library, a headless runner and a libretro core for RetroArch.

**Status: early.** The core includes the CPU, graphics and TIA/POKEY sound. Cartridge-board and peripheral
coverage remains limited; see the [compatibility ledger](docs/compatibility.md) for tested content. Releases are
published automatically.
NTSC board execution includes linear, SuperGame, Activision, Absolute and Fractalus cartridges.
Board diagnostics establish memory behavior; the ledger records tested gameplay separately. PAL execution remains unsupported.
Controller models include XG-1, paddles, keypad, driving, CX22/ST/Amiga motion and SNES2Atari/Mega7800.
Owned diagnostics establish model conformance on both ports; physical calibration and universal game compatibility
remain unmeasured. See [controls and bindings](docs/libretro.md#devices) and [typed runner replay](docs/runner.md#movies).

## Install in RetroArch (macOS, Apple Silicon)

Paste this line into Terminal. It puts the core and its information file into RetroArch's folders:

```sh
mkdir -p ~/Library/Application\ Support/RetroArch && curl -fsSL https://github.com/szTheory/muhtari/releases/latest/download/muhtari-libretro-macos-arm64.zip | tar -xf - -C ~/Library/Application\ Support/RetroArch cores info
```

On other systems, download `muhtari-libretro-<os>-<arch>.zip` from the
[latest release](https://github.com/szTheory/muhtari/releases/latest) (`<os>` is `linux`, `macos` or `windows`;
`<arch>` is `x64` or `arm64`) and copy its `cores` and `info` folders into the folders RetroArch lists under
Settings → Directory → Cores and Core Info.

## Requirements

- Linux x64/arm64: glibc 2.17+ for the core, 2.34+ for `muhtari-run`; musl: build from source.
- macOS 10.13+ (Intel) or 11+ (Apple Silicon).
- Windows 10+ x64/arm64, with no runtime install.

## Quick start: RetroArch

1. Main Menu → Load Core → **Atari - 7800 (muhtari)**.
2. Main Menu → **Start Core**.
3. The test card appears: a white border, a 16×16 colour chart, a red band at the top and a green band at the
   bottom. [docs/runner.md](docs/runner.md#test-card) explains what each part checks.

## Quick start: runner

Download `muhtari-runner-<os>-<arch>.zip` from the same release, unzip it, and run one frame:

```sh
bin/muhtari-run --frames 1 --hash-frame 1
```

It prints the SHA-256 of the frame's picture, the same on every platform:

```text
frame 1 sha256 9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4
```

## Documentation

- [docs/libretro.md](docs/libretro.md): the RetroArch core's controls, devices, options, BIOS files and achievements.
- [docs/runner.md](docs/runner.md): `muhtari-run` options, output, exit statuses and the test card.
- [docs/embedding.md](docs/embedding.md): using the C library from CMake or pkg-config.
- [docs/compatibility.md](docs/compatibility.md): which games and test ROMs run, generated from the compatibility ledger.
- [docs/testing.md](docs/testing.md): building, testing and the checks CI runs.

## Legal

muhtari ships no game, BIOS or high-score-cartridge images. See [PROVENANCE.md](PROVENANCE.md) for how code from
other projects is reused and [ASSET_POLICY.md](ASSET_POLICY.md) for which files may enter the repository.

## License

[MIT](LICENSE). Every release archive also carries [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md).
