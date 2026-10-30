---
phase: 06-retail-cartridge-boards
plan: "08"
status: complete
subsystem: testing
tags: [fuzz, properties, transactions, isolation, retail]
requires:
  - phase: 06-07
    provides: Truthful loaded retail maps and identified-layout equivalence
provides:
  - Bounded explicit retail execution builders and loader/execution seed coverage
  - Independent normalization checks and deep failed-load continuation controls
  - Retail property coverage with PCM, pointer-cleared state and preserved shrinking
affects: [06-09]
tech-stack:
  added: []
  patterns:
    - Reuse the existing isolated test database to make normalization coverage mandatory
    - Verify refused loads against audible changing controls and resumed execution
key-files:
  created: []
  modified:
    - tests/fuzz/run_cart.h
    - tests/fuzz/run.c
    - tests/fuzz/loader.c
    - tests/fuzz/seeds.c
    - tests/fuzz/run_seeds.c
    - tests/prop/test_prop.c
    - tests/core/test_core.c
    - tests/fuzz/tests.cmake
key-decisions:
  - Keep original linear execution inputs while using explicit supported headers to select every retail board.
  - Keep execution output and seed storage at131200 bytes; use the loader's existing larger bound for the larger Activision geometry neighbor.
  - Reserve one warm frame within the existing eight-frame per-instance property bound and preserve same-case shrinking.
requirements-completed: []
requirements-contributed: [BOARD-01, BOARD-02, BOARD-03]
coverage:
  - id: explicit-fuzz-selection
    description: Both replay targets select every retail board and retain original linear and negative replay controls
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "cmake --build --preset ci && ctest --preset ci -R '^fuzz[.]regress[.](loader|run)' --no-tests=error --output-on-failure; exit 0; 4/4; URL N/A-local"
        status: pass
    human_judgment: false
  - id: deep-load-failures
    description: Refusals and allocation failures preserve nontrivial loaded state, physical RAM and resumed CPU, frame and audio observations
    requirement: BOARD-02
    verification:
      - kind: integration
        ref: "Direct loader replay --seeds, execution replay --seeds and four property invocations; exit 0 each;31 loader/28 execution seeds and100 cases per property; URL N/A-local"
        status: pass
    human_judgment: false
  - id: allocation-and-isolation
    description: All properties cover every retail board, compare independent instances and deep state, and retain allocation-free bounded execution
    requirement: BOARD-03
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; exit 0;612 non-release plus6 release checks; URL N/A-local"
        status: pass
      - kind: other
        ref: "Exact612/6 JUnit inventories, unchanged product/API/docs/fixture/hash inputs, hygiene and exactly-three ZIP sum214131 <=399937; exit 0 each; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 9569
  tasks: 2
  commits: 2
plan_head_before: 71a48712fa2c9f899546d4749931894b3f27c8ae
plan_head_after: d090f1718ddae9a393815619b9802efd1e8a5bf4
duration: 9min
started: 2026-10-23T20:30:16Z
completed: 2026-10-23
---

# Phase 6 Plan 8: Bounded hostile retail execution Summary

**Explicit retail fuzz seeds and four deterministic properties now prove deep failed-load preservation and resumed frame/audio equality with nonzero banks and physical RAM.**

## Task commits

1. `6128b32` — retail fuzz builders, explicit seed selection, independent normalization and deep resumed-load controls.
2. `d090f17` — deep retail properties, allocation/lifecycle assertions, bounded seeds and unchanged-duration engine limits.

The persisted ledger measures two task commits. Actual tokens are the 38,275-character realized task diff divided by four, rounded up. Summary and subsequent state metadata are outside that interval.

## Accomplishments

- `run_cart` returns actual byte length and requested board. Original 48 KiB headerless linear bytes/footer policy, all seventeen original seeds and committed regression inputs retain their interpretation. Explicit A78 board declarations select Activision128 KiB, Absolute64 KiB or Fractalus32 KiB; supported headers and fixed-window reset vectors are constructed within131,200 bytes. Every execution input probes and requires the requested board before real bounded frames.
- Execution replay has28 seeds; loader replay has31. New cases cover all three boards, malformed geometries and valid unsupported SuperGame/A8 and BankSet/A8 combinations. Seed construction asserts valid board selection and malformed/unsupported outcomes. The execution corpus uses a truncated declared Activision case to stay within131,200 bytes; the actual larger geometry neighbor is in the loader corpus under its unchanged1,048,705-byte cap. Direct traffic covers all256 selector data values, address neighbors, and every Fractalus alias page/byte rather than hoping random instructions find them.
- Loader replay and engine use the existing uninstalled test database and shipped core objects, making the identified half-swapped seed mandatory without adding production keys. The normalization invariant independently enumerates each bank's two8 KiB input halves against the installed destination halves; unnormalized images retain ordinary byte equality. RAM must be zero and the single allocation exactly ROM plus physical RAM. Test-key absence remains verified by existing CI.
- Loader failures are also exercised after real execution with selected banks or nontrivial2048-byte physical RAM. An owned fixed-ROM program establishes changing system-RAM and audible synthesis history. A refusal or forced allocator failure must preserve the full local register snapshot and separately copied deep ROM/RAM. Two resumed frames must equal an independent unchanged control in CPU registers, system RAM, deep cartridge bytes, picture and PCM; PCM is required nonzero. Allocation/free counts protect the transaction and continued execution.
- Each property deterministically includes all three boards before its randomized pool choices, asserts the selection and fails on missing coverage. Peek purity, split-run, interleaved independent controls and input-at-VBLANK now include nonzero bank/physical-RAM state, real warm execution, geometry refusal and forced allocation failure. State digests clear pointers and include all ROM/RAM bytes. Frame records also contain canonical little-endian PCM hashes. One warm frame is reserved within the existing maximum eight frames per instance. Shrinking reuses the same pool case and random stream.
- Every property still runs100 ordinary cases from the established seed. Existing allocation/lifecycle tests retain all three boards' bus/frame/power-cycle checks and gain peer free-count/capacity assertions. The execution fuzzer's static bound and max_len agree at131,200; both engines retain max_total_time60 and timeout2. No lane or genuine test registration was added, so exact native/WASM inventories remain unchanged.

## Commands, exits and run URLs

All commands ran locally; URL is N/A-local. No logs are committed.

| Command | Exit | Result |
|---|---:|---|
| Initial loader replay during control-ROM construction | 8 via CTest | Exposed the missing7800 decode setup before audio writes; corrected below |
| `cmake --build --preset ci && ctest --preset ci -R '^fuzz[.]regress[.](loader\|run)' --no-tests=error --output-on-failure` | 0 | 4/4, including both no-input rejection controls; repeated after tracer commit |
| `ctest --preset ci -R '^prop[.]' --no-tests=error --output-on-failure` | 0 | Four properties plus two built-sample fixtures |
| Direct `build/ci/tests/muhtari-fuzz-loader-replay --seeds` | 0 | 31 seeds, mandatory normalized Activision included |
| Direct `build/ci/tests/muhtari-fuzz-run-replay --seeds` | 0 | 28 seeds, bounded requested-board assertions included |
| Direct `build/ci/tests/muhtari-test-prop NAME build/ci/built` for split_run, peek_pure, instances_isolated and input_at_vblank | 0 each | Each prints100 cases passed, seed20261020 and coverage of Activision, Absolute, Fractalus |
| Final `cmake --workflow --preset ci` | 0 | 612/612 non-release checks, three fresh ZIPs and6/6 release checks |
| `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` | 0 | All three packages refreshed and six release checks pass |
| cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake | 0 | Exact612 tests ran once each |
| cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake | 0 | Exact6 release checks ran once each |
| git diff --exit-code 71a4871 -- src include docs tests/roms/manifest.txt tests/built/expected.txt tests/audio/expected.txt tests/inventory/ci.txt tests/inventory/wasm32.txt | 0 | Product/API/docs/fixtures/prior predicates and exact inventories unchanged |
| `scripts/hygiene.sh --tree` and `git diff --check` | 0 each | Hygiene and whitespace pass; normal commit hooks retained |
| Exactly-three ZIP sum below | 0 | 214,131 <=399,937, delta0 from plan07 |

```sh
set -- build/ci/muhtari-*.zip
test "$#" -eq 3 || exit 1
archive_bytes=0
for archive do
  size=$(wc -c < "$archive")
  printf '%s %s bytes\n' "$archive" "$size"
  archive_bytes=$((archive_bytes + size))
done
printf 'archive_bytes=%s ceiling=399937\n' "$archive_bytes"
test "$archive_bytes" -le 399937
```

| Component ZIP, macOS arm64 | Bytes |
|---|---:|
| library | 68,753 |
| libretro | 68,092 |
| runner | 77,286 |
| Total | 214,131 |

Exactly three fresh archives were measured. Existing product binaries and the local total remain unchanged; six-native fit is still plan09 acceptance.

## Decisions and deviations

Followed the selected bounded builders, existing test-only seam and existing replay/property mechanisms. The test-database link in tests/fuzz/tests.cmake moved into task1 because normalization coverage cannot execute without it. Task2 finalized seed outcome assertions and bounds alongside the property pool. This is sequencing within the planned scope; no architectural change, dependency or new resource bound was introduced.

The initial changing/audio control omitted the explicit7800 decode setup already used by existing owned programs. The test's changing-RAM assertion exposed that construction error; setting INPTCTRL and disabling DMA before audio writes corrected it. No product bug or weakened invariant was used to pass. No unresolved issue, known stub, skipped test or unrun verify required by this plan remains, and no review-fix or gap allowance was consumed.

## Limits and next readiness

No BOARD requirement is complete. Actual hosted sanitizer/fuzz/nightly and six-native byte/equality acceptance remain assigned to09, with no new job, duration, owner UAT or hardware request. BOARD fallback/prohibition metadata remains unresolved for phase acceptance. Hardware/embedding/host docs from05/07 remain accurate and unchanged; no product source, public API, fixture or prior expected hash changed. No new unplanned security surface was introduced and historical retired-policy artifacts remain untouched.

Next incomplete plan: **06-09**. The parent owns remaining phase dispatch and gates within the current execution step; this executor stops after08.

## Self-Check: PASSED

All eight changed task files exist; both task commits are ancestors of HEAD and the persisted ledger measures two commits. Full local CI, release checks, direct bounded replay/property counts, exact inventories, hygiene and the three-ZIP bound passed. No unexpected deletion or generated untracked file remains.
