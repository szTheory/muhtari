---
phase: 07-battery-saves
fixed_at: 2026-10-24T13:59:00Z
review_path: .planning/phases/07-battery-saves/07-REVIEW.md
iteration: 1
findings_in_scope: 2
fixed: 2
skipped: 0
status: all_fixed
fixed_ids:
  - CR-01
  - CR-02
head: bd7e686d07fca10578be1e5258e3fde32f6a5815
---

# Phase 7: Code Review Fix Report

**Fixed at:** 2026-10-24T13:59:00Z
**Source review:** .planning/phases/07-battery-saves/07-REVIEW.md
**Iteration:** 1 of at most 2 code-review fix rounds

**Summary:**

- Findings in scope: 2
- Fixed: 2
- Skipped: 0

## Fixed Issues

### CR-01: Broken stdout pipe terminates the runner before its required final save

**Files modified:** `runner/stop.c`, `tests/runner/test_control.c`, `tests/runner/test_runner.c`, `tests/runner/tests.cmake`, `docs/runner.md`
**Commit:** f8cc10f
**Status:** fixed
**Applied fix:** Native POSIX runner initialization ignores SIGPIPE so failed output reaches the existing error and final-save path. Host policy stays in the runner; the core acquires no I/O or signal behavior. Existing stop/save signal handlers retain inherited-ignore semantics. Public runner prose states the broken-pipe exit and final-save behavior.

The existing native controls test now executes actual child runners with a pipe whose read end is closed. A single hash at frame 120 exercises final buffered flushing; 256 hashes from frames 120 through 375 force buffered writes during execution. Both cases run with SIGPIPE explicitly default and inherited ignored, require ordinary exit 1, and independently inspect the exact length and every expected byte of the diagnostic's 34,816-byte combined save. The existing stop test additionally exercises inherited ignored SIGINT/SIGTERM; existing SIGUSR1 and native stop/on-demand cases remain active. No new test registration or inventory change was needed.

### CR-02: A refused late save-device request drops joystick A-button fire

**Files modified:** `libretro/libretro.c`, `tests/libretro/host.c`, `docs/libretro.md`
**Commit:** bd7e686
**Status:** fixed: requires human verification
**Applied fix:** Resolve each effective port once, including automatic cartridge metadata, and use that result both for the core input device and RetroPad button translation. A refused late storage attachment therefore retains the loaded joystick's A-to-fire mapping. The documentation explicitly preserves the installed button mapping.

The existing loaded-module layout test holds A throughout late SaveKey and AtariVox requests through both controller callbacks and core options, on both ports, with absent storage and an HSC span. Option requests use an Auto callback against ProLine metadata to exercise the former independent-resolution defect. The actual cartridge's INPT4/INPT5 samples must remain fired, each changed request must warn about reload exactly once, repeated requests must not warn again, and the published span pointer, length and complete HSC contents must stay unchanged. Existing automatic ProLine and ordinary controller-change assertions still run.

The status above is the fixer's required logic-change review flag, not an owner UAT request. Automated semantic regressions pass; the orchestrator owns the phase re-review and final acceptance.

## Verification

All checks ran in the **main checkout** on the existing phase branch, honoring `workflow.use_worktrees=false`. The final verified source changes were committed unchanged as `bd7e686d07fca10578be1e5258e3fde32f6a5815`. Modified sections were reread after each fix and compiled with the repository's warning settings. No full suite ran between findings; full CI ran after the final source change. Run URL for every local command below: **N/A-local**.

| Command | Exit | Evidence |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci --target muhtari-test-control && caffeinate -i ctest --preset ci -R '^runner[.]save[.]controls$' --no-tests=error --output-on-failure` | 8 | Pre-fix regression: both default-SIGPIPE child cases terminate abnormally and produce no save; inherited-ignore controls succeed. |
| `cmake --build --preset ci --target muhtari-run muhtari-test-runner muhtari-test-control muhtari-test-save-runner && caffeinate -i ctest --preset ci -R '^runner[.](stop\|save[.](controls\|cadence\|faults))$' --no-tests=error --output-on-failure` | 0 | Four targeted runner tests pass after CR-01. In this table the displayed regex alternation bars are Markdown-escaped; the executed command used ordinary bars. |
| `cmake --build --preset ci --target muhtari-test-libretro-host && caffeinate -i ctest --preset ci -R '^libretro[.]host[.]layout$' --no-tests=error --output-on-failure` | 8 | Pre-fix loaded-module regression: held-A INPT4/INPT5 samples are released after refused requests; the input fixture setup passes. |
| `cmake --build --preset ci --target muhtari_libretro muhtari-test-libretro-host && caffeinate -i ctest --preset ci -R '^libretro[.]host[.](layout\|devices\|input\|battery\|battery_order)$' --no-tests=error --output-on-failure` | 0 | Five targeted adapter tests plus their input fixture pass. Regex alternation bars have the same display escaping described above. |
| `caffeinate -i cmake --workflow --preset ci` | 0 | All 641 normal tests, three-component packaging and all 6 release checks pass. |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 641 required normal names run once each without failure/error/skip. |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 6 release names run once each. |
| `caffeinate -i cmake --workflow --preset hygiene` | 0 | All 24 checks pass, including format, analyzer and tree checks. |
| `cmake -DJUNIT=build/hygiene/junit.xml -DINVENTORY=tests/inventory/hygiene.txt -P tests/cmake/check_junit.cmake` | 0 | Exactly 24 hygiene names run once each. |
| `scripts/hygiene.sh --tree` | 0 | Complete tracked tree passes. |
| `scripts/hygiene.sh --history f7e6bb1..HEAD` | 0 | Both fix commits pass history hygiene and use the GitHub noreply identity. |
| `git diff --check` | 0 | No whitespace errors. |

Additional exact local measurement command, exit 0, run URL N/A-local:

```sh
python3 -c 'from pathlib import Path; p=Path("build/ci"); z=sorted(p.glob("muhtari-*-macos-arm64.zip")); assert len(z)==3; total=sum(f.stat().st_size for f in z); assert total<=399937; h=(p/"hashes.txt").read_text().splitlines(); assert len(h)==69; print("three archives:",total,"bytes <= 399937; hashes:",len(h),"rows")'
```

The actual local three-component sum is 228,435 bytes and the complete evidence file retains 69 rows. No quota, timeout, pin, floor, inventory, required/advisory status or comparison authority changed. Regression fault behavior was demonstrated against the real pre-fix consumers, not a simulated signal or an internal adapter probe. No generated media or logs are committed.

Prior hosted CI 40095781311 and Nightly 40095782965 accepted pre-fix source `fae8e8f`; they do not cover these fixes. Final-source hosted refresh and phase re-review remain with the orchestrator. No phase-complete tracking or additional GSD step was performed. This report is intentionally uncommitted for the orchestrator.

---

_Fixed: 2026-10-24T13:59:00Z_
_Fixer: the agent (gsd-code-fixer)_
_Iteration: 1_
