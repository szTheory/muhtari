---
created: 2026-10-23T18:30:49Z
title: Confirm quality metric retirement against authentic hosted head and event base
area: ci
trigger_when: next implementation PR or explicit heavy CI run, before Phase 6 ship
files:
  - tests/cmake/quality.cmake
  - tests/cmake/quality_evidence.cmake
  - tests/cmake/quality_join.cmake
  - .github/workflows/ci.yml
---

## Problem

Quick task261023-jkd retired the LOC metric and passed local609 ordinary/6 release
tests plus direct/joined legacy-base controls. Authentic hosted acceptance has
not run for this change. Controlled observations are not six-platform evidence.

## Action and completion

At the next ordinary implementation PR, collect the exact head SHA, event-base
SHA and CI run URL. Use existing CI; add no job, lane or reporting service. Confirm
the actual six native plus hygiene/ASan observations, original raw base-policy
identity, surviving environment equality, quality aggregate and CI-required
result. Record the required performance artifact when the event requests it.
For a legacy base, confirm its retained gate_lines key does not block retirement;
for a migrated base, confirm ordinary comparisons still work. Do not fabricate
an authentic legacy-base result from a new-base run or rewrite old artifacts.

Use `gh run list --workflow ci.yml --commit <head>` and `gh run view <run>`;
record commands, exits and URL in the quick summary or Phase 6 acceptance summary.
Close only after actual applicable acceptance succeeds; keep any unrun dimension
explicit. A missing/failed required result is for the agent to investigate, not
owner UAT. No automatic timeout increase, allowance or safety bypass is authorized.

This is an existing integration acceptance obligation, not a new requirement or
permission to reopen general schema/quality design. Phase 6 may resume its real
prerequisite locally; ship still needs actual hosted acceptance.

## Completed 2026-10-23

Actual event base617dfa762ddb649839b925be5aa582180066fa78 retains the legacy key.
Source head25919fe822cd6cc23a6984c13ac826796e7d9cfe and its identical-tree PR
merge2fe713c1074a63bbf89384d56200bd48060c0549 have authenticated six-native,
hygiene and ASan observations with equal current/base environments. Every raw
base-policy file is byte-identical to that Git object; current observations omit
the retired metric. No surviving typed, identity, reason or bound check is removed.

`gh run view 40031550434 --json status,conclusion,jobs` exited0/success:
https://github.com/szTheory/muhtari/actions/runs/40031550434 .
Its unchanged nine-artifact quality join, PERF_REQUIRED=ON, exited0; current/base
worst ZIP375270/399385 both fit399937. The actual required performance artifact
measures3837262 Ir/frame for head and both independent base repeats. Called CI
in https://github.com/szTheory/muhtari/actions/runs/40031550534 independently
passes its eight-artifact join and CI-required at the exact source head.
Commands, exits and component sizes are in the Phase6 plan09 summary; no logs
are committed. This authentic event exercised the legacy-base branch. The
migrated-base branch retains passing producer/join controls; a hosted event
whose base is already migrated is not claimed here.
