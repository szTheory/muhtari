---
phase: 12-player-ready-delivery-and-playstead-bridge
plan: "10"
subsystem: testing
tags: [C, 6502, CTest, input, RetroArch-diagnostics]
requires: []
provides:
  - Bounded helper-post aggregation and exact-child cutoff classification for local diagnostics
  - Opt-in ordered-input history cartridge with an independent pixel judge and actual-cartridge controls
affects: [testing, diagnostics, PLAY-03]
tech-stack:
  added: []
  patterns:
    - Fixed-size per-key observation aggregates retain first and transition POST records without changing the writer protocol
    - Generated owned cartridge history is judged from fixed RGB samples independent of the cartridge state logic
key-files:
  created:
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-10-SUMMARY.md
  modified:
    - tests/retroarch/keyboard_preflight.c
    - tests/retroarch/input_trace_helper.h
    - tests/retroarch/test_input_trace_helper.c
    - tests/policy/retroarch_preflight.c
    - tests/libretro/input_trace.cmake
    - tests/input/test_input.c
    - tests/input/pixels.c
    - tests/input/tests.cmake
key-decisions:
  - Keep the repair local and diagnostic-only; PLAY-03 acceptance remains pending at 0/4
  - Preserve the helper protocol, input schedule, and existing CTest identities and time limits
metrics:
  duration: 24m
  completed: 2026-11-02
status: complete
plan_head_before: 66a320d3661de23aba9dd42b48f0baa77ea30c11
plan_head_after: 97d1af6550445f7b50a1fe270fa76afb70d21dfc
commits: 5
actuals:
  tokens: 5881
  tasks: 3
  commits: 5
requirements-completed: []
coverage:
  - id: PLAY-03-diagnostic
    description: "Local helper and generated-cartridge controls improve diagnostic reliability only; they do not prove actual frontend input delivery or complete PLAY-03."
    requirement: PLAY-03
    verification:
      - kind: integration
        ref: "tests/input/test_input.c#proline_one_button"
        status: pass
      - kind: integration
        ref: "tests/input/tests.cmake#input.history_cart"
        status: pass
      - kind: integration
        ref: "tests/libretro/input_trace.cmake#helper-high"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/41461948345 (historical failed, incomplete host observation)"
        status: fail
    human_judgment: false
---

# Phase 12 Plan 10: Local Input Diagnostic Repair Summary

**The helper retains bounded transition evidence through its cutoff, and an opt-in owned cartridge now shows an independently judged ordered hold-and-release result.**

Primary implementation files: `tests/retroarch/input_trace_helper.h` and `tests/input/test_input.c`.

## Performance

- **Duration:** 24 minutes
- **Started:** 2026-11-03T01:26:59Z (initial private preservation baseline)
- **Completed:** 2026-11-03T01:51:12Z
- **Tasks:** 3/3
- **Files modified:** 8 source/control files, plus this summary and normal STATE/ROADMAP bookkeeping

## Accomplishments

- Helper diagnostic POST records now retain first observations and down/up transitions, with fixed-size per-key/state counts and observed first/last times. The mocked event schedule and the 64-record/4096-byte protocol remain unchanged. At the no-ack cutoff, the preflight checks the exact child with `waitpid(..., WNOHANG)` before cleanup and preserves either the observed raw status, an unknown still-running status, or the wait error.
- The input test generator accepts `--write-cart FILE history` and writes an ignored generated cartridge. Its sampled state moves from waiting to held only for exact P1 Up plus button 1 with other controls idle, and reaches sticky completion only after a later neutral sample. The independent four-band RGB judge accepts actual completed pixels and classifies waiting, held, dimensions, and corrupted signatures separately.
- Actual cartridge controls cover idle-only, early release without a later release, stuck-held, wrong port, wrong button, partial chords, a delayed valid sequence, next-sampled-frame release, and 260 neutral frames across the frame counter wrap. Existing plain/even cartridge outputs are byte-identical to their private pre-change baselines.
- Post-review controls read back the generated history cartridge and compare its complete serialized bytes with the expected history image. The descriptor audit now enumerates open descriptors, checks a leaked alias at fd 128 or above, and fails if directory enumeration is incomplete.
- The full local `ci` workflow and exact ordinary/release inventories pass. No RetroArch process, hosted job, remote service, publication, or acceptance action ran.

## TDD Evidence

### Task 1: Helper trace aggregation and cutoff observation

- **RED:** The focused `policy.retroarch.preflight_api` control failed at the new 5000 ms cutoff expectation; `policy.retroarch.child_failure` remained passing. Configure/build succeeded. The RED evidence classifier returned `RED_EVIDENCE_OK`, and semantic inspection confirmed the target assertion—not a setup or compile fault—failed.
- **GREEN and tracer gate:** `cmake --preset ci` and `cmake --build --preset ci` exited 0. The final focused selector `ctest --test-dir build/ci -R '^(policy[.]retroarch[.](preflight_api|child_failure))$' --no-tests=error --output-on-failure` exited 0 with both controls passing. This repeats the tracer verification gate end-to-end; no human checkpoint was required.
- **Receipts:** Separate RED and final GREEN JUnit/command receipts are under `/private/tmp/muhtari-phase12-10-RjNszgLd/`. Local command URLs are `N/A-local`.

### Task 2: Ordered history cartridge and pixel oracle

- **RED:** `ctest --test-dir build/ci -R '^input[.]proline_one_button$' --no-tests=error --output-on-failure --output-junit build/ci/phase12-history-red.xml` exited 8. The named target compiled and ran, then failed the planned completion-marker assertion: observed palette index `0x09`, expected `0x46`. The unchanged JUnit report was accepted by `gsd_run check tdd-red-evidence build/ci/phase12-history-red-evidence.json --raw` as `RED_EVIDENCE_OK`.
- **GREEN:** `cmake --build --preset ci` exited 0. The focused `input.cart_files`, `input.proline_one_button`, and `runner.movie.replay` selector exited 0 with all three tests passing. The writer generated the history fixture; an unsupported `other` selector exited 2 and created no output.
- **Plain/even identity:** The generator exited 0. SHA-256 matched before and after for plain (`36009274274c48b9dcc299647bdf9e744f74a383ba174eedeb77fae729f36f35`) and even (`4b806be38aa7fbff744f023f5e26d4db0eb68b8ff18e180f938d181783e1c03b`). The generated ignored history cartridge is 49,280 bytes.
- Local command URLs are `N/A-local`; detailed command output and the generated JUnit report remain in `build/ci` or the private receipt root.

## Final Local Verification

| Check | Command | Result | Run URL |
|---|---|---:|---|
| Full local workflow | `cmake --workflow --preset ci` | Exit 0; 678 ordinary tests and 6 release tests passed; three local packages built | N/A-local |
| Ordinary inventory | `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | Exit 0; 678 tests ran once each | N/A-local |
| Release inventory | `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | Exit 0; 6 tests ran once each | N/A-local |
| Committed diff whitespace | `git diff --check 66a320d..HEAD` | Exit 0 | N/A-local |
| Working-tree whitespace | `git diff --check HEAD` | Exit 0 | N/A-local |
| Stub scan | Added-line scan of the eight scoped source/control files for TODO/FIXME/placeholders/empty rendered values | No matches | N/A-local |
| Independent bounded review | `$gsd-code-review 12 --depth=standard` on the ten-file final scope after two bounded fix rounds | Clean; no findings remain | N/A-local |

## Post-review Fixes

The standard code review found three issues, all resolved in `db134c4`:

- `WR-01`: the helper descriptor audit stopped at fd 127. It now enumerates the process's open descriptors, fails on enumeration errors, and has a high-fd leak control.
- `WR-02`: the generated history cartridge was not consumed. A fixture-dependent test now reads it back and checks its full serialized image against the expected history cartridge.
- `IN-01`: the two history response codes could drift. Their declarations now live in `tests/input/input_pixels.h`, shared by the judge and consumer.

The final standard-depth review covered ten files and found no remaining issues. The follow-up local `ci` workflow passed with 678 ordinary tests, 6 release tests and all three packages; both exact-inventory checks passed.

The full workflow output, exact focused JUnit reports, inventory outputs, and byte-identity receipts are retained privately at `/private/tmp/muhtari-phase12-10-RjNszgLd/`. Full local workflow is not hosted CI.

## Task Commits

1. **Task 1 RED:** `c427de3` — assert helper trace and cutoff child evidence.
2. **Task 1 GREEN:** `425ed12` — bound helper trace and observe cutoff child.
3. **Task 1 repeatability fix:** `bbb61c1` — isolate synthetic helper release output.
4. **Task 2 RED:** `50d8dd5` — add failing history completion assertion.
5. **Task 2 GREEN:** `97d1af6` — add generated ordered-input history cartridge.

The plan ledger measured 5 task commits from `66a320d3661de23aba9dd42b48f0baa77ea30c11` through `97d1af6550445f7b50a1fe270fa76afb70d21dfc` when this summary was written. The final STATE/ROADMAP metadata commit is recorded separately by the execution workflow.

## Files Created/Modified

- `tests/retroarch/input_trace_helper.h` — fixed-size transition and repeat aggregates without per-repeat record growth.
- `tests/retroarch/keyboard_preflight.c` — exact-child status observation at the acknowledgment cutoff before cleanup.
- `tests/retroarch/test_input_trace_helper.c`, `tests/policy/retroarch_preflight.c`, `tests/libretro/input_trace.cmake` — synthetic, cutoff, high-descriptor, and actual-wrapper controls.
- `tests/input/test_input.c` — generated history variant, serialized-image check, and real-cartridge state/fault schedules.
- `tests/input/input_pixels.h` and `tests/input/pixels.c` — shared private response codes and independent fixed-RGB state/failure judge.
- `tests/input/tests.cmake` and `tests/inventory/ci.txt` — registered the history-cart byte check under the generated cartridge fixture and exact CI inventory.
- `12-10-SUMMARY.md` — local-only execution record.

## Decisions Made

- Kept this repair local and diagnostic-only. It cannot resolve the missing frontend-delivery evidence or any other Phase 12 acceptance predicate.
- Kept the plain/even generator paths, trace record protocol, input event schedule, test registrations, and deadlines unchanged.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Made the synthetic helper output repeatable.**
- **Found during:** Task 1 tracer feedback gate.
- **Issue:** A fixed raw-release directory collided when the same focused test ran a second time.
- **Fix:** Use a fresh `mkdtemp` directory for each synthetic release.
- **Files modified:** `tests/retroarch/test_input_trace_helper.c`.
- **Verification:** Final task 1 focused selector passed after the change.
- **Committed in:** `bbb61c1`.

**2. [Rule 3 - Build blocker] Declared the test-private pixel judge before definition.**
- **Found during:** Task 2 GREEN build.
- **Issue:** The shared `pixels.c` object is compiled into multiple test executables with `-Wmissing-prototypes -Werror`.
- **Fix:** Add the test-private function prototype in `pixels.c` and its existing consumer declaration in `test_input.c`.
- **Files modified:** `tests/input/pixels.c`, `tests/input/test_input.c`.
- **Verification:** Full `ci` build passed.
- **Committed in:** `97d1af6`.

**3. [Rule 1 - Bug] Corrected history raster loop and sample centers from observed cartridge output.**
- **Found during:** Task 2 focused actual-cartridge checks.
- **Issue:** The first WSYNC loop branched to the color setup and reinitialized its line count; after correcting that, the actual first band began at framebuffer row 0, not row 24.
- **Fix:** Branch back to the WSYNC line wait and sample the four band centers at rows 14, 42, 70, and 98.
- **Files modified:** `tests/input/test_input.c`, `tests/input/pixels.c`.
- **Verification:** Waiting/held/completed states, all listed input faults, signature corruption controls, and the long neutral tail passed.
- **Committed in:** `97d1af6`.

---

**Total deviations:** 3 auto-fixed issues (2 Rule 1, 1 Rule 3).
**Impact on plan:** All fixes stayed within the authorized test/diagnostic scope and retained the planned interfaces and bounds.

## Preservation and Evidence Limits

- The execution-start gate passed with the eight source paths and all automatic final-commit paths clean, the summary path absent, and an empty index. The branch remained `phase/12-player-ready-delivery-and-playstead-bridge`.
- Private baseline HEAD was `66a320d3661de23aba9dd42b48f0baa77ea30c11`. Before summary/state bookkeeping, the raw Git-listed inventory remained 910 paths; exactly the eight scoped source/control paths differed. `.planning/REQUIREMENTS.md` remained byte-identical, the index was empty, and all 417 files in retained receipt root `/private/tmp/muhtari-trace-observation-20261102` matched their initial private hashes/modes/aliases.
- Normal state/roadmap bookkeeping now records 12-10 complete and the plan count as 6/9. Phase 12 remains halted at 12-01 with accepted 0/4; `REQUIREMENTS.md` is byte-identical and no requirement was checked off.
- The post-bookkeeping, pre-metadata-commit preservation comparison found 911 listed paths versus 910 initially: only the eight scoped source/control paths and `.planning/STATE.md`/`.planning/ROADMAP.md` changed, with this summary as the sole new path. There were no missing or unexpected deltas; the index was empty, the branch was unchanged, all 417 retained receipt files matched, and `REQUIREMENTS.md` remained unchanged. The metadata commit stages the summary and the two intended state files; the requirement path is unchanged and not staged.
- Ignored `build/ci` grew from 8,711 to 8,878 files during configure/build, focused checks, the full workflow, packaging, release checks, and the final inventories. Comparing baseline hashes, sizes and modes with current readable files found 414 added/removed/content-changed paths and 6,657 content-identical files with mtime-only changes; all are under the ignored build tree. The locked fixture's mtime also changed, but its bytes remain unverified because it was unreadable throughout. No `.wip` or `.stats` paths changed.
- The existing `build/ci/title/local_fixture/dir/locked.a78` remained mode `0200`, size 12, and unreadable (`PermissionError:13`) both before and after the established `title.local.fixture` recipe. Its bytes remain unverified; no permission change or bypass was used.
- An initial private comparison draft reported `CLAUDE.md` because its current symlink snapshot omitted the size field. The corrected comparison included that field and found no mismatch: `CLAUDE.md` remains a symlink to `AGENTS.md`. No repository edit was made to it.
- The original hosted [run 41461948345](https://github.com/szTheory/muhtari/actions/runs/41461948345) remains the historical failed/incomplete observation: zero captures at the 5014 ms cutoff, unknown child status, and an unfinished INIT-only core trace. The new local evidence proves no actual frontend delivery. Original input cause is unknown; accepted remains 0/4, PLAY-03 remains pending, D-04/D-05/D-06 host predicates remain unproven, Playstead evidence remains separate, all hosted allowances remain spent, and no retry or successor authorization follows.
- No source, requirement, publication, host, remote, pin, dependency, permission, timeout, workflow, or acceptance change outside this plan occurred.

## User Setup Required

None. No external service or owner action is needed to retain this local result.

## Next Phase Readiness

This diagnostic plan is complete. Phase 12 remains halted at accepted 0/4; this summary does not unblock the acceptance plans or authorize another host observation. Any later host action needs its own bounded scope and stopping rule.


## Follow-up implementation integration — 2026-11-05

A later owner-authorized local refinement in the Phase 13 checkout made the
ordered-history cartridge a generated CTest fixture consumed by
`input.proline_one_button`. The refreshed implementation and independent pixel
predicate are now integrated here as `tests/input/history_pixels.c` and
`tests/input/history_pixels.h`; the existing `input.history_cart` serialized
image check and CI inventory entry remain in place. This supplements the
original task record above without changing PLAY-03 acceptance.

Root-checkout verification:

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci` | 0 | Configured the merged tree. |
| `cmake --build --preset ci --target muhtari-test-input muhtari-test-retroarch-compare muhtari-test-battery-pixels muhtari-test-diag muhtari-test-retroarch-states muhtari-test-retroarch-receipt muhtari_libretro muhtari-run --parallel 4` | 0 | All changed local targets built with warnings as errors. |
| `ctest --test-dir build/ci -R '^(input\.(cart_files|history_cart|proline_one_button)|diag\.(battery|frontend_states)|retroarch\.states\.controls|policy\.docker_contract\.controls)$' --no-tests=error --output-on-failure` | 0 | Seven selected input, battery, state-control and policy tests passed. |

No RetroArch process, full workflow, hosted CI, remote action or acceptance
attempt ran. The prior Phase 12 host stops and spent allowances remain binding.

## Self-Check

PASSED — this summary exists with a final newline and clean whitespace; all five task commits are ancestors of the source HEAD.

---
*Phase: 12-player-ready-delivery-and-playstead-bridge*
*Completed: 2026-11-02*
