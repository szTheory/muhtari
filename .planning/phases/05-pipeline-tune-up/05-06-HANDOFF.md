# Central policy migration: measured allocation halt

Plan05-06 is halted during Task1, with0/3 tasks complete. Read05-06-SUMMARY.md,
the plan, AGENTS.md,05-01/05-05 summaries and current allocations before recovery.

Starting head5535b57168ef31a8993c28646920a8b88ed0e98a; branch phase/05-pipeline-tune-up.
Only task commit df6b0959dfbb31f843ada7fe3cbfc7e1b1f24c25 exists: RED central-reason
test, classifier RED_EVIDENCE_OK. No GREEN commit exists. The plan ledger is
`.git/gsd-plan-head-before-05-06` and contains the starting head.

Retain all uncommitted edits in quality.json, tests/cmake/quality.cmake,
tests/cmake/perf_compare.cmake and tests/perf/tests.cmake. They are the passing
draft, not production acceptance. Reverting only the draft would expose the intentional RED.
Do not blanket restore/reset/stash or resume accepted05-05/Windows work.

Last draft checks: all7 perf.gate cases plus policy.quality.controls pass, exit0;
gate_count observes14888. Original actual14841 rose47; quality967/920 and total
14888/14853 overrun. Fixed B12885/S2/N1970/C14853 and all allocations remain unchanged.
The BE12 is not reserve. First draft14892 fell4 through shared validation extraction;
no further savings are credited. No allocation increase is authorized.

Task2 compatibility migration/controls and Task3 real caller cutover/store deletion
are unstarted. Both allow files and existing caller arguments remain. Full ci/library
and hosted migration acceptance are unrun; no current run URL exists. Do not reuse
prior05-05 green URLs as this draft's acceptance. No push was made.

Evidence records live only under ignored build/plan-evidence; exact commands and
statuses are in SUMMARY. WINDOWS16/17 track unverified acceptance and the overrun.
The POKEY migration is spent in origin/main and current ledger; do not renew its reason.
Draft reader completeness, especially trusted policy wiring and deletion/identity behavior,
must be assessed before any GREEN claim. Preserve exact arithmetic/equality controls.

Next supported command: `$gsd-plan-phase 5 --gaps`. New evidence is the measured
construction overrun. Required scope decision: fit a fault-preserving migration inside
quality920/C14853, or explicitly revise the bounded migration scope. No cap increase,
BE transfer, hidden exclusions or control/comment removal is authorized. State remains
5/9 complete;05-07 depends on the unresolved05-06 halt. Do not execute another GSD
step in this session.
