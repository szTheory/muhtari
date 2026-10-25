---
phase: 09-pal-games-and-audio
plan: "08"
subsystem: documentation-and-acceptance
tags: [pal, portability, diagnostics, packages, ci]
requires:
  - phase: 09-07
    provides: Independent PAL numbers, sustained host execution and additive equality observations
provides:
  - Evidence-backed PAL capability records and qualified public contracts
  - Complete six-native and native/PowerPC acceptance against an independently built main base
  - Authenticated package measurements and retained frontend/performance contracts
affects: [10, 11, verification, shipping]
tech-stack:
  added: []
  patterns:
    - Existing producer, join, inventory, equality and frontend consumers
    - Temporary standard FIFO jobserver for a heavily contended local host
key-files:
  created:
    - .planning/phases/09-pal-games-and-audio/09-08-SUMMARY.md
  modified:
    - README.md
    - docs/compatibility.md
    - docs/embedding.md
    - docs/emulation-tests.md
    - docs/pal-tests.md
    - docs/quality.md
    - docs/testing.md
    - libretro/muhtari_libretro.info.in
    - tests/audio/tests.cmake
    - tests/boot/tests.cmake
    - tests/compat/ledger.csv
    - tests/diag/driver.c
    - tests/diag/tests.cmake
    - tests/input/tests.cmake
    - tests/policy/tests.cmake
    - tests/runner/tests.cmake
    - tools/compat_doc.cmake
key-decisions:
  - Record the executed PAL mailbox separately from the NTSC title/BIOS evaluator and retail playability.
  - Authenticate the actual tested PR merge SHA and prove its entire Git tree equals the branch head.
  - Calibrate named failing consumers from complete uncensored workloads while preserving all assertions and lane authority.
  - Preserve authenticated base archive totals without inventing component splits absent from existing uploads.
requirements-completed:
  - PAL-01
  - PAL-02
  - PAL-03
coverage:
  - id: pal-timing-and-confidence
    description: PAL selection, startup, raster timing and model limits have executed evidence and honest released claims.
    requirement: PAL-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; core.pal.* and diag.pal; CI https://github.com/szTheory/muhtari/actions/runs/40229267518"
        status: pass
      - kind: other
        ref: "Agent review of load/frame/boot/ports source against embedding, hardware and confidence documentation"
        status: pass
    human_judgment: false
  - id: pal-host-capability
    description: Library, actual runner and both loaded libretro pixel formats deliver the selected PAL geometry and audio.
    requirement: PAL-02
    verification:
      - kind: integration
        ref: "diag.pal; actual 601-call runner and both loaded pixel formats; complete native inventories in CI 40229267518"
        status: pass
      - kind: other
        ref: "docs.budgets, policy.compat_doc_fresh, libretro.info_claims and agent claim/source review"
        status: pass
    human_judgment: false
  - id: pal-sustained-portability
    description: Startup plus 600 full PAL frames and independently proved synthesis bounds retain complete native and PowerPC equality.
    requirement: PAL-03
    verification:
      - kind: integration
        ref: "tests/cmake/hashes.cmake COUNT=6 and tests/cmake/lane_equality.cmake over downloaded actual artifacts"
        status: pass
      - kind: integration
        ref: "Nightly https://github.com/szTheory/muhtari/actions/runs/40229267583; native/PPC 653+6 inventories; WASM 591 and Linux hash equality in CI 40229267518"
        status: pass
    human_judgment: false
  - id: authenticated-release-evidence
    description: Six genuine current/base package measurements retain archive integrity, performance and the two existing real frontend cases.
    requirement: PAL-03
    verification:
      - kind: integration
        ref: "tests/cmake/quality_join.cmake with actual nine-artifact set, tested merge SHA, independent main SHA and PERF_REQUIRED=ON"
        status: pass
      - kind: e2e
        ref: "CI 40229267518 retroarch job; DMA pixels and fresh-process battery media/pixels; agent viewed all three frontend PNGs"
        status: pass
    human_judgment: false
actuals:
  tokens: 9449
  tasks: 3
  commits: 5
plan_head_before: dff74334b493c0d77f2c49442301a73263ce079a
plan_head_after: 9d5f3a8a78ac872a170ad8bd7581becc3c0a98fe
duration: 65min
completed: 2026-10-25
status: complete
---

# Phase 9 Plan 8: PAL claims and execution acceptance Summary

**Qualified PAL capability records now accompany complete native/PPC execution, unchanged NTSC observations and authenticated native packages.**

## Performance and scope

- Started 2026-10-25T19:34:44Z; acceptance completed 2026-10-25, approximately 65 minutes.
- Three tasks complete; 17 task files changed; five source/task commits measured from the persisted plan ledger.
- Actual tokens are ceil(37793 characters in the realized binary-capable diff / 4), not a harness token count. Metadata is committed separately.
- The orchestrator owns STATE/ROADMAP position, requirement checkboxes and phase acceptance counts. This executor does not advance them independently.

## Accomplishments

The ledger appends only `diag/pal`, leaving all 42 old rows byte-identical. Its four image digests were measured from the actual admitted image; A78 SHA-256 is `676381441af4dd33568f6d4adf7b84fc602f42606b87dc888f29c582af74b216`. The manifest still pins MIT source `37c428687dd499caf5de14826a554136c46b6372`. Evidence commit `dff74334b493` actually executed startup plus 600 full frames: 601 calls, 577879 samples and mailbox CRC `52a5`. The local-tier `mailbox@601` record is distinct from the NTSC title/BIOS library evaluator. Recurring `diag.pal` and equality tests execute it; no PAL retail-title or BIOS-screen predicate is invented.

The existing generator regenerates compatibility documentation. README and experimental libretro information now describe reachable PAL execution without promoting diagnostic success to universal retail playability. Embedding/testing/quality/emulation guides explain post-load requery, 93440-byte packed storage, padded extent, validation ordering, rational FPS/PAR, NULL output, host failure differences and reload-only region selection. API 6, ABI 1, behavior 5 and reserved status 12 remain unchanged.

Confidence remains explicit: handoff 140016, optical D64/W30/L9/T8 and the paddle fit are selected policies; PAL 85:342 analog balance is unverified. Preliminary revision-2 25:38 production applicability remains unresolved. The warm palette is an approved licensed presentation seed. Five milliseconds remains the EEPROM compatibility duration. Arithmetic-only carry evidence does not replace actual 600-full-frame execution.

## Task commits

1. Task 1 — `c517824`: executed PAL ledger, generated claims and generator consistency.
2. Task 2 — `94a8468`: public contracts, model confidence and architecture rationale.
3. Task 3 — `5a7dc8a`: GCC shadow warning and PAL-block formatting correction.
4. Task 3 — `f0f7fb8`: complete-workload calibration of the seven named failing consumers.
5. Task 3 — `9d5f3a8`: incorporate the late completed Windows ARM diagnostic sample.

No production core behavior, fixture bytes, NTSC predicate, public version, dependency or lane authority changes in this plan.

## Exact source and base identity

Final branch/PR head is `9d5f3a8a78ac872a170ad8bd7581becc3c0a98fe`. Actual PR checkouts, packages, producers and equality artifacts identify synthetic merge `69d9c0a31859eb471f2535703348ee01e0ea54fc`. The merge has parents main `2cbbd6a78dabe2bc098615d49f7118231cc05626` and that branch head. Both complete Git trees are exactly `dbf0dac05d671efaf9f1071522f0b7cfd8e372cf`.

`git fetch --no-tags origin 69d9c0a31859eb471f2535703348ee01e0ea54fc`, tree-ID assertions, `git merge-base --is-ancestor <main> <head>` and `git diff --exit-code <head> <tested-merge> -- .` all exit 0; run URL N/A-local. Producer identities are retained as the actual merge SHA, never rewritten to the branch SHA.

All native current/base observations use their actual shared compiler/platform environment and the same authenticated raw main manifest/sample identities:

- Manifest: `62dc23cf0a687032b3c8d82f988c79d926a1814cd7eaf0f993cbb720a002141e`.
- Built samples: `406ca181ecad9c64d4030b09203f210b69d5f5c649453f0a5149fc864444cb4b`.

`git show <main>:<file> | shasum -a 256` confirms these raw identities. The current manifest adds only the exact PAL row. Built-sample and pin files are byte-identical to main; no annotation transition is needed or claimed.

## Final verification

Accepted hosted executions are attempt 1, completed/success with branch head 9d5f3a8 and the authenticated tested merge above:

- [CI 40229267518](https://github.com/szTheory/muhtari/actions/runs/40229267518): `CI required`, every required job, all six native current/base builds, equality, sanitizer, two nofp lanes, architecture/ABI/integer/hygiene checks, performance, authenticated quality and real frontend cases pass. Library remains advisory and passes. WASM remains advisory and passes all 591 tests and actual Linux-x64 hash equality.
- [Nightly 40229267583](https://github.com/szTheory/muhtari/actions/runs/40229267583): full native/PPC execution and equality, 258 full-vector inventory entries, ten bounded-fuzz entries and existing benches pass. The advisory 10000-case property job has one `instances_isolated` timeout at 390.03s; seven of eight entries pass. That failure remains unaccepted evidence and advisory. Its authority, case budget and timeout were not changed.

Commands and exit statuses:

| Check | Command / result | Exit | Run URL |
|---|---|---:|---|
| Task 1 | Configure/build, then `ctest --preset ci -R '^(docs[.]\|compat[.]\|libretro[.]info_claims\|policy[.]compat_doc_fresh)' --no-tests=error --output-on-failure`; 7/7 | 0 | N/A-local |
| Task 2 | Configure/build, then `ctest --preset ci -R '^(docs[.]\|libretro[.]info_claims\|abi[.]api\|runner[.])' --no-tests=error --output-on-failure`; 93/93 | 0 | N/A-local |
| Exact local source | Required full workflow/package/release/JUnit/tree sequence below; 653 ordinary in 53.57s, six release in 0.11s; explicit package and six release again in 0.11s | 0 | N/A-local |
| Complete hosted run | Save `gh run view 40229267518 --json attempt,status,conclusion,headSha,url,jobs`, then `cmake -DINPUT=<saved-response> -DEXPECTED_SHA=<branch-head> -P tools/check_ci_run.cmake` | 0 | CI link above |
| Six-native equality | `cmake '-DFILES=<six downloaded hashes files>' -DCOUNT=6 -DMODE=compare -P tests/cmake/hashes.cmake` | 0 | CI link above |
| Native/PPC equality | `cmake -DNATIVE=<downloaded-native> -DBE=<downloaded-be> -DEXPECTED_SHA=<tested-merge> -P tests/cmake/lane_equality.cmake` | 0 | Nightly link above |
| Downloaded full inventories | Four existing `check_junit.cmake` invocations over native/PPC normal/release artifacts; each 653+6 exactly once | 0 | Nightly link above |
| Authenticated join | Existing `quality_join.cmake` over all nine actual quality artifacts, trusted raw main policy, tested merge/main identities, exact CI source URL and `PERF_REQUIRED=ON` | 0 | CI link above |
| Actual archives / old observations | Standard-library ZIP CRC/notices and source/measurement checks over 18 actual candidate ZIPs; six 956-row files equal; first 342 rows retain independently pinned SHA-256 | 0 | CI link above |
| WASM | Actual uploaded WASM hash file compared with actual Linux-x64 file using `cmp` | 0 | CI link above |
| Tree / scope | `git diff --check`, `scripts/hygiene.sh --tree`, normal commit/push hooks; only intended task files staged | 0 | N/A-local |

Focused checks use separate JUnit names; final ordinary and release JUnit remain complete. Current/base native inventories are 653/649 ordinary plus six release each. ASan is 636/632; hygiene 24/24; ABI 17, fuzz replay 6, title 3; warning, ABI, analyzer and sanitizer findings are zero. Retained floors and all independent consumers remain authoritative.

The two actual frontend cases are unchanged: DMA's 320x242 pixels match the retained runner pin; battery launches two fresh frontend processes and independently judges every one of 34816 bytes and all pixels. Counters advance 1/1 to 2/2 and marker 5A to FF. Agent viewing of downloaded DMA and both battery PNGs agrees with the numeric/pixel judges. Launcher isolation, wrong-pixel and media/oracle fault controls pass. No PAL frontend case or extra support-file budget was added.

The native/PPC artifact contains the same tested merge, fixture/config identities and 956 equal rows. PPC's complete PAL diagnostic takes 261.62s and the expanded hash writer 167.88s, inside the unchanged scale 13. Every first-342-row NTSC block has SHA-256 `a036c0f9da8037b0581496182024f0b42de455e08576712e0cc890dd949462b5`; all 614 PAL observations follow independent numeric acceptance.

## Genuine package measurements

Each candidate has exactly library, libretro and runner ZIPs. Individual candidate sizes, sums, CRCs/notices and authenticated independently rebuilt main sums are actual observations from CI 40229267518:

| Native lane | Library | Libretro | Runner | Candidate sum | Main base sum |
|---|---:|---:|---:|---:|---:|
| Linux arm64 | 99297 | 109276 | 120379 | 328952 | 323690 |
| Linux x64 | 97114 | 106078 | 118300 | 321492 | 314951 |
| macOS arm64 | 81541 | 86874 | 96478 | 264893 | 261503 |
| macOS x64 | 83208 | 87692 | 96863 | 267763 | 262217 |
| Windows arm64 | 96714 | 138518 | 162342 | 397574 | 391862 |
| Windows x64 | 97136 | 148564 | 175072 | 420772 | 414113 |

Compiler/platform identities are GNU 15.2.0 Linux aarch64/x86_64, AppleClang 17.0.0.17000013 Darwin arm64/x86_64 and MSVC 19.51.36260.0 Windows ARM64/AMD64. Each pair matches exactly; source, base, run and trusted raw identities are checked by the existing producer/join. Base component splits are not in the existing upload; no split is invented. The producer actually measures exactly three base ZIPs before emitting their authenticated aggregate. Archive peaks 420772/414113 bytes are report-only, with no replacement cutoff. Members, notices, prohibited assets, platform floors and release integrity still block.

The existing armed test-card comparator passes: candidate 3838448 Ir/frame versus independently built main 3838688; main repeat is also 3838688. Its raw observation retains advisory availability while the existing PR performance job and +1% comparator retain their authority. This is test-card evidence, not a PAL/controller workload speed claim.

## Recovery, calibration and failed history

All failures remain recorded; none is counted as passing evidence. No committed log is added.

| Source / attempt | Execution and outcome |
|---|---|
| Initial local docs | Three contract runs exit 8 on the existing 40KiB document limits. Condensed duplicated prose; final guides are within budget and 93/93 pass. |
| Initial local full workflows | Exit 8: first PAL18.01 timeout; next PAL/input/hash timeouts; an environment-only concurrency attempt still uses automatic preset jobs and also times out dependent work. No numeric mismatch. A sandbox process-inspection denial was observed, but escalated execution alone did not resolve contention. |
| 94a8468 dispatch / 1 | [CI 40227081044](https://github.com/szTheory/muhtari/actions/runs/40227081044) fails GCC shadow/formatting and platform deadlines; missing quality artifacts correctly fail the join. [Nightly 40227082783](https://github.com/szTheory/muhtari/actions/runs/40227082783) fails at GCC build. Dispatch base c517824 is a slice comparison, never phase-versus-main acceptance. |
| 94a8468 PR / 1 | [CI 40227151344](https://github.com/szTheory/muhtari/actions/runs/40227151344) retains observed GCC/formatting/timeouts and is subsequently cancelled by source update; [Nightly 40227151299](https://github.com/szTheory/muhtari/actions/runs/40227151299) fails GCC. No property execution pass is inferred from its failed build. |
| 5a7dc8a PR / 1 | [CI 40227789474](https://github.com/szTheory/muhtari/actions/runs/40227789474) fixes GCC/formatting but fails named complete-workload deadlines. Windows ARM has only hash20.04 timeout, with PAL17.69 completed. [Nightly 40227789467](https://github.com/szTheory/muhtari/actions/runs/40227789467) fails native PAL18.01 before PPC; advisory isolation also times out390.01. |
| f0f7fb8 / 1 | [CI 40229022177](https://github.com/szTheory/muhtari/actions/runs/40229022177) and [Nightly 40229022156](https://github.com/szTheory/muhtari/actions/runs/40229022156) intentionally cancelled after saving full responses because the late successful ARM sample changes ceil(2x17.69) from35 to36. No passing claim; the CI roll-up rejects cancellation as designed. |
| 9d5f3a8 / 1 | Final CI and Nightly above complete successfully; required evidence is accepted. Advisory property failure stays visible. |

Read-only bounded evidence/operations specialists audited every failure before batching fixes. Intel jobs already peak at four concurrent tests; the local oversubscription explanation was not applied to hosted Intel. Complete, uncensored comparable work at the existing method selected these provisional deadlines:

| Consumer | Complete measurement used | Selected base timeout |
|---|---|---:|
| PAL diagnostic | 17.69s Windows ARM, run40227789474/job121272909093; exceeds normalized ASan34.29/2 from run40227081044/job121270648702 | 36 |
| Expanded hash writer | 28.91s ASan/scale2, run40227151344/job121270952001 | 29 |
| Full two-region spectral capture | 105.20s Intel, run40227081044/job121270648784 | 211 |
| BIOS handoff_simple | 0.95s same Intel job | 2 |
| Input directions | 0.98s Intel, run40227151344/job121270952138 | 2 |
| Complete optical workload | 2.70s Intel, run40227081044/job121270648784 | 6 |
| Apple current/base quality controls | 45.49s Intel, run40227151344/job121270952138 | 91 |

Each is ceil(twice the complete measurement after existing lane normalization). Timeouts are censored, not duration samples. Assertions, counts, names, fixtures and all case workloads remain. Existing scales 2/13, Windows/Linux quality scopes and all other deadlines remain. Registry checks at scales1/2/13 confirm659 names, multiplied deadlines and unchanged representative neighbors. Later final passing measurements are validation observations; no endless margin-retuning cycle is introduced.

The local machine reported18 CPUs and load74.02/51.47/41.41 from unrelated workloads, which were left untouched. Explicit preset `jobs: ""` overrides `CTEST_PARALLEL_LEVEL=4`. A tiny direct GNU make pipe-jobserver probe caps CTest, but its descriptors do not survive CMake workflow spawning. The actual workflow probe confirms a temporary FIFO with three explicit tokens plus the implicit slot gives peak4. A four-token control measures5 and fails the expected4 assertion. No preset, hosted concurrency, quota, dependency or check framework changes.

The ignored reusable invocation `sh build/ci/task08-jobserver-gate.sh` runs the exact required sequence:

```sh
cmake --workflow --preset ci
cmake --build --preset ci --target package
ctest --preset ci-release --output-on-failure
cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake
scripts/hygiene.sh --tree
```

It creates a temporary FIFO, opens it read/write, writes three token bytes and exports `MAKEFLAGS=--jobserver-auth=fifo:<temporary-path>`. It removes only its own temporary FIFO on exit. The exact final source prints `Connected to MAKE jobserver`; parsed Start/completion rows show peak4 and zero residual tests. The extra release JUnit check also exits0. No harness or log is committed.

## Deviations from plan

1. **Rule 3 — generator consistency:** `tools/compat_doc.cmake` hardcoded unsupported PAL. Corrected it with the generated page; otherwise freshness would restore the false claim. Existing fresh/stale controls pass. Commit c517824.
2. **Rule 1 — required portability:** GCC rejects the new diagnostic callback `run` shadowing the global function, and the PAL block fails clang-format21. Renamed only the callback and formatted that block. Existing GCC/hygiene and actual diagnostic execution supply regression coverage. Commit5a7dc8a.
3. **Rule 3 — complete workload acceptance:** expanded the planned file scope to existing test registrations and the PAL evidence guide for measured, named deadline calibration. No test is dropped, shortened or made advisory. Commits f0f7fb8/9d5f3a8.
4. **Evidence precision:** the plan asks for base component splits that the existing uploads do not retain. Preserved D-19/ENGINEERING11's actual producer/join contract: all six genuine three-ZIP aggregates and full provenance, plus all candidate components. No fabricated split, extra size-only hosted probe, schema expansion or substitute quota.

Architecture review uses current owners: transactional selection stays in the instance; bus/MARIA/synth/ports retain timing, rendering, arithmetic and device responsibility; hosts query the committed result. Streaming proof and independent literal/mailbox/host evidence constrain different faults without a new dispatch layer, dependency or frame allocation. Named consumer calibration addresses observed work rather than introducing an operational score. The two review-fix/one-gap limits remain; parent phase review has not been consumed by these task acceptance fixes.

## Confidence and readiness

Descriptor-less legal/transparency intent and the original unclassified PAL assumptions retain their flagged-unverified history. Agent review checks claims against implemented source and independent evidence; it does not manufacture production hardware calibration or mark serialized classifications resolved. No new security surface or unfinished product stub is introduced. The existing CSV escape comment's word placeholder is not a product stub.

No authentication gate, owner setup, routine owner UAT, merge, ship or post-merge result is pending in this plan. Independent phase review/verification and orchestrator closeout remain separate responsibilities. Draft PR33 remains unmerged. Next separate user command after execute closeout: `$gsd-verify-work 9`.

## Self-Check: PASSED

All 17 task files and this canonical summary exist. All five task commits are ancestors of HEAD. The persisted ledger measures five commits from dff7433 through9d5f3a8. Actual artifacts, source/tree identities, authenticated join, complete downloaded inventories and equality checks were verified above. No source file is uncommitted; the pre-existing STATE change remains owned by the orchestrator.
