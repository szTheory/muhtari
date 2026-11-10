---
phase: 06-retail-cartridge-boards
plan: "05"
status: complete
subsystem: cartridge
tags: [fractalus, aliases, absolute, lifecycle, portability]
requires:
  - phase: 06-04
    provides: Executable Activision OM mapper and supplied-image identity evidence
provides:
  - Physical 2 KiB Fractalus RAM with exact ROM geometry and explicit A8 combination refusal
  - Exhaustive independent retail address vectors and real CPU alias observations
  - Public load, power-cycle, RESET and allocation lifecycle evidence
affects: [06-06, 06-07, 06-08, 06-09]
tech-stack:
  added: []
  patterns:
    - Keep unsupported v4 combinations private until independent hash metadata overrides them
    - Derive physical expectations from page pairs and mirror enumeration
key-files:
  created: []
  modified:
    - src/cart.c
    - tests/cart/test_cart.c
    - tests/cart/tests.cmake
    - tests/inventory/ci.txt
    - docs/hardware/cartridge.md
    - tests/core/test_core.c
    - tests/inventory/wasm32.txt
    - docs/embedding.md
key-decisions:
  - Only Linear/A8 with the exact Fractalus geometry is supported; valid SuperGame/A8 and BankSet/A8 remain unsupported without v3 fallback.
  - Keep Fractalus physical capacity separate from CPU alias coverage and host achievement descriptors.
requirements-completed: []
requirements-contributed: [BOARD-02, BOARD-03]
coverage:
  - id: physical-aliases
    description: All 16384 window addresses reach independently derived physical bytes with eight aliases per byte across three patterns
    requirement: BOARD-03
    verification:
      - kind: integration
        ref: "cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^cart[.]retail[.]boards$' --no-tests=error --output-on-failure; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: retail-metadata-and-selectors
    description: Real CPU alias values, exact A8 acceptance/refusals, all Absolute data values from both latches and established Activision vectors pass
    requirement: BOARD-02
    verification:
      - kind: integration
        ref: "ctest --preset ci -R '^cart[.]' --no-tests=error --output-on-failure; exit 0; 21/21 including setup; URL N/A-local"
        status: pass
    human_judgment: false
  - id: volatile-lifecycle
    description: Public load and power cycle restore volatile state, console RESET preserves it, and all three boards run without post-load allocation or shared state
    requirement: BOARD-03
    verification:
      - kind: integration
        ref: "core.load.supergame_one_allocation and core.no_alloc_after_load through cmake --workflow --preset ci; exit 0; 612 non-release plus 6 release checks; URL N/A-local"
        status: pass
      - kind: other
        ref: "Both exact check_junit invocations, unchanged API/hash inputs, hygiene and exactly-three ZIP sum 213795 <= 399937; exit 0 each; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 7619
  tasks: 2
  commits: 2
plan_head_before: 5c6e09707f955d349982258b45ffcc5aa1baf571
plan_head_after: 2c0518f686062efffffb4d66dcf44e86ca5ba4a9
duration: 6min
started: 2026-10-23T19:59:39Z
completed: 2026-10-23
---

# Phase 6 Plan 5: Physical Fractalus memory Summary

**Fractalus maps 2,048 physical RAM bytes through eight aliases each, with independently verified CPU observations and volatile lifecycle behavior.**

## Task commits

1. `1d51980` — executable Fractalus geometry, physical address compression, private A8 refusal distinction, exhaustive retail vectors and hardware documentation.
2. `2c0518f` — three-board lifecycle/no-allocation checks, portable inventory and embedding contract.

The persisted ledger measures two task commits. Actual tokens are the 30,474-character realized task diff divided by four, rounded up. Summary and state metadata are outside that interval.

## Accomplishments and independent evidence

- Fractalus accepts exactly 32 KiB ROM and 2 KiB physical RAM with no board options, preserving one ROM-plus-RAM allocation. ROM occupies $8000-$FFFF. The RAM branch compresses disconnected A8 and upper mirror bits before the ordinary masking path. CPU, MARIA and peek see the same memory; declared audio writes retain priority.
- The private v4 decoder preserves unsupported SuperGame/A8 wiring without adding any public field or enum. Valid SuperGame/A8 and BankSet/A8 remain unsupported even over contradictory supported v3 headers. Linear/A8 at the supported geometry loads; malformed reserved bits keep ignored-v4 fallback. Independent test-table hash metadata still overrides the private refusal. Geometry invalid/truncated distinctions persist.
- The new `cart.retail.boards` uses eight explicit physical page pairs and four mirrors, rather than the mapper's compression expression. Three patterns distinguish low-byte offsets and physical blocks. Every one of 16,384 CPU window addresses is read and written, every physical byte has eight counted aliases, and deep physical comparisons detect unintended writes. Fixed ROM is checked across its entire range and remains unchanged. An actual CPU program reports independently derived values 90, 166 and 90 through distant aliases and a distinct physical block.
- Absolute vectors tabulate low-bit latch transitions independently from production code, testing all 256 data values from both possible initial banks, exact selector neighbors, endpoints, ROM immutability and CPU/MARIA/peek purity. The test also reuses established independent six-window Activision vectors for all eight banks.
- Existing lifecycle and no-allocation cases now exercise all three boards and two independent instances. Public load/reload and power cycle restore bank zero and zero physical RAM. RESET is applied through public input and observed as the held console switch after the existing VBLANK latch; it leaves cartridge state intact while an idle fixed-ROM program runs. Fractalus exposes exactly 2,048 volatile cartridge-RAM bytes, with no battery span. Bus traffic, frames and repeated power cycles allocate nothing after load. Existing `retro_reset` still directly calls `muhtari_power_cycle`; the unchanged adapter and its existing reset check remain in full CI.
- Native and WASM exact inventories contain the portable retail test with all prior rows preserved. This plan ran the native workflow; hosted WASM and other platform execution remain later acceptance. API 4, ABI 1, behavior 3, fixture manifest and prior frame/audio expectations are unchanged.

## Commands, exits and run URLs

All commands below ran locally; URL is N/A-local. No logs are committed.

| Command | Exit | Result |
|---|---:|---|
| Initial focused cartridge suite | 8 | Exposed an obsolete hash-listed Fractalus refusal vector; adjusted below |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^cart[.]retail[.]boards$' --no-tests=error --output-on-failure` | 0 | New tracer passes; repeated after its commit for the feedback gate |
| `cmake --build --preset ci && ctest --preset ci -R '^cart[.]' --no-tests=error --output-on-failure` | 0 | 21/21 including sample setup |
| Focused retail/lifecycle/no-allocation/get-memory suite | 0 | 4/4 after observing RESET at the existing second-frame VBLANK latch |
| `cmake --workflow --preset ci` | 0 | 612/612 non-release checks, three fresh packages, 6/6 release checks |
| `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` | 0 | Three fresh ZIPs and six release checks pass |
| cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake | 0 | Exactly 612 tests ran once each |
| cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake | 0 | Exactly six release tests ran once each |
| git diff --exit-code 5c6e097 -- include/muhtari.h src/instance.c tests/built/expected.txt tests/audio/expected.txt tests/roms/manifest.txt | 0 | API/version implementation and prior expected bytes unchanged |
| `scripts/hygiene.sh --tree` and `git diff --check` | 0 each | Hooks retained and no whitespace fault |
| Exactly-three ZIP expansion and sum below | 0 | 213,795 <= 399,937; +352 bytes from plan 04 |

```sh
set -- build/ci/muhtari-*.zip
test "$#" -eq 3 || exit 1
archive_bytes=0
for archive do
  size=$(wc -c < "$archive")
  printf '%s %s bytes\n' "$archive" "$size"
  archive_bytes=$((archive_bytes + size))
done
printf 'archive_bytes=%s ceiling=399937\n' "$archive_bytes"
test "$archive_bytes" -le 399937
```

| Component ZIP, macOS arm64 | Bytes | Change from 04 |
|---|---:|---:|
| library | 68,753 | +124 |
| libretro | 67,756 | +99 |
| runner | 77,286 | +129 |
| Total | 213,795 | +352 |

Exactly three fresh archives were measured. This local lane passes the unchanged archive ceiling; six-native fit remains unproved until plan 09.

## Decisions and deviations

Followed the selected physical capacity, page-pair map and metadata scope. No dependency, mapper framework, public layout, new resource bound or policy grant was introduced.

**Rule 1 — stale refusal expectations:** the old unsupported-board test used Fractalus both as a headered refusal and as a hash-listed refusal without physical RAM metadata. Fractalus is now executable; its new exhaustive case supplies correct physical RAM and tests acceptance. Removed the obsolete headered refusal and changed the hash-refusal control to still-unsupported BankSet, preserving its original fault detection. This is recorded as fixed WINDOWS entry 38 and committed with task 1. Initial lifecycle test construction sampled RESET before the established input latch; adding the required second frame corrected the test without a product change.

No unresolved implementation issue, known stub, skipped test or unrun verification required by this plan remains. No authentication gate, owner UAT or new unplanned security surface occurred. No review-fix or gap allowance was consumed.

## Limits and next readiness

No BOARD requirement is complete. BOARD-02/03 fallback rows remain unclassified/unresolved with verification null, and the A8/achievement transparency prohibition remains unresolved for phase acceptance. Documentation explicitly distinguishes physical 2 KiB from the A8 label, qualifies higher mirrors and startup as selected policies, and makes no retail-playability, hardware-measurement or complete-achievement claim. Truthful Fractalus host aliases and owned diagnostics remain assigned to plans 06-08; broader hostile-input and six-platform acceptance remain pending. Historical plan 01 and retired LOC policy are untouched.

Next incomplete plan is **06-06**. The parent owns remaining phase dispatch and gates within the current execution step; this executor stops after plan 05.

## Self-Check: PASSED

All eight changed task files exist; both task commits are ancestors of HEAD. The ledger measures two task commits. Full native CI, release checks, exact inventories, hygiene and bounded three-ZIP acceptance passed. No unexpected deletion or generated untracked file remains.
