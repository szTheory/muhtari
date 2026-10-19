---
phase: 02-the-cpu-matches-the-public-vectors
plan: 08
subsystem: cart
tags: [cartdb, mame-softlist, cc0, sha1, fips-180-4, hygiene, codegen]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 7's archive layout (muhtari_objs and muhtari_cpu OBJECT libraries) and the hygiene lane's registration pattern
provides:
  - tools/cartdb.cmake, the CMake -P generator of src/cartdb.c from MAME hash/a7800.xml and hash/a2600.xml at dddf4cd1faee
  - src/cartdb.c (committed, generated) and src/cartdb.h, the OBJECT library muhtari_cartdb in the muhtari archive
  - muhtari__cartdb_7800_keys (214), muhtari__cartdb_7800_entries (204), muhtari__cartdb_7800_layouts (6), muhtari__cartdb_2600_prefix (1,601) and their four counts
  - policy.cartdb_generated and its self test in the hygiene inventory; the hygiene job fetches both XML pins
  - hygiene.no_float passes over an SPDX licence expression
  - src/sha1.c and src/sha1.h (muhtari__sha1_init, _update, _final) and cart.sha1.fips
affects: [the probe and identification plans that read the tables, the runner's --probe hashes, the release firmware scan that parses the row forms]

actuals:
  tokens: 35187
  tasks: 2
  commits: 2
plan_head_before: 9bab69fe9a66965ad310d182ac33292cc57ff277
plan_head_after: 30dc83f855016705a467fdcd3b372bed1d263ecd

tech-stack:
  added: []
  patterns:
    - "Generated data under every gate: the generator is CMake -P, the output is committed between clang-format off and on, and the hygiene lane regenerates and byte-compares it"
    - "Table rows have fixed forms a scan can read: a 7800 key row starts {{0x, a 2600 prefix row starts {0x, entries and layouts are written in decimal so neither form is ambiguous"

key-files:
  created:
    - tools/cartdb.cmake
    - src/cartdb.h
    - src/cartdb.c
    - src/sha1.h
    - src/sha1.c
    - tests/cart/tests.cmake
    - tests/cart/test_sha1.c
  modified:
    - pins.txt
    - CMakeLists.txt
    - THIRD-PARTY-NOTICES.md
    - libretro/muhtari_libretro.info.in
    - tests/hygiene/tests.cmake
    - tests/cmake/no_float.cmake
    - tests/inventory/hygiene.txt
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/tests.cmake
    - .github/workflows/ci.yml
    - docs/testing.md

key-decisions:
  - "A key's part is its chip's place among the entry's slices; a layout row carries the slice sizes and, per slice, the part it holds, so MAME's reload (Food Fight prototype: 16 KiB, 8 KiB, the same 8 KiB again) is the slice tuple {16384, 8192, 8192} with parts {0, 1, 1}"
  - "A continue chip (dkongxmcc: 128 KiB at $4000, then its last 16 KiB at 0) stays one keyed chip with no layout: its SHA-1 is of the whole file"
  - "Entry flags are bits: NTSC 1, PAL 2, light gun 4, not a cartridge 8, so the one PAL,NTSC entry keeps both; RAM is stored as KiB in 16 bits, which makes an entry 8 bytes and the tables 19,284 bytes"
  - "Slot values map to D-02's values: a78_sg9 is SuperGame with ROM at $4000, a78_p450_ta adds POKEY at $0450, a78_sg_ram keeps the list's own RAM size (8 or 16 KiB), and a78_hsc, a78_xboard and a78_xm are board 0 marked not a cartridge"
  - "The generated header keeps the commit on line 3: clang-format 21 reflows the one-line form of line 2 (137 columns), so line 2 names the generator and both lists and line 3 the commit and do not edit, as the research's code example has it"
  - "The hygiene job's timeout is 5 minutes, marked provisional as a network-bound floor, because it now fetches two files"

patterns-established:
  - "A check whose shell text passes through expect_fail.cmake holds no semicolon, since CMD is a CMake list"
  - "A core comment cites each section as 'section N.N'; 'sections 4.1.1, 4.2.1' and a citation broken across lines trip hygiene.no_float"

requirements-completed: []

coverage:
  - id: D1
    description: "tools/cartdb.cmake writes src/cartdb.c from the two pinned lists: 214 keys sorted by SHA-1 then entry, 204 entries, 6 layouts and 1,601 sorted 2600 prefixes, each count on its own line after its table"
    requirement: CPU-05
    verification:
      - kind: other
        ref: "cmake -DA7800=build/pins/mame/a7800.xml -DA2600=build/pins/mame/a2600.xml -DOUT=src/cartdb.c -P tools/cartdb.cmake (exit 0, 0.29 s)"
        status: pass
      - kind: other
        ref: "python3 -I scratch/xcheck.py over both XML files and src/cartdb.c with an independent ElementTree parse: keys, entries and prefixes equal (exit 0)"
        status: pass
    human_judgment: false
  - id: D2
    description: "policy.cartdb_generated regenerates the file in the hygiene lane and byte-compares it with src/cartdb.c; the self test changes one byte and requires 'src/cartdb.c differs from the generator output'"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "ctest --test-dir build/hygiene -R '^policy\\.cartdb_generated'"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39187517812 (hygiene job)"
        status: pass
    human_judgment: false
  - id: D3
    description: "hygiene.no_float removes an SPDX licence expression before the decimal check and still scans the rest of its line; the self test holds seven float lines beside three look-alikes"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/hygiene -R '^hygiene\\.no_float'"
        status: pass
      - kind: other
        ref: "two mutants of no_float.cmake (no SPDX removal, removal to end of line) report 8 and 6 lines on the fixture, so the self test's regex fails both"
        status: pass
    human_judgment: false
  - id: D4
    description: "src/cartdb.c line 1 is the MIT AND CC0-1.0 SPDX line, the file is ASCII, the notice section names it, and the .info licence is MIT, BSD-3-Clause, CC0-1.0"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^policy\\.notices$'"
        status: pass
      - kind: other
        ref: "head -1 src/cartdb.c; LC_ALL=C grep -n '[^ -~]' src/cartdb.c (exit 1, nothing printed)"
        status: pass
    human_judgment: false
  - id: D5
    description: "muhtari_cartdb joins the muhtari archive as 19,284 bytes of read-only data and the archive passes abi.undefined, abi.globals and abi.symbols"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^abi\\.(undefined|globals|symbols)$'"
        status: pass
      - kind: other
        ref: "size -m build/ci/CMakeFiles/muhtari_cartdb.dir/src/cartdb.c.o: __TEXT,__const 19284, __text 0"
        status: pass
    human_judgment: false
  - id: D6
    description: "Hygiene configure stops naming both fetch commands when either XML file is missing"
    requirement: CPU-05
    verification:
      - kind: other
        ref: "a2600.xml moved aside, cmake --preset hygiene -B <scratch> (exit 1, 'a2600.xml is missing; fetch both with'), file restored"
        status: pass
    human_judgment: false
  - id: D7
    description: "src/sha1.c from FIPS 180-4 passes the three example messages and one million 'a' in chunks of 1, 63, 64 and 65 bytes, on every hosted leg including be32"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cart\\.sha1\\.fips$'"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39187517812"
        status: pass
      - kind: other
        ref: "src/sha1.c mutated to count the length in bytes; muhtari-test-sha1 fails; source restored, cmp exit 0"
        status: pass
    human_judgment: false

duration: 13min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 8: The CC0 cartridge tables and SHA-1 Summary

**`tools/cartdb.cmake` turns MAME's CC0 `a7800.xml` and `a2600.xml` into a committed, gate-checked `src/cartdb.c`.
That is 19,284 bytes of const tables in the core: 214 full SHA-1 keys, 204 entries, 6 slice layouts and 1,601 2600
prefixes. The hygiene lane regenerates the file and byte-compares it. The core gains its own FIPS 180-4 SHA-1, which
passes the standard's vectors on every hosted leg.**

## Performance

- **Duration:** 13 min
- **Started:** 2026-10-19T01:44:09Z
- **Completed:** 2026-10-19T01:57:25Z
- **Tasks:** 2
- **Files modified:** 19

## Accomplishments

- **Tables.** Row counts:

  | Table | Rows | Bytes |
  |---|---|---|
  | `muhtari__cartdb_7800_keys` | 214 | 4,708 |
  | `muhtari__cartdb_7800_entries` | 204 | 1,632 |
  | `muhtari__cartdb_7800_layouts` | 6 (the 11 multi-chip entries) | 120 |
  | `muhtari__cartdb_2600_prefix` | 1,601 | 12,808 |
  | four counts | 4 | 16 |

  Measured: `__TEXT,__const` 19,284 bytes, `__text` 0 (`size -m` on the object). `nm -P` types every symbol `S`
  (read-only).
- **Generator.**
  - It runs in 0.29 s (`time`, local Apple Silicon).
  - It stops, naming the XML byte offset, on any of: an unknown slot, feature, shared feature, compatibility value
    or data area; a chip it does not handle; a multi-chip layout that does not cover its area in address order;
    more than 256 entries; a SHA-1 that is not 40 hex digits; or two 2600 SHA-1s that share an 8-byte prefix.
  - Only SHA-1s and numbers leave the XML.
- **Entries that are not cartridges.** `hiscore`, `xboard` and `xm` are board 0 with the not-a-cartridge flag. The
  HSC key appears twice (`hiscore` and `xm`). The lower entry sorts first, so it resolves to `hiscore`.
- **Regeneration check.**
  - `policy.cartdb_generated` writes into `build/hygiene/hygiene/cartdb/` and runs `cmake -E compare_files` against
    `src/cartdb.c`.
  - Its self test compares against a copy whose first `{{0x` is `{{1x`.
  - Hygiene configure stops when either XML file is missing and prints both `fetch-pin.sh` commands.
- **Float scan.** `no_float.cmake` removes `SPDX-License-Identifier: [A-Za-z0-9 .+()-]*`, so `CC0-1.0` passes and
  text after `*/` is still scanned. The fixture adds the two SPDX lines, and its regex now requires `sample.c:10:` and
  `7 lines`.
- **SHA-1.** `src/sha1.c` is 107 lines, written from FIPS 180-4 (section 4.1.1, section 4.2.1, section 5.1.1,
  section 5.3.1, section 6.1). It has no signed 64-bit type, and `abi.undefined` passes.

## Hosted evidence

- **Run on head `30dc83f`:** https://github.com/szTheory/muhtari/actions/runs/39187517812.
  - `gh run watch --exit-status` exited 0, the conclusion was `success`, and every job concluded success.
  - be32 took 48 s (01:55:22Z to 01:56:10Z). The be32 leg is what checks the big-endian word handling.
  - The hygiene job took 43 s, fetch step included (01:55:14Z to 01:55:57Z).

## Recorded one-off checks (AGENTS rule 6)

- **Local lanes:**
  - `scripts/fetch-pin.sh mame-a7800-xml build/pins/mame` and `scripts/fetch-pin.sh mame-a2600-xml build/pins/mame`:
    exit 0, digests verified.
  - `cmake --workflow --preset hygiene`: exit 0, 22 tests. `check_junit.cmake` reports 22 tests, each run once.
  - `cmake --workflow --preset ci`: exit 0, 368 tests.
  - `ctest -R '^(abi\.(undefined|globals|symbols)|policy\.notices)$'`: 4 passed.
- **Independent parse.** A scratch Python ElementTree parse of both lists gives keys, entries and prefixes equal to
  the generated tables (exit 0). The script is not in the tree.
- **Mutation runs:**
  - Two `no_float.cmake` mutants, run on the fixture, report 8 lines and 6 lines, so both fail the self test's regex.
  - A `sha1.c` mutant that counts the length in bytes fails `muhtari-test-sha1`. The source was restored and `cmp`
    exited 0.
- **Missing pin.** With `a2600.xml` moved aside, configure exits 1 with the fetch commands. The file was restored and
  its SHA-256 prefix `efe03008c6c2a4aa` re-checked.

## Task Commits

1. **Task 1: End-to-end tables, generator, archive, regeneration check** - `4a20587` (feat)
2. **Task 2: SHA-1 from FIPS 180-4 with its test vectors** - `30dc83f` (feat)

## Files Created/Modified

- `tools/cartdb.cmake`: the generator
- `src/cartdb.h`: key, entry and layout types, flag bits, extern tables and counts
- `src/cartdb.c`: generated tables, committed
- `src/sha1.h`, `src/sha1.c`: the core's SHA-1
- `CMakeLists.txt`: `muhtari_cartdb` in the archive, `sha1.c` in `muhtari_objs`, and the hygiene lane's XML check
- `pins.txt`: the `mame-a7800-xml` and `mame-a2600-xml` rows
- `THIRD-PARTY-NOTICES.md`: the "MAME software lists" section with the two courtesy lines
- `libretro/muhtari_libretro.info.in`: licence `MIT, BSD-3-Clause, CC0-1.0`
- `tests/hygiene/tests.cmake`: `policy.cartdb_generated(.self_test)` and the no_float fixture lines
- `tests/cmake/no_float.cmake`: the SPDX removal
- `tests/cart/tests.cmake`, `tests/cart/test_sha1.c`, `tests/tests.cmake`: `cart.sha1.fips`
- `tests/inventory/{hygiene,ci,wasm32}.txt`: the new names
- `.github/workflows/ci.yml`: the hygiene job's fetch step and its provisional timeout
- `docs/testing.md`: the hygiene lane's fetch, the `cart` label and the two self tests

## Decisions Made

See `key-decisions` above. In short:

- **Parts and layouts.** A key's part is its slice position. A layout gives each slice's part, so a reloaded chip
  appears twice.
- **Continue chips.** A continue chip is one key over the whole file.
- **Flags.** The TV-system and light-gun flags are bits.
- **RAM.** RAM is stored in KiB.
- **Header lines.** The generated header spans lines 2 and 3 because clang-format reflows the one-line form.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] The generated header's commit moved to line 3**
- **Found during:** Task 1
- **Issue:** C2's one-line second line is 137 columns. `clang-format --dry-run --Werror` (21.1.8) rejects it at
  column 82. The line sits before `/* clang-format off */`, and line 1 must stay the SPDX line.
- **Fix:** Line 2 names the generator and both XML files. Line 3, in the same comment, gives the commit and
  `do not edit`. This is the form in the research's "SPDX header of the generated file" example.
- **Files modified:** tools/cartdb.cmake, src/cartdb.c
- **Committed in:** 4a20587

**2. [Rule 1 - Bug] A semicolon in the check's message split the self test's command**
- **Found during:** Task 1
- **Issue:** `expect_fail.cmake` treats `CMD` as a CMake list. The message's `;` split the `sh -c` script, so `sh`
  printed its usage and the self test failed.
- **Fix:** The message uses a full stop instead.
- **Files modified:** tests/hygiene/tests.cmake
- **Committed in:** 4a20587

**3. [Rule 2 - Missing critical] Documentation for the new lane input and self tests**
- **Found during:** Task 1 and Task 2
- **Issue:** The hygiene lane now needs two fetched files. `docs/testing.md` did not say so, nor did it list the new
  self test or the `cart` label (AGENTS rule 5).
- **Fix:** It now has the hygiene row's fetch commands, two negative-control rows and the `cart` label row.
- **Files modified:** docs/testing.md (outside the plan's file list)
- **Committed in:** 4a20587, 30dc83f

**4. [Rule 3 - Blocking] The hygiene job's timeout**
- **Found during:** Task 1
- **Issue:** The 1-minute timeout was measured before the job fetched anything.
- **Fix:** It is now 5 minutes, marked `provisional` as a network-bound floor, the convention of the other
  network-bound jobs. The first run took 43 s.
- **Files modified:** .github/workflows/ci.yml
- **Committed in:** 4a20587

---

**Total deviations:** 4 auto-fixed (2 Rule 3, 1 Rule 1, 1 Rule 2). **Impact on plan:** none on scope.

## TDD Gate Compliance

Task 2 carries `tdd="true"`, and `workflow.tdd_mode` is false.

- **RED:** RED ran against a stub `sha1.c` whose init, update and final give a zero digest. `muhtari-test-sha1` was
  built and ran. `cart.sha1.fips` failed on all four planned assertions, for example `SHA-1("abc"): expected
  a9993e364706816aba3e25717850c26c9cd0d89d got 0000000000000000000000000000000000000000`. ctest exited 8.
- **Semantic assessment:** the target test ran and failed on the digest assertions it exists for. There was no link,
  load or fixture fault.
- **GREEN:** the FIPS 180-4 implementation passes.
- **Commits:** following plans 5 to 7, the test and the implementation landed together in one `feat(02-08)` commit,
  and there is no separate `test(02-08)` commit.

## Issues Encountered

- `hygiene.no_float` reads `sections 4.1.1, 4.2.1` and a `section` broken across two lines as decimals. The
  `sha1.c` comments cite each section as `section N.N` on one line.

## Known Stubs

None.

## User Setup Required

None.

## Next Phase Readiness

- The probe and identification plans can link `muhtari__cartdb_*` and `muhtari__sha1_*`.
- Two things have no consumer yet: each slice's part in a layout, and the not-a-cartridge flag. The identify step
  reads them.
- The firmware scan can read key rows by their `{{0x` start and prefix rows by their `{0x` start. It can check its
  parse against the four `_count` lines.
- The hygiene job's timeout is provisional until it is re-measured.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files present: tools/cartdb.cmake, src/cartdb.h, src/cartdb.c, src/sha1.h, src/sha1.c, tests/cart/tests.cmake and
  tests/cart/test_sha1.c.
- Commits 4a20587 and 30dc83f are ancestors of HEAD, and `git rev-list --count 9bab69f..HEAD` is 2.

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
