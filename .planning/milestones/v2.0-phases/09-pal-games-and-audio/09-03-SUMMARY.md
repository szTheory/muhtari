---
phase: 09-pal-games-and-audio
plan: "03"
subsystem: runner
tags: [pal, palette, movies, metadata, png]
requires:
  - phase: 09-02
    provides: Safe selected-region execution, queried storage and PAL peripheral geometry
provides:
  - Approved warm PAL RGB and RGB565 presentation with unchanged legacy NTSC helpers
  - Explicit runner PAL selection and opt-in effective metadata
  - Immutable PAL movie replay with complete effective-height validation before effects
affects: [09-04, 09-07, 09-08]
tech-stack:
  added: []
  patterns:
    - Keep native identity separate from region-selected host presentation
    - Validate region-dependent movie semantics with a copied cursor after successful load
key-files:
  created: []
  modified:
    - host/palette.c
    - host/palette.h
    - tests/host/test_palette.c
    - PROVENANCE.md
    - THIRD-PARTY-NOTICES.md
    - runner/main.c
    - runner/movie.c
    - runner/movie.h
    - tests/runner/tests.cmake
    - tests/runner/test_movie.c
    - tests/runner/test_runner.c
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/runner.md
key-decisions:
  - Implement the eight explicitly specified info keys; the plan's reference to nine fields was a count typo.
  - Resolve semantic movie height from successful-load info while preserving immutable syntax validation before media access.
  - Retain exact line and Y-column diagnostics for either gun port.
  - Keep actual local pre-plan package measurements distinct from hosted or phase-versus-main acceptance.
requirements-completed: []
coverage:
  - id: selected-warm-palette
    description: All256 approved PAL RGB entries, independent RGB565 channel truncations and legacy NTSC parity
    requirement: PAL-02
    verification:
      - kind: unit
        ref: host.palette; policy.notices
        status: pass
    human_judgment: false
  - id: actual-runner-pal
    description: Automatic and explicit PAL preserve full93440-byte native extent, selected PNG pixels and raw/PCM identity
    requirement: PAL-01
    verification:
      - kind: integration
        ref: runner.load.pal; runner.region.pal; runner.dump_png; runner.png
        status: pass
    human_judgment: false
  - id: effective-metadata
    description: Opt-in region, dimensions, exact FPS/PAR and palette describe the session while old output remains unchanged
    requirement: PAL-02
    verification:
      - kind: integration
        ref: runner.region.pal; runner.probe.accepted; runner.probe.sample_a78
        status: pass
    human_judgment: false
  - id: immutable-regional-movies
    description: Both movie versions accept PAL and preserve CLI conflicts, row boundaries, exact diagnostics and before-effects failure
    requirement: PAL-01
    verification:
      - kind: integration
        ref: runner.movie_parse; runner.movie.v2_edges; runner.movie.owner
        status: pass
    human_judgment: false
  - id: runner-storage-lifecycle
    description: Queried PAL buffers,960/962/961 startup sample sequence, every allocation failure and unchanged replay limits
    requirement: PAL-03
    verification:
      - kind: integration
        ref: runner.movie.owner; runner.movie_parse; runner.load.pal
        status: pass
    human_judgment: false
  - id: local-product-and-packages
    description: Final source and genuine pre-plan base run full workflows and existing authenticated three-ZIP producers
    requirement: PAL-03
    verification:
      - kind: other
        ref: cmake --workflow --preset ci; tests/cmake/quality_evidence.cmake
        status: pass
    human_judgment: false
actuals:
  tokens: 13512
  tasks: 3
  commits: 7
plan_head_before: ba8c22e7b3403bb68e0e6baa62e2a4553bfa3fc9
plan_head_after: ef95e3727b1b08b34a83ba85884beca105b03a8b
duration: 16min
completed: 2026-10-25
status: complete
---

# Phase 9 Plan 3: PAL runner presentation and movie replay Summary

The real runner selects warm PAL presentation, reports effective session metadata on request and replays PAL movies with complete regional coordinate validation before outputs or saves can change.

## Accomplishments

- Two private region-aware palette helpers select the approved warm PAL table. All256 independently transcribed expected RGB literals and each RGB565 channel truncation pass. Existing helper signatures and explicit NTSC selection retain the historical table. RGB remains outside native hashing and light sensing. The approved BSD-3 file at full pin7a2afdc1ea08fc331b16b750d8c1f02d4ef62fc8 was fetched and its header checked; SHA-256 remains504b221d6dd1726cabd600b795a0243a6353d4642827f7d1bc438ea1d8ce31c2. Notices cover both implementation tables and the transcribed test oracle.
- `--region auto|ntsc|pal` follows existing load precedence. `--info` prints region,width,height,fps_num,fps_den,par_num,par_den,palette before frames, after successful load/validation/allocation. PAL reports320,292,3546894/71051,1843750/1773447,warm_pal; NTSC retains39375000/656711,6/7,warm_ntsc. No-content remains NTSC. Duplicate info and unknown region tokens are usage errors. Historical ordinary/probe output and the pinned NTSC PNG remain unchanged.
- The actual binary checks automatic/explicit PAL native/raw/PCM equality and first/full audio counts. An owned generated solid-background program independently proves all93440 native bytes, including row291, and the complete280700-byte PNG of literal RGBB0,57,46. Its expected PNG digest was separately constructed from the PNG specification, literal pixels, stored-block framing, CRC32 and Adler32; it differs from NTSC's AF,51,63 presentation.
- Movie1 and movie2 accept `region pal`; explicit CLI conflicts remain errors before media access. Syntax accepts gun Y through291 and rejects292. After successful load resolves region, a copied cursor checks the complete immutable gun session against queried height before output-buffer allocation, save import, info/hash output or frame execution. Tests cover NTSC241/242, PAL291/292, automatic PAL, reverse override and exact invalid-Y columns on either port. Existing and absent saves/PNGs remain unchanged on a late invalid row. Disk mutation after validation cannot alter replay; EOF retains aim and releases momentary controls.
- Actual runner ownership tests prove exactly93440-byte video and1924-byte audio allocations, PAL startup counts960/962/961, cleanup at every allocation failure, no frame/input/output allocations on invalid effective height, and existing32MiB/1000000-frame parsing bounds. No new runtime dependency, format, public ABI or core scheduler is introduced.

## Task Commits

| Task | RED | GREEN and follow-up |
|---|---|---|
| 1 — Approved warm PAL host palette | 2de3560 | f8ab973 |
| 2 — Explicit PAL runner and info | eea6a8e | 610abbe |
| 3 — PAL movie sessions and before-effects checks | 8f996b6 | 3f48235; ef95e37 preserves exact error columns |

Seven task commits are measured from the persisted pre-plan ledger, before this summary commit. Actual tokens are13512 characters/4 rounded up over the realized diff, not harness token usage. The observed16min window begins at the first RED test17:14:43UTC and excludes initial reading.

## Verification

All commands below exited0 unless explicitly identified as RED or a retained failure. Run URL for every command: N/A-local.

- Task1: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(host[.]palette|policy[.]notices|policy[.]assets|docs[.])' --no-tests=error --output-on-failure` —5/5.
- Task2: `ctest --preset ci -R '^(runner[.]load[.]|runner[.]region[.]|runner[.]dump_png|runner[.]png|runner[.]probe|host[.]palette|docs[.])' --no-tests=error --output-on-failure` —23/23 after the build.
- Task3: `ctest --preset ci -R '^(runner[.]movie_parse|runner[.]movie[.]|host[.]palette|policy[.]notices)' --no-tests=error --output-on-failure` —22/22. The exact-column follow-up ran5/5 targeted parser/replay/ownership checks.
- Final sourceef95e37: `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake --workflow --preset ci` —652 ordinary tests in49.45s,6 release checks in0.26s; all pass. Earlier source3f48235 also passed652+6 in30.37s/0.10s. No hardware or hosted acceptance is claimed.
- `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` —6/6 on3f48235; the complete final-source workflow also regenerates all three packages and passes all six release checks.
- `scripts/hygiene.sh --tree`, `git diff --check`, and clang-format21 `--dry-run --Werror` over all changed C/header files —exit0. `check evaluation-scope --plan 09-03 --commits-only --raw` resolves the task commits on this branch. No known stubs or new external trust surface remain.

## Local Package Measurements and Retained Failure

`env GITHUB_EVENT_PATH="$PWD/build/ci/quality-runner-local-event.json" cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-runner-local.json -DSHA=3f48235ae67dd6f334d60f1e27102136f7977c53 -DCONFIG=ci -DSOURCE=N/A-local -DBUILD_BASE=ON -P tests/cmake/quality_evidence.cmake` exited1 on its first baseline attempt. Candidate evidence was produced, but baseba8c22e had651/652 passing tests and `input.lightgun` timed out at2.06s under its existing2s deadline; packaging was not reached. This failed attempt is not a pass or a proven flake.

The unchanged base test passed alone in0.62s. That supported a finite transient scheduling-contention hypothesis. One bounded unchanged-base retry of `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake --workflow --preset ci` exited0,652+6,37.42s/0.21s. No source, workload, skip or deadline changed. The existing current producer was then invoked against each completed root separately:

`env GITHUB_EVENT_PATH="$PWD/build/ci/quality-runner-local-event.json" cmake -DROOT=build/ci/quality-base -DBUILD=build/ci/quality-base/build/ci -DOUT="$PWD/build/ci/quality-runner-base-local.json" -DSHA=ba8c22e7b3403bb68e0e6baa62e2a4553bfa3fc9 -DCONFIG=ci -DSOURCE=N/A-local -P tests/cmake/quality_evidence.cmake` —exit0.

`env GITHUB_EVENT_PATH="$PWD/build/ci/quality-runner-local-event.json" cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-runner-local.json -DSHA=ef95e3727b1b08b34a83ba85884beca105b03a8b -DCONFIG=ci -DSOURCE=N/A-local -P tests/cmake/quality_evidence.cmake` —exit0.

The local event descriptor's `before` is the genuine pre-plan ledger SHA, and its scope explicitly says local slice. Both producer reports authenticate their exact HEAD, complete652+6 inventories, manifests/sample identities and actual archives. A read-only report check verifies equal toolchain/fixture environment and archive sums.

| Source | Library | Libretro | Runner | Total bytes |
|---|---:|---:|---:|---:|
| Currentef95e37 | 81760 | 87073 | 96375 | 265208 |
| Pre-planba8c22e | 81692 | 86211 | 94774 | 262677 |

These report-only measurements establish a local slice comparison, not hosted, six-native, joined-lane or phase-versus-main acceptance. No quota or authority changes. The known detached baseline worktree was removed after checking exact HEAD and clean tracked state. Reports/logs stay in ignored build output; none is committed.

## TDD Gate Compliance

Every task has RED before GREEN. Each named target executed, failed the planned assertion and received `RED_EVIDENCE_OK` from `check tdd-red-evidence` using its unchanged complete CTest JUnit report and actual exit8. Evidence records are ignored build artifacts. Task1 exposed the existing sole NTSC lookup returning271500 instead of PAL0F2200; GREEN binds the unchanged PAL table expectations to the new selected helper while retaining the legacy NTSC assertions. Task2's actual subprocess lacked the planned info flag. Task3's actual subprocess rejected the planned PAL movie header. Semantic review distinguished these expected behavior failures from compile, collection or fixture errors. No separate refactor commit was necessary.

## Deviations and Failed Experiments

- The plan said nine info fields but explicitly named eight; those exact eight are implemented and documented.
- A CMake binary-fixture splice inserted an extra newline at a header boundary. The existing C runner test executable now makes the private fixture copy with checked binary I/O. No fixture bytes or new helper dependency enter the repository.
- Initial test expectations assumed two lagged frames for seed programs that poll on their second call, and962 samples on the third PAL call. The existing core was correct; expectations now retain the actual polling semantics and independently selected960/962/961 lattice. No production lag/audio behavior changed.
- Final source review found regional errors needed the existing exact line/column contract. The narrow follow-up supplies the offending Y column on either port, with actual-process assertions. This is task correctness work, not a formal phase code-review round.
- The full PAL expected table also needs its seed notice. Its test-file SPDX/holders and existing notice scope were completed with Task3. The stale internal movie-header region comment was corrected in runner/movie.h, the only additional file beyond the declared plan list.
- One broad patch initially placed the info block in the wrong function; the compiler rejected it. It was moved into the successful-load/output owner before any GREEN commit.

## Readiness

This slice is implemented and locally verified. PAL-01–03 remain pending the successor plans and final phase acceptance; no requirements checkbox or phase-completion claim is made here. No owner setup, consent or UAT is needed for this slice. Formal review-fix and gap rounds remain unused. Shared STATE/ROADMAP/REQUIREMENTS acceptance writes belong to the orchestrator.

## Self-Check: PASSED

All14 changed files exist. All seven task commits are ancestors of the current phase branch, with exact ledger baseba8c22e and final sourceef95e37. Final-source652+6, package integrity, focused behavioral checks, formatting and tree hygiene pass. The failure history and local-only measurement scope are retained explicitly.
