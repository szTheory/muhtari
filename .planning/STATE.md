---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
current_plan: 7
status: Ready to execute
stopped_at: Phase 9 plan 09-06 accepted; 09-07 ready; next $gsd-execute-phase 9
last_updated: "2026-10-25T18:34:55Z"
last_activity: 2026-10-25
last_activity_desc: Plan 09-06 accepted; 09-07 ready
state_head: 2b3ddffbe9a48d1507a6dbfc836934f70230151a
progress:
  total_phases: 7
  completed_phases: 4
  total_plans: 50
  completed_plans: 48
  percent: 57
total_phases: 7
current_phase_name: PAL games and audio
current_phase: 09
---

# Project State

## Project Reference

See PROJECT.md, REQUIREMENTS.md, ROADMAP.md and preparation/DECISIONS.md.
Core value: 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.
Current focus: Phase9 plans09-01–06 are accepted locally;09-07 is ready.
The library, automatic-region runner and both libretro formats execute PAL with
selected timing/geometry and independently proved bounded audio storage/arithmetic.
API6/ABI1/behavior5 and retained NTSC observations remain unchanged.
PAL-01–03 remain pending the two successor plans and final hosted acceptance.
Next separate command: $gsd-execute-phase 9.

## Current Position

Current Plan: 7
Total Plans in Phase: 8
Progress: [██████░░░░] 57%

Phase9 has6/8 accepted plans and16/22 tasks complete. Four of seven milestone
phases remain complete (57%); phases10–11 remain unstarted. Runtime totals are
48 summarized active plans of50 planned, including historical halted06-01;
accepted completed plans total47. These use the retained active-slot convention.
The raw on-disk inventory is54 summaries and56 plans, including historical
recovery artifacts. Summary existence alone is not phase acceptance.

Status: Ready to execute09-07
Last activity: 2026-10-25 — Plan09-06 accepted;09-07 ready
Next command: $gsd-execute-phase 9
Branch: phase/09-pal-games-and-audio

09-01 sourcead0d661 and summarya09ecc5 are committed. Task commits8b845bc
(RED),bb4e39c (GREEN),824a3ea (startup/lifecycle) andad0d661 (docs) resolve
on this branch. Focused checks23/23 and27/27 pass; final source workflow652+6
and explicit package/release checks pass. Local candidate producer records
81274+85807+94386=261467 bytes, with no independent-base or hosted claim.
Independent literal edge/startup, full591149-phase/4000-alignment coefficients,
physical buffers, failed loads, canaries and real host output are covered.

09-02 source634596b and summaryaec0bb3 are committed. Task commitse770594
(RED),a5dc4e2 (GREEN),81dca26 (optical) and634596b (lifecycle) resolve on
this branch. Focused checks23/23,15/15 and24/24 pass; final source workflow
652+6 and explicit package/release6/6 pass. PAL keypad2838, EEPROM35469 and
single-ceil paddle fits preserve NTSC predicates. Both-region/both-placement
battery, exact ACK/reset, failed-load and nine-family noalloc regressions pass.
Native lower-row optical/NULL-video raw/PCM coverage passes. Real D64 crosses
VBLANK; separately seeded pending deadlines prove physical-frame wrap retention.
Neither establishes measured hardware calibration.

Historical09-02 parent gate: `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake --workflow --preset ci`
exits0 (652 ordinary tests,27.62s;6 release checks,0.13s). Run URL:N/A-local.
`check evaluation-scope --plan 09-02 --commits-only --raw`,
`git diff --check` and `scripts/hygiene.sh --tree` exit0. All five declared
modified files and committed summary exist; Self-Check is PASSED. No active
wave-post hooks. Existing producer measures current 634596b: 262688 bytes versus
actual preplan 2096b28: 261466 bytes, each independently652+6. This is a local
slice comparison, not hosted or phase-versus-main acceptance. Temporary base
measurement worktree is removed.

09-03 sourceef95e37 and summaryd351008 are committed. Seven task commits
resolve on this branch, including three RED/GREEN pairs and precise regional
movie error columns. All256 approved warm PAL RGB entries, RGB565 truncation and
legacy NTSC parity pass. The actual runner covers explicit regions, opt-in eight-key
info, full93440-byte native/292-row PNG, raw/PCM identity and immutable movie row
bounds before output allocation/save import. Every allocation failure is covered.
PAL startup960/962/961 samples and unchanged legacy transcripts are retained.

Historical09-03 parent gate: `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake --workflow --preset ci`
exits0 (652 ordinary tests,44.82s;6 release checks,0.20s). Run URL:N/A-local.
`check evaluation-scope --plan 09-03 --commits-only --raw`,
`git diff --check` and `scripts/hygiene.sh --tree` exit0. All14 modified files
and committed summary exist; Self-Check is PASSED. No active wave-post hooks.
Existing producer measures final sourceef95e37:265208 bytes versus genuine
preplanba8c22e:262677 bytes, each independently652+6 with matching environment.
Base's initial lightgun timeout remains recorded; isolated0.62s and one bounded
unchanged-base full retry pass. No deadline/workload change or proven-flake claim.
This is a local slice comparison, not hosted or phase-versus-main acceptance.
Temporary base measurement worktree is removed. Full failure history remains
in09-03-SUMMARY.md; no logs are committed.

09-04 source03646b3 and summary98754eb are committed. RED/GREEN pairs
(d20ae9e/fe65173 andd2b845d/03646b3) resolve on this branch. Both actual pixel
modules prove load-captured auto/ntsc/pal selection, exact PAL AV/292 rows,
all256 warm colours, nonzero exact stereo PCM and missing-callback advancement.
Run/Restart never applies a pending region; reload does. Same-module override,
failed-load/unload/test-card/reinit lifetimes retain clean NTSC fallback with no
load-time AV/geometry callback. Native/raw/PCM observation seams pass both formats.

Historical09-04 parent gate: `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake --workflow --preset ci`
exits0 (652 ordinary tests,25.23s;6 release checks,0.09s). Run URL:N/A-local.
`check evaluation-scope --plan 09-04 --commits-only --raw`, `git diff --check`
and `scripts/hygiene.sh --tree` exit0. All three modified files and committed
summary exist; Self-Check is PASSED. No active wave-post hooks or gate failures.
Existing producer measures exact source03646b3:265608 bytes versus genuine
preplan472b7c6:265200 bytes, independently652+6 each with equal toolchain/fixture
identity. This is a local slice comparison, not hosted or phase-versus-main
acceptance; the clean temporary base worktree is removed. Required package,
archive and release checks pass. Optional local sanitizer runs remain unaccepted:
both focused cases time out60s and an empty instrumented program independently
returns124 at5s before output. No sanitizer success or deadline change is claimed;
final required hosted acceptance remains09-08. The best-effort WINDOWS append
encountered existing count drift and wrote nothing. Full failed history remains
in09-04-SUMMARY.md; no logs are committed. Shared progress fields were reconciled
against the actual index after the helper's advisory percent0 output.

09-05 sourceb8dbe18 and summaryafa03f4 are committed. Task commitsf4b0965,
e70ae8c,7896e5f andb8dbe18 resolve on this branch. All591149 arithmetic carry
steps and8000 actual signed-mass extremizers pass independently. Real startup,
456-tick TIA/stretched-phi2 POKEY, half-open horizons, transactional capacity,
NULL parity and long uint64 elapsed totals are covered. Both-region spectral
judges reject every bypass; real maximum-current/hostile source paths remain
bounded and allocation-free. No production code or quality threshold changed.
Spectral COST21 reflects a complete20.08s local workload; deadlines are unchanged.

Historical09-05 parent gate: `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake --workflow --preset ci`
exits0 (652 ordinary tests,34.25s;6 release checks,0.10s). Run URL:N/A-local.
`check evaluation-scope --plan 09-05 --commits-only --milestone-bound --max-commits 30 --raw`,
`git diff --check` and `scripts/hygiene.sh --tree` exit0. All six declared modified
files, four task commits and committed summary exist; Self-Check is PASSED.
No active wave-post hooks or gate failures. Existing producer measures exact
sourceb8dbe18:265610 bytes versus genuine pre-task5678c7b:265598 bytes,
independently652+6 each with matching toolchain/fixture identity. This is a local
slice comparison, not hosted or phase-versus-main acceptance; the clean temporary
base worktree is removed. Initial test-oracle and report-field assertion failures
remain recorded in09-05-SUMMARY.md and are not claimed as RED evidence or passes.
No logs are committed. PAL-03 classification and85:342 analog calibration remain
unresolved; actual sustained three-host output and hosted acceptance remain09-07/08.

09-06 source355d34b and summary2b3ddff are committed. RED7681723,
GREEN0ef76dc and join-controls355d34b resolve on this branch. The sole production
change adds the exact named owned PAL alternative. Real temporary Git event-base
producer and unchanged join pass/fault controls retain old manifest/sample bytes,
raw trusted identities, exact annotations and required measured three-ZIP evidence.
Two focused read-only boundary/evidence specialists found no material defect.
No deadline, COST, quota, threshold, lane or behavior revision changed.

Fresh09-06 parent gate: `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake --workflow --preset ci`
exits0 (652 ordinary tests,35.71s;6 release checks,0.13s;quality14.25s).
Run URL:N/A-local. `check evaluation-scope --plan 09-06 --commits-only --milestone-bound --max-commits 30 --raw`,
`git diff --check` and `scripts/hygiene.sh --tree` exit0. All three declared
modified files, task commits and committed summary exist; Self-Check is PASSED.
No active wave-post hooks or gate failures. Existing producer measures exact
source355d34b:265608 bytes versus genuine preplan4fb6302:265600 bytes,
independently652+6 each with matching full environment/raw fixture identity.
Explicit package/release6/6 and actual ZIP integrity pass. This is a local slice
comparison, not hosted or phase-versus-main acceptance; the clean temporary base
worktree is removed. Intentional admission RED and the corrected fault-input
restoration failure remain in09-06-SUMMARY.md; no logs are committed.
Actual owned PAL row/admission and sustained three-host observations remain09-07;
complete hosted/current-base acceptance remains09-08. PAL requirements stay open.

The installed execute-phase ready-wave gate requires dispatching the initially
ready wave then re-running, and forbids dispatch of this run's not-ready plans.
Only09-06 was initially ready. The fresh phase-plan-index now reports09-07 ready;
09-08 awaits09-07. Stop at this boundary with no phase completion,
review, verifier or requirement-completion claim. Review-fix and gap rounds unused.

Fast PAL startup140016 and zero undocumented residue remain fitted policy;
85:342 analog balance is unverified. The planned PAL-01 probe classification
remains flagged. Diagnostic/trust acceptance and complete hosted/current-base evidence remain
successor-plan work; expanded local audio evidence is accepted in09-05.

Historical Phase9 planning evidence is retained in09-RESEARCH.md and the planning
commit7019a6b:8 plans/22 tasks checked;19/19 decisions and22/22 source items
covered. Those planning passes do not establish runtime or hosted acceptance.

Historical Phase8 ship snapshot (retained evidence):
https://github.com/szTheory/muhtari/pull/31

Fresh shipping checks: `cmake --workflow --preset ci` exits0 (649+6),
`git diff --check` exits0, and `verification.status` exits0 with passed.
Run URL: N/A-local. Prior exact-source hosted evidence remains below.
The ship note retains CI triggering so the final PR head has required checks.
Existing code review is closed, both findings fixed; no routine owner review remains.

Final tested source70366fdab87b2f83d65c3bf865c850b2f08d9f24,
genuine parent7887ad65e7c7887032601109449758d6a8b0e9a9.
CI40157749294 attempt1 passes all required jobs, including actual RetroArch:
https://github.com/szTheory/muhtari/actions/runs/40157749294
Native current/base649+6 on six lanes, full native/PPC649+6, ASan632,
hygiene24, nofp18/architecture, WASM587 and342-row equality pass. All18 actual
candidate ZIPs and authenticated eight-lane join pass; peaks414114/414134 bytes
remain report-only. Original69 observations retain all digests/numbers except
seven explicit behavior4→5 annotations. Hardware fits remain unmeasured.

Nightly40157750598 is completed/success with eight successful jobs and one failed
advisory property job. Five10000-case properties pass; isolation10000 at seed
40157750598 times out390.08s (CTest8, inventory1). That evidence is unaccepted,
with no retry or flake claim. Required isolation passes; optional profiling is
in SEED-009. No advisory lane was promoted into a phase gate.
https://github.com/szTheory/muhtari/actions/runs/40157750598

Code review is clean after one of two allowed fix rounds: complete short gun aim
now retains zero absent flags, proved by24 regression cases and independent real
sensor recheck; ProLine guide labels are corrected. The expanded gun deadline is
measured2s with unchanged workload and lane scaling. Gap round unused.
Independent verification passes49/49; all20 locked decisions honored, no owner UAT.
The CLI-generated148-file fingerprint and exact evidence live in08-VERIFICATION.md.
`verification.status` exits0 with passed. Parent regression gate exits0 (649+6),
covering65 prior-phase test references; run URL N/A-local.

Completion-scanner command-as-path warnings are the reproduced advisory heuristic,
not missing implementation. Actual declared files/commits/consumers and the complete
independent fingerprint pass. Metadata progress fields were reconciled after the
helper's preserved-field warnings. No verification debt or required blocker remains.

## Execution evidence (historical slices)

08-14 tested sourcedf8f9a6 and genuine parent81eba27 have full exact-source
CI40153312357 and Nightly40153313963 success, attempt1. Native649+6 in all six
current/base lanes, full native/PPC649+6, WASM587 and342-row equality pass.
Actual RetroArch pixel/battery cases pass. All18 current ZIPs and authenticated
quality join pass; current/base peak414124/414132 bytes remains report-only.
Canonical14 summary preserves every failed/cancelled attempt and measured fixes.
At that earlier acceptance point, formal review and independent goal verification remained pending.

08-13 source38ec251 and summaryf08451c are accepted. Parent post-wave full-ci
gate exits0 (648+6), N/A-local. All-family transactional/noalloc/peek/storage, real
typed generators,10000-case isolation and bounded event/allocation probes pass.
Local ZIP sum259493 is unchanged. Cost basee83d478 is a local slice, not main.

08-12 implementation03b7f96 and summaryb11fb48 are accepted. Parent post-wave
full-ci gate exits0 (648+6), N/A-local. Conventional mappings and six fresh26-record
adapter groups pass every deliverable; all earlier7 reports remain byte-identical.
Local ZIPs80625+85066+93802=259493 bytes; hygiene24 passes.

08-11 implementation70465c9 and summaryc38c669 are accepted. Parent post-wave
full-ci gate exits0 (648+6), N/A-local. Complete17-bit SNES and six-pair Mega scans,
effective DDR edges, finite completion and eight opposite-EEPROM placements pass.
Local ZIPs80627+84195+93802=258624 bytes; hygiene24 passes.

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
That recovery closed only05; at that point all INPUT requirements and phase completion were pending.
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

**Stopped at:** Phase 9 plan 09-04 accepted; 09-05 ready; next $gsd-execute-phase 9
Last session: 2026-10-25T17:54:35.153Z
Resume file: .planning/phases/09-pal-games-and-audio/09-05-PLAN.md

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
| Phase 08 P11 | 8min | 2 tasks | source70465c9 |
| Phase 08 P12 | 14min | 3 tasks | source03b7f96 |
| Phase 08 P13 | 15min | 3 tasks | source38ec251 |
| Phase 08 P14 | 66min verification window | 3 tasks | sourcedf8f9a6 |
| Phase 09 P01 | 17min | 3 tasks | sourcead0d661 |
| Phase 09 P02 | 13min | 3 tasks | source634596b |
| Phase 09 P03 | 16min | 3 tasks | sourceef95e37 |
| Phase 09 P04 | 14min | 2 tasks | 3 files |

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

No required09-04 blocker.09-05 is ready;four successor plans and final phase acceptance remain.
Current required phase verification has not run. Historical advisory items follow.
None required for the prior phase. Current-source advisory10000-case isolation timing and earlier
publication/library costs remain in SEED-009; unaccepted evidence is preserved.
Next command: $gsd-execute-phase 9 in a new session. Phase9 review-fix and gap execution rounds remain unused.
