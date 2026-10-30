---
phase: 08-controllers-at-the-pins
plan: "04"
subsystem: runner
tags: [movie2, replay, controllers, bounded-memory, tdd]
requires:
  - phase: 08-03
    provides: Extended input selection and timed driving diagnostic
provides:
  - Complete typed movie2 grammar with immutable validation and sequential decoding
  - Actual driving replay and pre-effect refusal checks with unchanged movie1 behavior
  - Measured byte/frame bounds, allocation refusal cleanup and exact test inventories
affects: [08-05, 08-06, 08-07, 08-08, 08-09, 08-10, 08-11, 08-12, 08-14]
tech-stack:
  added: []
  patterns: [Immutable owned bytes, fixed cursor and last packet, actual runner fault seams]
key-files:
  created: []
  modified:
    - runner/movie.h
    - runner/movie.c
    - runner/main.c
    - tests/runner/test_movie.c
    - tests/runner/test_runner.c
    - tests/runner/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/runner.md
    - docs/testing.md
key-decisions:
  - Preserve movie1's parser entry point, 24-byte frame array, legacy setter and EOF path independently.
  - Validate movie2 before media/save/output work and retain immutable bytes plus one fixed cursor.
  - Accept complete typed syntax while retaining core capability refusals until each physical device lands.
requirements-completed: []
requirements-contributed: [INPUT-01]
coverage:
  - id: typed-grammar
    description: All declared typed payloads enforce canonical integers, ranges, masks, tags and the exact 83-byte frame bound.
    requirement: INPUT-01
    verification:
      - kind: unit
        ref: runner.movie_parse
        status: pass
      - kind: integration
        ref: runner.movie.v2_edges
        status: pass
    human_judgment: false
  - id: actual-driving-replay
    description: The shipped runner reproduces all 19 independently derived driving diagnostic pin records.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: runner.movie.replay
        status: pass
    human_judgment: false
  - id: immutable-pre-effect-refusal
    description: Late malformed rows, identity mismatches and CLI conflicts preserve existing outputs and saves and create neither when absent.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: runner.movie.v2_edges
        status: pass
      - kind: integration
        ref: runner.movie.owner immutable-file replacement and allocation refusal cases
        status: pass
    human_judgment: false
  - id: startup-and-eof
    description: Initial modern Auto resolves before the short first frame and EOF releases momentary state without repeating motion or losing absolute state.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: runner.movie.owner and runner.movie.v2_edges
        status: pass
      - kind: unit
        ref: runner.movie_parse typed absolute-state retention
        status: pass
    human_judgment: false
  - id: bounded-storage-and-legacy
    description: Exact 32 MiB and million-frame files retain one bounded byte buffer; one-over inputs refuse and movie1 regressions remain green.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: runner.movie.owner and cmake --workflow --preset ci
        status: pass
    human_judgment: false
duration: 14min
started: 2026-10-24T20:21:26Z
completed: 2026-10-24
status: complete
actuals:
  tokens: 15276
  tasks: 2
  commits: 3
plan_head_before: 288f727f165d42d8b1604804c27f534966619f6e
plan_head_after: a32abb08980278d2023e072ed94f404ad5e0fa2e
---

# Phase 8 Plan 4: Compact Typed Runner Replay Summary

**Movie2 validates immutable bytes before effects, decodes one complete typed packet per frame and reproduces the owned driving diagnostic while preserving movie1.**

## Accomplishments

The complete fixed-device grammar covers every phase device, including legacy letter fields, empty storage payloads, native bounded gun aim/flags, absolute pots, signed int32 motion, keypad and adapter masks. Decimal parsing rejects plus signs, whitespace, leading zeros, negative zero, overflow, invalid ranges/masks and missing/extra fields at deterministic 1-based positions. Two maximum 37-byte Amiga fields, four delimiters and five console positions derive the 83-byte bound. Headers retain their 64-byte and exact hash-key limits; total bytes, header lines and frame counts are unchanged.

The runner dispatches version 2 explicitly. Its new pure validator traverses the entire file before load/import/output, then the runner owns those bytes until cleanup. One cursor stores its offset, frame/line and last absolute packet; there is no frame array or index. Replacing the external file after the first accepted packet does not alter subsequent replay. Version 1 still refuses version 2 through its old parser entry point and retains its allocation, legacy input and EOF path. Non-movie sessions submit neutral modern Auto before frame execution. EOF zeros motion and releases buttons, keys and momentary console switches while retaining pots, aim/off-screen state and difficulty.

All CLI device names are recognized. Syntactically valid future devices still reach the core's explicit unsupported-controller refusal; this plan adds no physical models. The capability expectations in the actual-child test must advance with the corresponding device slices. INPUT-01 remains incomplete pending the rest of the phase and libretro integration.

## Task Commits

1. Task 1 RED — `2c425a6`: require typed replay through the actual runner.
2. Task 1 GREEN — `631b00e`: implement immutable typed replay, complete grammar, driving pin comparison and user docs.
3. Task 2 — `a32abb0`: actual child effects/refusals, allocation and startup seams, resource measurements, exact inventories and testing docs.

The three production/test commits above are measured from the persisted plan ledger. Actual tokens are ceil(61,103 realized diff characters / 4); documentation closeout commits are outside the recorded source interval.

## Verification Evidence

Every command below is local; run URL: N/A-local. No logs are committed.

- `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^runner[.]movie[.]replay$' --no-tests=error --output-on-failure --output-junit movie-red.xml` — exit 8 before implementation. The child executed and refused version 2 at 1:15 rather than producing the required frame evidence; its input fixture passed. `check tdd-red-evidence build/ci/movie-red-evidence.json --raw` — exit 0, RED_EVIDENCE_OK. This is the intended missing-grammar assertion, not a build/fixture failure.
- `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^runner[.]movie' --no-tests=error --output-on-failure` — exit 0, 17 fixture-inclusive cases after Task 1; final focused execution passes 19 after both new registrations. The unchanged movie1 version-refusal vector remains present.
- `ctest --preset ci -R '^runner[.]movie[.]' --no-tests=error --output-on-failure` — exit 0, 16 fixture-inclusive cases at the post-commit tracer gate, including the actual driving mailbox's 19 pin records.
- `cmake --workflow --preset ci` — Task 1 exit 0, 643 ordinary plus 6 release tests; final Task 2 exit 0, 645 ordinary plus 6 release tests, including package/floor/firmware checks and exact native inventory.
- Task 2's tests-only extension was additionally run against a real runner rebuilt from prefeature commit `2c425a6`, using the current core and the original runner sources. `ctest --test-dir build/ci/movie-before -R '^runner[.]movie[.]v2_edges$' --no-tests=error --output-on-failure --output-junit red.xml` — exit 8: the late-row numeric-diagnostic assertion fails because that consumer lacks movie2. `check tdd-red-evidence build/ci/movie-before/red-evidence.json --raw` — exit 0, RED_EVIDENCE_OK. The same suite against the implemented runner exits 0. No additional production expansion was needed in Task 2.
- `ctest --preset ci -R '^docs[.]budgets$' --output-on-failure` — exit 0; testing.md is exactly 40,960 bytes. Both new portable cases are also recorded in the WASM inventory.
- `git diff --check` and `check evaluation-scope --plan 08-04 --commits-only --raw` — exit 0; all three source commits resolve on this branch.

The child suite checks generated real inputs for every payload family, tag equality/unknown tags, signed extrema, masks/ranges and canonical faults. A malformed final row cannot even open a deliberately missing cartridge; actual requested images and HSC saves remain unchanged/absent. Matching/mismatched cartridge identity, behavior warnings, empty movies, CLI agreement/conflict and EOF versus explicit release traverse the shipped runner. The owner seam checks first-frame Auto/load override, immutable ownership, once-only deltas and every allocation failure with complete cleanup and old save bytes retained.

## Resource Measurements

Command: `python3 -c 'import subprocess,resource,time; start=time.monotonic_ns(); p=subprocess.run(["build/ci/tests/muhtari-test-runner","movie","build/ci/tests/movie-owner","build/ci/tests/input-carts/input.a78"],text=True,capture_output=True); print(p.stdout,end=""); print("exit",p.returncode,"wall_ns",time.monotonic_ns()-start,"peak_rss_bytes",resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss); raise SystemExit(p.returncode)'` — exit 0; run URL N/A-local.

| Construction | Bytes | Frames | Allocation calls | Retained/peak requested bytes | Elapsed ns |
|---|---:|---:|---:|---:|---:|
| Exact byte ceiling, maximum canonical rows | 33554432 | 399456 | 15 | 33554433 | 63096000 |
| Exact frame ceiling, short rows plus full cursor decode | 20000038 | 1000000 | 14 | 33554432 | 81081000 |

The native cursor is 104 bytes and the public packet 88 bytes. File storage grows geometrically only to the established byte ceiling plus its one-over detection byte. Parsing and decoding allocate no packets. Byte-ceiling plus one and frame-ceiling plus one refuse; growth allocation failures retain zero live bytes. Whole test-process wall time is 241,114,500 ns and peak RSS 169,902,080 bytes. RSS includes core startup, repeated allocation fault runs, file construction and allocator caching/transient storage; the requested-byte accounting isolates replay ownership. These are native desktop observations, with no invented speed threshold or low-power/cross-host claim.

## Deviations from Plan

- Task 1 also changed the existing `tests/runner/tests.cmake` owner to execute its required actual-runner tracer immediately; Task 2 subsequently added the two exact inventory entries there. No new source path, dependency or parser package was introduced.
- The existing invalid-port control used `paddle`, which is now a declared CLI value. It now uses unknown `mouse`, preserving the intended usage-refusal check.

## Issues Encountered

The first full Task 1 workflow exposed the stale paddle refusal control and passed after its correction. Task 2's expanded testing prose exceeded the existing documentation budget; it was reduced to the two new test names and invocation, with full grammar in runner.md and measurement evidence here. Final full CI passes after those corrections. The initial scratch baseline link lacked the existing host/stop objects; only the corrected executable's assertion failure was accepted as RED evidence. An attempted `/usr/bin/time -l` probe could not read a sandboxed sysctl; the successful standard-library subprocess/resource measurement above replaced it.

An extra `cmake --preset wasm32` probe exits 1 because this host lacks `clang-21`; no WASM build/test pass is claimed. Both inventory entries are updated, and real cross-host/WASM acceptance remains with the later acceptance plans. All automated verification commands required by this plan ran successfully. No package installation, review-fix round, gap round or owner verification was needed.

The optional `query verify-summary` scanner reports a backticked CTest command as a missing durable file and a failed self-check. This repeats the prior plan's command-string scanner limitation: the actual ten changed paths and three source ancestors were checked successfully, evaluation scope resolves them, and the explicit Self-Check section below passes. No implementation or refusal predicate was altered to silence that scanner warning.

## Known Stubs

None. Future physical-device refusals are intentional phase sequencing, not claimed device implementations.

## Next Phase Readiness

Four of fourteen phase plans are complete. Next separate command: `$gsd-execute-phase 8` for 08-05's real host equality/package gate. All INPUT requirements remain pending. No other GSD step or shipping action was invoked.

## Self-Check: PASSED

All ten changed source/test/doc/inventory paths and this summary exist. All three recorded source commits are ancestors of HEAD. Evaluation scope resolves this plan's actual commits, final full CI passes 645+6, and the tree has no uncommitted production changes.
