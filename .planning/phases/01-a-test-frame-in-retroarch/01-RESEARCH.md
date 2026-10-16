# Phase 1: A test frame in RetroArch - Research

**Researched:** 2026-10-16
**Domain:** C17 library + headless runner + libretro core; CMake presets and packaging; GitHub Actions CI on six platforms plus `be32` and wasm32; release-please with an App token, attestations and immutable releases; Cachegrind gate and nightly bench
**Confidence:** HIGH for the code, presets and toolchain facts (most checked locally or in an `ubuntu:26.04` container this session); MEDIUM for GitHub-side behaviour that only a hosted run can prove (rehearsal permissions, Windows import lists, hosted RetroArch with no content)

<user_constraints>
## User Constraints (from CONTEXT.md)

### Locked Decisions

All prior choices in `.planning/preparation/DECISIONS.md` stand. The owner selected every open area below and
asked for a deep fan-out on each, ending in one coherent recommendation per point that is followed without asking
again. Each area was researched by an advisor agent. The results were reconciled across areas and are locked
here.

#### Test card (FRAME-03, FRAME-04)
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

#### Public header in 0.1.0 (FRAME-02, FRAME-05)
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

#### Oldest systems the archives run on (FRAME-06, FRAME-07)
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

#### wasm32 lane (folded todo; every PR)
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

#### Reconciled across areas
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

#### Owner directive recorded in this discussion
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

### Deferred Ideas (OUT OF SCOPE)
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
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description (REQUIREMENTS.md) | Research support |
|----|-------------|------------------|
| FRAME-01 | `cmake --workflow --preset ci` (`ci-msvc`) builds library, runner, core and runs every test on six platforms; CI runs the same commands; `CI required` passes | §Presets, §Test inventory, §CI jobs; six runner labels verified live (F-07) |
| FRAME-02 | `install.consumer` / `embed.subdirectory`; header compiles as C++ | §File inventory (consumer, embed), F-02 (status typedef warning-free in C and C++), wasm32 compile-only C++ (F-05) |
| FRAME-03 | 320×242 test card, all 256 colours; `--dump-frame` image + SHA-256 identical on every platform | NTSC hash reproduced (F-01); PNG layout and size derived (§Pattern 3); wasm stack limit (F-04) |
| FRAME-04 | In-process host: 39375000/656711, 320×242, aspect 960/847, frame equal to runner's; hosted RetroArch screenshot equals runner's frame | §libretro adapter pattern; precedent run 38676328559 (pixel-exact compare on macos-15); DMG pin verified (F-08) |
| FRAME-05 | `asan`, `nofp`, `hygiene` pass; no FP, no mutable global, no undefined symbol beyond memory functions | F-03 (fortify/SSP defaults emit `__stack_chk_fail`), stack-protector recommendation, `abi.*` design |
| FRAME-06 | 18 archives with LICENSE + notices; release PR merged by App; SHA256SUMS; verified attestations; rehearsal | release-please outputs verified (`sha`, `id`); immutable-release rules (docs); rehearsal risk R-02 |
| FRAME-07 | One-line install against latest version-free archive; CI runs that line on macOS arm64 | §README install line; post-publish macOS check |
| FRAME-08 | settings script + ruleset; title check; `policy.dependabot`; Ir gate vs merge base, armed by two control runs | live settings `--check` = no drift (F-09); `workflow_dispatch` default-branch rule (R-01) |
| FRAME-09 | `ctest -L perf` zero allocations; nightly bench ≥ 300,000 frames, p50–p99.999, max, max RSS | §bench design; RSS via `/usr/bin/time` (`time` 1.9 in resolute) |
</phase_requirements>

## Summary

Phase 1 is a walking skeleton with an unusually wide perimeter: a ~400-line core (header, instance, frame driver, test card), a small runner (hash, PNG, bench), a ~300-line libretro adapter, and then a large amount of delivery machinery (six-leg CI, `be32`, wasm32, hosted RetroArch, attested release with rehearsal, settings, Dependabot, Cachegrind gate, nightly bench). The owner's sibling project szTheory/nesturbator @3200aa868371 seeds almost every file, and its own phase-1 squash commit (@5533aeb7e6e0, 12 plans) is a better size reference than the pinned tip, whose test machinery grew to about 9,000 lines of `tests/cmake/`.

The locked decisions hold up against the toolchains. This session verified, by running them: the NTSC card hash `9eb011e2…` (independent model, 256 distinct values); the warm-table colours of `$46` (R largest) and `$C6` (G largest), so D-03 keeps `$46`/`$C6`; `typedef uint32_t` statuses warning-free under Apple clang 21, LLVM clang 21 and GCC 15 in C17 and C++17, while `arm-none-eabi-gcc` really does make plain enums 1 byte; Apple's linker clamps arm64 `minos` to 11.0 and records x86_64 10.13 in the older `LC_VERSION_MIN_MACOSX` load command; Ubuntu 26.04's GCC 15 defaults to `_FORTIFY_SOURCE=3`, `-fstack-protector-strong`, `-march=x86-64` (v1), PIE and no DT_RELR; glibc is 2.43 and a `-std=c17` runner needs exactly `GLIBC_2.34`; wasm-ld 21's default stack is 64 KiB; the D-18 toolchain file links `wasm32-wasi` cleanly (and `wasm32-wasip1` fails as stated); all six runner labels exist; the wasmtime and RetroArch pins match their release digests.

Three findings change how the plan must be built. First, `workflow_dispatch` only fires for workflow files already on the default branch, so neither the perf control runs nor a dispatch-based release rehearsal can run before the phase PR merges, and the phase PR's own merge cuts 0.1.0. The rehearsal must run on the phase PR through a `pull_request` trigger. Second, CMake presets have no `${binaryDir}` macro, so D-20's emulator string must be rewritten. Third, AGENTS rule 7's gate-code ceiling (a tenth of `src/` from phase 3) is far below what a nesturbator-style port of the checks would produce, so every check must be written lean from the start.

**Primary recommendation:** Build the tracer first (header → `instance.c`/`testcard.c` → runner `--hash-frame` → `ci` preset → one local `cmake --workflow --preset ci`), widen locally to every preset including `be32` and wasm32 in containers, and only then open a short-lived draft "CI probe" PR to iterate the workflows. Close that PR before `/gsd-ship`, and run the release rehearsal on the phase PR itself as a `pull_request`-triggered prerelease that is published, verified and deleted.

## Project Constraints (from CLAUDE.md / AGENTS.md)

| # | Directive | Consequence for the plan |
|---|-----------|--------------------------|
| 1 | Reuse MIT/BSD/zlib/CC0 only, with notices; never open GPL/LGPL/unlicensed emulator source; reference emulators only as binaries | a7800's palette enters only from its BSD-3 file, never a GPL file; `libretro.h` is vendored with its MIT notice |
| 2 | Three deliverables; no window, audio-device or input-device code | the runner is headless; the hosted RetroArch job is a test, not product code |
| 3 | No ROM/BIOS/save bytes except manifest fixtures; no personal paths, e-mails or names; noreply identity; hygiene hooks | phase 1 commits no binary fixture; THIRD-PARTY-NOTICES names upstream holders only as their licences require |
| 4 | Integer-only, all state in the instance, same hashes everywhere, bounded calls | `nofp`, `abi.globals`, hash-equality across seven lanes |
| 5 | Every behaviour change lands with a test that `cmake --workflow --preset ci` runs; README short; docs in `docs/`; header terse | test inventory below; `docs.budgets` |
| 6 | Verification is automation-first (settled) | hosted evidence by run URL; the owner only for identity-bound steps |
| 7 | Repository checks are CMake `-P` or POSIX sh, ≤ 1/10 of `src/` from phase 3; each gate names its fault; ≤ 2 review-fix rounds + 1 gap round | gate-code budget, risk R-04 |
| 8 | One GSD step per session; never `--auto`/`--chain`, `/gsd-next`, etc.; never set `MUHTARI_ALLOW_*` bypasses | the execute step may span two sessions only by resuming `/gsd-execute-phase 1` |
| 9 | Branch `phase/01-…`; never commit on `main`; after ship: Conventional title, `gh pr merge --auto --squash` in a shell, confirm CI on head, then confirm release PR green | the probe-PR choreography (R-01) must close the probe before ship |
| 10 | Evidence is command + exit status + run URL, never a committed log | the bench trend data lives in artifacts/job summaries, not commits |
| 11 | Lessons are hypotheses until reproduced | XFER-026–032 adopted only as ENGINEERING already adopted them |
| Style | `muhtari_` public, `muhtari__<module>_` internal; fixed-width ints; hardware comments cite sources; no planning IDs in code, test names or user docs; ponytail ladder; copy-paste over dependency | test names like `core.frame.pitch_336`, never `frame03.*`; no D-NN in comments (nesturbator's comments cite D-NN; strip them when seeding) |

## Architectural Responsibility Map

| Capability | Primary tier | Secondary tier | Rationale |
|------------|-------------|----------------|-----------|
| Test-card pixels, frame driver, silence, size tags, allocator | Core library (`src/`, `include/`) | — | deterministic, integer-only, inside the hash domain |
| Native byte → RGB (warm palette) | Host helpers (`host/`) | runner, adapter | outside the hash (DEC.27); one table shared so runner PNG and libretro frame agree |
| SHA-256, PNG (CRC-32, Adler-32), bench percentiles, signal flag, clock | Runner (`runner/`) | — | host I/O; never in the core |
| libretro contract (AV info, pixel format, callbacks, float aspect/fps) | Adapter (`libretro/`) | host helpers | the adapter's division is the only float in the shipped code |
| Inventory, negative controls, ABI/floor/archive checks | Build system (CMake `-P`, POSIX sh) | — | rule 7 |
| Six-leg build, hash equality, hosted RetroArch, Ir gate | CI (`.github/workflows/ci.yml`) | presets | same commands as local |
| Release PR, attestation, publish, rehearsal | Release workflow (`release.yml`) + publish script | GitHub App | App token only on `push` to `main` |
| Settings and ruleset | `scripts/apply-repo-settings.sh` + `.github/rulesets/main.json` | owner's gh auth | identity-bound admin action |

## Facts verified this session (the CONTEXT "facts to verify" list)

| # | Fact | Result | How (command or source) |
|---|------|--------|-------------------------|
| F-01 | D-02 NTSC/PAL card hashes | **VERIFIED.** NTSC `9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4`, PAL `63de7dffa9b46da6a78e8fc0522941e9f2aa2804a0e117989e82270036f98a1e`, 256 distinct values each | independent Python model of D-02 rules 1–8, run with `python3 -I` over row-major bytes at pitch 320 |
| F-02 | Warm-table RGB of `$46` / `$C6` (D-03) | **CITED, re-check at seeding.** `$46` = `rgb_t(0xaf,0x51,0x63)` (R 175 > B 99 > G 81); `$C6` = `rgb_t(0x46,0x9a,0x46)` (G 154 largest). D-03 passes and `$46`/`$C6` stay. The table is the block commented `26.7 PHASE SHIFT - LUM3 from TREBORS PRO PALETTES V7` (`ntsc_hue4_warm`, `ntsc_hue12_warm`) in a7800 `src/mame/drivers/a7800.cpp` @7a2afdc1ea08fc331b16b750d8c1f02d4ef62fc8, whose first line is `// license:BSD-3-Clause` | WebFetch extraction of the BSD-3 file at the pin (a summarising model read it, so treat the bytes as cited, not verified); a direct raw download was refused by this session's permission classifier (R-09). The `host.palette` test asserts both orderings mechanically once the table is seeded |
| F-03 | `typedef uint32_t` status comparisons under `-Werror` | **VERIFIED** warning-free: Apple clang 21.0.0 and LLVM clang 21.1.8 (`-std=c17` with the ENGINEERING §2 set + `-Wsign-compare -Werror`, and `-Weverything`); `clang++ -std=c++17 -Weverything`; GCC 15.2 C17 with the GCC set. **One C++ wart:** `g++-15 -Wextra -Werror` rejects `cond ? st : MUHTARI_STATUS_X` ("enumerated and non-enumerated type in conditional expression"). MSVC `/W4 /WX` is **unverified** (settled by `abi.header.c17` on the two `ci-msvc` legs) | local compile of a probe header + use file; `ubuntu:26.04` container for GCC |
| F-04 | AAPCS short enums | **VERIFIED.** `arm-none-eabi-gcc` 16.2.0: `sizeof(enum {A=0,B=4})` = 1, `sizeof(muhtari_status)` = 4 | `-S` output of a probe file |
| F-05 | wasm-ld default stack vs runner buffers | **VERIFIED.** wasm-ld 21 default stack = 0x10000 = **64 KiB** (with `--stack-first`, data starts at 0x10000; with `-z stack-size=1048576` at 0x100000). Not only the 309,760-byte XRGB frame, but also the **77,440-byte native frame** exceeds it; every frame, PNG, palette-converted and bench buffer must be heap or static, never automatic | `-Wl,--Map` of a wasm32-wasi link in `ubuntu:26.04` with resolute clang-21/lld-21 |
| F-06 | D-18 toolchain | **VERIFIED.** resolute versions as D-18 states (`clang-21 1:21.1.8-6ubuntu1`, `wasi-libc 0.0~git20250726.3f7eb4c-4`); `libclang-rt-21-dev-wasm32` pulls the same clang-21 version; `--target=wasm32-wasi -mcpu=lime1 -Werror` gives no warning; `wasm32-wasip1` fails (`cannot open …/wasm32-unknown-wasip1/libclang_rt.builtins.a`); a CMake project with the D-18 toolchain file plus `CMAKE_CXX_COMPILER clang++-21`, `CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY` and `CMAKE_C_STANDARD_LIBRARIES -lwasi-emulated-signal` configures, links a C runner (output named `run`, **no `.wasm` suffix**) and compiles a C++ header check with no libc++. apt install of the wasm set took 15–17 s (emulated amd64 container) | container runs; CMake `Platform/WASI.cmake` sets no suffix |
| F-07 | Runner labels | **VERIFIED.** `ubuntu-26.04`, `ubuntu-26.04-arm`, `macos-15`, `macos-15-intel`, `windows-2025`, `windows-11-arm` all listed (also `macos-26`, `macos-26-intel`, `windows-11-vs2026-arm`) | github/docs `data/reusables/actions/supported-github-runners.md` @eb8f32b5dd88 (2026-09-17) via `gh api` |
| F-08 | Apple clang arm64 minimum OS clamp | **VERIFIED** with Apple clang 21 / ld-1267 (CI's Apple clang 17 unverified, same linker family): `-mmacosx-version-min=10.13 -arch arm64` → `LC_BUILD_VERSION minos 11.0`; `-arch x86_64` → **`LC_VERSION_MIN_MACOSX version 10.13`** (no `LC_BUILD_VERSION`); a bundle's `otool -L` lists only `/usr/lib/libSystem.B.dylib` | local `otool -l`, `vtool -show-build` |
| F-09 | ubuntu-26.04 defaults | **VERIFIED** in `ubuntu:26.04` (26.04.1 LTS): gcc = gcc-15 15.2.0, glibc 2.43, binutils 2.46; amd64 `-march=x86-64`, `-mtune=generic`, `--enable-cet`; both arches `--enable-default-pie`, `_FORTIFY_SOURCE=3`, `__SSP_STRONG__`; a `-shared` core has **no DT_RELR** (so no `GLIBC_ABI_DT_RELR`), needs `GLIBC_2.2.5 GLIBC_2.4`; a core-like file with a local array imports `__stack_chk_fail` unless `-fno-stack-protector`; a `-std=c17` runner needs `GLIBC_2.17 2.2.5 2.3.4 2.34 2.4` (`__fprintf_chk`, `__stack_chk_fail`), and the same file at GCC 15's default `gnu23` imports `__isoc23_strtoul@GLIBC_2.38` | container probe; the hosted image's own preinstalled toolchain may differ in packaging, not in Ubuntu's spec defaults |
| F-10 | Pins | **VERIFIED.** wasmtime v48.0.6 (published 2026-10-14) asset digests equal D-19's three values; RetroArch DMG URL returns 232,801,022 bytes, last-modified 2025-11-20; v1.22.2 is still RetroArch's latest release | `gh api repos/bytecodealliance/wasmtime/releases/tags/v48.0.6`; `curl -sI`; `gh api repos/libretro/RetroArch/releases/latest` |
| F-11 | release-please-action v5.0.0 outputs | **VERIFIED.** Every field of the created release becomes an output: `release_created`, `tag_name`, **`sha`**, **`id`**, `upload_url`, `draft`, `version`…; `prs_created`, `pr` | `src/index.ts` @45996ed1f6d0 L178–224; release-please `src/scm.ts` @v17.6.0 L63–72 (`ScmRelease {id, tagName, sha, …}`) |
| F-12 | `ci:`/`docs:` commits cut no release | **VERIFIED.** `ci`, `docs`, `chore`, `build`, `test`, `style`, `refactor` are hidden changelog sections; an empty changelog skips the release PR ("No user facing commits found") | release-please @v17.6.0 `src/util/filter-commits.ts` L20–32, `src/strategies/base.ts` L331–337 |
| F-13 | Presets | **VERIFIED.** No `${binaryDir}` macro exists (macros: `sourceDir sourceParentDir sourceDirName presetName generator hostSystemName fileDir dollar pathListSep $env $penv $vendor`); `"jobs": ""` in test presets needs presets **v11 = CMake ≥ 4.3**; a workflow preset may list any non-configure steps after its first configure step (so `test → package → test` is legal) | `cmake --help-manual cmake-presets` (CMake 4.4.3) |
| F-14 | Live repository state | **VERIFIED.** `scripts/apply-repo-settings.sh --check` → "no drift", exit 0; immutable releases `enabled: true`; `sha_pinning_required: true`; default token `read`, cannot approve; 0 rulesets; 0 releases; 0 Actions runs; `MUHTARI_APP_CLIENT_ID` variable present; `MUHTARI_APP_PRIVATE_KEY` present as Actions and Dependabot secret | `gh api` reads (no writes) |
| F-15 | resolute apt availability | **VERIFIED** candidates: `gcc-powerpc-linux-gnu`/`g++-powerpc-linux-gnu` 15.2.0, `qemu-user` 10.2.1 (`qemu-user-static`: none), `valgrind` 3.26.0, `time` 1.9, `clang-format-21`/`clang-tidy-21` 21.1.8, `jq` 1.8.1, `ninja-build` 1.13.2, `libc++-21-dev-wasm32` | `apt-cache policy` in `ubuntu:26.04` |
| F-16 | `video_gpu_screenshot=false` gives the core frame unscaled and unflipped | **VERIFIED by precedent:** nesturbator's required `retroarch-e2e` job on `macos-15` compares the screenshot pixel-for-pixel at exactly the core's size and passed (run 38676328559; DMG fetch + verify 12 s, one frame compare 2 s). **Unverified for the no-content launch on a hosted runner:** nesturbator's no-content `-L core --max-frames=5` case ran only locally in its phase 1 | `gh run view` |

## File inventory

Seeds are szTheory/nesturbator @3200aa868371 unless marked `@5533aeb` (its phase-1 squash, smaller). "new" means no seed. Strip every `D-NN`, phase or requirement ID from seeded comments (AGENTS Style). Commit bodies carry `Seeded from nesturbator <sha> <path> (same author, MIT).` (PROVENANCE §5a).

### Public header and core (`include/`, `src/`) — tracer
| File | Seed | Shaped by | Notes |
|---|---|---|---|
| `include/muhtari.h` | `include/nesturbator.h` (style only) | D-07, D-08, D-10 | exactly the D-08 block; terse one-line comments; ≤ 24 KB |
| `src/internal.h` | `src/internal.h` (pattern) | D-10, ARCH §4 | `struct muhtari {allocator; uint32_t audio_rem; …}`; first-release sizes via `offsetof`; `_Static_assert` layout per struct |
| `src/instance.c` | `src/instance.c` | D-08–D-10 | `check_size_in`/`write_out` helpers (seed L7–32), create/destroy/get_info/get_versions; **delete the malloc default** (seed L74–87) |
| `src/frame.c` | `src/frame.c` @5533aeb | D-04, D-10, D-26 | validate first, then render rows at `video_pitch`, then audio count from the 88/13,125 fraction |
| `src/testcard.c` | `src/testcard.c` | D-01, D-02 | rule order exactly as D-02 |
| `cmake/version.h.in` → `<build>/generated/muhtari_version.h` | new | ENGINEERING §5, D-08 | `MUHTARI_VERSION_{MAJOR,MINOR,PATCH}` private |

### Host helpers (`host/`)
| File | Seed | Shaped by | Notes |
|---|---|---|---|
| `host/palette.c`, `host/palette.h` | a7800 `src/mame/drivers/a7800.cpp` @7a2afdc1ea08 (BSD-3), warm NTSC block only | DEC.16, DEC.27, PROVENANCE §4–§5a | `SPDX-License-Identifier: MIT AND BSD-3-Clause` header naming the holders as the upstream `copyright-holders:` line does; `static const uint32_t` 0x00RRGGBB × 256; a conversion function XRGB8888 and RGB565 (8→5/6/5 by truncation, shared by adapter and libretro host test) |

### Runner (`runner/`)
| File | Seed | Shaped by | Notes |
|---|---|---|---|
| `runner/main.c` | `runner/main.c` @5533aeb (261 lines) | HOSTS §4, D-21 | options `--frames N`, `--hash-frame N`…, `--dump-frame N:FILE`…, `--bench N`, `--version`, `--help`; positional content → exit 2 in 0.1.0; output `frame N sha256 <hex>` (no ticks: 0.1.0 has no tick API) |
| `runner/sha256.c/.h` | `runner/sha256.c/.h` | D-27 | unchanged algorithm, renamed |
| `runner/png.c/.h` | new (replaces `runner/ppm.c`) | D-06, D-27 | stored-deflate writer (§Pattern 3) |
| `runner/crc32.c/.h` | `src/crc32.c` (moved to the runner) | D-27 | phase 2's `--probe` CRC-32 may move it into the core later |
| `runner/bench.c/.h` | `runner/bench.c` | FRAME-09 | ranks in parts per million to reach p99.999; samples heap-allocated |
| `runner/clock.c/.h` | `runner/clock.c` | D-21 | `_POSIX_C_SOURCE 200809L`; `_WIN32` QPC branch |
| `runner/stop.c/.h` | new (seed `runner/stop.c` is 192 lines with Windows console handlers) | D-21 | ~20 lines: ISO C `signal`, `volatile sig_atomic_t` |
| `runner/exit_status.h` | `runner/exit_status.h` | HOSTS §4, ENGINEERING §10 | 0, 1, 2 live; use a `const` table, not the seed's X-macro (§10 bans X-macros; R-08) |
| `runner/alloc.c` | new | D-10 | the six-line malloc wrapper the docs show; also used by the adapter |

### libretro adapter (`libretro/`)
| File | Seed | Shaped by | Notes |
|---|---|---|---|
| `libretro/libretro.c` | `libretro/libretro.c` (352 lines) | HOSTS §1, D-05, D-11 | drop serialize/memory/input to stubs; `retro_load_game(NULL)` → true, content → false; XRGB8888 or RGB565 under `USE_RGB565`; geometry 320×242, max 320×293, aspect `(float)(320*6)/(float)(242*7)`; fps `39375000.0/656711.0`; audio batch + single-sample fallback |
| `libretro/libretro.h` | vendored, `libretro.h` @RetroArch 69a4f0ea1e8a (= nesturbator's copy, SHA-256 `bd3398d2…acb`, checked this session) | PROVENANCE §8 | never edited |
| `libretro/muhtari_libretro.info.in` | `libretro/nesturbator_libretro.info` | HOSTS §1, discretion | generated by `configure_file` (`display_version`); `supports_no_game = "true"`, no `supported_extensions`, `license = "MIT"`, every other claim `"false"` |
| `libretro/CMakeLists.txt` | `libretro/CMakeLists.txt` | D-11 | `MODULE`, `PREFIX ""`, `.dylib` on Apple; plus a test-only `muhtari_libretro_rgb565` module (`USE_RGB565`, not installed) |

### Tests (`tests/`)
| File | Seed | Notes |
|---|---|---|
| `tests/tests.cmake` | `tests/CMakeLists.txt` (pattern) | includes each subsystem file (`include()`, never `add_subdirectory`) |
| `tests/{core,runner,libretro,abi,install,policy,release,perf,hygiene}/tests.cmake` | new | ≤ 400 lines each |
| `tests/check.h` | `tests/check.h` | unchanged macros |
| `tests/api/v1.c` | new | D-12 |
| `tests/core/test_*.c` (testcard, frame, audio, size_tags, create, info, no_alloc) | `tests/core/test_frame.c`, `test_api.c` | counting allocator for `core.no_alloc_after_load` |
| `tests/runner/test_sha256.c`, `test_png.c`, `test_bench.c`, `test_stop.c` | seed equivalents | PNG test decodes stored blocks with an independent bitwise CRC |
| `tests/libretro/host.c` | `tests/libretro/libretro_host.c` @5533aeb (342 lines; the tip is 2,767) | `dlopen`/`LoadLibraryA`; modes `xrgb8888`, `rgb565` |
| `tests/consumer/{CMakeLists.txt,main.c}` | `tests/consumer/*` | passes the malloc allocator; `CMAKE_MSVC_RUNTIME_LIBRARY` from a `-D` |
| `tests/embed/CMakeLists.txt` | `tests/embed/CMakeLists.txt` | only the `muhtari` target and `muhtari::muhtari` |
| `tests/header/header.c`, `header.cpp` | `tests/header/*` | compile-only OBJECT targets (works on wasm32 without libc++, F-06) |
| `tests/abi/float_fixture.c`, `global_fixture.c` | `tests/abi/float_fixture.c` | negative controls |
| `tests/retroarch/test.cfg.in`, `run_retroarch.cmake`, `bmp.c`, `compare.c` | `tests/retroarch/test.cfg.in`, `bmp_ppm.c`, `compare_frame.c`, `run_retroarch.cmake` @5533aeb | ≤ 300 lines total (DEC.56); no self-skip |
| `tests/inventory/{ci,nofp,hygiene,wasm32}.txt` | gabbaboy pattern (ENGINEERING §3) | sorted `name<TAB>labels` |
| `tests/perf/allow.txt` | `tests/perf/allow.txt` | empty with a header comment |
| `tests/hygiene/fixtures/*` | `tests/hygiene/fixtures/*` | for self-tests only |

### Check scripts (`tests/cmake/`, `scripts/`) — budget them (R-04)
| File | Seed | ~lines |
|---|---|---|
| `cmake/MuhtariTests.cmake` (`muhtari_add_test`, `MUHTARI_TIMEOUT_SCALE`, inventory compare via `cmake_language(DEFER)`) | gabbaboy `ExpectedTests.cmake` | 60 |
| `tests/cmake/expect_fail.cmake` | new | 25 |
| `tests/cmake/expect_output.cmake` | `expect_output.cmake` | 40 |
| `tests/cmake/check_junit.cmake` | new | 40 |
| `tests/cmake/abi.cmake` (undefined, globals, symbols in one script, mode argument) | `undefined_symbols.cmake`, `global_symbols.cmake` | 90 |
| `tests/cmake/api_version.cmake` | new | 25 |
| `tests/cmake/no_float.cmake` | `float_scan.cmake` | 60 |
| `tests/cmake/no_skip.cmake` | `skip_policy.cmake` (146 → rewrite) | 25 |
| `tests/cmake/archive_contents.cmake` (members, LICENSE + notices byte-equal, build-path scan) | `check_archives.cmake` @5533aeb (110) | 70 |
| `tests/cmake/floor.cmake` (imports, version needs, minos, exports, `/directives`) | new | 80 |
| `tests/cmake/install_line.cmake` | `check_install_line.cmake` | 50 |
| `tests/cmake/ci_required.cmake` | `ci_required_rollup.cmake` | 50 |
| `tests/cmake/dependabot.cmake` | new | 35 |
| `tests/cmake/notices.cmake`, `budgets.cmake`, `single_version.cmake`, `timeouts.cmake`, `info_claims.cmake`, `vendored.cmake` | seeds where named | 25–35 each |
| `tests/cmake/perf_compare.cmake` | `perf_compare.cmake` (291 → rewrite) | 120 |
| `tests/cmake/hashes.cmake` (write `hashes.txt`, compare N files and assert N) | `write_hashes.cmake`, `hash_inventory.cmake` | 40 |
| `scripts/fetch-pin.sh` + `pins.txt` | new | 35 |
| `scripts/classify-changes.sh`, `scripts/check-title.sh` | new | 20 each |
| `scripts/publish.sh` (shared by real and rehearsal) | `release.yml` publish steps | 90 |
| `scripts/apply-repo-settings.sh` (extend `--check`) | existing | +60 |

### Presets, CMake, packaging, toolchains
| File | Seed | Notes |
|---|---|---|
| `CMakeLists.txt` | `CMakeLists.txt` | `cmake_minimum_required(VERSION 3.25)`; C only for embedders; `enable_language(CXX)` top-level; core flags `-fno-stack-protector -U_FORTIFY_SOURCE -ffile-prefix-map=…` (GCC/Clang), `/GS- /Zl` (MSVC) on the `muhtari` target only (R-05); warnings set from ENGINEERING §2; options `MUHTARI_BUILD_LIBRETRO`, `MUHTARI_NOFP`, `MUHTARI_HYGIENE`, `MUHTARI_SHORT_RUNS`, `MUHTARI_TIMEOUT_SCALE` |
| `CMakePresets.json` (version 11) | `CMakePresets.json` | presets `dev ci ci-msvc asan nofp hygiene be32 wasm32 bench`; `ci` sets `CMAKE_OSX_DEPLOYMENT_TARGET=10.13`; `ci-msvc` sets `CMAKE_MSVC_RUNTIME_LIBRARY`; workflow `ci` = configure, build, test `ci` (excludes label `release`), package, test `ci-release` (label `release`) |
| `cmake/packaging.cmake` | `cmake/packaging.cmake` | version-free names `muhtari-<component>-<os>-<arch>` |
| `cmake/muhtari.pc.in`, `cmake/muhtariConfig.cmake.in` | seeds | relocatable |
| `cmake/toolchains/ppc-linux-gnu.cmake` | new | `powerpc-linux-gnu-gcc`/`g++`; emulator `qemu-ppc;-L;/usr/powerpc-linux-gnu` |
| `cmake/toolchains/wasm32-wasi.cmake` | new | D-18 plus the three lines from F-06 |
| `.clang-format` | `.clang-format` | LLVM base, indent 4, 100 columns |
| `version.txt` (`0.0.0`), `.release-please-manifest.json` (`{".": "0.0.0"}`), `release-please-config.json` | seeds | ENGINEERING §5 JSON exactly; no `extra-files` |
| `pins.txt` | new | `wasmtime 48.0.6 <url> a04bbb20…` (+ aarch64 rows for local) and `retroarch-dmg 1.22.2 <url> 81b79121…` |

### GitHub (`.github/`)
| File | Seed | Notes |
|---|---|---|
| `.github/workflows/ci.yml` | `ci.yml` | jobs in §CI below; `workflow_call` input `ref`; every checkout `persist-credentials: false` and `ref: ${{ inputs.ref }}` |
| `.github/workflows/release.yml` | `release.yml` | adds `sha` checkout, rehearsal mode, re-runnable publish, user-download and install-line checks |
| `.github/workflows/nightly.yml` | `nightly.yml` (pattern only) | bench × 3 + wasm32 row + rolling issue; `pull_request` with `paths` on its own inputs; `workflow_dispatch` input to force a failure |
| `.github/workflows/dependabot-automerge.yml` | lattice_stripe pattern (ENGINEERING §7) | ~25 lines |
| `.github/rulesets/main.json` | `rulesets/main.json` | unchanged content |
| `.github/dependabot.yml` | `dependabot.yml` | adds the minor/patch group and `commit-message.prefix: ci` |

### Docs and records
`README.md` (status, install line, Requirements per D-17, RetroArch and runner quick start, docs ladder, licence; ≤ 12 KB), `docs/embedding.md` (size tags, allocator wrapper), `docs/runner.md` (options, exit table, test-card section per D-06), `docs/testing.md` (presets, inventory, negative controls, floors commands with "checked from the binaries, not run on these systems"), `docs/maintaining.md` (App/secrets, settings, release, rehearsal, withdrawal, pins bump policy), `THIRD-PARTY-NOTICES.md` (libretro.h MIT; a7800 palette BSD-3), `PROVENANCE.md` §8 vendored row, CONFORMANCE §2(a) wasmtime 48.0.6 (D-19), move the folded todo to `done/`, add pending todos "promote wasm32 lane" (D-22) and "arm the Ir gate" (R-01).

## Test inventory

Names follow `<subsystem>.<behaviour>`. "SELF" marks the negative-control pair `x.self_test`, run through `expect_fail.cmake -DCODE=<n> -DREGEX=<rule>` (never bare `WILL_FAIL`). Lanes: **6** = the six `ci`/`ci-msvc` legs (also `dev`, `asan`); **be32**/**wasm32** run label `portable`; **nofp** runs label `abi`; **hyg** = `hygiene` preset on ubuntu x64.

| Test | Labels | Lanes | Proves |
|---|---|---|---|
| `core.testcard.pinned_hash` | core, portable | 6, be32, wasm32 | NTSC hash `9eb011e2…` (F-01) |
| `core.testcard.all_colours` | core, portable | 6, be32, wasm32 | 256 values (D-05) |
| `core.testcard.static` | core, portable | 6, be32, wasm32 | frame 1 = frame 600 |
| `core.frame.pitch_336` | core, portable | 6, be32, wasm32 | padding untouched; rows equal (D-26) |
| `core.frame.validate_first` | core, portable | 6, be32, wasm32 | pitch 319/65537 → ARGUMENT; capacity 799 → BUFFER_TOO_SMALL with no state change; `audio_count` written |
| `core.audio.silence` | core, portable | 6, be32, wasm32 | 800, 801, 800, 801…; Σ over N frames = ⌊N·119402·88/13125⌋; all zero |
| `core.api.size_tags` | core, portable | 6, be32, wasm32 | below first size, zero tail, non-zero tail, output `min(size)` for every struct |
| `core.create.allocator` | core, portable | 6, be32, wasm32 | NULL alloc/free → ARGUMENT; failing alloc → NO_MEMORY; free gets the alloc size; `destroy(NULL)` |
| `core.info` | core, portable | 6, be32, wasm32 | D-08 constants |
| `host.palette` | host, portable | 6, be32, wasm32 | warm table: R largest in `$46`, G largest in `$C6` (D-03); XRGB8888 and RGB565 conversion of spot values |
| `core.no_alloc_after_load` | perf, portable | 6, be32, wasm32 | counting allocator: 0 calls in 600 frames after create (FRAME-09) |
| `abi.api_v1` | abi, portable | 6, be32, wasm32, nofp | `tests/api/v1.c` |
| `abi.api_version` + SELF | abi | 6, nofp | D-12 |
| `abi.undefined` + SELF (float fixture) | abi | 6, nofp | allowlist `memcpy memmove memset memcmp` + per-target (`bzero` on Darwin, `_GLOBAL_OFFSET_TABLE_` on x86-64 ELF, libgcc helpers on ppc32) |
| `abi.globals` + SELF | abi | 6, nofp | no `B b D d C` |
| `abi.symbols` | abi | 6, nofp | defined `muhtari_*` = declared functions, both ways |
| `abi.header.c17`, `abi.header.cxx17` | abi | 6, nofp | build compile-only targets with `-Werror` / `/W4 /WX` |
| `abi.nofp_fixture` | abi | nofp only (`nofp.txt`) | a `double` under `-mgeneral-regs-only` fails compile or `abi.undefined` |
| `runner.hash` | runner, portable | 6, be32, wasm32 | `--frames 1 --hash-frame 1` prints the pinned line |
| `runner.hash.repeat` | runner, portable | 6, be32, wasm32 | `--frames 600 --hash-frame 1 --hash-frame 600` |
| `runner.dump_png` | runner, portable | 6, be32, wasm32 | size 232,645 B and pinned file SHA-256 (pin set at implementation) |
| `runner.png` | runner, portable | 6, be32, wasm32 | CRC-32("123456789") = `CBF43926`, Adler-32("Wikipedia") = `11E60398`; stored blocks decode to palette(card) |
| `runner.sha256` | runner, portable | 6, be32, wasm32 | FIPS 180-4 answers |
| `runner.bench.percentile`, `runner.bench.format` | runner, bench, portable | 6, be32, wasm32 | nearest rank incl. p99.999; report shape |
| `runner.stop` | runner, portable | 6, be32, wasm32 | `raise(SIGINT)` sets the flag (WASI emulated signal calls the handler — unverified, R-07) |
| `runner.usage.{no_args,unknown,positional,frame_range,dump_syntax}` | runner, portable | 6, be32, wasm32 | exit 2 |
| `runner.exit.failure` | runner, portable | 6, be32, wasm32 | unwritable dump path → 1 |
| `runner.exit.table` | runner | 6 | `--help` table = `docs/runner.md` table |
| `runner.version` | runner, portable | 6, be32, wasm32 | equals `version.txt` |
| `runner.hashes_file` | runner, portable | 6, be32, wasm32 | writes `<build>/hashes.txt` for `hash-equality` |
| `libretro.host.xrgb8888` | libretro, portable | 6, be32 | no game; pixel format; one `video_cb` per frame at 320×242 / 1280; pixels; fps; aspect; batch audio |
| `libretro.host.rgb565` | libretro, portable | 6, be32 | the same through `USE_RGB565` (pitch 640), single-sample audio path |
| `libretro.vendored` | libretro | 6 | `libretro.h` SHA-256 |
| `libretro.info_claims` + SELF | libretro | 6 | each `"true"` claim maps to a registered test |
| `install.stage` (fixture), `install.consumer`, `install.consumer.dll_crt`, `embed.subdirectory` | install | 6 | FRAME-02; `dll_crt` builds the consumer with `MultiThreadedDLL` (ignored off MSVC), so one `ci.txt` serves all legs |
| `policy.no_skip` + SELF | policy | 6 | no `SKIP_*`/`DISABLED` |
| `policy.notices` | policy | 6 | SPDX lines ↔ notices sections ↔ `.info` licence |
| `release.single_version_source` | policy | 6 | version string only in `version.txt`, manifest, CHANGELOG |
| `docs.budgets` | policy | 6 | README ≤ 12 KB, header ≤ 24 KB, each doc ≤ 40 KB |
| `perf.gate.{identical,plus_1_00,plus_1_01,unstable,allowed}` | perf | 6 | Ir-gate script on synthetic Cachegrind files |
| `release.archive_contents` + SELF | release | 6 (second test step) | members, notices byte-equal, no build path |
| `release.floor` + SELF | release | 6 (second test step) | D-13–D-15 + libretro exports `retro_*` only |
| `hygiene.tree` | hygiene | hyg | `scripts/hygiene.sh --tree` |
| `hygiene.format` | hygiene | hyg | clang-format 21 exactly |
| `hygiene.no_float` + SELF | hygiene | hyg | text scan of `src/`, `include/` |
| `hygiene.analyzer` | hygiene | hyg | `clang-tidy-21 -checks='-*,clang-analyzer-*'`, zero findings |
| `hygiene.timeout_comments` + SELF | hygiene | hyg | every `timeout-minutes` has a run ID or `provisional` |
| `policy.ci_required` + SELF | hygiene | hyg | rollup jq on synthetic `needs` |
| `policy.dependabot` | hygiene | hyg | decision expression on synthetic events (FRAME-08) |
| `policy.title` | hygiene | hyg | both title regexes on good/bad fixtures |
| `ci.classify` | hygiene | hyg | mixed, renamed, empty lists |

Dropped versus ENGINEERING's list, each with the reason (gate-code budget): `hygiene.action_pins` (the live setting `sha_pinning_required: true` already refuses an unpinned action, and `--check` watches the setting); `release.firmware_scan` (no ROM or BIOS code path exists until phase 2; seed it then). After every test step CI runs `tests/cmake/check_junit.cmake` against the inventory filtered by the step's labels (a CI step, since a test cannot read its own run's JUnit file).

### CI jobs and `CI required`
| Job | Runner | In `needs` of `CI required` | Notes |
|---|---|---|---|
| `changes` | ubuntu-26.04 | yes | heavy unless `pull_request` and every path is under `.planning/` |
| `hygiene` | ubuntu-26.04 | yes | preset `hygiene` |
| `title` | ubuntu-26.04 | yes | `scripts/check-title.sh` on the live title and body |
| `build` × 6 | the six labels | yes | workflow `ci`/`ci-msvc`; uploads `archives-*`, `hashes-*`; macOS arm64 also runs the install-line check |
| `asan` | ubuntu-26.04 | yes | clang-21 |
| `nofp` × 2 | ubuntu-26.04, -arm | yes | GCC 15 |
| `be32` | ubuntu-26.04 | yes (while ≤ 5 min) | uploads `hashes-be32` |
| `hash-equality` | ubuntu-26.04 | yes | needs `build`, `be32`; exactly 7 files |
| `retroarch` | macos-15 | yes | builds `ci`, extracts the libretro zip, runs the DMG pinned in `pins.txt` |
| `perf` | ubuntu-26.04 | yes (skip accepted outside PRs) | report-only until armed |
| `wasm32` | ubuntu-26.04 | **no** (D-22) | `cmp` against `hashes-linux-x64` |
| `CI required` | ubuntu-26.04 | — | `if: always()`; jq over `toJSON(needs)` |

## Build order, tracer and waves

**Tracer (prove the architecture first):** `include/muhtari.h` → `src/instance.c`, `src/frame.c`, `src/testcard.c` → `runner/main.c` + `sha256.c` → `CMakeLists.txt` + `CMakePresets.json` (`ci`) + `cmake/MuhtariTests.cmake` with `tests/inventory/ci.txt` → `core.testcard.pinned_hash`, `runner.hash`. Done when `cmake --workflow --preset ci` passes locally on the Mac and prints `9eb011e2…`. Everything else hangs off this.

`parallelization: false`, one branch. Recommended plans at coarse granularity (**9 plans, 5 waves**; nesturbator's narrower phase 1 took 12):

| Wave | Plan | Content | Verifiable where |
|---|---|---|---|
| 1 | 01 Tracer | header, core, runner hash, `ci`/`dev` presets, inventory + junit + expect_fail machinery, core and runner hash tests | Mac |
| 2 | 02 Runner complete | PNG/CRC/Adler, palette seed + notices, bench, stop, exit table, usage tests, docs/runner.md | Mac |
| 2 | 03 libretro | adapter, `.info`, vendored header, in-process host (both formats), `libretro.*` tests | Mac |
| 3 | 04 Package and ABI | install/embed/consumer, packaging, `abi.*`, `release.archive_contents`/`floor`, install-line script, `asan`/`nofp`/`hygiene` presets and their tests, docs budgets, README | Mac + `ubuntu:26.04` container for GCC/nofp/hygiene |
| 3 | 05 Portability | `be32` and wasm32 toolchains/presets, `pins.txt` + `fetch-pin.sh`, timeout scale | containers (qemu-ppc, wasmtime) |
| — | **session boundary** | everything above is green locally; resume with `/gsd-execute-phase 1` | |
| 4 | 06 CI | `ci.yml` (all jobs), `classify`/`title` scripts and policy tests; open the draft probe PR; iterate to green; provisional timeouts; apply ruleset once `CI required` reports | GitHub |
| 4 | 07 Release | `release.yml` + `publish.sh` + rehearsal on the probe PR; `release-please-config.json`; Dependabot config + auto-merge + `policy.dependabot`; extend `apply-repo-settings.sh --check` | GitHub |
| 4 | 08 Perf and nightly | Cachegrind job + compare script + self-tests; nightly bench × 3 + wasm32 row + rolling issue | GitHub (nightly via its `pull_request` paths trigger) |
| 5 | 09 Close | docs/maintaining.md, CONFORMANCE/PROVENANCE edits, todos, close the probe PR, phase evidence (run URLs) | GitHub |

Serial constraints: 01 before all; 04 needs 02 and 03 (archives contain all three deliverables); 06 needs 04 and 05; 07 needs 06 (`uses: ./ci.yml`, ruleset); 08 needs 06. Post-merge (outside the execute step, GSD-HANDOFF §3 steps 6–8): two `workflow_dispatch` perf control runs on `main`, one forced-failure nightly dispatch, then a `ci:` PR that arms the gate.

**Can one execute session carry it?** Probably not. Plans 01–05 are local and fast, but plans 06–08 wait on six-leg CI, the hosted macOS job and release runs, and gabbaboy needed 18 release-fix PRs on its first tag. The natural boundary is after plan 05: the tree is fully green locally, and nothing after it can be checked without GitHub. Resuming `/gsd-execute-phase 1` in a new session respects AGENTS rule 8 without a roadmap change.

## Standard Stack

| Tool | Version | Where | Status |
|---|---|---|---|
| CMake | ≥ 4.3 for presets v11 (images: 4.4.2–4.4.4; local 4.4.3) | all | VERIFIED (F-13, ENGINEERING §1) |
| GCC | 15.2 (ubuntu default `gcc`) | Linux legs, `nofp` | VERIFIED (F-09) |
| clang-21 / clang-format-21 / clang-tidy-21 | 21.1.8 | `asan`, `hygiene`, wasm32 | VERIFIED (F-15) |
| Apple clang | 17.0.0 on macos-15 (local 21) | macOS legs | CITED (ENGINEERING §1) |
| MSVC | VS 2026 18.10, env from vswhere + vcvarsall | Windows legs | CITED; seed step proven in nesturbator |
| `powerpc-linux-gnu-gcc`/`g++` 15.2 + `qemu-user` 10.2.1 | resolute apt | `be32` | VERIFIED available (F-15) |
| wasi-libc + libclang-rt-21-dev-wasm32 + wasmtime 48.0.6 | apt + pinned tarball | wasm32 | VERIFIED (F-06, F-10) |
| valgrind 3.26.0 | apt | `perf` | VERIFIED available |
| GNU `time` 1.9 / BSD `/usr/bin/time -l` | image/apt | nightly RSS | available in resolute (F-15); presence on the hosted image unverified, so install it with apt |
| RetroArch 1.22.2 Metal DMG | pinned SHA-256 | `retroarch` job | VERIFIED (F-10) |
| Actions | SHAs in ENGINEERING §1 L43–51 (checkout v7.0.1 `3d3c42e5…`, upload v7.0.2 `cf430e03…`, download v8.0.2 `9000827c…`, create-github-app-token v3.3.0 `64ab729a…`, attest v4.2.2 `1e69f48a…`, release-please-action v5.0.0 `45996ed1…`, fetch-metadata v3.1.0 `25dd0e34…`) | workflows | CITED (ENGINEERING §1, read this session) |

## Package Legitimacy Audit

No npm, PyPI or crates package is installed in this phase. External bytes are: Ubuntu apt packages, which are signed and stay unpinned (D-24); two pinned downloads whose digests equal the publisher's release API values (F-10); and SHA-pinned GitHub Actions under the live `sha_pinning_required` setting. **SLOP:** none. **SUS:** none. `gsd-tools package-legitimacy` does not apply to these ecosystems.

## Architecture Patterns

### System diagram
```
caller buffers ──► muhtari_run_frame ─► validate (size tag, pitch 320..65536, capacity)
                        │                       │ error → status, nothing changed
                        ▼                       ▼
                 testcard_pixel(x,y,242) ──► video bytes (native $hl)   audio: 800/801 zeros (88/13125 fraction)
                        │
      ┌─────────────────┴──────────────────────────┐
runner: SHA-256 over 320×242 bytes → stdout         adapter: host palette → XRGB8888 / RGB565 → video_cb
        palette → PNG (stored deflate) → file                mono zeros → stereo batch → audio_batch_cb
        bench: clock per frame → percentiles → stderr        fps/aspect as double/float (only floats)
      │                                                 │
hash-equality (7 lanes), runner.dump_png pin       libretro.host (in-process) ; RetroArch DMG (hosted, sips → BMP → compare)
```

### Pattern 1: size-tag helper (seed `src/instance.c` L7–32, adapted)
```c
/* Reads the untrusted tag first; accepts first <= size, a zero tail beyond ours. */
static muhtari_status check_size(const void *s, uint32_t first, uint32_t ours)
{
    const uint8_t *b = (const uint8_t *)s;
    uint32_t size;
    memcpy(&size, s, sizeof size);
    if (size < first) {
        return MUHTARI_STATUS_ERR_STRUCT_SIZE;
    }
    for (uint32_t i = ours; i < size; i++) {
        if (b[i] != 0u) {
            return MUHTARI_STATUS_ERR_STRUCT_SIZE;
        }
    }
    return MUHTARI_STATUS_OK;
}
```
For input structs smaller than `ours`, copy `min(size, ours)` bytes into a zeroed local before reading fields; for outputs build a full local and copy `min(size, ours)` back (seed `write_out`).

### Pattern 2: audio count without drift
`n = (rem + 119402u * 88u) / 13125u; rem = (rem + 119402u * 88u) % 13125u;` fits `uint32_t` (10,507,376 + 13,124 < 2³²). The sequence starts 800, 801, 800, 801 (derived). `rem` is updated only after validation succeeds.

### Pattern 3: PNG with stored deflate (D-06, D-27)
Signature; `IHDR` (320, 242, depth 8, colour type 2, compression 0, filter 0, interlace 0); one `IDAT`: zlib header `78 01` (`0x7801 % 31 == 0`), stored blocks of ≤ 65,535 bytes (`00`/`01` final, `LEN` LE16, `NLEN`), raw = 242 rows × (filter byte 0 + 960) = 232,562 bytes → 4 blocks, Adler-32 big-endian; `IEND`. No `gAMA`, `sRGB`, `iCCP` or `pHYs`. Total **232,645 bytes** (derived). Write row by row from a heap/static row buffer (F-05).

### Pattern 4: wasm32 preset (corrects D-20's macro)
```json
{ "name": "wasm32", "inherits": "base",
  "toolchainFile": "${sourceDir}/cmake/toolchains/wasm32-wasi.cmake",
  "cacheVariables": { "CMAKE_BUILD_TYPE": "Release", "CMAKE_COMPILE_WARNING_AS_ERROR": "ON",
    "MUHTARI_BUILD_LIBRETRO": "OFF",
    "CMAKE_CROSSCOMPILING_EMULATOR": "${sourceDir}/build/pins/wasmtime-48.0.6/wasmtime;run;--dir=${sourceDir}::${sourceDir}" } }
```
`binaryDir` is `${sourceDir}/build/${presetName}`, so one preopen covers source and build. Configure fails with a clear message if the emulator path is missing (`scripts/fetch-pin.sh wasmtime` creates it). Toolchain additions from F-06: `CMAKE_CXX_COMPILER clang++-21`, `CMAKE_CXX_COMPILER_TARGET wasm32-wasi`, `CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY`, `CMAKE_C_STANDARD_LIBRARIES -lwasi-emulated-signal`.

### Pattern 5: `CI required` roll-up (seed `ci.yml` L436–452, extended for light changes)
```yaml
run: echo "$NEEDS" | jq -e --arg event "$EVENT" --arg heavy "$HEAVY" 'to_entries | all(
  .value.result == "success"
  or (.key == "perf" and .value.result == "skipped" and $event != "pull_request")
  or ($heavy == "false" and .value.result == "skipped" and (.key | IN("changes","hygiene","title") | not)))'
```
`policy.ci_required` extracts this program from `ci.yml` and runs it on synthetic `needs`.

### Pattern 6: release and rehearsal
- `release.yml` on `push` to `main`: App token → release-please → `gh pr merge --auto --squash` on a created PR; on `release_created`, `ci` via `uses: ./.github/workflows/ci.yml` with `ref: ${{ needs.release-please.outputs.sha }}`; `publish` runs `scripts/publish.sh real <tag> <release id>` (find the draft by the `id` output, F-11).
- `release.yml` also on `pull_request` (paths: `release.yml`, `scripts/publish.sh`, `release-please-config.json`, `cmake/packaging.cmake`; same-repo heads only) and `workflow_dispatch`: `ci` on the PR head, then `scripts/publish.sh rehearsal rehearsal-${{ github.run_id }}` with `GITHUB_TOKEN` (`contents: write`, `id-token: write`, `attestations: write`). The script creates a **prerelease** `rehearsal-<run_id>` (`--latest=false`), uploads, attests (`actions/attest` step), verifies each zip and the tampered copy, publishes it, runs `gh release verify`, downloads from `releases/download/rehearsal-<id>/`, checks `SHA256SUMS` and `gh attestation verify --signer-workflow szTheory/muhtari/.github/workflows/release.yml`, then deletes the release and its tag (R-02).
- A `install-line` job on `macos-15` after `publish` runs the README line verbatim (rehearsal: URL rewritten to the rehearsal tag) with `HOME` pointed into the job's temp dir, and requires `cores/muhtari_libretro.dylib` and `info/muhtari_libretro.info` (FRAME-07 "against the latest release").

### Anti-patterns to avoid
- Porting nesturbator's grown checks (`check_archives.cmake` 693 lines, `info_claims.cmake` 578, `run_retroarch.cmake` 1,197): port the @5533aeb sizes or rewrite.
- Self-skipping tests (nesturbator's phase-1 RetroArch test exited 77 off macOS); here the hosted job is a CI step, never a CTest.
- Locals ≥ 64 KiB anywhere that runs under wasm32 (F-05).
- `-std=gnu23` anywhere in the runner (F-09: `__isoc23_strtoul@GLIBC_2.38`).
- `GET releases/tags/{tag}` for drafts (it omits drafts; use the release `id`).

## Don't Hand-Roll

| Problem | Don't build | Use instead | Why |
|---|---|---|---|
| PNG decode of RetroArch's screenshot | an inflate decoder | `sips --deleteColorManagementProperties` then `sips -s format bmp`, both for the runner PNG and the screenshot, and the seeded BMP reader | proven pixel-exact in nesturbator; also validates the runner's PNG with Apple's own decoder |
| Release versioning | version edits in several files | release-please `simple` + `configure_file` | ENGINEERING §5 (the `extra-files` markers broke releases in nesturbator) |
| Attestation | own signing | `actions/attest` + `gh attestation verify` | OIDC, no secrets |
| Packaging | zip scripts | CPack ZIP components | seed works on six legs |
| Test framework | a library | `tests/check.h` (48 lines) | D-27 |

## Common Pitfalls and Risks

### R-01: `workflow_dispatch` cannot run before the phase PR merges (new evidence vs ENGINEERING §5/§8 sequencing)
**What goes wrong:** "This trigger only receives events when the workflow file is on the default branch" (github/docs `data/reusables/actions/workflow-dispatch.md`). The dispatch rehearsal, the perf control runs and the forced nightly failure are impossible before merge, and the merge of a `feat:` phase PR immediately opens the 0.1.0 release PR, which the App auto-merges.
**Mitigation:** the rehearsal runs on `pull_request` (Pattern 6). Perf stays report-only in the phase PR, the two control dispatches run on `main` right after merge, and arming lands in a later `ci:` PR (D-22 sets the same pattern for wasm32; `ci:` cuts no release, F-12). The forced nightly failure is a post-merge evidence command.

### R-02: an untagged draft cannot exercise "every publish step" (new evidence vs ENGINEERING §5 "untagged draft")
**What goes wrong:** `gh release verify` and public downloads need a published release. With immutable releases on (F-14), a published release can be deleted, after which its tag can be deleted but the name is never reusable (github/docs immutable-releases).
**Mitigation (recommended):** rehearse as a published **prerelease** with a unique `rehearsal-<run_id>` tag and `--latest=false`, then delete it. The only real-path difference left is the `releases/latest/download/` alias. **Fallback:** the draft-only rehearsal from ENGINEERING §5; then `publish`, `gh release verify` and the latest download are first exercised on 0.1.0, and recovery is "Re-run failed jobs" (XFER-027). Either way this touches DEC.49's wording ("a rehearsal before the first tag") and the owner should note it. **Unverified:** that `GITHUB_TOKEN` on a same-repo `pull_request` can take `contents: write` and `id-token: write` under the repo's read default. The probe PR settles it.

### R-03: `${binaryDir}` is not a preset macro (corrects D-20 text)
Use Pattern 4. Verified (F-13).

### R-04: gate-code budget (AGENTS rule 7)
**What goes wrong:** the check scripts in the inventory above add up to about 1,100–1,300 lines, plus 290 existing lines of `scripts/`. From phase 3 the total must stay under a tenth of `src/`, and phase 3's `src/` will plausibly be 5,000–7,000 lines (nesturbator's whole v1 `src/` is 8,297).
**Mitigation:** write the leanest form of each check, merge siblings (one `abi.cmake`, one `hashes.cmake`), drop the two checks listed above, and keep a line tally in each plan's SUMMARY. Tell the owner if phase 3 will exceed the ceiling, rather than trimming gates blindly.

### R-05: stack protector, fortify and MSVC `/GS` (Claude's discretion → decided)
Images emit `__stack_chk_fail` by default (F-09), Apple clang's default also protects frames with arrays, and MSVC's default `/GS` references `__security_check_cookie`/`__security_cookie` (assumed). **Recommendation:** build the `muhtari` target only with `-fno-stack-protector -U_FORTIFY_SOURCE` (GCC/Clang) and `/GS- /Zl` (MSVC). The allowlist then stays the four memory functions plus documented compiler helpers on every lane, and bare-metal embedders see the same contract. Cost: none for a core with no input-indexed stack arrays. The runner and adapter keep their platform hardening, within the floors (runner `__fprintf_chk@GLIBC_2.3.4` < 2.34).

### R-06: macOS x86_64 floor uses the old load command
`release.floor` must read `LC_VERSION_MIN_MACOSX version` as well as `LC_BUILD_VERSION minos` (F-08), or the x86_64 check silently finds nothing.

### R-07: WASI emulated signals
`signal()` links with `-lwasi-emulated-signal`, but whether `raise(SIGINT)` calls the handler under wasmtime is unverified. If it does not, `runner.stop` must leave the `portable` label for wasm32 only (`wasm32.txt` differs anyway).

### R-08: X-macro exit table
HOSTS §4 says "one X-macro table", but ENGINEERING §10 forbids X-macros. Use a `const` struct array printed by `--help`. This is a style conflict, not a behaviour one.

### R-09: palette transcription
This session's permission classifier refused a raw download of the a7800 file, and a summarising fetch is not a safe way to copy 256 triplets. **Mitigation:** the executor fetches `src/mame/drivers/a7800.cpp` at `7a2afdc1ea08fc331b16b750d8c1f02d4ef62fc8` into a scratch directory outside the repo (owner approval if the classifier blocks again), checks that line 1 is `// license:BSD-3-Clause`, extracts the 16 `ntsc_hue*_warm` rows mechanically, and records the upstream file's SHA-256 in `THIRD-PARTY-NOTICES.md`. `host.palette` (label `host;portable`, add to the inventory) asserts D-03 and spot values. a7800 as a whole is GPL-2.0-or-later and only that file is cleared (PROVENANCE §2): open nothing else.

### R-10: MSVC unknowns (settled by the first Windows run)
`/W4 /WX` on `uint32_t` vs enum constants (C4389/C4245, assumed silent for non-negative constants); a `/MT` EXE/DLL importing only `KERNEL32.dll` (assumed); `llvm-nm` reading the `.lib` (ENGINEERING says yes). Put all three in plan 06's first CI iteration.

### R-11: hosted RetroArch with no content
nesturbator ran no-content `-L core --max-frames=5 --max-frames-ss --max-frames-ss-path=…` only locally (@5533aeb `tests/retroarch/run_retroarch.cmake` L93–103). Run that exact form first in the probe PR. If the hosted app refuses to start a core without content, stop and report a blocker with RetroArch's `--verbose` log. Each possible fix changes a decision and needs the owner: letting the adapter accept a content file and still show the card would reverse the discretion that content returns false. Keep the seed's isolated `HOME`, `CFFIXED_USER_HOME`, `bundle_assets_extract_enable = "false"` and the 50 s process timeout.

### R-12: CTest timeouts under emulators
qemu-ppc and wasmtime slowdowns are unmeasured. Start `MUHTARI_TIMEOUT_SCALE` provisional at 10 (`be32`) and 5 (wasm32), and fix them at phase close from measured runs. `be32` becomes nightly-only if it exceeds 5 minutes (ENGINEERING §9 flip).

### R-13: `install.consumer` on Windows twice vs one `ci.txt`
Register `install.consumer` and `install.consumer.dll_crt` on every leg (the runtime variable is ignored off MSVC), so the inventory stays identical. `ctest -R install.consumer` matches both.

### R-14: probe PR vs `/gsd-ship`
`/gsd-ship` runs `gh pr create` and fails if an open PR exists for the branch (`workflows/ship.md`, create_pr step). Close the draft probe PR before ship. The ruleset can be applied once the probe PR's run has reported `CI required`. That is an admin write with the owner's `gh` identity, so the plan should run `scripts/apply-repo-settings.sh` and then `--check`, or hand the owner that one command.

### R-15: release-please and the ruleset
Without a required check, `gh pr merge --auto` on the release PR may merge at once. Apply the ruleset before the phase PR merges.

### R-16: `D-10` "zero means absent for every version number"
`library_major` is legitimately 0 for 0.1.0. Read "absent" as applying to `api`, `abi` and `behaviour_revision` (which start at 1), and say so in `docs/embedding.md`.

### R-17: GCC C++ ternary warning (F-03)
Not a header defect, but C++ embedders writing `cond ? st : MUHTARI_STATUS_X` under `g++ -Wextra -Werror` get an error. Note it in `docs/embedding.md` (cast or compare instead); do not reopen D-08.

## Code examples (load-bearing values)

```c
/* adapter av info: the only floating point in shipped code */
info->geometry.base_width = 320u;  info->geometry.base_height = 242u;
info->geometry.max_width  = 320u;  info->geometry.max_height  = 293u;
info->geometry.aspect_ratio = (float)(320.0 * 6.0 / (242.0 * 7.0)); /* 960/847 */
info->timing.fps = 39375000.0 / 656711.0;
info->timing.sample_rate = 48000.0;
```
The values come from HOSTS §1 (max 320×293) and D-08; the expression is derived.

```text
README install line (one line; version-free; seed check rewritten for it)
mkdir -p ~/Library/Application\ Support/RetroArch && curl -fsSL https://github.com/szTheory/muhtari/releases/latest/download/muhtari-libretro-macos-arm64.zip | tar -xf - -C ~/Library/Application\ Support/RetroArch cores info
```

## State of the Art
| Old (seed) | Current here | Impact |
|---|---|---|
| ubuntu-24.04, GCC 14, clang-format 18 | ubuntu-26.04, GCC 15 default, clang-format 21 | no `CC=gcc-14` override; format check pinned to 21 |
| PPM dumps | PNG (stored deflate) | sips decodes it; size 232,645 B |
| malloc default allocator | required allocator | consumer/embed/adapter pass a wrapper |
| versioned asset names + `extra-files` | version-free names + `configure_file` | README line never changes |
| `create-github-app-token` v3.2.0, `client-id: vars.*_APP_ID` | v3.3.0, `client-id: vars.MUHTARI_APP_CLIENT_ID` | — |

## Assumptions Log
| # | Claim | Section | Risk if wrong |
|---|---|---|---|
| A1 | MSVC `/W4 /WX` is silent on `uint32_t` vs non-negative enum constants | R-10 | header test fails on Windows; fix with casts in tests (header unchanged) |
| A2 | `/MT` runner and DLL import only `KERNEL32.dll` | R-10 | floor allowlist needs another DLL, or the D-15 claim changes |
| A3 | MSVC `/GS` emits `__security_check_cookie` in the core | R-05 | harmless: `/GS-` is set anyway |
| A4 | Same-repo `pull_request` runs can take `contents/id-token/attestations: write` under a read default | R-02 | the rehearsal falls back to post-merge dispatch with the release PR held (manual) |
| A5 | Hosted RetroArch starts a `supports_no_game` core with `-L` and no content | R-11 | the phase 1 hosted case needs another content source |
| A6 | `raise(SIGINT)` calls the handler under wasmtime's emulated signals | R-07 | one wasm32-only inventory difference |
| A7 | `vars` resolve in Dependabot runs | ENGINEERING open question | hard-code the (non-secret) client ID in `dependabot-automerge.yml` from the start |
| A8 | GNU `time` is on the hosted ubuntu image | bench | `apt-get install time` (recommended anyway) |
| A9 | Phase-3 `src/` size 5–7k lines | R-04 | budget pressure higher or lower |

## Open Questions
1. **Rehearsal form** (R-02): prerelease publish-and-delete (recommended) versus draft-only. The owner may want to record the DEC.49 wording change.
2. **Who applies the ruleset** (R-14): the agent with the owner's `gh` auth, or the owner with one command. Both satisfy rule 6; it is identity-bound.
3. **Palette scope:** seed only the warm NTSC table now (recommended: the only one used and tested), or all six PROVENANCE §4 tables. The rest can arrive with the `muhtari_palette` option in phase 3.

## Environment Availability
| Dependency | Needed by | Available here | Version | Fallback |
|---|---|---|---|---|
| CMake + Ninja | all | ✓ | 4.4.3 / Homebrew | — |
| Apple clang | Mac builds | ✓ | 21.0.0 | — |
| LLVM 21 (clang, clang-format, clang-tidy) | hygiene locally | ✓ (Homebrew llvm@21) | 21.1.8 | — |
| Docker (ubuntu:26.04, amd64 emulation) | GCC, nofp, be32, wasm32 local checks | ✓ | 29.5.2 | CI only |
| RetroArch 1.22.2 Metal | optional owner smoke | ✓ installed | 1.22.2 | hosted job |
| arm-none-eabi-gcc | AAPCS probe only | ✓ | 16.2.0 | — |
| MSVC | Windows legs | ✗ locally | — | CI only (R-10) |
| `gh` | settings, release checks | ✓ authenticated | 2.101.0 (`release verify`, `attestation verify --signer-workflow` present) | — |

## Sources
### Primary (HIGH)
- Local and container runs listed in F-01 to F-16 (2026-10-16).
- github/docs @eb8f32b5dd88 `supported-github-runners.md`; `data/reusables/actions/workflow-dispatch.md`; `content/code-security/concepts/supply-chain-security/immutable-releases.md`; `content/code-security/reference/supply-chain-security/dependabot-on-actions.md`.
- googleapis/release-please-action @45996ed1f6d0 `src/index.ts`; googleapis/release-please @v17.6.0 `src/scm.ts`, `src/manifest.ts`, `src/util/filter-commits.ts`, `src/strategies/base.ts`.
- bytecodealliance/wasmtime release v48.0.6 asset digests; libretro/RetroArch latest release; buildbot DMG headers.
- CMake 4.4.3 `cmake-presets(7)`; CMake `Modules/Platform/WASI*.cmake`.
- szTheory/nesturbator @3200aa868371 and @5533aeb7e6e0 (files named in the inventory); Actions runs 37084642541 and 38676328559.
- Project: CONTEXT.md, ENGINEERING.md (§1–§13), HOSTS-AND-INTEGRATION §1/§3/§4/§6, ARCHITECTURE §1/§4/§5/§11, A7800-MARIA §6/§7, CONFORMANCE §2/§6, GSD-HANDOFF §3/§7, DECISIONS, LESSONS XFER-026–033, PROVENANCE, ASSET_POLICY.
### Secondary (MEDIUM)
- a7800 `src/mame/drivers/a7800.cpp` @7a2afdc1ea08 (BSD-3 file in a GPL repository), read through a summarising fetch for two entries only (F-02).
### Tertiary (LOW)
- MSVC behaviour (A1–A3).

## Metadata
**Confidence:** stack HIGH (versions read from the toolchains); architecture HIGH (seeded and decided); pitfalls MEDIUM-HIGH (R-02, R-10, R-11 need the first hosted runs).
**Research date:** 2026-10-16. **Valid until:** 2026-11-15 (runner images and action versions move monthly; `ubuntu-latest` moves to 26.04 in Nov 2026).
