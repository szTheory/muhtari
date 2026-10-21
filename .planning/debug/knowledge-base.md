# GSD Debug Knowledge Base

Resolved debug sessions. Used by `gsd-debugger` to surface known-pattern hypotheses at the start of new investigations.

---

## powerpc-equality-relative-paths — Relative evidence directories produce an empty member list
- **Date:** 2026-10-21
- **Error patterns:** lane_equality, members of NATIVE, empty member list, relative evidence directories, absolute replay passes
- **Root cause(s):** REAL_PATH results were used only for duplicate detection; file(GLOB) still consumed the original relative arguments. Hosted callers used relative paths while controls used only absolute paths.
- **Fix:** Reuse canonical NATIVE/BE arguments throughout the comparator; run every existing positive/fault case with both path forms. Remove completed temporary ABI diagnosis and document the path contract.
- **Files changed:** tests/cmake/lane_equality.cmake, tests/policy/tests.cmake, .github/workflows/be-equality.yml, docs/testing.md
- **Why not caught:** policy.lane_equality.controls supplied only absolute WORK paths and missed the hosted caller shape.
- **Recurrence guard:** tests/policy/tests.cmake:policy.lane_equality.controls now proves sixteen absolute/relative valid/fault cases through the real comparator in ci; comparator-only revert/reapply reproduces and removes the exact failure.
---
