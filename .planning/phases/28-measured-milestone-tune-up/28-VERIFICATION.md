---
phase: 28-measured-milestone-tune-up
verified: 2026-11-19T19:19:03Z
status: passed
score: 10/10 truths verified
covered_files:
  - ".github/workflows/ci.yml"
  - ".github/workflows/release.yml"
  - ".planning/LESSONS.md"
  - ".planning/REQUIREMENTS.md"
  - ".planning/ROADMAP.md"
  - ".planning/STATE.md"
  - ".planning/WINDOWS.md"
  - ".planning/milestones/v3.0-phases/14-proven-frontend-states-and-responsive-play/14-07-SUMMARY.md"
  - ".planning/phases/28-measured-milestone-tune-up/28-01-PLAN.md"
  - ".planning/phases/28-measured-milestone-tune-up/28-01-SUMMARY.md"
  - ".planning/phases/28-measured-milestone-tune-up/28-02-PLAN.md"
  - ".planning/phases/28-measured-milestone-tune-up/28-02-SUMMARY.md"
  - ".planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md"
  - ".planning/phases/28-measured-milestone-tune-up/28-REVIEW.md"
  - ".planning/phases/28-measured-milestone-tune-up/28-TUNEUP.md"
  - "build/ci-run-attempt-2.json"
  - "docs/maintaining.md"
  - "docs/testing.md"
  - "scripts/classify-changes.sh"
  - "scripts/classify-pr-paths.jq"
  - "scripts/hygiene.sh"
  - "tests/cmake/budgets.cmake"
  - "tests/cmake/quality_evidence.cmake"
  - "tests/hygiene/tests.cmake"
  - "tests/inventory/hygiene.txt"
  - "tests/policy/quality.cmake"
  - "tests/tests.cmake"
  - "tools/check_ci_run.cmake"
covered_digest: "v4:sha256:6eb2c77423a936ca7c465af70c74373b247a017fb653844d2290342a03da346f"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 0
  total: 0
  not_honored: []
re_verification:
  previous_status: passed
  previous_score: 8/8
  gaps_closed: []
  gaps_remaining: []
  regressions: []
---

# Phase 28: Measured milestone tune-up Verification Report

**Phase Goal:** Maintainers can rely on current source-qualified release/CI evidence and an accurate deferred-items register before extending runner path behavior.
**Verified:** 2026-11-19T19:19:03Z
**Status:** passed
**Re-verification:** Yes — after Phase 28 requirement-metadata correction and completion of dependent Phase 29.

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | Maintainers can identify the published stable release and current main CI source, distinguish accepted evidence from skipped work and failures, and keep release-PR failure separate from publication acceptance. | ✓ VERIFIED | `28-TUNEUP.md` records v0.17.0 and its source, the v11.0 main CI pass on `119e6e30e9f29644a4c555a34172c35574cd7f67`, and the unshipped v0.18.0 release PR #82. The two pre-change PR runs failed in the independent-base Windows ARM64 quality step; neither run exercises the new metadata classifier or proves the path correction. |
| 2 | Maintainers can inspect queue precision, critical paths, job durations, skipped/cancelled work and evidence limits beside each conclusion. | ✓ VERIFIED | `28-TUNEUP.md` records main CI at 45m11s, the 41m34s ARM64 job and report-only WASM tail; it records PR #82 attempts at 44m03s and 42m28s, the 42m08s second ARM64 rerun, five-second rerun dispatch-to-job delay, reused jobs, skipped dependents and no cancellations. The `CI required` roll-up and failure limits are explicit. |
| 3 | The maintenance disposition covers performance, CI/CD, ease of use, architecture, documentation and upgrade path; it keeps unsupported timeout/topology/cache changes out while applying only repeated-evidence-supported CI corrections. | ✓ VERIFIED | `28-TUNEUP.md` preserves all six dimensions: four metadata PRs support one narrow pre-merge matrix skip; the main-run sample does not justify timeout, topology, cache or report-only coverage changes. Compatibility, hash equality, artifact joins, required CI/equality and publication gates remain intact; Phase 29 owns Windows namespace behavior. |
| 4 | The shared register reports 52 total, 16 open, 0 waived and 36 fixed while preserving every other byte, entry and verdict. | ✓ VERIFIED | `gsd_run windows status --raw` exited 0 with 52/16/0/36. A read-only Node comparison against blob `fe94888ffdfb0deeed942adaf97dd4f110737a4d` exited 0: current `.planning/WINDOWS.md` equals the baseline with only `open_count` 17→16 and `fixed_count` 35→36. The inherited id 51 Markdown/JSON mirror difference is disclosed and unchanged. |
| 5 | Cheap preservation, syntax, documentation and planning checks precede longer work, and local results remain distinct from hosted-source acceptance. | ✓ VERIFIED | The earlier configured local CI receipt remains bound to task commit `d0c2f04` (exit 0, 736/736 tests, three packages and 8/8 release checks). The follow-up ran classifier and quality-policy controls, the 31-test hygiene workflow, docs budgets, `act -l`, shell syntax and tree/whitespace checks. The exact-source audit for PR #82 exits 1 because the hosted required gate is red; this is recorded as a failed gate, not a local pass or v11 release acceptance. |
| 6 | Phase 28 execution metrics and active plan index agree with the completed summaries. | ✓ VERIFIED | The prior carried-forward gap is closed. The focused Node state assertion exited 0; `.planning/STATE.md` reports 2/2 plans and `.planning/ROADMAP.md` indexes both plans as executed. `validate.consistency --raw` exited 0 (`passed`), and `roadmap.analyze --raw` reports Phase 28 plan_count 2 and summary_count 2. |
| 7 | P01 retains its measured 39 minutes, 2 tasks and 5 files; P02 contributes only its measured 18 minutes, 2 tasks and 2 files; aggregate metrics derive from those summaries. | ✓ VERIFIED | `28-01-SUMMARY.md` and `28-02-SUMMARY.md` each record their elapsed duration and task/file counts. STATE retains P01 at 39/2/5, records P02 at 18/2/2, and reports 57 min total and 28.5 min average across 2/2 plans. The focused state assertion checks these values against completed summaries and the active plan files. |
| 8 | Phase 29 followed Phase 28's required dependency; Phase 28's own acceptance was advanced only after verification. | ✓ VERIFIED | `ROADMAP.md` records Phase 29 as dependent on Phase 28, with both phase records complete in order. The Phase 28 summary lists TUNE-15/TUNE-16 in `requirements-completed`, and `REQUIREMENTS.md` marks both complete. This phase-order status does not imply the v11 release gate or milestone closeout is green. |
| 9 | Only the exact same-repository release-please metadata PR can skip the redundant pre-merge matrix; mismatched paths, counts, branch identity, line feeds or API failure keep heavy CI enabled, and publication still requires exact-source CI and equality. | ✓ VERIFIED | `ci.classify` covers exact and mismatched paths/counts/flags; `classify-pr-paths.jq` rejects line feeds in current and prior filenames and emits the fail-closed sentinel. The workflow-guard mutation test and partial-API failure check pass. `release.yml` keeps exact-SHA CI/equality before publish. The current hosted PR #82 run used the pre-change workflow and does not validate the new skip. |
| 10 | The Windows independent-base quality worktree uses `RUNNER_TEMP`, while non-Windows configs and local invocations without that variable retain their prior path behavior and quality comparison controls. | ✓ VERIFIED | `quality_base_tree_path()` selects `RUNNER_TEMP` only for `ci-msvc` when the variable exists; other configurations and local runs without it retain `${BUILD}/quality-base`. The focused `policy.quality.controls` test passed and checks Windows temp selection plus the non-Windows fallback. The correction has no post-change hosted Windows evidence: PR #82 attempt 2 failed before it was applied, so SMB acceptance and the release gate remain unverified/red. |

**Score:** 10/10 truths verified (0 present, behavior-unverified).

### Deferred Items

None. Phase 29 was the planned dependent implementation phase and has since completed after Phase 28; no Phase 28 requirement was deferred for acceptance.

### Advisory (New Scope, Unevidenced)

None. Re-verification found no unevidenced new-scope blocker.

## Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `.planning/phases/28-measured-milestone-tune-up/28-01-PLAN.md` | Plan 01 truth, artifact and link contracts | ✓ VERIFIED | Present and substantive; its five must-have truths and artifact/link declarations were read and checked. GSD artifact query passed 4/4. |
| `.planning/phases/28-measured-milestone-tune-up/28-01-SUMMARY.md` | Measured execution summary with TUNE-15/TUNE-16 coverage | ✓ VERIFIED | Present; 39 min, 2 tasks, 5 files; `requirements-completed` lists TUNE-15 and TUNE-16; block-style coverage for both has `human_judgment: false`. Commit verification passed. |
| `.planning/phases/28-measured-milestone-tune-up/28-02-PLAN.md` | Gap-closure truth, artifact and link contracts | ✓ VERIFIED | Present and substantive; its three must-have truths and artifact/link declarations were read and checked. GSD artifact query passed 2/2. |
| `.planning/phases/28-measured-milestone-tune-up/28-02-SUMMARY.md` | Measured state-metrics closeout with TUNE-15 coverage | ✓ VERIFIED | Present; 18 min, 2 tasks, 2 files; `requirements-completed: []`; block-style TUNE-15 coverage and `human_judgment: false`. Focused command/exit evidence is included. Commit verification passed. |
| `.planning/phases/28-measured-milestone-tune-up/28-TUNEUP.md` | Source-qualified receipts, dispositions and local evidence | ✓ VERIFIED | Includes historical receipts, current main CI pass, PR #82's pre-change failed attempts, the locally verified Windows baseline-path correction, matrix-cost evidence and release-gate limits. |
| `.planning/phases/28-measured-milestone-tune-up/28-REVIEW.md` | Review disposition for the CI classifier and regression controls | ✓ VERIFIED | Current code review is clean; the newline-path spoof was closed with an API-path filter and regression cases. |
| `.github/workflows/ci.yml`, `scripts/classify-pr-paths.jq`, `scripts/classify-changes.sh` | Narrow, fail-closed release-please classifier | ✓ VERIFIED | Requires the same-repository release branch and `main` base; rejects LF in current/prior paths; only the exact unique 3-file set and matching count can skip heavy jobs. Hygiene, title and `CI required` remain. |
| `.github/workflows/release.yml` | Exact-source release checks before publication | ✓ VERIFIED | Reusable CI and equality receive the release SHA; `publish` requires both to pass. |
| `tests/hygiene/tests.cmake`, `tests/inventory/hygiene.txt`, `tests/tests.cmake` | Recurring classifier and workflow-guard regression checks | ✓ VERIFIED | Both CTest checks are registered in hygiene and passed 2/2; controls include line-feed path spoofing and a weakened-guard mutation. |
| `tests/cmake/quality_evidence.cmake`, `tests/policy/quality.cmake` | Isolated Windows quality-baseline worktree selection | ✓ VERIFIED | Uses `RUNNER_TEMP` for `ci-msvc` and the prior path elsewhere; policy controls pass. Windows SMB acceptance remains unproven. |
| `.planning/WINDOWS.md` | Correct register counts over preserved entries | ✓ VERIFIED | Existing validator reports 52/16/0/36; independent baseline-blob comparison proves only the two authorized count substitutions. |
| `.planning/STATE.md` | Accurate execution position and plan-derived metrics | ✓ VERIFIED | Phase 28 remains 2/2; its 57-minute aggregate and per-plan durations match completed summaries. Current state records Phase 29 complete after Phase 28. |
| `.planning/ROADMAP.md` | Phase 28 plan index and Phase 29 dependency/status | ✓ VERIFIED | Phase 28 is 2/2 and Phase 29 is 3/3; both are complete, and Phase 29 depends on Phase 28. |

All `verify.artifacts` results were positive (28-01: 4/4; 28-02: 2/2). Both plan files and summaries are substantive; no stub or orphaned phase artifact was found.

## Key Link Verification

| From | To | Via | Status | Details |
|---|---|---|---|---|
| `.planning/phases/28-measured-milestone-tune-up/28-TUNEUP.md` | `.planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md` | Five locked receipts preserve exact source and evidence class | WIRED | `verify.key-links` passed; exact receipt identifiers and source hashes appear in the report and CONTEXT. |
| `.planning/WINDOWS.md` | `.planning/phases/28-measured-milestone-tune-up/28-TUNEUP.md` | Existing validator and baseline-blob preservation proof | WIRED | `verify.key-links` passed; the target documents the validator and the exact two-field comparison. |
| `.planning/STATE.md` | `.planning/phases/28-measured-milestone-tune-up/28-01-SUMMARY.md` | P01 duration and aggregate completed-plan accounting | WIRED | `verify.key-links` passed; the 39-minute P01 record is present and included in the derived aggregate. |
| `.planning/STATE.md` | `.planning/phases/28-measured-milestone-tune-up/28-02-SUMMARY.md` | Gap-plan duration included after measured completion | WIRED | `verify.key-links` passed; the 18-minute P02 record is present and included in the 57-minute aggregate. |
| `.github/workflows/ci.yml` | `scripts/classify-pr-paths.jq` → `scripts/classify-changes.sh` | Paginated PR filenames are checked before classification | WIRED | LF in current/prior names or partial API failure emits the failure sentinel; the classifier maps it to `heavy=true`. |
| `tests/tests.cmake` | `tests/hygiene/tests.cmake` and `tests/inventory/hygiene.txt` | Hygiene lane registers classifier tests | WIRED | Both focused tests ran successfully; mismatch, newline, API sentinel and workflow-guard mutation controls are covered. |
| `.github/workflows/release.yml` | Reusable CI and equality workflows | Release SHA is checked before publish | WIRED | Both receive the immutable release target SHA; publication requires both checks to succeed. |
| `tests/policy/quality.cmake` | `tests/cmake/quality_evidence.cmake` | Baseline worktree root selection is asserted | WIRED | The focused policy test covers Windows temp-root selection and the non-Windows prior-path fallback; it does not simulate Windows SMB acceptance. |

## Data-Flow Trace (Level 4)

Not applicable. The phase changes maintainer documentation and planning metadata; it introduces no rendered runtime data or application data source.

## Behavioral Spot-Checks

| Behavior/check | Command or evidence | Result | Status |
|---|---|---|---|
| Derive Phase 28 metrics from active plans and completed summaries | Focused Node assertion recorded verbatim in `28-02-PLAN.md` Task 1 and `28-02-SUMMARY.md` | Exit 0; aggregate, P01 row and Phase 29 `0/TBD` assertions passed. | ✓ PASS |
| Planning consistency | `gsd_run query validate.consistency --raw` | Exit 0; output `passed`. | ✓ PASS |
| Current plan index and Phase 29 dependency/status | `gsd_run query roadmap.analyze --raw` | Exit 0; Phase 28 has 2 plans/2 summaries and Phase 29 has 3/3; both roadmap records are complete, and Phase 29 depends on Phase 28. | ✓ PASS |
| Canonical register counts | `gsd_run windows status --raw` | Exit 0; canonical ledger is 52 total, 16 open, 0 waived, 36 fixed. | ✓ PASS |
| Whole-file register preservation | Node comparator using `git cat-file -p fe94888ffdfb0deeed942adaf97dd4f110737a4d` and current `.planning/WINDOWS.md` | Exit 0; all bytes match except the two required count changes. | ✓ PASS |
| Planning whitespace | `git diff --check HEAD -- .planning` | Exit 0; no diagnostics. | ✓ PASS |
| Context decision coverage | `gsd_run query check.decision-coverage-verify .planning/phases/28-measured-milestone-tune-up .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md` | Exit 0; skipped because there are no trackable decisions (0 total). | ✓ PASS |
| Release-PR classifier and fail-closed controls | `ctest --test-dir build/hygiene --output-on-failure -R '^ci[.]classify($|[.])'` | Exit 0; classifier and workflow-guard mutation tests passed 2/2, including current/prior LF names. | ✓ PASS |
| Windows quality-baseline path policy | `ctest --test-dir build/ci --output-on-failure -R '^policy[.]quality[.]controls$'` | Exit 0; focused control passed in 25.48s on this verifier run. TUNEUP separately records the earlier 21.60s pass. | ✓ PASS (local policy only) |
| Quality evidence producer parse | `cmake -DQUALITY_BASE_WORKFLOW_ONLY=ON -P tests/cmake/quality_evidence.cmake` | Exit 0. | ✓ PASS |
| Current CI workflow graph | `act -l -W .github/workflows/ci.yml` | Exit 0; current YAML lists `changes`, `hygiene` and `CI required`. No workflow job/container was run. | ✓ PASS |
| Follow-up hygiene workflow | `cmake --workflow --preset hygiene` | `28-TUNEUP.md` records exit 0, 31/31 tests, including classifier controls; tree and whitespace hygiene passed. | ✓ PASS (recorded) |
| Documentation budget | `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` | Exit 0; 25 files within budget. | ✓ PASS |
| Shell syntax and whitespace | `sh -n scripts/classify-changes.sh`; `git diff --check` | Both exit 0. | ✓ PASS |
| Prior configured local CI | Recorded command `cmake --workflow --preset ci`; exit 0 in `28-TUNEUP.md`, bound to task commit `d0c2f04`; not rerun. | Reported configure/build, 736/736 CI tests, three package outputs, 8/8 release checks. Existing `LastTest.log` is only the final 8-entry test invocation (8 `Test Passed`, no failure file); it does not independently reconstruct the 736-test output. | ✓ PASS (carried forward) |
| Initial verifier baseline lookup | Initial command `git show fe94888ffdfb0deeed942adaf97dd4f110737a4d:.planning/WINDOWS.md` | Exit 128 because the supplied identifier is a blob, not a commit. Corrected to `git cat-file -p` and the comparator passed; no file was changed. | Corrected |

No additional hosted retry, host/device check or full local `cmake --workflow --preset ci` was started during this re-verification. Existing run 44575006882 was inspected read-only; its exact-source audit remains red. Local policy and hygiene results do not establish Windows SMB acceptance or a new release publication.

## Current v11 Release Gate

Main CI for Phase 29's merged source passed in run 44574938376. Main Release run 44574938682 opened v0.18.0 PR #82 but did not publish. PR #82 attempt 2 failed on the pre-change workflow: the primary Windows ARM64 suite passed `runner.trace.paths`, while the independent-base quality run failed the same test during its UNC state save; `quality` and `CI required` failed closed. The exact-source audit command exits 1 and reports those failed jobs. The correction that moves only the Windows baseline worktree to `RUNNER_TEMP` has local policy evidence, but no post-change hosted Windows run. **The v11 release gate is still red; this Phase 28 evidence report does not mark the release or milestone closeout green. A fresh exact-source hosted run must pass before publication.**

The required `verification.status` self-check exited 0 and returned `passed`.

## Probe Execution

Not applicable. This is a documentation and register phase; neither plan declares a probe, migration, or new tooling probe.

## Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| TUNE-15 | 28-01, 28-02 | Source-qualified current release/CI evidence, timing and preflight detail, proportionate maintenance disposition, and accurate execution metrics | ✓ SATISFIED | `28-TUNEUP.md` records the main CI pass, unshipped PR #82 failures, exact costs, the local Windows path correction and its evidence limits; focused controls, STATE assertion and GSD consistency/roadmap checks pass. |
| TUNE-16 | 28-01 | 52-entry register with 16 open, 0 waived, 36 fixed while preserving all item text/verdicts | ✓ SATISFIED | `windows status --raw` exit 0 plus baseline-blob comparison exit 0 proving only the two count fields changed. |

`REQUIREMENTS.md` marks TUNE-15 and TUNE-16 complete, consistent with this passed verification and the `requirements-completed` metadata in 28-01. **Orphaned requirements:** none; Phase 28 owns only TUNE-15 and TUNE-16, and both are covered by its plans.

### Test Quality Audit

| Test File | Linked Requirement | Active | Skipped | Circular | Assertion Level | Verdict |
|---|---|---:|---:|---:|---|---|
| `tests/hygiene/tests.cmake` | TUNE-15 | 2 | 0 | N/A | Exact/mismatch classifier matrix, LF-name rejection and workflow-guard mutation | ✓ PASS |
| `tests/policy/quality.cmake` | TUNE-15 | 1 | 0 | N/A | Windows temp-root selection and non-Windows fallback assertion | ✓ PASS |

TUNE-16 is validated by the existing ledger validator and exact-byte comparison, not a new product test. The focused state assertion passed, the CI controls are present in the hygiene inventory, and the 31-test hygiene run is recorded. The path-selection test does not prove Windows SMB behavior; the hosted release gate remains explicitly red.

### Decision Coverage

No trackable decisions in `28-CONTEXT.md`; the decision-coverage query returned `skipped: true`, `total: 0`, `honored: 0`, `not_honored: []`.

## Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| Historical Phase 28 plan/state snapshot | 66, 94; 41 in the original report | Phase 29 `0/TBD` plan count | ℹ️ Info | Accurate when Phase 28 was verified, before its dependent Phase 29 began. It is superseded by the current roadmap's 3/3 Phase 29 completion and is not an outstanding Phase 28 marker. |

No emulator product code changed. The follow-up changes CI classification and a Windows-only quality-evidence worktree location, with recurring controls for both. Code review is clean. No unresolved Phase 28 `TBD`, `FIXME`, or `XXX` marker, empty implementation or unwired test was found. The old Phase 29 `0/TBD` plan-count snapshot was accurate before that dependent phase began; current roadmap state records both v11.0 phases complete, while shipment remains blocked by the red release gate above.

## Human Verification Required

None. This phase has no user-facing flow or owner-only judgment. The remaining Windows acceptance is an automated exact-source CI/release gate, not a UAT request; it is red and must pass before publication.

## Gaps Summary

Phase 28's STATE metrics aggregate 2/2 plans, 57 minutes total and 28.5 minutes per plan, preserving P01's 39-minute measurement. Both summaries have the required coverage blocks; 28-01 lists TUNE-15/TUNE-16 as completed, and the requirements table agrees. The register remains 52/16/0/36 with only its two aggregate fields changed. The classifier is narrowly guarded and its local fault controls pass; four repeated metadata PRs support skipping only the redundant pre-merge matrix. The separate Windows baseline path correction has local policy coverage but no post-fix hosted proof. All ten observable truths pass for Phase 28; PR #82's required gate is red, so v11 release publication and milestone closeout remain blocked.

---

_Verified: 2026-11-19T19:19:03Z_
_Verifier: the agent (gsd-verifier)_
