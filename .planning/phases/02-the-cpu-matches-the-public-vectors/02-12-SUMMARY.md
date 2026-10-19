---
phase: 02-the-cpu-matches-the-public-vectors
plan: 12
subsystem: runner
tags: [runner, probe, md5, sha1, sha256, crc32, rcheevos, exit-status, host]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 11's muhtari-test-images DIR and the seed images, and plans 9 and 10's muhtari_probe
provides:
  - muhtari-run --probe FILE, its key value lines, the eight hashes and the refusal statuses and messages
  - runner/md5.{c,h}, MD5 from RFC 1321 section 3
  - host/status_text.{c,h} in muhtari_host, the English status messages and value names
  - exit status 77 refused, and status 1 widened to unreadable, malformed, truncated and oversized images
  - runner.md5 and runner.probe.{images,accepted,2600,unsupported_board,malformed,truncated,oversized,usage,unreadable,headerless_rule}
affects: [plan 13's 7800basic sample through --probe, phase 3's libretro adapter messages, phase 3's --capabilities]

actuals:
  tokens: 12643
  tasks: 2
  commits: 3
plan_head_before: ced9ee92323bb34f13371d878b8b5552482a968b
plan_head_after: 84f9e55d1e65048f0530e8af3478da38816eed32

tech-stack:
  added: []
  patterns:
    - "English lives in host/: the core returns statuses and numbers, and host/status_text.c names them for the runner and the libretro adapter"
    - "A runner check compares the runner's digests with CMake's own file(SHA1|SHA256|MD5), an independent implementation"

key-files:
  created:
    - runner/md5.c
    - runner/md5.h
    - host/status_text.c
    - host/status_text.h
  modified:
    - CMakeLists.txt
    - runner/main.c
    - runner/exit_status.h
    - tests/runner/tests.cmake
    - tests/runner/test_runner.c
    - tests/cart/write_images.c
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/runner.md
    - docs/testing.md

key-decisions:
  - "muhtari__status_text gives \"unsupported board\" for that status, and the runner appends \": \" and muhtari__board_name, so the text table holds one string per status"
  - "Value names come from muhtari__kind_name, source, board, port and region, and muhtari__bit_name(set, bit) for the five bit sets; anything outside a table is \"unknown\""
  - "muhtari_host gets include/ on its public include path instead of linking muhtari, which put libmuhtari.a twice on every link line"
  - "--help prints the code column as %2d, so 77 aligns and the 0, 1 and 2 rows stay byte-identical"
  - "A FILE that starts with -- after --probe is a usage error, so --probe --help exits 2 rather than opening a file named --help"

patterns-established:
  - "tests/runner/tests.cmake's probe(name), expect_keys, probe_value and expect_hashes check a --probe run's status, line keys and digests"

requirements-completed: []

coverage:
  - id: D1
    description: "muhtari-run --probe FILE reads with fread until it returns 0 into one buffer of MUHTARI_IMAGE_SIZE_MAX + 1 bytes, with no seek, passes the byte count to muhtari_probe, and prints the identity fields, the raw A78 fields for a headered image, one warning line per bit, then the eight hashes, in the documented order and formats"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#probe.accepted (runner.probe.accepted: exit 0, every key in order, the identity and A78 lines pinned exactly)"
        status: pass
      - kind: other
        ref: "grep -n 'fseek\\|ftell' runner/main.c (exit 1, no line)"
        status: pass
      - kind: other
        ref: "cat build/ci/runner-probe/linear-v3-16k | build/ci/runner/muhtari-run --probe /dev/stdin prints the same lines"
        status: pass
    human_judgment: false
  - id: D2
    description: "SHA-1 through src/sha1.h, SHA-256 and CRC-32 from the runner, and MD5 from runner/md5.c; the file and headerless digests equal CMake's own for the accepted image and its header-stripped copy"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "tests/runner/test_runner.c#md5 (runner.md5: RFC 1321's seven test-suite messages, whole and one byte per update)"
        status: pass
      - kind: integration
        ref: "tests/runner/tests.cmake#probe.accepted (sha1, sha256, md5 and their _headerless forms against file(SHA1|SHA256|MD5))"
        status: pass
      - kind: other
        ref: "python3 zlib.crc32 of linear-v3-16k and of its bytes after 128: 43eb8af7 and 3d0ae8d4, the runner's crc32 and crc32_headerless"
        status: pass
    human_judgment: false
  - id: D3
    description: "The headerless hashes follow rcheevos @1433173220a7 src/rhash/hash_rom.c: a file of exactly 128 bytes holding only the magic hashes whole"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "tests/runner/tests.cmake#probe.headerless_rule (runner.probe.headerless_rule)"
        status: pass
      - kind: other
        ref: "mutant: n > 128u changed to n >= 128u; runner.probe.headerless_rule fails with sha1_headerless da39a3ee5e6b4b0d3255bfef95601890afd80709; main.c restored, cmp exit 0"
        status: pass
    human_judgment: false
  - id: D4
    description: "Each refusal has its own status, message from host/status_text.c and lines: 2600 exits 77 with kind, source, rom_size and the hashes; an unsupported board exits 77 with every line and unsupported board: souper; malformed and truncated images exit 1 with only the hashes; an oversized file exits 1 with nothing on standard output; a missing file exits 1 naming it; --probe with another option, no file or two files exits 2"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^runner\\.(probe\\.|exit\\.table)' --output-on-failure (11 of 11 passed; ctest -N -R '^runner\\.probe\\.' lists 10)"
        status: pass
      - kind: other
        ref: "grep -n '\"' src/cart.c | grep -i 'unsupported board\\|not a 7800' (no line: no English in the core)"
        status: pass
    human_judgment: false
  - id: D5
    description: "runner/exit_status.h has 77 refused and status 1's wider meaning; --help prints them and docs/runner.md repeats both lines verbatim and documents --probe, its order, formats, the headerless rule with its rcheevos citation and the refusals"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "tests/cmake/exit_table.cmake (runner.exit.table, 4 lines of --help found in docs/runner.md)"
        status: pass
    human_judgment: false
  - id: D6
    description: "The probe PR's head commit 84f9e55 is green on every hosted leg, both Windows legs and wasm32 included"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39192885762"
        status: pass
    human_judgment: false

duration: 9min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 12: The runner probes an image Summary

**`muhtari-run --probe FILE` prints what `muhtari_probe` says about an image as `key value` lines, followed by the
SHA-1, SHA-256, MD5 and CRC-32 of the file and of its headerless form. Each refusal class gets its own exit status,
message and set of lines, and status 77 `refused` is new. The English text lives in `host/`, and MD5 is our own code,
written from RFC 1321 section 3.**

## Performance

- **Duration:** 9 min
- **Started:** 2026-10-19T02:55:16Z
- **Completed:** 2026-10-19T03:04:21Z (CI watch finished afterwards)
- **Tasks:** 2
- **Files modified:** 14

## Accomplishments

- **`--probe FILE` (runner/main.c).**
  - It is accepted only as the whole command line. With another option, no file, two files, or `--` in the file's
    place, it exits 2 with the usage text.
  - The file is read in chunks with `fread` into one buffer of the cap plus one byte, so a pipe works.
  - Output order: the identity fields; then, for a headered image, `title` and the 13 `a78_*` lines; then one
    `warning` line per bit; then the eight hashes.
  - Formats: lowercase names, comma-separated bit names or `none`, `0x%02x` for raw fields (`0x%04x` for cart type and
    audio), and decimal sizes.
- **Hashes.** SHA-1 is the core's own, through `src/sha1.h` on the runner's private include path. SHA-256 and CRC-32
  are the runner's. MD5 comes from `runner/md5.c`: init, update and final like `sha256.h`, a table-driven round loop
  with the section 3.4 constants and word orders, and `runner.md5` on RFC 1321's test suite. The headerless input
  follows rcheevos's 7800 rule.
- **Refusals.**

  | Class | Exit | Standard output |
  |---|---|---|
  | 2600 | 77 | `kind`, `source`, `rom_size` and the hashes |
  | unsupported board | 77 | every line, with `unsupported board: <name>` on standard error |
  | malformed or truncated | 1 | the hashes only |
  | oversized | 1 | nothing |

  An unopenable or unreadable file exits 1 with `cannot open` or `cannot read`.
- **`host/status_text.{c,h}`** is in `muhtari_host`. It has `muhtari__status_text` with D-07's five messages and text
  for the other statuses, `muhtari__board_name`, the names for kind, source, port and region, and
  `muhtari__bit_name` for board options, sound, devices, flags and warnings.
- **Exit table.** `{77, "refused", ...}` is new, and status 1's meaning is widened. `--help` right-aligns the code,
  and docs/runner.md repeats the table and has a "Probing an image" section. docs/testing.md's `runner` label row
  names the new tests.
- **Tests.** `muhtari-test-images` also writes `linear-v3-16k-body` (the accepted image without its header) and
  `magic-only-128`. `runner.probe.images` writes them into `build/<preset>/runner-probe` as a fixture. The other nine
  `runner.probe.*` cases and `runner.md5` are in the ci and wasm32 inventories.

## Recorded one-off checks (AGENTS rule 6)

- `cmake --workflow --preset ci`: exit 0, 399 tests, then 4.
- `ctest --test-dir build/ci -R '^runner\.(probe\.|exit\.table)' --output-on-failure`: 11 of 11 passed.
  `ctest -N -R '^runner\.probe\.'` lists 10.
- `cmake --workflow --preset hygiene`: exit 0, 22 tests.
- A pipe works: `cat build/ci/runner-probe/linear-v3-16k | build/ci/runner/muhtari-run --probe /dev/stdin` prints the
  same lines. The oversized image through `/dev/stdin` gives the size message and nothing on standard output.
- CRC-32 cross-check with Python's `zlib.crc32`: `43eb8af7` and `3d0ae8d4`, the runner's `crc32` and
  `crc32_headerless` for `linear-v3-16k`.
- Mutant `n > 128u` changed to `n >= 128u`: `runner.probe.headerless_rule` fails, giving the SHA-1 of empty input.
  `main.c` was restored and checked with `cmp` (exit 0).
- **Hosted run on head `84f9e55`:** https://github.com/szTheory/muhtari/actions/runs/39192885762.
  - `gh run watch --exit-status` exited 0, and the conclusion was `success`.
  - All 18 jobs passed, including build on both Windows legs, wasm32, be32, asan, both nofp legs, hash-equality and
    retroarch.

## Task Commits

1. **Task 1: End-to-end probe of a written image** - `3dd0ed6` (feat)
2. **Task 2 RED: failing probe tests for each refusal class** - `116d5b5` (test)
3. **Task 2 GREEN: statuses, messages, lines, exit 77, docs** - `84f9e55` (feat)

## Files Created/Modified

- `runner/md5.{c,h}`: MD5 (new)
- `host/status_text.{c,h}`: messages and names (new)
- `runner/main.c`: `--probe`, the read loop, the line printers, `%2d` in `--help`
- `runner/exit_status.h`: 77 `refused`, status 1's wider meaning
- `CMakeLists.txt`: `status_text.c` in `muhtari_host` with `include/` public, `md5.c` and `src/` for `muhtari-run`
- `tests/runner/{tests.cmake,test_runner.c}`: the probe cases and helpers, the md5 case
- `tests/cart/write_images.c`: `linear-v3-16k-body` and `magic-only-128`
- `tests/inventory/{ci,wasm32}.txt`: 11 names
- `docs/runner.md`, `docs/testing.md`

## Decisions Made

See `key-decisions` above.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] The exit table's code column**
- **Found during:** Task 2
- **Issue:** `"  %d  %-7s"` printed `  77  refused`, one column out of line with the other rows.
- **Fix:** The format is `" %2d  %-7s"`. The 0, 1 and 2 rows are byte-identical, so the existing doc lines still
  match.
- **Committed in:** 84f9e55

**2. [Rule 3 - Blocking] muhtari_host and the public header**
- **Found during:** Task 1
- **Issue:** `status_text.h` includes `muhtari.h`, which `muhtari_host` could not find. Linking `muhtari` PUBLIC
  fixed that, but put `libmuhtari.a` twice on every link line, and ld warned.
- **Fix:** `include/` is on `muhtari_host`'s public include path, and every consumer links the library itself.
- **Committed in:** 3dd0ed6

**3. [Rule 2 - Missing critical] docs/testing.md**
- **Found during:** Task 2
- **Issue:** AGENTS rule 5 documents tests with behaviour, and the `runner` label row did not mention the probe tests.
- **Fix:** One row extended.
- **Committed in:** 84f9e55

---

**Total deviations:** 3 auto-fixed (1 Rule 1, 1 Rule 2, 1 Rule 3). **Impact on plan:** none on scope.

- `tests/fuzz/tests.cmake` is in Task 1's file list but needed no change. Its comment already says
  `muhtari-test-images` serves the runner tests.
- Acceptance check `grep -n '77' runner/exit_status.h` prints the enum line `MUHTARI__EXIT_REFUSED = 77`, not the row
  itself. The table rows use the enum names, as rows 0 to 2 do.

## TDD Gate Compliance

Task 2 carries `tdd="true"`, and `workflow.tdd_mode` is false.

- **RED (`116d5b5`).** Five tests failed on their planned assertions: `runner.probe.2600`, `unsupported_board`,
  `malformed`, `truncated` and `headerless_rule`. Each failed `expected exit 77/1 and keys ...`, because the tracer
  printed nothing on standard output for any refusal and exited 1. There was no build, load or fixture fault.
  - `oversized`, `usage` and `unreadable` passed already. The Task 1 tracer implemented strict `--probe` parsing and
    printed nothing for a refusal, which is what these three expect.
  - `runner.exit.table` cannot fail first, because it checks only that `--help`'s lines appear in the document. It
    failed after GREEN's table change, until docs/runner.md was updated.
- **Semantic assessment:** the target tests ran and failed on the exit status and line keys they exist to check.
- **GREEN (`84f9e55`):** all 11 pass, and so does the whole ci workflow (399 tests).
- The ctest console output is not a format the RED-evidence classifier reads. With `tdd_mode` off, the evidence is
  recorded here, as in plans 5 to 11.

## Issues Encountered

None.

## Known Stubs

None.

## User Setup Required

None.

## Next Phase Readiness

- Plan 13 can run `--probe` on the 7800basic sample. Its runner test can reuse `probe()`, `expect_keys` and
  `expect_hashes` in tests/runner/tests.cmake.
- Phase 3's libretro adapter can link `muhtari_host` for the same messages.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files present: runner/md5.c, runner/md5.h, host/status_text.c and host/status_text.h.
- Commits 3dd0ed6, 116d5b5 and 84f9e55 are ancestors of HEAD, and `git rev-list --count ced9ee9..HEAD` is 3.

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
