---
phase: 06-retail-cartridge-boards
plan: "03"
status: complete
subsystem: api
tags: [activision, abi, api, smoke, portability]
requires:
  - phase: 06-02
    provides: Absolute three-host tracer, behavior revision 3 and local package acceptance
provides:
  - Append-only API 4 orientation and assumed-layout constants with unchanged ABI 1
  - Linked current and older API clients, header controls and exact inventory entries
  - Tested host bit names and documented supplied-image identity semantics
affects: [06-04, 06-06, 06-07, 06-08, 06-09]
tech-stack:
  added: []
  patterns:
    - Preserve frozen older smoke clients with an API-version floor
    - Register API smoke and exact native, nofp and WASM inventories atomically
key-files:
  created:
    - tests/api/v4.c
  modified:
    - include/muhtari.h
    - tests/api/v3.c
    - tests/core/test_core.c
    - tests/abi/tests.cmake
    - tests/header/header.c
    - tests/header/header.cpp
    - tests/inventory/ci.txt
    - tests/inventory/nofp.txt
    - tests/inventory/wasm32.txt
    - host/status_text.c
    - docs/embedding.md
key-decisions:
  - Publish descriptive orientation and explicit assumed-layout bits without changing structures or supplied-image identity.
  - Reuse the registered API 4 smoke for host bit-name boundary assertions; loader and host-equivalence behavior remains assigned to later plans.
requirements-completed: []
requirements-contributed: [BOARD-01]
coverage:
  - id: additive-api
    description: API 4 exports both new fixed-value constants through the linked library while ABI 1 and the 140-byte cart-info layout persist
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(abi[.]api|cart[.]identify[.])' --no-tests=error --output-on-failure; exit 0; 16/16; URL N/A-local"
        status: pass
    human_judgment: false
  - id: compatibility-and-host-names
    description: Older linked API clients, C17 and C++17 headers, API-version negative control and host bit names pass with exact inventories
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; exit 0; 611 non-release and 6 release tests; URL N/A-local"
        status: pass
      - kind: other
        ref: "Both check_junit commands against tests/inventory/ci.txt; exit 0 each; exact 611/6 executions; URL N/A-local"
        status: pass
    human_judgment: false
  - id: documented-transparency-contract
    description: Embedding docs distinguish hash-authorized input orientation from assumed OM layout and preserve raw identity, ABI and later-loader scope
    requirement: BOARD-01
    verification:
      - kind: other
        ref: "Agent read docs/embedding.md against include/muhtari.h, host/status_text.c and current loader scope; scripts/hygiene.sh --tree and git diff --check exit 0 each; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 3124
  tasks: 2
  commits: 2
plan_head_before: 3dfddc0d5df1f670cbe1a939b5490a7aeb51c699
plan_head_after: 0e42dff99d0fdc1824d28a10a855eab8ee86365c
duration: 4min
started: 2026-10-23T19:42:43Z
completed: 2026-10-23
---

# Phase 6 Plan 3: Append-only Activision interface Summary

**API 4 adds fixed-value descriptive orientation and assumed-layout bits, backed by linked smoke clients and tested host names, with ABI 1 and public layouts unchanged.**

## Accomplishments

- Added `MUHTARI_CART_ACTIVISION_HALF_SWAPPED=4u` and `MUHTARI_CART_WARN_ACTIVISION_LAYOUT_ASSUMED=1024u`; no public structure, signature or old constant changed. The linked v4 smoke asserts both constants, runtime API at least 4, ABI 1 and cart-info size 140.
- Added `abi.api_v4` and its ci/nofp/wasm32 inventory rows atomically. All prior rows persist. Older v3 now uses the established minimum-version convention; its constant and layout checks remain intact. The API negative control still rejects the deliberately untested field at version 4.
- Added `activision_half_swapped` and `activision_layout_assumed` to existing host tables, with persistent assertions for new names, old neighboring names and out-of-range bits in the same registered smoke.
- Documented API 4 / ABI 1 / behavior 3 and preserved supplied full/headerless hash and trailer meanings. The docs explicitly disclose that the Activision loader is not enabled by this interface slice. Assumed OM order is not described as detected; unmatched modified AM needs external conversion. No override API or production database entry was added.

## Task Commits

1. Task 1: `234f243` — append tested Activision API four constants.
2. Task 2: `0e42dff` — publish tested orientation names and embedding contract.

The persisted ledger measures two task commits. Actual tokens are the 12,496-character realized task diff divided by four; summary and state metadata are outside that interval.

## Commands, exits and run URLs

All commands ran locally; URL is N/A-local. No logs are committed.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(abi[.]api\|cart[.]identify[.])' --no-tests=error --output-on-failure` | 0 | 16/16 including API 1–4, deliberately untested field rejection and production test-key absence |
| Post-commit repeat of the same focused CTest command | 0 | Tracer feedback passed, 16/16 |
| First `cmake --workflow --preset ci` | 8 | C17/C++17 header-only assertions still required API 3; corrected below |
| Final `cmake --workflow --preset ci` | 0 | 611/611 non-release checks, all three fresh ZIPs, 6/6 release checks |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 611 tests ran once each |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly six release tests ran once each |
| `scripts/hygiene.sh --tree` and `git diff --check` | 0 each | Hygiene and whitespace pass; commit hooks enabled |
| `git diff 3dfddc0..HEAD -- tests/built/expected.txt tests/audio/expected.txt` | 0 | Empty: all old frame/audio expectations and behavior metadata unchanged |
| Exactly-three ZIP expansion and `wc -c` sum | 0 | 213,074 <= 399,937 |

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
| library | 68,504 |
| libretro | 67,566 |
| runner | 77,004 |
| Total | 213,074 |

This local lane does not establish six-native package fit. Plan 09 retains that acceptance and the unchanged archive ceiling. No nofp/WASM execution is claimed here; their exact inventory rows are updated with the registered portable smoke.

## Deviations from Plan

**[Rule 1 - Bug] Current API assertions in header-only C17/C++17 clients.** Full CI exposed two assertions of API 3 outside the named task files. Updated them to 4 while retaining all signature and offset checks; both passed in the fresh full workflow. Resolved in `0e42dff` and recorded as fixed in WINDOWS36. The host bit-name assertions reuse the v4 smoke and private host target rather than adding a separate test registration.

## Issues Encountered

The sandbox required escalation for Git index/commit-ledger writes; required hook-bearing task commits succeeded. No authentication or owner action was needed.

## Known Scope and Next Readiness

No BOARD requirement is complete. The BOARD-01 transparency prohibition remains unclassified/unresolved with verification null in the plan; concrete API checks do not silently change that metadata. This plan supplies interface evidence only. Plan 04 owns loader normalization, map and probe results; plans 06–08 own broader fixture/host equivalence and hostile behavior. No private orientation result was needed in this interface-only slice; retaining the lookup pass belongs to plan 04. No stub prevents this plan's goal and no new security surface was introduced.

## Self-Check: PASSED

The new smoke exists, both task commits are ancestors of HEAD, and all 12 task files are present. Fresh full CI, release checks, exact native inventories, hygiene and the three-package bound passed. No unexpected deletion or generated untracked file remains.
