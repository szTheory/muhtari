---
phase: 02-the-cpu-matches-the-public-vectors
plan: 15
subsystem: release
tags: [firmware-scan, asset-policy, publish, rehearsal, cmake-script]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: src/cartdb.c (plan 3), tests/built/expected.txt (plan 5), the release label and package step (phase 1)
provides:
  - tests/cmake/firmware_scan.cmake, the zip-member and tracked-file firmware scan
  - tests/release/firmware.txt, five firmware SHA-1s
  - release.firmware_scan, release.firmware_scan.self_test, hygiene.firmware_scan, hygiene.firmware_scan.self_test
  - scripts/publish.sh prepare runs the scan before SHA256SUMS
affects: [phase 3 OpenBIOS build adds two firmware.txt rows, the phase 2 ship step]

actuals:
  tokens: 6930
  tasks: 2
  commits: 2
plan_head_before: 0b255d408f971f3a62182a433635d5a2d4096e8a
plan_head_after: 26e0083665d04941b31910b6aec27281aa204c8c

tech-stack:
  added: []
  patterns:
    - "file(DOWNLOAD file://... RANGE_START 128) copies a byte range with NULs in pure CMake, so the A78 header can be stripped before hashing"
    - "Each digest list becomes one variable per digest (fw_<hex>), so a lookup is a DEFINED test"

key-files:
  created:
    - tests/cmake/firmware_scan.cmake
    - tests/release/firmware.txt
  modified:
    - tests/release/tests.cmake
    - tests/hygiene/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/inventory/hygiene.txt
    - scripts/publish.sh
    - ASSET_POLICY.md
    - docs/testing.md
    - docs/maintaining.md

key-decisions:
  - "The scan makes its own temporary directory under RUNNER_TEMP, TMPDIR, TEMP or TMP and refuses one inside a zip's directory, so no caller passes a work path and prepare's dist/ is never written"
  - "The header-stripped hash uses file(DOWNLOAD) of a file:// URL with RANGE_START 128, the one CMake call that copies a byte range with NULs"
  - "A 2600 prefix is matched against the first 8 bytes of a SHA-1 only, never of a SHA-256"
  - "The firmware.txt comment says the OpenBIOS rows arrive with the OpenBIOS build in CI, without naming a phase (no planning IDs in code)"

patterns-established:
  - "A failure lists every match on its own indented line, so CMake does not wrap the message and a self test's regex sees it whole"

requirements-completed: [CPU-05]

coverage:
  - id: D1
    description: "firmware_scan.cmake hashes every zip member with SHA-1 and SHA-256, whole and without 128 bytes when ATARI7800 sits at offset 1, against firmware.txt, the expected.txt SHA-256s and the cartdb 7800 keys and 2600 prefixes, and names the member and the entry"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci (exit 0); ctest --test-dir build/ci -R '^release\\.firmware_scan' (2 of 2 passed)"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39196253809 (release.firmware_scan and its self test passed on all six build legs)"
        status: pass
    human_judgment: false
  - id: D2
    description: "The scan loads 214 7800 keys and 1601 2600 prefixes and fails naming parsed against declared on a row-form drift"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^release\\.firmware_scan$' -V (firmware_scan: 214 7800 keys, 1601 2600 prefixes loaded)"
        status: pass
      - kind: other
        ref: "cmake -DCARTDB=<cartdb.c with one row respaced> ... -P tests/cmake/firmware_scan.cmake (exit 1, cartdb.c: parsed 213 7800 keys, declared 214)"
        status: pass
    human_judgment: false
  - id: D3
    description: "An empty, truncated or garbage zip, or a glob that matches nothing, fails the scan"
    requirement: CPU-05
    verification:
      - kind: other
        ref: "cmake -DZIPS=<0-byte zip> ... -P tests/cmake/firmware_scan.cmake (exit 1, empty.zip yields no member)"
        status: pass
      - kind: other
        ref: "cmake -DZIPS=<garbage, truncated, unmatched glob> ... -P tests/cmake/firmware_scan.cmake (exit 1 each: cannot extract, cannot extract, no zip matches)"
        status: pass
    human_judgment: false
  - id: D4
    description: "hygiene.firmware_scan scans every tracked file, with a synthetic tracked A78 file as its negative control"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "cmake --workflow --preset hygiene (exit 0, 24 of 24 passed; firmware_scan: 319 tracked files, none listed)"
        status: pass
    human_judgment: false
  - id: D5
    description: "publish.sh prepare runs the scan over the 18 zips before SHA256SUMS and fails when a member is listed; the rehearsal ran it"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "policy.publish_set (passed; firmware_scan: 18 members of 18 zips, none listed)"
        status: pass
      - kind: other
        ref: "EXPECTED_SHA=fixture sh <copy with LICENSE's SHA-1 in firmware.txt>/scripts/publish.sh prepare build/hygiene/hygiene/publish_set/full (exit 1, 18 listed digests found)"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39196253836 (rehearsal success; prepare: firmware_scan: 96 members of 18 zips, none listed)"
        status: pass
    human_judgment: false

duration: 17min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 15: The release firmware scan Summary

**`tests/cmake/firmware_scan.cmake` hashes every member of every release zip, and every tracked file, whole and with
its A78 header stripped. It fails on any firmware SHA-1, built-sample SHA-256, MAME 7800 SHA-1 or 2600 SHA-1 prefix,
and on a zip with no member. Four tests and `publish.sh prepare` run it, and the rehearsal on PR #9 scanned 96 members
of the 18 zips and passed.**

## Performance

- **Duration:** 17 min
- **Started:** 2026-10-19T03:39:32Z
- **Completed:** 2026-10-19T03:56:10Z
- **Tasks:** 2
- **Files modified:** 12

## Accomplishments

- **The scan.** Arguments: `-DZIPS=<zip, glob or list>` or `-DTRACKED=1 -DROOT= [-DGIT=]`, plus `-DFIRMWARE`,
  `-DEXPECTED` and `-DCARTDB`.
  - firmware.txt rows are linted (`sha1` takes 40 hex, `sha256` 64). From expected.txt it takes every 64-hex field,
    named after the sample.
  - cartdb.c is read line by line: `{{0x..` rows inside `muhtari__cartdb_7800_keys` and `{0x..` rows inside
    `muhtari__cartdb_2600_prefix`, with the section set by each array's definition line. Each count must equal the
    declared count and be above zero.
  - Zips are extracted with `cmake -E tar xf` into a fresh directory under the system temporary directory, which is
    refused if it is inside a zip's directory, and removed on every exit path.
  - Every match is listed as `<zip>/<member>: matches <name>[ (header stripped)]` before the scan fails.
- **firmware.txt.** NTSC, PAL and Asteroids BIOS and the HSC ROM from ASSET_POLICY section 4. The Supercharger BIOS
  `cd9d030a…` comes from the pinned a2600.xml L16483 (fetched with `scripts/fetch-pin.sh mame-a2600-xml`). A comment
  says the two OpenBIOS rows arrive with the OpenBIOS build.
- **Tests.**
  - `release.firmware_scan` runs over `muhtari-*-<os>-<arch>.zip`.
  - `hygiene.firmware_scan` runs over tracked files.
  - Each has a self test. The release one zips a synthetic 256-byte A78 file; the hygiene one tracks the same file in
    a `git init` fixture repository. Each self test lists the SHA-1 of the file's last 128 bytes and expects
    `matches synthetic (header stripped)`.
  - All four are in their inventories, with provisional TIMEOUTs.
- **publish.sh prepare.** It runs `${CMAKE:-cmake}` over the 18 named zips after the commit.txt checks and before
  `SHA256SUMS`, with paths relative to `$(dirname "$0")/..`. `policy.publish_set` now writes 18 one-member zips (LICENSE)
  and passes `CMAKE=${CMAKE_COMMAND}`.
- **Docs.** ASSET_POLICY section 5 drops "(untested)" and names the four tests and prepare. docs/testing.md lists both
  self tests and the scan in the rehearsal row, and docs/maintaining.md lists it in prepare. The todo has moved to
  `done/`.

## Recorded runs (AGENTS rules 6 and 10)

| Run | Command | Exit | Notes |
|---|---|---|---|
| Local | `cmake --workflow --preset ci` | 0 | release label 6 of 6; `check_junit` with `-DLABELS=release`: 6 tests ran once each |
| Local | `cmake --workflow --preset hygiene` | 0 | 24 of 24; 319 tracked files, none listed |
| Local, edges | the scan on a 0-byte, a garbage and a truncated zip, an unmatched glob, and a cartdb.c with one row respaced | 1 each | `yields no member`, `cannot extract` (twice), `no zip matches`, `parsed 213 7800 keys, declared 214` |
| Local, negative | `publish.sh prepare` from a copy whose firmware.txt lists LICENSE's SHA-1 | 1 | 18 listed digests found; prepare fails |
| Hosted CI | https://github.com/szTheory/muhtari/actions/runs/39196253809 | 0 | every job succeeded; the two release scan tests passed on all six legs (12 passes) |
| Hosted rehearsal | https://github.com/szTheory/muhtari/actions/runs/39196253836 | 0 | rehearsal, install-line and cleanup succeeded. In the `publish.sh prepare dist` step: `214 7800 keys, 1601 2600 prefixes loaded`, then `96 members of 18 zips, none listed`, then `SHA256SUMS written`. Afterwards `gh release list` shows no rehearsal release and `git ls-remote --tags origin 'rehearsal-*'` prints nothing |

## Task Commits

1. **Task 1: the scan, firmware.txt, release.firmware_scan and its self test** - `8b5b151` (test)
2. **Task 2: publish.sh prepare, the publish_set fixture, hygiene.firmware_scan, ASSET_POLICY, docs, the todo** -
   `26e0083` (ci)

## Files Created/Modified

- `tests/cmake/firmware_scan.cmake`: the scan (new)
- `tests/release/firmware.txt`: five firmware SHA-1s (new)
- `tests/release/tests.cmake`, `tests/hygiene/tests.cmake`: the four tests; the one-member publish_set zips
- `tests/inventory/ci.txt`, `wasm32.txt`, `hygiene.txt`: two rows each
- `scripts/publish.sh`: prepare runs the scan; the header names it and `CMAKE`
- `ASSET_POLICY.md`, `docs/testing.md`, `docs/maintaining.md`
- `.planning/todos/done/2026-10-16-add-the-release-firmware-scan-with-the-first-rom-code.md` (moved from pending)

## Decisions Made

See `key-decisions` above.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] Stripping the header in pure CMake**
- **Found during:** Task 1
- **Issue:** CMake strings cannot hold NUL, so the scan cannot drop 128 bytes with `file(READ)` and `file(WRITE)`.
  A shell `tail` would break the CMake -P rule and the Windows legs.
- **Fix:** It copies the file to the work directory and uses `file(DOWNLOAD file://... RANGE_START 128)`, which needs
  CMake 3.24 (the project requires 3.25). Spaces and `%` are percent-encoded, and a drive path gets a leading `/`.
- **Verification:** both self tests match the header-stripped SHA-1 locally, and the release one passes on all six
  hosted legs, Windows included.
- **Committed in:** 8b5b151

**2. [Rule 3 - Blocking] No WORK argument**
- **Found during:** Task 1
- **Issue:** The plan asks for a temporary directory outside the zips' directory. The ctest zips sit at the top of
  the build directory, so a work path under the build tree would be inside it.
- **Fix:** The script makes a random directory under RUNNER_TEMP, TMPDIR, TEMP or TMP (falling back to /tmp), refuses
  one inside any zip's directory, and removes it on every exit path.
- **Committed in:** 8b5b151

**3. [Rule 2 - Missing critical] docs/testing.md and docs/maintaining.md**
- **Found during:** Task 2
- **Issue:** AGENTS rule 5 says docs land with each behaviour change, and no task listed these two files.
- **Fix:** Both self tests are in the negative-control table, and the scan is in the rehearsal row and in maintaining's
  prepare step.
- **Committed in:** 26e0083

---

**Total deviations:** 3 auto-fixed (2 Rule 3, 1 Rule 2). **Impact on plan:** none on scope.

- publish.sh passes the 18 names as an explicit list rather than a glob, since prepare has already proved the set.
- The firmware.txt comment leaves out "phase 3", following the rule against planning IDs in code.

## Issues Encountered

- The first self-test run failed only because CMake wrapped the FATAL_ERROR text inside `(header stripped)`. Each match
  now goes on its own indented line, which CMake prints as is.
- `file(COPY_FILE)` needs the destination directory to exist, so the publish_set fixture creates `full/` and `short/`
  first.

## Known Stubs

None.

## User Setup Required

None.

## Next Phase Readiness

- Plan 16 measures the provisional TIMEOUTs added here: 60 and 30 s on each scan test.
- The OpenBIOS build adds two `sha1` rows to `tests/release/firmware.txt`.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files present: tests/cmake/firmware_scan.cmake, tests/release/firmware.txt, and the done todo. The pending todo is
  gone.
- Commits 8b5b151 and 26e0083 are ancestors of HEAD, and `git rev-list --count 0b255d4..HEAD` is 2.
- Acceptance checks:
  - firmware.txt has five `sha1` rows and the OpenBIOS comment.
  - The scan has the `yields no member` and `declared` checks.
  - `firmware_scan.cmake` appears in publish.sh at L61, before the SHA256SUMS write at L62.
  - hygiene.txt has two `hygiene.firmware_scan` rows.
  - ASSET_POLICY.md has no `firmware_scan … untested`.
  - The rehearsal URL is recorded, with its prepare step green.

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
