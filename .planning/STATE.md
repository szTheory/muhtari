---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
current_plan: 4
status: executing
stopped_at: Completed 07-03 serial EEPROM; phase execution remains active
last_updated: "2026-10-24T03:00:23.137Z"
state_head: 1f6ca3c3d16937403c8b92693d395b5fd1fa0edf
progress:
  total_phases: 7
  completed_phases: 2
  total_plans: 28
  completed_plans: 22
  percent: 29
last_activity: 2026-10-23
total_phases: 7
current_phase: 07
current_phase_name: Battery saves across hosts
last_activity_desc: Phase 7 evidence HSC and serial EEPROM verified; six plans remain
---

# Project State

## Project Reference

See PROJECT.md, REQUIREMENTS.md, ROADMAP.md and preparation/DECISIONS.md.
Core value: 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.
Current focus: Phase 7 execution is active. Plans 07-01–03 prove evidence controls,
HSC and serial EEPROM hardware; robustness and host acceptance remain in plans 04–09.

## Current Position

Current Plan: 4
Total Plans in Phase: 9

Phase 6 passed33/33 substantive truths: BOARD-01–03 and all9 active plans02–10
are complete. Phase 5 and6 are2/7 milestone phases,18/27 currently planned active
slots; Phase 7 has three completed plans and six pending plans. Phases 8–11 remain unplanned. Historical halted plans remain excluded
with their original records preserved.

Next command: `$gsd-execute-phase 7`.
Phase 6 shipping PR: https://github.com/szTheory/muhtari/pull/24 .
The ship step confirms the exact-head CI, squash merge and release-PR checks before reporting its outcome.
No next GSD step was invoked.

## Phase 7 Planning

Execution evidence: 07-01 adds only the battery fixture name and exact 3-to-4 sample
annotation relation. Actual producer recursion and unchanged authenticated join controls
pass, including base/run/identity faults. Local CI passes612+6; exact JUnit inventories
and hygiene pass. No SAVE requirement is completed, no fixture is admitted, no hosted
acceptance is claimed and no review-fix or gap round is consumed. See07-01-SUMMARY.md
for commands, exits and local evidence identity. Ten assumption flags remain unresolved.

Plan07-02: API5/ABI1/behavior4, HSC/firmware/battery, frozen API4 client; CI619+6, ZIP218325 <=399937.
Old digests retained; commands/exits N/A-local in07-02-SUMMARY.md. Serial/host/whole SAVE acceptance pending.

Plan07-03: actual ORA/DDRA serial EEPROM on either port; exact due/reset and no-poll
semantics. CI626+6, exact inventories626/6, hygiene24 and ZIP223038 <=399937 pass.
Old no-EEPROM frame pins retained with explicit topology; default metadata executes
separate real-pin tests. Commands/exits N/A-local in07-03-SUMMARY.md; no review/gap
round consumed. Robustness and host acceptance remain pending; SAVE IDs stay open.

Research and nine plans in nine dependent waves cover SAVE-01–06 and D-01–18.
The independent plan checker passed after one focused revision; no findings remain.
The decision gate passed18/18 and post-planning coverage passed24/24.
Verification command probes report25 stated failure directions, no errors; their
path parser does not interpret CMake/CTest commands. Structure and hygiene checks
pass. No hosted run was started for this planning step and no save requirement is
completed. Archive feasibility, actual fixture admission, native Windows controls
and fresh-process RetroArch persistence remain mandatory execution evidence.

Planning evidence (all exit0; run URL not applicable for local artifact checks):

- `node ~/.codex/gsd-core/bin/gsd-tools.cjs query check.decision-coverage-plan .planning/phases/07-battery-saves .planning/phases/07-battery-saves/07-CONTEXT.md` — 18/18.
- `node ~/.codex/gsd-core/bin/gsd-tools.cjs check gap-analysis-plan-post .planning/phases/07-battery-saves 'SAVE-01, SAVE-02, SAVE-03, SAVE-04, SAVE-05, SAVE-06' --raw` — 24/24.
- `git diff --check` and `scripts/hygiene.sh --tree` — no findings.

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
remain in its artifacts. The package before this ship step is v0.4.0. Phase 6 shipping and post-merge
release checks belong to this step; release-please owns the next package version.
WR-01/COST-01, SEED-009 and v1.0 audit deferrals retain their scope.
Raw monthly cost totals remain untrusted; no global hosted-cost reduction is claimed.

The completion helper counted historical06-01 as10/9 and interpreted command
strings as missing paths. Active counts are reconciled above; scanner caveats
do not override the independent103-file fingerprint and33/33 verification.
No required acceptance is missing.

## Session Continuity

**Stopped at:** Completed 07-03 serial EEPROM; phase execution remains active

Last session: 2026-10-24T03:00:23.053Z
Resume file: .planning/phases/07-battery-saves/07-04-PLAN.md

### Quick Tasks Completed

| Task | Description | Commit |
|---|---|---|
| 261023-jkd | Retire maintenance line budgets and adopt contract review | 2ab7e0d |

## Decisions

- [Phase 07]: The runtime counts 28 plans and 19 summaries including superseded 06-01; accepted active completion is 18/27 across phases 5–7. Phase 7 has nine checked plans and no completed save requirements. Percent reports two completed phases of seven.
- [Phase 07]: HSC load stages firmware in a fixed unpublished instance buffer to avoid compiler stack helpers while keeping the one raw battery span authoritative.
- [Phase 07]: Historical frame chains explicitly select no EEPROM; default metadata attachment is separately executed through real RIOT waveform tests, preserving every old digest.

## Performance Metrics

| Plan | Duration | Tasks | Files |
|------|----------|-------|-------|
| Phase 07 P01 | 6 min | 2 tasks | 3 files |
| Phase 07 P01 | 3 min | 2 tasks | 3 files |
| Phase 07 P02 | 11 min | 3 tasks | 22 files |
| Phase 07 P03 | 15 min | 3 tasks | 22 files |
