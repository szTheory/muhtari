---
phase: 07
review: 07-REVIEW.md
titles: json
findings:
  - id: CR-03
    severity: critical
    disposition: fixed
    title: "Resolving Auto to unsupported header codes rejects the whole frontend input update"
  - id: CR-01
    severity: critical
    disposition: fixed
    title: "Broken stdout pipe terminates the runner before its required final save"
  - id: CR-02
    severity: critical
    disposition: fixed
    title: "A refused late save-device request drops joystick A-button fire"
open: 0
total: 3
recorded: 2026-10-24T14:14:30.461Z
---

# Phase 07: Code Review Disposition

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| CR-03 | critical | fixed | 07-REVIEW-FIX.md (not in the current review) |
| CR-01 | critical | fixed | 07-REVIEW-FIX.md (not in the current review) |
| CR-02 | critical | fixed | 07-REVIEW-FIX.md (not in the current review) |

Dispositions: `open` (recorded, not yet triaged), `fixed`, `skipped`, `deferred`.
Set `deferred` by hand and put the reason in the Source cell; both are preserved. A `|` in the reason is kept as prose and escaped on the next run.
Re-running the gate keeps every row it can. A row the current review no longer reports is kept and its Source cell flagged, so a finding does not leave this record silently. ONE exception: when a finding id is REUSED by a different finding, the earlier decision cannot keep a row — the id is taken — and it is dropped. A RECORDED decision (anything but `open`) is named on the console when that happens; a row still at `open` is replaced silently, because `open` records no decision to lose.

## Acceptance-test portability correction after clean review

The three finding outcomes above remain fixed. The clean production review covers
90b9b7a; both allowed review-fix rounds are used and the gap allowance remains unused.

Actual full PowerPC execution in Nightly40103107489 exposed the new closed-pipe
test's hardcoded five-second child wait bypassing the existing project scale13.
Two during-output children were killed by that harness wait before final saving;
this was new acceptance-test portability evidence, not a new product finding.
Correction54485d6 wires the existing scale only into that wait through the existing
CMake consumer. Native1, PPC13, outer deadlines, other waits, workloads and all
exit/full34816-byte assertions are unchanged. No third review-fix round, gap/replan,
product semantics change, quota or timeout grant was used.

The real isolated QEMU consumer passes all four cases in4.92s, exit0; representative
hung-child and wrong-exit controls fail with bounded cleanup, and the asserting
probe exits0. Full local CI641+6 and hygiene24 pass. Final full PowerPC controls
pass in7.82s, with complete native/PPC641+6 inventories, floor and69-row equality.

Final acceptance checkout is54485d630c421716d51a8d7919ded429742acf4a, genuine
independent parent6784e4758ddb8e96a6048c986ff0e6b5e4248c01. Actual final
[CI40104749040](https://github.com/szTheory/muhtari/actions/runs/40104749040)
and [Nightly40104750452](https://github.com/szTheory/muhtari/actions/runs/40104750452)
are successful, including all six current/base package sums within399937,
actual frontend/Windows controls and WASM580. CI's first Macx64 policy timeouts
are preserved; an identical-head job retry passes without source/bound changes.
Commands, exits, complete measurements and historical pending-proof closure are
in07-09-SUMMARY.md. Formal phase verification/completion remain with the parent.
