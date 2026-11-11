---
phase: 18-source-bound-emscripten-release-bundle
plan: "01"
subsystem: release-packaging
tags: [emscripten, libretro, cmake, licensing, zip]
requires: []
provides:
  - Source-bound Emscripten libretro ZIP with exact members, provenance, pair digests, and qualified notices
  - Clean-extraction Node/native consumer verification and recurring package fault controls
affects: [phase-18-release-pipeline, emscripten-hosting]
actuals:
  tokens: 13852
  tasks: 2
  commits: 2
plan_head_before: 89f105f0518664c443addc3cedddd6418ba6dae7
plan_head_after: 84b596bc8580ef345a25248a9d0681af69b5e908
tech-stack:
  added: []
  patterns: [fixed archive member contract, internal notice-table validation seam, CTest package creation dependency]
key-files:
  created:
    - tests/emscripten/bundle-members.txt
    - tests/emscripten/bundle.cmake
    - docs/emscripten-hosting.md
  modified:
    - scripts/emscripten.sh
    - tests/emscripten/tests.cmake
    - tests/inventory/emscripten.txt
    - THIRD-PARTY-NOTICES.md
requirements-completed: [WASM-05, WASM-06, WASM-07]
coverage:
  - id: D1
    description: Exact source-bound libretro ZIP with pinned SDK identity, matched loader/Wasm digests, host contract, and five qualified notices.
    requirement: WASM-05
    verification:
      - kind: integration
        ref: "sh scripts/emscripten.sh build/ci (exit 0; local, URL N/A-local)"
        status: pass
      - kind: other
        ref: "sh scripts/emscripten.sh --preflight build/ci (exit 0; local, URL N/A-local)"
        status: pass
    human_judgment: false
  - id: D2
    description: Clean extraction passes the existing Node/native lifecycle, while representative missing, mismatched, and tampered packages fail with specific diagnostics.
    requirement: WASM-06
    verification:
      - kind: e2e
        ref: "sh scripts/emscripten.sh build/ci (exit 0; local, URL N/A-local)"
        status: pass
      - kind: integration
        ref: "cmake -DROOT=$PWD -DWORK=build/phase18-bundle-controls -DMODE=controls -P tests/emscripten/bundle.cmake (exit 0; local, URL N/A-local)"
        status: pass
      - kind: integration
        ref: "Pinned emscripten/emsdk@sha256:6bd7aef93408b10bf7b36642cb7c91119b302f720b9ec2d20439f73cf07fe894; cmake --preset emscripten && ctest --test-dir build/emscripten --output-on-failure -R '^emscripten[.]bundle([.]controls)?$' --no-tests=error (exit 0; local, URL N/A-local)"
        status: pass
    human_judgment: false
  - id: D3
    description: Packaging verifies the five exact SDK notice hashes and recurring production controls reject altered notice files.
    requirement: WASM-07
    verification:
      - kind: integration
        ref: "sh scripts/emscripten.sh --preflight build/ci and sh scripts/emscripten.sh build/ci (both exit 0; local, URL N/A-local)"
        status: pass
      - kind: integration
        ref: "Pinned SDK emscripten.bundle.controls production notice mutations (exit 0 with each expected rejection; local, URL N/A-local)"
        status: pass
    human_judgment: false
metrics:
  duration: 20m
  completed: 2026-11-10
status: complete
---

# Phase 18 Plan 01: Source-Bound Emscripten Release Bundle Summary

**A pinned, source-bound libretro ZIP now carries exact runtime notices and passes clean-extraction lifecycle checks.**

## Performance

- **Duration:** 20 minutes from the first recorded task commit through Task 2; earlier Task 1 time was not separately recorded.
- **Started:** 2026-11-10T23:03:36-05:00 (first task commit; actual work began earlier)
- **Completed:** 2026-11-10T23:22:25-05:00
- **Tasks:** 2
- **Files modified:** 7

## Accomplishments

- Added the exact 14-member archive contract, package creation/validation, current-source and SDK metadata, pair digests, and all five hash-qualified SDK notices.
- Verified the freshly extracted module with the existing Node/native oracle and kept observer, fixture, measurement, and surface data outside the archive.
- Registered package creation and fixed-contract fault controls in the existing Emscripten CTest lane, including missing companions, unsafe/extra/duplicate members, identity mismatches, digest faults, tampering, and production notice mutations.
- All evidence was produced locally; run URLs are N/A-local.

## Task Commits

1. **Task 1: Build one licensed ZIP and run the existing lifecycle from its extraction** - `63e5e19` (`feat`)
2. **Task 2: Make package omissions, mismatches and tampering recurring required failures** - `84b596b` (`feat`)

## Files Created/Modified

- `tests/emscripten/bundle-members.txt` - sorted, exact 14-member archive inventory.
- `tests/emscripten/bundle.cmake` - producer, strict validator, extracted consumer, and representative synthetic/production controls.
- `scripts/emscripten.sh` - early notice-hash preflight and current-source identity handoff into the existing pinned producer.
- `tests/emscripten/tests.cmake`, `tests/inventory/emscripten.txt` - required package and controls registration.
- `THIRD-PARTY-NOTICES.md` - archive-specific pinned SDK provenance and notice digests.
- `docs/emscripten-hosting.md` - concise loader and companion-file integration contract, linked from the ZIP.

## Decisions Made

- Production create and validate paths always use their five literal qualified notice digests. Only the included internal structural-validation function accepts synthetic tables for owned controls; a direct production validation rejects a synthetic archive.
- The registered package test creates the current bundle before its dependent controls run, keeping package work inside the existing SDK lane.

## Deviations from Plan

### Auto-fixed Issues

**1. Rule 1 - Bug: Corrected package manifest and metadata parsing defects found by the first real tracer run.**
- **Found during:** Task 1
- **Issue:** The member ordering and metadata/source-sidecar path parsing did not match the fixed archive contract.
- **Fix:** Corrected the sorted inventory and CMake key/path capture handling.
- **Files modified:** `tests/emscripten/bundle-members.txt`, `tests/emscripten/bundle.cmake`
- **Verification:** `sh scripts/emscripten.sh build/ci` exited 0 and the extracted Node/native lifecycle passed.
- **Committed in:** `63e5e19`

**Total deviations:** 1 auto-fixed bug. No scope change or new dependency.

## Recorded Results

- **Pass, exit 0:** `sh -n scripts/emscripten.sh`; `sh scripts/emscripten.sh --preflight build/ci`; `sh scripts/emscripten.sh build/ci`; and `git diff --check` during Task 1. The full producer run created the ZIP and passed the fresh-extraction Node/native oracle and existing Emscripten checks.
- **Pass, exit 0:** `cmake -DROOT="$PWD" -DWORK=build/phase18-bundle-controls -DMODE=controls -P tests/emscripten/bundle.cmake`; all synthetic expected failures had their named diagnostics.
- **Pass, exit 0:** With the exact pinned image `emscripten/emsdk@sha256:6bd7aef93408b10bf7b36642cb7c91119b302f720b9ec2d20439f73cf07fe894`, `cmake --preset emscripten` and `ctest --test-dir build/emscripten --output-on-failure -R '^emscripten[.]bundle([.]controls)?$' --no-tests=error`; both package tests passed (2.73s and 17.68s).
- **Failed invocation, exit 125:** One Docker command used a mistyped image digest and Docker rejected its digest length before starting a container. The digest was checked against the plan and script, then the exact pinned-image command above passed; this was not a product or test failure.

## Issues Encountered

The malformed image-reference invocation was corrected before execution; the focused package tests then passed against the locally available pinned SDK image.

## User Setup Required

None - no external service configuration required.

## Next Phase Readiness

The reusable source-bound package contract and recurring local controls are ready for plan 18-02 to integrate into the release pipeline and run the final full native/SDK workflows. Phase acceptance remains owned by the orchestrator.

## Self-Check: PASSED

- Summary file exists at the planned output path.
- Task commits `63e5e19` and `84b596b` are present on the current branch.
- Measured pre-summary plan commit count: 2; `plan_head_before` and `plan_head_after` match the measured commit range.

---
*Plan: 18-01*
*Completed: 2026-11-10*
