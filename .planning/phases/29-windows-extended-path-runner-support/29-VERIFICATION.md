---
phase: 29-windows-extended-path-runner-support
verified: 2026-11-19T15:58:13Z
status: passed
score: 9/9 must-haves verified
covered_files:
  - .github/workflows/ci.yml
  - .planning/phases/29-windows-extended-path-runner-support/29-01-PLAN.md
  - .planning/phases/29-windows-extended-path-runner-support/29-01-SUMMARY.md
  - .planning/phases/29-windows-extended-path-runner-support/29-02-PLAN.md
  - .planning/phases/29-windows-extended-path-runner-support/29-02-SUMMARY.md
  - .planning/phases/29-windows-extended-path-runner-support/29-03-PLAN.md
  - .planning/phases/29-windows-extended-path-runner-support/29-03-SUMMARY.md
  - docs/runner.md
  - runner/main.c
  - scripts/ubuntu-apt.sh
  - tests/policy/ubuntu_apt.sh
  - tests/runner/test_runner.c
  - tests/runner/test_save.c
  - tests/runner/trace.cmake
  - tests/runner/windows_paths.c
  - tests/runner/windows_paths.ps1
covered_digest: "v4:sha256:5b9b793f9376937388b01a03e0193127a457e2e1d3dbebc9f86b9436903c2d21"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 8
  total: 8
  not_honored: []
---

# Phase 29: Windows extended-path runner support — Verification

**Phase Goal:** Windows users can use explicit absolute extended paths throughout `muhtari-run` while retaining literal names, collision protection and safe output replacement.
**Verified:** 2026-11-19T15:58:13Z
**Status:** passed
**Re-verification:** No — initial verification

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | Windows process arguments preserve Unicode and literal extended-namespace spelling through every documented input role, including cartridge/probe, movie, BIOS, HSC firmware and state input. | ✓ VERIFIED | `wmain` converts with strict UTF-8 handling; filesystem calls convert to UTF-16 and use wide APIs. The real-process path matrix passed on Windows x64 and ARM64 in [CI](https://github.com/szTheory/muhtari/actions/runs/44543122949) and [Release](https://github.com/szTheory/muhtari/actions/runs/44543122965). Logs confirm Unicode and literal suffix cases in both drive and UNC namespaces. |
| 2 | Save-directory, state, PNG, trace and derived battery paths work in both namespaces, preserve the requested output, and replace supported existing destinations. | ✓ VERIFIED | The real runner produced and read back state; the matrix checked PNG signature, complete trace, battery bytes, successful replacement and state reload. x64 and ARM64 `runner.trace.paths` passed in CI and Release; see the four Windows jobs below. |
| 3 | Distinct suffix-preserving destinations succeed, while existing-file, hard-link, junction, mixed-form and input/output/output aliases are rejected before protected files change. | ✓ VERIFIED | `runner_invocation_path_conflicts` compares all inputs against outputs and all output pairs. Existing files use volume/file IDs; absent targets use parent identity and namespace-aware leaf comparison. The active Windows identity and junction cases pass, with prior bytes checked after refusal. |
| 4 | An unavailable required identity query fails closed before output mutation or emulation. | ✓ VERIFIED | The Windows `IDENTITY_QUERY` fault seam makes the file-ID query return access denied; the harness asserts failure, zero frames, no output and unchanged input bytes. Windows `runner.trace.paths` passed in both architectures. |
| 5 | Failed create, write, flush, sync, close or replacement preserves prior state, PNG, trace and battery bytes and removes only owned temporary files; successful publication remains atomic. | ✓ VERIFIED | `write_sibling_atomic_stream` uses an exclusive sibling temporary and replaces only after write/flush/sync/close. CI Windows `runner.save.faults`, `runner.state.files`, `runner.trace.io`, `runner.dump_png`, and `runner.png` all passed; the actual-process Windows matrix also verified blocked replacement, prior-byte preservation and temporary cleanup for every output class. |
| 6 | Malformed, relative, device and volume-GUID namespace forms fail clearly without changing an existing output. | ✓ VERIFIED | The real-process matrix runs invalid forms against a sentinel trace destination and checks the refusal diagnostic and unchanged bytes. The actual Windows path test passed on both architectures. |
| 7 | The existing Windows CI route executes the real runner on x64 and ARM64 through drive and owned SMB UNC paths, and does not accept missing share setup or cleanup as a pass. | ✓ VERIFIED | `runner.trace.paths` is registered in the existing Windows test inventory and invokes the PowerShell-owned share plus bounded `CreateProcessW` driver. CI and Release logs report both namespace measurements and “Windows SMB share lifecycle: create/read/write/remove verified”; all four Windows jobs completed their 736-test and 8-test release inventories. |
| 8 | User documentation states only the supported absolute forms, observed filesystem/component boundary and clear refusal behavior. | ✓ VERIFIED | `docs/runner.md` matches the successful observations: NTFS component limit 255; output lengths 500/492 UTF-16 units for drive/UNC; battery `.sav` lengths 489/481. The 518/510 temporary-name values are marked as calculated candidates, not guaranteed creations. |
| 9 | Cheap checks, local configured CI and internal review cover the current scheduling-only registration change without reducing test membership or timeout. | ✓ VERIFIED | `git diff --check 6c7e9c4..HEAD` and `scripts/hygiene.sh --tree` exited 0. The local JUnit records 736 tests, 0 failures, and 0 disabled; release JUnit records 8 tests, 0 failures. The current local CTest `runner.trace.paths` check passed 1/1 (portable control only). The focused review of the current `COST 50` change is clean. |

**Score:** 9/9 truths verified (0 present, behavior-unverified).

## Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `runner/main.c` | Unicode-preserving argument and wide filesystem boundary; invocation-wide identity and atomic outputs | ✓ VERIFIED | Substantive production code, called from the Windows `wmain` entry. Paths cross strict UTF-8/UTF-16 conversion; wide file, metadata, temporary and replacement APIs are used. CI Windows suites exercise the real executable. |
| `tests/runner/windows_paths.c` | Native process-level path, alias, replacement and measurement driver | ✓ VERIFIED | Substantive bounded driver; `CreateProcessW` launches the built `muhtari-run`, then independently checks files, identities, diagnostics and bytes. Wired into `runner.trace.paths`. |
| `tests/runner/windows_paths.ps1` | Owned SMB share setup, loopback I/O, runner invocation and verified cleanup | ✓ VERIFIED | Uses a unique share, refuses a name collision, runs drive and UNC cases, preserves primary failures, and fails if files/share remain. Called by the existing Windows CTest branch. |
| `tests/runner/trace.cmake` | Windows path acceptance registered in existing runner test | ✓ VERIFIED | `runner.trace.paths` invokes the harness and PowerShell route; test failures propagate. `COST 50` is Windows-only and leaves membership, fixtures and timeout unchanged. |
| `tests/runner/test_save.c` | Regression and fault controls at production filesystem boundaries | ✓ VERIFIED | Includes active path-identity, state/PNG/write-failure controls; `runner.trace.paths`, `runner.trace.io`, `runner.state.files`, and `runner.save.faults` pass in Windows CI. |
| `docs/runner.md` | Accurate supported Windows path contract | ✓ VERIFIED | Exists, is substantive and matches actual Windows measurements and refusal behavior. |
| `.planning/phases/29-windows-extended-path-runner-support/29-ACCEPTANCE.md` | Source-qualified evidence matrix and limits | ✓ VERIFIED | Records CI, Release and Nightly identities, commands, outcomes, run links, measured paths, prior failures and efficiency limits. Independently checked the c3 run/job metadata and Windows logs. |

## Key Link Verification

| From | To | Via | Status | Details |
|---|---|---|---|---|
| `runner/main.c` | Windows wide CRT and Win32 APIs | `wmain` → strict conversion → path wrappers | ✓ WIRED | Directly traced `wmain`, `_wfopen`, `_wopen`, `_wremove`, `MoveFileExW`, metadata queries and path conversion. |
| `runner/main.c` option validation | Every input and output role | `check_options` → `runner_invocation_path_conflicts` | ✓ WIRED | The preflight includes cartridge, movie, BIOS, firmware, state input, frame/state/trace outputs and the derived battery path; unresolved identity returns an error. |
| `runner/main.c` PNG output | `write_sibling_atomic_stream` | PNG stream writer | ✓ WIRED | PNG rows flow through the same exclusive temporary and checked publication path as other outputs. |
| `tests/runner/trace.cmake` | `tests/runner/windows_paths.ps1` | Windows `CASE paths` branch | ✓ WIRED | The helper runs after the ordinary/junction harness; nonzero status or missing drive/UNC/cleanup measurements fails CTest. |
| `tests/runner/windows_paths.ps1` | `tests/runner/windows_paths.c` | Driver calls for drive and UNC | ✓ WIRED | Share creation/read/write and cleanup are checked around both actual-process invocations. |
| `tests/runner/windows_paths.c` | `muhtari-run` | bounded `CreateProcessW` invocation | ✓ WIRED | The driver starts the built executable with Unicode path arguments, checks exit/diagnostics and inspects output files independently. |
| `.github/workflows/ci.yml` | `runner.trace.paths` | Existing Windows x64/ARM64 `ci-msvc` inventory | ✓ WIRED | The c3 CI and Release Windows job logs show the named test ran and passed, not merely that its registration exists. |

The generic `verify.key-links` heuristic returned false negatives for descriptive endpoints in plan frontmatter (it expects literal path-like target references). Manual source tracing above confirms the actual call sites and runtime wiring; no code link is missing.

## Data-Flow Trace (Level 4)

| Artifact | Data variable | Source | Produces real data | Status |
|---|---|---|---|---|
| CLI output files | state, PNG, trace and battery payloads | Emulator/state serializers and runner capture; written through the atomic sibling-file path | Yes | ✓ FLOWING |

This is a CLI/file-output feature; there are no rendered UI values or database query paths.

## Behavioral Spot-Checks

| Behavior | Command / evidence | Result | Status |
|---|---|---|---|
| Actual Windows drive and SMB UNC acceptance on x64 | CI `runner.trace.paths`; [job 135626560630](https://github.com/szTheory/muhtari/actions/runs/44543122949/job/135626560630); Release [job 135626571973](https://github.com/szTheory/muhtari/actions/runs/44543122965/job/135626571973) | Passed 8.68s / 5.25s; both namespaces and SMB cleanup reported. Full 736-test and 8-test release inventories passed. | ✓ PASS |
| Actual Windows drive and SMB UNC acceptance on ARM64 | CI [job 135626560798](https://github.com/szTheory/muhtari/actions/runs/44543122949/job/135626560798); Release [job 135626571891](https://github.com/szTheory/muhtari/actions/runs/44543122965/job/135626571891) | Passed 47.02s / 45.78s; both namespaces and SMB cleanup reported. Full 736-test and 8-test release inventories passed. | ✓ PASS |
| Current local portable path harness | `ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.]paths$'` | Exit 0; 1/1 in 0.02s. This does not prove Windows behavior. | ✓ PASS |
| Current local configured CI and packaging | Recorded `cmake --workflow --preset ci` on snapshot `83a1709`; local JUnit and package outputs | 736 tests, 0 failures; three macOS packages and 8/8 release tests. This does not substitute for the Windows runs above. | ✓ PASS |

The PR CI synthetic merge `ff5beaef5fde97d51a8858f485620669290495fe` has the same complete tree as candidate `be331d2f82666d3f5e6c1f1b307a8fb9f4aa0235`; base `f709d493e6660b3b53a98b49b693ab91a714dcb0` is an ancestor. The current branch adds a Windows-only `COST 50` scheduling property to `runner.trace.paths`; it does not change the implementation or test cases exercised by c3.

## Probe Execution

N/A — no probe-based verification script is declared or present. The runner’s `--probe` input role is exercised by the actual-process path matrix.

## Requirements Coverage

| Requirement | Source plan | Description | Status | Evidence |
|---|---|---|---|---|
| WINPATH-01 | 29-01, 29-03 | Every documented runner input accepts the supported drive and extended UNC path forms with Unicode, literal suffixes and tested long paths. | ✓ SATISFIED | Real runner matrix passed on x64/ARM64 in [CI](https://github.com/szTheory/muhtari/actions/runs/44543122949) and [Release](https://github.com/szTheory/muhtari/actions/runs/44543122965); both logs report both namespaces and successful output/readback cases. |
| WINPATH-02 | 29-02, 29-03 | Distinct literal names are accepted; same-target and overlapping paths are refused before writes. | ✓ SATISFIED | Code traces shared identity preflight across all roles; identity, hard-link, junction, mixed-form and suffix-distinct tests passed in the Windows inventories. |
| WINPATH-03 | 29-02, 29-03 | State, PNG, trace and battery output use atomic replacement and preserve previous bytes on failure. | ✓ SATISFIED | Active failure tests plus Windows process replacement/failure-preservation cases passed; the full Windows inventories include `runner.save.faults`, `runner.state.files`, `runner.trace.io`, `runner.dump_png` and `runner.png`. |
| WINPATH-04 | 29-01, 29-03 | Windows CI exercises the actual runner and accessible SMB UNC matrix; docs state measured boundaries. | ✓ SATISFIED | Both architectures passed the full path/share matrix with confirmed share cleanup; docs’ supported-form and path/component values match the logs. |

All four requirements mapped to Phase 29 are accounted for; no orphaned Phase 29 requirement was found.

### Decision Coverage

All trackable CONTEXT.md decisions are honored by shipped artifacts. The decision gate verified 8 of 8 decisions and found none unhonored.

### Test Quality Audit

| Test file / route | Linked requirements | Active | Skipped | Circular | Assertion level | Verdict |
|---|---|---:|---:|---:|---|---|
| `tests/runner/windows_paths.c`, `windows_paths.ps1`, `trace.cmake` | WINPATH-01 through WINPATH-04 | Yes | 0 | 0 | Behavioral / end-to-end | PASS |
| `tests/runner/test_save.c`, `test_runner.c` | WINPATH-02, WINPATH-03 | Yes | 0 | 0 | Value, identity and failure-preservation assertions | PASS |

The ordinary-path state file is used only as a differential control for path-form parity; fixed sentinel bytes, file identity, PNG/trace structure, battery size and state reload checks independently test the results. No requirement depends only on a test-generated expected value from the system under test.

**Disabled tests on requirements:** 0. **Circular patterns detected:** 0. **Insufficient assertions:** 0.

## Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| None | — | No debt markers, empty stubs, skipped requirement tests or placeholder behavior found in the changed implementation/test files. | — | None |

## CI Efficiency Review

| Observation | Evidence | Disposition |
|---|---|---|
| Queue delay and cancellations | c3 CI, Release and Nightly jobs began within seconds of dispatch; no c3 jobs were cancelled. | Queue delay is not the critical bottleneck. |
| Critical path | Windows ARM64 jobs took 45m02s in CI and 44m52s in Release; the path test itself took 47.02s and 45.78s. | Preserve the required inventories and independent-base comparison; these runs support no safe topology/cache/deadline reduction. |
| Duplicate equality work | Release and Nightly overlapped for 52m53s of runner time. A changed-file classifier would add alignment and maintenance risk and has no cross-PR evidence. | Record the runner-minute opportunity; no structural change is supported here. |
| Windows path scheduling | c3 ran the test late under default cost. Current `COST 50` is registered locally with the original timeout and fixtures. | The hosted ordering and feedback-time effect is not measured yet; claim no time saving. This is an efficiency hypothesis, not a product acceptance gap. |

## Human Verification Required

None. All phase success criteria are exercised by deterministic local or hosted checks; project rule 6 does not require blanket UAT.

## Gaps Summary

None. The product path behavior is verified on the exact c3 product tree in both Windows architectures. The only change after that run is Windows-only CTest scheduling metadata, which preserves membership, fixtures and timeout. Its hosted ordering effect remains explicitly unclaimed until another run.

---

_Verified: 2026-11-19T15:58:13Z_
_Verifier: the agent (gsd-verifier)_
