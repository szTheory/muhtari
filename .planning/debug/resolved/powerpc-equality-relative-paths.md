---
status: resolved
trigger: "PowerPC equality caller rejects relative evidence directories"
created: 2026-10-21
updated: 2026-10-21
---

## Current Focus

bug_class: bohrbug
hypothesis: GLOB receives the original relative arguments after canonical paths were computed only for duplicate detection.
test: Add a relative-path positive case to the existing equality control, observe RED, then reuse canonical arguments throughout the comparator.
expecting: Existing absolute controls pass; relative positive fails at the empty-member error before the fix.
next_action: Commit and push the planning-only reconciliation, confirm the implementation tree is unchanged, then stop. Exact next session command is $gsd-execute-phase 5; scheduler ready_plans contains only 05-04.
candidate_causes:
  - code: Original relative paths used for the GLOB pattern and RELATIVE base.
  - environment: CMake working directory or version changes path interpretation.
  - data: Downloaded evidence has missing members or divergent hashes.
and_gate: no; absolute replay changes only argument spelling and passes on the same files.
reasoning_checkpoint:
  hypothesis: GLOB cannot discover members using these relative directory arguments; canonical paths are computed but discarded before member enumeration.
  confirming_evidence:
    - Authentic Release pair fails with relative paths and passes with absolute paths on the same CMake 4.4.3 executable and working directory.
    - New existing-control relative pass fails at the same empty NATIVE members after all eight absolute cases pass.
  falsification_test: Canonicalizing NATIVE and BE without changing any validation must make the authentic relative replay and the relative positive pass; otherwise this hypothesis is wrong.
  fix_rationale: Reusing the existing REAL_PATH results establishes one canonical argument contract for duplicate detection, member enumeration, reads and byte comparison.
  blind_spots: GNU/Linux hosted callers still need final-commit proof; local success alone does not satisfy full producer acceptance.
  candidate_causes:
    - code: Canonical paths discarded after duplicate detection.
    - environment: CMake version or working-directory differences.
    - data: Missing evidence members.
  and_gate: no; the same files and environment pass after changing only path spelling.

## Symptoms

expected: Complete native and PowerPC evidence from the same producer passes the unchanged inventory and hash equality checks.
actual: Both hosted callers complete all 607 normal and six release checks per lane, then fail at the final equality command; absolute-path replay passes.
errors: "lane_equality: members of NATIVE:" with an empty member list.
timeline: Observed after the PowerPC ABI repair at 96dc316; the full producers completed with this separate caller defect.
reproduction: Replay tests/cmake/lane_equality.cmake on build/recovery-full-failed/evidence/native and its BE pair with producer SHA 96dc316427cbcb686b06b650d4cc16d729c419d3; compare relative arguments with absolute arguments.

## Eliminated

- hypothesis: Native and PowerPC frame/audio hashes differ.
  evidence: Unchanged absolute-path comparator replay exits 0 with two identical files on both authentic pairs.

- hypothesis: Downloaded Release evidence is missing members.
  evidence: Authentic relative replay exits 1 at members of NATIVE; absolute replay of the same directories exits 0 with hashes: 2 identical files.
  timestamp: 2026-10-21

## Evidence

- timestamp: 2026-10-21
  checked: Prior execution halt record and authentic hosted runs.
  found: Release run 39659029223 and Nightly run 39659029340 fail only at final equality after complete inventories pass.
  implication: Investigate path handling without relaxing comparator, inventory, ABI or identity controls.

- timestamp: 2026-10-21
  checked: Knowledge-base and skill discovery.
  found: No local knowledge base or MemPalace tool is available; project has no local skills and debugger agent-skills query emits none.
  implication: Use direct differential reproduction. SBFL skipped because no per-test coverage exists for these CMake controls; git bisect is unnecessary after isolating the exact deterministic call boundary.

- timestamp: 2026-10-21
  checked: Authentic Release artifact pair at producer 96dc316427cbcb686b06b650d4cc16d729c419d3.
  found: Relative cmake -DNATIVE=build/recovery-full-failed/evidence/native -DBE=build/recovery-full-failed/evidence/be -DEXPECTED_SHA=96dc316427cbcb686b06b650d4cc16d729c419d3 -P tests/cmake/lane_equality.cmake exits 1 at empty members. Identical command with absolute directories exits 0, hashes: 2 identical files.
  implication: A deterministic path contract fault precedes hash comparison and is independent of artifact contents.

- timestamp: 2026-10-21
  checked: Agent-authored extension of policy.lane_equality.controls to run every existing case with absolute then relative arguments.
  found: cmake --preset ci exits 0; ctest --preset ci -R '^policy[.]lane_equality[.]controls$' --output-on-failure exits 8. All eight original absolute cases pass, then the relative positive fails at lane_equality: members of NATIVE.
  implication: Specified oracle has a tight RED reproduction; existing absolute-only controls missed the actual caller shape.

- timestamp: 2026-10-21
  checked: Fixed comparator, unchanged authentic Release/Nightly pairs and existing equality controls.
  found: ctest --preset ci -R '^policy[.]lane_equality[.]controls$' --output-on-failure exits 0 for all sixteen cases; both relative authentic replays exit 0 with hashes: 2 identical files. The eleven-line temporary ABI step is removed; read-only full producer and all validators remain intact.
  implication: Central argument normalization repairs both callers while preserving positive and negative contracts; hosted acceptance is still pending.

- timestamp: 2026-10-21
  checked: Comparator-only revert/reapply and local fixed-budget acceptance.
  found: Reverting only three substitutions returns regression exit 8 and the original empty-member failure; reapplying returns exit 0. Gate count exits 0 at 13445 lines, 8580 full/6533 authored source; ci_required exits 0 for 15 cases; hygiene and diff checks exit 0. Both historical pairs' eight JUnit judges exit 0 for 607 normal and six release tests each.
  implication: Direct causation is proved. Net gate reduction is six lines; BE allocation is 193/300, with B12885/S2/N1970/C14853 unchanged.

- timestamp: 2026-10-21
  checked: First complete local ci and documentation size correction.
  found: First ci exits 8 only at docs.budgets because the added explanation exceeded 40960 bytes. The existing portability paragraph was shortened without changing its contracts; docs.budgets now exits 0. One stale ci attempt was interrupted with exit 130 before editing again; no successful result is inferred from it. Final complete ci is running after the last edit.
  implication: No budget was raised; only a fresh complete ci may authorize the implementation commit.

- timestamp: 2026-10-21
  checked: Fresh complete local acceptance after the final implementation edit.
  found: cmake --workflow --preset ci exits 0; normal/release check_junit commands exit 0 for 607 and six exact executions. docs/testing.md is 40949/40960 bytes. ci_required passes 15 cases; count remains 13445 with 193/300 BE use and unchanged inventory/budgets.
  implication: Implementation is ready to commit and push; completed hosted success at the resulting immutable head is still required.

- timestamp: 2026-10-21
  checked: Implementation commit, retained PR and launched hosted producers.
  found: Commit 0901c877977e9f712a77193d4840df5ff5b5408d contains only four implementation/test/doc files; git push exits 0. Draft PR 20 retains its phase branch. Release https://github.com/szTheory/muhtari/actions/runs/39663281026 (job 119441583140) and Nightly https://github.com/szTheory/muhtari/actions/runs/39663280880 (job 119441580417) are executing; direct CI is run 39663280906.
  implication: No implementation edits are planned during these final-head runs. Existing halted summary remains unchanged until acceptance; scheduler correctly exposes no runnable downstream plan yet.

- timestamp: 2026-10-21
  checked: Completed Release and Nightly equality jobs, downloaded artifacts and logs.
  found: Release job 119441583140 succeeds at 19:49:09 UTC, 689 seconds, producer 0901c877977e9f712a77193d4840df5ff5b5408d. Nightly job 119441580417 succeeds at 19:50:01 UTC, 741 seconds, actual merge producer 5ad2484b9d48c0ef860c4a1d9a23f18a9dd7b2a2. Both artifact downloads and log fetches exit 0. All eight existing JUnit judges exit 0, with 607 normal and six release executions per lane, and independent Counter checks prove exact membership/no extra, missing, duplicate, failure, error or skip. Each relative-path comparator replay exits 0 with hashes: 2 identical files, exact producer SHA and matching identities.
  implication: Full caller contract and diagnostic-free implementation are proved on both authentic hosted producers; called CI required remains pending before recovery reconciliation.

- timestamp: 2026-10-21
  checked: Successful full-workload timing and image provenance.
  found: Release uses ubuntu-26.04 image 20261018.168.1, native normal/release 52.89/0.62 seconds and BE 594.96/0.18 seconds. Nightly image 20261012.163.1 has native 52.41/0.41 seconds and BE 642.17/0.19 seconds. Each caller/image cohort has one completed-success sample; twice-max rounds to 23 and 25 minutes respectively (five-minute floor). Previous 706/740-second failed jobs and cancelled 6m35s stay outside successful cohorts.
  implication: Both sub-ten cohorts remain provisional; retain the unchanged 30-minute bound without an extra workflow/head edit.

- timestamp: 2026-10-21T19:58:48Z
  checked: Remaining Release-called CI and direct CI jobs.
  found: Both Release-called macOS legs are still queued from 19:37:47 UTC with no started step. Direct CI macOS x64 completed successfully; direct arm64 and Nightly macOS bench are queued. Every other observed job has completed without failure. Running-log fetch before equality completion returned HTTP 404, then succeeded after completion; no unobserved partial progress was claimed.
  implication: Hosted scheduling is the remaining external wait. Do not replace the required called CI roll-up with direct-job evidence or mark recovery complete yet.

- timestamp: 2026-10-21T20:08:18Z
  checked: Read-only runner-label, allocation and official service-status diagnosis.
  found: Called jobs 119441654408 (macos-15) and 119441654646 (macos-15-intel) both report queued, runner_id 0, empty runner_name and no executed steps. The CI workflow is unchanged from 96dc316, git diff exit 0. Official supported-runner documentation lists both labels (https://docs.github.com/en/actions/reference/runners/github-hosted-runners). Same Intel label completed direct job 119441645305 from 19:52:42 to 19:55:08; same arm64 label completed prior called job 119426935539 at 19:13:50. Official https://www.githubstatus.com/api/v2/summary.json reports Actions operational and https://www.githubstatus.com/api/v2/incidents/unresolved.json contains no incident.
  implication: Invalid labels are eliminated. Allocation is externally stalled for these specific jobs; no global incident or precise capacity cause is claimed. Further wait is bounded to 20:18:18 UTC by the orchestrator; acceptance cannot be replaced or cleared without the called CI result.

- timestamp: 2026-10-21T20:12:48Z
  checked: Required called macOS jobs during the bounded external wait.
  found: Called macOS x64 job 119441654646 has transitioned from queued to in_progress; called arm64 remains queued. No acceptance job reports failure.
  implication: Runner scheduling has progressed within the ten-minute observation bound. Follow actual results as directed; an unavailable-runner checkpoint is not yet warranted.

- timestamp: 2026-10-21T20:18:00Z
  checked: Completed called CI acceptance and final checkout binding.
  found: Called macOS x64 job 119441654646 succeeds at 20:15:29, with authentic log showing 0901c877 checkout and exact 607/6 JUnit checks. Called arm64 119441654408 succeeds at 20:16:47, same checkout and exact 607/6 checks. Six-native-file hash equality job 119461219251 succeeds at 20:16:56. Called CI required job 119461511750 succeeds at 20:17:26, with HEAVY=true and CALLED=true in its authentic log. Both log downloads exit 0. Direct CI 39663280906 and direct Nightly 39663280880 are completed success. The Release workflow still has advisory/rehearsal work outside this recovery's required-CI acceptance; no overall Release completion is claimed at this observation.
  implication: All authorized recovery acceptance outcomes are now proved at immutable implementation 0901c877 and Nightly producer 5ad2484b. The external queue has cleared without label, timeout, privilege or acceptance changes.

- timestamp: 2026-10-21T20:21:18Z
  checked: Successful recovery reconciliation and actual scheduler readiness.
  found: 05-10 SUMMARY is complete with requirements-completed empty and block-style coverage, all human_judgment false. STATE/ROADMAP show 3/9 plans and zero completed phases. Windows fixed commands exit 0 for only entries 11–14; unrelated historical entry 6 remains open. phase-plan-index exits 0 with only 05-04 ready and no halted live plan. Historical HALTED files and 05-03 records are unchanged.
  implication: No owner UAT is required under the explicit automation-first AGENTS rule 6 and task authorization. All required outcomes are directly observable and verified by the agent. This resolves only the scoped debug/recovery, without executing downstream work or completing phase-wide requirements.

## Resolution

root_cause: The comparator canonicalized NATIVE and BE only into separate duplicate-check variables, then gave original relative arguments to file(GLOB), producing an empty member list before content comparison. Both hosted callers supplied relative paths; existing controls supplied only absolute paths.
fix: Store existing REAL_PATH results into NATIVE and BE, so all validators consume canonical paths. Run existing positive and fault cases with both absolute and relative arguments. Remove completed ABI diagnostic step and document the path contract.
oracle_type: specified
verification:
  target_test:
    result: pass
    command: ctest --preset ci -R '^policy[.]lane_equality[.]controls$' --output-on-failure
    exit: 0
  mutation_check:
    result: skipped
    reason_if_skipped: No Stryker is configured for CMake; comparator-only reversal is exercised by the mandatory revert signal.
  no_op_deletion:
    result: pass
    deletion_justified_by_rca: true
    reason: All validations remain; removed workflow block is completed temporary ABI diagnosis, with historical evidence retained.
  adjacent_tests:
    result: pass
    suites_run:
      - cmake --workflow --preset ci; exit 0; 607 normal and six release checks
      - ci_required; exit 0; 15 cases
      - authentic Release and Nightly relative artifact replay; both exit 0
  revert_and_reconfirm:
    result: pass
    bug_returned_on_revert: true
    fixed_on_reapply: true
  guardrail_verdict: accepted
  hosted_acceptance:
    release_equality: pass; run 39663281026, job 119441583140, producer 0901c877977e9f712a77193d4840df5ff5b5408d
    nightly_equality: pass; run 39663280880, job 119441580417, producer 5ad2484b9d48c0ef860c4a1d9a23f18a9dd7b2a2
    release_called_ci: pass; run 39663281026, CI required job 119461511750, producer 0901c877977e9f712a77193d4840df5ff5b5408d
  fix_commit: 0901c877977e9f712a77193d4840df5ff5b5408d
files_changed:
  - tests/cmake/lane_equality.cmake
  - tests/policy/tests.cmake
  - .github/workflows/be-equality.yml
  - docs/testing.md

## Prevention

Code branch: canonicalization was scoped to duplicate detection rather than the
whole input contract. Configuration branch: hosted callers used relative paths,
while generated controls used absolute WORK paths. Environment/data branches are
ruled out by same-file relative/absolute replay and successful Darwin/Linux
verification; no multi-condition root-cause AND gate is needed.

why_not_caught: policy.lane_equality.controls covered only absolute arguments and therefore missed the actual callers' path shape.
recurrence_guard: tests/policy/tests.cmake policy.lane_equality.controls runs the existing positive and all seven named negative cases with both absolute and relative evidence directories through the real comparator in ci.
semantic_index: skipped; MemPalace CLI/tool is unavailable. knowledge-base.md is the durable fallback.
