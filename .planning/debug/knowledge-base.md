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

## windows-digest-controls — Native Windows path and text boundaries prevent complete controls
- **Date:** 2026-10-22
- **Error patterns:** digest valid preview failed, no such file or directory, invalid drive path, required run identity, CRLF, control timeout
- **Root cause(s):** Native PATH was interpolated as a POSIX/CMake list; drive-rooted output was prefixed with PWD; native jq emitted CRLF into shell fields; complete native arm64 work exceeded the unmeasured30s bound.
- **Fix:** Native path_list_prepend, shell directory resolution, jq binary output, retained platform-boundary cases and measured provisional207s control bound.
- **Files changed:** scripts/digest.sh, tests/policy/digest.cmake, tests/policy/tests.cmake, docs/maintaining.md
- **Why not caught:** POSIX local controls/Linux preview missed native PATH/drive/CRLF behavior and complete Windows timing.
- **Recurrence guard:** policy.digest.controls semicolon-PATH, CRLF proxy on POSIX/real native jq on Windows, relative/hyphen/CDPATH cases, all existing privilege/API/report assertions; separate image timing provenance in registration and resolved session.
---

## preplan-lightgun-evidence — Expanded work retains an older deadline and an outer wrapper truncates complete evidence
- **Date:** 2026-10-25
- **Error patterns:** input.lightgun timeout2s, isolated pass, full inventory incomplete, outer timeout124, missing independent base packages
- **Root cause(s):** Matrix expanded714→1484 instances without recalibration; valid complete co-scheduled work measured2.1942s. Separate60s wrapper conflicted with allowed130s publication work. Exact historical host pressure remains unknown.
- **Fix:** Provisional5s from ceil(2×2.1942), same scenarios/assertions/scales; GSD config0 selects supported foreground/no-outer-timer execution for the one-shot workflow, preserving all659 per-test deadlines; separate focused reports and fresh complete producer acceptance.
- **Files changed:** tests/input/tests.cmake, docs/maintaining.md, .planning/config.json, .planning/preparation/GSD-HANDOFF.md
- **Why not caught:** The retained timing provenance described a smaller workload; an isolated pass neither calibrated full-suite work nor repaired the failed inventory.
- **Recurrence guard:** Existing measured registration and docs/maintaining.md require complete-work timing, effective explicit CTest worker controls and distinct focused JUnit. Exact committed current653+6/unchanged base652+6 and all six ZIPs pass; hosted calibration remains the next plan.
---
