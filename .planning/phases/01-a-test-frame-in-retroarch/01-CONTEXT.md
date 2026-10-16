# Phase 1: A test frame in RetroArch - Context

**Gathered:** 2026-10-16
**Status:** Ready for planning

<domain>
## Phase Boundary

Phase 1 ships the whole delivery path with no ROM, BIOS or emulated chip. The C library, `muhtari-run` and the
libretro core show a built-in test card. It is built and tested on Linux, macOS and Windows on x64 and arm64, plus
the 32-bit big-endian `be32` lane and, folded in here, a wasm32 lane. The phase also delivers the repository
settings and ruleset, an attested automatic release, and the one-line RetroArch install on an Apple Silicon Mac.
Requirements FRAME-01 to FRAME-09. Merging publishes 0.1.0.

Not in this phase: the CPU, the bus and RIOT (phase 2); cartridge probing and loading (phase 2); MARIA, boards,
boot and input (phase 3); sound beyond silence (phase 4).

</domain>

<decisions>
## Implementation Decisions

All prior choices in `.planning/preparation/DECISIONS.md` stand. The owner selected every open area below and
asked for a deep fan-out on each, ending in one coherent recommendation per point that is followed without asking
again. Each area was researched by an advisor agent. The results were reconciled across areas and are locked
here.

### Test card (FRAME-03, FRAME-04)
- **D-01:** The card is static and carries no text, version or font. It is one pure integer function,
  `uint8_t muhtari__testcard_pixel(uint32_t x, uint32_t y, uint32_t h)`, with the width fixed at 320 and `h` = 242
  (NTSC) or 292 (PAL, later). It uses no table and no state, so DEC.31 cannot apply and the hash never changes
  between releases. Precedent: szTheory/nesturbator @3200aa868371 `src/testcard.c`.
- **D-02:** Pixel rules, applied in order (first match wins), with `cy = (h − 192) / 2` (NTSC 25):
  1. `x < 8 && y < 8` → `$0F`: a white corner block, top-left only. It reveals a mirror or a flip.
  2. `x == 0 || x == 319 || y == 0 || y == h−1` → `$0F`: a 1-px border. It reveals cropping.
  3. `y == 8 || y == h−11` → `$08`: grey lines on the first and last rows of the 224-row overscan crop (frame rows
     8–231, MiSTer's crop, A7800-MARIA §6).
  4. `cy ≤ y < cy+192 && 48 ≤ x < 272` → `(((y−cy)/12) << 4) | ((x−48)/14)`: the colour chart.
     - Each row is one hue and each column one luma, so cell (r, c) is colour `$rc`, and every value $00–$FF
       appears once.
     - The cells are 14×12 pixels, which is square at 6:7. The whole chart (224×192) is therefore square only when
       the host applies 960/847.
     - It covers frame rows 25–216, which is exactly the Software Guide's NTSC safe area (A7800-MARIA §6).
  5. `cy ≤ y < cy+192 && x < 48` → `((x ^ y) & 1) ? $0F : $00`: a 1×1 checker. It reveals filtering and
     non-integer scaling.
  6. `cy ≤ y < cy+192 && x ≥ 272` → `(((x >> 1) ^ y) & 1) ? $0F : $00`: a 2×1 checker, MARIA's 160-px pixel pair.
  7. `y < cy` → `$46`: a red top band.
  8. Otherwise → `$C6`: a green bottom band. Red above green makes an R/B swap look different from a vertical flip.
- **D-03:** Before pinning the hash, check in the warm NTSC table that red is the largest channel of `$46` and
  green the largest of `$C6`. If either fails, take the nearest luma-6 hue that passes, then freeze it.
- **D-04:** While no content is loaded, the frame driver calls the function for every pixel of every frame. It
  writes into the caller's buffer at the caller's pitch and outputs silence. Nothing is cached in the instance:
  the card is shown only without content. A span renderer may replace this later if bench data asks for it, held
  to the same hash.
- **D-05:** Tests:
  - Library:
    - The frame 1 and frame 600 hashes are equal.
    - The test-card hash is pinned on every lane.
    - All 256 values occur.
    - A render at pitch 336 leaves the 16 padding bytes of every row untouched and equals the pitch-320 rows. This
      catches stride bugs.
  - In-process libretro host:
    - `SET_SUPPORT_NO_GAME` is set and `retro_load_game(NULL)` returns true.
    - `SET_PIXEL_FORMAT` is accepted before load: XRGB8888, or RGB565 under `USE_RGB565`. The deprecated
      0RGB1555 default must never apply.
    - Exactly one non-NULL `video_cb` per frame, with width 320, height 242 and pitch 1280 (or 640).
    - Each pixel is compared as a native-endian `uint32_t`/`uint16_t` value against the palette of the runner's
      byte, using the same 8→5/6/5 rule. This catches channel order, 555 vs 565, width, pitch, row order and byte
      order on `be32`.
  - Hosted RetroArch: the screenshot, decoded to RGB24, equals the runner's `--dump-frame N` PNG byte for byte.
    Because the card is static, N only has to be large enough for the core to load.
- **D-06:** The runner's PNG is 320×242 RGB with R, G and B written explicitly. It is unscaled and has no aspect
  metadata. One short section in `docs/` explains what each element of the card reveals and that cell (r, c) is
  colour `$rc`.

### Public header in 0.1.0 (FRAME-02, FRAME-05)
- **D-07:** The header declares only calls that work and grows by appending. A name enters `include/muhtari.h` in
  the same pull request that implements and tests it, and that pull request raises `MUHTARI_API_VERSION` by 1.
  Stubs, reserved fields and future functions, statuses or enum values are never declared. — **Reversibility:**
  costly — the 0.1.0 header is a published API. Pre-1.0 semver lets a later minor version break it (DEC.14), but
  every embedder then has to change.
- **D-08:** 0.1.0 declares exactly this, including only `<stddef.h>` and `<stdint.h>`, inside `extern "C"`
  guards. It has no export macro and no `MUHTARI_VERSION_*` macros: the version stays in the private generated
  header (ENGINEERING §5).
  ```c
  #define MUHTARI_API_VERSION 1

  typedef uint32_t muhtari_status;
  enum {
      MUHTARI_STATUS_OK = 0,
      MUHTARI_STATUS_ERR_ARGUMENT = 1,
      MUHTARI_STATUS_ERR_STRUCT_SIZE = 2,
      MUHTARI_STATUS_ERR_NO_MEMORY = 3,
      MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL = 4
  };

  typedef struct muhtari muhtari;

  typedef struct muhtari_allocator {
      void *(*alloc)(void *user, size_t size);           /* max_align_t-aligned */
      void (*free)(void *user, void *ptr, size_t size);
      void *user;
  } muhtari_allocator;

  typedef struct muhtari_config {
      uint32_t size;
      muhtari_allocator allocator;                        /* alloc and free required */
  } muhtari_config;

  typedef struct muhtari_versions {
      uint32_t size;
      uint32_t library_major, library_minor, library_patch;
      uint32_t api;                 /* 1 */
      uint32_t abi;                 /* 1 */
      uint32_t behaviour_revision;  /* 1 */
  } muhtari_versions;

  typedef struct muhtari_info {
      uint32_t size;
      uint32_t width, height;                     /* 320, 242 */
      uint32_t fps_num, fps_den;                  /* 39375000, 656711 */
      uint32_t par_num, par_den;                  /* 6, 7 */
      uint32_t sample_rate_num, sample_rate_den;  /* 48000, 1 */
      uint32_t max_audio_samples;                 /* per frame: 801 */
  } muhtari_info;

  typedef struct muhtari_frame {
      uint32_t size;
      uint32_t video_pitch;       /* in: bytes per row, 320..65536 */
      uint8_t *video;             /* in: NULL skips video */
      int16_t *audio;             /* in: NULL skips audio */
      uint32_t audio_capacity;    /* in */
      uint32_t audio_count;       /* out: samples this frame (800 or 801) */
  } muhtari_frame;

  muhtari_status muhtari_get_versions(muhtari_versions *out);
  muhtari_status muhtari_create(const muhtari_config *cfg, muhtari **out);
  void           muhtari_destroy(muhtari *m);                          /* NULL-safe */
  muhtari_status muhtari_get_info(const muhtari *m, muhtari_info *out);
  muhtari_status muhtari_run_frame(muhtari *m, muhtari_frame *io);
  ```
- **D-09:** A new instance has no content, and every `muhtari_run_frame` gives the test card and silence. 0.1.0
  has no `muhtari_load`. When `load` arrives, NULL media returns `MUHTARI_STATUS_ERR_ARGUMENT`; NULL never means
  "test card".
- **D-10:** Struct and call rules:
  - **Size tags.** A `size` below the struct's first released size returns `ERR_STRUCT_SIZE`. That first size is
    `offsetof(last first-release field) + sizeof` of that field, computed in `src/`. A size between that and the
    current size is accepted: missing input fields take their zero default, and outputs write
    `min(size, sizeof)` bytes. A larger size is accepted only if its extra tail is zero. Every field appended
    later keeps the old behaviour when zero.
  - **Initialisation.** Callers zero a struct and then set `size`. There is no init macro.
  - **Statuses.** `muhtari_status` is a `uint32_t` typedef with the enum used only as constants, because ARM
    AAPCS bare-metal targets default to short enums.
  - **Allocator.** `alloc` and `free` are both required (NULL → `ERR_ARGUMENT`), with no malloc default: the core
    links nothing beyond `memcpy memmove memset memcmp` (ARCHITECTURE §1, ENGINEERING §2 `abi.undefined`). The
    runner and the adapter pass a six-line malloc wrapper, which the docs show.
  - **Versions.** API 1, ABI 1 and behaviour revision 1. The behaviour revision is runtime-only, because it
    describes the binary rather than the header. Zero means "absent" for every version number.
  - **`run_frame` checks** happen before anything changes:
    - A `video_pitch` outside 320..65536 with non-NULL `video` returns `ERR_ARGUMENT`. The cap keeps the
      arithmetic inside a 32-bit `size_t` on wasm32 and be32.
    - An `audio_capacity` below this frame's count with non-NULL `audio` returns `ERR_BUFFER_TOO_SMALL`.
    - `audio_count` is always written.
  - **Layout.** Field order leaves no inner padding. A `_Static_assert` on `offsetof` and `sizeof` per struct
    guards the layout on every lane.
  - **No `uint64_t`** appears in any 0.1.0 signature or struct, so a JS host needs no BigInt.
- **D-11:** The library ships static only:
  - **Library form.** `libmuhtari.a`, or `muhtari.lib` (D-15), built with `POSITION_INDEPENDENT_CODE ON` and
    hidden visibility, without LTO in the shipped archive.
  - **Zip contents.** `include/muhtari.h`; `lib/cmake/muhtari/` with target `muhtari::muhtari` and
    `COMPATIBILITY SameMinorVersion` (pre-1.0, a minor version may break); a relocatable `lib/pkgconfig/muhtari.pc`
    using `${pcfiledir}`, as nesturbator does; `LICENSE`; `THIRD-PARTY-NOTICES.md`.
  - **Embedding.** `add_subdirectory` gives the same alias, with tests and install rules gated on
    `PROJECT_IS_TOP_LEVEL`.
  - **No shared library.** There is no SOVERSION while no shared library exists; the ABI number lives in
    `muhtari_versions.abi`. A shared library arrives only when a consumer must dlopen it (Python, v5). It then has
    SOVERSION = ABI and exports defined in `src/` or a generated list, so the public header still needs no macro.
  - **The libretro core** exports only `retro_*`.
- **D-12:** The header stays honest through three checks:
  - `abi.symbols` runs both ways: the defined external `muhtari_*` symbols equal the declared functions.
  - `tests/api/v1.c` calls every function, touches every field and constant, and asserts the constant values. It
    is frozen once released; each later API version adds `vN.c`.
  - `abi.api_version`, a CMake `-P` check of about 25 lines, fails when a header identifier or field appears in no
    `tests/api/vN.c` with N ≤ `MUHTARI_API_VERSION`, or when `v<MUHTARI_API_VERSION>.c` is missing. Fault
    prevented: something added to the header without a version bump or a test.

### Oldest systems the archives run on (FRAME-06, FRAME-07)
Evidence: the binaries of szTheory/nesturbator v0.1.10, which set no minimums, have macOS `minos 15.0` and import
`VCRUNTIME140.dll` on Windows. RetroArch 1.22.2 Metal itself runs from macOS 10.13 (x86_64) and 11.0 (arm64), and
buildbot cores import only `KERNEL32.dll` and `msvcrt.dll`. Defaults would therefore ship a core that fails where
RetroArch runs.
- **D-13:** Linux builds stay on `ubuntu-26.04` and `ubuntu-26.04-arm`, with no container and no zig.
  - **Core `.so`.** `muhtari_libretro.so` (x64, arm64) needs no `GLIBC_*` above **2.17**.
  - **Runner.** `muhtari-run` needs none above **2.34**. `__libc_start_main@GLIBC_2.34` sets that floor; Debian
    11, Ubuntu 20.04 and ArkOS cannot run the runner, but can load the core.
  - **What fails.** `NEEDED` must stay within `libc.so.6` and the `ld-linux` loader (no `libm`, no `libgcc_s`).
    Any version need that is not a plain `GLIBC_<n>.<n>[.<n>]`, such as `GLIBC_ABI_DT_RELR`, fails, as does any
    `__isoc23_*` reference.
  - **Source mode.** The runner keeps `-std=c17` without `_GNU_SOURCE`.
  - **Library.** The `.a` states no floor. Its undefined symbols stay within `abi.undefined`, so it should also
    link against musl (untested). `__*_chk` fortify symbols are never allowed: build the core with
    `-U_FORTIFY_SOURCE`.
  - **musl.** No musl zip; the README says "musl: build from source".
- **D-14:** macOS: the `ci` configure preset sets `CMAKE_OSX_DEPLOYMENT_TARGET` to `10.13`. It is set in the preset
  and not in `CMakeLists.txt`, so embedders choose their own. The minimum OS recorded in each binary's load
  commands must be ≤ 10.13 for x86_64 and ≤ 11.0 for arm64, and `otool -L` lists only
  `/usr/lib/libSystem.B.dylib`.
- **D-15:** Windows: the `ci-msvc` preset sets `CMAKE_MSVC_RUNTIME_LIBRARY` to
  `MultiThreaded$<$<CONFIG:Debug>:Debug>`, so every target uses the static CRT.
  - **Static library.** It adds `/Zl`, so it carries no `/DEFAULTLIB`; `dumpbin /directives` must show none.
  - **Imports.** The libretro DLL and the runner EXE import exactly `KERNEL32.dll` (`dumpbin /dependents` or
    `llvm-readobj --coff-imports`).
  - **Consumers.** `install.consumer` runs twice on Windows, with the consumer on `MultiThreaded` and on
    `MultiThreadedDLL`. Both runs are in the test inventory.
  - **API level.** `_WIN32_WINNT` stays undefined, since there are no Win32 API calls.
  - **Claim.** Windows 10 or later, x64 and arm64.
  - **Reconciled.** This replaces the header research's "default `/MD`" for the `.lib`.
- **D-16:** One per-leg CTest, `release.floor`, proves D-13 to D-15. It is a CMake `-P` script of about 60–80
  lines, run on the extracted archives next to `release.archive_contents`, with the floors in one table.
  - **Fault named.** A released binary refuses to load on an OS or runtime the README lists as supported.
  - **Self-test.** `release.floor.self_test` runs it through `expect_fail.cmake` with an impossible floor (glibc
    2.0, macOS 9.0, or an empty import allowlist) and passes only on that exit code and message.
  - **What it does not do.** No container runs and no floor-OS machines. The check reads exactly what the OS
    loader enforces.
- **D-17:** The README gets a four-line "Requirements" section:
  - Linux x64/arm64: glibc 2.17+ for the core, 2.34+ for `muhtari-run`; musl: build from source.
  - macOS 10.13+ (Intel) or 11+ (Apple Silicon).
  - Windows 10+ x64/arm64, with no runtime install.

  The commands, and the note "checked from the binaries, not run on these systems", go in `docs/`.

### wasm32 lane (folded todo; every PR)
- **D-18:** The toolchain is the image's clang-21 and lld-21, plus
  `apt-get install -y --no-install-recommends wasi-libc libclang-rt-21-dev-wasm32` (resolute
  `0.0~git20250726.3f7eb4c-4`, `1:21.1.8-6ubuntu1`).
  - **Toolchain file.** `cmake/toolchains/wasm32-wasi.cmake` sets:
    - `CMAKE_SYSTEM_NAME WASI`
    - `CMAKE_C_COMPILER clang-21`
    - `CMAKE_C_COMPILER_TARGET wasm32-wasi`
    - `CMAKE_C_FLAGS_INIT "-mcpu=lime1 -D_WASI_EMULATED_SIGNAL"`, using the frozen lime1 feature set (`generic`
      drifts between releases)
    - `CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--stack-first"`, so a stack overflow traps instead of corrupting static
      data and changing the hash
    - `-lwasi-emulated-signal`
  - **Triple.** It is `wasm32-wasi` because the distro's compiler-rt layout breaks `wasm32-wasip1` linking
    (tested in an `ubuntu:26.04` container).
  - **Not used.** No SIMD, threads or wasip2.
  - **Fallback**, if clang warns on or rejects the triple: wasi-sdk-34, tarball SHA-256
    `b761e3a0721dbae9c09a0059e5fdb2bf917d1b4a8a7b430fb3b5aafb0984b2c4`, its `wasi-sdk-p1.cmake` and
    `wasm32-wasip1`.
- **D-19:** The runtime is wasmtime **v48.0.6**, an LTS release supported for 24 months.
  - `wasmtime-v48.0.6-x86_64-linux.tar.xz`, SHA-256
    `a04bbb203271301e923ccc54ebe834145b1595af1ffb457c8e5dbf1baadd7248`.
  - For local runs: aarch64-macos `0b24079ba8c8a23df9cd9a11e40f0b5edc95398a70a7cdec9286133f18701c69`, aarch64-linux
    `206a572cd4fab81e86bb4e808976e822603fd2aa9c67730c921710c2c6bf1c00`.
  - Node's WASI is rejected: it is Experimental, its own docs say it can be escaped, and it would be a second
    runtime.
  - Phase 3's 7800basic tools reuse this pin after re-running their byte-identity check. CONFORMANCE §2(a)
    (v49.0.1) is updated in the pull request that lands the pin.
- **D-20:** A `wasm32` preset:
  - Release, warnings fatal, `MUHTARI_BUILD_LIBRETRO=OFF` (WASI has no dlopen).
  - `CMAKE_CROSSCOMPILING_EMULATOR` =
    `<wasmtime>;run;--dir=${sourceDir}::${sourceDir};--dir=${binaryDir}::${binaryDir}`. Each directory maps to the
    same path inside the guest, so CMake's absolute paths work unchanged.
  - The test step runs the label `portable`, exactly as `be32` does, with the timeout scale measured for this
    lane.
  - Scripts must not assume a `.wasm` suffix.
- **D-21:** The runner needs no new `#ifdef`s for WASI:
  - **Stopping.** ISO C `signal(SIGINT/SIGTERM, …)` sets a `volatile sig_atomic_t` flag. This is valid on MSVC,
    a no-op under WASI's emulated signals, and covers HOSTS §4's "finish the frame, flush, exit".
  - **Bench clock.** `clock_gettime(CLOCK_MONOTONIC)` with `_POSIX_C_SOURCE=200809L`; only `_WIN32` gets its own
    branch.
  - **Files.** A file outside a preopened directory fails with ENOENT, which reaches exit status 1 through the
    normal I/O path.
- **D-22:** Job and gating:
  - **Job.** `wasm32` runs on `ubuntu-26.04` x64 and is skipped when the change is light. Steps: checkout, apt
    install, fetch the wasmtime pin and verify it, `cmake --workflow --preset wasm32`, upload `hashes-wasm32`. The
    timeout is provisional at 10 minutes. Estimated cost: about 1 runner-minute per PR (untested on GitHub).
  - **Report-only start.** As the owner chose, the lane reports only for its first 10 runs on main
    (CONFORMANCE §6). It `cmp`s its hashes against `hashes-linux-x64` outside `CI required`.
  - **Promotion.** One later `ci:` pull request adds it to the `needs` of `hash-equality` and `CI required`. From
    then on `hash-equality` compares eight files and asserts that the count is exactly eight, so a lane that stops
    uploading cannot silently drop out.
  - **Tracking.** Phase 1 adds a pending todo (area ci) for the promotion.
- **D-23:** The nightly bench adds a `wasmtime run muhtari-run --bench` row, tagged `wasm32-wasmtime-48.0.6`. It
  records frames per second and percentiles but not RSS, which would measure wasmtime. It is trend data for the
  browser priority and never a gate.
- **D-24:** One committed pins file lists every pinned download as `name version url sha256`: wasmtime, the
  RetroArch DMG, and later the vector archive and the 7800basic package. A POSIX sh fetch script downloads by
  name and verifies the SHA-256.
  - **Bump policy.** Take an LTS patch on a security advisory, and move to the next LTS before end of life. Each
    bump is one `ci:` pull request taking the digest from the release API.
  - **apt packages** stay unpinned: they are signed, and drift fails loudly as a build failure or a hash change.
  - **Reports.** The weekly digest that reports newer upstream pins arrives with the v2.0 tune-up (SEED-007).

### Reconciled across areas
- **D-25:** CI gating:
  - The lanes FRAME-01 to FRAME-09 name are part of `CI required` from their first run: the six builds, `hygiene`,
    `title`, `asan`, `nofp`, `be32` (per pull request while it stays within 5 minutes, ENGINEERING §9),
    `hash-equality`, and the hosted RetroArch job.
  - `perf` follows FRAME-08's own arming rule.
  - Only lanes no requirement names start report-only (CONFORMANCE §6); in phase 1 that is the wasm32 lane.
  - Precedent: szTheory/nesturbator @3200aa868371 `ci.yml`, whose `CI required` needs `retroarch-e2e`.
  - This corrects the discussion's summary, which had the RetroArch job report-only at first.
- **D-26:** The pitch rule in D-10 and the pitch-336 test in D-05 are one contract. The silence in D-04 and the
  audio fields in D-08 are another: every frame writes 800 or 801 zero samples at 48 kHz.

### Owner directive recorded in this discussion
- **D-27:** "Another copy and paste beats another dependency" (now in `AGENTS.md` Style). For phase 1 this means:
  - no zlib, lodepng, stb or test framework;
  - the runner's PNG writer uses stored deflate blocks with its own CRC-32 and Adler-32, copying
    szTheory/nesturbator's CRC-32 and SHA-256 code;
  - RetroArch's screenshot is decoded by copied code or a platform tool, never a new package;
  - third-party code enters only as a PROVENANCE seed with its notice.

### Claude's Discretion
- **Content in 0.1.0.** The phase 1 `.info` sets `supports_no_game = "true"` and lists no
  `supported_extensions`; the extensions arrive with the phase in which games play. Until then,
  `retro_load_game` with content returns false.
- **Palette.** The warm palette (a7800's BSD-3 Trebor tables, DEC.16, DEC.27) enters as a PROVENANCE seed with
  its notice. It lives in host-helper code linked by the runner and the libretro adapter, not in the public
  header.
- **Seeds.** Seed the runner, the adapter, the tests and CI from szTheory/nesturbator @3200aa868371 (same owner,
  MIT, no notice needed). Where its choices differ from the decisions above, the decisions win: allocator,
  floors, header growth, and PNG instead of PPM.
- **Stack protector.** Either allow `__stack_chk_fail` as a per-target helper in `abi.undefined`, or build the
  core with `-fno-stack-protector`. Decide from what the images emit.

### Folded Todos
- **Consider a wasm32 build and hash lane in phase 1 CI** (`.planning/todos/pending/2026-10-15-consider-a-wasm32-build-and-hash-lane-in-phase-1-ci.md`,
  area ci). Problem: the browser build is SEED-008, so nothing would show that a wasm32 build still compiles or
  gives the native frame hash. The owner folded it in with "every PR", implemented by D-18 to D-24. Move the todo
  to `done/` when the lane lands.

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**

### Phase scope
- `.planning/ROADMAP.md` (Phase 1): goal and the five success criteria.
- `.planning/REQUIREMENTS.md`: FRAME-01 to FRAME-09, and the preamble's definitions of "platform" and
  "every platform".
- `.planning/PROJECT.md`: constraints (deliverables, C17, determinism, efficiency, privacy, delivery).
- `.planning/preparation/DECISIONS.md`: settled choices. Read DEC.14, 16, 19, 21, 26, 27, 31, 43, 45–53, 55–57
  before planning.

### Design and contracts
- `.planning/preparation/ARCHITECTURE.md` §1 (shape and link rule), §4 (public API, size tags, version numbers),
  §5 (video and audio contracts), §11 (the phase 1 tracer: test card → runner → libretro → RetroArch).
- `.planning/preparation/A7800-MARIA.md` §6 (rows, PAR and DAR, safe area, overscan crops) and §7 (palettes).
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` §1 (libretro contract, `.info`), §3 (RetroArch on Apple
  Silicon, the hosted job, the install line), §4 (runner options and exit statuses), §6 (WASM).

### Build, CI and release
- `.planning/preparation/ENGINEERING.md`:
  - §1: runners, action SHAs, release-please facts.
  - §2: C rules, flags, presets, `abi.*` checks.
  - §3: exact inventory, no self-skip, negative controls.
  - §4: CI design and the `CI required` roll-up.
  - §5: release, archives, rehearsal, re-runs.
  - §6: settings as code and `--check`.
  - §7: Dependabot, the nightly issue, reporters.
  - §8: the Ir/frame gate, zero allocations, bench.
  - §9: `be32`.
  - §10–11: style, docs budgets.
  - §13: secrets.
- `.planning/preparation/CONFORMANCE.md` §6: CI tiers and "measure before gating". §2(a): the wasmtime pin to
  update.
- `.planning/preparation/GSD-HANDOFF.md` §3 (per-phase routine, ship and merge) and §7 (the release App and its
  secrets).
- `.planning/LESSONS.md` XFER-026 to XFER-033: CI and release lessons from lattice_stripe and exifcleaner.

### Rules and provenance
- `AGENTS.md`: rules 1–11 and Style, including the dependency preference.
- `PROVENANCE.md`: seeds, notices, the `THIRD-PARTY-NOTICES.md` format, vendored files.
- `ASSET_POLICY.md`: which bytes may be committed or packaged.
- `.planning/seeds/SEED-008-browser.md`: the browser build the wasm32 lane prepares for.
- `.planning/todos/pending/2026-10-15-consider-a-wasm32-build-and-hash-lane-in-phase-1-ci.md`: the folded todo.

### External (owner's MIT sibling, cite by name and commit)
- szTheory/nesturbator @3200aa868371: `src/testcard.c`, `src/instance.c`, `src/crc32.c`, `runner/` (`sha256.c`,
  `bench.c`, `clock.c`, `stop.c`, `exit_status.h`), `libretro/` (the 352-line adapter shape), `tests/check.h`,
  `tests/cmake/` (`action_pins`, `check_archives`, `check_install_line`, `ci_required_rollup`, `global_symbols`,
  `float_scan`, `perf_compare`, `perf_gate_selftest`, `release_*`, `runner_*`, `skip_policy`, `timeout_comments`,
  `pins_check`, `info_claims`), `cmake/packaging.cmake`, the `.pc.in` and `Config.cmake.in` templates,
  `.github/workflows/{ci,nightly,release}.yml`, `.github/rulesets/main.json`, `.github/dependabot.yml`.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `scripts/hygiene.sh` runs as the pre-commit and pre-push hook (`--staged`, `--history`, `--tree`). The
  `hygiene` preset wraps `--tree` in CI.
- `scripts/apply-repo-settings.sh` applies the repository settings. FRAME-08 extends `--check` to every setting it
  applies, and the ruleset is applied once `CI required` has reported (ENGINEERING §6).
- `.githooks/pre-commit` and `.githooks/pre-push` are active through `core.hooksPath`.
- `.env.example` names the local-only variables; agents never read `.env.local`.
- The rest comes from szTheory/nesturbator @3200aa868371 as seeds (see canonical refs). No muhtari C code exists
  yet.

### Established Patterns
- One entrypoint, `cmake --workflow --preset <lane>`, used both locally and in CI. `workflow.test_command` is
  `cmake --workflow --preset ci`, and GSD's test gate fails until this phase creates it.
- Repository-check code is CMake `-P` or POSIX sh, and each check names the fault it prevents (AGENTS rule 7).
- Evidence is a command, an exit status and a run URL (AGENTS rule 10).

### Integration Points
- The release App `muhtari-release` (variable `MUHTARI_APP_CLIENT_ID`; secret `MUHTARI_APP_PRIVATE_KEY` for both
  Actions and Dependabot) is already set up for `release.yml` and the Dependabot auto-merge job.
- RetroArch 1.22.2 Metal is installed on the owner's Mac for the optional two-minute smoke test. CI uses the
  pinned DMG.

</code_context>

<specifics>
## Specific Ideas

- **Owner's brief for open decisions:** fan out through every relevant lens with primary sources and an
  adversarial pass, then follow one coherent recommendation per point (GSD-HANDOFF §3).
- **Independent cross-check for D-02:** a Python model of the rules, using `$46` and `$C6`, gives NTSC SHA-256
  `9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4` and PAL
  `63de7dffa9b46da6a78e8fc0522941e9f2aa2804a0e117989e82270036f98a1e`. The C card must reproduce the NTSC value
  unless D-03 changes a band value.
- **Facts to verify while planning** (from the advisors; (untested) or (unverified)):
  - RetroArch with `video_gpu_screenshot=false` saves the core's frame unscaled and unflipped.
  - The default `-march` on ubuntu-26.04 is x86-64 v1, and binutils does not default to
    `-z pack-relative-relocs`.
  - Whether Ubuntu's default fortify and stack protector add symbols to the core.
  - Whether Apple clang 17 clamps the arm64 minimum OS to 11.0.
  - `apt` cost on the runner, and whether `libclang-rt-21-dev-wasm32` pulls a different clang-21.
  - wasm-ld's default stack size versus the runner's buffers.
  - Whether `typedef uint32_t` status comparisons are warning-free under `-Werror` and `/W4 /WX`.
  - The warm-table RGB of `$46` and `$C6`.
- The rate is 39375000/656711 ≈ 59.958 Hz (FRAME-04). A 59.92 Hz figure in the discussion brief was a slip.

</specifics>

<deferred>
## Deferred Ideas

- **Public palette helper** for library-only embedders (browser, Python), who otherwise cannot turn native bytes
  into RGB: decide with SEED-008, or in phase 3 if needed.
- **Shared library and SOVERSION**: when a consumer must dlopen it (Python, v5).
- **A wasm disassembly check** for f32/f64 opcodes in the core: a seed, not a requirement (AGENTS rule 7). `nofp`
  already gates integer-only.
- **A container smoke test for the Linux floors**: only if `release.floor` ever passes while a supported system
  fails to load a release.
- **The Linux runner on glibc below 2.34** (Debian 11, Ubuntu 20.04, ArkOS): build it in a pinned
  `debian:bullseye` container if a user asks.
- **musl or Alpine archives**: build from source until someone asks.
- **A browser or Node hash step**: SEED-008, if browser engines are ever found to differ.
- **The digest's section for new upstream pins**: the v2.0 tune-up (SEED-007).

</deferred>

---

*Phase: 01-a-test-frame-in-retroarch*
*Context gathered: 2026-10-16*
