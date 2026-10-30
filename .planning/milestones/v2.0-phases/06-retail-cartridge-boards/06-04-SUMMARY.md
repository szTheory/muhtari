---
phase: 06-retail-cartridge-boards
plan: "04"
status: complete
subsystem: cartridge
tags: [activision, normalization, identity, runner]
requires:
  - phase: 06-03
    provides: API4 orientation and assumption bits with linked smoke coverage
provides:
  - Identified-only private AM to OM normalization with raw caller identity preserved
  - Exact Activision geometry and six-window mapper with bounded address-only selectors
  - Real CPU and runner probe evidence with numeric values and fault controls
affects: [06-05, 06-06, 06-07, 06-08, 06-09]
tech-stack:
  added: []
  patterns:
    - Existing instance latch and board switch with copy-time normalization
    - Independent half tags and real probe identities through isolated test tables
key-files:
  created: []
  modified:
    - src/cart.c
    - runner/main.c
    - tests/cart/test_cart.c
    - tests/cart/test_cartdb.c
    - tests/cart/images.c
    - tests/cart/images.h
    - tests/cart/write_images.c
    - tests/runner/tests.cmake
    - docs/hardware/cartridge.md
    - docs/runner.md
key-decisions:
  - Preserve supplied raw identity and normalize only identified half-swapped Activision copies.
  - Keep existing runner probe lines stable and append a flag line only for identified half-swapped input.
requirements-completed: []
requirements-contributed: [BOARD-01]
coverage:
  - id: activision-map-and-copy
    description: Identified AM and OM install identical private OM bytes and execute numeric bank observations with immutable caller bytes
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "cmake --build --preset ci && ctest --preset ci -R '^cart[.](identify[.]activision_swap|probe[.]|load[.])' --no-tests=error --output-on-failure; exit 0; 10/10; URL N/A-local"
        status: pass
    human_judgment: false
  - id: original-probe-identity
    description: Actual testdb runner reports original AM and OM hashes, identified orientation, raw header fields, trailers and explicit assumed-layout warning
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "ctest --preset ci -R '^runner[.]probe[.]sample_hash_match$' --no-tests=error --output-on-failure; exit 0; 3/3 including fixtures and pass/fault controls; URL N/A-local"
        status: pass
    human_judgment: false
  - id: retained-contracts
    description: API4 and ABI1 remain unchanged, old frame/audio expectations persist and exactly three local packages retain the archive bound
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; exit 0; 611 non-release plus 6 release tests; both check_junit invocations exit 0; URL N/A-local"
        status: pass
      - kind: other
        ref: "Exactly-three ZIP expansion and wc -c sum; exit 0; 213443 <= 399937; scripts/hygiene.sh --tree and git diff --check exit 0; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 10473
  tasks: 2
  commits: 2
plan_head_before: 8de594abfdc0750b4f0f055ae15e15bdaa714e71
plan_head_after: 5459c4105b061e65dc39a8327959b06d58029f03
duration: 9min
started: 2026-10-23T19:47:10Z
completed: 2026-10-23
---

# Phase 6 Plan 4: Identified Activision mapping Summary

Identified AM and OM inputs now run through one OM mapper, retain their original raw identities, and produce independent bank-zero $31 and bank-five $3A CPU observations.

## Task commits

| Task | Commit | Outcome |
|---|---|---|
| 1 | `2a588bb` | Exact 128 KiB acceptance, identified-only copy normalization, six OM windows, selector policies, independent mapping/CPU/copy/failure vectors and hardware docs |
| 2 | `5459c41` | Actual testdb runner probes, raw identity/trailer/header assertions, report fault controls and runner docs |

Actual tokens are the 41,889-character realized task diff divided by four and rounded up. The persisted ledger measures the two production task commits; metadata commits are outside that interval.

## Acceptance and evidence

All evidence is local, with run URL N/A-local; no logs are committed.

| Command | Exit | Result |
|---|---:|---|
| First focused cartridge CTest command below | 8 | Two existing warning vectors still expected an unsupported board; corrected to accepted Activision and the explicit assumption warning |
| `cmake --build --preset ci && ctest --preset ci -R '^cart[.](identify[.]activision_swap\|probe[.]\|load[.])' --no-tests=error --output-on-failure` | 0 | 10/10, including new map/normalization/CPU and transactional checks |
| Post-commit repeat of the same focused CTest | 0 | Tracer feedback passed, 10/10 |
| `ctest --preset ci -R '^runner[.]probe[.]sample_hash_match$' --no-tests=error --output-on-failure` | 0 | 3/3 including image/sample fixtures; real probe pass/fault controls execute inside the existing case |
| `cmake --workflow --preset ci` | 0 | 611/611 non-release checks, three fresh ZIPs and 6/6 release checks |
| `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` | 0 | All three packages rebuilt and six release checks passed |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 611 tests ran once each |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly six release tests ran once each |
| `scripts/hygiene.sh --tree` and `git diff --check` | 0 each | Repository hygiene and whitespace pass |
| `git diff 8de594a..5459c41 -- tests/built/expected.txt tests/audio/expected.txt` | 0 | Empty: prior frame/audio hashes and revision metadata unchanged |
| Exactly-three ZIP expansion and sum below | 0 | 213,443 <= 399,937 |

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

| Component ZIP, macOS arm64 | Bytes |
|---|---:|
| library | 68,629 |
| libretro | 67,657 |
| runner | 77,157 |
| Total | 213,443 |

The existing cartridge case checks both ends of all six windows for every bank through CPU, MARIA and peek; all 256 data values at all 16 selectors; passive reads and selector neighbors; ROM immutability; bank-zero power cycle; repeated loads; caller-byte preservation; allocation refusal retaining the deep instance and canonical ROM; invalid and truncated geometry; v4/hash precedence and declared POKEY overlaps. The actual mapped program reads $A000 at bank zero, writes $FF8D with data zero, reads $C000 at bank five, and stores the independent values at $2200/$2201.

The OM test key `e8cc27cc77bffe9c9af22683cde9fac69e03e82d` was independently computed with Python's hashlib from the sixteen $30-$3F 8 KiB half tags, the specified 6502 instruction bytes and reset/footer fields. The production-key absence control remains registered and passes. No production key or committed ROM was added. Generated raw probes are confined to the ignored build tree.

## Deviations from plan

1. **[Rule 2 - Missing observation surface]** `print_identity` emitted no cartridge flag lines. Added `flag activision_half_swapped` only for that new input-orientation bit, using the already accepted host bit-name table; existing outputs remain identical for other images. This makes the planned real probe acceptance possible.
2. **[Rule 2 - Shared synthetic observation]** Added the owned OM builder to existing `images.c`/`images.h` and runtime writer. Both library and actual probe paths use the same disposable bytes, with expected bank tags independently specified by tests and the test-only key independently calculated. Existing registrations/inventories are reused.
3. **[Rule 1 - Test integration]** Existing Activision warning/refusal vectors were advanced to the now-executable contract. The first focused run exposed them; the rerun and full workflow pass. WINDOWS37 records the issue as fixed.
4. **[Rule 1 - Documentation consistency]** Corrected the Absolute subsection's stale API3 prose to API4 while retaining ABI1 and behavior3.

## Remaining acceptance and metadata

No BOARD requirement is marked complete by this slice. Owned diagnostic ROMs and actual runner/libretro numeric AM/OM equivalence, achievement descriptors, expanded fuzz/lifecycle coverage, and all six native package/portability lanes remain assigned to06–09. Local package fit proves only macOS arm64. BOARD-01's descriptorless transparency prohibition and unclassified edge remain `unresolved` with `verification:null`; concrete tests do not silently classify or dismiss that metadata. Assumed OM is explicitly described as assumed; no owner verification is requested.

No new dependency, policy bound, runtime mapper framework, public layout override, allocation after load, or security-relevant external service is introduced. The runner's existing read-only fixed probe surface gains an additive flag line.

## Self-Check: PASSED

Both task commits are reachable on the existing phase branch, all modified files exist, no blocking stub was introduced, exact inventories and full CI pass, and all three packages fit the retained bound. Phase-goal verification and hosted acceptance belong to the parent after the remaining plans.
