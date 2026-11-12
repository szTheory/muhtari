---
phase: 20-complete-ym2151-homebrew-sound
reviewed: 2026-11-12T15:08:24Z
depth: standard
files_reviewed: 6
files_reviewed_list:
  - src/internal.h
  - src/state.c
  - src/synth.c
  - tests/audio/test_ym2151.c
  - tests/core/test_state.c
  - .planning/LESSONS.md
findings:
  critical: 0
  warning: 0
  info: 0
  total: 0
  necessity: 0
status: clean
---

# Phase 20: Code Review Report

**Reviewed:** 2026-11-12T15:08:24Z
**Depth:** standard
**Files Reviewed:** 6
**Status:** clean

## Summary

Re-reviewed the rewind correction and resolved CR-01. After a successful state apply, the observer now clears queued pending/carry records and copies the restored `synth.raw` values into its transition cache, without resetting its timing cursor. This prevents abandoned future values from suppressing replayed transitions. The regression captures a control frame, advances through an abandoned future, queues a same-value future carry, restores the checkpoint, and compares the replay's count, sequence hash, source mask, and per-channel counts and polarity totals with the control. Buffer refusal still occurs before observer processing, so a refused frame does not consume carry. No remaining issue found by source review. Tests were not run, as requested.

## Narrative Findings (AI reviewer)

### Resolved Findings

#### CR-01: Rewind leaves discarded carry levels in the raw transition cache

**File:** `src/synth.c:318-325,362-372`; `src/state.c:1436-1445`
**Severity:** BLOCKER

**Disposition:** RESOLVED in the current working tree. `muhtari__state_payload_apply` calls `muhtari__synth_observe_reset` after applying validated state. The helper clears `count` and `carried`, then synchronizes all observer raw values to restored synthesis state. The replay regression at `tests/audio/test_ym2151.c:1513-1564` compares the replay summary against a control continuation and injects an abandoned future carry with the replayed YM level to exercise the prior suppression case. The timing cursor remains intact. This re-review was source-only; tests were not run.

---

_Reviewed: 2026-11-12T15:08:24Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_
