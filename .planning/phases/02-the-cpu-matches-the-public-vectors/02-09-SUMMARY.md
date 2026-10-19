---
phase: 02-the-cpu-matches-the-public-vectors
plan: 09
subsystem: cart
tags: [a78, probe, api-v2, cartdb, sha1, footer-test, geometry]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 8's src/cartdb.{c,h} tables, src/sha1.{c,h} and the muhtari_cartdb OBJECT library
provides:
  - API version 2 in include/muhtari.h - muhtari_probe, muhtari_cart_info, statuses 5 to 9 and the kind, source, board, board-option, sound, device, port, region, cart-flag and warning enums
  - src/cart.c - muhtari_probe and muhtari__cart_identify, which takes the tables as arguments
  - MUHTARI__CART_INFO_SIZE_FIRST and the 28 layout asserts in src/internal.h; muhtari__write_out is extern
  - tests/api/v2.c and abi.api_v2 on the ci, wasm32 and nofp lanes
  - tests/cart/images.{c,h} (images built in memory), muhtari-test-cart linked from the core's objects, and the eight cart.probe.* tests
  - docs/hardware/cartridge.md, and the probe section of docs/embedding.md
affects: [the identification plan (slices, the Activision swap, the test list, HEADER_CORRECTED), the runner's --probe, the loader fuzz target, the internal loader]

actuals:
  tokens: 24279
  tasks: 3
  commits: 4
plan_head_before: 041bb5ecf79056fbe1593c12904959ce8dd196c9
plan_head_after: a678fdcb21626fd55dce5fa3a03cdf1c56f94f9a

tech-stack:
  added: []
  patterns:
    - "A core string constant is a static const byte array, never a literal: MSVC on ARM64 names a literal as an external COMDAT symbol, and abi.symbols rejects it"
    - "A test program that may swap a table links the core's OBJECT libraries, not the archive"

key-files:
  created:
    - src/cart.c
    - tests/api/v2.c
    - tests/cart/images.h
    - tests/cart/images.c
    - tests/cart/test_cart.c
    - docs/hardware/cartridge.md
  modified:
    - include/muhtari.h
    - src/internal.h
    - src/instance.c
    - CMakeLists.txt
    - tests/api/v1.c
    - tests/abi/tests.cmake
    - tests/core/test_core.c
    - tests/cart/tests.cmake
    - tests/header/header.c
    - tests/header/header.cpp
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/inventory/nofp.txt
    - docs/embedding.md
    - docs/testing.md

key-decisions:
  - "Geometry as implemented: linear bodies are a multiple of 4 KiB from 4 KiB to 48 KiB, or exactly 64 KiB; SuperGame bodies are multiples of 16 KiB from 32 KiB to 1 MiB; SG9 bodies are (2^n + 1) x 16 KiB up to 1 MiB (48, 80, 144, 272, 528 KiB). Every size in MAME's 7800 list fits"
  - "The v4 block replaces the cart type's board, RAM and sound only when it validates and its board fits the mapped body (a refused board needs no fit); otherwise the cart type applies with HEADER_V4_IGNORED. A v4 header with a zero block over a SuperGame body therefore loads as SuperGame"
  - "v4 device codes: A8 reads as the Fractalus board and M2 as halt-banked RAM, both refused, so the unverified readings can only refuse an image"
  - "Cart-type conflicts follow MAME's order with the POKEY-beside-ROM-at-$4000 exception; bits 3-5 without SuperGame set SuperGame; two special boards conflict and the last of Fractalus, Absolute, Activision, bankset, Souper wins"
  - "SIZE_FIELD_MISMATCH is set whenever the size field differs from the bytes after the header, so trailing bytes past the field also set it"
  - "RESERVED_NONZERO covers bytes 59-62 and 70-99, bytes 64-69 below version 4, and TV and save bits 3-7; a78_mapper and the other v4 fields read 0 below version 4"
  - "A headerless list match takes the entry's region (PAL only for a PAL-only entry) and puts a light gun on port 1 for a light-gun entry, ProLine on port 2"
  - "The cart_info size cases join core.api.size_tags rather than a new core case name, so no inventory line changed for them"

patterns-established:
  - "Each cart.probe case checks out byte for byte: a 0x5A-filled struct must come back unchanged for every error that writes nothing"

requirements-completed: []

coverage:
  - id: D1
    description: "API version 2 adds exactly muhtari_probe, muhtari_cart_info, statuses 5 to 9 and the new enums; ABI 1 and behaviour revision 1 stay; v1.c checks its version as a floor and v2.c names every new name and value"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^(abi\\.api_v[12]|abi\\.api_version|abi\\.symbols|docs\\.api_names|core\\.info)$'"
        status: pass
      - kind: integration
        ref: "SP=<scratch copy of the tree>; docker run gcc:15 ... cmake --workflow --preset nofp (exit 0, 12 abi tests)"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39189405561"
        status: pass
    human_judgment: false
  - id: D2
    description: "muhtari_cart_info is 140 bytes on every target, each field pinned by _Static_assert, and the probe honours size tags of first and first + 8 and rejects a non-zero tail"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^core\\.api\\.size_tags$'"
        status: pass
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cart\\.probe\\.arguments$'"
        status: pass
    human_judgment: false
  - id: D3
    description: "A clean linear A78 (v3 and v4), a SuperGame header and headerless images by their footer probe OK with every field, and two calls write the same 140 bytes"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cart\\.probe\\.accepted$'"
        status: pass
    human_judgment: false
  - id: D4
    description: "Argument, size-tag and size-cap checks come first; the cap probes OK and cap + 1 is ERR_UNSUPPORTED_SIZE with out untouched; NULL with n = 0 is ERR_INVALID_IMAGE"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cart\\.probe\\.(arguments|oversized)$'"
        status: pass
    human_judgment: false
  - id: D5
    description: "Every refusal class has its own status, and only OK, ERR_UNSUPPORTED_2600 and ERR_UNSUPPORTED_BOARD write out; a 2600 image writes only kind, source, rom_size and flags"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cart\\.probe\\.(2600|unsupported_board|malformed|truncated)$'"
        status: pass
      - kind: other
        ref: "three src/cart.c mutants (2600 list never hits; every status writes out; no trailing-space trim) fail probe.2600, probe.truncated and probe.malformed, and probe.warnings; source restored, cmp exit 0"
        status: pass
    human_judgment: false
  - id: D6
    description: "Each accepted header defect sets its own warning bit, a clean header and space-padded magic set none, and cart-type conflicts resolve in MAME's order"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cart\\.probe\\.warnings$'"
        status: pass
    human_judgment: false
  - id: D7
    description: "docs/embedding.md documents the probe, every new public name and the one write exception; docs/hardware/cartridge.md records the A78 fields, the warnings, the probe order, the footer test and the geometry table with the CC0 list's sizes"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^(docs\\.api_names|docs\\.budgets)$'"
        status: pass
      - kind: other
        ref: "grep -n '^## Geometry' docs/hardware/cartridge.md (line 142)"
        status: pass
    human_judgment: false
  - id: D8
    description: "No image bytes are tracked: every path git ls-files tests/cart prints ends in .c, .h or .cmake"
    requirement: CPU-05
    verification:
      - kind: other
        ref: "git ls-files tests/cart (images.c, images.h, test_cart.c, test_sha1.c, tests.cmake)"
        status: pass
    human_judgment: false

duration: 21min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 9: The probe, published as API version 2 Summary

**`muhtari_probe` is public as API version 2. It reads an A78 header (v3, or v4 when the block validates and fits),
looks the mapped body's SHA-1 up in the bundled CC0 7800 list, and identifies headerless images by the 7800 list,
the 2600 prefix list, the NTSC BIOS footer test and the 2600 sizes. Each refusal class has its own status, and each
header defect it accepts sets its own warning bit. Every case is built in memory, and the hosted run is green on
every leg.**

## Performance

- **Duration:** 21 min
- **Started:** 2026-10-19T02:00:21Z
- **Completed:** 2026-10-19T02:21:04Z
- **Tasks:** 3
- **Files modified:** 21

## Accomplishments

- **Public header.**
  - `MUHTARI_API_VERSION` is 2 and `MUHTARI_IMAGE_SIZE_MAX` is 1048704u.
  - Statuses 5 to 9 are appended with the `ERR_` prefix.
  - The kind, source, board, board-option, sound, device, port, region, cart-flag and warning enums are added; zero
    means absent in each.
  - `muhtari_cart_info` has one line per field, and `muhtari_probe` is declared.
  - Nothing else is added: no load, media, digest, database name or status string.
  - `src/internal.h` pins the 26 field offsets, `title` at 104 and `sizeof` 140.
- **Probe order.** Arguments, then the size tag, then the cap (before any hashing), then identification:
  1. The A78 magic. Truncated and zero-field checks come first. The header is parsed into v3 and v4 fields. The
     mapped size is min(field, n - 128), hashed once with SHA-1 and looked up in the 7800 list. A match that is not
     a cartridge is invalid; any other match overrides the board, RAM and sound. Refused boards are checked next,
     then the geometry.
  2. Headerless images: the 7800 list (whole image), the 2600 prefix list, the footer test, the 2600 sizes, and
     finally invalid.
- **Geometry as implemented.**

  | Board | Bodies |
  |---|---|
  | Linear | multiples of 4 KiB from 4 to 48 KiB, or exactly 64 KiB |
  | SuperGame | multiples of 16 KiB from 32 KiB to 1 MiB |
  | SG9 | (2^n + 1) x 16 KiB up to 1 MiB |

  The CC0 list's sizes per slot are recorded in `docs/hardware/cartridge.md`:
  - linear: 16, 32, 48 and 64 KiB (27, 57, 24 and 11 entries);
  - linear with POKEY: 16 and 32 KiB (1 and 4);
  - SuperGame: 64 and 128 KiB (5 and 41);
  - SuperGame with RAM: 64 and 128 KiB (2 and 10);
  - SG9: 144 KiB (13).
- **Tests.** `cart.probe.accepted`, `.arguments`, `.oversized`, `.2600`, `.unsupported_board`, `.malformed`,
  `.truncated` and `.warnings` (labels `cart;portable`). `abi.api_v2` (labels `abi;portable`) runs on the ci, wasm32
  and nofp lanes. The `cart_info` size cases run in `core.api.size_tags`.
  - The 2600 list, a 7800 list hit, a not-a-cartridge entry and a refused entry go through
    `muhtari__cart_identify`, using a one-entry list built from the image's own SHA-1.

## Hosted evidence

- **Run on head `a678fdc`:** https://github.com/szTheory/muhtari/actions/runs/39189405561.
  - `gh run watch --exit-status` exited 0, and the conclusion and every job were `success`.
  - be32 took 41 s (02:19:24Z to 02:20:05Z).
- **The first push failed on one leg:** https://github.com/szTheory/muhtari/actions/runs/39189208610 (head `ab608ac`).
  - The build (windows-arm64) leg failed `abi.symbols` with `outside the muhtari_ namespace: ??_C@_09OGCKONJP@ATARI7800@`.
  - Fixed in `a678fdc` (deviation 2).

## Recorded one-off checks (AGENTS rule 6)

- `cmake --workflow --preset ci`: exit 0, 377 tests.
- `cmake --workflow --preset hygiene`: exit 0, 22 tests. That is the format, analyzer and float scans over the new
  code.
- `cmake --workflow --preset nofp` in a `gcc:15` container (aarch64): exit 0, 12 abi tests. The container used a
  scratch copy of the tree.
  - The first attempt used `ubuntu:24.04`, whose GCC 13 lacks `-Wmissing-variable-declarations`. That failure was
    about the toolchain, not the code.
- **Mutation runs on `src/cart.c`**, each restored and checked with `cmp` (exit 0):
  - The 2600 list never hits: `probe.2600` exits 1.
  - Every status writes `out`: `probe.truncated` and `probe.malformed` exit 1.
  - No trailing-space trim: `probe.warnings` exits 1.

## Task Commits

1. **Task 1: End-to-end probe of a clean linear A78** - `e3a7d69` (feat)
2. **Task 2: Every refusal class, the argument and size edges, the headerless path** - `a59dd1f` (feat)
3. **Task 3: Warnings, the size-tag cases, cartridge.md** - `ab608ac` (feat)
4. **Fix: the A78 magic as a byte array** - `a678fdc` (fix)

## Files Created/Modified

- `include/muhtari.h`: API version 2
- `src/internal.h`: `MUHTARI__CART_INFO_SIZE_FIRST`, the layout asserts, `muhtari__write_out` and `muhtari__cart_identify`
- `src/instance.c`: `write_out` renamed to `muhtari__write_out` and made extern
- `src/cart.c`: `muhtari_probe` and identification
- `CMakeLists.txt`: `src/cart.c` in `muhtari_objs`
- `tests/api/v1.c`: the version floor; `tests/api/v2.c`: new
- `tests/abi/tests.cmake`: `abi.api_v2`, and the self test's regex now reads `K <= 2`
- `tests/header/header.c`, `tests/header/header.cpp`: API version 2, the `cart_info` size tag first, and the probe's signature
- `tests/core/test_core.c`: `v.api` 2, `union tagged` gains `cart`, and the `cart_info` size cases
- `tests/cart/images.{c,h}`, `tests/cart/test_cart.c`, `tests/cart/tests.cmake`: the builders, the program and the eight tests
- `tests/inventory/{ci,wasm32,nofp}.txt`: the new names
- `docs/embedding.md`: "Probing a cartridge image"; `docs/hardware/cartridge.md`: new; `docs/testing.md`: the `cart` label row

## Decisions Made

See `key-decisions` above. In short:

- **Geometry.** The table above.
- **The v4 block.** It wins only when it validates and its board fits the body.
- **v4 device codes.** The A8 and M2 readings both refuse.
- **Conflicts.** Resolved in MAME's order.
- **Size field.** Any difference from the bytes present sets `SIZE_FIELD_MISMATCH`.
- **Reserved bytes.** The v4 block counts as reserved below version 4.
- **Headerless list match.** The region and ports come from the entry.
- **Size cases.** They run inside `core.api.size_tags`.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] The header compile tests pinned API version 1**
- **Found during:** Task 1
- **Issue:** `tests/header/header.c` and `header.cpp` assert `MUHTARI_API_VERSION == 1`, so `abi.header.c17` and
  `abi.header.cxx17` failed with the bump. The plan's file list does not name them.
- **Fix:** Both files now assert 2. They also check that the `cart_info` size tag is first, and they pin
  `muhtari_probe`'s signature through a function pointer.
- **Files modified:** tests/header/header.c, tests/header/header.cpp
- **Committed in:** e3a7d69

**2. [Rule 1 - Bug] MSVC on ARM64 exported the magic literal**
- **Found during:** Task 3's hosted run 39189208610
- **Issue:** `memcmp(..., "ATARI7800", 9)` gave the literal the external COMDAT name `??_C@_09OGCKONJP@ATARI7800@`.
  `abi.symbols` failed on build (windows-arm64) only.
- **Fix:** The magic is now `static const uint8_t cart_magic[9]`, with a comment that says why. No other core source
  has a string literal.
- **Files modified:** src/cart.c
- **Verification:** run 39189405561 is green on every leg.
- **Committed in:** a678fdc

**3. [Rule 2 - Missing critical] The testing guide's cart label**
- **Found during:** Task 3
- **Issue:** `docs/testing.md` described the `cart` label as SHA-1 only (AGENTS rule 5).
- **Fix:** The row now names the probe tests and the in-memory images.
- **Files modified:** docs/testing.md
- **Committed in:** ab608ac

**4. [Rule 3 - Blocking] The api_version self test's expected text**
- **Found during:** Task 1
- **Issue:** The self test's regex reads `K <= 1`, and the script now prints `K <= 2`.
- **Fix:** The regex reads `K <= 2`.
- **Files modified:** tests/abi/tests.cmake
- **Committed in:** e3a7d69

---

**Total deviations:** 4 auto-fixed (2 Rule 3, 1 Rule 1, 1 Rule 2). **Impact on plan:** none on scope.

## TDD Gate Compliance

Tasks 2 and 3 carry `tdd="true"`, and `workflow.tdd_mode` is false.

- **Task 2 RED.**
  - The tests were built against Task 1's `cart.c`, which answered `ERR_INVALID_IMAGE` for every headerless image.
  - `cart.probe.accepted`, `cart.probe.2600` and `cart.probe.unsupported_board` failed on their planned assertions,
    for example `2600 size: expected status 8, got 5` and `footer: expected status 0, got 5`.
  - `cart.probe.malformed`, `cart.probe.truncated`, `cart.probe.arguments` and `cart.probe.oversized` passed at RED,
    because Task 1's header path already gave those statuses. The mutants above show these tests fail when the
    write rule breaks.
- **Task 3 RED.**
  - `cart.probe.warnings` failed on each unimplemented bit, for example `invalid v4 block: expected warnings 0x1,
    got 0x0` and `version 5: expected warnings 0x4, got 0x0`.
  - The probe's size-tag cases in `core.api.size_tags` passed at RED: they exercise `muhtari__check_size` and
    `muhtari__write_out` from Task 1.
- **Semantic assessment:** each failing target ran and failed on the status or warning it exists for. There was no
  build, load or fixture fault.
- **GREEN:** all 377 tests pass.
- **Commits:** following plans 5 to 8, each task's tests and implementation landed in one `feat(02-09)` commit, and
  there is no separate `test(02-09)` commit.

## Issues Encountered

- GCC 13 (ubuntu:24.04) cannot build the nofp lane locally because it lacks `-Wmissing-variable-declarations`.
  GCC 15 and the hosted lanes can.

## Known Stubs

- `src/cart.c`, `muhtari__cart_identify`: the `layouts` and `layout_count` arguments are not read yet.
  - Not yet built: the hash of each slice of a multi-chip layout, and the half-swapped hash of a 128 KiB image.
  - Until then, a multi-chip dump is identified by its header or its footer.
  - The identification plan that follows adds them, with the test list and the `HEADER_CORRECTED` case.
  - Recorded in `.planning/WINDOWS.md` as entry 1.

## User Setup Required

None.

## Next Phase Readiness

- The identification plan can pass `tests/cart/test_cartdb.c` to `muhtari__cart_identify`. It can substitute
  `muhtari_cartdb` in `muhtari-test-cart`'s object list.
- The loader can reuse `cart_header`'s result and the same statuses.
- The fuzz target can call `muhtari_probe` twice and compare the results.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files present: src/cart.c, tests/api/v2.c, tests/cart/images.c, tests/cart/images.h, tests/cart/test_cart.c and
  docs/hardware/cartridge.md.
- Commits e3a7d69, a59dd1f, ab608ac and a678fdc are ancestors of HEAD, and `git rev-list --count 041bb5e..HEAD` is 4.

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
