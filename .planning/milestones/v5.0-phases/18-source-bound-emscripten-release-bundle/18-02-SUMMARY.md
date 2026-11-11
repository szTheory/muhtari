---
phase: 18-source-bound-emscripten-release-bundle
plan: "02"
subsystem: release-packaging
tags: [emscripten, libretro, release, cmake]
requires:
  - phase: 18-01
    provides: Source-bound Emscripten archive, fixed member validator, and Node/native controls
provides:
  - Portable ZIP integrated into the existing exact-source publisher and workflows
  - Download, extraction, Node loading, provenance, and evidence-limit documentation
affects: [release-pipeline, libretro-hosting]
actuals:
  tokens: 27245
  tasks: 3
  commits: 11
plan_head_before: 06ea0616f9228bbbe1f3450cb6e2d8a11106ebbc
plan_head_after: f400a76ee9e66fbd37e709a8628f1e6486899c22
tech-stack:
  added: []
  patterns: [single publisher asset iterator, strict bundle validation before checksums, source-relative host guidance]
key-files:
  created:
    - .planning/phases/18-source-bound-emscripten-release-bundle/18-REVIEW-FIX-2.md
  modified:
    - scripts/publish.sh
    - tests/policy/publish.cmake
    - .github/workflows/ci.yml
    - .github/workflows/release.yml
    - tests/hygiene/tests.cmake
    - tests/emscripten/bundle.cmake
    - tests/cmake/install_line.cmake
    - README.md
    - docs/libretro.md
    - docs/testing.md
    - PROVENANCE.md
    - THIRD-PARTY-NOTICES.md
    - .planning/preparation/ENGINEERING.md
    - .planning/preparation/DECISIONS.md
key-decisions:
  - "Add one portable ZIP to the existing release set: 19 ZIPs, 20 assets including SHA256SUMS, and six unchanged native commit receipts."
  - "Keep the bundle unpublished until a separately authorized release; document only local Node/native evidence."
patterns-established:
  - "Release preparation validates the portable archive against the current source before checksums."
requirements-completed: [WASM-08, WASM-09]
coverage:
  - id: D1
    description: The existing release path validates, inventories, and transports the portable archive while preserving native release checks.
    requirement: WASM-08
    verification:
      - kind: integration
        ref: "cmake -DROOT=\"$PWD\" -DWORK=\"$PWD/build/phase18-final-production-publish-controls\" -DPRODUCTION_ARCHIVE=\"$PWD/build/emscripten/muhtari-libretro-emscripten.zip\" \"-DEXPECTED_SHA=$(git rev-parse HEAD)\" -P tests/policy/publish.cmake (exit 0; URL N/A-local)"
        status: pass
      - kind: integration
        ref: "python3 build/ship-local-gate.py; native JUnit 692/692 and release JUnit 6/6 (exit 0; URL N/A-local)"
        status: pass
    human_judgment: false
  - id: D2
    description: Integrators have reproducible download, extraction, companion resolution, Node loading, and offline evidence limits.
    requirement: WASM-09
    verification:
      - kind: e2e
        ref: "sh scripts/emscripten.sh build/ci (exit 0; 4/4 SDK tests, Node/native parity; URL N/A-local)"
        status: pass
      - kind: integration
        ref: "README portable link controls: correct link passes; missing, plain-text, wrong-target, plain-text guide, and fenced-only links fail with named diagnostics (URL N/A-local)"
        status: pass
      - kind: other
        ref: "Extracted ZIP members matched tests/emscripten/bundle-members.txt; docs/libretro.md Node snippet and evidence limits reviewed (URL N/A-local)"
        status: pass
    human_judgment: false
metrics:
  duration: not recorded
  completed: 2026-11-11
  status: complete
---

# Phase 18 Plan 02: Portable Release Integration Summary

**The source-bound Emscripten ZIP now travels through the existing exact-source release path with local native, SDK, and hygiene gates passing.**

## Task Commits

- Task 1 RED/GREEN: `938f69c`, `082a7fb`
- Task 2 documentation: `8686f14`
- Task 3 README checker/testing guide: `99b6b03`
- Review/fix commits: `7d7aed6`, `c590569`, `c0fc3af`, `26df012`, `b3f73ab`, `ffaf025`, `f400a76`.

## Accomplishments

- Added the portable ZIP to one publisher iterator, retaining all 18 native ZIPs and six native commit receipts. The prepared release set is 19 ZIPs and 20 total assets including `SHA256SUMS`.
- Added the Linux x64 SDK upload and source-ID evidence to existing workflows, plus exact-source, notice, download, signer/ref, tamper, and inventory controls.
- Documented the version-free URL, extraction layout, Node factory example, companion files, provenance, and limits of offline evidence. README checks require clickable portable and guide links.

## Recorded Results

- Pass, exit 0: synthetic bundle controls and offline publisher controls; expected omissions, source mismatch, damage, signer/ref and download corruption were rejected.
- Pass, exit 0: final source `f400a76ee9e66fbd37e709a8628f1e6486899c22`; python3 build/ship-local-gate.py (692/692 CI tests, 6/6 release tests; CTest 102.86s). Both native JUnit inventory checks passed.
- Pass, exit 0: `sh scripts/emscripten.sh build/ci` (~16s); 4/4 package tests and all four SDK JUnit entries passed, with offline Node/native parity.
- Pass, exit 0: the production publisher branch accepted the final SDK archive and rejected each of five qualified-notice mutations (~22s). `unzip -t` passed; the archive had the exact 14 manifest members, and its embedded commit and source-ID sidecar matched the final checkout/build.
- Pass, exit 0: python3 build/phase18-hygiene-gate.py; 30/30 hygiene tests passed in 51.96s using the locally qualified one-slot FIFO jobserver. Workflow preset, tests and deadlines were unchanged.
- URLs for all checks: N/A-local. No browser, device, remote CI, rehearsal, publication, push, or merge was run.

### Preserved failures and fixes

- The first native attempt on `b3f73ab` exited 8: `policy.notices` treated the generated archive `.mjs` and `.wasm` names as repository paths. `THIRD-PARTY-NOTICES.md` was clarified in `ffaf025`; the focused policy test and later native gates passed. Original logs and JUnit are in ignored `build/phase18-final-evidence/native-first-failure/`.
- The first SDK attempt on `ffaf025` exited 8: production notice fault controls used slash-containing member names as validator work-directory names, so the expected notice-digest rejection was preempted by a missing work parent. `tests/emscripten/bundle.cmake` now sanitizes only the work-directory name (`f400a76`); all five actual notice mutations and the final SDK workflow passed. Original logs are in ignored `build/phase18-final-evidence/emscripten-first-failure/`.
- An earlier publisher mock invocation also exposed a duplicate `;;` syntax error; it was fixed before the green publisher commit, and later controls passed.

## Review Notes

The review-required publisher fixture and README link issues were fixed. Optional seed: the README checker does not generalize to inline-code or tilde-fenced wrappers; the current clickable links and triple-backtick fence fault control pass. No required requirement or safety gap remains open.

## Deviations from Plan

Rule 1 fixes addressed the notice policy failure and the production-control work-path bug described above. Review also updated the hygiene publisher fixture to require the new portable ZIP while retaining a missing-native-archive fault. No dependency, timeout, preset, authority, or release behavior outside the planned inventory was added.

## User Setup Required

None. The ZIP remains planned for a separately authorized future release.

## Self-Check: PASSED

- This summary records requirements WASM-08 and WASM-09 with automated, local evidence and `human_judgment: false`.
- Evidence references identify commands, outcomes and source commits.
- The source snapshot passed all final gates at `f400a76`; build evidence remains ignored.
