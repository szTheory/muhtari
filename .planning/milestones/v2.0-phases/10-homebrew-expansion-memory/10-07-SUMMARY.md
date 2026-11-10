---
phase: 10-homebrew-expansion-memory
plan: "07"
subsystem: testing
tags: [bankset, diagnostics, provenance, quality-evidence]
requires:
  - phase: 10-06
    provides: Stable CPU memory maps and independent physical storage views
provides:
  - Immutable owned BankSet source and reproducible canonical fixture
  - Exact source-pin and digest admission through the real producer and join
  - Independently judged executable geometry and RAM matrix across real deliverables
affects: [expansion-diagnostics, semantic-equality, final-acceptance]
tech-stack:
  added: []
  patterns:
    - Existing pinned assembly and manifest authority
    - Existing CMake orchestration and native audio/video taps
    - Existing independent PNG reader with the two native heights
key-files:
  created:
    - tests/diag/bankset.s
    - tests/diag/bankset.cfg
    - tests/diag/bankset.a78
    - docs/bankset-tests.md
  modified:
    - tests/roms/manifest.txt
    - tests/diag/driver.c
    - tests/diag/expected.csv
    - tests/diag/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/cmake/quality.cmake
    - tests/policy/quality.cmake
    - tests/libretro/tests.cmake
    - tests/runner/png_read.c
    - tests/runner/png_read.h
    - docs/emulation-tests.md
    - docs/quality.md
key-decisions:
  - Keep the canonical template immutable and derive only build-directory geometry cases
  - Judge independent CPU records and native pixels before comparing actual host outputs
  - Preserve raw trusted-base manifest and sample identities
requirements-completed: []
coverage:
  - id: D1
    description: Owned source-pinned canonical BankSet fixture with exact rebuild and sole manifest addition
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: diag.rebuild and diag.rebuild.self_test
        status: pass
      - kind: other
        ref: Immutable source/config Git comparison and scripts/hygiene.sh --tree
        status: pass
    human_judgment: false
  - id: D2
    description: Exact BankSet source and digest admission through actual recursive producer and authenticated join
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: policy.quality.controls
        status: pass
    human_judgment: false
  - id: D3
    description: All fourteen accepted geometry and RAM combinations plus two PAL representatives execute on actual hosts
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: diag.bankset and diag.bankset_tracer
        status: pass
      - kind: integration
        ref: Existing BankSet CMake script with the directly compiled module-free driver and actual runner
        status: pass
    human_judgment: false
  - id: D4
    description: Strict numeric and pixel judgments reject malformed reports and equal or incorrect set graphics before equality
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: diag.bankset report, wrong-view, PNG height, checksum and CRC-valid filter controls
        status: pass
      - kind: integration
        ref: runner.png, runner.dump_png and runner.boot_to_play controls
        status: pass
    human_judgment: false
  - id: D5
    description: Complete local native workflow and genuine preplan three-component archive measurements
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: sh build/ci/task08-jobserver-gate.sh at 3507558
        status: pass
      - kind: other
        ref: Existing quality_evidence BUILD_BASE producer at 3507558 against f124167
        status: pass
    human_judgment: false
actuals:
  tokens: 19522
  tasks: 3
  commits: 8
plan_head_before: f12416734671d8ccaaed017fe7854f26ee1ab6c2
plan_head_after: 350755854ccc3217f1a9582b755158ca759defb2
duration: 44min
started: 2026-10-26T19:06:15Z
completed: 2026-10-26
status: complete
---

# Phase 10 Plan 07: Owned BankSet behavioral evidence Summary

**An immutable MIT BankSet stimulus measures CPU ROM/RAM effects and unequal MARIA pixels across the full selected geometry matrix, with independent judgments before real three-host equality.**

## Accomplishments

- Committed source/config first at 6321fa62cf7cf29c7033b09931edb884079c1545. The existing pinned assembly KEEP path produced the 32 KiB-per-set/no-RAM canonical image, SHA256 32f5870fbfcc4745967b8b94c69f87dac4879587c4b0561256cc5160d61a3390. Its this-repo/MIT row is the only manifest addition; every prior byte remains exact. Automatic rebuild discovery is unchanged.
- The ROM initializes an empty mailbox, measures seventeen CPU observations, and computes its CRC at runtime. Two bounded sweeps perform all 256 selector values at both bank endpoints. Actual stores measure shared RAM and HALT redirects while fixed upper ROM remains readable. The driver preserves fixed CPU code, creates distinct bank tags, validates geometry/metadata and template identity, and never derives expected values from production mapper helpers.
- All six no-RAM sizes and shared/HALT RAM for 32/128/256/512 KiB execute: fourteen accepted configurations. Two representative PAL cases bring the matrix to sixteen cases. Each uses twelve calls on the public library, actual runner process and loaded module; CPU work finishes by call four, followed by eight independently judged full display frames. The older narrow tracer now uses the same admitted template.
- Expected CSV values follow documented geometry and owned literal data. The last switched bank contains fixed-code marker D0 and vector high byte C1, included explicitly in the independent sweep arithmetic. DMA line32 at OFFSET0 plays on raster33/native row17. ROM C0/C8 versus CPU 40/80 and shared/HALT RAM distinguish actual views. Both library and module judge native samples; the runner's actual frame5 PNG is independently parsed and judged.
- Numeric count/order/identity/flags/CRC, reserved bytes and values pass before host equality. Missing/truncated/extra reports, duplicate/reordered records, wrong identities, CRC corruption, CRC-valid wrong values, hidden extra data and wrong graphics fail their actual consumers. Runner PNG controls include both heights, bad checksums, wrong-view pixels and CRC/Adler-valid nonzero filters. Native frames, canonical raw tuples, PCM and counts then agree across the real hosts.
- Exact fixture admission accepts only the named immutable BankSet row. Current and later-base cases exercise the genuine recursive producer and unchanged authenticated join. Forged source pins/digests, changed old rows, duplicates, missing old rows, unrelated additions and wrong producer/configuration/run/base identities fail. Raw original base manifest/sample bytes remain the comparison authority.

## Task commits

| Task | Commit | Result |
|---|---|---|
| 1 | 6321fa6 | Immutable owned source/config pin |
| 1 | 2a093be | Rebuilt signed/headered canonical fixture and sole manifest row |
| 2 | cef9f37 | Exact actual-row producer/join admission and fault controls |
| 3 RED | 3dbd907 | Executed owned fixture exposes missing seventeen-record judgment |
| 3 GREEN | 53b9318 | Geometry/RAM matrix, independent records/pixels and actual host comparisons |
| 3 self-check | 975fdc0 | CRC-valid PNG filter rejection |
| 3 calibration | 9abaf16 | Existing timeout rule applied to the complete measured workload |
| 3 portability | 3507558 | Module-free compilation and portable inventory rows |

The persisted ledger measures eight commits before this summary commit. Actuals are ceil(78087 realized diff characters / 4) = 19522, covering seventeen changed files; this is not a harness token count.

## TDD evidence

RED command: `ctest --preset ci -R '^diag[.]bankset$' --no-tests=error --output-on-failure --output-junit tests/diag/bankset-red.xml`, exit8, N/A-local. The actual ROM executed and emitted seventeen records; the old tracer judge expected six and failed the intended count assertion. The unchanged fresh JUnit document and command/status record were checked with `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/tests/diag/bankset-red.json --raw`: exit0, RED_EVIDENCE_OK. Semantic assessment: this was an executed numeric assertion, not discovery, setup or syntax failure.

GREEN replaces the six-value special case with independent per-scenario CSV expectations, runtime-measuring owned bytes, all geometry/RAM variants, real host observations and strict controls. Final exact test and complete workflow pass below. No separate behavior-changing refactor was needed.

## Verification evidence

Every run URL in this table is N/A-local. No logs are committed.

| Command / source | Exit | Result |
|---|---:|---|
| Existing assemble.cmake KEEP invocation for bankset.s/bankset.cfg, followed by ci configure/build and `ctest --preset ci -R '^diag[.]rebuild' --no-tests=error --output-on-failure` | 0 | Canonical digest exact; rebuild and hash-fault control pass |
| `ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure` | 0 | Exact real-row admission, recursive producer and authenticated join controls |
| `ctest --preset ci -R '^(diag[.]|runner[.].*png|runner[.]boot_to_play|policy[.]quality[.]controls$)' --no-tests=error --output-on-failure --output-junit tests/diag/bankset-consumers.xml` | 0 | All26 selected/dependency tests pass |
| `ctest --preset ci -R '^(diag[.]bankset|docs[.]budgets|runner[.]png$|runner[.]dump_png$|runner[.]boot_to_play)' --no-tests=error --output-on-failure --output-junit tests/diag/bankset-green.xml` | 0 | Matrix, canonical case, existing PNG/boot consumers and documentation budgets pass |
| `cc -std=c17 -O3 -Wall -Wextra -Wpedantic -Werror -Iinclude -Isrc -Ihost tests/diag/driver.c tests/runner/png_read.c runner/sha256.c build/ci/libmuhtari.a build/ci/libmuhtari_host.a -o build/ci/task10-plan07-no-module-diag` | 0 | Actual driver compiles/links without DIAG_LIBRETRO |
| `cmake -DDIAG_BANKSET_RUN=ON -DBANKSET_EXPANDED=ON -DROOT=. -DOUT=build/ci/task10-plan07-no-module-cases -DDRIVER=build/ci/task10-plan07-no-module-diag -DRUNNER=build/ci/runner/muhtari-run -DEXPECTED=tests/diag/expected.csv -DMODULE= -P tests/diag/tests.cmake` | 0 | Entire module-free matrix and controls through library and actual runner |
| `sh build/ci/task08-jobserver-gate.sh` at3507558 | 0 | Unchanged full workflow:660 ordinary tests in48.13s, all6 release tests; repeated6 release tests, exact660 inventory and hygiene pass |
| Same unchanged full command at53b9318 /975fdc0 /9abaf16 | 0 | Preserved complete660+6 passes before later execution-time corrections |
| `sh /private/tmp/muhtari-bankset-quality.sh` at3507558 | 0 | Fresh actual current660+6/base659+6; base ordinary52.69s; genuine independent f124167 build and all three ZIPs |
| Source/config Git-byte and manifest/digest comparison; `git diff --check`; clang-format21 dry run | 0 | Immutable source pin, exactly one manifest addition, unchanged old bytes and clean formatting |

The complete expanded diagnostic measured1.85s in focused dependent checks and2.32s under the complete workflow; the final full run measured1.71s. The provisional timeout is ceil(2 x2.32)=5s under the existing rule, with existing lane scaling and every assertion/case retained. Hosted calibration remains later acceptance work.

## Actual package measurements

Final source is 350755854ccc3217f1a9582b755158ca759defb2. The fresh independent comparison base is f12416734671d8ccaaed017fe7854f26ee1ab6c2, not a relabelled cache or a claim against origin/main.

The producer reports measured/native/local on both sides, warnings0 and identical AppleClang21.0.0.21000101/Darwin-arm64 environment. Trusted raw fixture identity is 750d8b722c88a03aff050dec5e961a44523a703afcc134f7dce4e3ebf5107932; trusted raw sample identity is d549d4691aca7c004325102613fd6ac2337ecfe8a4c0cec004472e5bbcc80ca8. Both raw files were checked against the actual Git base. Independent filesystem sums match producer JSON.

| Actual archive | Candidate bytes | Preplan bytes | Candidate SHA256 | Preplan SHA256 |
|---|---:|---:|---|---|
| muhtari-library-macos-arm64.zip | 83242 | 83225 | d822806458331f6f4c0a4759b12a8f4ecfe1bafaaadad292b04c89722176eca4 | a180af8a5156927d2c99fa1d21ce1c894d72345f7c6aec082800af53967d5409 |
| muhtari-libretro-macos-arm64.zip | 89625 | 89625 | d12b5a89fb7dccf375334cd34e718d09baaf046b35cf538a3cd791c1a999a7e9 | 6d564ba0c789bab2667e13127e6e707f396d532b5960981eab13ad2ea7a679d5 |
| muhtari-runner-macos-arm64.zip | 98477 | 98477 | 97668675272219abeecee1f222967fe824adcc556b0fe304ebabf0c90d06406c | 1a64370c1d53bf4bbaa3d11b471452bea25081b1f9f07ef22ba0b8b246b77392 |
| Total, report-only | 271344 | 271327 | +17 bytes | No archive cutoff |

An earlier fresh measurement at9abaf16 also passed against the same genuine f124167:271347/current and271316/base bytes,660/base659 ordinary tests plus6 each. Those original reports and actual base ZIPs were preserved separately. They were not relabelled as final-source measurements. Both temporary producer-owned detached base worktrees were removed after preserving their reports and archives.

## Execution-time issues and preserved failures

These are implementation/self-check corrections, not the formal end-of-phase GSD code review, which has not been invoked here.

| Command / failing consumer | Exit | Failure and resolution |
|---|---:|---|
| Initial assemble.cmake KEEP invocation | 1 | Signer required FF at FFF8 in the MARIA tail; flags/vectors were corrected before the immutable source pin |
| Initial `build/ci/muhtari-run --frames 12 --diagnostic build/ci/tests/diag/owned-pin.a78` | 127 | Wrong executable path; used the existing runner/muhtari-run target thereafter |
| `ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure` during admission controls | 8 | The new negative control expected an extra word in the failure regex; actual producer-identity rejection was preserved and the regex corrected |
| Exact diag.bankset command during GREEN | 8 | Derived header size initially used the wrong byte offsets; corrected to the documented four-byte field |
| Same executed diagnostic during native judgment | 8 | Old row40/startup assumptions were wrong for sparse OFFSET15 graphics and runtime CPU measurements; independent raster derivation selects row17 after four warmup calls, preserving twelve calls and eight full judged frames |
| Temporary diagnostic build | 1 | Debug-only references named nonexistent statistics fields; temporary instrumentation was removed |
| Same executed diagnostic during report controls | 8 | Extending the fault loop exposed an out-of-bounds old offset-array lookup; BankSet-specific faults now avoid that lookup |
| Same executed diagnostic at refusal boundaries | 8 | Tests initially expected runner exit3 instead of its documented exit1, then expected INVALID_IMAGE for truncation; exact documented runner and public INVALID/TRUNCATED statuses now pass |
| Module-free syntax compilation | 1 | Two module-only buffers were unguarded; DIAG_LIBRETRO guards and the actual module-free matrix now pass |
| Custom ci configure with MUHTARI_BUILD_LIBRETRO=OFF | 1 | The ci inventory correctly required its21 module-dependent registrations; no gate was bypassed. Direct compilation and the existing module-free script supplied the intended once-only check |

Initial unprivileged Git staging was denied by the workspace sandbox; authorized escalated commits succeeded with hooks and noreply identity. No failed product or check attempt above is counted as a pass.

The first summary commit was rejected by the hygiene hook because its evidence command spelled out a home-directory path. The command now uses the equivalent HOME-based spelling; the hook remained enabled and the corrected metadata commit was retried.

## Deviations from plan

1. **[Rule3 - Blocking consumer reuse]** Extended existing tests/runner/png_read.c/.h to accept only242/292 rows, retaining its242 wrapper and all original format/checksum behavior. Linked that same reader into the second diagnostic target in tests/libretro/tests.cmake. This permits independent actual runner pixels in both regions without a second parser or dependency.
2. **[Rule3 - Documentation budget]** The existing emulation-tests.md was40946/40960 bytes. Moved the detailed BankSet derivation into docs/bankset-tests.md and tightened duplicate overview prose, keeping the original file40896 bytes and preserving the existing budget.
3. **[Rule1 - Portable consumer correctness]** Guarded newly added module-only buffers and added exact BankSet/tracer rows to tests/inventory/wasm32.txt. The source was tested without a module; the row list is sorted. Full WASM/hosted phase acceptance remains with the later plan.
4. **[Rule2 - Numeric image correctness]** The new PNG consumer now rejects nonzero row filters even when CRC and Adler-32 are valid; the real judge exercises that fault in both native heights.

No package/framework, production ownership layer, required report, proxy quota, assertion deletion or owner verification flow was added. No stubs, skipped tests or unrun plan verification remain. Shared STATE/ROADMAP/REQUIREMENTS updates belong to the orchestrator and were not modified by this executor.

## Confidence and readiness

This proves the selected emulator model through the actual three deliverables; it does not establish physical cartridge capacities or new HALT timing fits. Shared RAM and256/512 KiB sizes retain their producer/emulator-extension confidence. Aggregate executable bank sums accompany the existing exact cheap per-bank tests; exact edge/four-pair arithmetic stays in bus tests. Audio comparisons capture actual streams, including silence, rather than infer a pass.

API7/ABI1/behavior6 is unchanged. BOARD-04 is contributed to, not marked complete. Later semantic release equality and final hosted/platform/frontend acceptance remain later-plan work. The ordinary600-frame PAL diagnostic is unchanged and was not multiplied through this matrix.

Within the active execution step, plan10-08 is ready. Next separate allowed command: `$gsd-execute-phase 10`. No additional GSD step was launched.

## Self-Check: PASSED

All seventeen changed files and this summary exist; all eight recorded task commits are reachable ancestors. The final native workflow, module-free matrix, exact fixture identity and fresh independent-base producer pass. Requirements-completed remains empty, and shared state is left to its designated owner.
