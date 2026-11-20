# Phase 31: Installed C library quickstart - Research

**Researched:** 2026-11-19  
**Repository revision:** `eb0647bdafb83e049e55055adcd571020d33b0f2`  
**Domain:** Installed C library package consumption and first-use documentation  
**Confidence:** MEDIUM — package layout and the current CMake consumer flow are directly verified; the official Ubuntu 26.04 image inventory lists pkg-config, but the workflow has not exercised it yet; Windows/MSVC compatibility is not established.

<user_constraints>
## User Constraints

Phase initialization reported `"has_context": false`. The autonomous discussion has not yet been recorded; the recommendation below is research input, not a locked decision. This artifact remains research only: compare doing nothing, the smallest useful implementation, and the strongest meaningful alternative; do not implement beyond evidence. [VERIFIED: `gsd-tools query init.phase-op 31`, exit 0.]
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| EMBED-01 | A C host integrator can build and run a complete minimal example against the installed library package using CMake or pkg-config, with the required allocator, buffers, one frame, and expected result explained. [VERIFIED: .planning/REQUIREMENTS.md:16 — quote: “A C host integrator can build and run a complete minimal example against the installed library package using CMake or pkg-config, with the required allocator, buffers, one frame, and expected result explained.”] | Reuse the existing tested no-content one-frame behavior, but expose it as one concise source shared by the user guide and clean extracted-package consumers. Verify the extracted archive through CMake on all package lanes and pkg-config on the supported POSIX toolchains; add one exact-failure control for a damaged imported target. |
</phase_requirements>

## Summary

The repository already ships a relocatable static C library package with both a CMake config package and a pkg-config file. The existing consumer source also has the essential no-content path: it supplies the required allocator, sizes video and audio buffers from `muhtari_get_info`, runs one frame, and checks that the first frame contains 800 audio samples. [VERIFIED: docs/embedding.md:3-5,9-23; `tests/consumer/main.c`:161-214 — quote: “io.audio_count == 800u”; the same source sets `cfg.allocator.alloc = consumer_alloc;`, `cfg.allocator.free = consumer_free;`, `io.video_pitch = info.width;`, and `io.audio_capacity = info.max_audio_samples;` at lines 173-174 and 189-197.]

The gap is that this small path is buried in a larger test consumer and is not presented as a user example. The release-archive consumer unpacks the actual library ZIP and verifies it through CMake, but invokes only the separate YM fixture mode; the inspected script does not exercise pkg-config. [VERIFIED: tests/release/ym_consumers.cmake:18-21,85-109 — quote: `"-DCMAKE_PREFIX_PATH=${WORK}/library"` and `--ym-fixture`; tests/consumer/main.c:24-153 contains the YM path.] The lowest-change useful approach is a new compact, standalone C example shared by documentation and the existing installed-consumer checks. Keep the CMake route on every current release lane and test pkg-config against the extracted archive only where a supported POSIX compiler and pkg-config are available. Use CMake for Windows until an MSVC-compatible pkg-config toolchain is demonstrated.

**Primary recommendation:** Add one small no-content quickstart source; have clean-consumer tests compile that same source from the extracted release package through CMake on every platform and pkg-config on one explicit Linux CI lane. Validate the pkg-config route once more against the locally built Apple Silicon release archive; use CMake on Windows. Add an expected-failure test using a copied package whose exported CMake target file is damaged. The quickstart should link to the complete [Embedding the library API guide](../../../docs/embedding.md) and say that it demonstrates package discovery, linkage, instance creation, buffers and one frame—not gameplay compatibility.

## Architectural Responsibility Map

| Capability | Primary Tier | Secondary Tier | Rationale |
|------------|-------------|----------------|-----------|
| C host calls and frame buffers | API / Backend (in-process C library) | Host process | The host invokes the static library directly and owns its allocator and frame buffers; the library starts no threads or files. [VERIFIED: docs/embedding.md:3-5 — quote: “It allocates memory only through the allocator you pass, keeps all state in the instance, starts no threads, opens no files”.] |
| Installed package discovery and linkage | CDN / Static (release archive) | API / Backend (in-process library) | CMake or pkg-config resolves the installed archive and header before the host runs. [VERIFIED: docs/embedding.md:9-18,25-48.] |
| Quickstart behavior claim | API / Backend (in-process C library) | CDN / Static (documentation) | The example's expected result must be the tested first-frame sample count, with no game image involved. [VERIFIED: tests/install/tests.cmake:56-69 and tests/consumer/main.c:181-213.] |

## Standard Stack

### Core

| Technology | Version / form | Purpose | Why Standard |
|------------|----------------|---------|--------------|
| Muhtari C library | Static archive plus `muhtari.h`; C17. [VERIFIED: docs/embedding.md:3-5 — quote: “one static archive and one header, `muhtari.h`, written in C17”.] | Host calls, instance lifecycle and frame output | This is the installed artifact required by EMBED-01. |
| CMake package | `find_package(muhtari CONFIG REQUIRED)` and target `muhtari::muhtari`. [VERIFIED: docs/embedding.md:25-33 — quote: “find_package(muhtari CONFIG REQUIRED)” and “target_link_libraries(app PRIVATE muhtari::muhtari)”.] | Portable discovery for the clean consumer | Already documented, staged and exercised on every current native CI lane. [VERIFIED: tests/install/tests.cmake:19-29,46-69; .planning/preparation/ENGINEERING.md:109 — quote: “One `ci.txt` serves all six legs”.] |
| pkg-config file | `lib/pkgconfig/muhtari.pc`. [VERIFIED: docs/embedding.md:12-18 — quote: “lib/pkgconfig/muhtari.pc”.] | POSIX-toolchain discovery for the same archive | Already included in the package; its generated `Cflags` and `Libs` refer to paths relative to the `.pc` file. [VERIFIED: cmake/muhtari.pc.in:1-9 — quote: `prefix=${pcfiledir}/../..`, `Libs: -L${libdir} -lmuhtari`, and `Cflags: -I${includedir}`.] |

### Supporting

| Tool | Version / requirement | Purpose | Availability |
|------|----------------------|---------|--------------|
| C compiler | C17; the repository's consumer project sets `CMAKE_C_STANDARD 17`. [VERIFIED: tests/consumer/CMakeLists.txt:5-10 — quote: `set(CMAKE_C_STANDARD 17)` and `set(CMAKE_C_EXTENSIONS OFF)`.] | Compile the example | Local `cc --version` reports Apple clang 21.0.0 (exit 0). |
| CMake | At least 3.25 for the existing outside-consumer project. [VERIFIED: tests/consumer/CMakeLists.txt:5 — quote: `cmake_minimum_required(VERSION 3.25)`.] | Configure the installed target route | Local CMake is 4.4.3 (exit 0). |
| pkg-config | Tool required for the `.pc` route; official Ubuntu 26.04 runner inventory lists version 2.5.1-4. | Emit include and link flags from the extracted package | Local pkg-config is 3.0.9 (exit 0); actual hosted workflow execution remains unverified. |

No new runtime or build package is recommended. Do not add a wrapper library, package manager dependency, or custom discovery layer.

The Package Legitimacy Gate is not applicable because no external package is recommended for installation.

## Project Constraints (from AGENTS.md)

- Keep the deliverable an in-process C library example; the repo has three deliverables and “No window, audio-device or input-device code.” [VERIFIED: AGENTS.md:49-50,61-64 — quote: “Library, runner, libretro core. No window, audio-device or input-device code.”]
- Keep the example ROM-free and do not add copyrighted media. The tree rule is “No ROM, BIOS or save bytes except fixtures listed in `tests/roms/manifest.txt`.” [VERIFIED: AGENTS.md:65-67 — quote reproduced.]
- Keep implementation C17 and make the example/documentation agree with the checked behavior. “Every behaviour change lands with a test that `cmake --workflow --preset ci` runs.” [VERIFIED: AGENTS.md:71-73 — quote reproduced; tests/consumer/CMakeLists.txt:8-10.] The phase should not change core behavior or the public API, which are explicitly out of scope. [VERIFIED: .planning/REQUIREMENTS.md:37-48 — quote: “New emulation behavior, public API/ABI, or serialized-state changes” are out of scope.]
- Keep the README short, put getting-started/API detail in `docs/`, and leave the public header terse. [VERIFIED: AGENTS.md:71-73 — quote: “The README stays user-facing and short; testing, CI and maintainer docs live in `docs/`; the public header stays terse and puts behaviour prose in `docs/`.”]
- Extend existing CMake `-P` or POSIX shell checks. A materially changed check must identify its fault and include representative pass and fault behavior; preserve existing checks. [VERIFIED: AGENTS.md:86-91 — quote: “Repository checks use CMake `-P` or POSIX sh and reuse existing mechanisms. A new or materially changed check names its concrete fault and exercises representative pass/fault behavior”.]
- Use exact failure and diagnostic checks for the negative control; the project helper rejects a control that passes on any failure. [VERIFIED: tests/cmake/expect_fail.cmake:1-5,13-18 — quote: “this passes only when CMD exits with exactly CODE and its output matches REGEX”.]
- The project requires a phase branch before the first committing GSD step and forbids committing on `main`. [VERIFIED: AGENTS.md:142-147 — quote: “Before the first GSD step that commits, branch off `origin/main`: `phase/NN-slug` for a phase”; “Never commit on `main` once a remote exists.”]

## Architecture Patterns

### Recommendation and alternatives

| Option | Assessment |
|--------|------------|
| Do nothing | The current guide documents both package routes and the current `install.consumer` verifies the no-argument one-frame path. But the user still has no concise standalone source, and the extracted-release check runs only the YM fixture path through CMake. This does not meet the phase's explicit example and dual-route acceptance. [VERIFIED: docs/embedding.md:7-48; tests/install/tests.cmake:56-84; tests/release/ym_consumers.cmake:85-109.] |
| Smallest useful change — recommended | Add a compact standalone source and a short getting-started page. Reuse that source in the outside CMake consumer and extracted release-archive route; run the CMake route on all existing package lanes and the pkg-config route on one Linux CI lane plus one local Apple Silicon archive check. Keep the existing YM fixture consumer and its deeper checks. Add one isolated broken-export control. This gives one implementation of the example, a recurring pkg-config check on shipped Linux package bytes, and no new runtime dependency. |
| Strongest alternative | Reframe `tests/consumer/main.c` as the documented example and keep its `--ym-fixture` branch. That avoids a new source, and the local staged CMake consumer already runs its no-argument path. The cost is that a host-facing “small example” remains embedded in a larger test-only file, while the extracted release test still needs changes to run the no-argument path and pkg-config. [VERIFIED: tests/consumer/main.c:24-153,155-214; tests/install/tests.cmake:56-84; tests/release/ym_consumers.cmake:85-109.] |

### Suggested component responsibilities

- **New example source** (suggested location `examples/embedding/quickstart.c`): own the full minimal C program. It should use the library's required allocator, query frame dimensions/capacity, allocate host-owned video and audio buffers, run one frame, check the first-frame result, and clean up.
- **Getting-started page** (suggested location `docs/embedding-quickstart.md`): show how to unpack the matching release ZIP, build the example through CMake or pkg-config, state tool/platform needs, explain the no-content result, and link to `docs/embedding.md` for the complete API contract.
- **Existing installed consumer project**: compile the same example source against the staged package so normal installed-consumer CI proves it still builds and runs.
- **Existing release archive consumer**: use the already-extracted archive under `tests/release/ym_consumers.cmake`; exercise CMake on every package lane and pkg-config on the POSIX toolchains where the tool is an explicit available prerequisite. Keep its current YM and artifact-integrity controls.

These paths are proposed locations; the planner should choose the final filenames.

### Package route evidence and platform boundary

- The documented CMake route is relocatable: the current guide tells consumers to set `CMAKE_PREFIX_PATH` to the unzipped directory. [VERIFIED: docs/embedding.md:25-33 — quote: `-DCMAKE_PREFIX_PATH=<unzipped directory>`.]
- The release consumer extracts `muhtari-library-<os>-<arch>.zip`, hashes the packaged header against the checked-out header, disables user/system package registries, and configures against the extracted package directory. [VERIFIED: docs/embedding.md:9-18; tests/release/ym_consumers.cmake:18-21,36-38,85-90 — quote: `-DCMAKE_FIND_USE_PACKAGE_REGISTRY=OFF` and `-DCMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY=OFF`.]
- The release archive uses `lib/muhtari.lib` on Windows, while the `.pc` template emits `-L${libdir} -lmuhtari`. [VERIFIED: docs/embedding.md:12-16 — quote: `lib/libmuhtari.a                 (lib/muhtari.lib on Windows)`; cmake/muhtari.pc.in:8-9 — quote reproduced.]
- Existing Windows/MSVC installed-consumer coverage uses CMake and explicitly tests static and DLL CRT consumer configurations. [VERIFIED: tests/install/tests.cmake:46-69 — quote: `set(install_runtime "MultiThreaded$<$<CONFIG:Debug>:Debug>")` and `set(install_runtime "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")`.]
- **Inference:** Keep CMake package verification on all six existing native CI legs. Verify pkg-config on one Linux CI lane where the tool is an explicit prerequisite and once locally against the Apple Silicon release archive; use CMake on Windows until an actual Windows compiler/pkg-config pair proves the packaged `.pc` flags work. The `.pc` file is present in the Windows archive, but present metadata alone does not establish MSVC compatibility. This bounds CI setup while validating a shipped Linux archive and a macOS archive locally. Hosted runner availability for pkg-config has not been checked in this research. [VERIFIED: .planning/preparation/ENGINEERING.md:109; tests/install/tests.cmake:46-69; tests/release/ym_consumers.cmake:12-23; cmake/muhtari.pc.in:1-9.]

### Example behavior

The no-argument consumer path is media-free: it creates an instance, queries info, sizes both outputs from that info, runs a frame, and prints a fixed success line after checking the sample count. [VERIFIED: tests/consumer/main.c:155-213 — quote: `if (argc != 1)`, `io.audio_capacity = info.max_audio_samples;`, `ok = st == MUHTARI_STATUS_OK && io.audio_count == 800u;`, and `printf("consumer: one frame, 800 samples\n");`.]

Keep the known result limited to that exact behavior: the default no-content frame returns 800 audio samples. It demonstrates that the host found and linked the package, created an instance, supplied required buffers, and ran one frame. It establishes no cartridge boot or gameplay compatibility. [VERIFIED: tests/consumer/main.c:181-213; .planning/ROADMAP.md:64-68 — quote: “without copyrighted media” and “without implying gameplay compatibility”.]

### Representative negative control

Add the fault case to the existing extracted-package consumer flow: copy the extracted library prefix to a separate scratch directory, remove or deliberately break `lib/cmake/muhtari/muhtariTargets.cmake` in that copy, and configure the same example consumer against it. Require the expected configure failure and a diagnostic naming the missing/broken target export, using `tests/cmake/expect_fail.cmake`; never mutate the good extracted package or use bare `WILL_FAIL`. The package config directly includes `muhtariTargets.cmake`, so this fault is on the public CMake package boundary. [VERIFIED: cmake/muhtariConfig.cmake.in:1-6 — quote: `include("${CMAKE_CURRENT_LIST_DIR}/muhtariTargets.cmake")`; tests/cmake/expect_fail.cmake:1-5,13-18.]

The current release consumer already checks corrupt fixture rejection and substituted core identity, but those controls do not demonstrate detection of a broken imported library target or header. Keep those controls intact. [VERIFIED: tests/release/ym_consumers.cmake:123-138 — quote: “corrupt fixture accepted by extracted consumer” and “core source differs”.]

## Don't Hand-Roll

| Problem | Don't Build | Use Instead | Why |
|---------|-------------|-------------|-----|
| Installed target discovery | A custom CMake find module or hand-maintained library paths | Existing `find_package(muhtari CONFIG REQUIRED)` and `muhtari::muhtari` target | The release package already exports this target and locates its files relative to the package. [VERIFIED: docs/embedding.md:25-33; cmake/muhtariConfig.cmake.in:1-6.] |
| pkg-config flags | Manually duplicate include/library paths in docs or test commands | `pkg-config --cflags --libs muhtari` with `PKG_CONFIG_PATH` pointed at the extracted ZIP | The `.pc` metadata computes paths relative to itself. [VERIFIED: docs/embedding.md:44-48; cmake/muhtari.pc.in:1-9.] |
| Documentation/test parity | Maintain separate copies of sample logic | Compile and run one source from docs-linked instructions and both package-route tests | The test source already demonstrates the behavior; sharing the concise version prevents docs and checks from diverging. [VERIFIED: tests/consumer/main.c:161-214; tests/install/tests.cmake:56-69.] |

## Common Pitfalls

1. **A partial snippet looks complete.** The existing “A frame” section shows only a fragment and says to handle errors; do not mistake it for the complete executable the requirement asks for. [VERIFIED: docs/embedding.md:112-138.]
2. **Testing a staged build is not the same as testing a release ZIP.** `install.consumer` consumes `<build>/stage`; `release.ym_consumers` is the path that extracts and uses the actual archive. Keep both checks, and run the documented source against the extracted package. [VERIFIED: tests/install/tests.cmake:19-28,56-69; tests/release/ym_consumers.cmake:24-38,85-109.]
3. **The existing release check is not the minimal example check.** It runs `consumer --ym-fixture`; the no-argument one-frame result is a separate mode in that source. Explicitly run the example path. [VERIFIED: tests/release/ym_consumers.cmake:101-109; tests/consumer/main.c:155-160.]
4. **Do not turn an unverified platform path into a support claim.** The current source proves CMake consumer usage on Windows, not that MSVC can consume the emitted `.pc` flags. Keep the Windows CMake route documented unless a Windows pkg-config toolchain passes against the extracted archive. [VERIFIED: tests/install/tests.cmake:46-69; cmake/muhtari.pc.in:8-9.]
5. **A generic expected-failure check can pass for the wrong reason.** Use the existing helper's exact exit code and output regex for the broken-target control; it explicitly rejects unrelated failures. [VERIFIED: tests/cmake/expect_fail.cmake:3-5,13-18.]
6. **A clean result is not a compatibility claim.** The no-content sample establishes an API/package path and one frame's sample count only. Do not imply an Atari 7800 game was loaded or validated. [VERIFIED: tests/consumer/main.c:155-213; .planning/ROADMAP.md:65-68.]

## Code Examples

Use the tested no-content path as the source of truth. These lines from the current consumer show the required allocator, info-sized buffers, and result check; the recommended new source should preserve this behavior while removing the unrelated YM fixture branch. [VERIFIED: tests/consumer/main.c:161-197 — exact values include `cfg.allocator.alloc = consumer_alloc;`, `cfg.allocator.free = consumer_free;`, `io.video_pitch = info.width;`, `io.audio_capacity = info.max_audio_samples;`, and `io.audio_count == 800u`; the status constant is declared as `MUHTARI_STATUS_OK = 0` at include/muhtari.h:15-23.]

```c
cfg.allocator.alloc = consumer_alloc;
cfg.allocator.free = consumer_free;
io.video_pitch = info.width;
io.audio_capacity = info.max_audio_samples;
st = muhtari_run_frame(m, &io);
ok = st == MUHTARI_STATUS_OK && io.audio_count == 800u;
```

The published commands should stay aligned with the current package routes. [VERIFIED: docs/embedding.md:25-48 — exact package names and commands quoted below.]

```cmake
find_package(muhtari CONFIG REQUIRED)
target_link_libraries(app PRIVATE muhtari::muhtari)
```

```sh
PKG_CONFIG_PATH=<unzipped directory>/lib/pkgconfig cc app.c $(pkg-config --cflags --libs muhtari)
```

## Environment Availability

| Dependency | Required By | Available locally | Version | Fallback |
|------------|-------------|-------------------|---------|----------|
| C compiler | Compile quickstart | Yes | Apple clang 21.0.0 (`cc --version`, exit 0) | — |
| CMake | CMake consumer | Yes | 4.4.3 (`cmake --version`, exit 0); project consumer minimum is 3.25. [VERIFIED: tests/consumer/CMakeLists.txt:5 — quote: `cmake_minimum_required(VERSION 3.25)`. ] | — |
| pkg-config | pkg-config consumer | Yes locally; listed on the configured Ubuntu CI image | 3.0.9 locally (`pkg-config --version`, exit 0); 2.5.1-4 in the official Ubuntu 26.04 image inventory, image version 20261111.196.1 | Use the existing Ubuntu 26.04 lane; separately run the Mac archive check locally. |

No external runtime service, ROM, BIOS, save, or added package is needed for the no-content example. [VERIFIED: tests/consumer/main.c:155-213; AGENTS.md:65-70.]

## Assumptions Log

| # | Claim / unresolved point | Section | Risk if wrong |
|---|--------------------------|---------|---------------|
| A1 | The packaged `.pc` output is not proven usable by a Windows/MSVC consumer. [ASSUMED — no supported Windows pkg-config/compiler pairing was verified.] | Standard Stack; Package route evidence | Documentation could promise a Windows route that does not work; keep CMake as the documented Windows route. |

## Open Questions

1. **Which existing CI image should own the recurring pkg-config check? Resolved for planning.**
   - What we know: `.github/workflows/ci.yml` uses `ubuntu-26.04`; the official `actions/runner-images` Ubuntu 26.04 image inventory, image version `20261111.196.1`, lists `pkg-config` 2.5.1-4. The package includes `lib/pkgconfig/muhtari.pc`. [VERIFIED: workflow line 49; official [Ubuntu 26.04 image inventory](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2604-Readme.md), checked 2026-11-19; `docs/embedding.md:12-18`; `cmake/muhtari.pc.in:1-9`.]
   - Remaining limit: the image inventory is not a receipt from this repository's hosted workflow; the new check must fail clearly if the tool is absent at runtime.
   - Recommendation: exercise pkg-config through the existing extracted-archive flow on one Ubuntu 26.04 CI lane. No new package installation is indicated by the image inventory. Run one local Apple Silicon check against the locally produced archive. Keep CMake coverage in all six current legs and do not add a pkg-config-only Windows promise.
2. **Does any supported Windows pkg-config/compiler pair consume the current `.pc` file?**
   - What we know: the Windows archive contains `lib/muhtari.lib`, while the `.pc` file emits `-L`/`-l`; existing Windows installed-consumer tests use CMake. [VERIFIED: docs/embedding.md:12-16; cmake/muhtari.pc.in:8-9; tests/install/tests.cmake:46-69.]
   - What's unclear: whether the project's intended Windows toolchain supports this output.
   - Recommendation: document CMake as the Windows route. Revisit only if a concrete Windows pkg-config consumer is in scope and can be tested.

## Sources

### Primary — repository source of truth

- `.planning/ROADMAP.md:56-68` and `.planning/REQUIREMENTS.md:16` — Phase 31 goal, acceptance and EMBED-01.
- `docs/embedding.md:1-48,71-150` — archive layout, CMake/pkg-config instructions, C17, allocator and frame contracts.
- `include/muhtari.h:15-23,40-83` — status, required allocator, info and frame API.
- `tests/consumer/main.c:1-24,155-214` and `tests/consumer/CMakeLists.txt:1-15` — current media-free sample path and outside consumer.
- `tests/install/tests.cmake:13-106` — staged installed-package consumer and source-tree embedding tests.
- `tests/release/ym_consumers.cmake:18-38,84-138` and `tests/release/tests.cmake:98-105` — actual archive extraction, CMake consumer, existing fixture/control behavior.
- `cmake/muhtariConfig.cmake.in:1-6`, `cmake/muhtari.pc.in:1-9`, `tests/cmake/expect_fail.cmake:1-18` — package metadata and precise negative-control helper.
- `AGENTS.md`, `.planning/preparation/README.md`, `.planning/preparation/DECISIONS.md`, `.planning/preparation/ENGINEERING.md`, `.planning/LESSONS.md` — project constraints and relevant package/CI/testing lessons.

### External sources

- **GitHub Actions hosted runner image inventory:** [`actions/runner-images` Ubuntu 26.04 README](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2604-Readme.md), image version `20261111.196.1`, lists `pkg-config` version `2.5.1-4`; checked 2026-11-19. Evidence class: official hosted image inventory; confidence high for the published image contents, not a substitute for a run of this repository's workflow. This resolves the earlier local-only assumption that hosted availability was unknown.

## Metadata

**Confidence breakdown:**
- Package layout and CMake behavior: HIGH — read the package template, documentation and extracted-archive test source.
- One-frame example behavior: HIGH — direct source and CTest expected-output assertion.
- pkg-config on configured Ubuntu CI image: MEDIUM-HIGH — official inventory lists it; this repository's hosted execution remains unverified until the normal CI run.
- pkg-config on Windows/MSVC: LOW — no positive toolchain evidence in the repository.

**Research date:** 2026-11-19  
**Source revisions:** repository `eb0647bdafb83e049e55055adcd571020d33b0f2`; Ubuntu hosted image `20261111.196.1` (official image inventory).  
**Revisit triggers:** the configured Ubuntu image no longer lists `pkg-config`; a Windows pkg-config compiler pairing becomes supported; package archive layout or `.pc` flags change; or the no-content first-frame result changes.

Security and Nyquist sections are omitted because the project config sets `"security_enforcement": false` and `"nyquist_validation": false`. [VERIFIED: .planning/config.json:36,60 — quotes: `"nyquist_validation": false` and `"security_enforcement": false`.]
