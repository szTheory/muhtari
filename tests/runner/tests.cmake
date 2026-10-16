# muhtari-run tests.
#
# This file is also the runner.hash and runner.dump_png checks themselves,
# run as
#   cmake -DRUN=<muhtari-run> -P tests/runner/tests.cmake
#   cmake -DRUN=<muhtari-run> -DOUT=<file.png> -P tests/runner/tests.cmake
if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
  if(DEFINED OUT)
    # runner.dump_png. Fault prevented: the PNG the runner dumps, the image
    # compared with RetroArch's screenshot, changes bytes (palette, layout or
    # writer) while the native-frame hash still passes. The pin is the PNG of
    # the pinned test card through the warm NTSC palette.
    set(want_sha256 "1c0b16cb9c92d70869d3aee5cd581627f8902cb528e90f1f1d4252d9288ac956")
    file(REMOVE "${OUT}")
    execute_process(COMMAND "${RUN}" --frames 1 --dump-frame "1:${OUT}"
      OUTPUT_VARIABLE out ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0" OR NOT out STREQUAL "" OR NOT EXISTS "${OUT}")
      message(FATAL_ERROR "expected exit 0, no output and ${OUT}; got exit ${rc} and\n${out}${err}")
    endif()
    file(SIZE "${OUT}" size)
    file(SHA256 "${OUT}" sha256)
    if(NOT size STREQUAL "232645" OR NOT sha256 STREQUAL want_sha256)
      message(FATAL_ERROR "expected 232645 bytes with SHA-256 ${want_sha256}\n"
        "got ${size} bytes with SHA-256 ${sha256}")
    endif()
    return()
  endif()

  # runner.hash. Fault prevented: the runner's printed hash, the line hosts
  # and CI compare, drifts from the pinned test card or exits non-zero, while
  # the library test still passes.
  execute_process(COMMAND "${RUN}" --frames 1 --hash-frame 1
    OUTPUT_VARIABLE out ERROR_VARIABLE err RESULT_VARIABLE rc)
  set(want "frame 1 sha256 9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4\n")
  if(NOT rc STREQUAL "0" OR NOT out STREQUAL want)
    message(FATAL_ERROR "expected exit 0 and output\n${want}got exit ${rc} and output\n${out}${err}")
  endif()
  return()
endif()

muhtari_add_test(NAME runner.hash
  COMMAND ${CMAKE_COMMAND} -DRUN=$<TARGET_FILE:muhtari-run> -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 10)
muhtari_add_test(NAME runner.dump_png
  COMMAND ${CMAKE_COMMAND} -DRUN=$<TARGET_FILE:muhtari-run>
    -DOUT=${PROJECT_BINARY_DIR}/tests/dump.png -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 10)

# The PNG writer and its checksums, checked by an independent parse.
add_executable(muhtari-test-png
  ${CMAKE_CURRENT_LIST_DIR}/test_png.c
  ${PROJECT_SOURCE_DIR}/runner/png.c
  ${PROJECT_SOURCE_DIR}/runner/crc32.c)
target_link_libraries(muhtari-test-png PRIVATE muhtari::muhtari muhtari_host)
set_target_properties(muhtari-test-png PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-png)
muhtari_add_test(NAME runner.png
  COMMAND muhtari-test-png ${PROJECT_BINARY_DIR}/tests/png_test.png
  LABELS runner portable TIMEOUT 10)
