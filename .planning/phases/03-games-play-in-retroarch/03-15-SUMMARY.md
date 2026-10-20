---
phase: 03-games-play-in-retroarch
plan: 15
subsystem: libretro
tags: [libretro, retroarch, retropad, core-options, bios, memory-map, rcheevos, achievements]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "muhtari_load, muhtari_power_cycle, muhtari_set_input and muhtari_get_memory (plans 03-01, 03-10, 03-11, 03-13); the input cartridge fixture (03-11); the built samples and OpenBIOS fixtures (03-08, 03-12); the diag/dma ledger row (03-14)"
provides:
  - "The libretro adapter loads content through muhtari_load, with ProSystem's RetroPad layout, four devices per port, Restart as a power cycle, optional BIOS files and four core options"
  - "SET_MEMORY_MAPS for console 51 in aligned power-of-two pieces, with pointer-less descriptors over unmapped spans, and SET_SUPPORT_ACHIEVEMENTS"
  - "libretro.host.{input,layout,devices,reset,bios,options,memory_map} and libretro.rcheevos on the ci inventory"
  - "pins.txt row rcheevos 12.1.0, fetched by build, asan and be32"
  - "run_retroarch.cmake -DROM=; the retroarch job runs tests/diag/dma.a78 and greps the diag/dma ledger hash"
  - "docs/libretro.md"
affects: [03-16, 03-17, 03-19, libretro core, retroarch job, rcheevos]

actuals:
  tokens: 23705
  tasks: 3
  commits: 5
plan_head_before: 690bbfeeae720e7128aad949c17bd62c187ee9dd
plan_head_after: eefbf1283d9d9f446e63946748dee06906c86660

tech-stack:
  added:
    - "rcheevos v12.1.0 (MIT), test-only, fetched by pin and never vendored"
  patterns:
    - "A libretro memory map is emitted as aligned power-of-two descriptors (select 0) with a pointer-less descriptor for every unmapped span, so rcheevos' resolver walks past holes"
    - "The adapter keeps every structure it hands the frontend (map, options, controller info, input descriptors) in static storage"
    - "A third-party test-only library builds with warnings and sanitizers off for its own target only"

key-files:
  created:
    - tests/libretro/rcheevos.c
    - docs/libretro.md
    - .planning/phases/03-games-play-in-retroarch/03-15-SUMMARY.md
  modified:
    - libretro/libretro.c
    - libretro/CMakeLists.txt
    - libretro/muhtari_libretro.info.in
    - pins.txt
    - tests/libretro/host.c
    - tests/libretro/tests.cmake
    - tests/cmake/info_claims.cmake
    - tests/retroarch/run_retroarch.cmake
    - tests/inventory/ci.txt
    - .github/workflows/ci.yml
    - docs/testing.md
    - README.md

key-decisions:
  - "The memory map covers the whole 64 KiB bus: D-25's spans in aligned power-of-two pieces (libretro.h L3634 requires a power-of-two len when select is 0), and pointer-less descriptors over registers, $3000-$3FFF, banked windows and the rest, because rcheevos fills a console region with nothing from the first address no descriptor covers, which would hide $0040 and $C000"
  - "$2800-$2FFF maps RAM+$800 although the core's bus leaves it to the cartridge; libretro.rcheevos compares it with RAM+$800 and every other mapped byte with muhtari__peek"
  - "muhtari_boot applies when content is loaded; Restart is a power cycle of the loaded media and reapplies only the difficulty options, since reloading would move cartridge RAM under the declared map"
  - "A BIOS file that muhtari_load refuses (ERR_INVALID_BIOS) is logged and the core falls back to fast boot rather than refusing the content"
  - "muhtari_palette has one value, NTSC (warm), the only palette the host helper has"
  - "SuperGame's fixed $4000 window (bank 0, bank 6 or the second-to-last) is mapped as CONST when the board has no RAM there; sg144 pins bank 0"
  - "rcheevos needs two rhash stubs, rc_hash_iterator_error and rc_path_compare_extension; they are generated into the build tree"

patterns-established:
  - "Each libretro host case drives the committed or generated cartridge through dlopen and reads the effect back through retro_get_memory_data"
  - "An .info firmwareN_opt = true claim maps to libretro.host.bios through info_claims.cmake"

requirements-completed: []

coverage:
  - id: D1
    description: "retro_load_game loads content through muhtari_load and a RetroPad press on either port reaches SWCHA as the cartridge reads it"
    requirement: GAME-05
    verification:
      - kind: integration
        ref: "tests/libretro/host.c#case_input (libretro.host.input)"
        status: pass
    human_judgment: false
  - id: D2
    description: "ProSystem's layout: B and A to buttons 1 and 2 on both ports, port 1 Select, Start and X to Select, Pause and Reset, L and R flipping the difficulty switches with an on-screen message, A firing on a Joystick"
    requirement: GAME-05
    verification:
      - kind: integration
        ref: "tests/libretro/host.c#case_layout (libretro.host.layout)"
        status: pass
    human_judgment: false
  - id: D3
    description: "Auto, ProLine, Joystick and None on both ports, None unplugging the port, and 17 input descriptors"
    requirement: GAME-05
    verification:
      - kind: integration
        ref: "tests/libretro/host.c#case_devices (libretro.host.devices)"
        status: pass
    human_judgment: false
  - id: D4
    description: "retro_reset power-cycles and gives the frames of a fresh load after play and a flipped switch"
    requirement: GAME-05
    verification:
      - kind: integration
        ref: "tests/libretro/host.c#case_reset (libretro.host.reset)"
        status: pass
    human_judgment: false
  - id: D5
    description: "muhtari_boot = bios boots through 7800 BIOS (U).rom or 7800 BIOS (E).rom from the system directory and logs the file's SHA-256; with no file it logs the fast-boot fallback and gives the fast boot's frames"
    requirement: GAME-05
    verification:
      - kind: integration
        ref: "tests/libretro/host.c#case_bios (libretro.host.bios)"
        status: pass
    human_judgment: false
  - id: D6
    description: "Four core options through SET_CORE_OPTIONS_V2, or v1 when the frontend has no v2; the difficulty options set the switches at load, at Restart and on a mid-game change"
    requirement: GAME-05
    verification:
      - kind: integration
        ref: "tests/libretro/host.c#case_options (libretro.host.options)"
        status: pass
    human_judgment: false
  - id: D7
    description: "SET_MEMORY_MAPS resolves every CPU address to the D-25 byte or to no memory for a linear, a SuperGame-with-RAM and a SuperGame 2^n + 1 cartridge, and SET_SUPPORT_ACHIEVEMENTS is true"
    requirement: GAME-05
    verification:
      - kind: integration
        ref: "tests/libretro/host.c#case_memory_map (libretro.host.memory_map)"
        status: pass
    human_judgment: false
  - id: D8
    description: "rcheevos v12.1.0's rc_libretro_memory_init(..., 51) over the core's real map for simple and doublebuffer: under 32 regions, total_size 65536, every resolved byte equal to muhtari__peek, $2800-$2FFF equal to RAM+$800, registers and $3000-$3FFF without memory, $0040 and $C000 reached"
    requirement: GAME-05
    verification:
      - kind: integration
        ref: "tests/libretro/rcheevos.c#check_cart (libretro.rcheevos)"
        status: pass
    human_judgment: false
  - id: D9
    description: "The .info declares memory_descriptors, input_descriptors, core_options, core_options_version 2.0, supported_extensions and two optional firmware entries, each true claim mapped to its test"
    requirement: GAME-05
    verification:
      - kind: other
        ref: "tests/cmake/info_claims.cmake (libretro.info_claims)"
        status: pass
    human_judgment: false
  - id: D10
    description: "Hosted RetroArch 1.22.2 boots tests/diag/dma.a78 with an empty system directory; its frame-120 screenshot equals the runner's frame pixel for pixel, the runner's hash equals the diag/dma ledger pin, and the no-content compare and its negative control still pass"
    requirement: GAME-05
    verification:
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39373524239 (retroarch job 118466988090)"
        status: pass
    human_judgment: false
  - id: D11
    description: "docs/libretro.md describes the controls, devices, options, BIOS names and fallback, Restart, and the achievement map with why $2800 is mapped and $3000 is not; the README and docs/testing.md point at it"
    requirement: GAME-05
    verification:
      - kind: other
        ref: "tests/cmake/budgets.cmake (docs.budgets); docs read against libretro/libretro.c by the agent"
        status: pass
    human_judgment: false

duration: 29min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 15: Games play in RetroArch (libretro adapter) Summary

**The libretro core now loads cartridges with ProSystem's RetroPad layout, four devices per port, optional BIOS files and four core options, and declares a whole-bus achievement map that rcheevos v12.1.0's own resolver reads correctly. Hosted RetroArch shows the dma diagnostic's frame 120 exactly as the runner hashes it.**

## Performance

- **Duration:** 29 min
- **Started:** 2026-10-20T04:45:56Z
- **Completed:** 2026-10-20T05:14:49Z
- **Tasks:** 3
- **Files modified:** 14

## Accomplishments

- `retro_load_game` copies the content and loads it through `muhtari_load`. A refused image is logged with its status text. The pixel format is set only after the content is accepted.
- ProSystem's RetroPad layout, with L and R flipping the difficulty switches and showing an on-screen message. Each port offers Auto, ProLine, Joystick and None. Restart is `muhtari_power_cycle`, and the difficulty options are applied again.
- Optional BIOS files `7800 BIOS (U).rom` and `7800 BIOS (E).rom` from the system directory. The log names the file and its SHA-256 (runner/sha256.c is compiled into the core); with no file, the core falls back to fast boot and says so.
- Core options v2, with a v1 fallback: `muhtari_boot`, `muhtari_palette`, `muhtari_left_difficulty` and `muhtari_right_difficulty`.
- An achievement map that rcheevos' resolver accepts: 11 regions for simple and 13 for doublebuffer, 39,424 bytes with memory in each case, every one equal to the core's own peek.
- The hosted `retroarch` job adds the dma diagnostic case and grows by exactly 15 lines (72 to 87, header comment included).

## Task Commits

1. **Task 1: Content load, port D-pad, in-process host reads it back (tracer)**: `63765ba` (feat)
2. **Task 2: Layout, devices, reset, BIOS, options, memory maps, .info**: `aa57920` (test, RED), then `048afa3` (feat, GREEN)
3. **Task 3: rcheevos resolver, RetroArch on dma.a78, docs/libretro.md**: `0570138` (feat), then `eefbf12` (fix, found on the first hosted run)

**Plan metadata:** the SUMMARY commit follows.

## TDD Gate Compliance (Task 2)

- **RED** `aa57920`: six new host cases. Every one failed on its planned assertion: no input descriptors (0 != 17), SWCHB staying $3F under Select, Start, X, L and R, A not firing on a Joystick, options never declared (option_set_with 0), BIOS name and SHA-256 missing from the log, no SET_SUPPORT_ACHIEVEMENTS and no memory map. There were no build, fixture or load faults. The cases already covered by Task 1 (B and A on both ports) passed, as expected. Semantic assessment: a valid RED. The failures were the absent behaviours, read from ctest's `--output-on-failure` (CTest output, which the classifier's adapters do not parse; `workflow.tdd_mode` is off).
- **GREEN** `048afa3`: all 16 libretro-label tests pass. The memory_map case changed in this commit, from an exact list of six or seven descriptors to a per-address resolution of the whole bus, with sg144 added. The BIOS case now compares frame 120, not frames 1 to 4, which are blank on both paths. Both changes are recorded below as deviations.
- Mutation checks, each run against the GREEN code: power_cycle removed fails reset; a $0140 offset or the sg144 bank changed fails memory_map; the joystick merge removed fails layout; the option-change filter removed fails options; a cartridge RAM offset of 1 fails libretro.rcheevos; the hole descriptors removed fail libretro.rcheevos ($0040, $01FF and $0480 unreached).

## Files Created/Modified

- `libretro/libretro.c`: the adapter (content, input, devices, options, BIOS, memory map, and a test-only instance accessor under `MUHTARI_LIBRETRO_TEST`)
- `libretro/CMakeLists.txt`: runner/sha256.c compiled into the core; `_CRT_SECURE_NO_WARNINGS` on MSVC
- `libretro/muhtari_libretro.info.in`: supported_extensions, memory_descriptors, input_descriptors, core_options and core_options_version, firmware_count 2 and its two entries, and the description
- `tests/libretro/host.c`: the input, layout, devices, reset, bios, options and memory_map cases
- `tests/libretro/rcheevos.c`: rcheevos' resolver over the real map
- `tests/libretro/tests.cmake`: seven host tests and libretro.rcheevos, the pin check, the rcheevos static library and the generated rhash stubs
- `tests/cmake/info_claims.cmake`: each new true claim mapped to its test
- `tests/retroarch/run_retroarch.cmake`: optional `-DROM=`
- `.github/workflows/ci.yml`: the rcheevos fetch in build, asan and be32; the dma case in the retroarch job, with a `retroarch-frames` artifact
- `pins.txt`: the `rcheevos 12.1.0` row
- `tests/inventory/ci.txt`: the eight new tests
- `docs/libretro.md` (new), `docs/testing.md`, `README.md`

## Decisions Made

See key-decisions in the frontmatter.

## Evidence

- **rcheevos archive re-check:** `gh api repos/RetroAchievements/rcheevos/git/ref/tags/v12.1.0` returned `6755915f2bdf6c11c83b3e0e68c4623a39c0dabe commit`. `shasum -a 256` of the downloaded archive was `c8bb9f66239f1b6c1f66417d26b341ce284ef2475acdc4f6fce0d04ac1678c55`, which equals the pin. `scripts/fetch-pin.sh rcheevos build/pins/rcheevos` exited 0. `git ls-files '*rc_libretro*'` prints nothing.
- **Local:** `cmake --workflow --preset ci` exited 0 (573 tests and 6 release tests). `cmake --workflow --preset hygiene` exited 0 (24 tests). `scripts/hygiene.sh --tree` exited 0. `ctest -N -L '^libretro$'` lists 9 `libretro.host.*` tests.
- **Local RetroArch 1.22.2:** `run_retroarch.cmake -DROM=tests/diag/dma.a78 -DFRAMES=120` exited 0. Its screenshot compared with `muhtari-run --frames 120 --dump-frame 120:...` gave `320x242, every pixel equal`, exit 0. RetroArch logged SET_CONTROLLER_INFO, SET_CORE_OPTIONS_V2, SET_INPUT_DESCRIPTORS, SET_MEMORY_MAPS and `SET_SUPPORT_ACHIEVEMENTS: yes`.
- **Hosted:** run https://github.com/szTheory/muhtari/actions/runs/39373524239 is green on every job: six build legs, asan, be32, nofp ×2, hygiene, hash-equality, wasm32, retroarch, perf, title and CI required. In the retroarch job the runner printed `frame 120 sha256 145799ed1868d43e10f18fdcd1529120274fe5c142957962760c02da6cb67955`; the grep matched the `diag/dma` ledger row; both compares printed `320x242, every pixel equal`; the negative control printed `first difference at 7,3: expected EFEFEF, actual EFEF10 (exit 1)`.
- **Screenshot:** the `retroarch-frames` artifact's `dma-shot.png` shows the dma diagnostic's on-screen hex records 01 to 1E, with values such as `00F1`, `00EF` and `00F5`, and the checksum line `EEBF`, white on black. It matches `dma-runner.png`.
- **Job growth:** the retroarch job, header comment included, went from 72 to 87 lines (+15, the limit).
- **be32:** the job took 2 m 43 s on the green run (its timeout comment cites 45 s). That is inside its 5-minute limit; plan 03-19, which re-measures the limits, should look at it.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] D-25's spans emitted as aligned power-of-two pieces with pointer-less hole descriptors**
- **Found during:** Task 2, from reading rcheevos' `rc_libretro_memory_init_from_memory_map` and libretro.h's descriptor docs (both MIT)
- **Issue:** A descriptor with select 0 must have a power-of-two length (libretro.h L3634), and $0040-$00FF ($C0 bytes) and $1800-$27FF ($1000 at a $800 boundary) do not qualify. rcheevos also registers the rest of a console region as no memory at the first address no descriptor covers. With $0000 unmapped, the page 0 and page 1 shadows and the RIOT RAM were out of its reach; with the banked $8000 window unmapped, so was a SuperGame's fixed ROM at $C000.
- **Fix:** `lr_span`/`lr_map` split each span into aligned power-of-two descriptors and fill every gap with a descriptor that has no pointer and no flags. The plan's "registers have no descriptor" becomes "registers have no memory". The host and rcheevos tests assert it that way.
- **Files modified:** libretro/libretro.c, tests/libretro/host.c
- **Verification:** libretro.rcheevos reaches $0040, $01FF, $0480 and $C000. With the hole descriptors removed, it fails on exactly those addresses.
- **Committed in:** 048afa3

**2. [Rule 1 - Bug] The memory map outlived SET_MEMORY_MAPS only by luck**
- **Found during:** Task 3, on the first hosted run (39372960254). libretro.rcheevos failed on the GCC legs and be32; memory_map and rcheevos failed on asan.
- **Issue:** `retro_memory_map` was a local in `lr_memory_maps`. RetroArch copies it straight away, but both tests keep the pointer, which dangled once `retro_load_game` returned. Reproduced in `gcc:14` under Docker: one region, no memory anywhere.
- **Fix:** the map is adapter state, cleared in `lr_release`.
- **Files modified:** libretro/libretro.c
- **Verification:** the gcc:14 reproduction gives 11 and 13 regions. A local ASan+UBSan build passes every libretro case. The hosted run is green on every leg.
- **Committed in:** eefbf12

**3. [Rule 3 - Blocking] rcheevos built without sanitizers; MSVC fopen and const warnings**
- **Found during:** Task 3, first hosted run
- **Issue:** UBSan reports `rc_libretro.c:469`, where rcheevos adds an offset to a null region pointer (third-party code). MSVC raised C4996 for `fopen` in the new code and C4090 for a `memset` over an array of const pointers. The project's existing `fopen` calls do not hit C4996.
- **Fix:** `-fno-sanitize=all` for the rcheevos target only. `_CRT_SECURE_NO_WARNINGS` on MSVC for the core, the host test and the rcheevos test. A loop replaces the `memset`.
- **Files modified:** tests/libretro/tests.cmake, libretro/CMakeLists.txt, tests/libretro/host.c
- **Committed in:** eefbf12

**4. [Rule 3 - Blocking] A second rhash stub**
- **Found during:** Task 3
- **Issue:** linking also needs `rc_path_compare_extension` (rhash/hash.c). The research had found only `rc_hash_iterator_error`; D-26 does say "a two-function stub".
- **Fix:** both are generated into `build/<preset>/tests/rc_hash_stub.c`.
- **Committed in:** 0570138

**5. [Rule 1 - Test fix] The BIOS case compares frame 120; memory_map resolves every address and adds sg144**
- **Found during:** Task 2, GREEN
- **Issue:** the fast and BIOS paths both show blank frames 1 to 4, so comparing those frames proved nothing. The exact descriptor list in the RED test no longer applied after deviation 1.
- **Fix:** the BIOS case compares frame 120 (the title against OpenBIOS's splash) and requires the fallback's 120 frames to equal the fast boot's. memory_map checks each descriptor's shape and resolves all 65,536 addresses against the D-25 spans for three boards.
- **Committed in:** 048afa3

---

**Total deviations:** 5 auto-fixed (3 Rule 1, 2 Rule 3).
**Impact on plan:** deviation 1 makes D-25's map work in rcheevos and RetroArch, which is what D-25 is for. Deviation 2 was a latent use-after-scope bug. The others are build and test fixes. Scope did not grow.

## Issues Encountered

- The first hosted run (39372960254) failed on six build legs, asan and be32, for the reasons in deviations 2 and 3. The second run (39373524239) is green.

## Known Stubs

None. `muhtari_palette` has a single value by design: the host helper has one palette.

## Threat Flags

| Flag | File | Description |
|------|------|-------------|
| threat_flag: file-read | libretro/libretro.c | The core reads at most 16,385 bytes from `<system directory>/7800 BIOS (U).rom` or `(E).rom`, only when `muhtari_boot` is bios; `muhtari_load` validates the bytes, and a refused image falls back to fast boot. |

## User Setup Required

None. No external service configuration is required.

## Next Phase Readiness

- Plan 03-16 can add boot-to-play on top of this adapter. The RetroArch job's `-DROM=` takes any committed ROM.
- Carry-forward: the adapter exposes no `RETRO_MEMORY_SAVE_RAM`. Cartridge RAM is in the achievement map, but no battery save exists until saves land (v2).
- The probe PR #12 is still a draft (head eefbf12); it was not marked ready and not merged.

## Self-Check: PASSED

- FOUND: tests/libretro/rcheevos.c, docs/libretro.md, libretro/libretro.c, tests/libretro/host.c
- FOUND commits: 63765ba, aa57920, 048afa3, 0570138, eefbf12 (each an ancestor of HEAD)
- Acceptance: `grep -c 'firmware_count = "2"'` printed 1; the BIOS names appear 3 times in libretro.c; SET_SUPPORT_ACHIEVEMENTS is present; 9 `libretro.host.*` tests; the pins row is present; no tracked `rc_libretro` file; `-DROM` is in run_retroarch.cmake; `diag/dma` is in ci.yml; `libretro.rcheevos` is in ci.txt
