---
phase: 16-emscripten-libretro-core
verified: 2026-11-10T22:43:52.113584Z
status: passed
score: 7/7 must-haves verified
covered_files:
  - .github/workflows/ci.yml
  - .planning/milestones/v4.0-phases/16-emscripten-libretro-core/16-01-PLAN.md
  - .planning/milestones/v4.0-phases/16-emscripten-libretro-core/16-01-SUMMARY.md
  - CMakePresets.json
  - docs/libretro.md
  - libretro/CMakeLists.txt
  - scripts/emscripten.sh
  - tests/emscripten/host.mjs
  - tests/emscripten/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/emscripten.txt
  - tests/libretro/observe.c
  - tests/libretro/tests.cmake
  - tests/tests.cmake
covered_digest: "v4:sha256:ac9fe36cde407f1f1a048c31f636abc878968a9ea0c0c7c0c656c862ced7c786"
behavior_unverified: 0
overrides_applied: 0
human_verification: []
---

# Phase 16: Emscripten Libretro Core — approved publication source

This inline verifier re-evaluated the isolated candidate based on origin/main, not the failed combined PR. Original reports remain preserved at [the combined source](https://github.com/szTheory/muhtari/tree/1929a0baae50d3e146369f305e78be60ffe9b35c/.planning/milestones/v4.0-phases). Source implementations and controls are byte-identical to the reviewed/fixed canonical v4.0 files; adapted CMake insertion points and preserved main checks were inspected directly. The current production core, libretro C source, fixture manifest and quality policy are unchanged from main.

## Observable truths

| Truth | Status | Evidence |
|---|---|---|
| Pinned production factory and Wasm exist | VERIFIED | The exact SDK/CMake/Node lane emits the production .mjs/.wasm; import/export controls and read-only mount/process-status controls pass. |
| Offline Node exercises the real lifecycle | VERIFIED | The exact two-test artifact/controls inventory passes with 14 parity frames and callbacks/load/run/serialize/unserialize/unload. |
| Native numeric observations precede hash comparison | VERIFIED | Current native receipt checks 320x242 video, 48 kHz, 800/801 audio counts and SWCHA 255/239/254 before adapter parity; mono samples are compared to both stereo channels without unused-buffer reads. |
| Malformed content/state is refused and preserved as specified | VERIFIED | Real artifact/native tests exercise invalid content and null/short/long/corrupt state, preservation and replay. |
| Instances remain isolated | VERIFIED | The direct consumer interleaves two production factories and checks saved-state independence. |
| Tests are wired into ordinary and required offline CI | VERIFIED | Native observer and fault controls are registered; exact JUnit inventories pass. CI differs from main only by the 15 added offline-lane/upload lines; existing required checks are unchanged. |
| Measurements are same-source and honestly bounded | VERIFIED | Current measurement JSON records artifact sizes and warmed native/Node windows, exact source/fixture identity and Linux amd64 SDK emulation on Darwin arm64; no browser/device performance claim is made. |

## Requirements coverage

| Requirement | Status | Verification |
|---|---|---|
| WASM-01 | SATISFIED | Current-source tests and wiring above; requirement remains checked complete in the archived requirements and summary frontmatter. |
| WASM-02 | SATISFIED | Current-source tests and wiring above; requirement remains checked complete in the archived requirements and summary frontmatter. |
| WASM-03 | SATISFIED | Current-source tests and wiring above; requirement remains checked complete in the archived requirements and summary frontmatter. |
| WASM-04 | SATISFIED | Current-source tests and wiring above; requirement remains checked complete in the archived requirements and summary frontmatter. |

## Current-source commands

- `python3 build/ship-local-gate.py`: exit 0; unchanged native CMake workflow through packages and release tests, bounded by the temporary two-worker FIFO jobserver.
- Native, release and hygiene `tests/cmake/check_junit.cmake` audits: exit 0; every inventoried case ran once without skips.
- `cmake --workflow --preset hygiene`: exit 0.
- `ctest --test-dir build/ci --output-on-failure -R '^(docs\.budgets(\.controls)?|libretro\.observe(\.controls)?)$'`: fresh exit 0.
- `sh scripts/emscripten.sh build/ci`: fresh exit 0; emitted artifact, exact two-case inventory, 14 parity frames, replay and isolation.

All run URLs are N/A-local. The source did not change between full workflow receipts and the fresh scope checks. The CLI generated covered_files/covered_digest after this review; no digest was handwritten. Hosted checks must pass on this candidate's own head before merge. Combined-branch passes and failures remain distinct evidence. All denied/spent host scopes and nine deferred v3.0 requirements remain unchanged. No owner validation is required for these offline criteria.
