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
