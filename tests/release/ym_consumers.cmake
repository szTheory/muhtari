cmake_minimum_required(VERSION 3.25)
include("${ROOT}/tests/emscripten/bundle.cmake")
_muhtari_bundle_normalize("${ROOT}" "${BUILD}" "${WORK}" ROOT BUILD WORK)
file(MAKE_DIRECTORY "${WORK}")
_muhtari_bundle_reject_tree_symlinks("${WORK}" "native package scratch")
set(lib lib/libmuhtari.a)
set(exe bin/muhtari-run)
set(core cores/muhtari_libretro.so)
set(suffix "")
if(SUFFIX MATCHES "^macos-")
  set(core cores/muhtari_libretro.dylib)
elseif(SUFFIX MATCHES "^windows-")
  set(lib lib/muhtari.lib)
  set(exe bin/muhtari-run.exe)
  set(core cores/muhtari_libretro.dll)
  set(suffix .exe)
endif()
set(library_files LICENSE THIRD-PARTY-NOTICES.md include/muhtari.h ${lib}
  lib/pkgconfig/muhtari.pc lib/cmake/muhtari/muhtariConfig.cmake
  lib/cmake/muhtari/muhtariConfigVersion.cmake lib/cmake/muhtari/muhtariTargets.cmake
  lib/cmake/muhtari/muhtariTargets-release.cmake)
set(runner_files LICENSE THIRD-PARTY-NOTICES.md ${exe})
set(libretro_files LICENSE THIRD-PARTY-NOTICES.md ${core} info/muhtari_libretro.info)
foreach(component library runner libretro)
  list(SORT ${component}_files)
  set(archive "${BUILD}/muhtari-${component}-${SUFFIX}.zip")
  _muhtari_bundle_preflight("${archive}" "${${component}_files}" TRUE)
  set(extract "${WORK}/${component}")
  file(REMOVE_RECURSE "${extract}")
  file(MAKE_DIRECTORY "${extract}")
  file(ARCHIVE_EXTRACT INPUT "${archive}" DESTINATION "${extract}")
  foreach(notice LICENSE THIRD-PARTY-NOTICES.md)
    _muhtari_bundle_hash_equal("${ROOT}/${notice}" "${extract}/${notice}" "native ${notice}")
  endforeach()
endforeach()
# Bind actual shipped binary/header bytes to this product snapshot before use.
_muhtari_bundle_hash_equal("${ROOT}/include/muhtari.h" "${WORK}/library/include/muhtari.h" "public header")
foreach(component library runner libretro)
  if(component STREQUAL library)
    set(binary "${lib}")
  elseif(component STREQUAL runner)
    set(binary "${exe}")
  else()
    set(binary "${core}")
  endif()
  file(GLOB stages "${BUILD}/_CPack_Packages/*/ZIP/*/${component}/${binary}")
  list(LENGTH stages count)
  if(NOT count EQUAL 1)
    message(FATAL_ERROR "exact current CPack staged ${component} is missing or ambiguous")
  endif()
  list(GET stages 0 stage)
  _muhtari_bundle_hash_equal("${stage}" "${WORK}/${component}/${binary}" "${component} package source")
endforeach()
get_filename_component(corename "${core}" NAME)
# Unix install may rewrite the archive index. Compare every object payload
# against the current library as well; index timestamps carry no code.
if(NOT SUFFIX MATCHES "^windows-")
  execute_process(COMMAND "${AR}" t "${BUILD}/libmuhtari.a"
    RESULT_VARIABLE rc OUTPUT_VARIABLE members ERROR_VARIABLE err)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "current archive member listing failed: ${err}")
  endif()
  execute_process(COMMAND "${AR}" t "${WORK}/library/${lib}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE packaged_members ERROR_VARIABLE err)
  if(NOT rc STREQUAL "0" OR NOT members STREQUAL packaged_members)
    message(FATAL_ERROR "packaged archive member set differs")
  endif()
  string(REGEX MATCHALL "[^\n]+" members "${members}")
  foreach(member IN LISTS members)
    foreach(side source package)
      set(archive "${BUILD}/libmuhtari.a")
      if(side STREQUAL package)
        set(archive "${WORK}/library/${lib}")
      endif()
      execute_process(COMMAND "${AR}" p "${archive}" "${member}"
        OUTPUT_FILE "${WORK}/${side}-object" RESULT_VARIABLE rc)
      if(NOT rc STREQUAL "0")
        message(FATAL_ERROR "archive object extraction failed")
      endif()
    endforeach()
    _muhtari_bundle_hash_equal("${WORK}/source-object" "${WORK}/package-object" "archive object ${member}")
  endforeach()
endif()
set(quickstart_build "${WORK}/quickstart-cmake")
file(REMOVE_RECURSE "${quickstart_build}")
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${ROOT}/examples/embedding" -B "${quickstart_build}"
  "-DCMAKE_C_COMPILER=${CC}" -DCMAKE_BUILD_TYPE=Release
  "-DCMAKE_PREFIX_PATH=${WORK}/library" -DCMAKE_FIND_USE_PACKAGE_REGISTRY=OFF
  -DCMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY=OFF
  "-Dmuhtari_DIR=${WORK}/library/lib/cmake/muhtari"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "extracted quickstart CMake configure: ${rc} ${out}${err}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${quickstart_build}" --config Release --parallel 2
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "extracted quickstart CMake build: ${rc} ${out}${err}")
endif()
set(quickstart "${quickstart_build}/quickstart${suffix}")
if(NOT EXISTS "${quickstart}")
  set(quickstart "${quickstart_build}/Release/quickstart${suffix}")
endif()
execute_process(COMMAND ${EMULATOR} "${quickstart}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
string(STRIP "${out}" quickstart_output)
if(NOT rc STREQUAL "0" OR NOT quickstart_output STREQUAL "quickstart: one frame, 800 samples")
  message(FATAL_ERROR "extracted quickstart CMake run: ${rc}; expected one frame, 800 samples; got ${out}${err}")
endif()
message(STATUS "extracted quickstart CMake consumer passed: ${quickstart_output}")
# Fault prevented: a release ZIP can contain the right library bytes while
# omitting the exported target required by a clean CMake host project.
set(damaged_parent "${WORK}/quickstart-damaged")
file(REMOVE_RECURSE "${damaged_parent}")
file(MAKE_DIRECTORY "${damaged_parent}")
file(COPY "${WORK}/library" DESTINATION "${damaged_parent}")
set(damaged_prefix "${damaged_parent}/library")
set(damaged_target "${damaged_prefix}/lib/cmake/muhtari/muhtariTargets.cmake")
if(NOT EXISTS "${damaged_target}")
  message(FATAL_ERROR "quickstart damaged-prefix setup is missing exported target: ${damaged_target}")
endif()
file(REMOVE "${damaged_target}")
set(damaged_build "${WORK}/quickstart-damaged-build")
file(REMOVE_RECURSE "${damaged_build}")
set(damaged_configure_command "${CMAKE_COMMAND}" -S "${ROOT}/examples/embedding" -B "${damaged_build}"
  "-DCMAKE_C_COMPILER=${CC}" -DCMAKE_BUILD_TYPE=Release
  "-DCMAKE_PREFIX_PATH=${damaged_prefix}" -DCMAKE_FIND_USE_PACKAGE_REGISTRY=OFF
  -DCMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY=OFF
  "-Dmuhtari_DIR=${damaged_prefix}/lib/cmake/muhtari")
set(damaged_regex "include could not find requested file:[ \t\r\n]+[^ \t\r\n]*[/\\\\]muhtariTargets\\.cmake")
execute_process(COMMAND "${CMAKE_COMMAND}" "-DCMD=${damaged_configure_command}" -DCODE=1
    "-DREGEX=${damaged_regex}" -P "${ROOT}/tests/cmake/expect_fail.cmake"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "quickstart missing-target control was not accepted: ${rc} ${out}${err}")
endif()
message(STATUS "quickstart missing-target control passed: ${out}${err}")
if("$ENV{MUHTARI_RELEASE_QUICKSTART_PKG_CONFIG}" STREQUAL "1")
  if(NOT SUFFIX MATCHES "^(linux|macos)-(x64|arm64)$")
    message(FATAL_ERROR "extracted quickstart pkg-config route requires a POSIX native compiler lane: ${SUFFIX}")
  endif()
  set(pkg_config_metadata "${WORK}/library/lib/pkgconfig/muhtari.pc")
  if(NOT EXISTS "${pkg_config_metadata}")
    message(FATAL_ERROR "extracted quickstart pkg-config metadata is missing: ${pkg_config_metadata}")
  endif()
  find_program(PKG_CONFIG_EXECUTABLE NAMES pkg-config)
  if(NOT PKG_CONFIG_EXECUTABLE)
    message(FATAL_ERROR "extracted quickstart pkg-config route requires pkg-config, but it was not found")
  endif()
  execute_process(COMMAND "${CMAKE_COMMAND}" -E env
      "PKG_CONFIG_LIBDIR=${WORK}/library/lib/pkgconfig" "PKG_CONFIG_PATH="
      "${PKG_CONFIG_EXECUTABLE}" --cflags --libs muhtari
    RESULT_VARIABLE rc OUTPUT_VARIABLE pkg_config_flags ERROR_VARIABLE err)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "extracted quickstart pkg-config metadata lookup failed: ${rc} ${pkg_config_flags}${err}")
  endif()
  if(pkg_config_flags STREQUAL "")
    message(FATAL_ERROR "extracted quickstart pkg-config metadata lookup returned no flags")
  endif()
  separate_arguments(pkg_config_arguments UNIX_COMMAND "${pkg_config_flags}")
  if(NOT pkg_config_arguments)
    message(FATAL_ERROR "extracted quickstart pkg-config metadata produced no compiler arguments")
  endif()
  set(pkg_config_executable "${WORK}/quickstart-pkg-config${suffix}")
  execute_process(COMMAND "${CC}" -std=c17 "${ROOT}/examples/embedding/quickstart.c"
      ${pkg_config_arguments} -o "${pkg_config_executable}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "extracted quickstart pkg-config compile failed: ${rc} ${out}${err}")
  endif()
  execute_process(COMMAND "${pkg_config_executable}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
  string(STRIP "${out}" pkg_config_output)
  if(NOT rc STREQUAL "0" OR NOT pkg_config_output STREQUAL "quickstart: one frame, 800 samples")
    message(FATAL_ERROR "extracted quickstart pkg-config run: ${rc}; expected one frame, 800 samples; got ${out}${err}")
  endif()
  message(STATUS "extracted quickstart pkg-config consumer passed: ${pkg_config_output}")
endif()
set(fixture "${BUILD}/tests/ym2151/ym2151.a78")
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${ROOT}/tests/consumer" -B "${WORK}/consumer"
  "-DCMAKE_C_COMPILER=${CC}" "-DCMAKE_BUILD_TYPE=Release"
  "-DCMAKE_PREFIX_PATH=${WORK}/library" -DCMAKE_FIND_USE_PACKAGE_REGISTRY=OFF
  -DCMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY=OFF
  "-Dmuhtari_DIR=${WORK}/library/lib/cmake/muhtari"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "extracted library configure: ${rc} ${out}${err}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${WORK}/consumer" --config Release --parallel 2
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "extracted library build: ${rc} ${out}${err}")
endif()
# This script runs outside CTest, so cross-compiled consumers need the same
# emulator CTest normally prepends to executable test commands.
set(consumer "${WORK}/consumer/consumer${suffix}")
if(NOT EXISTS "${consumer}")
  set(consumer "${WORK}/consumer/Release/consumer${suffix}")
endif()
execute_process(COMMAND ${EMULATOR} "${consumer}" --ym-fixture "${fixture}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "extracted library YM: ${rc} ${out}${err}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" "-DRUN=${WORK}/runner/${exe}"
  "-DDRIVER=${BUILD}/tests/muhtari-test-ym2151${suffix}" "-DDIR=${BUILD}/tests/ym2151"
  "-DWORK=${WORK}/runner-check" "-DEMULATOR=${EMULATOR}" -DCASE=ym.public_runner
  -P "${ROOT}/tests/runner/tests.cmake" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "extracted runner YM: ${rc} ${out}${err}")
endif()
execute_process(COMMAND ${EMULATOR} "${BUILD}/tests/muhtari-test-libretro-host${suffix}"
  "${WORK}/libretro/${core}" ym "${fixture}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "extracted libretro YM: ${rc} ${out}${err}")
endif()
# Exercise this exact consumer's corrupt-content refusal and source binding.
file(READ "${fixture}" corrupt HEX LIMIT 16)
file(WRITE "${WORK}/truncated.a78" "${corrupt}")
execute_process(COMMAND ${EMULATOR} "${consumer}" --ym-fixture "${WORK}/truncated.a78"
  RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET)
if(rc STREQUAL "0")
  message(FATAL_ERROR "corrupt fixture accepted by extracted consumer")
endif()
file(WRITE "${WORK}/substituted-core" "substitute")
set(script "${WORK}/substitution.cmake")
file(WRITE "${script}" "include([==[${ROOT}/tests/emscripten/bundle.cmake]==])\n_muhtari_bundle_hash_equal([==[${BUILD}/libretro/${corename}]==] [==[${WORK}/substituted-core]==] [==[core source]==])\n")
execute_process(COMMAND "${CMAKE_COMMAND}" -P "${script}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(NOT rc STREQUAL "1" OR NOT "${out}${err}" MATCHES "core source differs")
  message(FATAL_ERROR "substituted core identity was not rejected")
endif()
message(STATUS "clean extracted library, runner and loaded core YM passed with source/content controls")
