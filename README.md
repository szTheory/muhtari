# muhtari

Play Atari 7800 games in RetroArch with muhtari, an MIT-licensed emulator core.
RetroArch provides the window, sound and controls; you supply your game files.

**Early release:** NTSC and PAL, sound, supported controllers and cartridge battery saves are available.
Save states, rewind and run-ahead are next. Compatibility varies; check the [evidence](docs/compatibility.md).
Evidence covers the rows listed; there is no claim of universal title compatibility.

## Install in RetroArch (macOS, Apple Silicon)

Install the [RetroArch Metal build](https://www.retroarch.com/?page=platforms), then paste this into Terminal:

```sh
mkdir -p ~/Library/Application\ Support/RetroArch && curl -fsSL https://github.com/szTheory/muhtari/releases/latest/download/muhtari-libretro-macos-arm64.zip | tar -xf - -C ~/Library/Application\ Support/RetroArch cores info
```

**Windows, Linux or Intel Mac:** download `muhtari-libretro-<os>-<arch>.zip` from the
[latest release](https://github.com/szTheory/muhtari/releases/latest). Choose `linux`, `macos` or `windows`
and `x64` or `arm64` to match RetroArch. Copy the archive's `cores` and `info` contents into the folders
shown under **Settings → Directory → Cores / Core Info**, then restart RetroArch.

## Start playing

1. **Load Core → Atari - 7800 (muhtari)**.
2. **Load Content** → your Atari 7800 `.a78` or `.bin` file. Extract ZIP downloads first.
   Choose muhtari if asked which core to use. **No BIOS is required** with the default Fast boot setting.
3. Use the D-pad and fire buttons. Some games start with the console **Reset** switch.

Default keyboard controls: **arrows** move, **Z/X** fire, **S** is Reset, **Right Shift** is Select,
**Enter** is Pause, and **F1** opens RetroArch's Quick Menu.
[Gamepad bindings, BIOS setup and troubleshooting](docs/libretro.md) cover the other options.

No game yet? After loading muhtari, choose **Start Core** to see its built-in test card.

### Start a game from Terminal (macOS)

Replace the last path with your game file; omit it to show the test card:

```sh
/Applications/RetroArch.app/Contents/MacOS/RetroArch -L "$HOME/Library/Application Support/RetroArch/cores/muhtari_libretro.dylib" "/path/to/game.a78"
```

## Requirements

Linux x64/arm64: glibc 2.17+ for the core, 2.34+ for the runner; musl users build from source.
macOS: 10.13+ on Intel, 11+ on Apple Silicon. Windows: 10+ x64/arm64, no runtime install.

## For hosts and developers

muhtari also ships a C library and headless `muhtari-run` binaries in each release.
The runner has no interactive window or controls.

- [Runner](docs/runner.md): probe a game, run frames and compare hashes.
- [Embedding](docs/embedding.md): use the library from CMake or pkg-config.
- [Testing](docs/testing.md): build and test the core.

## License and game files

[MIT](LICENSE). No games, BIOS or high-score-cartridge firmware are bundled.
See [PROVENANCE.md](PROVENANCE.md) and [ASSET_POLICY.md](ASSET_POLICY.md) for source and asset rules.
