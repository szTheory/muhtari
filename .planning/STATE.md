---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
current_plan: 9
status: executing
stopped_at: Completed 07-08 fixture and actual hosted persistence proof; final acceptance remains 07-09
last_updated: "2026-10-24T09:55:05.871Z"
state_head: 1e58f1bace9fa4946abccb62bccfbfdd01b939f9
progress:
  total_phases: 7
  completed_phases: 2
  total_plans: 28
  completed_plans: 27
  percent: 29
last_activity: 2026-10-24
total_phases: 7
current_phase: 07
current_phase_name: Battery saves across hosts
last_activity_desc: Phase 7 actual hosted persistence verified; final acceptance plan remains
---

# Project State

## Project Reference

See PROJECT.md, REQUIREMENTS.md, ROADMAP.md and preparation/DECISIONS.md.
Core value: 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.
Current focus: Phase 7 execution is active. Plans 07-01–08 prove evidence controls,
HSC/EEPROM hardware, robustness, runner saves/controls, loaded libretro behavior,
owned fixture admission and actual fresh-process RetroArch persistence.
Final archive and portability acceptance remain07-09.

## Current Position

Current Plan: 9
Total Plans in Phase: 9

Phase 6 passed33/33 substantive truths: BOARD-01–03 and all9 active plans02–10
are complete. Phase 5 and6 are2/7 milestone phases with18 completed active plans;
Phase 7 has eight completed plans and one pending plan, making26/27 currently planned
active slots complete. Phases 8–11 remain unplanned. Historical halted plans remain excluded
with their original records preserved.

Next command: `$gsd-execute-phase 7`.
No next GSD step was invoked.

## Phase 7 Execution

Eight plans are committed; final local CI641+6 and exact inventories641/6 pass.
Hosted hygiene passes; final local23 hygiene checks pass and the unchanged complete
tree scan exits0 standalone after two60-second test timeouts under host load.
The owned battery fixture is admitted through actual producer/join evidence.
No whole SAVE requirement is complete and no review-fix or gap round is consumed.
Ten assumption flags remain unresolved. Commands, exits and run URLs live in07-01–08-SUMMARY.md.

07-01 preserves actual producer/join authority with only named fixture and exact3-to-4
annotation relations.07-02 adds API5/ABI1/behavior4, HSC/firmware and a frozen API4 client.
07-03 implements RIOT data-tick serial EEPROM, exact deadlines/reset and no-poll completion.
Historical no-EEPROM hashes retain explicit topology; default metadata has real-pin tests.
07-04 passes1000-case lifecycle/observer properties and bounded4096-action I2C replay.
Local UBSan/libFuzzer3236 runs/61s passes; early local ASan spun before main in runtime startup.
Actual hosted ASan now passes inCI40080473918.
07-05 implements opt-in exact raw import, final dirty replacement, CLI/movie provisioning,
copied firmware and actual-main/fresh-process byte and failure tests. No clean rewrite.

07-06 adds bounded cadence, synchronized requests, real POSIX child controls and expanded
failure/commit-point tests. Actual Windows x64/arm64 current/base inventories now pass;
final source acceptance remains07-09.
07-07 publishes final core-owned SAVE_RAM before execution, freezes late storage requests,
copies optional HSC firmware and proves actual HSC descriptors through the pinned resolver.
The .info save/optional-HSC claims remain gated until final acceptance.
07-08 proves actual1/1 then2/2 HSC/EEPROM hardware counters, both34816-byte media
snapshots and independent firmware5A/stubFF pixel fields through fresh runner and
RetroArch processes. Exactly two hosted cases,300 lines and50-second launch bound remain.
CI40080473918 has six native current/base641+6, RetroArch, ASan, nofp and equality green:
https://github.com/szTheory/muhtari/actions/runs/40080473918 . Overall CI correctly fails
Windows archive407930>399937 and the pre-fix WASM getpid compilation. The WASI no-PID
exclusive-create fix is committed and existing consumers exercise that branch locally.
07-09 must resolve the unchanged archive ceiling and collect final actual WASM proof;
it owns final native/BE,
Windows, WASM, normal ASan/nofp,10000-case, hosted and six-native archive acceptance.

Research/checked plans cover SAVE-01–06 and D-01–18; decision gate18/18 and post-plan
coverage24/24 passed. Planning evidence remains in the phase planning artifacts.
No owner UAT or hardware request is introduced; final source acceptance is still required.

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

**Stopped at:** Completed 07-08 fixture and actual hosted persistence proof; final acceptance remains 07-09

Last session: 2026-10-24T09:55:05.758Z
Resume file: .planning/phases/07-battery-saves/07-09-PLAN.md

### Quick Tasks Completed

| Task | Description | Commit |
|---|---|---|
| 261023-jkd | Retire maintenance line budgets and adopt contract review | 2ab7e0d |

## Decisions

- [Phase 07]: The runtime counts28 plans and27 summaries including superseded06-01; accepted active completion is26/27 across phases5–7. Phase7 has nine checked plans and no completed save requirements. Percent reports two completed phases of seven.
- [Phase 07]: HSC load stages firmware in a fixed unpublished instance buffer to avoid compiler stack helpers while keeping the one raw battery span authoritative.
- [Phase 07]: Historical frame chains explicitly select no EEPROM; default metadata attachment is separately executed through real RIOT waveform tests, preserving every old digest.
- [Phase 07]: Independent hardware counters and complete raw snapshots plus independent glyphs prove the exercised fresh-process save path; whole requirements await final acceptance.

## Performance Metrics

| Plan | Duration | Tasks | Files |
|------|----------|-------|-------|
| Phase 07 P01 | 3 min | 2 tasks | 3 files |
| Phase 07 P02 | 11 min | 3 tasks | 22 files |
| Phase 07 P03 | 15 min | 3 tasks | 22 files |
| Phase 07 P04 | 11min | 2 tasks | 9 files |
| Phase 07 P05 | 11min | 3 tasks | 12 files |
| Phase 07 P06 | 10min | 3 tasks | 12 files |
| Phase 07 P07 | 47min | 3 tasks | 8 files |
| Phase 07 P08 | 304min | 3 tasks | 23 files |
