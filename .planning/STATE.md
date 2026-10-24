---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
current_plan: 4
status: executing
stopped_at: Completed 08-03-PLAN.md
last_updated: "2026-10-24T20:19:28.512Z"
last_activity: 2026-10-24
last_activity_desc: Completed 08-03 timed driving and owned motion fixture; full local CI passes 643+6
state_head: 49480033ee27af834f7fa3e17d2eb6a88971cce3
progress:
  total_phases: 7
  completed_phases: 3
  total_plans: 42
  completed_plans: 31
  percent: 43
total_phases: 7
current_phase: 08
current_phase_name: Controllers at the pins
---

# Project State

## Project Reference

See PROJECT.md, REQUIREMENTS.md, ROADMAP.md and preparation/DECISIONS.md.
Core value: 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.
Current focus: Phase 8 execution; 08-01 proves the controller evidence prerequisite,
08-02 publishes API6, and 08-03 proves timed driving with an admitted owned numeric executable.
The remaining 11 sequential plans own replay, host input, other device models and final acceptance.
All five INPUT requirements remain incomplete. Phase 7 details
below are historical acceptance/shipping records.

## Current Position

Current Plan: 4
Total Plans in Phase: 14
Progress: [████░░░░░░] 43%

Phase 7 is complete: nine plans, SAVE-01–06 and35/35 verified must-haves.
Phase 5–7 are3/7 milestone phases with30/41 currently planned active slots complete.
Phase 8 has3/14 plans complete; Phases 9–11 remain unplanned. Runtime totals31/42 include the historical halted06-01
SUMMARY; it remains excluded from accepted active outcomes and is not a budget grant.

Status: Ready to execute
Last activity: 2026-10-24 — Completed 08-03 timed driving and owned motion fixture; full local CI passes643+6
Next command: $gsd-execute-phase 8
Branch: `phase/08-controllers-at-the-pins`. No Phase 8 pull request yet; Phase 7 merged in PR #28.

Planning checks: `check.decision-coverage-plan` exits0 with20/20 covered;
`check gap-analysis.plan-post` exits0 with25/25 source items covered. All38 automated
commands pass `sh -n`; the failure-direction probe reports no findings. The path probe
does not interpret CMake command forms; the independent checker reviewed actual/planned
test registrations. These are local planning checks (run URL: N/A-local), not runtime
or hosted acceptance. Archive fit and operating cost remain execution gates; hardware
calibration remains deferred. No review-fix or gap-execution round was used here.

Historical Phase 7 ship checks: `cmake --workflow --preset ci` exits 0 (641+6 tests),
`git diff --check` exits 0, and `verification.status` exits 0 with `passed`.
The existing CI/nightly run URLs below were rechecked as completed/success.
The ship-note commit deliberately retains CI triggering so the final PR head has a run.

## Phase 7 Acceptance

API5/ABI1/behavior4; one core-owned raw battery span, HSC firmware/stub boundaries,
timed either-port EEPROM, exact runner saves and actual fresh-process RetroArch persistence.
Nine committed plan summaries retain commands, exits, run URLs and failed experiments.
Review is clean: CR-01/02/03 fixed, both allowed review-fix rounds used, gap round unused.
Descriptor-less flags retain their original serialized history; intent review is recorded.
No owner UAT, identity consent or hardware request remains.

Final tested source54485d6 contains reviewed production90b9b7a and only the narrow
acceptance-test correction applying existing native1/PPC13 timeout scaling to the
closed-pipe child wait. Actual QEMU exit/raw-byte checks pass; hung/wrong-exit controls
remain bounded. No product behavior, workload, skip, quota or comparison predicate changed.

`gh run view <id> --json status,conclusion,headSha,url` exits0 with completed/success
and exact source54485d630c421716d51a8d7919ded429742acf4a for:

- CI40104749040 attempt2: https://github.com/szTheory/muhtari/actions/runs/40104749040
- Nightly40104750452: https://github.com/szTheory/muhtari/actions/runs/40104750452

All six current/base native641+6 and full native/PPC641+6 pass.69-row equality
retains47 old observations except five named behavior annotations3->4.
WASM580, ASan626, nofp, seeded10000-case properties, full vectors258 and fuzz10 pass.
Actual Windows console/replacement and exactly two RetroArch cases pass. Both fresh
battery launches independently judge all34816 bytes and pixels: counters1/1->2/2,
supplied marker5A->stubFF, with isolation and oracle fault controls.

Actual current three-component ZIP sums are284545/277824/230503/232671/371070/393952
for Linuxarm/Linuxx64/macOSarm/macOSx64/Windowsarm/Windowsx64. Worst genuine parent
is393994; all fit the unchanged399937 ceiling. Authenticated joins retain raw policy,
manifest/sample authority, independent parent6784e47 and required/advisory status.
Failed size experiments, PPC unscaled-wait failure and unchanged-source policy/local
timeout retries remain documented; no failed attempt is counted as a pass.

The independent verifier ran37 named behavioral spots, restored the complete local
workflow (exit0,641+6), checked exact inventories and actual hosted artifacts, and
passed35/35 must-haves. Its120-file CLI fingerprint and full evidence are in
`phases/07-battery-saves/07-VERIFICATION.md`. Covered inputs remain unchanged.

The completion helper emitted nine scanner warnings because it treats command strings
and one-time generated fixture-script references as missing durable files. These are
not missing implementation or verification evidence; the independent fingerprint,
actual consumers and inventories resolve them. Helper preservation fields were
reconciled here; no verification debt or required acceptance is missing.

## Decisions and Limits

Core state remains integer-only, bounded, instance-owned and allocation-free after load.
Battery layout1 is absent,2048,32768 or34816 bytes; attachment freezes at successful load.
Fixed5ms EEPROM timing is a compatibility model, not a hardware measurement.
The ROM-free stub supplies signatures only; AtariVox speech, full OpenHSC, save states,
PAL completion, later controllers and broader retail playability remain outside this phase.
Historical no-EEPROM frame pins retain explicit topology; automatic save metadata has
separate executed hardware tests. Archive/floor/performance guards remain unchanged.

Phase 5 verification20/20 and Phase 6 verification33/33 remain in their own artifacts.
WR-01/COST-01, SEED-009 and v1.0 audit deferrals retain scope. Maintenance-line admission
remains retired; no replacement quota or global hosted-cost reduction is claimed.

## Session Continuity

**Stopped at:** Completed 08-03-PLAN.md
Last session: 2026-10-24T20:19:28.327Z
Resume file: None

### Quick Tasks Completed

| Task | Description | Commit |
|---|---|---|
| 261023-jkd | Retire maintenance line budgets and adopt contract review | 2ab7e0d |

## Performance Metrics

| Plan | Duration | Tasks | Files |
|------|----------|-------|-------|
| Phase 08 P01 | 6min | 2 tasks | 3 files |
| Phase 08 P02 | 7min | 3 tasks | 16 files |
| Phase 08 P03 | 10min | 2 tasks | 12 files |

## Decisions

- [Phase 08]: Preserve separate fixture admission, device behavior and evidence comparison authorities.
- [Phase 08]: Admit only the exact 4-to-5 annotation transition alongside existing equality and historical adjacent pairs.
- [Phase 08]: Preserve the old input validator and Auto fallback independently of extended device support.
- [Phase 08]: Normalize complete extended scalar fields with fixed-size copies to preserve the restricted core link surface.
- [Phase 08]: Keep motion timing in the existing bus owner with cached idle deadlines and fixed instance state.
- [Phase 08]: Decode the diagnostic selector before mailbox initialization can consume the short handoff frame.
- [Phase 08]: Exercise actual admitted controller rows through independent new-row and later-base producer controls.
