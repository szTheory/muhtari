# muhtari

An Atari 7800 ProSystem emulator core in C: a small library, a headless runner and a libretro core for RetroArch.

Muhtari has a C library, a headless command-line runner and a libretro core for RetroArch. The host supplies
the window and audio and input devices.

The latest published package includes the corrected Souper mapper and selected YM2151 digital sound profile.
Download it from the [latest release](https://github.com/szTheory/muhtari/releases/latest). Souper checks use generated fixtures, not retail games. Evidence covers the rows listed; there is no claim of universal title compatibility.
See the [project status](docs/status.md), [compatibility evidence](docs/compatibility.md), and [RetroArch setup](docs/libretro.md).

## Install in RetroArch (macOS, Apple Silicon)

On Apple Silicon, this command assumes RetroArch is installed in its usual location and the running RetroArch
process is native arm64. It puts the core and its information file into RetroArch's configured folders:

```sh
mkdir -p ~/Library/Application\ Support/RetroArch && curl -fsSL https://github.com/szTheory/muhtari/releases/latest/download/muhtari-libretro-macos-arm64.zip | tar -xf - -C ~/Library/Application\ Support/RetroArch cores info
```

For other installations, choose the native archive for the operating system and architecture of the running
RetroArch process. The latest release provides `muhtari-libretro-linux-arm64.zip`,
`muhtari-libretro-linux-x64.zip`, `muhtari-libretro-macos-arm64.zip`, `muhtari-libretro-macos-x64.zip`,
`muhtari-libretro-windows-arm64.zip` and `muhtari-libretro-windows-x64.zip`. Copy the core module and `.info`
file into the configured Cores and Core Info directories shown under Settings → Directory. See RetroArch's
[core installation guide](https://docs.libretro.com/guides/download-cores/) and
[directory configuration guide](https://docs.libretro.com/guides/change-directories/) for its current menu terms.

The architecture-independent Emscripten libretro ZIP has a
[version-free download URL](https://github.com/szTheory/muhtari/releases/latest/download/muhtari-libretro-emscripten.zip)
and [portable host instructions](docs/libretro.md#portable-emscripten-bundle).

## Requirements

- Linux x64/arm64: glibc 2.17+ for the core, 2.34+ for `muhtari-run`; musl: build from source.
- macOS 10.13+ (Intel) or 11+ (Apple Silicon).
- Windows 10+ x64/arm64, with no runtime install.

## Quick start: RetroArch

1. Main Menu → Load Core → **Atari - 7800 (muhtari)**.
2. Main Menu → **Start Core**.
3. The test card appears: a white border, a 16×16 colour chart, a red band at the top and a green band at the
   bottom. [docs/runner.md](docs/runner.md#test-card) explains what each part checks.

To try a lawfully obtained Atari 7800 image, keep Muhtari selected and choose Main Menu → Load Content, then
select an `.a78` or headerless `.bin` file. A supported cartridge shows its own startup screen. Fast boot is the
default and starts without a console BIOS; users choosing BIOS boot can follow the
[BIOS file guide](docs/libretro.md#bios-files). The RetroPad uses D-pad, B for button 1, A for button 2, Start
for Pause and X for Reset; keyboard bindings are set by the frontend. See the
[complete controls reference](docs/libretro.md#controls).

Compatibility evidence is scoped: the pinned list has 204 titles, with 201 unevaluated and 3 skipped; no title
has a verified pass in that baseline. Unevaluated means not tested, not a failure. See the generated
[compatibility ledger](docs/compatibility.md) for per-title evidence and limits.

## Quick start: runner

Download `muhtari-runner-<os>-<arch>.zip` from the same release, unzip it, and run one frame:

```sh
bin/muhtari-run --frames 1 --hash-frame 1
```

On Windows, the executable is `bin/muhtari-run.exe`. To build the runner from source, see the
[local build instructions](docs/testing.md#the-entry-point).

It prints the SHA-256 of the frame's picture, the same on every platform:

```text
frame 1 sha256 9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4
```

For a lawfully obtained Atari 7800 image, you can identify it and then try a finite run:

```sh
bin/muhtari-run --probe "game.a78"
bin/muhtari-run --frames 120 --hash-frame 120 --rom "game.a78"
```

`--probe` prints image metadata and hashes; identification does not establish that the board, devices or sound
configuration can run. A successful run prints a content-dependent `frame 120 sha256` value. Unsupported images
or configurations exit 77, malformed or unreadable input or another failure exits 1, and invalid command usage
exits 2. See the [full runner reference](docs/runner.md) for output and refusal details.

## Documentation

- [docs/status.md](docs/status.md): what is available, what is missing and how completion is judged.
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
