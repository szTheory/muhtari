---
phase: 05-pipeline-tune-up
reviewed: 2026-10-22T23:52:49Z
depth: standard
files_reviewed: 40
files_reviewed_list:
  - .github/workflows/be-equality.yml
  - .github/workflows/ci.yml
  - .github/workflows/digest.yml
  - .github/workflows/nightly.yml
  - .github/workflows/release.yml
  - AGENTS.md
  - CMakePresets.json
  - README.md
  - cmake/toolchains/ppc-linux-gnu.cmake
  - docs/emulation-tests.md
  - docs/hardware/audio.md
  - docs/maintaining.md
  - docs/quality.md
  - docs/testing.md
  - pins.txt
  - quality.json
  - scripts/digest.sh
  - scripts/publish.sh
  - src/synth.c
  - tests/audio/tests.cmake
  - tests/built/tests.cmake
  - tests/cmake/archive_contents.cmake
  - tests/cmake/ci_required.cmake
  - tests/cmake/gate_count.cmake
  - tests/cmake/lane_equality.cmake
  - tests/cmake/quality.cmake
  - tests/cmake/quality_evidence.cmake
  - tests/cmake/quality_join.cmake
  - tests/compat/allow.txt
  - tests/core/test_core.c
  - tests/hygiene/tests.cmake
  - tests/install/tests.cmake
  - tests/inventory/ci.txt
  - tests/perf/tests.cmake
  - tests/policy/digest.cmake
  - tests/policy/gate-lines.txt
  - tests/policy/publish.cmake
  - tests/policy/quality.cmake
  - tests/policy/tests.cmake
  - tests/runner/tests.cmake
findings:
  critical: 0
  warning: 2
  info: 0
  total: 2
status: issues_found
---

# Phase 5: Code Review Report

**Reviewed:** 2026-10-22T23:52:49Z
**Depth:** standard
**Files Reviewed:** 40
**Status:** issues_found; two deferred tooling seeds under AGENTS.md rule 7, including the attributed final-audit addendum

## Summary

Reviewed the actual phase changes from `origin/main` through
`580005192388a2faad5088f964da36008299a11c`, including the publisher and its
workflow callers, CLI provisioning, digest, aggregate policy/evidence, native/BE
equality, arithmetic correction, associated controls and documentation. Planning
artifacts supplied context and accepted-history boundaries; they were excluded
from the source-file count. No structural pre-pass was supplied.

The owner-approved D-35 scope and fixed allocation supersede the older 384-line
limit. The review does not reopen accepted earlier slices, promote advisory
lanes, add a requirement, or authorize another review/gap round. The concurrent
closeout correction to the publisher-control timeout and its one counted comment
line, coordinated 15281 tightening, and corrected performance documentation were
also read. The stale report-only sentence is therefore not an open finding.

No substantiated blocker was found in the changed emulator arithmetic or
publisher safety behavior. The remaining finding concerns a delayed-release
quality-baseline edge and is a tooling seed, as rule 7 requires. This report
does not accept the phase or claim current hosted publication, install, cleanup
or final CI acceptance; those remain the orchestrator's evidence obligations.

## Narrative Findings (AI reviewer)

## Warnings

### WR-01: Delayed release CI can select its own commit as the independent base

**Classification:** WARNING
**Disposition:** Tooling seed under AGENTS.md rule 7; no new phase requirement.
**File:** `tests/cmake/quality_evidence.cmake:135-145`
**Related:** `.github/workflows/ci.yml:607-608`;
`.github/workflows/release.yml:184-192`.

**Issue:** Release-called CI checks out release-please's release SHA, which the
release workflow explicitly allows to precede the triggering push. The quality
adapter nevertheless takes its baseline from that push event's `before` field.
When a release for commit R is created by the immediately following push, both
the selected checkout and `before` can be R. The adapter then rejects the run at
line 145 because the baseline equals the measured commit. The aggregate caller
independently makes the same event-base selection. A valid delayed release thus
cannot reach publication through this path, even when its product tests pass.

**Evidence:** Ran the actual adapter against the existing 610-test/six-release-test
build with a synthetic push-event fixture whose `before` equals the selected
release SHA. Inventory checks and gate counting completed, then the adapter
exited 1 with `quality_evidence: independent trusted event base required`.
This is a local edge-case reproduction, not a claim about a completed hosted
release. The separate source-run field in this probe used a synthetic fixture URL.

**Fix:** In a future tooling change, resolve the independent base for an explicit
release checkout from the authenticated selected release commit when it differs
from the push's target commit, and pass that same resolved base to both the
producer and aggregate. For example, validate the release SHA against the trusted
release output and resolve its first parent for this delayed-release case;
preserve event-base selection for ordinary PR/push CI. Exercise the existing
caller with release R, triggering push R+1, and `before=R` so this valid case
gets distinct, matching producer/aggregate identities without bypassing baseline
validation.

## Review evidence and limits

- `cmake -DROOT=<repository> -DWORK=<absolute ignored review fixture> -P tests/policy/publish.cmake`
  — exit 0. Ran the real script's lifecycle, supplied-ID, complete-page,
  HTTP/status/body, short-circuit, version, genuine/downloaded signer/ref,
  changed-byte and actual workflow-output controls outside CTest's old 30-second
  bound. Fake transport checks argument/control flow, not cryptographic validity.
- `cmake -DROOT=. -DWORK=build/plan-evidence/code-review-quality -P tests/policy/quality.cmake`
  — exit 0 before the coordinated 15281 follow-up; covered the actual judge and
  aggregate controls. The follow-up changes were subsequently inspected.
- `build/ci/tests/muhtari-test-core audio.silence` — exit 0; includes the changed
  signed-rounding cases, interpolation/bounds and production reconstruction.
- `sh -n scripts/publish.sh` and `sh -n scripts/digest.sh` — exit 0.
- Actual delayed-release probe: `env GITHUB_EVENT_PATH=<ignored synthetic push fixture>
  GITHUB_EVENT_NAME=push cmake -DROOT=. -DBUILD=build/ci
  -DOUT=build/plan-evidence/code-review-release-observations.json
  -DSHA=580005192388a2faad5088f964da36008299a11c -DCONFIG=ci -DBUILD_BASE=ON
  -DSOURCE=https://github.com/example/public/actions/runs/1
  -P tests/cmake/quality_evidence.cmake` — exit 1 at the intended equal-base
  rejection after 610/six inventory verification and physical count 15281.

Local probes have no hosted run URL. The supplied direct run
[39874135765](https://github.com/szTheory/muhtari/actions/runs/39874135765) and
called/rehearsal run
[39874135839](https://github.com/szTheory/muhtari/actions/runs/39874135839) are
not accepted as successful current-head evidence by this review. The orchestrator
reported publisher-control 30-second timeouts in those runs and is obtaining
uncensored measurements under the existing approved closeout.

Only this report was added to tracked source/planning scope by the reviewer;
review fixtures and observations remain ignored. No source was modified and no
commit was created by the reviewer.

---

_Reviewed: 2026-10-22T23:52:49Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Subsequent integration verification

The orchestrator's hosted runs exposed the native checksum-format fixture mismatch
and expanded-control/job bounds after this standard review. The integration fixes
retain every control, seed checksums with the host shell tool, and schedule the
measured longest control first. The matched local ordering experiment executes the
same610 tests in each sample; final full/inventory and genuine hosted acceptance
are recorded in15/09 summaries and checked by the phase verifier. This paragraph
does not claim another independent code-review round or retrofit the review date.

### COST-01: Retry metadata can distort monthly execution cost

**Classification:** WARNING
**Author:** Orchestrator final-evidence addendum,2026-10-23; subsequent to the independent standard review.
**Disposition:** Deferred tooling seed under AGENTS7/D-35; no new phase requirement or fix round.
**File:** `scripts/digest.sh:110`; aggregation at134–140 and monthly seconds at164–175.

The collector retains distinct retry job IDs, including metadata-only copies whose
created_at follows their copied old start/end. The monthly filter then treats each
interval as another execution and can also subtract negative skipped-job intervals.
Real run39879193669 has original120172221128 and copied120180716506, both reporting
209s; the copy was created after the original execution finished. Running the actual
monthly jq filter on these two authenticated API records emits418s, rather than one
observed209s execution. `python3 build/plan-evidence/release-continuation/prove-copied-cost.py`
exits0 and requires that exact reproduction. Raw report totals are therefore not
accepted cost evidence. The closeout audit excludes confirmed copies and nonexecuted
skipped placeholders. This does not change the reporter or claim its bug is fixed;
a future scoped fix must add controls for genuine API retry/skip semantics.

The frontmatter now totals both findings; the initial independent review contained
WR-01 only. The addendum preserves that authorship/date and is not another round.
