---
status: resolved
trigger: "Windows policy.digest.controls fails in run 39694430448; unblock 05-05 Task2"
created: 2026-10-21
updated: 2026-10-22
---

## Current Focus

bug_class: bohrbug
hypothesis: The complete native arm64 control requires96.22s; the original30s registration truncates otherwise valid finite work.
test: Replace temporary300s with measured ceil(2 x96.22)=193s, remove temporary diagnostics, and run full local/final hosted acceptance.
expecting: Both Windows architectures complete every retained control and independent-base artifact on the diagnostic-free head.
next_action: Commit/push the verified207s calibration and durable partial-plan handoff, confirm latest-head CI exists without claiming it completed, then stop. Next session command is $gsd-execute-phase 5.
allocation: Before implementation, charge this repair and its regression controls/glue to the existing reproduced-fixes100; fixed N1970/C14853 and digest447 remain unchanged. No new allocation or automatic growth.
reasoning_checkpoint:
  hypothesis: Original30s truncates complete native arm64 work that demonstrably takes96.22s on the supported image.
  confirming_evidence:
    - Two actual arm64 controls hit30.02s with progress but no assertion failure; those censored times were not successful samples.
    - Job119563460101 completes the same retained control in96.22s, current609+6 and unchanged base605+6; artifact/log reads exit0.
  falsification_test: An assertion or193s timeout failure on the final diagnostic-free head rejects this complete-work calibration as sufficient acceptance.
  fix_rationale: Apply the documented successful slow-test rule ceil(2 x96.22)=193; preserve every assertion, numeric guard, inventory, producer and7min job cap. Restore original fatal base-workflow behavior after temporary observation.
  blind_spots: One complete arm64 sample remains provisional below10; final-head native/base proof remains outstanding. Earlier silent base-consumer failure is unexplained, not fixed.
  candidate_causes:
    - config: Original30s registration was never calibrated from a complete native success.
    - environment: Supported arm64 image incurs native process work above that bound.
    - code: Finite retained semantic cases complete successfully when allowed to run; no infinite loop or assertion failure is observed.
  and_gate: Yes for timing; insufficient configured bound and native process cost interact. Earlier PATH/directory/CRLF failures remain independently evidenced sequential defects.
repair_cycle: 6; before edits, existing fixes100 covers193s provenance and removal of11 temporary diagnostic lines. Fixed N1970/C14853 and all allocations unchanged.

## Symptoms

expected: Native Windows CI passes policy.digest.controls and reaches the current/base quality observation producers for 05-05 Task2.
actual: Both Windows x64 and arm64 jobs fail the native workflow before quality observations are produced.
errors: tests/policy/digest.cmake:52 reports "digest valid preview failed: no such file or directory"; cmake --workflow --preset ci-msvc exits 8.
timeline: Observed on candidate e6d725faef06b8753945795244ccf78eb1cb2f20 in run 39694430448; earliest introduction and any previous Windows success are not established.
reproduction: Inspect jobs 119548291959 and 119548291973 in https://github.com/szTheory/muhtari/actions/runs/39694430448, then run the native Windows ci-msvc workflow on the branch.

## Eliminated

- hypothesis: Windows is missing the required sh or jq dependency.
  evidence: Correct native PATH construction reaches the real digest and Windows x64 completes609+6; dependency absence does not explain the subsequent failures.
  timestamp: 2026-10-22
- hypothesis: Fake API IDs are invalid independent of native output formatting.
  evidence: Identical fixture passes with binary LF output and fails with the native-CRLF proxy, including an explicit retained carriage-return diagnostic.
  timestamp: 2026-10-22
- hypothesis: The added jq proxy is the sole reason arm64 exceeds30s.
  evidence: Actual native jq without the proxy still reaches30.02s. x64 successful samples24.09s with proxy and26.94s without do not establish a speedup; complete native workload needs measured calibration.
  timestamp: 2026-10-22

## Evidence

- timestamp: 2026-10-22
  checked: Final authentic native/current-base acceptance and monotonic calibration
  found: CI39699892851 completes success at1abc98a, merge011fb5b9d0286a311508d0097fd122663b2d6748 (GitHub API parents65b360d/1abc98a, exit0). Windows jobs119566353447/119566353467 each pass609+6 and unchanged base605+6. Download and pair SHA/source/environment/status checks exit0 after normalizing scalar SHA-file CRLF. Both complete every retained control under193s; x64 takes27.58s and arm64 takes103.28s on image20261013.183.4.
  implication: Separate arm64 image cohorts require final ceil(2 x103.28)=207s. Calibration4af6e9b changes timeout/provenance only, so the stronger193s native pass remains valid behavior evidence. Full local207s CI609+6, focused digest/docs, hygiene/count14528/timeout29/diff pass. No latest-calibration hosted completion is claimed.
- timestamp: 2026-10-22
  checked: Durable continuation and installed scheduler
  found: SUMMARY status blocked preserves incomplete Task2 instead of treating halted as completed. query phase-plan-index5 --pick ready_plans returns only05-05, exit0. Task1 and original task commits are preserved; Task3 remains unstarted. Root policy is intentionally still pre-adoption; direct new-judge replay rejects missing availability, which remains unfinished Task2 rather than a debug-policy change.
  implication: The scoped Windows blocker is resolved by automation under AGENTS6; no owner UAT or downstream execution is needed. Continue only with the next session's execute command.

- timestamp: 2026-10-22
  checked: Final cleanup local acceptance after last code edit
  found: cmake --workflow --preset ci exits0 (609 regular plus six release tests). Diff against pre-debug quality_evidence.cmake is empty: original fatal complete-base workflow restored exactly. Physical count14528/C14853, net repair29 and fixes30/100. Timeout judge retains29 jobs and no workflow timeout change.
  implication: Final implementation is ready for hosted proof; readiness/state/partial-SUMMARY updates remain gated on authentic native/base acceptance.

- timestamp: 2026-10-22
  checked: Cycle5 arm64 job119563460101 and quality-windows-arm64 artifact/log (exits0)
  found: Job succeeds00:19:53–00:24:14 UTC. Image windows-11-vs2026-arm64 20261018.189.1; complete control96.22s, current609+6 and unchanged base605+6. No consumer diagnostic triggers.
  implication: Apply provisional n=1 bound ceil(2 x96.22)=193s. Distinct x64 cohort max26.94s on image20261018.275.1. Remove temporary diagnostics and verify final head; no base-source fix is claimed.

- timestamp: 2026-10-22
  checked: Cycle5 Windows stages in CI39699026057
  found: Arm64 job119563460101 completes current609+6 and reaches the independent-base step under the temporary measurement bound; complete duration awaits the final immutable log. X64 job119563459989 fails before build with curl35 CRYPT_E_REVOCATION_OFFLINE while fetching rcheevos, after earlier unchanged x64 current/base success in cycle4.
  implication: Do not change certificate validation or infer a digest regression from fetch infrastructure. Obtain arm64 base/timing, then run final calibrated diagnostic-free head with fresh x64 acceptance.

- timestamp: 2026-10-22
  checked: Cycle4 x64 complete producer and cycle5 local controls
  found: Job119561446439 completes success, including current609+6 and unchanged base605+6. No consumer diagnostic triggers; earlier silent DLL-CRT failure is unreproduced and no base fix is claimed. Cycle5 local fullci609+6/hygiene/diff exits0; timeout-comments judge exits0 for29 jobs. 05-02 slow-test policy specifies ceil(2 x successful maximum), provisional below10, with changed-workload/image cohorts separated.
  implication: Preserve the prior failed-base evidence as an unresolved transient seed; accept only actual complete independent evidence. Temporary300s measurement is not the final calibrated control bound.

- timestamp: 2026-10-22
  checked: Cycle4 arm64 job119561446457 log (API exit0)
  found: Without the simulation proxy the actual native control still reaches30.02s, but now emits successful relative preview and required-API negative-control evidence. Earlier x64 with proxy completed the full control in24.09s. No complete arm64 timing exists, so30s has no measured successful native basis.
  implication: Measure complete unchanged work under a temporary bounded300s control (existing7min job cap retained), then derive the final timeout from actual completion. Do not accept timeout as assertion success.

- timestamp: 2026-10-22
  checked: Cycle4 final local workflow, hygiene, diff and counter
  found: Full cmake --workflow --preset ci exits0 (609+6), hygiene and diff exit0. Counter14537/C14853 includes11 temporary base-diagnostic lines and one additional proxy-condition line; fixes39/100 at this observation. No test, predicate, source/baseline tree or timeout changed.
  implication: Push the measurement-only correction and fail-closed diagnostics; remove diagnostics after the exact base failure is understood.

- timestamp: 2026-10-22
  checked: Cycle3 x64 job119559093658 full log (API exit0)
  found: Current609+6 passes; unchanged trusted base65b360d builds and runs605 regular tests, but install.consumer.dll_crt links successfully then emits no output and fails its required success regex. Static consumer and embed succeed. Existing adapter correctly rejects the failed base; no artifact is uploaded.
  implication: Exact consumer exit and imported runtime DLLs are required before any fix. Do not modify the trusted source or replace its complete test evidence with a skip/retry success.

- timestamp: 2026-10-22
  checked: Cycle3 native stages and arm64 job119559093616 logs (API exit0)
  found: Windows x64 completes native609+6 and reaches independent-base work. Arm64 executes previews without previous diagnostics but the retained control hits30.02s. Its proxy unnecessarily spawns a Git shell per jq call even though real native jq already supplies native output; POSIX requires that simulation, Windows does not.
  implication: Remove redundant simulation overhead only on the actual platform and remeasure at the unchanged timeout before any calibration. Full root-cause acceptance remains pending.

- timestamp: 2026-10-22
  checked: Cycle3 target reversion and complete local acceptance
  found: Removing only jq -b reproduces required run identity with 1 carriage-return, exit8; reapplying passes every control, exit0. Full cmake --workflow --preset ci passes609+6, hygiene and diff checks exit0. Counter14525/C14853; total repair delta26, fixes27/100. Proxy preserves real jq nonzero statuses through exec/explicit exit.
  implication: Minimal binary-mode change is causally verified; native hosted acceptance remains required and cannot be inferred from simulation.

- timestamp: 2026-10-22
  checked: Native-CRLF proxy and normalized-directory boundary neighbors
  found: Proxy without binary selection reproduces required run identity, exit8. Selecting jq -b clears that stage. A leading-hyphen relative output then exposes option handling introduced by directory normalization, exit8; preserve the original valid-path behavior with -- and reset CDPATH before cd to prevent unexpected lookup/output.
  implication: Boundary regression now covers relative output, leading hyphen and exported CDPATH as well as native absolute Windows output; no identity guard or subprocess status is weakened.

- timestamp: 2026-10-22
  checked: gh api repos/szTheory/muhtari/actions/jobs/119557150265/logs --allow-escape-sequences (exit0), run39697107445
  found: Cycle2 reaches run enumeration, then digest rejects required run identity. jq documents that native jq.exe under MSYS emits CRLF unless --binary/-b is selected (https://jqlang.org/manual/#invoking-jq); local jq1.7.1 accepts -b. Shell read strips newline but preserves carriage return in the last TSV field.
  implication: Output-directory correction cleared the previous file-open error but whole-workflow diagnosis still lacked the native line-ending boundary. Add a local native-output proxy before repair.

- timestamp: 2026-10-22
  checked: Cycle2 local complete workflow and both failed native job logs
  found: cmake --workflow --preset ci exits0 (609 normal plus six release tests), scripts/hygiene.sh --tree and git diff --check exit0. Count14509/C14853, total repair delta10 charged to fixes11/100; all fixed allocations unchanged. Windows x64 log119555526921 independently confirms the same duplicated drive path as arm64. All four other native current/base producers complete success at cycle1.
  implication: Commit only output normalization, relative-output control and maintainer path contract; collect a fresh actual native run before acceptance.

- timestamp: 2026-10-21
  checked: gh api repos/szTheory/muhtari/actions/jobs/119555526925/logs --allow-escape-sequences (exit0), run39696634106
  found: PATH repair launches the preview and fake API, then jq fails to open a path containing both PWD and the C:/ drive-rooted WORK argument. Windows still exits8; Linux current/base and hygiene producers succeed. This is a second stage, not proof of full repair.
  implication: First hypothesis explains the launch defect but was incomplete as whole-workflow diagnosis. Resolve shell output-directory paths before retrying hosted verification. No guard or required evidence is removed.

- timestamp: 2026-10-21
  checked: ctest --preset ci -R '^policy[.]digest[.]controls$' --output-on-failure with semicolon-path fixture and command-only revert/reapply
  found: Old command fails at the first preview with the exact no-such-file diagnostic, exit 8; reapplied native command passes every digest semantic control, exit 0. Full cmake --workflow --preset ci exits 0, 609 regular and six release tests. Physical count14504/C14853, delta5; reproduced fixes now6/100 including the earlier freshness fix.
  implication: A retained regression catches CMake-list PATH corruption locally; hosted native runs must still confirm OS-specific launch behavior.
- timestamp: 2026-10-21
  checked: Installed runtime phase-plan-index/plan-scan semantics
  found: status halted SUMMARY counts as completed and blocks successors; status blocked preserves a partial failure record and exposes its plan as incomplete. Current index has no runnable plan.
  implication: Once repaired evidence is accepted, change the partial SUMMARY to blocked with an explicit resume contract; do not mark it complete or remove its history. Verify only05-05 becomes ready.
- timestamp: 2026-10-21
  checked: Commit/push and hosted producer launch
  found: Implementation899f6400cdd95f7360c7e018de55f4c4d88cdb27 committed with enabled hygiene hooks/noreply identity and pushed, exits0. PR CI39696634106 runs both native Windows jobs119555526921/119555526925. Maintenance preview39696634094 completes success with publisher skipped; hygiene current/base completes success. docs.budgets plus digest control pass2/2 after the documentation edit, exit0.
  implication: Continue actual hosted Windows verification. Release39696634093 startup_failure is the documented unfinished Task3 permission union; repair does not claim release/aggregate completion.

- timestamp: 2026-10-21
  checked: PR20 base metadata, origin/main and retained digest controls
  found: PR base is 65b360d1afd37b7dad9f94dcff497c85df84fd1d, which predates the digest fixture. Native path_list_prepend passes the existing complete local digest controls, exit 0. A dispatch using HEAD^1 would retain the historical fault; the authentic PR event instead selects the unchanged known base.
  implication: Use the actual PR-triggered run and verify current/base artifacts; do not patch or skip the trusted base. Add a legal POSIX semicolon-path boundary to reproduce the CMake-list corruption locally.

- timestamp: 2026-10-21
  checked: Knowledge base and agent skills discovery
  found: Prior PowerPC resolution concerns relative paths, not native PATH separators; no configured debugger skills or project-local skills. Common environment/config pattern applies. No per-test coverage spectrum is available, so SBFL is skipped.
  implication: Test native environment construction directly; preserve existing digest semantic controls.
- timestamp: 2026-10-21
  checked: gh run view 39694430448 -R szTheory/muhtari --job 119548291959 --log-failed (exit 0)
  found: policy.digest.controls fails before the first preview; adjacent quality and lane-equality controls pass. Native cmake -E env receives PATH=WORK/bin:existing-PATH from tests/policy/digest.cmake.
  implication: The error is executable lookup, before digest report logic or API fixture parsing.

- timestamp: 2026-10-21
  checked: 05-05-HALTED.md and STATE.md
  found: Required Windows evidence is absent; quality controls pass; the existing digest control is the recorded blocker.
  implication: Repair this reproduced tooling fault within the debug step; retain the 05-05 acceptance gate and downstream dependencies.

## Resolution

root_cause:
  - Interpolating POSIX PATH syntax into native cmake -E env uses the wrong Windows separator and expands inherited PATH semicolons into CMake command-list arguments, preventing sh from launching.
  - The digest script classifies only slash-rooted paths as absolute, prepending PWD to native C:/ or D:/ output and passing malformed paths to jq after the launch issue is corrected.
  - Native jq text-mode CRLF interacts with POSIX shell numeric/TSV consumption, retaining carriage returns in run-attempt fields and rejecting valid API identities.
  - Complete supported native arm64 work takes96.22s/103.28s on distinct images, exceeding the unmeasured30s registration; censored runs had never established a complete native bound.
fix: Native CMake PATH modification, shell directory resolution with option/CDPATH protection, and jq binary output preserve every existing semantic predicate. Add semicolon-PATH, CRLF, relative/hyphen/CDPATH boundaries; use documented provisional207s from ceil(2 x103.28), retaining7min job cap. Temporary diagnostics/measurement bound are removed.
oracle_type: specified
verification:
  target_test:
    result: pass
    evidence: retained policy.digest.controls with semicolon-path regression, exit 0
  mutation_check:
    result: skipped
    reason_if_skipped: Stryker is neither configured nor applicable to this CMake/sh repository; command-site revert nevertheless kills the repair locally.
  no_op_deletion:
    result: pass
    deletion_justified_by_rca: true
    evidence: POSIX-only path classifier replaced by actual directory resolution; all semantic assertions retained and platform-boundary controls added
  adjacent_tests:
    result: pass
    suites_run: [ci609, release6]
  revert_and_reconfirm:
    result: pass
    bug_returned_on_revert: true
    fixed_on_reapply: true
  hosted_windows:
    result: pass
    run: https://github.com/szTheory/muhtari/actions/runs/39699892851
    tested_commit: 1abc98a00e4e5b42db19ce96d1e9882dc27e97f1
    producer_merge: 011fb5b9d0286a311508d0097fd122663b2d6748
    trusted_base: 65b360d1afd37b7dad9f94dcff497c85df84fd1d
    exact_inventories: current609+6 and base605+6 on each Windows architecture
    tested_timeout: 193s
  final_calibration:
    commit: 4af6e9b5396d16438d293ac0c1e04ae6f6bd4ea1
    timeout: 207s
    local_full_ci: pass609+6
    latest_head_ci: pending; confirm launch at closeout, no completion claimed
  guardrail_verdict: accepted
files_changed: [scripts/digest.sh, tests/policy/digest.cmake, tests/policy/tests.cmake, docs/maintaining.md]

## Prevention

Code branch: POSIX assumptions crossed native Windows PATH, output-directory and
jq text boundaries. Environment branch: native process cost exceeded an unmeasured
bound. Each revealed the next stage after the preceding defect cleared; invalid
fixture data/missing tools were ruled out by same-data boundary tests and native success.

why_not_caught: POSIX-only local controls and Linux preview lacked native path/CRLF cases and complete Windows timing.
recurrence_guard: policy.digest.controls now retains semicolon-PATH, native-CRLF proxy on POSIX, real native jq on Windows, absolute/relative/hyphen/CDPATH cases and every privilege/report/API assertion. Registration uses measured separate provisional image cohorts.
limits: The earlier silent base DLL-CRT failure and certificate-revocation fetch failure remain recorded transient observations, not source fixes. Task2 policy adoption/bounds and Task3 aggregate/permissions remain incomplete.
semantic_index: skipped; no MemPalace CLI/tool exists, knowledge-base.md is durable fallback.
