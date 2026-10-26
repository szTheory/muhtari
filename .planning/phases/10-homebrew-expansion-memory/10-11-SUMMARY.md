---
phase: 10-homebrew-expansion-memory
plan: "11"
subsystem: testing
tags: [bankset, xboard, xm, fuzz, properties, bounded-execution]
requires:
  - phase: 10-homebrew-expansion-memory
    provides: Accepted hardware, host, diagnostic, equality and lifetime contracts from plans 01-10
provides:
  - Regional hostile replay across all accepted BankSet geometry and expansion combinations
  - Malformed declaration and effective public-load override replay
  - Required seeded isolation, observer and hidden-battery lifecycle properties
affects: [10-12, 10-13, BOARD-04, BOARD-05]
tech-stack:
  added: []
  patterns:
    - Existing generated replay inputs and seeded shrinking harness
    - Independent physical offset assertions and real-consumer fault controls
key-files:
  created: []
  modified:
    - tests/fuzz/run.c
    - tests/fuzz/run_cart.h
    - tests/fuzz/run_seeds.c
    - tests/fuzz/tests.cmake
    - tests/fuzz/loader.c
    - tests/fuzz/seeds.c
    - tests/fuzz/seeds.h
    - tests/prop/test_prop.c
    - docs/hardware/bus.md
    - docs/testing.md
key-decisions:
  - Retain the existing four-line replay tail assertion and physical capacities; expand only the test input to the supported image cap.
  - Use a provisional three-second run replay allowance from the complete 1.21-second workload, retaining existing scaling and hosted calibration.
  - Keep required 100-case properties and advisory 10000-case policy distinct; no advisory failure is promoted.
requirements-completed: []
coverage:
  - id: hostile-regional-replay
    description: All fourteen BankSet ROM/RAM geometries with expansion off, XBoard and XM in both regions retain bounded deterministic calls.
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: fuzz.regress.run at 464b01c, 122 inputs including 84 new profiles
        status: pass
      - kind: other
        ref: One-off corrupted expansion-store consumer aborts at the independent physical-offset assertion
        status: pass
    human_judgment: false
  - id: effective-loader-refusal
    description: Malformed geometry, unsupported declarations, sound requests and effective expansion/HSC overrides preserve a live instance on refusal.
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: fuzz.regress.loader at 464b01c, 52 inputs and twelve public overrides per input
        status: pass
      - kind: other
        ref: One-off ADPCM-refusal removal is rejected by the real loader replay
        status: pass
    human_judgment: false
  - id: seeded-lifecycle-observers
    description: Independent backing, pure observers, restored hidden HSC bytes and load-stable battery identity and generation survive controls, Reset, power and refusals.
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: All six prop properties at 100 cases each and core.no_alloc_after_load at 464b01c
        status: pass
      - kind: other
        ref: Ignored XM HSC gate fails battery_lifecycle with seed 20261020 case 34 minimized size 1
        status: pass
    human_judgment: false
  - id: preserved-workflow-packages
    description: Existing ordinary, release, inventory and genuine-base package consumers remain green.
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: Unchanged task08-jobserver-gate.sh at 464b01c, 661 ordinary plus six release tests
        status: pass
      - kind: integration
        ref: Fresh quality_evidence BUILD_BASE at 464b01c against genuine pre-plan 50f48ce
        status: pass
    human_judgment: false
duration: 16min
completed: 2026-10-26
status: complete
actuals:
  tokens: 14477
  tasks: 2
  commits: 3
plan_head_before: 50f48ce1fa28d67421a5a06217e807ad4c99bb33
plan_head_after: 464b01c7f59c244071e17c13776e51544b87f147
---

# Phase 10 Plan 11: Hostile Expansion Replay and Properties Summary

**All accepted BankSet/expansion profiles now reach regional bounded replay and seeded physical-state, refusal and battery lifecycle checks through existing consumers.**

## Accomplishments

- The run corpus retains 38 prior inputs and adds fourteen accepted ROM/RAM geometries times three expansion profiles times two regions. Explicit test-input selectors choose off/XBoard/XM without inventing A78 declarations. Generated loops/JAM run through actual VBE/HALT scheduling. All 256 control bytes, exact/neighbor/absent registers, repeated stores, disabled overlays, page/window endpoints and both memory views reach the existing replay. Independent CPU physical-offset assertions reach expansion offset 131071. Separate instances compare status, ticks, actual regional native video and PCM.
- Test input storage and libFuzzer max_len both become the existing 1048704-byte image cap, with a source static assertion. PAL uses 320x292 video and 962-sample buffers; NTSC comparisons retain their actual height. The emulated four-line replay tail bound is unchanged. Counting allocation checks cover traffic, run calls and power; installed backing remains bounded by the real 1048576+32768+131072=1212416 bytes. No production allocation, resource quota, traffic quota, framework, inventory entry or ROM fixture was added.
- Twelve new loader seeds cover shared/HALT RAM and maximum storage, 52KiB/HSC conflicts, odd/truncated halves, invalid RAM, preserved unsupported v4, byte63 warnings, conflicting POKEY and YM/Covox/ADPCM including audio0x20. Seed builders assert specific malformed statuses and retained descriptive fields. Every input traverses all twelve expansion/HSC combinations through public load after a real warm frame. Refusals preserve the complete live instance, ROM and allocator counts; accepted loads have exact physical allocation sizes. Existing internal-load and audible failed-continuation replay remain intact.
- Frame properties select all84 new profiles before randomized selection. Digests normalize instance pointers and include the entire cartridge/expansion allocation. Bank/page/gate/physical-RAM traffic, refused reload and power precede pure public span/info/peek observations and independent versus interleaved execution. The six existing battery layouts remain; additional profiles restore raw bytes before execution while XM HSC is hidden, reject hidden writes, expose the imported bytes, distinguish changed and repeated visible stores, and preserve save topology, generation and backing through gates, console Reset, power and failed reload. The existing printed seed and shrinking mechanism remain.
- No core, API, ABI, timing, pinned hash, fixture or inventory expectations changed. API7/ABI1/behavior6 and prior required contracts remain in force.

## Task Commits

1. Task1 regional hostile run replay: `26c10dd`.
2. Task2 loader and seeded lifecycle properties: `f7ea578`.
3. Required documentation-budget correction: `464b01c`.

The persisted ledger measures exactly three commits through the source head above. Actual tokens are ceil(57905 realized diff characters /4), on the estimate's scale; the metadata commit is separate.

## Verification Evidence

All commands below ran locally; run URL is **N/A-local** for every row. No execution log is committed.

| Command | Exit | Observation |
|---|---:|---|
| Initial configure/build and `ctest --preset ci -R '^fuzz[.]regress[.]run$' --no-tests=error --output-on-failure` |0|Existing GREEN baseline,0.10s |
| `/usr/bin/time -p build/ci/tests/muhtari-fuzz-run-replay --seeds` |0|Complete122-input workload1.21s, including actual DMA assertions |
| Task1 final configure/build and same run selector |0|Complete replay passes1.49s; later full-lane1.36s stays within3s |
| `/usr/bin/time -p build/ci/tests/muhtari-fuzz-loader-replay --seeds` |0|Complete52-input loader replay0.62s |
| Configure/build and `ctest --preset ci -R '^(fuzz[.]regress[.]loader$|prop[.]|core[.]no_alloc_after_load$)' --no-tests=error --output-on-failure` |0|10 tests including fixtures; six required100-case properties, loader0.64s, isolation6.47s and noalloc5.94s |
| Configure/build and inherited `ctest --preset ci -R '^(core[.]pal[.]|abi[.]api_|core[.]testcard[.]|core[.]load[.]keeps_previous$|audio[.]frames|runner[.]load[.]pal$|libretro[.]host[.](xrgb8888|rgb565)$)' --no-tests=error --output-on-failure` |0|All24 inherited API/PAL/core/audio/real-host checks pass |
| `build/ci/task11-run-fault --seeds` |134 expected|One-off cart object corrupts expansion stores; real replay rejects the wrong physical byte |
| `build/ci/task11-loader-fault --seeds` |134 expected|One-off instance object omits ADPCM refusal; real public-load replay rejects it |
| `build/ci/task11-prop-fault battery_lifecycle build/ci/built` |1 expected|One-off cart object ignores XM HSC gate; hidden-store assertion fails, seed20261020 case34 shrinks to size1 |
| Initial `sh build/ci/task08-jobserver-gate.sh` |8|660/661 ordinary pass; only docs.budgets fails at43395/40960 bytes; release steps do not run |
| Initial `sh build/ci/task10-11-quality.sh` |1|Producer refuses the failed current JUnit before creating a comparison baseline |
| `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` |0|All22 documents checked; testing40784 bytes and bus24762 bytes |
| Final `sh build/ci/task08-jobserver-gate.sh` at464b01c |0|661 ordinary74.59s; six release0.24s plus repeated six0.14s; package generation, exact661-once inventory and hygiene pass |
| Final `sh build/ci/task10-11-quality.sh` at464b01c versus50f48ce |0|Fresh recursive producer; current661+6/base661+6; base ordinary60.17s/release0.09s; source, environment and fixture identities agree |
| clang-format21 dry-run on all changed C/headers; `git diff --check`; workflow hygiene |0|Formatting, whitespace, tree/history and noreply identity checks pass |
| `git worktree remove build/ci/quality-base` |0|Completed clean temporary baseline removed; only phase checkout remains |

The quality wrapper uses the unchanged four-slot FIFO jobserver and invokes:

`GITHUB_EVENT_PATH="$PWD/build/ci/task10-11-event.json" GITHUB_EVENT_NAME=push cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/task10-11-quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE=local -P tests/cmake/quality_evidence.cmake`

The event's genuine before is50f48ce1fa28d67421a5a06217e807ad4c99bb33; candidate is464b01c7f59c244071e17c13776e51544b87f147. Both envelopes retain AppleClang21.0.0.21000101/Darwin/arm64, trusted manifest5d355f481b2aaf34f67a003c938a2a14a3d5e554455568d3ce9437901142c527 and sample d549d4691aca7c004325102613fd6ac2337ecfe8a4c0cec004472e5bbcc80ca8. The baseline already contains plans01-10; this is no cached-base relabel or phase-versus-main comparison.

| Actual macOS arm64 ZIP | Candidate bytes | Genuine pre-plan bytes |
|---|---:|---:|
| Library |83245|83222|
| Runner |98522|98522|
| Libretro |89625|89625|
| Three-component sum |271392|271369|

Archive bytes remain report-only under DEC.61. No all-platform performance or hosted result is claimed.

## TDD and Failed History

Global TDD mode is false. Task1 extends a falsifier for already implemented behavior: the baseline and expanded correct implementation are honestly GREEN. No production defect or fabricated RED is claimed; fault-control variants above are intentional one-off falsifier checks.

- Initial new run attempts exit134 while correcting test-input assumptions: the historical truncated Activision seed had depended on the smaller cap; the52KiB/XM combination needs explicit HSC_OFF; PAL needs962 audio slots instead of801. The strengthened replay also prints a failed invariant's line. Only test builders/buffers/overrides changed; no runtime contract was weakened.
- Initial task2 builds exit1 for a new loop counter shadowing the image length and a const-qualified helper calling the existing writable-span API. A distinct step counter and a mutable observer helper restore the existing warning-clean conventions.
- The first ADPCM fault-control compilation exits1 because its one-off command omitted the generated-header include directory; attempting the absent executable then exits127. Adding the existing generated include directory builds the fault executable, which exits134 for the intended invariant. This setup error is not production RED evidence.
- The first git ledger write exits1 because normal sandbox access denies `.git` mutation. Authorized escalated task commits succeed with all hooks enabled and the configured noreply identity; no bypass is used.
- Initial full-workflow and producer failures are preserved above. A preliminary direct budget command with relative ROOT checked only two explicit files and was insufficient; the corrected absolute-root command checks all22 files. The final required workflow also exercises the unchanged budget consumer with its canonical root.

## Deviations from Plan

**[Rule 3 - Blocking issue] Provisional replay deadline follows the expanded complete workload.** Task1 uses the already-owned tests/fuzz/tests.cmake to align max_len with the supported image cap and change the old one/two-second replay allowances to3s, ceil(2*1.21s). Existing timeout scaling, membership, assertions and workload are retained. Later successful1.49/1.36s variation does not trigger another deadline loop. Final platform/sanitizer calibration remains plan13.

**[Rule 1 - Bug] Keep expanded evidence within the existing documentation budget.** Found by the full workflow after task2: docs/testing.md had only47 bytes of prior room. Commit464b01c consolidates its existing fuzz/property descriptions and links detailed coverage/fault evidence into the already-declared docs/hardware/bus.md. Both pages fit without changing any limit or dropping evidence. This necessary correction touches task1's declared bus documentation in addition to task2's declared testing documentation.

The previously reproduced optional WINDOWS count mismatch and summary command-as-path heuristic are inherited from plan10's receipt. They were not rerun or repaired merely to reconfirm advisory limitations. Actual files, commits and required consumers were checked directly.

## Limitations and Next Work

No new runtime stub, security boundary, dependency, owner verification or shared planning-state update was introduced. The selected XM model and larger/shared-RAM BankSet extensions retain their prior confidence limits. The10000-case nightly properties were not promoted or rerun here; the historical advisory isolation timeout remains unaccepted. Required100-case checks pass. Final sanitizer/nofp/native/PowerPC execution, hosted equality and cross-platform timeout calibration remain assigned to plan13. BOARD-04/05 receive safety evidence here and remain pending final acceptance.

Plan10-12 is ready within the existing execution step. The next separate allowed command is `$gsd-execute-phase 10`; no other GSD step was launched.

## Self-Check: PASSED

All ten task files exist and all three source commits are reachable. The persisted ledger measures three commits through464b01c. Both task selectors, inherited24-check regression, final661+6 workflow, fresh genuine-base producer, actual three-ZIP sums, unchanged production/fixture/inventory paths and clean tree before metadata were checked. The baseline worktree is removed. Shared STATE/ROADMAP/REQUIREMENTS remain owned by the orchestrator.
