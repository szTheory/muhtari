---
phase: 15-measured-milestone-tune-up
verified: 2026-11-10T22:43:52.113584Z
status: passed
score: 7/7 must-haves verified
covered_files:
  - .planning/milestones/v4.0-phases/15-measured-milestone-tune-up/15-01-PLAN.md
  - .planning/milestones/v4.0-phases/15-measured-milestone-tune-up/15-01-SUMMARY.md
  - docs/testing.md
  - tests/cmake/budgets.cmake
  - tests/inventory/ci.txt
  - tests/inventory/wasm32.txt
  - tests/policy/budgets.cmake
  - tests/policy/tests.cmake
covered_digest: "v4:sha256:66648c733d5d612751186b5b3567954c93e5221ec1cdb258b0efd17797e8d28d"
behavior_unverified: 0
overrides_applied: 0
human_verification: []
---

# Phase 15: Measured Milestone Tune Up — approved publication source

This inline verifier re-evaluated the isolated candidate based on origin/main, not the failed combined PR. Original reports remain preserved at [the combined source](https://github.com/szTheory/muhtari/tree/1929a0baae50d3e146369f305e78be60ffe9b35c/.planning/milestones/v4.0-phases). Source implementations and controls are byte-identical to the reviewed/fixed canonical v4.0 files; adapted CMake insertion points and preserved main checks were inspected directly. The current production core, libretro C source, fixture manifest and quality policy are unchanged from main.

## Observable truths

| Truth | Status | Evidence |
|---|---|---|
| Relative and absolute roots enforce the same document set | VERIFIED | The checker implementation and synthetic controls are byte-identical to the reviewed canonical versions; fresh docs.budgets and controls pass. |
| Exact limits pass | VERIFIED | Synthetic exact-size fixtures pass the registered control. |
| Each named over-limit case is rejected | VERIFIED | The full policy control checks each diagnostic and expected failing exit. |
| Missing roots are rejected | VERIFIED | The registered control exercises both missing-root forms. |
| The control runs in ordinary CI | VERIFIED | Registration and both inventories include docs.budgets.controls; current-source native inventory audit passes. |
| Maintainer reproduction is documented | VERIFIED | docs/testing.md documents the root behavior and command; current document budgets pass. |
| The tune-up preserves resource and acceptance policy | VERIFIED | Limits, deadlines, quality policy, core and host scope match main; historical baseline remains in 15-TUNEUP.md with this source projection explicitly separated. |

## Requirements coverage

| Requirement | Status | Verification |
|---|---|---|
| TUNE-08 | SATISFIED | Current-source tests and wiring above; requirement remains checked complete in the archived requirements and summary frontmatter. |

## Current-source commands

- `python3 build/ship-local-gate.py`: exit 0; unchanged native CMake workflow through packages and release tests, bounded by the temporary two-worker FIFO jobserver.
- Native, release and hygiene `tests/cmake/check_junit.cmake` audits: exit 0; every inventoried case ran once without skips.
- `cmake --workflow --preset hygiene`: exit 0.
- `ctest --test-dir build/ci --output-on-failure -R '^(docs\.budgets(\.controls)?|libretro\.observe(\.controls)?)$'`: fresh exit 0.
- `sh scripts/emscripten.sh build/ci`: fresh exit 0; emitted artifact, exact two-case inventory, 14 parity frames, replay and isolation.

All run URLs are N/A-local. The source did not change between full workflow receipts and the fresh scope checks. The CLI generated covered_files/covered_digest after this review; no digest was handwritten. Hosted checks must pass on this candidate's own head before merge. Combined-branch passes and failures remain distinct evidence. All denied/spent host scopes and nine deferred v3.0 requirements remain unchanged. No owner validation is required for these offline criteria.
