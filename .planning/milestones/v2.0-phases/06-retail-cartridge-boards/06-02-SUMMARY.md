---
phase: 06-retail-cartridge-boards
plan: "02"
status: complete
subsystem: cartridge
tags: [absolute, diagnostic, runner, libretro, cmake]
requires:
  - phase: 06-10
    provides: Tested trusted fixture comparability through real producers and join
provides:
  - Executable Absolute mapping and transactional exact geometry
  - Fixed runner mailbox report and three-deliverable numeric tracer
  - Behavior revision 3 and truthful Absolute fixed-ROM descriptors
affects: [06-03, 06-04, 06-05, 06-06, 06-07, 06-08, 06-09]
tech-stack:
  added: []
  patterns:
    - Retain the instance latch and board switch
    - Validate observed mailbox bytes independently of cartridge mapping
key-files:
  created: []
  modified:
    - src/cart.c
    - src/instance.c
    - runner/main.c
    - libretro/libretro.c
    - tests/diag/driver.c
    - tests/diag/tests.cmake
    - tests/inventory/ci.txt
    - tests/cart/test_cart.c
    - tests/built/expected.txt
    - tests/audio/expected.txt
    - docs/hardware/cartridge.md
    - docs/runner.md
    - docs/libretro.md
key-decisions:
  - Keep diagnostic reporting fixed at 256 mailbox bytes with explicit frame bounds and test-owned numeric verdicts.
  - Preserve every prior frame and audio hash while advancing only their behavior revision metadata to 3.
requirements-completed: []
requirements-contributed: [BOARD-01, BOARD-02, BOARD-03]
coverage:
  - id: absolute-execution
    description: Exact 64 KiB geometry and bank-zero startup execute through library, actual runner and actual module
    requirement: BOARD-02
    verification:
      - kind: integration
        ref: "cmake --build --preset ci && ctest --preset ci -R '^diag[.]retail$' --no-tests=error --output-on-failure; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: mailbox-and-load-faults
    description: Strict numeric mailbox and report parsing, selector precedence, geometry refusal and allocation failure preserve bounded behavior
    requirement: BOARD-03
    verification:
      - kind: integration
        ref: "diag.retail protocol/report/usage/transaction controls and cart/diag focused CTest; 31/31; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: host-contract-and-package
    description: Fixed Absolute descriptors, ABI 1 and behavior 3 agree with docs and unchanged prior hash predicates
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; 610 non-release and 6 release tests; exit 0; URL N/A-local"
        status: pass
      - kind: other
        ref: "Exactly-three ZIP wc -c sum 212982 <= 399937; both exact check_junit invocations and scripts/hygiene.sh --tree; exit 0 each; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 10882
  tasks: 2
  commits: 2
plan_head_before: 2f9a2c44c9aab11d931778b7e6d1e1f4d64da0bc
plan_head_after: f0e1659f1fd460444d82619f9deac6e93d514a9c
duration: 7min
started: 2026-10-23T19:32:05Z
completed: 2026-10-23
---

# Phase 6 Plan 2: Absolute three-deliverable tracer Summary

**A real Absolute CPU program reports its bank-zero tag as numeric value 90 through the library, shipped-style runner and dynamically loaded libretro module.**

## Accomplishments

- Implemented exactly 65,536 ROM bytes with the existing instance latch and switch: switched $4000-$7FFF, fixed same-offset $8000-$FFFF, exact $8000 write selection, D0 precedence, retain otherwise, high data bits ignored. CPU/MARIA/peek memory agreement includes the write-only overlapping POKEY declaration; audio retains write priority. Load validation precedes allocation and installation remains transactional.
- Added boolean `--diagnostic`, requiring content and explicit ordinary `--frames` bounds. After the normal run it reads system RAM offset $A00 and prints exactly the fixed 256 bytes as lowercase hex. Tests own protocol and numeric verdicts; no extra frames, debugger API, address selector or dependency was introduced.
- Added native `diag.retail` and its exact inventory row together. The persistent synthetic tracer uses disposable runtime bytes and always runs. The batch discovers admitted named owned retail fixtures from the manifest, requiring bytes, expectations and all three observations. No retail diagnostic fixture is admitted yet; later fixture additions reuse this path. WASM retains its prior eligibility and inventory; dynamic loading uses the existing native Windows/POSIX pattern and cross-compiling emulator launch prefix.
- Published behavior revision 3, retaining API 3, ABI 1 and public layouts. Absolute libretro descriptors cover original-content ROM offsets $8000-$FFFF and leave the switched window unmapped. The test checks every cartridge-space descriptor address. Documentation identifies uncertain selector edges and startup as selected compatibility/profile policies.

## Task Commits

1. Task 1: `27627f1` — execute Absolute tracer across all three deliverables.
2. Task 2: `f0e1659` — publish Absolute mapping and behavior revision three.

Actual tokens are realized task-diff characters divided by four, rounded up (43,526 characters). The persisted ledger measures two task commits; summary and later state metadata are outside that interval.

## Commands, exits and run URLs

All commands ran locally; run URL is N/A-local. No logs are committed.

| Command | Exit | Result |
|---|---:|---|
| `cmake --build --preset ci && ctest --preset ci -R '^(diag[.]\|cart[.])' --no-tests=error --output-on-failure` | 0 | 31/31 including fixture setup, all existing cart/diag checks and new tracer |
| `cmake --build --preset ci && ctest --preset ci -R '^diag[.]retail$' --no-tests=error --output-on-failure` | 0 | Post-commit tracer feedback gate passed, 0.14 s |
| `cmake --workflow --preset ci` | 0 | 610/610 non-release tests, all three packages, 6/6 release checks |
| cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake | 0 | Exact 610 tests ran once each |
| cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake | 0 | Exact 6 release tests ran once each |
| `scripts/hygiene.sh --tree` and `git diff --check` | 0 each | Clean hygiene and whitespace; hooks also ran on both task commits |
| Read-only comparison of built/audio expectation files against the parent task commit, allowing only revision 2 to 3 metadata substitutions | 0 | All prior frame/audio hash bytes unchanged; full workflow verified them |
| Exactly-three ZIP expansion and `wc -c` sum below | 0 | 212,982 <= 399,937 |

```sh
set -- build/ci/muhtari-*.zip
test "$#" -eq 3 || exit 1
bytes=0
for archive do
  size=$(wc -c < "$archive")
  printf '%s %s bytes\n' "$archive" "$size"
  bytes=$((bytes + size))
done
printf 'archive_bytes=%s ceiling=399937\n' "$bytes"
test "$bytes" -le 399937
```

| Component ZIP, macOS arm64 | Bytes |
|---|---:|
| library | 68,470 |
| libretro | 67,537 |
| runner | 76,975 |
| Total | 212,982 |

The workflow packages immediately after its full test pass and runs the release archive/floor/firmware checks. Exactly three fresh component ZIPs were measured. This one-lane result does not establish six-native fit; plan 09 retains that explicit acceptance condition and the unchanged 399,937-byte ceiling.

## Fault Evidence

The synthetic image runs real 6502 fixed-ROM code, copies the complete version-1 mailbox, reads tagged bank zero, replaces the record and publishes status last. Expectations are independent of the mapper. Controls reject wrong magic, version, status, ROM ID, count, CRC, record ID, flags and value, including internally consistent CRCs for record faults. Report controls reject absent files, empty/malformed reports and extra output; runner usage controls reject missing content/frame bounds, benchmark mode and duplicate flags.

The same test exercises every Absolute data value and selector neighbors, CPU/MARIA/peek agreement, unchanged ROM bytes, truncated and invalid geometry without allocation, allocation failure without live-state mutation, subsequent execution against a control instance, reload/power-cycle bank zero and overlapping audio write priority. Absolute has no cartridge RAM; later plans extend board/RAM adversarial coverage.

## Deviations from Plan

- **Rule 1 — stale refusal predicate:** the existing `cart.probe.unsupported_board` still expected valid Absolute to be refused. Moved its existing valid case into `cart.probe.accepted` and retained all remaining refusal rows (`tests/cart/test_cart.c`, task 1). The focused suite first exposed the stale expectation, then passed 31/31.
- **Rule 3 — revision-coupled expectation metadata:** the required behavior revision bump also requires existing built/audio regression files to name revision 3. Updated only those metadata fields, preserving every pinned numeric/hash predicate (`tests/built/expected.txt`, `tests/audio/expected.txt`, task 2). Full CI passed without regenerating hashes.

These are completed integration adjustments with no outstanding defect, skipped test or unrun verify. Git metadata writes required sandbox escalation; hooks stayed enabled. No authentication gate, owner UAT, new resource bound, external source reuse or architecture change occurred. No review-fix or gap allowance was consumed.
The deviations are recorded as fixed entries 34/35 in the cross-phase WINDOWS ledger.

## Limits and Next Readiness

No BOARD requirement is complete: Activision, Fractalus, owned board fixtures, further hostile-input/host coverage and six-native package acceptance remain in plans 03-09. No known stub prevents this tracer goal, and no unplanned security surface was added. Historical plan 01 remains superseded and unchanged. Archive excess was not observed.

Next incomplete plan: **06-03**. The parent continues phase dispatch within the current execution step; the next command after the phase step remains `$gsd-verify-work 6` when execution finishes.

## Self-Check: PASSED

All thirteen changed task files exist. Both task commits are ancestors of HEAD. The ledger measures two commits and full local tests, release checks, exact inventories, hygiene and bounded three-ZIP acceptance passed. Generated runtime files remain under ignored build output; no unexpected deletion or untracked file remains.
