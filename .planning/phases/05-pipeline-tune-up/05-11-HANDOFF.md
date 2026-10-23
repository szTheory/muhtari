# Replacement central policy migration: bounded construction halt

Plan05-11 is halted during Task1, with0/3 tasks complete. The existing RED
df6b095 belongs to superseded05-06 and remains an ancestor. No new task or GREEN
commit exists. Read05-11-SUMMARY.md, the plan and AGENTS.md before any recovery.

Branch phase/05-pipeline-tune-up; starting HEAD91c31654a5c655b3f1dd0aa99f53bcbfc8c1e462.
The plan ledger .git/gsd-plan-head-before-05-11 records that HEAD.
There is no isolated worktree. Preserve all retained draft files and the two
additional shared-reader/control edits. Never blanket restore/reset/stash them.

The six uncommitted implementation paths are quality.json,
tests/cmake/quality.cmake, tests/cmake/quality_join.cmake,
tests/cmake/perf_compare.cmake, tests/perf/tests.cmake and tests/policy/quality.cmake.
The last two new paths relative to the retained draft are quality_join.cmake and
the exact forged-SHA expectation in tests/policy/quality.cmake.

Final selected verification: cmake --preset ci followed by ctest --preset ci -R
'^(perf[.]gate[.]|policy[.]quality[.]controls$)' --no-tests=error --output-on-failure,
exit0, all8 cases. git diff --check and scripts/hygiene.sh --tree exit0.
Counter exit0 observes14879. The explicit jq numeric gate exits1/false.
Quality958/920 misses38 lines; total14879 exceeds strict14841 by38 and C14853 by26.
Net recovery from retained14888 is9 physical lines, including all helper/call/control costs.
Other components remain count368, BE193, digest447, fixes30, reserve0.
B12885/S2/N1970/C14853 and every allocation remain fixed; BE12 is not reserve.

Task1 extracts quality_bound and quality_observation into the existing quality.cmake.
The bound operation retains independent ordinary introductions, narrow structural gate
initialization, domain floor defaults and inherited unwaived values. Join authenticates
every original item before extrema. A forged smaller lane still rejects; its new
expected diagnostic checks the exact observed and expected SHA values. Required/advisory
status handling and actual bound judgement stay with their original callers.
All original tests remain; no comments, controls, inventory rows or stores were deleted.

This is a failed construction gate, not another semantic RED. The brief failed rerun
from CMake diagnostic line wrapping was corrected with whitespace-tolerant matching
of the same exact SHA values; final8-case verification passes. No GREEN is claimed.

Task2 and Task3 are unstarted. Both allow stores and production callers remain.
Full ci/library, generated-corpus compatibility migration, direct PR numeric perf/library/
quality, release-called CI and authentic current artifact joins are unrun.
Current migration run URL: unavailable. No push or dispatch occurred.
Earlier green URLs and accepted predecessor summaries cannot accept this draft.
WINDOWS18/19 record the new halt; historical16/17 and05-06 records remain unchanged.

Concrete alternatives require a scoped decision: retain the existing semantic stores
and explicitly defer their centralization, or approve a different fault-preserving
construction within the same allocation with measured savings before expansion.
No unrelated savings hunt, increased cap, allocation transfer, renewed reason or
automatic additional gap round is authorized. Compatibility/store-retirement forecasts
cannot finance the Task1 gate retroactively.

Next command in a fresh session: `$gsd-discuss-phase 5` to resolve that scope decision.
Do not run another GSD step now.05-07/08/09 and phase-wide TUNE-03/TUNE-04 remain pending.
