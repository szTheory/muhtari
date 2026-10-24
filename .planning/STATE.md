---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
current_plan: 7
status: executing
stopped_at: Completed 07-06 runner cadence and controls; phase execution remains active
last_updated: "2026-10-24T03:39:12.874Z"
state_head: 528006ce793ccbbb4119b49b1d1d06d7b60311b9
progress:
  total_phases: 7
  completed_phases: 2
  total_plans: 28
  completed_plans: 25
  percent: 29
last_activity: 2026-10-23
total_phases: 7
current_phase: 07
current_phase_name: Battery saves across hosts
last_activity_desc: Phase 7 runner cadence and POSIX controls verified; three plans remain
---

# Project State

## Project Reference

See PROJECT.md, REQUIREMENTS.md, ROADMAP.md and preparation/DECISIONS.md.
Core value: 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.
Current focus: Phase 7 execution is active. Plans 07-01–06 prove evidence controls,
HSC/EEPROM hardware, robustness and runner saves/controls locally; host acceptance remains 07–09.

## Current Position

Current Plan: 7
Total Plans in Phase: 9

Phase 6 passed33/33 substantive truths: BOARD-01–03 and all9 active plans02–10
are complete. Phase 5 and6 are2/7 milestone phases,18/27 currently planned active
slots; Phase 7 has six completed plans and three pending plans. Phases 8–11 remain unplanned. Historical halted plans remain excluded
with their original records preserved.

Next command: `$gsd-execute-phase 7`.
No next GSD step was invoked.

## Phase 7 Execution

Six plans are committed; full local CI 637+6, exact inventories 637/6 and hygiene 24 pass.
Actual macOS arm64 ZIP sum 226048 <=399937. No whole SAVE requirement is complete,
no fixture is admitted, and no review-fix or gap round is consumed. Ten assumption
flags remain unresolved. Commands, exit statuses and N/A-local URLs live in 07-01–06-SUMMARY.md.

07-01 preserves actual producer/join authority with only named fixture and exact3-to-4
annotation relations.07-02 adds API5/ABI1/behavior4, HSC/firmware and a frozen API4 client.
07-03 implements RIOT data-tick serial EEPROM, exact deadlines/reset and no-poll completion.
Historical no-EEPROM hashes retain explicit topology; default metadata has real-pin tests.
07-04 passes1000-case lifecycle/observer properties and bounded4096-action I2C replay.
Local UBSan/libFuzzer3236 runs/61s passes; ASan spins before main in runtime startup.
07-05 implements opt-in exact raw import, final dirty replacement, CLI/movie provisioning,
copied firmware and actual-main/fresh-process byte and failure tests. No clean rewrite.

07-06 adds bounded cadence, synchronized requests, real POSIX child controls and expanded
failure/commit-point tests. Native Windows tests are implemented but runtime proof remains 07-09.
07-07 owns libretro;
07-08 owns fixture admission/real fresh-process RetroArch;07-09 owns final native/BE,
Windows, WASM, normal ASan/nofp,10000-case, hosted and six-native archive acceptance.

Research/checked plans cover SAVE-01–06 and D-01–18; decision gate18/18 and post-plan
coverage24/24 passed. Planning evidence remains in the phase planning artifacts.
No owner UAT or hardware request is introduced; final hosted evidence is still required.

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

**Stopped at:** Completed 07-06 runner cadence and controls; phase execution remains active

Last session: 2026-10-24T03:39:12.788Z
Resume file: .planning/phases/07-battery-saves/07-07-PLAN.md

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
| Phase 07 P01 | 3 min | 2 tasks | 3 files |
| Phase 07 P02 | 11 min | 3 tasks | 22 files |
| Phase 07 P03 | 15 min | 3 tasks | 22 files |
| Phase 07 P04 | 11min | 2 tasks | 9 files |
| Phase 07 P05 | 11min | 3 tasks | 12 files |
| Phase 07 P06 | 10min | 3 tasks | 12 files |
