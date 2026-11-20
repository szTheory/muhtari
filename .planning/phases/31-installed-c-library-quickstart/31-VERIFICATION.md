---
phase: 31-installed-c-library-quickstart
verified: 2026-11-20T04:54:06Z
status: passed
score: 6/6 must-haves verified
covered_files:
  - .github/workflows/ci.yml
  - .planning/phases/31-installed-c-library-quickstart/31-01-PLAN.md
  - .planning/phases/31-installed-c-library-quickstart/31-01-SUMMARY.md
  - .planning/phases/31-installed-c-library-quickstart/31-02-PLAN.md
  - .planning/phases/31-installed-c-library-quickstart/31-02-SUMMARY.md
  - CMakeLists.txt
  - CMakePresets.json
  - README.md
  - cmake/muhtari.pc.in
  - cmake/muhtariConfig.cmake.in
  - docs/embedding-quickstart.md
  - docs/embedding.md
  - examples/embedding/CMakeLists.txt
  - examples/embedding/quickstart.c
  - include/muhtari.h
  - tests/cmake/expect_fail.cmake
  - tests/consumer/CMakeLists.txt
  - tests/consumer/main.c
  - tests/consumer/quickstart_faults.c
  - tests/install/tests.cmake
  - tests/release/tests.cmake
  - tests/release/ym_consumers.cmake
  - tests/tests.cmake
covered_digest: "v4:sha256:ae9b84da60f75d477c18a4431fd7735b5e1f6330a94f4f6b0eaf7a10ac0481a0"
behavior_unverified: 0
overrides_applied: 0
re_verification:
  previous_status: human_needed
  previous_score: 4/5
  gaps_closed:
    - "Quickstart setup and frame failures return an error without success output and release acquired resources."
  gaps_remaining: []
  regressions: []
decision_coverage:
  honored: 5
  total: 5
  not_honored: []
---

# Phase 31: Installed C library quickstart Verification Report

**Phase Goal:** A C host integrator can build and run a complete minimal example against an installed Muhtari package with the required resources and a known result.
**Verified:** 2026-11-20T04:54:06Z
**Status:** passed
**Re-verification:** Yes — after the failure-path evidence was added

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | One small C example creates an instance, supplies the required allocator and frame buffers, runs a frame, and checks a known no-media result. | VERIFIED | [quickstart.c](examples/embedding/quickstart.c:21) initializes size-tagged public structs, provides allocator callbacks, sizes buffers from muhtari_get_info, checks frame status and exactly 800 samples, and only then prints success. Both extracted-archive routes executed it successfully. |
| 2 | Create, partial buffer allocation, real frame rejection, and wrong-sample-count failures return unsuccessfully with the expected diagnostic and no example success line. | VERIFIED | [quickstart_faults.c](tests/consumer/quickstart_faults.c:39) includes the exact published source with scoped fault wrappers. Its frame mode sends non-null buffers and zero audio capacity to the real muhtari_run_frame, requiring MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL; sample-count mode changes the count only after a successful frame. [tests/install/tests.cmake](tests/install/tests.cmake:32) requires exit 1, exactly the cleanup marker on stdout, and the exact mode-specific diagnostic on stderr, so the example success line cannot appear. |
| 3 | Every pointer acquired by the example is released exactly once on each exercised failure path. | VERIFIED | The allocator tracker records every non-null allocation, flags unknown or duplicate releases, and requires every recorded pointer to be released once. The scratch omitted-free(audio) control made the existing CMake observer fail with “audio allocation was not released”; the control exited 1 as expected. |
| 4 | A clean consumer builds and runs the same source against an extracted release archive through both documented CMake and pkg-config routes. | VERIFIED | [ym_consumers.cmake](tests/release/ym_consumers.cmake:84) configures, builds and runs the standalone consumer from the extracted archive. Lines 138–173 resolve flags only from that archive, compile the same C source and run it. Both local routes produced “quickstart: one frame, 800 samples”. |
| 5 | Staged installed-consumer checks exercise the example and fault evidence, while a copied package missing its exported target fails for the expected reason. | VERIFIED | [tests/consumer/CMakeLists.txt](tests/consumer/CMakeLists.txt:16) links the shared source to the imported target and builds the fault driver for staged checks. [tests/install/tests.cmake](tests/install/tests.cmake:86) registers both existing consumer paths and requires all four fault cases. The extracted archive control removes only muhtariTargets.cmake from a copy and requires the matching include diagnostic. |
| 6 | The getting-started path links the full API guide and states package/platform requirements and the result's limits. | VERIFIED | [README.md](README.md:86) links the quickstart. [embedding-quickstart.md](docs/embedding-quickstart.md:3) links the exact source, standalone CMake project and complete API guide; it documents CMake/C17, POSIX pkg-config and Windows CMake requirements, the 800-sample result, and the no-game-compatibility limit. |

**Score:** 6/6 truths verified (0 present, behavior-unverified)

### Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| examples/embedding/quickstart.c | Complete installed public-API example | VERIFIED | Substantive C17 source; exercised against the staged package and extracted archive. |
| examples/embedding/CMakeLists.txt | Standalone installed-package consumer | VERIFIED | Finds and links the exported muhtari::muhtari target; archive test configures and builds it. |
| docs/embedding-quickstart.md | Package instructions, expected result and limitations | VERIFIED | Gives both routes, platform/tool requirements and source/API links. |
| tests/consumer/quickstart_faults.c | Test-only fault driver with per-pointer release checks | VERIFIED | Exercises four failures through the exact example and real installed API. |
| tests/consumer/CMakeLists.txt | Staged installed-target wiring | VERIFIED | Builds the example and optional fault driver against the staged imported target. |
| tests/install/tests.cmake | Staged consumer and failure assertions | VERIFIED | Checks exact child status, stdout, stderr and all four fault modes. |
| tests/release/ym_consumers.cmake | Extracted archive routes and target-export negative control | VERIFIED | Builds and runs both archive routes and validates the damaged-target control. |
| .github/workflows/ci.yml | Native package lanes and scoped pkg-config route | VERIFIED | Six native lanes remain; pkg-config opt-in is scoped to Ubuntu 26.04 x64. Hosted execution is not claimed here. |
| README.md, docs/embedding.md, package metadata and CMake registration | Entry points, complete reference and installed package wiring | VERIFIED | Links and package configuration trace to the standalone consumer. |

### Key Link Verification

| From | To | Via | Status | Details |
|---|---|---|---|---|
| README.md | docs/embedding-quickstart.md | C host entry point | WIRED | README links the quickstart and its source/API references. |
| examples/embedding/CMakeLists.txt | tests/release/ym_consumers.cmake | Extracted archive consumer configures the standalone project | WIRED (manual trace) | The direction is indirect: the archive script invokes the example CMake project with the extracted prefix. |
| tests/consumer/CMakeLists.txt | examples/embedding/quickstart.c | Staged package consumer compiles the documented source | WIRED | The executable source is the shared quickstart, linked to the installed target. |
| tests/install/tests.cmake | tests/consumer/CMakeLists.txt | Staged test builds consumer and fault targets | WIRED (manual trace) | ctest --build-and-test points to ../consumer, enables the fault target, then invokes both executables. |
| .github/workflows/ci.yml | tests/release/ym_consumers.cmake | Package test and scoped pkg-config opt-in | WIRED (manual trace) | The workflow runs the CI preset with the opt-in; CTest registers the release consumer script. |
| tests/consumer/quickstart_faults.c | examples/embedding/quickstart.c | Scoped macro substitutions around included source | WIRED | The driver includes the exact example source and substitutes only allocator/create/frame calls. |
| tests/release/ym_consumers.cmake | tests/cmake/expect_fail.cmake | Missing exported target negative control | WIRED | The isolated copied-prefix configure is checked for exit 1 and the expected missing-target diagnostic. |

The static key-link query reported direct-reference misses for indirect CMake/workflow links. Source and test-registration inspection confirmed each connection above; none is orphaned.

### Data-Flow Trace (Level 4)

| Artifact | Data variable | Source | Produces real data | Status |
|---|---|---|---|---|
| quickstart.c | frame dimensions and audio capacity | muhtari_get_info() from the linked staged or extracted installed library | Yes | FLOWING |
| quickstart.c | frame.audio_count | muhtari_run_frame() from the linked installed library | Yes; the count is checked against 800 before success output | FLOWING |

### Behavioral Spot-Checks

| Behavior | Command | Result | Status |
|---|---|---|---|
| Fault modes and normal quickstart in both staged consumer paths | ctest --preset ci --no-tests=error --output-on-failure -R '^(install\.consumer(\.dll_crt)?)$' | Exit 0; staged fixture plus both consumer paths passed, including create, partial audio-buffer allocation, real buffer-too-small frame call, wrong sample count, exact diagnostics and cleanup assertions. | PASS |
| Extracted archive CMake and pkg-config routes plus broken-target control | MUHTARI_RELEASE_QUICKSTART_PKG_CONFIG=1 ctest --preset ci-release --no-tests=error --verbose -R '^release\.ym_consumers$' | Exit 0; extracted CMake and pkg-config consumers printed the expected result, and the damaged target control matched its expected failure. | PASS |
| Deliberately omitted free(audio) sensitivity control | cmake -DQUICKSTART_PROGRAM="$PWD/build/ci/consumer/quickstart" -DQUICKSTART_FAULTS_PROGRAM="$PWD/build/ci/quickstart-fault-control/build/quickstart_faults" -DEMULATOR= -P tests/install/tests.cmake | Expected exit 1; the observer rejected the scratch copy with “audio allocation was not released” and emitted no staged success marker. | PASS (fault detected) |
| Full local regression workflow | MUHTARI_RELEASE_QUICKSTART_PKG_CONFIG=1 cmake --workflow --preset ci | Exit 0; 736 CI tests and 8 release tests passed and local ZIP packages were generated. This is local evidence; no hosted CI receipt is claimed. | PASS (local evidence) |

### Probe Execution

N/A — the phase plans and success criteria do not declare probe scripts or probe-based verification.

### Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| EMBED-01 | 31-01-PLAN.md, 31-02-PLAN.md | A C host integrator can build and run a complete minimal example against the installed library package, with required allocator/buffers, one frame and expected result explained. | SATISFIED | The shared source ran from both extracted archive routes; staged consumers exercise successful and fault paths; the guide covers setup, package selection and limits. |

### Decision Coverage

All trackable CONTEXT.md decisions are honored by shipped artifacts (5/5).

### Test Quality Audit

| Test File | Linked Req | Active | Skipped | Circular | Assertion Level | Verdict |
|---|---|---:|---:|---:|---|---|
| tests/install/tests.cmake | EMBED-01 | Yes | No | No | Behavioral: exact result, error status, diagnostics, stdout and fault cleanup | PASS |
| tests/release/ym_consumers.cmake | EMBED-01 | Yes | No | No | Behavioral: extracted archive CMake/pkg-config builds and runs plus strict negative control | PASS |
| tests/consumer/quickstart_faults.c | EMBED-01 | Yes | No | No | Behavioral: actual example fault paths, acquisition ordering and per-pointer release checks | PASS |

**Disabled tests on requirement:** 0. **Circular patterns detected:** 0. **Insufficient assertions:** 0.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| None | — | No debt markers, placeholders, empty implementations or console-only implementations found in phase files. NULL matches are ordinary initialization, checks or fault-driver return values. | — | No blocker. |

### Human Verification Required

N/A — all phase criteria are tied to reproducible package builds, output, diagnostics and cleanup behavior. The phase has no visual interface or external service behavior requiring human judgment.

### Gaps Summary

No remaining gaps. The previous human-needed item is closed by four staged fault modes that exercise the actual quickstart source, exact failure output, a real MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL frame call and one-time release of each acquired pointer. The scratch omitted-free control confirms the observer detects a specific missing release. Staged and archive checks plus the full local workflow passed. Hosted CI was not run or claimed.

---

_Verified: 2026-11-20T04:54:06Z_
_Verifier: the agent (gsd-verifier)_
