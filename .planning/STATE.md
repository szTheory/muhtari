---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
current_plan: 11
status: executing
stopped_at: 08-10 verified;08-11 ready
last_updated: "2026-10-25T00:54:53.359378+00:00"
last_activity: 2026-10-24
last_activity_desc: 08-10 accepted after648+6 local checks;08-11 ready
state_head: 6621779
progress:
  total_phases: 7
  completed_phases: 3
  total_plans: 42
  completed_plans: 38
  percent: 43
total_phases: 7
current_phase_name: Controllers at the pins
current_phase: 08
---

# Project State

## Project Reference

See PROJECT.md, REQUIREMENTS.md, ROADMAP.md and preparation/DECISIONS.md.
Core value: 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.
Current focus: Controllers at the pins. Plans01–10 are accepted; the first driving
slice runs through library, immutable runner replay and actual libretro/RetroArch.
A separately authorized DEC.61 review retired only the unsupported archive cutoff;
required authenticated measurements and package integrity remain. No INPUT
requirement is complete. Next is08-11 edge-clocked SNES2Atari and Mega7800.

## Current Position

Current Plan: 11
Total Plans in Phase: 14
Progress: [████░░░░░░] 43%

Phase8 has10/14 accepted plans;08-10 passes full local CI648+6 and package checks.
Runtime summarized totals are38/42, including historical halted06-01.
Accepted active progress is37/41;3/7 milestone phases remain complete (43%).
The runtime summary count is not an accepted-completion count.

Status: Executing Phase 08;08-11 ready
Last activity: 2026-10-24 —08-10 accepted; continuing the dependency-ready wave
Next command: $gsd-execute-phase 8
Branch: phase/08-controllers-at-the-pins. No Phase8 PR, merge or release.

08-10 implementation0f590fe and summary6621779 are accepted. Parent post-wave
full-ci gate exits0 (648+6), N/A-local. Connector-specific CX22/ST/Amiga cycles,
chronological PA7 and27 records/family pass all hosts; driving19 records persist.
Local ZIPs79860+83497+93116=256473 bytes; hygiene24 passes.

08-09 implementation2815568 and summary34e8e8e are accepted. Parent post-wave
full-ci gate exits0 (648+6), N/A-local. Seven-node keypad connectivity, exact2864
tick recovery,27 new records and all12 key mappings pass every deliverable.
Paddle19 records persist. Local ZIPs79429+82858+92800=255087 bytes; hygiene24 passes.

08-08 implementationf33ad71 and summary10a2354 are accepted. Parent post-wave
full-ci gate exits0 (648+6), N/A-local. Full-range Q32 paddle charge, independent
19-record analog diagnostic and three-host numbers/native/raw/PCM pass. Local
three-ZIP sum250408 bytes (77799+81122+91487); hygiene24 passes.

08-07 implementation9ca62b3 and summary1d204a7 are accepted. Parent post-wave
full-ci gate exits0 (648+6), N/A-local. Owned19-record light diagnostic, actual
three-host numbers/native/raw/PCM, fixture rebuild/admission and absolute mappings
pass. Local three-ZIP sum246732 bytes; calibration remains unmeasured.

08-06 implementation6fab3e0 and summary3e376b4 are accepted. Parent post-wave
`gsd-tools.cjs run-with-timeout 60 -- cmake --workflow --preset ci` exits0 (648+6),
run URL N/A-local. Native light timing and TIA latch corrections retain all69 prior
digests/numbers; only seven named revision4→5 metadata values change. Local three-ZIP
sum245942 bytes is authenticated by the actual producer. No hosted acceptance is
claimed for this intermediate slice.

Historical recovery tested source62fa325f665984fadc6324d65a49baed77fb31d2, genuine parentae71481227e38105ee51ca9e92f9cc5f85285b8d.
https://github.com/szTheory/muhtari/actions/runs/40139744520 attempt1 passed every required job, including quality and actual RetroArch;
native current/base647+6/646+6, hygiene24, ASan, nofp2, BE32 compilation and equality
pass. WASM585 passes. Complete CMake outcome audit and actual eight-lane join exit0.
Machine peaks401683/401727 remain reported above the retired historical cutoff.
No retry or evidence normalization was used.

Overall run conclusion is failure solely from advisory library2s pokey/rmtramdemo
timeouts; missing results remain unaccepted advisory evidence. They are recorded
without a proven-flake claim, retry or promotion into a new gate. The later cost
profiling seed does not block development. Dispatch performance proves same-head
repeatability, not phase-versus-main comparison. The armed PR+1% guard is unchanged.

The canonical05 summary preserves all historical cutoff/link/timeout failures and
adds explicit DEC.61 resolution. Defect51 is resolved by authorized policy retirement,
not a claim the old cap passed. Defect52 remains fixed and hosted-verified.
That recovery closed only05. All INPUT requirements and phase completion remain pending.
Future slices keep local full-ci/package evidence; final08-14 owns the full exact-source
hosted matrix, with earlier hosted probes for concrete platform/integration questions.

Planning checks: `check.decision-coverage-plan` exits0 with20/20 covered;
`check gap-analysis.plan-post` exits0 with25/25 source items covered. All38 automated
commands pass `sh -n`; the failure-direction probe reports no findings. The path probe
does not interpret CMake command forms; the independent checker reviewed actual/planned
test registrations. These are local planning checks (run URL: N/A-local), not runtime
or hosted acceptance. Required package evidence and operating cost remain execution gates; hardware
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

**Stopped at:**08-10 verified;08-11 ready
Last session: 2026-10-24T22:21:15.223Z
Resume file: .planning/phases/08-controllers-at-the-pins/08-11-PLAN.md

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
| Phase 08 P04 | 14min | 2 tasks | 10 files |
| Phase 08 P05 initial halt | 101min | 2 tasks | 17 files |
| Phase 08 P05 recovery | accepted separately | Task3 | source62fa325 |
| Phase 08 P06 | 12min | 3 tasks | source6fab3e0 |
| Phase 08 P07 | 14min | 3 tasks | source9ca62b3 |
| Phase 08 P08 | 11min | 3 tasks | sourcef33ad71 |
| Phase 08 P09 | 13min | 3 tasks | source2815568 |
| Phase 08 P10 | 8min | 3 tasks | source0f590fe |

## Decisions

- [Phase 08]: Preserve separate fixture admission, device behavior and evidence comparison authorities.
- [Phase 08]: Admit only the exact 4-to-5 annotation transition alongside existing equality and historical adjacent pairs.
- [Phase 08]: Preserve the old input validator and Auto fallback independently of extended device support.
- [Phase 08]: Normalize complete extended scalar fields with fixed-size copies to preserve the restricted core link surface.
- [Phase 08]: Keep motion timing in the existing bus owner with cached idle deadlines and fixed instance state.
- [Phase 08]: Decode the diagnostic selector before mailbox initialization can consume the short handoff frame.
- [Phase 08]: Exercise actual admitted controller rows through independent new-row and later-base producer controls.
- [Phase 08]: Preserve movie1 parsing, allocation and legacy submission independently from immutable typed movie2 replay.
- [Phase 08]: Typed syntax acceptance remains separate from physical-device capability refusal until each controller slice lands.
- [Phase 08]: Historical399937 halt preserved; DEC.61 separately retires only that unsupported cutoff while preserving required measurements and all independent contracts.
- [Phase 08]: Audit every required workflow job; standalone diagnostic consumers link the existing owned digest helper and actual RetroArch acceptance is verified separately from package metrics.

### Blockers

None required. Advisory library timing evidence and measured publication-control
cost remain in the existing seed; neither is promoted into a controller gate.
Current execution continues at08-11; no requirement is complete.
