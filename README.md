# muhtari

An Atari 7800 ProSystem emulator core in C: accurate, fast, deterministic and small enough to embed anywhere.

**Status: planning complete; no release yet.** The first release will run in RetroArch and show a built-in test
picture; each later phase adds a capability and publishes a new release automatically.

## What it will be

- **A C library** with a small, stable API: create an instance, load a cartridge, run a frame, read the picture
  and the sound. No global state, no threads, no files, no floating point.
- **A headless runner**, `muhtari-run`, for automation: hashes of frames and audio, deterministic input movies,
  benchmarks, and a process contract other frontends can drive.
- **A libretro core**, so RetroArch (and other libretro frontends) can play 7800 games with it.

Goals: games behave as they do on the console, identically on every platform; low latency without stutter; easy
to embed, script and port, down to low-power devices; tools for homebrew developers, ROM hackers, speedrunners,
TAS authors and AI agents. Atari 2600 cartridges are a later milestone.

## Legal

muhtari ships no game, BIOS or high-score-cartridge images. It boots cartridges without a BIOS by default; you
can supply your own dumps. See [PROVENANCE.md](PROVENANCE.md) for how code from other projects is reused and
[ASSET_POLICY.md](ASSET_POLICY.md) for which files may enter the repository.

## License

[MIT](LICENSE).
