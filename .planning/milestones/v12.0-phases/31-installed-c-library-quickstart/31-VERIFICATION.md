---
phase: 31-installed-c-library-quickstart
verified: 2026-11-20T10:16:10Z
status: passed
score: 6/6 truths verified
covered_files:
  - .planning/phases/31-installed-c-library-quickstart/31-01-PLAN.md
  - .planning/phases/31-installed-c-library-quickstart/31-01-SUMMARY.md
  - .planning/phases/31-installed-c-library-quickstart/31-02-PLAN.md
  - .planning/phases/31-installed-c-library-quickstart/31-02-SUMMARY.md
  - tests/consumer/quickstart_faults.c
  - tests/emscripten/bundle.cmake
  - tests/policy/publish.cmake
covered_digest: "v4:sha256:d29c08dcc68d83afa2abc970a7a9885f00af65edaddf92bc0d59479077b7f5c9"
behavior_unverified: 0
overrides_applied: 0
re_verification:
  previous_status: passed
  previous_score: 6/6
  gaps_closed: []
  gaps_remaining: []
  regressions: []
decision_coverage:
  honored: 5
  total: 5
  not_honored: []
---

# Phase 31: Installed C library quickstart Verification Report

**Phase Goal:** A C host integrator can build and run a complete minimal example against an installed Muhtari package with the required resources and a known result.
**Verified:** 2026-11-20T10:16:10Z
**Status:** passed
**Re-verification:** No — the previous report had no gaps, so this refresh used initial goal-check mode after three covered source files changed.

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | One small C example creates an instance, supplies the required allocator and frame buffers, runs a frame, and checks a known no-media result. | VERIFIED | [quickstart.c](../../../examples/embedding/quickstart.c:21) initializes size-tagged public structs, supplies both allocator callbacks, sizes video and audio buffers from muhtari_get_info, checks muhtari_run_frame and exactly 800 samples before printing success. Current staged and extracted-archive runs produced the required line without media. |
| 2 | Create, partial buffer allocation, real frame rejection, and wrong-sample-count failures return unsuccessfully with the expected diagnostic and no example success line. | VERIFIED | [quickstart_faults.c](../../../tests/consumer/quickstart_faults.c:39) includes the exact published source with scoped fault wrappers. The frame mode reaches the real API with zero audio capacity and requires MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL; sample-count mode changes the count only after a successful frame. [tests/install/tests.cmake](../../../tests/install/tests.cmake:32) requires exit 1, only the cleanup marker on stdout, and the exact mode diagnostic on stderr. |
| 3 | Every pointer acquired by the example is released exactly once on each exercised failure path. | VERIFIED | The allocator tracker records every non-null allocation, flags unknown or duplicate releases, and requires every recorded pointer to be released once. The scratch omitted-free(audio) control made the existing CMake observer fail with “audio allocation was not released”; the control exited 1 as expected. |
| 4 | A clean consumer builds and runs the same source against an extracted release archive through CMake and pkg-config; the native matrix requires CMake everywhere and scopes pkg-config to Ubuntu 26.04 x64. | VERIFIED | [ym_consumers.cmake](../../../tests/release/ym_consumers.cmake:84) configures, builds and runs the standalone consumer from the extracted archive; lines 138–177 resolve flags only from that archive and compile the same source. The current Apple Silicon CTest passed both routes. [ci.yml](../../../.github/workflows/ci.yml:159) retains six native lanes and enables pkg-config only on ubuntu-26.04. |
| 5 | Staged installed-consumer checks exercise the example and four fault cases, while a copied package missing its exported target fails for the expected reason. | VERIFIED | [CMakeLists.txt](../../../tests/consumer/CMakeLists.txt:16) links the shared source to the imported target and builds the fault driver. [tests.cmake](../../../tests/install/tests.cmake:86) registers both CRT paths and requires all four exact fault responses. The extracted-archive control removes only muhtariTargets.cmake from a copy and the current CTest required its matching include diagnostic. |
| 6 | The getting-started path links the full API guide and states package/platform requirements and the result's limits. | VERIFIED | [README.md](../../../README.md:86) links the quickstart. [embedding-quickstart.md](../../../docs/embedding-quickstart.md:3) links the exact source, standalone CMake project and complete API guide; it documents the CMake 3.25/C17 requirements, POSIX pkg-config and Windows CMake setup, matching archive choice, 800-sample result, and no-game-compatibility limit. |

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
| tests/emscripten/bundle.cmake | Portable bundle capability and YM-receipt validation | VERIFIED | Changed matcher uses the stable selected-YM claim substring; bundle creation passed and direct production publisher controls passed. Its separate bundle-controls CTest is recorded as a local timeout below, not as a pass. |
| tests/policy/publish.cmake | Publisher fixture transports YM evidence for the package description | VERIFIED | Changed matcher is exercised by the publisher controls and by the direct production archive validation run in this refresh. |

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
| Staged quickstart and publisher controls | ctest --preset ci --parallel 1 --no-tests=error --output-on-failure -R '^(install[.]consumer|install[.]consumer[.]dll_crt|policy[.]publish[.]controls)$' | Exit 0; the four CTest records (install-stage fixture plus three selected tests) passed. Both staged variants rebuilt the quickstart and fault driver and checked all four fault modes; publisher controls passed. | PASS |
| Extracted archive CMake and pkg-config routes plus broken-target control | MUHTARI_RELEASE_QUICKSTART_PKG_CONFIG=1 ctest --preset ci-release --parallel 1 --no-tests=error --verbose -R '^release[.]ym_consumers$' | Exit 0; both consumers printed the expected result, the copied missing-target package matched the expected CMake include diagnostic, and archive/YM/source controls passed. | PASS |
| Production publisher controls against the current local Emscripten ZIP | cmake -DROOT="$PWD" -DWORK="$PWD/build/ci/policy/publish-emscripten-production" -DPRODUCTION_ARCHIVE="$PWD/build/emscripten/muhtari-libretro-emscripten.zip" "-DEXPECTED_SHA=$(git rev-parse HEAD)" -P tests/policy/publish.cmake | Exit 0; the real SDK archive validated and all five qualified-notice mutation controls were rejected. Local validation only; no publish occurred. | PASS |
| Deliberately omitted free(audio) sensitivity control | cmake -DQUICKSTART_PROGRAM="$PWD/build/ci/consumer/quickstart" -DQUICKSTART_FAULTS_PROGRAM="$PWD/build/ci/quickstart-fault-control/build/quickstart_faults" -DEMULATOR= -P tests/install/tests.cmake | Recorded in 31-02-SUMMARY.md: expected script exit 1; observer rejected the scratch copy with “audio allocation was not released” before the staged success marker. | PASS (fault detected) |
| Full serial local CI, package build, and release checks | ctest --preset ci --parallel 1; cmake --build --preset ci --target package; ctest --preset ci-release --parallel 1 | Existing local evidence in [LESSONS.md](../../LESSONS.md): 736 CI tests passed in 481.59s, package build exited 0, and all 8 release checks passed. No hosted result or release publication is claimed. | PASS (local evidence) |
| Default-parallel full workflow and Emscripten controls | MUHTARI_RELEASE_QUICKSTART_PKG_CONFIG=1 cmake --workflow --preset ci; emscripten.bundle.controls (local CTest log) | Existing local workflow run exited 8 after four CTest timeouts under default parallelism; the same affected tests and the full CI suite passed serially. The separate bundle-controls test reached its 30s limit under Docker amd64 on arm64 after earlier controls had reported success; emscripten.bundle itself passed in 22.26s. | LOCAL LIMIT; not a phase truth |

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
| tests/consumer/quickstart_faults.c | 67, 74, 80 | return NULL | Info | Expected allocation-failure injection and tracking-capacity guard; injected failures are asserted by the staged fault tests. |
| tests/emscripten/bundle.cmake | 788 | Synthetic module returns an empty object | Info | Minimal JavaScript module is generated only as a bundle-control fixture; it is not product code. No debt markers or production stubs were found. |

### Human Verification Required

N/A — all phase criteria are tied to reproducible package builds, output, diagnostics and cleanup behavior. The phase has no visual interface or external service behavior requiring human judgment.

### Gaps Summary

No phase gaps remain. Current staged tests exercise four failure modes through the exact example source and verify the exit, diagnostic, absence of example success output, and one-time release of each acquired pointer. The current extracted-archive test passes both documented package routes and the strict damaged-target control. The scratch omitted-free sensitivity control is preserved in the plan summary and shows that the release observer rejects a missing audio release.

Local execution evidence remains source- and environment-specific: the full serial CI suite, package build, and release tests passed as recorded in LESSONS.md. A default-parallel workflow attempt hit four local test timeouts, then passed on serial rerun. The Emscripten bundle creation test passed, while its separate controls test reached the local 30-second limit under Docker amd64 on this arm64 host; the direct production publisher controls against the local archive passed. These results do not leave a Phase 31 criterion unverified. Hosted CI and a new release publication are not claimed.

---

_Verified: 2026-11-20T10:16:10Z_
_Verifier: the agent (gsd-verifier)_
