---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
current_plan: Not started
status: ready
stopped_at: Phase 9 complete and verified; ready to ship Phase 9
last_updated: "2026-10-25T22:01:44.013609Z"
last_activity: 2026-10-25
last_activity_desc: Phase 9 verified 42/42; PAL requirements complete; ship next
state_head: 8814d3e80e1221cc0da71b84560c4f001852212a
progress:
  total_phases: 7
  completed_phases: 5
  total_plans: 50
  completed_plans: 50
  percent: 71
total_phases: 7
current_phase: 10
current_phase_name: Homebrew and expansion memory
---

# Project State

## Project Reference

See PROJECT.md, REQUIREMENTS.md, ROADMAP.md and preparation/DECISIONS.md.
Core value: 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.
Current focus: Phase9 is complete:8/8 plans,22/22 tasks and42/42 verified truths.
PAL-01–03 are complete. The library, runner and both libretro formats execute PAL
with selected timing/geometry and independently proved bounded audio arithmetic.
API6/ABI1/behavior5 and all342 original NTSC observations remain unchanged.
The one review-fix round resolves borrowed BIOS lifetime; no required blocker or owner UAT remains.
Next separate command: $gsd-ship 9. Phases10–11 remain unstarted.

## Current Position

Current Phase: 10 — Homebrew and expansion memory (unstarted)
Current Plan: Not started
Total Plans in Phase: Not planned
Progress: [███████░░░] 71%

Phase9 has8/8 accepted plans and22/22 accepted tasks. Five of seven milestone
phases are complete (71%); phases10–11 remain unstarted. Active-slot runtime totals
are50 summarized plans of50 planned, including historical halted06-01;49 accepted
completed plans. The retained active-slot convention differs from raw historical artifacts.

Status: Phase9 verified; ready to ship
Last activity: 2026-10-25 — Source-fresh acceptance and independent42/42 verification passed
Next command: $gsd-ship 9
Branch: phase/09-pal-games-and-audio

## Phase 9 final acceptance

All eight plans and PAL-01–03 pass. Independent09-VERIFICATION.md covers42/42
truths and112 fingerprinted inputs. One of two permitted review-fix rounds was used;
CR-01 is fixed, re-review is clean, and the gap round is unused. The BIOS alias
regression exercises ten region/source/size combinations and transactional failures.
The16KiB private staging adds no allocation, public ABI change or per-frame work.

Final runtime fixaf9dc83 is accepted through branch head419bb31 and actual tested
mergedf6aeefcd4e6e5f13c4350f3a6fe71d202a0940d; their complete trees match.
Genuine main base2cbbd6a78dabe2bc098615d49f7118231cc05626 is rebuilt independently.
CI40233948646 attempt1 and Nightly40233948516 attempt2 pass:
https://github.com/szTheory/muhtari/actions/runs/40233948646
https://github.com/szTheory/muhtari/actions/runs/40233948516/attempts/2
Native current654/base649+6 in all six lanes, ASan637/base632, full native/PPC654+6,
WASM592, all956 equality rows, vectors258, fuzz10, integer/ABI/architecture/hygiene,
all18 actual candidate ZIPs, authenticated current/base join, armed+1% test-card
performance and the two existing actual RetroArch cases pass. The old342-row
prefix matches actual accepted main artifacts. Archive totals remain report-only.

The delegated post-review regression command
`node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 0 -- sh build/ci/task08-jobserver-gate.sh`
exits0:654 ordinary45.13s, release6 in0.17s, explicit package/release6 in0.13s,
complete JUnit and hygiene. It covers110 prior-phase test references; URL:N/A-local.
The standard FIFO context caps local workers at4 without changing the configured
workflow, presets, workloads or deadlines. Verifier spot checks5/5 exit0.
`verification.status`, `git diff --check` and tree hygiene exit0; URL:N/A-local.

Nightly attempt1 was preempted after apt consumed29m13s of its30m job budget;
567 native cases passed, no source/test error occurred, and PPC did not run.
One targeted same-source retry completed apt in13s and the full loop in9m47s.
All failed/cancelled history remains in09-08-SUMMARY.md. The advisory10000-case
isolation timeout390.00s remains unaccepted and advisory under SEED-009.
PAL fast handoff, optical/RC fits and85:342 analog balance retain their documented
confidence limits; no hardware calibration or universal retail playability is claimed.

The completion helper's seven summary warnings reproduce its command-as-path
heuristic. Actual declared files, reachable commits, consumers, inventories and the
112-file fingerprint pass. Progress/current-position preservation fields are
reconciled here. No new gate, quota, required debt or metadata-retry loop is added.
Draft PR33 remains unmerged. Shipping and post-merge release confirmation are the
next separate step, before starting Phase10.

## Phase 9 execution history

The following slice snapshots retain their original scope and pending statements;
the final acceptance above supersedes them.

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

09-07 tasks37c4286/7bae373/c7867f2/7ad503c and executor summary1061527
are committed. The601-call library, actual runner and loaded two-format paths
agree on independent numeric/native/raw/PCM evidence; all342 old observation
rows retain exact bytes and614 PAL rows are additive. Parent workflow exits0,
653+6 (40.74s/0.13s), N/A-local. Scope, artifacts, inventories and hygiene pass.
Candidate1061527 ZIPs81763+87472+96375=265610 have valid CRCs and measured
producer identities; source is unchanged from7ad503c. At that original handoff,
no base comparison passed; debug recovery below supersedes the acceptance halt.

The genuine pre-plan009f37e producer rebuild exits1 after651/652 pass;
lightgun times out2.07303s. Isolated lightgun passes0.84s, but one bounded
unchanged full retry exits124 and repeats lightgun2.01s before publication
finishes. Initial JUnit and failed outputs remain ignored local evidence;
commands/statuses are in09-07-SUMMARY.md. Publication's existing130s timeout
can outlast the separate60s workflow wrapper. No cause or proven flake is
established; no deadline, workload or policy changes. Two bounded read-only
specialists confirm required base inventory is missing and advise against
another unchanged retry. The clean temporary pre-plan worktree is removed.
That historical halt and all failures remain in canonical09-07 summary.
The separate debug step found pre-expansion calibration on the doubled gun matrix;
complete co-scheduled valid work2.1942s supports provisional5s, preserving all cases,
assertions and lane scales. Exact historical host contention remains unknown.
Foreground execution removes the independently inadequate60s outer wrapper.
The genuine clean009f37e base independently passes652+6 (34.23s/0.13s), including
lightgun1.38419s under its unchanged2s deadline. Fix a93a11d recalibrates the gun;
ddf51d4 also removes GSD's redundant outer timer through supported config0.
The configured foreground workflow passes653+6 (53.53s/0.18s), lightgun2.35s;
all659 CTest deadlines remain positive and wrapper pass/fault exits propagate.
Fresh existing producer observations accept
both complete inventories with matching environment/fixture identities; all six ZIP
CRCs pass. Current81763+87472+96375=265610 versus base81753+87472+96375=265600.
Commands exit0, run URLs N/A-local; full evidence is in the09-07 summary and
.planning/debug/resolved/preplan-lightgun-evidence.md. This is a local slice
comparison, not hosted phase-versus-main acceptance.09-07 is accepted;09-08 ready.
Requirements, hosted acceptance, phase review and verification remain open.
Review-fix/gap rounds remain unused. Next separate command: $gsd-execute-phase 9.

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
broader retail playability and later expansion features remain outside Phase9.
Historical no-EEPROM frame pins retain explicit topology; automatic save metadata has
separate executed hardware tests. Archive/floor/performance guards remain unchanged.

Phase 5 verification20/20 and Phase 6 verification33/33 remain in their own artifacts.
WR-01/COST-01, SEED-009 and v1.0 audit deferrals retain scope. Maintenance-line admission
remains retired; no replacement quota or global hosted-cost reduction is claimed.

## Session Continuity

**Stopped at:** Phase9 complete and verified; ready to ship Phase9
Last session: 2026-10-25T22:01:44.013609Z
Resume file: None
Next command: $gsd-ship 9

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
| Phase 09 P07 | accepted after debug recovery | 3 accepted tasks | source7ad503c; recoverya93a11d/ddf51d4 |

## Decisions

- [Phase 09]: Keep one load-selected regional timing/geometry/audio profile and preserve existing NTSC outputs.
- [Phase 09]: Judge independent startup plus600 full-frame numbers before956-row equality and authenticated main-base evidence.
- [Phase 09]: Stage borrowed BIOS bytes in bounded unpublished instance storage before load commit; preserve live state on failure.
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

No required blocker remains. Phase9 is complete with clean review and42/42
verification; ship9 is the next separate command. Advisory10000-case isolation
cost remains unaccepted in SEED-009. Hardware calibration, broader retail
playability and existing historical deferrals retain their stated scope.
