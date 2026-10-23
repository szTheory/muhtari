---
phase: 06-retail-cartridge-boards
plan: "06"
status: complete
subsystem: testing
tags: [activision, absolute, fractalus, diagnostic, mailbox, provenance]
requires:
  - phase: 06-10
    provides: Authenticated additive owned-fixture comparability
  - phase: 06-05
    provides: Executable retail maps and physical Fractalus RAM
provides:
  - Owned reproducible Activision, Absolute and Fractalus diagnostic fixtures
  - Independently specified 27, 21 and 24 numeric mailbox records
  - Nine complete fixture observations through library, actual runner and module
affects: [06-07, 06-08, 06-09]
tech-stack:
  added: []
  patterns:
    - Existing pinned dasm/sign/header pipeline with KEEP admission and exact rebuild
    - Test-owned expected values specified before first fixture execution
key-files:
  created:
    - tests/diag/activision.s
    - tests/diag/activision.cfg
    - tests/diag/activision.a78
    - tests/diag/absolute.s
    - tests/diag/absolute.cfg
    - tests/diag/absolute.a78
    - tests/diag/fractalus.s
    - tests/diag/fractalus.cfg
    - tests/diag/fractalus.a78
  modified:
    - tests/diag/expected.csv
    - tests/roms/manifest.txt
key-decisions:
  - Keep Activision helpers, selector code, font and vectors in fixed E000-FFFF.
  - Distinguish guide-derived map observations from selected compatibility and deterministic startup policies in source comments and expected-row evidence labels.
requirements-completed: []
requirements-contributed: [BOARD-01, BOARD-02, BOARD-03]
coverage:
  - id: activision-owned-diagnostic
    description: Fixed-window code reports both tagged halves of all eight banks, fixed windows, endpoints and hotspot neighbors in 27 records
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^diag[.](retail|rebuild)$' --no-tests=error --output-on-failure; exit 0; repeated after commit; URL N/A-local"
        status: pass
    human_judgment: false
  - id: absolute-owned-diagnostic
    description: Fixed upper ROM reports startup, D0 and D1 selection, precedence, high-bit independence, retained bank and fixed ROM in 21 records
    requirement: BOARD-02
    verification:
      - kind: integration
        ref: "cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^diag[.](retail|rebuild)$' --no-tests=error --output-on-failure; exit 0; library, runner and libretro observations; URL N/A-local"
        status: pass
    human_judgment: false
  - id: fractalus-owned-diagnostic
    description: Twenty-four records observe zero state, adjacent-page aliases, upper mirrors, distinct physical locations, multiple patterns and fixed ROM
    requirement: BOARD-03
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; exit 0; 612 non-release and 6 release tests including exact rebuild and nine fixture host observations; URL N/A-local"
        status: pass
    human_judgment: false
  - id: owned-admission-and-prior-predicates
    description: Exactly three owned fixtures match admitted digests, old manifest and expectation rows persist, and packages fit the unchanged bound
    requirement: BOARD-01
    verification:
      - kind: other
        ref: "Read-only manifest/expectation assertions plus verbose diag.retail observation-count assertion; exit 0; exactly 3 fixture identities and 9 host observations plus persistent smoke; URL N/A-local"
        status: pass
      - kind: other
        ref: "Exact native JUnit inventory 612/6, scripts/hygiene.sh --tree, git diff --check and three-ZIP sum 213796 <= 399937; exit 0 each; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 3294
  tasks: 3
  commits: 3
plan_head_before: 4dbe0376fcf99ed77a6032e35834fe893b7c9681
plan_head_after: bc1657953ea2ef2f2e20d39b0dc22aaa9cc38581
duration: 6min
completed: 2026-10-23
---

# Phase 6 Plan 6: Owned retail-board diagnostics Summary

**Three MIT-owned, exactly reproducible cartridge diagnostics report 72 independently specified numeric records through the library, real runner and actual libretro module.**

## Accomplishments

- Activision ROM ID 10 has exactly 131,072 ROM bytes plus its 128-byte A78 header. Each 8 KiB OM half is independently tagged. All helpers and executing code stay in fixed $E000-$FFFF, including the shared font and vectors. Its 27 records cover both switched halves of all eight banks, four fixed-window tags, endpoints, an extended hotspot and the two hotspot-range neighbors.
- Absolute ROM ID 11 has exactly 65,536 ROM bytes plus its header. Fixed upper-ROM code reports 21 observations of startup, D0/D1 choice, unusual-data precedence, retained bank, ignored high bits, fixed ROM and nonselector writes.
- Fractalus ROM ID 12 has exactly 32,768 ROM bytes plus its header. Its 24 observations cover initial zeros, adjacent-page pairs, mirrors above $4FFF, distinct physical blocks and bytes, a second write pattern and fixed ROM. The pinned header tool's existing `mram@4000` declaration emits Linear/A8; the actual runner probe reports Fractalus, ROM 32768 and physical RAM 2048.
- Each expected row was written from the documented map and fixture tags before that fixture's first execution. The source distinguishes uncertain compatibility edges and deterministic startup from hardware measurements. No retail ROM, copied emulator source, synthetic production hash entry or persistence data was added.
- The existing batch discovers every newly admitted named fixture without registration glue. Final verification explicitly observed exactly three named identities and nine complete numeric observations, alongside the persistent synthetic tracer's three observations. All previous manifest and expectation rows remain byte-identical. No old frame/audio expectation or pin changed.

## Task Commits

1. Task 1: `fc1b35a` — admit owned Activision bank diagnostic.
2. Task 2: `7875422` — admit owned Absolute selector diagnostic.
3. Task 3: `bc16579` — admit owned physical Fractalus alias diagnostic.

The persisted ledger measures three task commits. Actual tokens are the 13,174-character realized task diff divided by four, rounded up; binary fixture contents are not printed in the diff. Summary and state metadata are outside that interval.

## Commands, exits and run URLs

All runs were local; URL is N/A-local. No logs are committed.

| Command | Exit | Result |
|---|---:|---|
| `cmake -DWASMTIME=build/pins/wasmtime/wasmtime -DPKG=build/pins/7800basic -DSRC=tests/diag/<name>.s -DCFG=tests/diag/<name>.cfg -DOUT=build/ci/tests/diag/admit -DMANIFEST=tests/roms/manifest.txt -DKEEP=tests/diag/<name>.a78 -P tests/diag/assemble.cmake`, for each named fixture | 0 each | Existing pinned assemble/sign/header admission, exact digests below |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^diag[.](retail\|rebuild)$' --no-tests=error --output-on-failure` after Activision and Absolute admission | 0 each | Every admitted fixture on all three hosts and exact rebuild; Activision tracer repeated after commit |
| `cmake --workflow --preset ci` after Fractalus admission | 0 | 612/612 non-release tests, all three fresh packages, 6/6 release tests |
| Read-only Python assertions on the manifest and prior expected.csv prefix; captured `ctest --preset ci -R '^diag[.]retail$' --no-tests=error -V` | 0 | Exactly Activision/Absolute/Fractalus identities; one successful three-host summary each, plus smoke |
| `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` after each task | 0 each | Three fresh component ZIPs and six release checks each time |
| Exactly-three ZIP expansion and `wc -c` sum after each task | 0 each | 213,796 <= 399,937 every time |
| `ctest --preset ci --output-on-failure` after the final focused check | 0 | Fresh full 612/612 inventory evidence |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 612 tests ran once each |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly six release tests ran once each |
| `scripts/hygiene.sh --tree` and `git diff --check` | 0 each | Admitted bytes, source/license hygiene and whitespace pass; task hooks ran normally |

The read-only observation assertion parsed the verbose test output, requiring exactly four successful three-host summaries: `smoke`, `absolute`, `activision`, `fractalus`. It also required exactly the three named manifest paths and every old manifest row / expected.csv prefix from `4dbe037` unchanged. Numeric records remain judged by the existing strict mailbox parser and test-owned expected.csv, not by summary text or hashes.

| Fixture | File bytes | SHA-256 |
|---|---:|---|
| activision.a78 | 131,200 | 7656d3f24d717637aa2621dd761541ed715f414c0e8171dd49d28da44fe24b03 |
| absolute.a78 | 65,664 | 47b5dc6649fd7a19f50e6e01492c67c05dda1bd9ea933d5a738a33845567c123 |
| fractalus.a78 | 32,896 | f96d8279c2445e23e9a890f85f24272648958e12608a03384a93bc7e33cdc728 |

```sh
set -- build/ci/muhtari-*.zip
test "$#" -eq 3 || exit 1
archive_bytes=0
for archive do
  archive_size=$(wc -c < "$archive")
  printf '%s %s bytes\n' "$archive" "$archive_size"
  archive_bytes=$((archive_bytes + archive_size))
done
printf 'archive_bytes=%s ceiling=399937\n' "$archive_bytes"
test "$archive_bytes" -le 399937
```

| Component ZIP, macOS arm64, after every task | Bytes |
|---|---:|
| library | 68,754 |
| libretro | 67,756 |
| runner | 77,286 |
| Total | 213,796 |

## Deviations from Plan

None. The nine new source/config/fixture paths and two shared data files use existing assembly, admission, discovery and mailbox mechanisms; no framework or new test registration was needed.

## Issues Encountered

The final verbose focused CTest command replaced the preset's full-suite JUnit file with a one-test file. The first subsequent exact non-release inventory check correctly failed (exit 1). Regenerated the full 612-test file with the same preset and reran both exact inventory checks successfully. This was evidence-file scheduling, with no code or predicate change. Local verification output remains only under ignored build output.

## Next Readiness

Plan 07 can use these fixtures for identified AM/OM and achievement-map host equivalence. These observations prove the selected board semantics, not retail playability or physical cartridge measurements. BOARD requirements remain pending; six-native fit and hosted acceptance remain assigned to plan 09 and final phase gates. No known stub prevents this plan's goal and no new trust-boundary surface was introduced.

## Self-Check: PASSED

All nine new paths and both modified data files exist. All three task commits are ancestors of HEAD. All source rebuilds, nine numeric fixture host observations plus persistent smoke, full CI, release checks, exact native inventories and the unchanged package bound pass. Old fixture/expectation/hash predicates remain intact. Generated files stay under ignored build output, with no unexpected deletion.
