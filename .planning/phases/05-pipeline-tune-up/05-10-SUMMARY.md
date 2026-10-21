---
phase: 05-pipeline-tune-up
plan: "10"
status: halted
subsystem: infra
tags: [powerpc, cmake, audio, portability]
requires:
  - phase: 05-01
    provides: Frozen maintenance count and scoped allocation
  - phase: 05-02
    provides: Measured bounds and isolated outputs
provides:
  - Actual PowerPC ABI recovery with unchanged output and fault controls
  - Complete native and PowerPC normal/package inventory evidence
  - Reproduced hosted relative-path equality failure and mandatory halt
affects: [05-04, 05-05, 05-06, 05-07, 05-08, 05-09]
tech-stack:
  added: []
  patterns: [unsigned bounded magnitude division, target-only constant placement]
key-files:
  created:
    - .planning/phases/05-pipeline-tune-up/05-10-HALTED.md
  modified:
    - src/synth.c
    - tests/core/test_core.c
    - cmake/toolchains/ppc-linux-gnu.cmake
    - docs/hardware/audio.md
    - .github/workflows/be-equality.yml
key-decisions:
  - Stop on the first completed full producer equality failure as required by this recovery plan.
  - Preserve all ABI predicates and fixed allocations; do not repair the newly isolated caller defect without a scoped recovery decision.
duration: 20min
completed: 2026-10-21
actuals:
  tokens: 1655
  tasks: 0
  commits: 2
plan_head_before: a2daeb660fd7ca74912fa1a8dd7c58617b863e16
plan_head_after: 96dc316427cbcb686b06b650d4cc16d729c419d3
requirements-completed: []
requirements-contributed: [TUNE-02, TUNE-04, TUNE-05]
coverage:
  - id: target-abi
    description: PowerPC archive satisfies unchanged ABI judges and real audio regression checks
    requirement: TUNE-02
    verification:
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39659029223
        status: pass
      - kind: unit
        ref: core.audio.silence rounding equivalence and production reconstruction, exit 0
        status: pass
    human_judgment: false
  - id: full-inventories
    description: Each native and PowerPC lane executes exactly 607 normal and six release tests
    requirement: TUNE-04
    verification:
      - kind: integration
        ref: Both check_junit selections in Release 39659029223 and Nightly 39659029340, exit 0
        status: pass
      - kind: other
        ref: Independent exact Counter comparison of downloaded release JUnit reports, exit 0
        status: pass
    human_judgment: false
  - id: hosted-equality
    description: Read-only release and nightly producers complete their exact pair comparator
    requirement: TUNE-05
    verification:
      - kind: integration
        ref: Release job 119426862308 and Nightly job 119426862666, comparator exit 1
        status: fail
    human_judgment: false
  - id: final-cleanup
    description: Diagnostic-free final implementation and matching coverage documentation are accepted
    requirement: TUNE-05
    verification:
      - kind: integration
        ref: Not run after mandatory full-producer failure; temporary diagnostic remains
        status: unknown
    human_judgment: false
---

# Phase 5 Plan 10: PowerPC ABI recovery Partial Summary

**Unsigned rounding and read-only counts restore the real PowerPC ABI; full
inventories pass, but the hosted equality caller fails on relative paths.**

This is a halt record, not successful replacement coverage. Neither task meets
every acceptance criterion: Task 1 still requires diagnostic cleanup and Task 2
requires successful final callers and documentation. Progress remains 2/9;
05-04 onward remain blocked. Historical 05-03 records are unchanged.

## Partial commits

1. `f4ce072` — target diagnostic exposes the existing archive's RED before changes.
2. `96dc316` — unsigned magnitude rounding, sign restoration, 19 literal cases
   exercised at both signs, target flag and arithmetic documentation.

The ledger measures two commits between the recorded heads. Actual tokens are
ceil(6620 realized diff characters / 4), excluding halt/tracking metadata.
No tracked file was deleted. Normal hooks and noreply identity were used.

## Diagnosis and TDD evidence

`cmake --workflow --preset be32-compile` and the archive/member `nm -P` and
`objdump -dr/-t/-h` commands ran on actual GNU 15.2.0-16ubuntu1/qemu 10.2.1
targets, not inferred native objects. The temporary step is visible in the
workflow. No compiler/runtime implementation source or emulator source was read.

RED: [run 39658546154](https://github.com/szTheory/muhtari/actions/runs/39658546154),
job 119425180941, source `f4ce07210bfd5d3f1cc553cb8a057c5406401714`.
The targeted CTest regex selects `abi.undefined`, `abi.globals`, both original
self-tests and `core.audio.silence`; exit 8, two failures and three passes.
The unchanged undefined assertion rejects `__divdi3`; the unchanged globals
assertion rejects four cartdb count objects. Disassembly locates signed helper
calls only in `muhtari__synth_round` and its inlined coefficient calls. DC
feedback's constant quotient/remainder operations emit no forbidden helper and
remain unchanged. The four const uint32 declarations occupy `.sdata`, whose
actual section lacks READONLY; GCC reports readonly-in-sdata enabled by default.
Both original fault controls reject abort/writable counter for their named faults.

`node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence
build/recovery-red/red-evidence.json --raw` — exit 0, RED_EVIDENCE_OK.
The unchanged actual JUnit report identifies both intended ABI assertion
failures; semantic inspection confirms they executed. The arithmetic cases
already pass the old implementation, so no arithmetic failure was invented.
`cmake --build --preset ci && ctest --preset ci -R '^core[.]audio[.]silence$'
--output-on-failure` — exit 0 before the source correction.

GREEN target: Release job 119426862308, exact source
`96dc316427cbcb686b06b650d4cc16d729c419d3`, targeted CTest exit 0, five passes.
The synth object has only permitted unsigned divide/remainder helpers. All four
count names are type R and reside in actual READONLY `.rodata`; generated names,
values and tables are untouched. The target-only init flag preserves existing
flags and PIC. Both ABI fault controls still pass, including the writable counter.
Nearest/ties-away results, Q30 feedback truncation, coefficients, revision and
all expected hashes are unchanged. Source/test formatting passes clang-format 21.

The implementation commit is a `fix` for a real target ABI defect rather than
a new feature. This execute plan uses existing recurring ABI tests as RED;
its phase-wide optional TDD mode is false. No REFACTOR commit was necessary.

## Complete inventories and failed hosted consumer

`gh run view RUN -R szTheory/muhtari --json status,conclusion,headSha,jobs,url`,
`gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/JOB/logs`
and `gh run download RUN -R szTheory/muhtari -n lane-equality -D build/<directory>`
— exit 0 for both completed runs below. Raw logs stay temporary, never committed.

| Run/job | Actual producer SHA | Result |
|---|---|---|
| [Release 39659029223](https://github.com/szTheory/muhtari/actions/runs/39659029223), 119426862308 | 96dc316427cbcb686b06b650d4cc16d729c419d3 | Native 607+6 and BE 607+6 pass; final comparator exits 1. Called CI required job 119431669581 succeeds, including all six native legs and RetroArch. |
| [Nightly 39659029340](https://github.com/szTheory/muhtari/actions/runs/39659029340), 119426862666 | aebf09ffb8e5cd6937202edc1831de2b37c40f1e | Native 607+6 and BE 607+6 pass; final comparator exits 1. Reporter is skipped on this PR. |

Nightly's API headSha is the PR head while its actual producer checkout is the
merge SHA above, verified from commit.txt. Each run's native and BE identities
match its own immutable checkout, pinned fixtures and semantic configuration.
Architecture/compiler/emulator/timeout scale intentionally differ.

Both normal and release `check_junit.cmake` selections pass in both lanes of
both runs. Replaying all four judges on the downloaded Release reports exits 0.
An independent Python Counter additionally checks exact registered membership,
no extra/duplicate/missing tests and no failure/error/skip: exit 0, 607+6 each.
This includes install.consumer and embed.subdirectory, and post-package archive,
firmware, floor, notice and path checks; none were dropped or skipped.

Both final hosted comparisons fail `lane_equality: members of NATIVE:` with an
empty member list. Read-only local replay with the same relative path shape
reproduces exit 1. The downloaded independent directories each contain exactly
commit.txt, hashes.txt and identity.txt. With absolute path arguments and the
exact expected SHA, the unchanged lane_equality.cmake exits 0 for each run:
`hashes: 2 identical files`. This narrows the failure to the relative-path
caller contract. It is diagnostic proof, not successful hosted acceptance.
No comparator/predicate/allowlist/inventory/permission edit was made.

Read-only workflow inspection confirms rehearsal needs ci and equality, publish
needs release-ci and release-equality, and actual release callers use the
release-please immutable SHA. Nightly reporting depends on equality; issues-write
and App credentials remain outside the read-only producer. Publication/default
branch truths remain ship checks; no production publication or merge ran.

## Timing and fixed allocations

RED uses ubuntu-26.04 image 20261018.168.1; corrected Release and Nightly jobs
use image 20261012.163.1. All observed target versions are GNU 15.2.0-16ubuntu1,
qemu 10.2.1 and CMake 4.4.4. These differ from the historical CMake 4.4.3 cohort.
Corrected producer jobs span 19:08:45–19:20:31 (706 s) and
19:08:45–19:21:05 (740 s), both failed. They are not completed-success job
samples. Full producer success n=0, maximum and twice-maximum unavailable;
retain the provisional 30-minute bound and five-minute network floor. Historical
cancelled 6m35s remains censored. No run was manufactured to fill a cohort.

B12885, S2, N1970 and C14853 are unchanged. The fixed allocation remains
count400/BE300/digest650/quality450/fixes100/reserve70. Current use is count368,
BE199, fixes1 and zero in other components: 13451 charged physical lines,
including the two toolchain lines and all 11 temporary diagnostic lines.
Source denominators are 8580/6533, advisory ratios 156.7716%/205.8932%; broad
sensitivity is 13510. No ratio compliance, scope increase, hidden exclusion,
source padding, permanent rebaseline or redistribution is claimed.

## Local verification and deferred acceptance

- `cmake --workflow --preset ci` — exit 0 after source changes, 607 normal and
  six release tests. Both JUnit judges pass, exit 0.
- `cmake -DCI_YML=.github/workflows/ci.yml -DJQ=jq -DREPORT_ONLY=wasm32,library
  -P tests/cmake/ci_required.cmake` — exit 0, 15 cases.
- `ctest --preset ci -R '^policy[.]lane_equality[.]controls$' --output-on-failure`
  — exit 0, valid pair and all seven retained faults.
- Final six-test ABI/audio/equality-control selection — exit 0.
- Physical counter, absolute-root docs budget, hygiene tree, clang-format 21
  dry-run and git diff --check — exit 0. Local commands have no hosted URL.
- `git diff --exit-code a2daeb6..HEAD -- src/cartdb.c src/cartdb.h tools/cartdb.cmake
  tests/cmake/abi.cmake tests/abi` — exit 0, unchanged declarations/generation/judges.

The plan requires stopping on a complete inventory/equality failure. No second
implementation attempt at that failure, no second gap round and no downstream
plan ran. docs/testing.md was not edited; final diagnostic-free hosted proof and
its coverage wording remain unfinished. WINDOWS entries 13/14 record the new
consumer failure and unrun final acceptance. Earlier 11/12 remain historical
open records rather than silently rewritten success.

## Known Stubs and unrun verification

The temporary diagnostic at be-equality.yml:38 remains intentionally visible at
the mandatory halt. It must be removed during an explicitly authorized recovery,
then the final immutable-head full callers re-proved. No product stub or skipped
test was introduced. No new security endpoint, auth path or trust-boundary schema
was introduced. All seven flagged assumptions and the unresolved transparency
prohibition remain as authored; no classifier lifecycle or requirement was cleared.

## Deviations from Plan

No additional implementation deviation was undertaken. The designed stop condition
was reached on new hosted caller evidence. The source/ABI work is retained; full
recovery and phase-wide TUNE completion are explicitly not claimed.

The required SDK state/progress commands counted the halted summary as completed
and checked its roadmap item. Tracking was immediately corrected to 2/9, unchecked
05-10 and blocked downstream status. The SDK implementation was not changed.

## Self-Check: PASSED (halt record)

All five changed implementation paths exist; f4ce072 and 96dc316 are ancestors
of HEAD and the persisted ledger measures two commits. This verifies the partial
record only. The hosted equality and final cleanup acceptance remain incomplete.
