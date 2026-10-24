---
phase: 08-controllers-at-the-pins
plan: "02"
subsystem: input
tags: [api, controllers, lifecycle, tdd]
requires:
  - phase: 08-01
    provides: Controller evidence prerequisite
provides:
  - API6 separate extended input boundary and frozen linked clients
  - Transactional digital selection, pending packet and neutral reset lifecycle
affects: [08-03, 08-04, 08-05, 08-06, 08-07, 08-08, 08-09, 08-10, 08-11, 08-12, 08-13, 08-14]
tech-stack:
  added: []
  patterns: [Separate legacy validation, complete-scalar normalization, remembered load choices]
key-files:
  created:
    - tests/api/v6.c
    - tests/api/v6-layout.c
  modified:
    - include/muhtari.h
    - src/internal.h
    - src/ports.c
    - src/instance.c
    - src/frame.c
    - tests/input/test_input.c
    - tests/core/test_core.c
    - tests/abi/tests.cmake
    - tests/header/header.c
    - tests/header/header.cpp
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/inventory/nofp.txt
    - docs/embedding.md
key-decisions:
  - Preserve the old setter's validator and automatic fallback independently of extended support.
  - Normalize complete scalar fields with fixed-size memory copies, preserving the restricted link surface.
requirements-completed: []
coverage:
  - id: additive-digital-input
    description: Separate 88-byte packet reaches real CPU controller and switch reads without broadening the old API.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: input.directions, input.switches, input.unplugged
        status: pass
    human_judgment: false
  - id: transactional-packet
    description: Complete scalar prefixes, bounded unknown tails and invalid second-port fields preserve accepted pending state.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: tests/input/test_input.c extended_digital
        status: pass
    human_judgment: false
  - id: selection-lifecycle
    description: Initial extended input precedes the first opcode, Auto restores load choice, and reset preserves frozen battery storage.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: core.load.keeps_previous, core.hsc.topology, core.battery.reset, core.no_alloc_after_load
        status: pass
    human_judgment: false
  - id: frozen-clients
    description: API6 and independently declared layout clients execute alongside unchanged old clients and exact lane inventories.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: abi.api_v1 through abi.api_v6, abi.api_v4_layout, abi.api_v6_layout, abi.api_version
        status: pass
    human_judgment: false
duration: 7min
completed: 2026-10-24
status: complete
actuals:
  tokens: 11031
  tasks: 3
  commits: 6
plan_head_before: 57b0a97d6e96e3581631639022483dadb15f1620
plan_head_after: f775bcc2257b3b452cce8a453a6165086a8e9b26
---

# Phase 8 Plan 2: Additive Input Boundary Summary

**API6 adds a separate 88-byte transactional digital packet, remembered load selection and first-opcode input, while preserving the 24-byte old packet and frozen battery attachment.**

## Performance

- Started: 2026-10-24T19:55:59Z
- Completed: 2026-10-24T20:02:55Z
- Tasks: 3
- Product/test/document files changed: 16
- Actual tokens use realized diff characters divided by four, rounded up; commits are measured from the persisted plan-base ledger. Metadata commit is separate.

## Accomplishments

The extended setter validates both resolved devices and payloads before replacing one pending packet. All complete 4-byte prefixes from 4 through 88 normalize neutrally; unknown zero tails are accepted through the existing 4096-byte cap. Unsupported controller models still refuse. Digital fields reach the existing CPU pin-reading cartridge, with held state, latest-packet replacement and transactional rejection evidence. The old validator, values, 24-byte layout and automatic fallback remain independent.

Successful load stores explicit choices separately from per-frame selection and starts them neutrally. Initial extended input applies before fast boot's first opcode; subsequent packets remain VBLANK-applied. Auto restores the load choice. Power cycle clears pending/volatile input after settling already-due EEPROM programming, retaining attachment and committed bytes. Tests exercise SaveKey and AtariVox on both ports, forbidden add/remove/move/replace requests, complete storage bytes, pointer/length/layout/generation and old initial-frame timing.

Two API6 clients cover every new name and exact layout; one declares the layout independently of the header. All frozen v1–v5 and v4-layout clients remain byte-identical. Native, WASM and nofp exact inventories include both new clients. The library lane selects only its ledger tests, so its inventory is unchanged. The API coverage parser already derives the version dynamically and needed no change.

## Task Commits

1. Task 1 RED: `2ada9a4` — exercise extended digital packets at CPU pins.
2. Task 1 GREEN: `84f7f0d` — validate and latch additive digital packets.
3. Task 2 RED: `cef86dc` — pin initial input and immutable storage lifecycle.
4. Task 2 GREEN: `170b4e3` — preserve load choices and neutral reset ordering.
5. Task 3 RED: `2d4aa79` — freeze API6 clients and exact lane inventories.
6. Task 3 GREEN: `f775bcc` — publish API6 with complete linked-client coverage.

## Command Evidence

All run URLs below are `N/A-local`; no hosted or cross-platform execution is claimed.

| Command | Exit | Observed result |
|---|---|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^input[.](directions\|switches\|unplugged)$' --no-tests=error --output-on-failure` | 0 | Three cases pass; tracer observes SWCHA BD, SWCHB 37 and expected fire pins. Re-run before expansion also passes. |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^core[.](load[.]keeps_previous\|hsc[.]topology\|battery[.]reset\|no_alloc_after_load)$' --no-tests=error --output-on-failure` | 0 | Four required lifecycle cases pass. |
| `ctest --preset ci -R '^core[.](load[.]keeps_previous\|hsc[.]topology\|battery[.]reset\|no_alloc_after_load\|set_input[.].*)$\|^input[.](directions\|switches\|unplugged)$' --no-tests=error --output-on-failure` | 0 | Nine focused new and legacy cases pass after formatting. |
| `/usr/bin/cc -std=c17 -Wall -Wextra -Wpedantic -Werror tests/api/v6-layout.c build/ci/libmuhtari.a -o build/ci/tests/muhtari-test-api-v6-layout && build/ci/tests/muhtari-test-api-v6-layout` | 0 | Independently declared client links and executes before registration. |
| `cmake --workflow --preset ci` | 0 | Final full run passes 643 ordinary tests and six release tests, including API, docs, install, frame/audio pins and packaging. |
| `/opt/homebrew/opt/llvm@21/bin/clang-format --dry-run --Werror include/muhtari.h src/internal.h src/ports.c src/instance.c src/frame.c tests/input/test_input.c tests/core/test_core.c tests/api/v6.c tests/api/v6-layout.c tests/header/header.c tests/header/header.cpp` | 0 | Changed source and client files formatted with pinned clang-format 21. |
| `sh scripts/hygiene.sh --tree` | 0 | Tree hygiene passes. All commits also ran enabled hooks with the noreply identity. |
| `git diff --check` | 0 | No whitespace errors. |
| git diff --name-only 57b0a97 -- tests/api/v1.c tests/api/v2.c tests/api/v3.c tests/api/v4.c tests/api/v5.c tests/api/v4-layout.c | 0 | Empty output: frozen old clients unchanged. |

`python3 -c 'import pathlib; a=list(pathlib.Path("build/ci").glob("muhtari-*.zip")); assert len(a)==3; print(sum(p.stat().st_size for p in a))'` exits 0 and reports 232285 bytes for the three local macOS arm64 archives (run URL N/A-local). This single-lane observation does not establish final six-native fit, cross-host equality or independent comparable-base acceptance.

## TDD Evidence

Each task has RED then GREEN commits. The RED setter initially returned unsupported solely to allow the actual behavioral assertions to run; no such stub remains. API advertisement moved to Task 3 with the frozen clients, as planned.

- Task 1: `ctest --preset ci -R '^input[.]directions$' --no-tests=error --output-on-failure --output-junit red-input.xml` exits 8: accepted digital input returns 13 instead of 0, and real CPU reads show FF/3F instead of BD/37. A preliminary misspelled device constant caused a compile failure; corrected before RED and not counted as evidence.
- Task 2: `ctest --preset ci -R '^core[.]load[.]keeps_previous$' --no-tests=error --output-on-failure --output-junit red-lifecycle.xml` exits 8: first opcode reads FF instead of EF, Auto loses joystick selection and reset retains pressed controls.
- Task 3: `ctest --preset ci -R '^abi[.]api_v6$' --no-tests=error --output-on-failure --output-junit red-api.xml` exits 8: the actual linked client asserts compile/runtime API at least 6 against 5. Missing-client/configure failures are not RED evidence.

For each record, `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/red-input.json --raw` (substituting `red-lifecycle.json` and `red-api.json` for the other two records) exits 0 with `RED_EVIDENCE_OK` using actual CTest JUnit reports. Semantic inspection confirms executed target assertions fail for the intended absent behavior. Reports and records remain ignored local build output, not committed logs. GREEN focused commands and final full workflow above pass. No additional refactor commit was needed.

## Decisions Made

Followed the settled API6/ABI1 contract; behavior revision remains 4 pending the later hardware correction slice. No dependency, device model, new queue or scheduler was added. Complete-field normalization reuses the existing media-boundary copy pattern; callers on one instance remain single-threaded and independent instances share no state.

## Deviations from Plan

**[Rule 3 - Blocking] Include the actual nofp ABI inventory.** Inspection showed nofp includes the entire ABI registration file. Added the two client entries to `tests/inventory/nofp.txt` rather than leaving that lane's configure broken. Library inventory correctly remains unchanged. Commit `2d4aa79`.

**[Rule 1 - Bug] Integrate the API bump with existing current-version probes and restricted linking.** The first full workflow exited 8 with five directly caused failures: stale API5 expectations in `core.info` and the C/C++ header checks, missing exact adapter names in embedding docs, and Apple's generated `memcpy_chk` from a dynamic-size normalization copy. Updated current-version probes and exact documentation names, added the extended setter's C/C++ signature checks, and reused complete-scalar fixed-size copies to preserve the existing ABI link restriction. All six focused affected/tracer cases and the final full workflow pass. Frozen old clients were untouched. Commit `f775bcc`.

Task 2 also carries formatter-only corrections to new Task 1 code; ownership and behavior are unchanged. These implementation corrections used one Task 3 auto-fix attempt; no phase review-fix or gap round was consumed.

The SDK advanced the current plan to 3, recorded metrics/decisions and counted 30/42 runtime summaries. Its roadmap helper updated the plan checkbox/detail while retaining the stale progress-table cell; reconciled that cell to 2/14. Retained the distinct 3/7 milestone phases (43%) and 29/41 active slots, excluding the historical halted 06-01 summary from active acceptance. No requirement was prematurely marked complete.

## Limits and Next Readiness

INPUT-01 remains incomplete: runner replay, libretro input mappings, device implementations and phase-wide acceptance are still owned by later plans. All five INPUT requirements remain open. Fields for those models are published with documented units but are deliberately refused until real device behavior is installed; they are not mock controls or a claim of those devices working. Relative contribution storage exists for the following motion slices; current supported digital models reject nonzero motion. Hardware calibration and final cross-platform acceptance remain pending.

No authentication gate, owner verification, unexpected deletion, untracked generated file, known implementation stub, skipped test, unrun plan verification or unplanned security-relevant surface remains. New input validation is in the planned boundary. Licensing and fixture policy are unchanged; no external emulator source or new fixture bytes were admitted.

Next separate command: `$gsd-execute-phase 8` to continue with 08-03. Stop after this execution step; shipping remains separate.

## Self-Check: PASSED

The public header, ports implementation, both new clients and this summary exist. All six task commits are reachable from HEAD; the measured task range is six commits. Full workflow, formatting, tree hygiene, frozen-client identity and whitespace checks pass. No unexpected deletion or generated untracked output remains. Metadata commit is recorded separately by the executor.
