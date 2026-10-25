---
status: resolved
trigger: "Resolve phase 9 plan 09-07 pre-plan lightgun timeouts and incomplete package evidence"
created: 2026-10-25
updated: 2026-10-25
---

## Current Focus

bug_class: heisenbug-mandelbug
hypothesis: The old2s calibration predates a2.08x workload expansion; actual full-suite co-scheduling leaves little headroom, while the detached60s wrapper independently prevents valid publication work from completing.
test: Execute the authoritative recursive producer directly in the foreground, without the detached outer60s wrapper. Preserve the genuine base's original2s test bound and all source bytes.
expecting: A complete652+6 genuine-base pass establishes acceptance under the changed process/deadline scope; a timeout refutes foreground execution as sufficient for the immutable base and requires a new scheduling decision.
next_action: Debugging complete; plan09-07 is locally accepted and09-08 ready. Stop this step; next separate command is $gsd-execute-phase 9.
reasoning_checkpoint:
  hypothesis: The2s check calibration predates the2.08x lightgun workload expansion, so ordinary co-scheduling consumes its former headroom; the separate detached60s orchestration scope also truncates valid publication work.
  confirming_evidence:
    - Historical immutable-base full runs hit2.07303/2.01s without semantic failures; current/base test and core paths are byte-identical.
    - Complete current co-scheduled matrix passes1.61s and uncensored peer1.6374s; standalone pairs pass0.44–0.59s; setup count doubled at81dca26 without recalibration.
    - Direct foreground authoritative producer exits0 with base652+6, lightgun1.38s and publication33.63s; source remains exact009f37e with clean tree.
  falsification_test: A semantic assertion failure or unbounded guest loop would invalidate a deadline-only fix; a base failure under foreground scope would invalidate its sufficiency for this acceptance.
  fix_rationale: TIMEOUT5 restores the existing twice-complete-success policy from the newly completed2.1942s full-suite sample while preserving every assertion and lane scale. Foreground scope respects existing per-test deadlines and avoids the shorter detached outer wrapper; no base source or policy is changed.
  blind_spots: The exact historical OS scheduling or host-load mechanism was not captured; local5s calibration remains provisional until hosted cohorts in the next plan. No hardware optical calibration is inferred.
  candidate_causes:
    - config — test deadline provenance predates doubled workload; outer60s is shorter than allowed130s publication.
    - environment — parallel nested workloads inflate wall time; detached launcher is a separate changed execution condition, not proven sole cause.
    - code — a semantic race, shared output or unbounded loop was not observed and is contradicted by source trace and complete repeated passes.
    - data — the expanded matrix has1484 instances and2230 loads rather than714 and1075.
  and_gate: Yes for the lightgun timeout — expanded workload plus contended wall scheduling; the outer-scope interruption is independent and also must be addressed for complete evidence.

## Symptoms

expected: Independently build and test candidate and genuine pre-plan source, verify exact ordinary/release inventories, measure all three actual ZIPs with trusted identities and integrity, and restore plan 09-08 readiness only after acceptance passes.
actual: Candidate 1061527 passes 653+6 and measures 265610 bytes; genuine pre-plan 009f37e lacks an accepted full inventory and package comparison. Plan 09-07 is implemented but halted; 09-08 is blocked.
errors: Initial pre-plan full run exits 1 with input.lightgun timing out at 2.07303s (651/652 pass). Isolated test passes 0.84s. One unchanged full retry exits 124, repeating lightgun timeout at 2.01s while publication is unfinished.
timeline: Halt recorded in plan 09-07 parent acceptance on 2026-10-25. Prior plan 09-03 also recorded an isolated pass after a full-run lightgun timeout, without establishing a cause.
reproduction: See .planning/phases/09-pal-games-and-audio/09-07-SUMMARY.md Parent Acceptance. Existing tests/cmake/quality_evidence.cmake producer rebuilds genuine base 009f37ef1b9082c8825419735bfa050a89271f5d using the CI workflow; prior wrapper is gsd-tools.cjs run-with-timeout 60. Source and failure artifacts remain in ignored build/ci; no full retry is authorized without new evidence or a changed falsifiable condition.

## Eliminated

- hypothesis: CTEST_PARALLEL_LEVEL=4 controls the existing ci preset's worker count.
  evidence: The preset explicitly sets jobs to an empty string; all17 input tests start before the first completes despite that environment variable. The override is ineffective and will not be used as evidence.
  timestamp: 2026-10-25

## Evidence

- timestamp: 2026-10-25
  checked: Canonical STATE and 09-07 summary, preparation index, DECISIONS and GSD-HANDOFF section 3.1.
  found: Candidate passes; base failure history retained; publication timeout 130s can exceed outer 60s. No proven flake or cause. Working tree clean on phase/09-pal-games-and-audio.
  implication: Preserve historical halt, all workloads, required checks and failed history; investigate before accepting new evidence. Shared STATE/ROADMAP remain parent-owned.

- timestamp: 2026-10-25
  checked: Current source, input registration, workflow presets, genuine-base failure logs and debug knowledge base.
  found: Both failed tests started in18-worker CTest; TIMEOUT2 has no COST and cites0.97s Windows and0.969s concurrent ASan calibration. Current lightgun source/registration are unchanged from genuine base009f37e. The complete matrix includes both regions, raster boundaries, prefix/default cases and optical history. Existing KB has no matching cause; no per-test coverage for SBFL exists. Current HEAD6d1cc4d differs from candidate1061527 only in parent-owned planning files.
  implication: Classify the observed timeout as environment-sensitive pending controlled measurement. Immutable source comparison is feasible without changing the base; source-level fixes require additional evidence.

- timestamp: 2026-10-25
  checked: Complete unchanged lightgun batches at1/4/18 workers, historical matrix and current/full failure JUnit.
  found: All23 complete binaries pass; batch maxima0.4405/0.4949/0.7286s, total CPU0.4352/1.9304/10.2495s. Specialist independently counts1,484 instances/2,230 loads after81dca26 versus714/1,075 before; timeout provenance predates that2.08x expansion. A separate current binary sample varied to1.248s. Current full JUnit sums220.57s and lightgun1.31829s; first base failure sums265.69s and lightgun2.07303s.
  implication: Light-only18-worker pressure does not reproduce the failure. Co-scheduled nested workloads and process-launch conditions need discrimination before altering a deadline; old pre-expansion calibration is inapplicable.

- timestamp: 2026-10-25
  checked: Direct/detached execution and one instrumented complete current workflow, with an independent complete lightgun peer launched at the actual CTest start.
  found: Three direct/detached pairs pass0.4431–0.4735/0.5203–0.5900s; wrapper alone does not explain2s. The foreground full workflow exits0 with653 ordinary tests34.60s and6 release tests0.11s; lightgun1.61s and uncensored peer1.6374s both pass. GSD's positive-timeout launcher starts a detached process group; its60s outer scope is shorter than publication's permitted130s. No semantic assertion fails.
  implication: Measured complete co-scheduled workload supports provisional ceil(2x1.6374)=4s for the expanded current check. The immutable base must instead pass with its original2s bound under an explicitly changed foreground scope; exact historical host scheduling remains unknown.

- timestamp: 2026-10-25
  checked: Existing recursive producer run directly in foreground, actual event base009f37e, current6d1cc4d and new current output path.
  found: Exit0; current653+6 accepted, genuine base652 ordinary34.23s and6 release0.13s, lightgun1.38s under its original2s deadline, publication33.63s. Fresh base JSON/policy/SHA written. Base HEAD is009f37ef1b9082c8825419735bfa050a89271f5d and status is clean. The two bounded specialist questions are terminal; adversarial synthesis approves minimal4s calibration and requires fresh output plus completed six-ZIP checks, with no producer rewrite.
  implication: Full immutable-base inventory is now established under named changed process/deadline scope. This proves acceptance in that condition, not a sole low-level cause for historical contention. Keep earlier failures canonical.

- timestamp: 2026-10-25
  checked: First complete workflow after provisional4s registration and maintainer-doc changes.
  found: Exit0;653 ordinary51.61s and6 release0.18s. Crucially, complete lightgun2.1942s passes every assertion, naturally exceeding the old2s cutoff; publication50.3981s shows stronger co-scheduled host pressure. Actual build compiler identity is AppleClang21.0.0.21000101, not the preparation table's older17; new provenance is corrected.
  implication: This uncensored complete sample directly demonstrates the old deadline cannot admit required valid work. Recalibrate to ceil(2x2.1942)=5s, then verify once more; this is a measured update, not an unchanged retry.

- timestamp: 2026-10-25
  checked: Final5s working-tree workflow, authoritative fresh current/base observations, full inventories, six actual ZIPs, registration revert/reapply calibration control and hygiene.
  found: All commands exit0, run URLs N/A-local. `cmake --workflow --preset ci` completes653 ordinary50.01s and6 release0.20s; lightgun2.00791s, publication48.9618s. Fresh current/base producer calls each accept exact653+6/652+6. Focused post-control lightgun/docs2/2 passes0.95s into a separate report. `git diff --check` and `scripts/hygiene.sh --tree` exit0. ZIP CRC checks pass all six; current81761+87472+96375=265608 bytes, base81753+87472+96375=265600 bytes. No production source/header/runner/libretro diff exists.
  implication: Local implementation and independent actual package acceptance pass. Final candidate is modified6d1cc4d, not an exact clean-commit claim; parent alone owns its commit and planning acceptance. No owner workflow check is required under AGENTS6.

### Final source and artifact identities

- Candidate HEAD:6d1cc4d7713c0e6ef26dc467caaa1ccebbf8b41f plus only the registration/docs patch below; SHA256 of `git diff HEAD -- tests/input/tests.cmake docs/maintaining.md`:ca45f0ea79139ee8dc23b85abd9bc6fcae9ecd59f0f5a56722a520fbb69bb284. The debug file is additionally untracked. This is not a claim that the modified check/docs were committed at6d1cc4d.
- Genuine base:009f37ef1b9082c8825419735bfa050a89271f5d; `git -C build/ci/quality-base status --short` is empty. Original deadline2s, all assertions/source unchanged.
- Shared environment:AppleClang-21.0.0.21000101-Darwin-arm64; CMake/CTest4.4.3;18 logical processors. Trusted normalized manifest62dc23cf0a687032b3c8d82f988c79d926a1814cd7eaf0f993cbb720a002141e; sample406ca181ecad9c64d4030b09203f210b69d5f5c649453f0a5149fc864444cb4b. Raw current manifest750d8b722c88a03aff050dec5e961a44523a703afcc134f7dce4e3ebf5107932; raw base equals trusted normalized identity. Admission uses existing trusted producer controls.
- Final current ZIP SHA256, library/libretro/runner:9f1ac7e6a448a4587032243e5a63f2f087d4b6c5d9b063bbf849f54d6577a4a5 /9b92c400b3723704f6739521d7557669a03c07a47fd49cacf9b4d8d76becc1c6 /407b2d9e54b1b1179ad5160a41412b6ad4fb5c4951bc1e992323ee672b48cb7f.
- Genuine base ZIP SHA256, library/libretro/runner:385a9ede0c895006200105363a1b9997c304279c4dad3d6d4bfbd47f0b492032 /1c6baebd3f161a03bb56ad032b59cf56e157485d833eabf33c3e576438db6f13 /27ff60cc0fd72d8b008864974b8e9da80a4df87298e5a8bcc68489c14aaa7212.
- Actual archives remain under `build/ci` and `build/ci/quality-base/build/ci`; final observations are `build/ci/quality-debug-final-current.json` and `build/ci/quality-debug-final-base.json`. Initial failed reports/logs remain untouched in ignored build files. Earlier265610 candidate observation is historical; final fresh measurement is265608.

### Accepted commands

All statuses0; run URLs N/A-local.

1. Instrumented complete foreground workflow: `cmake --workflow --preset ci`, with an independently executed `build/ci/tests/muhtari-test-input lightgun` at the actual lightgun launch.653+6 and peer1.6374s complete.
2. Genuine recursive producer: `env GITHUB_EVENT_PATH="$PWD/build/ci/quality-pal-local-event.json" cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-debug-current.json -DSHA=6d1cc4d7713c0e6ef26dc467caaa1ccebbf8b41f -DCONFIG=ci -DSOURCE=N/A-local -DBUILD_BASE=ON -P tests/cmake/quality_evidence.cmake`. This exact clean-source pair precedes the two-file fix; base full652+6 passes unchanged.
3. Final modified-source workflow: `cmake --workflow --preset ci`.653+6; no focused report overwrites its inventories.
4. Fresh final current observation: the producer command above with `-DOUT=build/ci/quality-debug-final-current.json` and without `-DBUILD_BASE=ON`.
5. Fresh genuine-base observation: `env GITHUB_EVENT_PATH="$PWD/build/ci/quality-pal-local-event.json" cmake -DROOT=build/ci/quality-base -DBUILD=build/ci/quality-base/build/ci -DOUT=build/ci/quality-debug-final-base.json -DSHA=009f37ef1b9082c8825419735bfa050a89271f5d -DCONFIG=ci -DSOURCE=N/A-local -P tests/cmake/quality_evidence.cmake`.
6. Post-control focus: `cmake --preset ci && ctest --preset ci -R '^(input[.]lightgun|docs[.]budgets)$' --output-junit lightgun-final-focused.xml --no-tests=error --output-on-failure`.2/2; canonical inventories remain complete.
7. Python standard-library `ZipFile.testzip()` over exactly three actual ZIPs on each side, SHA256/size calculation and distinct complete JUnit membership assertions. No failure/error/skipped element exists; existing release member/notices/asset/floor controls pass6/6 on each side.
8. `git diff --check && scripts/hygiene.sh --tree`.

## Resolution

root_cause:
  - Expanded deterministic lightgun matrix retained a2s calibration from less than half its instance/load work; complete valid co-scheduled work measured2.1942s, proving the old bound inadequate. Exact historical OS pressure was not captured.
  - An independent detached60s outer orchestration deadline was shorter than valid130s publication work and prevented a complete retry inventory.
fix: Current lightgun registration uses provisional5s from ceil(2x2.1942s) complete co-scheduled measurement; no assertion, workload, scale or production behavior changes. GSD config0 selects its supported no-outer-timer foreground mode for the one-shot CMake workflow while all659 CTest deadlines remain enforced. Maintainer guidance requires effective explicit scheduling measurements, separate focused JUnit and fresh complete current/base producer acceptance. Genuine base remains unmodified at its original2s.
oracle_type: derived
verification:
  target_test:
    result: pass
    evidence: Complete original matrix finishes2.1942s under provisional4s and2.00791s under final5s; full653+6 passes. Genuine base passes652+6 with its original2s.
  mutation_check:
    result: skipped
    reason_if_skipped: Stryker is absent/not configured in this C/CMake repository; no production logic changed.
  no_op_deletion:
    result: pass
    deletion_justified_by_rca: true
    evidence: Only timeout/provenance and maintainer execution guidance changed. No assertion, case, inventory member, lane scale or product operation is deleted.
  adjacent_tests:
    result: pass
    suites_run:
      - Full current653 ordinary and6 release tests, including unchanged producer/join and package pass/fault controls.
      - Genuine base652 ordinary and6 release tests, source and registration unchanged.
      - Focused input/docs18 tests and final lightgun/docs2 tests, separate JUnit outputs.
  revert_and_reconfirm:
    result: pass
    bug_returned_on_revert: true
    fixed_on_reapply: true
    oracle: Derived twice-complete-success calibration from the actual2.1942s completed workload.
    evidence: Temporarily restoring only the old2s registration makes the measurement-derived headroom check fail; restoring exact5s registration makes it pass. The final focused run passes after exact restoration.
    limitation: This replays the deadline-calibration defect, not the exact historical OS scheduling sample. No deterministic host-load reproduction is claimed.
  guardrail_verdict: accepted
  limitations:
    - Initial debugger evidence used a modified6d1cc4d tree; the parent subsequently committed and independently rechecked exact sourcea93a11d below.
    - Local provisional timing only; hosted phase-wide calibration/portability acceptance remains the next plan.
    - The precise historical OS pressure was unrecorded; foreground base acceptance and completed2.1942s current work establish the required facts without assigning a sole scheduler cause.
files_changed:
  - tests/input/tests.cmake
  - docs/maintaining.md
  - .planning/config.json
  - .planning/preparation/GSD-HANDOFF.md

## Initial Parent Closeout

Source fix committed as `a93a11d4f7bb25e5334138bdc39261bc6ff173fa`; only
registration/provenance and maintainer guidance changed. The later saved-config
correction and final producer observations are recorded below. Fresh commands
in this initial closeout all exit0, run URL N/A-local:

- `cmake --workflow --preset ci` on that exact commit:653 ordinary50.62s,
  release6/6 in0.19s, lightgun1.99s. Parent focused2/2 uses its own JUnit path.
- `env GITHUB_EVENT_PATH="$PWD/build/ci/quality-pal-local-event.json" cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-debug-parent-current.json -DSHA=a93a11d4f7bb25e5334138bdc39261bc6ff173fa -DCONFIG=ci -DSOURCE=N/A-local -P tests/cmake/quality_evidence.cmake`
  accepts complete current653+6 and actual three ZIPs.
- Same command with `-DROOT=build/ci/quality-base`,
  `-DBUILD=build/ci/quality-base/build/ci`,
  `-DOUT=build/ci/quality-debug-parent-base.json` and
  `-DSHA=009f37ef1b9082c8825419735bfa050a89271f5d` freshly accepts the independent
  exact clean base652+6 built by the recursive producer above. No base changes.
- `python3 build/ci/lightgun-parent-verify.py` validates both full unique successful
  inventories, matching producer source/SHA/environment/count/sum and all six ZIP CRCs.
  Current sizes81761/87472/96375 total265608; base81753/87472/96375 total265600.
  Final committed-source candidate ZIP SHA256 values, library/libretro/runner:
  `9eef1740ad7b7511d0bc3ae8d834f8366c6b3b40c9945849382df3b9f1f1c33b`,
  `df9975ee25aed5abf6989c083e0d8cef3e13046b19d7427137debc80562bc3b4`,
  `a509bb8546d934b485f7eeebeba02f212234ca7f1ad39db3977494dbb9deda69`.
  Base digests and normalized/raw fixture identities remain as recorded above.
- `git diff --check` exits0. No log, generated JSON or package is committed.

Canonical09-07 summary retains its original halt/failures and adds passing recovery
acceptance. STATE/ROADMAP advance only to ready09-08; requirements remain pending
hosted/final phase acceptance. No next GSD step or routine owner UAT was invoked.

## Final Configured-Consumer Acceptance

Parent inspection found `.planning/config.json` still saved60s and the installed
regression gate actually reads it before invoking `run-with-timeout`. Documentation
alone would leave that recurrence path. The installed runner explicitly supports0
as no timer/non-detached foreground execution. Commit
`ddf51d4565596a8b8435e5977217482b1a542a12` sets `workflow.test_gate_timeout: 0`
for the one-shot CMake workflow and documents the reason in maintaining/GSD-HANDOFF.
No CTest deadline or failure predicate is removed; this removes the redundant
outer watch-mode detector instead of guessing a replacement orchestration cap.

Fresh final commands exit0, run URL N/A-local:

- `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" query config-get workflow.test_gate_timeout --raw`
  prints0. `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 0 -- cmake --workflow --preset ci`
  passes653 ordinary53.53s and6 release checks0.18s on the exact configuration and
  source content committed asddf51d4. Lightgun2.35s still fits the same5s calibration.
- Existing producer current command above, with
  `-DSHA=ddf51d4565596a8b8435e5977217482b1a542a12`, and unchanged genuine-base
  command both exit0 and accept653+6/652+6 exactly once with matching environment.
- `python3 build/ci/lightgun-parent-verify.py` additionally confirms config0,
  all659 registered CTest TIMEOUT values positive (maximum250s), and real wrapper
  pass/fault propagation using `cmake -E true`/`false` (exits0/1).
  Both full unique inventories, actual producer identities and six ZIP CRCs pass.
  Final current81763/87472/96375 totals265610 bytes; unchanged base totals265600.
  Final current ZIP SHA256 values, library/libretro/runner:
  `ad83f96df389139f0c7139a1a27c053d67d4f4be009e07b329207fecf31018b5`,
  `6ad7430b2e31d4a128ca4e97641325a9caf3a71d49fdf4a19e6d185a47aadd8c`,
  `7b4b7cd29c604d3b14cb57c21b8240a1cbe574ebd26338629e66698f5cebeee1`.
  These supersede earlier candidate measurements; all earlier successful and
  failed observations retain their actual source labels.

One parent one-off registration assertion initially expected659 tests from
`ctest --preset ci --show-only=json-v1` and exited1: that preset excludes release
and selects653. Corrected inspection uses `ctest --test-dir build/ci --show-only=json-v1`
and checks all659. This was a parent inspection error, not a product/test failure.
The first attempt to commit config/docs was sandbox-denied; the same authorized
Git operation succeeded with required metadata access. No bypass was used.

Local09-07 acceptance is complete; only09-08 may execute next. Original halt,
full failure history, provisional hosted timing and pending phase requirements
remain explicit. Final closeout adds only planning evidence afterddf51d4.

Final `git diff --check`, `scripts/hygiene.sh --tree` and
`query phase-plan-index 9 --pick ready_plans` exit0; scheduler output is only09-08.
Accepted base JUnit files are copied into ignored build/ci before
`git worktree remove build/ci/quality-base` exits0; the base was clean and exact.
No temporary worktree, committed log or generated package remains in the source tree.
