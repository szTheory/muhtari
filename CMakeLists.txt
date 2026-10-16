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

# The core library. It links nothing beyond memcpy, memmove, memset and
# memcmp: no stack protector or fortify helpers, no MSVC security cookie and
# no default runtime library, and no build path recorded in the objects.
add_library(muhtari STATIC
  src/instance.c
  src/frame.c
  src/testcard.c)
add_library(muhtari::muhtari ALIAS muhtari)
target_include_directories(muhtari PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
set_target_properties(muhtari PROPERTIES
  C_VISIBILITY_PRESET hidden
  POSITION_INDEPENDENT_CODE ON)
target_compile_options(muhtari PRIVATE
  $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-fno-stack-protector -U_FORTIFY_SOURCE -ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=.>
  $<$<C_COMPILER_ID:MSVC>:/GS- /Zl>)
muhtari_warnings(muhtari)

# The library version reaches the core through a private generated header;
# the public header carries no version macros.
configure_file(cmake/version.h.in ${PROJECT_BINARY_DIR}/generated/muhtari_version.h @ONLY)
target_include_directories(muhtari PRIVATE ${PROJECT_BINARY_DIR}/generated)

if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

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
  runner/crc32.c
  runner/png.c
  runner/sha256.c)
target_link_libraries(muhtari-run PRIVATE muhtari::muhtari muhtari_host)
target_compile_definitions(muhtari-run PRIVATE _POSIX_C_SOURCE=200809L)
set_target_properties(muhtari-run PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/runner)
muhtari_warnings(muhtari-run)

# Tests. MUHTARI_TIMEOUT_SCALE multiplies every test's timeout on slow lanes;
# MUHTARI_INVENTORY names the tests/inventory/<name>.txt the registered tests
# must equal.
set(MUHTARI_TIMEOUT_SCALE 1 CACHE STRING "Multiplier for every test timeout")
set(MUHTARI_INVENTORY ci CACHE STRING "Test inventory file under tests/inventory")
enable_testing()
include(cmake/MuhtariTests.cmake)
include(tests/tests.cmake)
