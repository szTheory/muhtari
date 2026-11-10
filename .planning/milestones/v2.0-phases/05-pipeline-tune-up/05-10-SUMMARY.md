---
phase: 05-pipeline-tune-up
plan: "10"
status: complete
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
  - Canonical evidence-path contract with retained absolute/relative controls
  - Diagnostic-free full equality and release-called required CI at an immutable implementation SHA
affects: [05-04, 05-05, 05-06, 05-07, 05-08, 05-09]
tech-stack:
  added: []
  patterns: [unsigned bounded magnitude division, target-only constant placement, canonical evidence paths]
key-files:
  created:
    - .planning/phases/05-pipeline-tune-up/05-10-HALTED.md
  modified:
    - src/synth.c
    - tests/core/test_core.c
    - cmake/toolchains/ppc-linux-gnu.cmake
    - docs/hardware/audio.md
    - .github/workflows/be-equality.yml
    - docs/testing.md
    - tests/cmake/lane_equality.cmake
    - tests/policy/tests.cmake
key-decisions:
  - Preserve the original mandatory halt and failed hosted evidence; complete only the explicitly authorized scoped debug continuation.
  - Normalize evidence arguments once, retaining every identity, inventory, ABI and byte-comparison predicate.
  - Keep separate image/caller timing cohorts, the provisional 30-minute bound and all fixed allocations.
duration: 83min wall
completed: 2026-10-21
actuals:
  tokens: 2113
  tasks: 2
  commits: 5
plan_head_before: a2daeb660fd7ca74912fa1a8dd7c58617b863e16
plan_head_after: 0901c877977e9f712a77193d4840df5ff5b5408d
requirements-completed: []
requirements-contributed: [TUNE-02, TUNE-04, TUNE-05]
coverage:
  - id: target-abi
    description: PowerPC archive satisfies unchanged ABI judges and real audio regression checks
    requirement: TUNE-02
    verification:
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39663281026/job/119441583140
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
        ref: Both check_junit selections in Release 39663281026 and Nightly 39663280880, exit 0
        status: pass
      - kind: other
        ref: Independent exact Counter comparison of all eight downloaded final JUnit reports, exit 0
        status: pass
    human_judgment: false
  - id: hosted-equality
    description: Read-only release and nightly producers complete their exact pair comparator
    requirement: TUNE-05
    verification:
      - kind: integration
        ref: Release job 119441583140 and Nightly job 119441580417, comparator exit 0
        status: pass
    human_judgment: false
  - id: final-cleanup
    description: Diagnostic-free final implementation and matching coverage documentation are accepted
    requirement: TUNE-05
    verification:
      - kind: integration
        ref: Diagnostic-free implementation 0901c877, both full hosted producers, docs.budgets and local ci, exit 0
        status: pass
    human_judgment: false
  - id: relative-path-contract
    description: Existing valid and seven fault cases execute with absolute and relative directories
    requirement: TUNE-02
    verification:
      - kind: integration
        ref: policy.lane_equality.controls RED exit 8, GREEN exit 0, comparator-only revert/reapply RED/GREEN confirmed
        status: pass
    human_judgment: false
  - id: called-native-ci
    description: Release-called required CI and six-native-file equality complete on the implementation SHA
    requirement: TUNE-05
    verification:
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39663281026/job/119461511750
        status: pass
    human_judgment: false
---

# Phase 5 Plan 10: PowerPC recovery Summary

**PowerPC ABI, complete native/BE inventories, exact equality and release-called
required CI now pass at the immutable implementation head.**

The original attempt stopped on the required boundary. Its failure is preserved
in unchanged 05-10-HALTED.md and in the historical sections below. The owner then
authorized this narrow find-and-fix debug continuation. Both recovery tasks are
accepted; Phase 5 progress is 3/9, with 05-04 next. No downstream plan, second gap
round, ship, merge or owner UAT ran. Phase-wide requirements remain pending.

## Scoped caller diagnosis and correction

Authentic Release artifacts at `96dc316427cbcb686b06b650d4cc16d729c419d3`
reproduce `lane_equality: members of NATIVE:` with relative directories, exit 1;
the unchanged comparator with absolute directories exits 0. The comparator had
computed REAL_PATH values only for duplicate detection, then passed the original
relative values to GLOB. Reusing the existing canonical values as NATIVE and BE
repairs member enumeration, reads and comparison together without changing any
predicate or adding comparator lines.

The existing `policy.lane_equality.controls` now runs its valid case and all seven
named faults with absolute then relative directories. Before the fix, the eight
absolute cases pass and the relative positive fails, CTest exit 8. After the fix,
all sixteen pass, exit 0. Reverting only the three comparator substitutions
restores that exact failure; reapplying restores success. Mixed absolute/relative
artifact replay passes and mixed-spelling duplicate directories remain rejected.
Stryker is not configured for this CMake repository; that mutation signal is
explicitly skipped, while the comparator-only revert signal passes.

Implementation commit `0901c877977e9f712a77193d4840df5ff5b5408d` also removes
all eleven temporary ABI diagnostic lines and updates the existing portability
paragraph. No ABI judge, allowlist, generated data, inventory, expected hash,
permission, public API or timeout declaration changed. The retained root fix is
within the prior BE allocation. The diagnostic-free workflow itself produced
the successful evidence below.

## Final immutable hosted acceptance

| Run/job | Actual producer SHA | Accepted result |
|---|---|---|
| [Release equality](https://github.com/szTheory/muhtari/actions/runs/39663281026/job/119441583140) | 0901c877977e9f712a77193d4840df5ff5b5408d | Native 607+6, BE 607+6, four JUnit judges and exact two-file equality succeed. |
| [Direct Nightly equality](https://github.com/szTheory/muhtari/actions/runs/39663280880/job/119441580417) | 5ad2484b9d48c0ef860c4a1d9a23f18a9dd7b2a2 | Native 607+6, BE 607+6, four JUnit judges and exact two-file equality succeed. |
| [Release-called CI required](https://github.com/szTheory/muhtari/actions/runs/39663281026/job/119461511750) | 0901c877977e9f712a77193d4840df5ff5b5408d | All six native legs and six-file hash equality succeed; roll-up has HEAVY=true and CALLED=true, exit 0. |
| [Direct CI](https://github.com/szTheory/muhtari/actions/runs/39663280906) | PR merge for head 0901c877 | Completed success; supplementary evidence rather than a substitute for called CI. |

Nightly's API headSha is the PR head; its actual checkout is the separately
identified merge SHA. Each pair's downloaded commit.txt files both match its own
producer; identity.txt and nonempty hashes.txt match byte-for-byte. PR merge
metadata independently identifies `5ad2484b` for this head.

`gh run view RUN -R szTheory/muhtari --json status,conclusion,headSha,jobs,url`,
`gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/JOB/logs`
and `gh run download RUN -R szTheory/muhtari -n lane-equality -D build/<directory>`
— exit 0 for both final producers. Their eight normal/release `check_junit.cmake`
replays exit 0. An independent Python stdlib Counter/ElementTree check over the
downloaded JUnit files exits 0: exact registered membership and no extra,
missing, duplicate, failure, error or skipped execution. Both relative-path
`lane_equality.cmake` replays with the exact producer SHA exit 0,
`hashes: 2 identical files`. Artifacts and raw logs remain ignored build output.
The same eight historical inventory judges also pass; that does not relabel
historical failed hosted equality as success.

Authentic called Linux x64, macOS x64 and arm64 logs explicitly show the
implementation checkout and exact 607/6 JUnit results. Called CI required
completed at 20:17:26 UTC, six-native equality at 20:16:56. macOS jobs spent a
long external queue with no runner assigned; unchanged labels are documented
as supported and eventually received runners. No published GitHub incident or
precise capacity cause was established. Queue time is not test duration or a
successful producer timing sample. No rerun or relaxed gate was used.

Workflow inspection confirms rehearsal needs ci+equality, publication needs
release-ci+release-equality, and real release callers use the release-please SHA.
Nightly reporting needs equality; write credentials remain outside the producer.
Production publication/default-branch truths remain ship checks. This summary
claims the successful required jobs above, not an unobserved overall Release
publication result. Later planning-only commits are not claimed as producer SHAs.

## Final local verification and fixed bounds

- `cmake --workflow --preset ci` — exit 0 after the final implementation edit;
  both JUnit judges exit 0, 607 normal and six release executions exactly once.
- `ctest --preset ci -R '^policy[.]lane_equality[.]controls$' --output-on-failure`
  — exit 0; sixteen absolute/relative valid/fault cases, with recorded RED and
  comparator-only revert/reapply evidence above.
- `cmake -DCI_YML=.github/workflows/ci.yml -DJQ=jq -DREPORT_ONLY=wasm32,library
  -P tests/cmake/ci_required.cmake` — exit 0, fifteen retained cases.
- `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt
  -DOUT=build/ci/gate-count.json -P tests/cmake/gate_count.cmake` — exit 0,
  13,445 physical lines, source denominators 8,580/6,533.
- `scripts/hygiene.sh --tree`, `git diff --check` and `docs.budgets` — exit 0.
  docs/testing.md is 40,949/40,960 bytes. An initial local workflow correctly
  failed its oversized explanation; the paragraph was shortened and the fresh
  complete workflow passed. A stale interim run was interrupted, exit 130,
  before the final edit; it contributes no acceptance evidence.
- `git diff --exit-code 96dc316..0901c877 -- src/synth.c tests/core/test_core.c
  cmake/toolchains/ppc-linux-gnu.cmake tests/cmake/abi.cmake tests/abi src/cartdb.c
  src/cartdb.h tools/cartdb.cmake tests/inventory/ci.txt quality.json` — exit 0.
  This continuation preserves the already proved arithmetic/ABI correction.

Local checks have no hosted URL; their hosted counterpart is the final producer
and called-CI evidence above. Every behavior change is reached by existing ci
registration; no test name or expected output was added or dropped.

| Successful producer cohort | n | Image | Native normal/release seconds | BE normal/release seconds | Job maximum | Twice-max minutes |
|---|---:|---|---|---|---:|---:|
| Release-called | 1 provisional | ubuntu-26.04 20261018.168.1 | 52.89/0.62 | 594.96/0.18 | 689 s | 23 |
| Direct Nightly | 1 provisional | ubuntu-26.04 20261012.163.1 | 52.41/0.41 | 642.17/0.19 | 741 s | 25 |

GNU identifies 15.2.0; installed cross package is 15.2.0-16ubuntu1cross1 and
qemu-user is 1:10.2.1+ds-1ubuntu3.2 in each completed log. CMake 4.4.4 is listed
in each exact image's included-software manifest, fetched with `gh api`, exit 0.
Workload is complete native+BE configure/build/607 normal/package/six release
and unchanged equality. Formula is ceil(2 x maximum / 60) with the five-minute
floor. The image/caller cohorts remain separate and below ten samples; retain
the unchanged provisional 30-minute bound. Earlier failed 706/740-second jobs
and cancelled 6m35s remain excluded. No run was manufactured for a cohort.

B12885, S2, N1970 and C14853 remain fixed. Current use is count368/400,
BE193/300, fixes1/100, digest0/650, quality0/450 and reserve0/70.
The caller continuation adds five control lines and removes eleven temporary
lines, net minus six; recovery ends seven lines above prior BE186. Total is
13,445 with 1,408 headroom; broad sensitivity adds the unchanged 59-line
RetroArch product wrapper, 13,504. Advisory full/authored source ratios are
156.7016%/205.8013%. No ratio compliance, rebaseline, allowance transfer,
unallocated growth or weakened control is claimed.

The header's tokens field retains the prior realized-diff proxy, not a model
usage claim: 8,452 implementation/test/doc diff characters / 4 = 2,113. Five
commits lie between the recorded heads: three implementation/test commits and
two historical halt/tracking commits. Wall envelope is 83 minutes from the
prior plan head timestamp to called-CI acceptance, including external queues;
it is not an active coding-time or latency measurement.

## Historical attempt through the mandatory halt

The following original attempt record describes the state at `96dc316`, before
the explicitly authorized debug continuation. Its unresolved/current wording
is historical and is superseded by the acceptance above. Both HALTED files and
05-03's partial SUMMARY are unchanged. Failed runs remain failed.

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
