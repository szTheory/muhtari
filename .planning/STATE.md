---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
status: ready_to_plan
stopped_at: Phase 7 verified complete; next separate command is gsd-ship 7
last_updated: "2026-10-24T15:56:23.485Z"
state_head: 36a9b7f14153b65fa86dfb44b593f1ebe8e7c2a6
progress:
  total_phases: 7
  completed_phases: 3
  total_plans: 28
  completed_plans: 28
  percent: 43
last_activity: 2026-10-24
total_phases: 7
current_phase: 08
current_phase_name: Controllers at the pins
current_plan: Not started
last_activity_desc: Phase 7 passed 35/35 must-haves and all six SAVE requirements; shipping is next
---

# Project State

## Project Reference

See PROJECT.md, REQUIREMENTS.md, ROADMAP.md and preparation/DECISIONS.md.
Core value: 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.
Current focus: ship the verified Phase 7 branch with `$gsd-ship 7` in a separate session.
The helper's next-phase pointer is parked at Phase 8; no Phase 8 work was started.

## Current Position

Phase 7 is complete: nine plans, SAVE-01–06 and35/35 verified must-haves.
Phase 5–7 are3/7 milestone phases with27/27 currently planned active slots complete.
Phases 8–11 remain unplanned. Runtime totals28/28 include the historical halted06-01
SUMMARY; it remains excluded from accepted active outcomes and is not a budget grant.

Next command: `$gsd-ship 7`.
Branch: `phase/07-battery-saves`. No next GSD step, PR or merge was invoked here.
Phase 6 merged in PR24 and package v0.5.0 is published; Phase 7 shipping is pending.

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

**Stopped at:** Phase 7 verified complete; next separate command is gsd-ship 7
Last session: 2026-10-24T15:56:23.485Z
Resume file: None

### Quick Tasks Completed

| Task | Description | Commit |
|---|---|---|
| 261023-jkd | Retire maintenance line budgets and adopt contract review | 2ab7e0d |
