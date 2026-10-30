---
phase: 10-homebrew-expansion-memory
plan: "10"
subsystem: testing
status: complete
tags:
  - c17
  - transactions
  - borrowed-media
  - allocation
requires:
  - phase: 10-homebrew-expansion-memory
    provides: BankSet and expansion hardware, public API 7 and judged diagnostic equality through plan 09
provides:
  - Complete failed-load state and independent continuation controls across supported storage topologies
  - Poison-retained borrowed-media replacement and exact allocation/free accounting
  - Maximum-capacity public clients and both-region 600-frame allocation-free expansion workloads
affects:
  - hostile-load properties
  - final native and PowerPC acceptance
tech-stack:
  added: []
  patterns:
    - Extend the existing counting allocator and registered core/public-client tests
key-files:
  created:
    - .planning/phases/10-homebrew-expansion-memory/10-10-SUMMARY.md
  modified:
    - tests/alloc.h
    - tests/core/test_core.c
    - tests/core/tests.cmake
    - tests/api/v7.c
    - docs/embedding.md
key-decisions:
  - Preserve the already-correct staged loader; no speculative instance.c repair or production refactor
  - Measure complete expanded workloads before calibrating existing target deadlines; preserve assertions, membership and timeout scaling
requirements-completed: []
coverage:
  - id: transaction-preservation
    description: Every supported BankSet storage topology preserves the complete live machine on candidate refusal or allocation failure
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: core.load.keeps_previous at ecb3fde, including 324 topology/region/battery cases and 15 candidate failures per case
        status: pass
    human_judgment: false
  - id: borrowed-media-lifetime
    description: Borrowed image, BIOS and HSC inputs survive free or clearing of every valid live backing kind
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: core.load.keeps_previous, 40 borrowed-source cases with poisoned retired allocations and independent boot/continuation controls
        status: pass
      - kind: unit
        ref: core.create.allocator, representative wrong-free-size and retained-poison controls
        status: pass
    human_judgment: false
  - id: physical-capacity-and-noalloc
    description: Exact maximum backing, stable public spans and field boundaries coexist with allocation-free dual-POKEY execution
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: core.no_alloc_after_load, six maximum-storage profile/region workloads of 600 frames with gates, power cycles and observers
        status: pass
      - kind: unit
        ref: abi.api_v7 and all frozen API clients, maximum 1212416-byte backing and complete selector boundaries
        status: pass
      - kind: integration
        ref: sh build/ci/task08-jobserver-gate.sh at ecb3fde, 661 ordinary plus six release tests, packages, inventory and hygiene
        status: pass
      - kind: other
        ref: Fresh quality_evidence BUILD_BASE at ecb3fde versus genuine pre-plan 6f055d2
        status: pass
    human_judgment: false
actuals:
  tokens: 10377
  tasks: 2
  commits: 3
plan_head_before: 6f055d2fb3217f86ae632a7fe6b58849bd953799
plan_head_after: ecb3fdea656a942db89a392db267c404b811c9fe
duration: approximately 24min
completed: 2026-10-26
---

# Phase 10 Plan 10: Transactional loads and exact physical accounting Summary

**Supported BankSet/expansion topologies preserve complete live state on failure, consume borrowed media before invalidation and execute at maximum backing without allocation after load.**

## Accomplishments

- The existing load-preservation test now covers all 14 legal BankSet geometry/RAM combinations, Off/XBoard/XM, both regions and every valid HSC/EEPROM topology: 324 cases. Each receives 15 candidate failures, including invalid selectors/struct minima, truncated images, forbidden geometry, unsupported/conflicting sound, bad firmware size/vector, invalid ports/serial placement, a partial expansion field and both complete/partial-selector allocation failures. The only fallible candidate allocation is observed; earlier validation makes no allocator calls.
- Snapshots compare the complete instance except its explicitly unpublished firmware staging, every byte of its physical cartridge/expansion allocation, effective information, all public memory spans, firmware bytes/identity, battery attachment/layout/bytes/generation and exact allocator accounting. An independent instance receives no failed loads. Subsequent CPU, native frame, raw event hash/count, PCM and filter/chip state agree, including after power cycling the original firmware.
- Forty successful and failed borrowed-load cases supply image/BIOS/HSC inputs from live cartridge ROM, cartridge RAM, expansion RAM, battery storage, HSC firmware, BIOS and system RAM wherever the required lengths fit. Retired allocations remain poisoned until a later free; embedded backing is cleared during successful installation/power. Copied bytes, exact old free lengths, independently loaded control output and executable boot markers prove successful consumption. RIOT RAM cannot fit the minimum image/firmware and is not passed as an invalid readable span.
- The counting allocator's optional aligned size header detects incorrect frees without a tracking quota. Its own representative fault control detects a deliberately wrong length and checks retained poison. Default allocator behavior remains compatible with the existing boot allocator. Successful strict lifecycles end with zero live bytes and zero size errors.
- The real maximum is 1,048,576 ROM + 32,768 HALT RAM + 131,072 expansion RAM = 1,212,416 bytes, with maximum expansion offset 131,071. Public API 7 tests validate this allocation and exact replacement/free sizes, failed-load span/battery preservation, all selector choices before/inside/at the complete field end, zero tails, frozen layouts and the old 40-byte/24-byte media minima. The existing checked-addition owner in cart.c is unchanged.
- The existing no-allocation workload retains its previous work and adds six maximum-storage profile/region runs, each 600 frames in the ordinary CI configuration. Both installed $0450/$0440 POKEYs have audible raw-source observations. RAM/page/HSC/POKEY gates, console Reset, power cycles, input submission, public spans/battery/firmware/info/counters and repeated observer setup make zero allocation or free calls after load. Every span remains stable and final replacement/destroy frees exact lengths. Short-run configurations retain their existing scale.
- Lifetime and measured physical-resource prose landed with the tests. Embedding documentation remains 40,865 bytes, below the unchanged 40 KiB cap. Production source, public headers, API 7/ABI 1/behavior 6, previous hash expectations, admitted fixture identities and test inventory membership are unchanged.

## Task Commits

1. Task 1 — `74cc908`: complete transaction snapshots, independent continuation, borrowed-media/poison controls and lifetime documentation.
2. Task 2 — `967ed14`: maximum backing, complete public selector boundaries, long-run hardware/observer workload and resource documentation.
3. Measured task 2 follow-up — `ecb3fde`: final existing-target deadline calibration from complete full-lane measurements.

All commits used the phase branch, supplied-root/protected-branch assertions, GitHub noreply identity and enabled hooks. No tracked file was deleted. The ledger measures three task commits from the genuine pre-plan base. Actual tokens are ceiling(41,508 realized diff characters / 4), not harness tokens. Shared STATE, ROADMAP and REQUIREMENTS remain orchestrator-owned and untouched.

## Verification Evidence

All run URLs in this section are **N/A-local**. No committed logs or hosted acceptance are claimed.

| Command | Exit | Evidence |
|---|---:|---|
| Task 1 configure/build and `ctest --preset ci -R '^(core[.]load[.]|core[.]hsc[.]|core[.]battery[.]|core[.]get_memory[.]|core[.]no_alloc_after_load$|abi[.]api_)' --no-tests=error --output-on-failure` |0|26 checks; the final stronger selector also includes `core.create.allocator` and passes27 checks, load5.56s |
| Task 2 configure/build and `ctest --preset ci -R '^(core[.]no_alloc_after_load$|core[.]get_memory[.]|core[.]load[.]|abi[.]api_|docs[.]budgets$)' --no-tests=error --output-on-failure` |0|19 checks; final load6.03s/noalloc5.16s, all frozen clients and docs budget pass |
| `/usr/bin/time -p build/ci/tests/muhtari-test-core load.keeps_previous` |0|Complete corrected workload5.80s direct; stronger focused run6.68s |
| `/usr/bin/time -p build/ci/tests/muhtari-test-core no_alloc_after_load` |0|Complete corrected workload4.71s direct; focused run5.19s |
| `sh build/ci/task08-jobserver-gate.sh`, initial complete workload |0|661 ordinary plus six release checks, three ZIPs, repeated six release checks, exact inventory and hygiene; ordinary69.06s, expanded targets6.79/6.08s |
| `sh build/ci/task08-jobserver-gate.sh`, task2 source967ed14 |0|661+6 and packages/inventory/hygiene; expanded targets7.66/6.85s establish final provisional calibration |
| `cmake --preset ci && ctest --test-dir build/ci -R '^(core[.]load[.]keeps_previous$|core[.]no_alloc_after_load$)' --no-tests=error --output-on-failure` |0|Final16s/14s registrations pass complete cases5.58/4.77s, with existing timeout scaling retained |
| `sh build/ci/task08-jobserver-gate.sh`, final sourceecb3fde |0|661 ordinary65.89s, release6 0.15s and repeated6 0.15s; three ZIPs, exact661-once inventory and hygiene; expanded targets7.48/6.56s |
| `sh build/ci/task10-10-quality.sh`, sourceecb3fde against genuine6f055d2 |0|Fresh existing recursive BUILD_BASE producer, current661+6/base661+6; base ordinary72.59s/release0.12s; environment, sources, fixture identities and actual archive sums agree |
| `git diff --check`; clang-format21 dry-run on changed C/header tests; `scripts/hygiene.sh --tree` |0|Clean diff, formatting, current tree and committed history checks |
| `git worktree remove build/ci/quality-base` |0|Clean temporary baseline worktree removed; only the main phase checkout remains |

The quality wrapper reuses the unchanged four-slot FIFO jobserver and invokes:

`GITHUB_EVENT_PATH="$PWD/build/ci/task10-10-event.json" GITHUB_EVENT_NAME=push cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/task10-10-quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE=local -P tests/cmake/quality_evidence.cmake`

Its genuine before is6f055d2fb3217f86ae632a7fe6b58849bd953799; candidate isecb3fdea656a942db89a392db267c404b811c9fe. Both envelopes retain raw trusted manifest5d355f481b2aaf34f67a003c938a2a14a3d5e554455568d3ce9437901142c527 and raw sample d549d4691aca7c004325102613fd6ac2337ecfe8a4c0cec004472e5bbcc80ca8, with AppleClang21.0.0.21000101/Darwin/arm64. This base already contains the prior expansion hardware and diagnostics; no cached-base relabel or phase-versus-main claim is made.

| Actual macOS arm64 ZIP | Candidate bytes | Genuine pre-plan bytes |
|---|---:|---:|
| Library |83241|83215|
| Runner |98522|98522|
| Libretro |89625|89625|
| Three-component sum |271388|271362|

Archive bytes are report-only under DEC.61. This local producer supplies no new all-platform performance claim.

## TDD and Failed History

Global TDD mode remains false. These tasks extend falsifiers for behavior already implemented; no production defect was exposed and no RED result was fabricated by breaking the loader. Initial nonzero results below are test-stimulus or expectation corrections, not valid production RED evidence.

- Initial `build/ci/tests/muhtari-test-core load.keeps_previous` exited1: the new test expected INVALID_IMAGE instead of TRUNCATED_IMAGE, expected UNSUPPORTED_DEVICE for an invalid numeric port instead of ARGUMENT, selected supported v4 audio code1 instead of conflicting code5, and omitted generated headerless-image footers. Those expectations/stimuli were corrected against the existing public contract.
- Subsequent timed/diagnostic load runs exited1 because generated BIOS firmware left INPTCTRL unlocked: its first TIA audio write legitimately switched BIOS out. Disabling DMA alone did not correct that. Locking the BIOS-visible 7800 map provides the intended executable, non-silent control. No source timing, raw output or hash expectation changed.
- A build after expanding the failure loop exited1 for an unused local counter; removing only that unused test variable restored the build. Adding the system-RAM borrowed source then made the stronger focused CTest run exit8 because it copied the NOP-only first4KiB of a16KiB fixture. Selecting its executable final4KiB restored all40 borrowed cases. The final27-check task1 run exits0.
- Initial timed noalloc execution exited1: the new stimulus sent XM page writes to XBoard's control aliases and thereby disabled its primary POKEY. Profile-specific register writes restored the intended audible dual-chip control. The complete workload and all audible-source assertions remain.
- The first normal-sandbox git staging attempt exited128 because `.git` writes were denied; no commit was created. The authorized escalated retry committed with all hooks enabled. No hygiene bypass or unsafe git recovery was used.

## Deviations from Plan

**[Rule 3 - Blocking issue] Recalibrated existing target deadlines after complete workload measurement.** The old one-second load allowance did not describe the expanded matrix. The final existing registrations use16s load and14s noalloc, twice complete full-lane7.66/6.85s rounded upward. Task1, task2 and the final calibration commits touch only the owning registration file in addition to the declared files. The orchestrator authorized that narrow ownership adjustment. No target, assertion, workload, timeout scale, framework or acceptance quota was added or removed. These local reliability allowances remain provisional pending the final cross-platform evidence in plan13; small successful variation does not trigger another calibration loop.

The best-effort `windows append --kind deviation --phase 10 --file tests/core/tests.cmake` exited1 with the already-known ledger count mismatch: frontmatter17/0/35/52 versus entries16/0/36/52. It made no mutation. This optional scanner limitation remains advisory; it does not waive a required product check.

## Limitations and Next Work

No new runtime stub, security boundary, dependency, external service or owner verification was introduced. The selected XM physical model and larger/shared-RAM BankSet extensions retain their prior confidence limits. Final sanitizer/nofp/native/PowerPC and hosted acceptance remain assigned to plan13; BOARD-04/05 are contributed to here, not marked complete. No shared planning state was advanced by this executor.

Plan10-11 is ready within the existing execution step. The next separate allowed command is `$gsd-execute-phase 10`; no other GSD step was launched.

## Self-Check: PASSED

The summary and all five modified task files exist. All three task commits are reachable; the persisted ledger measures exactly three commits through ecb3fde. Both task selectors, final full workflow, fresh genuine-base producer, actual ZIP sums, unchanged production/fixture/inventory paths and clean working tree before metadata are verified. The temporary baseline checkout is removed. Shared state files remain untouched.
