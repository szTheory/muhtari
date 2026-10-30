---
phase: 09-pal-games-and-audio
plan: "06"
subsystem: testing
tags:
  - pal
  - quality
  - provenance
  - evidence
requires:
  - phase: 09-pal-games-and-audio
    provides: Selected PAL behavior and independent audio bounds through09-05
provides:
  - Exact additive owned PAL manifest comparison permission
  - Genuine recursive producer and unchanged join pass/fault controls
  - Independent local candidate and pre-plan three-ZIP measurements
affects:
  - 09-07
  - 09-08
tech-stack:
  added: []
  patterns:
    - Reuse temporary Git event-base objects and existing authenticated consumers
    - Restore exact raw bytes between independent evidence mutations
key-files:
  created:
    - .planning/phases/09-pal-games-and-audio/09-06-SUMMARY.md
  modified:
    - tests/cmake/quality.cmake
    - tests/policy/quality.cmake
    - docs/quality.md
key-decisions:
  - Extend only the exact named diagnostic alternative with pal; preserve all old raw identities and annotation transitions.
  - Use a renamed owned manifest row until actual PAL admission, then automatically exercise the admitted row.
  - Retain measured report-only archives, all deadlines and lane authority, and the armed test-card performance contract.
requirements-completed: []
coverage:
  - id: D1
    description: Exact owned PAL addition, unchanged manifest and later-base retention pass while old-row and malformed-addition faults fail.
    requirement: PAL-03
    verification:
      - kind: integration
        ref: policy.quality.controls actual recursive producer cases
        status: pass
    human_judgment: false
  - id: D2
    description: Genuine producer outputs join while missing, mixed, forged and incomplete PAL evidence fails independently.
    requirement: PAL-03
    verification:
      - kind: integration
        ref: policy.quality.controls actual quality_join and failed-producer cases
        status: pass
    human_judgment: false
  - id: D3
    description: Full local inventories, package integrity and authenticated candidate versus genuine pre-plan archive measurements remain required.
    requirement: PAL-03
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci and tests/cmake/quality_evidence.cmake
        status: pass
    human_judgment: false
actuals:
  tokens: 4089
  tasks: 2
  commits: 3
  plan_head_before: 4fb630298c5e04334ede8553dfb4c72c7e4158fc
  plan_head_after: 355d34bebddc921a39f3382f012f2e8629f5aa5c
duration: 8min
started: 2026-10-25T18:23:53Z
completed: 2026-10-25
status: complete
---

# Phase 9 Plan 6: PAL Fixture Trust Summary

**Exact owned PAL manifest comparison permission now passes genuine recursive producer and authenticated join controls without changing old identities.**

## Changes and Task Commits

1. **Task1: Admit only the named PAL fixture and preserve every old identity.** RED `7681723` adds actual producer controls; GREEN `0ef76dc` adds only `|pal` to the exact diagnostic-name alternative and documents the boundary. The existing source-owner, nonempty pin, MIT, lowercase64hex and trailer checks, path uniqueness and byte-exact removal relation are unchanged.
2. **Task2: Prove authenticated joins reject incomplete or mixed PAL evidence.** `355d34b` extends the same control with independently mutated copies of actual recursive output and failed-producer checks; documentation distinguishes these controls from hosted acceptance. No producer or join implementation change was necessary.

The PAL controls create genuine temporary parent/addition Git commits and obtain raw trusted manifest/sample identities independently with `git show`. They pass unchanged, additive and later-base cases. Existing-row edit/deletion/reorder, old/new/later duplicate paths, later PAL edit/deletion, unrelated names, wrong owner/license, empty pin, short/uppercase digest, malformed fields/trailer, extra comment and candidate-supplied trust inputs fail. Once the actual PAL row exists, the same control extracts it automatically; until then it renames the admitted owned DMA row for comparison permission only. No new fixture bytes or manifest row enters this plan.

The unchanged join accepts attributable recursive candidate/base output. Eleven independent mutations reject missing lane/file, swapped current/base SHA, wrong run, candidate-normalized manifest identity, changed raw sample identity, forged base SHA, changed raw policy and missing current/base or unavailable archive measurement. Three additional actual producer calls reject failed ordinary/release inventories and a missing component ZIP, asserting that no fresh output was emitted. Controlled eight-lane copies exercise consumer semantics; they are not eight executed hosted lanes. The existing controlled three-file90-byte measurement is distinct from the actual ZIP measurements below.

Every historical control remains, including exact annotation2→3,3→4,4→5, skipped/reverse/duplicate/missing annotations, sample source/tool/digest/numeric/trailer mutations, compiler/platform identities and raw-base authority. The complete production diff is the single `|pal` alternative. API6/ABI1/behavior5, every existing NTSC predicate, quality bounds, lane membership, required measured archives, report-only size policy and armed+1% test-card guard are unchanged. No timeout or COST changed.

## Verification

Every accepted command below exited0; all run URLs are **N/A-local**. No log is committed.

- Task1: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure` —1/1,8.83s.
- Complete expanded Task2 control: `ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure` —1/1,13.21s. Existing deadline remains sufficient locally; no unmeasured hosted timing claim is made.
- Task2 working-source gate: `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake --workflow --preset ci` —652 ordinary checks32.36s,6 release checks0.12s; quality13.18s.
- `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` —three component ZIPs and6/6 release checks0.12s.
- Exact committed source355d34b: the same full workflow command —652 ordinary checks35.79s,6 release checks0.11s; complete quality workload13.66s. This rerun establishes the precise source used by the producer.
- `cmake -DJUNIT=<ordinary-or-release-report> -DINVENTORY=<matching-source>/tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake`, using `-DLABELS=release` instead for release —four independent current/base ordinary/release reports each pass,652+6 on both sides.
- Python standard-library JSON/ZIP assertions over both actual reports and archive sets —exact source SHA, measured status, source `N/A-local`,652 tests, exactly three ZIPs, CRC integrity, actual component sizes/sum and equal full environments pass.
- `git diff --check`, `scripts/hygiene.sh --tree`, and `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 09-06 --commits-only --raw` —exit0. Scope resolves all three task commits on this branch. Normal hooks and existing noreply identity were used.

## Independent Local Package Measurements

`env GITHUB_EVENT_PATH="$PWD/build/ci/quality-pal-trust-local-event.json" node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-pal-trust-local.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DSOURCE=N/A-local -DBUILD_BASE=ON -P tests/cmake/quality_evidence.cmake` —exit0.

The ignored event descriptor names the actual pre-plan4fb630298c5e04334ede8553dfb4c72c7e4158fc. The existing producer independently rebuilt it and ran652+6 checks in31.37s/0.12s; its original quality control took10.92s. Both reports identify their exact source and have equal environment `AppleClang-21.0.0.21000101-Darwin-arm64`, raw manifest SHA256 `62dc23cf0a687032b3c8d82f988c79d926a1814cd7eaf0f993cbb720a002141e` and raw sample SHA256 `406ca181ecad9c64d4030b09203f210b69d5f5c649453f0a5149fc864444cb4b`.

| Source | Library | Libretro | Runner | Total bytes |
|---|---:|---:|---:|---:|
| Candidate355d34b |81761|87472|96375|265608|
| Pre-plan4fb6302 |81753|87472|96375|265600|

The observed difference is8 bytes; compressed bytes remain report-only. This is one local macOS-arm64 slice comparison, not phase-versus-main performance or six-native/hosted acceptance. Local reports use `N/A-local`; synthetic control run URLs are test identities, not hosted runs. After exact-base and clean-tree assertions, `git worktree remove build/ci/quality-base` exited0. Only the original checkout remains.

## TDD Gate Compliance and Failed Experiments

Task1's pre-implementation configure/build/CTest command with `--output-junit quality-pal-red.xml` exited8: the target `policy.quality.controls` executed historical controls and failed specifically when the real producer rejected the intended valid `tests/diag/pal.a78` addition. The unchanged JUnit report and command/exit/target/expected/actual/freshness/semantic assessment are retained only as ignored build evidence. `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/quality-pal-red.json --raw` exited0 with `RED_EVIDENCE_OK`. Semantic inspection confirms intended admission rejection, not a setup, syntax or unrelated failure. RED7681723 precedes GREEN0ef76dc; all subsequent controls pass.

Task2 extends tests of already enforced production contracts. Its first focused run exited8 in10.83s because timestamp-preserving `file(COPY)` did not reliably restore the deliberately corrupted raw-policy copy between independent fault cases. The missing-archive case therefore encountered the preceding trusted-policy mismatch. The fix explicitly reads and writes the exact raw policy/SHA bytes before each case. The corrected complete control passes13.21s and both full workflows pass. This harness failure is preserved and is not product RED evidence; no fake RED/GREEN pair or production modification is claimed for Task2. Runtime `workflow.tdd_mode` remains false.

The initial ordinary-sandbox commit/ledger attempt could not write `.git` and created no commit; the authorized escalated attempt succeeded with normal hooks. No automatic approval rejection or owner action occurred.

## Deviations and Remaining Scope

No scope, architecture, quota, policy authority or acceptance threshold changed. The harness state restoration correction was found and fixed during Task2. Actuals use16355 realized task-diff characters/4, rounded up, and the persisted pre-plan ledger measures three task commits through355d34b; the separate summary commit follows.

This is prerequisite evidence for PAL-03, not requirement completion. Actual owned PAL source/admission and numeric/hash observations remain09-07; that plan must rerun these controls with its genuine row. Full exact-source required hosted/native/equality acceptance remains09-08. Descriptor-less flagged-unverified legal intent retains its provenance/asset judgment boundary; no prohibited emulator source or private asset was opened or introduced. The inherited PAL-03 classification question and unmeasured physical PAL calibration are not resolved by a fixture trust relation.

No known stub, skipped test, unrun local verification, new security surface outside the plan, authentication gate or owner verification remains for this plan. Parent exclusively owns subsequent STATE/ROADMAP updates; neither is staged or changed by this executor. No requirement was marked complete and no later plan or GSD step was launched. Next command in a new session: `$gsd-execute-phase 9`.

## Self-Check: PASSED

All three task files and this summary exist. All three recorded task commits are ancestors of the current phase branch and match the persisted ledger's measured range. Both actual source inventories, independent recursive measurements, ZIP integrity, full local gates and hygiene pass. No task deletion or unexpected untracked file remains. The immediate summary commit completes this executor's output before parent-owned shared-state reconciliation.
