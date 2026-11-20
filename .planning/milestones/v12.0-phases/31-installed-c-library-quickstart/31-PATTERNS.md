# Phase 31: Installed C Library Quickstart — Pattern Map

**Mapped:** 2026-11-19  
**Files analyzed:** 5 likely implementation/test surfaces plus package metadata  
**Analogs found:** 4 / 5; no existing compiled pkg-config consumer test

The phase directory had no `CONTEXT.md` or `RESEARCH.md` when mapped. The likely file set below is inferred from the Phase 31 goal and success criteria in `.planning/ROADMAP.md:56-68`; refine it if phase context is added. All analog paths below are git-tracked source files.

## File Classification

| New/Modified File or Surface | Role | Data Flow | Closest Analog | Match Quality |
|------------------------------|------|-----------|----------------|---------------|
| `docs/embedding.md` | documentation | request-response | `docs/embedding.md` | exact |
| `tests/consumer/main.c` | example / test | request-response | `tests/consumer/main.c` | exact |
| `tests/consumer/CMakeLists.txt` | test config | request-response | `tests/consumer/CMakeLists.txt` | exact for CMake package route |
| `tests/install/tests.cmake` | integration test | install, build, run | `tests/install/tests.cmake` | exact for CMake installed consumer |
| pkg-config installed-consumer check (location not established) | integration test | install, compile, run | `cmake/muhtari.pc.in`, `docs/embedding.md` | no compiled consumer analog |
| `cmake/muhtari.pc.in` and `cmake/muhtariConfig.cmake.in` (only if package metadata needs changes) | package config | installed-path resolution | same files | exact |

## Pattern Assignments

### `docs/embedding.md` (documentation, request-response)

**Analog:** `docs/embedding.md`

The existing guide is the source of package locations, language standard, allocator contract, and output sizing. Its package routes already show the commands a quickstart should agree with:

```text
docs/embedding.md:25-33
find_package(muhtari CONFIG REQUIRED)
target_link_libraries(app PRIVATE muhtari::muhtari)
```

```text
docs/embedding.md:44-48
PKG_CONFIG_PATH=<unzipped directory>/lib/pkgconfig cc app.c $(pkg-config --cflags --libs muhtari)
```

The allocator and frame section at `docs/embedding.md:93-110,112-138` explains the required callback pair, `size` tags, info-derived buffer sizes, and status handling. The existing frame snippet is not a complete compilable program: `cfg`, `host_alloc`, `video`, and `audio` are supplied from earlier context. The standalone consumer pattern below has the corresponding complete setup. If the guide adds a full quickstart, keep the release archive layout and package instructions consistent with `docs/embedding.md:9-23,25-48`, link the complete API reference, and state that the no-content result proves the library/package call path rather than gameplay compatibility.

### `tests/consumer/main.c` (C example / test, request-response)

**Analog:** `tests/consumer/main.c`

This is the closest concrete implementation of the requested no-ROM quickstart. It uses only the public header and host C library; the default path creates an empty NTSC instance, sizes buffers from `muhtari_get_info`, runs one frame, checks the exact first-frame audio count, cleans up, and emits a stable success line.

**Public includes and allocator** (`tests/consumer/main.c:4-22`):

```c
#include <muhtari.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *consumer_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void consumer_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}
```

**Struct initialization, buffer sizing, and known result** (`tests/consumer/main.c:170-213`): zero each public struct before setting its `size`; query dimensions/capacity instead of duplicating them; check `MUHTARI_STATUS_OK` and `audio_count == 800`; print the exact success line matched by CTest. `docs/embedding.md:140-145` describes the neighboring regional behavior, so keep the 800-sample assertion scoped to this no-content NTSC first frame.

The same source also has a longer YM fixture mode (`tests/consumer/main.c:24-153`). Do not copy that mode into the minimal guide unless the phase intentionally expands the example; it adds fixture loading, NTSC/PAL loops, state replay, and additional numeric assertions.

The public contract supporting these calls is in tracked `include/muhtari.h:40-77,79-86`: allocator and config, info and frame layouts, and create/destroy/query/run declarations. `docs/embedding.md:71-91,93-110,132-145` supplies the behavioral details omitted from the terse public header.

### `tests/consumer/CMakeLists.txt` (consumer build config, request-response)

**Analog:** `tests/consumer/CMakeLists.txt`

The consumer is a distinct C17 project, discovers the installed config package rather than reaching into project targets, and links the exported target:

```cmake
tests/consumer/CMakeLists.txt:5-15
cmake_minimum_required(VERSION 3.25)
project(consumer C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(muhtari CONFIG REQUIRED)

add_executable(consumer main.c)
target_link_libraries(consumer PRIVATE muhtari::muhtari)
```

Keep this independent-project boundary for checks of the installed CMake target. The source-tree `add_subdirectory` consumer under `tests/embed/` is a different flow and does not prove the installed archive works.

### `tests/install/tests.cmake` (installed integration test, install/build/run)

**Analog:** `tests/install/tests.cmake`

The harness installs only the `library` component into a staging prefix, then uses `ctest --build-and-test` on `tests/consumer` with a fresh build directory and executes the result. `tests/install/tests.cmake:25-29,31-43,46-85` is the exact pattern for fixture ordering, compiler/config forwarding, static and DLL CRT variants, the staged `CMAKE_PREFIX_PATH`, and the known success-line check.

```cmake
tests/install/tests.cmake:25-29
muhtari_add_test(NAME install.stage
  COMMAND ${CMAKE_COMMAND} --install ${PROJECT_BINARY_DIR}
    --component library --prefix ${PROJECT_BINARY_DIR}/stage --config $<CONFIG>
  LABELS install TIMEOUT 60)
set_tests_properties(install.stage PROPERTIES FIXTURES_SETUP install_staged)
```

```cmake
tests/install/tests.cmake:56-69
muhtari_add_test(NAME ${install_name}
  COMMAND ${CMAKE_CTEST_COMMAND} --build-and-test
    ${CMAKE_CURRENT_LIST_DIR}/../consumer ${PROJECT_BINARY_DIR}/${install_dir}
    --build-generator ${CMAKE_GENERATOR}
    ${install_config}
    --build-options
      -DCMAKE_PREFIX_PATH=${PROJECT_BINARY_DIR}/stage
      -DCMAKE_MSVC_RUNTIME_LIBRARY=${install_runtime}
      ${install_build_options}
    --test-command ${CMAKE_CROSSCOMPILING_EMULATOR} consumer
  LABELS install TIMEOUT 120)
set_tests_properties(${install_name} PROPERTIES
  FIXTURES_REQUIRED install_staged
  PASS_REGULAR_EXPRESSION "consumer: one frame, 800 samples")
```

The release-archive counterpart is `tests/release/ym_consumers.cmake:18-21,84-109`: it extracts the real library ZIP, binds header and archive contents to the build snapshot, configures the same independent CMake consumer against the extracted package, builds, and runs it. It currently invokes only the CMake route. When a CMake script starts a cross-compiled consumer with `execute_process`, apply the configured emulator explicitly; `.planning/LESSONS.md:819-835` records the reproduced failure and correction.

### pkg-config installed-consumer check (new test surface; path not established)

**Closest references:** `docs/embedding.md:44-48`, `cmake/muhtari.pc.in:1-9`, `CMakeLists.txt:167-185`, and `tests/release/ym_consumers.cmake:18-21,84-109`.

There is no tracked test that compiles and runs a C source through pkg-config. The `.pc` file is packaged and listed in the extracted archive, but `tests/release/ym_consumers.cmake` configures with `find_package` and never invokes pkg-config. The `.pc` metadata uses `prefix=${pcfiledir}/../..`, then exports the include and static library flags; preserve this relocation behavior. Reuse the public C example for the second route so the two package routes compare the same result. Existing CMake installed-consumer coverage remains required.

## Shared Patterns

### Package locations and relocation

`CMakeLists.txt:162-185` installs `include/muhtari.h`, the static archive, exported `muhtari::muhtari` target, CMake config/version files, and `lib/pkgconfig/muhtari.pc` as the `library` component. `cmake/muhtariConfig.cmake.in:1-6` includes targets relative to `CMAKE_CURRENT_LIST_DIR`; `cmake/muhtari.pc.in:1-9` resolves paths relative to `pcfiledir`. Keep consumer checks against a staged/extracted prefix, not the source or build tree.

### Public API setup

`include/muhtari.h:40-49,60-77,79-83` defines the required allocator pair, `size`-tagged config/info/frame structs, and the minimal create/destroy/get-info/run-frame calls. The tested usage initializes structs to zero and then sets `size = (uint32_t)sizeof struct`; it obtains frame geometry and audio capacity from info before allocating. No ROM or BIOS bytes are needed for the one-frame no-content example.

### Known-result assertion

`tests/consumer/main.c:181-213` uses the actual status plus `io.audio_count == 800` as its semantic check, then prints `consumer: one frame, 800 samples`; `tests/install/tests.cmake:67-69` requires that exact line. Preserve both layers if refactoring: program-level result check and harness-level recognizable success output. The YM mode's detailed results are supplementary and do not replace the minimal no-content assertion.

### Negative controls

There is no existing negative control that specifically breaks the installed CMake target or public header. `tests/cmake/expect_fail.cmake:1-19` is the reusable strict failure matcher: it passes only for an exact exit code and output regex, avoiding false success from a missing file, typo, or crash. `tests/policy/tests.cmake:14-30` shows a gate and its `.self_test` using that helper. The phase's representative fault should be applied to the package/header consumer itself and checked for the intended configure, compile, or link failure; the existing release consumer's corrupt-ROM and substituted-binary controls (`tests/release/ym_consumers.cmake:123-138`) exercise different faults and do not establish package-target/header rejection.

The project policy in `.planning/preparation/ENGINEERING.md:109-113` also requires exact test inventory, no self-skip, and fault-specific negative controls. Do not weaken the CMake route coverage while adding pkg-config coverage, and do not add a package dependency for the example.

## No Analog Found

| Surface | Role | Data Flow | Reason |
|---------|------|-----------|--------|
| Compiled pkg-config consumer | integration test | install, compile, run | Only pkg-config metadata and documentation exist; no tracked test invokes the `.pc` route. |
| Installed-package/header negative control | integration test | request-response | Existing installed consumers are positive tests; current negative controls cover other rules or corrupt content, not the target/header package fault. |

## Metadata

**Analog search scope:** `docs/`, `include/`, `cmake/`, `tests/consumer/`, `tests/install/`, `tests/release/`, `tests/cmake/`, `tests/policy/`, `.planning/`  
**Files scanned:** 12 focused tracked sources and project guidance files  
**Pattern extraction date:** 2026-11-19
