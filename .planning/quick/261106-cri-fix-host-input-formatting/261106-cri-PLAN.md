---
phase: quick-261106-cri
plan: "01"
type: execute
wave: 1
depends_on: []
autonomous: true
files_modified:
  - tests/input/history_pixels.c
  - tests/input/history_pixels.h
  - tests/input/test_input.c
  - tests/retroarch/compare.c
  - tests/retroarch/input_trace_helper.h
  - tests/retroarch/test_input_trace_helper.c
requirements: [PLAY-03]
estimate:
  tokens: 4000
  raw_tokens: 4000
  tasks: 2
  confidence: low
must_haves:
  truths:
    - "The six files rejected by the required hygiene.format job pass the repository's pinned clang-format 21.1.8 check locally."
    - "The repair contains formatter output only and preserves every existing input, comparator, helper, fixture, runtime bound and acceptance predicate."
    - "The local repository hygiene scan passes; the spent Plan 12-12 host attempt and all Phase 12/14 acceptance stops remain intact."
  artifacts:
    - path: tests/input/history_pixels.c
      provides: "Formatted owned-input pixel helper with existing behavior"
    - path: tests/retroarch/test_input_trace_helper.c
      provides: "Formatted existing helper controls without a workload or behavior change"
  key_links:
    - from: .clang-format
      to: the six listed C and header files
      via: "Installed llvm@21 clang-format 21.1.8, matching the CI hygiene lane"
---

<objective>
Repair the exact formatting gate that stopped the newly authorized ordinary host run.
Purpose: restore the required local format contract before any separately authorized future host evidence scope.
Output: formatter-only changes to six files and a quick-task summary with local command exits. This task completes no PLAY requirement or host acceptance.
</objective>

<execution_context>
@~/.codex/gsd-core/workflows/execute-plan.md
@~/.codex/gsd-core/templates/summary.md
</execution_context>

<context>
@AGENTS.md
@.clang-format
@.planning/STATE.md
@.planning/preparation/ENGINEERING.md
@.planning/phases/12-player-ready-delivery-and-playstead-bridge/12-12-HOST-EVIDENCE.md
@.planning/phases/12-player-ready-delivery-and-playstead-bridge/12-12-SUMMARY.md
@scripts/hygiene.sh
</context>

## Scope and evidence

Run 42227540379 rejected these six files in required `hygiene.format` before RetroArch started. Planning-time local `clang-format --dry-run --Werror` using installed llvm@21 version 21.1.8 reproduces the same six-file drift. The root checkout was clean at planning entry, source `451a2a18868d9a1644a697ed86edfadbf4d693e1`; the explicit diff range below covers task commits and the working tree. The calibration factor is 1 with zero samples and confidence low.

Use the current root checkout through the tool working directory. Resolve the already installed formatter as `$(brew --prefix llvm@21)/bin/clang-format` and require version 21.1.8 before editing; do not install a tool or substitute another major. Every project path below is relative to the checkout root. No CTest, full workflow, process inspection, Docker, RetroArch, network action, push, dispatch, PR, merge or release belongs to this quick task. The one Plan 12-12 host attempt remains spent. Leave STATE, ROADMAP, REQUIREMENTS and earlier evidence records to the orchestrator.

<tasks>
<task type="auto">
  <name>Format the owned-input history helper and tests</name>
  <read_first>.clang-format; tests/input/history_pixels.c; tests/input/history_pixels.h; tests/input/test_input.c; AGENTS.md formatting and bounded-work rules</read_first>
  <files>tests/input/history_pixels.c, tests/input/history_pixels.h, tests/input/test_input.c</files>
  <precondition>The root checkout has no unrelated changes, and the installed llvm@21 formatter reports version 21.1.8.</precondition>
  <action>Resolve the installed llvm@21 formatter, check its version, and apply its -i mode only to the three listed input files. Review the resulting diff against the starting source and accept only whitespace, indentation and line wrapping emitted by that formatter. Preserve constants, strings, assertions, generated cartridge data, input-history predicates and all other non-whitespace source bytes. Do not change .clang-format, comments for content, tests or fixtures for behavior, registrations, inventories or budgets. Commit this formatter-only input group normally with repository hooks.</action>
  <acceptance_criteria>The three input files pass the pinned formatter's dry-run check; their diff contains only formatting and names no unrelated file.</acceptance_criteria>
  <verify>
    <automated>sh -c 'formatter="$(brew --prefix llvm@21)/bin/clang-format" && "$formatter" --version | grep -F "clang-format version 21.1.8" && "$formatter" --dry-run --Werror tests/input/history_pixels.c tests/input/history_pixels.h tests/input/test_input.c'</automated>
    <fails_when>The installed formatter is absent or a different version, or any listed input file still has a format violation.</fails_when>
    <automated>git diff --check 451a2a18868d9a1644a697ed86edfadbf4d693e1 -- tests/input/history_pixels.c tests/input/history_pixels.h tests/input/test_input.c</automated>
    <fails_when>The formatter repair introduces whitespace errors.</fails_when>
  </verify>
  <done>The input group is formatter-clean and committed without behavior changes or expanded scope.</done>
</task>

<task type="auto">
  <name>Format the RetroArch controls and verify the complete local repair</name>
  <read_first>.clang-format; tests/retroarch/compare.c; tests/retroarch/input_trace_helper.h; tests/retroarch/test_input_trace_helper.c; scripts/hygiene.sh; the Task 1 diff</read_first>
  <files>tests/retroarch/compare.c, tests/retroarch/input_trace_helper.h, tests/retroarch/test_input_trace_helper.c</files>
  <action>Apply the same pinned formatter's -i mode only to the three listed RetroArch files. Review the diff as formatter output and preserve all comparator predicates, lifecycle handling, trace fields, helper controls, arguments, runtime bounds and strings. Run the pinned dry-run check over all six task files, scoped git diff whitespace checks, and scripts/hygiene.sh --tree. Inspect the final quick-task diff to confirm no source beyond these six paths changed. Commit the RetroArch formatting group with normal hooks. Record local exits and the earlier failed-run URL in the quick summary; do not claim the failed remote source was repaired retroactively or that host acceptance passed.</action>
  <acceptance_criteria>All six listed files pass clang-format 21.1.8, the existing repository tree hygiene scan exits 0, and the final source diff is formatting-only within the six authorized files.</acceptance_criteria>
  <verify>
    <automated>sh -c 'formatter="$(brew --prefix llvm@21)/bin/clang-format" && "$formatter" --version | grep -F "clang-format version 21.1.8" && "$formatter" --dry-run --Werror tests/input/history_pixels.c tests/input/history_pixels.h tests/input/test_input.c tests/retroarch/compare.c tests/retroarch/input_trace_helper.h tests/retroarch/test_input_trace_helper.c'</automated>
    <fails_when>Any authorized file is not clean under the pinned formatter.</fails_when>
    <automated>git diff --check 451a2a18868d9a1644a697ed86edfadbf4d693e1 -- tests/input/history_pixels.c tests/input/history_pixels.h tests/input/test_input.c tests/retroarch/compare.c tests/retroarch/input_trace_helper.h tests/retroarch/test_input_trace_helper.c</automated>
    <fails_when>Any current task change contains whitespace errors.</fails_when>
    <automated>sh scripts/hygiene.sh --tree</automated>
    <fails_when>The existing repository scan reports a hygiene violation. Investigate its retained output locally; do not expand source scope or run a host workflow.</fails_when>
  </verify>
  <done>The six-file formatter gate is locally repaired and committed; the quick summary preserves spent host allowances and pending Phase 12/14 acceptance.</done>
</task>
</tasks>

## Artifacts this task produces

Only formatter output in the six existing source/header files and the quick-task SUMMARY. No symbols, tests, fixtures, APIs, flags, dependencies, runtime metadata or installed artifacts are introduced.

## Source audit

| Source | Scope item | Disposition |
|---|---|---|
| GOAL | Restore the required format gate that prevented the ordinary candidate host observation | Both tasks; no host acceptance claim |
| REQ | PLAY-03 prerequisite hygiene | Local formatting repair only; requirement remains pending |
| RESEARCH | ENGINEERING pins clang-format 21 and committed style | Both tasks use installed 21.1.8 and .clang-format |
| CONTEXT | Preserve bounded attempts, failed evidence and separate Phase 14 stop | No remote or protected workload; parent retains acceptance owners |

<verification>
Use the pinned formatter's read-only check, diff review/whitespace checks and existing repository tree hygiene scan. No behavior changed, so no behavioral tests or full workflow are required in this formatting task.
</verification>

<success_criteria>
The exact six files pass local clang-format 21.1.8 and repository hygiene, with formatter-only commits and an honest quick summary. The prior hosted run remains failed, PLAY-03 remains unproven, and no host allowance is renewed.
</success_criteria>

<output>
Create `.planning/quick/261106-cri-fix-host-input-formatting/261106-cri-SUMMARY.md` with command/exit evidence, normal task commits, `requirements-completed: []` and block-style coverage with human_judgment false. Use N/A-local for local commands and link the existing run as the source of the reported fault. The orchestrator owns current STATE/ROADMAP reconciliation and any later host authorization.
</output>
