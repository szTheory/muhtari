cmake_minimum_required(VERSION 3.25)

# version.txt is the single version source; release-please edits only it.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" MUHTARI_VERSION LIMIT_COUNT 1)
if(NOT MUHTARI_VERSION MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
  message(FATAL_ERROR "version.txt must hold MAJOR.MINOR.PATCH, not '${MUHTARI_VERSION}'")
endif()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS version.txt)

# Only C is needed to build the library. A project that embeds this one with
# add_subdirectory gets the library and the muhtari::muhtari alias; the runner,
# the libretro core and the tests are added only when this is the top-level
# project.
project(muhtari VERSION ${MUHTARI_VERSION} LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(MUHTARI_BUILD_LIBRETRO "Build the libretro core" ON)

# The warning set, applied only when this is the top-level project so an
# embedder's older or console compiler never meets an unknown flag. Presets
# make warnings fatal through CMAKE_COMPILE_WARNING_AS_ERROR.
function(muhtari_warnings target)
  if(PROJECT_IS_TOP_LEVEL)
    target_compile_options(${target} PRIVATE
      $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Wmissing-prototypes -Wvla -Wundef -Wcast-qual -Wimplicit-fallthrough -Wmissing-variable-declarations>
      $<$<C_COMPILER_ID:GNU>:-Wduplicated-cond -Wlogical-op>
      $<$<C_COMPILER_ID:MSVC>:/W4 /utf-8>)
  endif()
endfunction()

option(MUHTARI_NOFP "Build the core with general registers only; register only the abi tests" OFF)

# The library version reaches the core through a private generated header;
# the public header carries no version macros.
configure_file(cmake/version.h.in ${PROJECT_BINARY_DIR}/generated/muhtari_version.h @ONLY)

# The options of every target that holds core code. The core links nothing
# beyond memcpy, memmove, memset and memcmp: no stack protector or fortify
# helpers, no MSVC security cookie and no default runtime library, and no
# build path recorded in the objects. The nofp lane proves it integer-only by
# machine code, not text: GCC's -mgeneral-regs-only rejects any
# floating-point type, and the abi fixtures copy these options from muhtari.
function(muhtari_core_options target)
  set_target_properties(${target} PROPERTIES
    C_VISIBILITY_PRESET hidden
    POSITION_INDEPENDENT_CODE ON)
  target_compile_options(${target} PRIVATE
    $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-fno-stack-protector -U_FORTIFY_SOURCE -ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=.>
    $<$<C_COMPILER_ID:MSVC>:/GS- /Zl>)
  if(MUHTARI_NOFP)
    target_compile_options(${target} PRIVATE -mgeneral-regs-only)
  endif()
  target_include_directories(${target} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/generated)
  muhtari_warnings(${target})
endfunction()

# The core library, built from object libraries so each object is compiled
# once: the CPU test programs link the CPU's object with their own bus, so the
# object under test is the object that ships. muhtari_cpu joins the archive
# with the bus that defines its two bus functions.
add_library(muhtari_objs OBJECT
  src/instance.c
  src/frame.c
  src/testcard.c)
add_library(muhtari_cpu OBJECT src/cpu.c)
add_library(muhtari STATIC $<TARGET_OBJECTS:muhtari_objs>)
add_library(muhtari::muhtari ALIAS muhtari)
target_include_directories(muhtari PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
foreach(core_target IN ITEMS muhtari muhtari_objs muhtari_cpu)
  muhtari_core_options(${core_target})
endforeach()

if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# C++ only compiles the public header as a C++ embedder would; an embedding
# project never meets it.
enable_language(CXX)

# The library component: the header, the static library, a CMake package
# exporting muhtari::muhtari and a pkg-config file, and nothing else. Before
# 1.0 a minor version may break the API, so the package accepts a request
# only from the same MAJOR.MINOR. Both files locate the prefix from their own
# directory, so an extracted archive works wherever it lands.
include(CMakePackageConfigHelpers)
set(MUHTARI_CMAKE_DIR lib/cmake/muhtari)
install(TARGETS muhtari EXPORT muhtariTargets
  ARCHIVE DESTINATION lib COMPONENT library)
install(FILES include/muhtari.h DESTINATION include COMPONENT library)
install(EXPORT muhtariTargets NAMESPACE muhtari::
  DESTINATION ${MUHTARI_CMAKE_DIR} COMPONENT library)
configure_package_config_file(cmake/muhtariConfig.cmake.in
  ${PROJECT_BINARY_DIR}/muhtariConfig.cmake
  INSTALL_DESTINATION ${MUHTARI_CMAKE_DIR})
write_basic_package_version_file(${PROJECT_BINARY_DIR}/muhtariConfigVersion.cmake
  COMPATIBILITY SameMinorVersion)
configure_file(cmake/muhtari.pc.in ${PROJECT_BINARY_DIR}/muhtari.pc @ONLY)
install(FILES
    ${PROJECT_BINARY_DIR}/muhtariConfig.cmake
    ${PROJECT_BINARY_DIR}/muhtariConfigVersion.cmake
  DESTINATION ${MUHTARI_CMAKE_DIR} COMPONENT library)
install(FILES ${PROJECT_BINARY_DIR}/muhtari.pc
  DESTINATION lib/pkgconfig COMPONENT library)

# Host helpers the runner and the libretro adapter share: the palette that
# turns native colour bytes into RGB. Outside the core and its hash. PIC lets
# it link into the libretro module; hidden visibility keeps its symbols out of
# that module's exports.
add_library(muhtari_host STATIC host/palette.c)
target_include_directories(muhtari_host PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/host)
set_target_properties(muhtari_host PROPERTIES
  C_VISIBILITY_PRESET hidden
  POSITION_INDEPENDENT_CODE ON)
muhtari_warnings(muhtari_host)

# The headless runner, built as ISO C17 with POSIX 2008 and no GNU extensions.
add_executable(muhtari-run
  runner/main.c
  runner/bench.c
  runner/clock.c
  runner/crc32.c
  runner/png.c
  runner/sha256.c
  runner/stop.c)
target_link_libraries(muhtari-run PRIVATE muhtari::muhtari muhtari_host)
target_compile_definitions(muhtari-run PRIVATE _POSIX_C_SOURCE=200809L)
set_target_properties(muhtari-run PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/runner)
muhtari_warnings(muhtari-run)
install(TARGETS muhtari-run RUNTIME DESTINATION bin COMPONENT runner)

# The libretro core links muhtari_host, which exists only at top level, so an
# add_subdirectory embedding never builds it.
if(MUHTARI_BUILD_LIBRETRO)
  add_subdirectory(libretro)
endif()

# Release archives, one per install component.
include(cmake/packaging.cmake)

# The hygiene lane registers only the repository checks (formatting, the
# integer-only text scan, the analyzer, scripts/hygiene.sh) and finds its own
# tools, so no one sets PATH: clang-format and clang-tidy 21 by their Debian
# names, then Homebrew's keg-only llvm@21, then PATH.
option(MUHTARI_HYGIENE "Register only the hygiene tests" OFF)
if(MUHTARI_HYGIENE)
  set(hint "")
  find_program(MUHTARI_BREW brew)
  if(MUHTARI_BREW)
    execute_process(COMMAND ${MUHTARI_BREW} --prefix llvm@21
      OUTPUT_VARIABLE prefix OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET RESULT_VARIABLE rc)
    if(rc EQUAL 0)
      set(hint ${prefix}/bin)
    endif()
  endif()
  set(get "macOS: brew install llvm@21; Ubuntu: apt install clang-format-21 clang-tidy-21")
  find_program(MUHTARI_CLANG_FORMAT NAMES clang-format-21 clang-format HINTS ${hint})
  find_program(MUHTARI_CLANG_TIDY NAMES clang-tidy-21 clang-tidy HINTS ${hint})
  foreach(tool CLANG_FORMAT CLANG_TIDY)
    if(NOT MUHTARI_${tool})
      string(TOLOWER ${tool} name)
      string(REPLACE "_" "-" name ${name})
      message(FATAL_ERROR "hygiene: ${name} 21 not found (${get})")
    endif()
  endforeach()
  execute_process(COMMAND ${MUHTARI_CLANG_FORMAT} --version OUTPUT_VARIABLE version)
  if(NOT version MATCHES "version 21\\.")
    string(REGEX MATCH "version [0-9.]+" found "${version}")
    message(FATAL_ERROR "hygiene: ${MUHTARI_CLANG_FORMAT} is ${found}, not 21 (${get})")
  endif()
  find_program(MUHTARI_JQ jq REQUIRED)
  find_program(MUHTARI_SH sh REQUIRED)
  find_package(Git REQUIRED)
endif()

# The nofp lane builds every core object with -mgeneral-regs-only (see
# muhtari_core_options) and registers only the abi tests.
if(MUHTARI_NOFP AND NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
  message(FATAL_ERROR "nofp: -mgeneral-regs-only needs GCC, not ${CMAKE_C_COMPILER_ID}")
endif()

# Tests. MUHTARI_TIMEOUT_SCALE multiplies every test's timeout on slow lanes;
# MUHTARI_INVENTORY names the tests/inventory/<name>.txt the registered tests
# must equal.
set(MUHTARI_TIMEOUT_SCALE 1 CACHE STRING "Multiplier for every test timeout")
# MUHTARI_SHORT_RUNS shortens the long runs from 600 frames to 60 on slow
# lanes such as the sanitizers. Every test still runs.
option(MUHTARI_SHORT_RUNS "Run long tests for 60 frames instead of 600" OFF)
set(MUHTARI_LONG_RUN 600)
if(MUHTARI_SHORT_RUNS)
  set(MUHTARI_LONG_RUN 60)
endif()
set(inventory ci)
if(MUHTARI_HYGIENE)
  set(inventory hygiene)
elseif(MUHTARI_NOFP)
  set(inventory nofp)
endif()
set(MUHTARI_INVENTORY ${inventory} CACHE STRING "Test inventory file under tests/inventory")
# Fault prevented: every wasm32 test failing on a missing runtime, with a
# message that does not say where the runtime comes from.
if(CMAKE_SYSTEM_NAME STREQUAL "WASI")
  list(GET CMAKE_CROSSCOMPILING_EMULATOR 0 emulator)
  if(NOT EXISTS "${emulator}")
    message(FATAL_ERROR "wasm32: the test runtime '${emulator}' is missing; fetch it with\n"
      "  scripts/fetch-pin.sh wasmtime-<arch>-linux build/pins/wasmtime")
  endif()
endif()
enable_testing()
include(cmake/MuhtariTests.cmake)
include(tests/tests.cmake)
