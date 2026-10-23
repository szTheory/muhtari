---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
current_phase: "07"
current_phase_name: Battery saves across hosts
current_plan: Not started
status: ready_to_plan
stopped_at: Phase 6 verified complete; next separate command is gsd-ship 6
last_updated: "2026-10-23T22:56:28Z"
state_head: 65ae7c0aa8db2caf3ed13242e1da44a5638e46c3
last_activity: 2026-10-23
last_activity_desc: Phase 6 passed 33/33; BOARD-01–03 complete; ship remains separate
total_phases: 7
progress:
  total_phases: 7
  completed_phases: 2
  total_plans: 18
  completed_plans: 18
  percent: 29
---

# Project State

## Project Reference

See PROJECT.md, REQUIREMENTS.md, ROADMAP.md and preparation/DECISIONS.md.
Core value: 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.
Current focus: ship verified Phase 6. Phase 7 has not started.

## Current Position

Phase 6 passed33/33 substantive truths: BOARD-01–03 and all9 active plans02–10
are complete. Phase 5 and6 are2/7 milestone phases,18/18 currently planned active
slots; later phases remain unplanned. Historical halted plans remain excluded
with their original records preserved.

Next command: `$gsd-ship 6`.
Draft PR24 remains unmerged: https://github.com/szTheory/muhtari/pull/24 .
No next GSD step was invoked.

## Phase 6 Acceptance

Source25919fe and identical-tree PR merge2fe713c were accepted against actual
base617dfa7. API4/ABI1/behavior3; all three owned diagnostics and nine fixture
host observations; identified AM/OM identity/equivalence; mandatory resolver
maps13/11/21; physical2048-byte Fractalus RAM; bounded hostile/lifecycle checks.

Local CI612+6 and hygiene24 pass. Full native/PowerPC inventories are612+6 each,
with47 identical equality rows. Hosted fuzz,10000-case properties, full vectors,
WASM and actual RetroArch pass at unchanged required/advisory status.
Worst three-component ZIP375270 <=399937; independent base399385 also fits.
Measured402586-byte failure remains in plan09 history; ZIP9 and MSVC Release
size preference resolve it without a byte grant. Test-card3837262 Ir/frame equals
both independent base repeats.

`gh run view <id> --json status,conclusion,jobs` exits0/success for:

- CI40031550434: https://github.com/szTheory/muhtari/actions/runs/40031550434
- Nightly40031550533: https://github.com/szTheory/muhtari/actions/runs/40031550533
- Release40031550534 attempt2: https://github.com/szTheory/muhtari/actions/runs/40031550534

Both authenticated quality joins pass with raw legacy policy preserved.
The hosted-retirement todo is completed. Temporary release422369508 and
tagrehearsal-40031550534-2 have exact-ID/tag/ref404 and complete-list absence.
See06-09-SUMMARY.md and06-VERIFICATION.md for commands, exits and run URLs.

## Decisions and Limits

Core state remains deterministic, bounded and instance-owned. Fractalus RAM is
volatile; higher achievement-map mirrors remain documented holes. Diagnostics
do not establish untested retail playability, PAL completion or new hardware
measurements. Three descriptorless flags remain unresolved metadata with separate
agent intent review. No owner UAT or hardware request remains.

Maintenance-line admission remains retired; no replacement score or allocation
is introduced. Archive399937, old predicates/pins and ordinary guards persist.
Historical06-01 and its665/654-line constructions remain failed history, not grants.
Review/gap allowances were not reset; no blocking review finding exists.

## Prior Acceptance and Seeds

Phase 5 shipped in PR20 at6d57d69; its20/20 verification and lifecycle evidence
remain in its artifacts. Latest published package remainsv0.4.0; Phase 6 shipping
and post-merge release checks are the next separate step.
WR-01/COST-01, SEED-009 and v1.0 audit deferrals retain their scope.
Raw monthly cost totals remain untrusted; no global hosted-cost reduction is claimed.

The completion helper counted historical06-01 as10/9 and interpreted command
strings as missing paths. Active counts are reconciled above; scanner caveats
do not override the independent103-file fingerprint and33/33 verification.
No required acceptance is missing.

## Session Continuity

Last session:2026-10-23. Stopped after Phase 6 execution and verification.
Resume file:None. Next separate command:`$gsd-ship 6`.

### Quick Tasks Completed

| Task | Description | Commit |
|---|---|---|
| 261023-jkd | Retire maintenance line budgets and adopt contract review | 2ab7e0d |
