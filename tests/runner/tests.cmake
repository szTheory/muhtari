# muhtari-run tests.
#
# This file is also the command-line checks themselves, run as
#   cmake -DRUN=<muhtari-run> -DCASE=<name> [-DOUT=<file>] [-DVERSION_FILE=<file>]
#     [-DFRAMES=<long run>]
#     -P tests/runner/tests.cmake
if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
  set(card "9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4")

  # Runs muhtari-run with ARGN; fails unless it exits 0 with stdout want.
  function(expect_output want)
    execute_process(COMMAND "${RUN}" ${ARGN}
      OUTPUT_VARIABLE out ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0" OR NOT out STREQUAL want)
      message(FATAL_ERROR "muhtari-run ${ARGN}: expected exit 0 and output\n${want}"
        "got exit ${rc} and output\n${out}${err}")
    endif()
  endfunction()

  # Runs muhtari-run with ARGN through expect_fail.cmake: exit code and
  # output matching regex, or the test fails.
  function(expect_exit code regex)
    set(CMD "${RUN}" ${ARGN})
    set(CODE ${code})
    set(REGEX "${regex}")
    include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cmake/expect_fail.cmake)
  endfunction()

  set(usage_line "usage: muhtari-run ")
  if(CASE STREQUAL "hash")
    # Fault prevented: the runner's printed hash, the line hosts and CI
    # compare, drifts from the pinned test card or exits non-zero, while the
    # library test still passes.
    expect_output("frame 1 sha256 ${card}\n" --frames 1 --hash-frame 1)
  elseif(CASE STREQUAL "hash.repeat")
    # Fault prevented: repeated or unordered --hash-frame options print
    # duplicate or out-of-order lines, so a host comparing line by line
    # sees a mismatch.
    expect_output("frame 1 sha256 ${card}\nframe ${FRAMES} sha256 ${card}\n"
      --frames ${FRAMES} --hash-frame ${FRAMES} --hash-frame 1 --hash-frame 1)
  elseif(CASE STREQUAL "dump_png")
    # Fault prevented: the PNG the runner dumps, the image compared with
    # RetroArch's screenshot, changes bytes (palette, layout or writer) while
    # the native-frame hash still passes. The pin is the PNG of the pinned
    # test card through the warm NTSC palette.
    set(want_sha256 "1c0b16cb9c92d70869d3aee5cd581627f8902cb528e90f1f1d4252d9288ac956")
    file(REMOVE "${OUT}")
    expect_output("" --frames 1 --dump-frame "1:${OUT}")
    file(SIZE "${OUT}" size)
    file(SHA256 "${OUT}" sha256)
    if(NOT size STREQUAL "232645" OR NOT sha256 STREQUAL want_sha256)
      message(FATAL_ERROR "expected 232645 bytes with SHA-256 ${want_sha256}\n"
        "got ${size} bytes with SHA-256 ${sha256}")
    endif()
  elseif(CASE STREQUAL "version")
    # Fault prevented: --version reports a version other than the release's.
    file(STRINGS "${VERSION_FILE}" version LIMIT_COUNT 1)
    expect_output("muhtari-run ${version}\n" --version)
  # Fault prevented for the usage cases: an invalid command line runs
  # anyway, or fails with a status other than 2, so a script cannot tell a
  # typo from a failed run.
  elseif(CASE STREQUAL "usage.no_args")
    expect_exit(2 "${usage_line}")
  elseif(CASE STREQUAL "usage.unknown")
    expect_exit(2 "unknown option.*${usage_line}" --nope)
  elseif(CASE STREQUAL "usage.positional")
    expect_exit(2 "unexpected argument.*${usage_line}" card.a78)
  elseif(CASE STREQUAL "usage.frame_range")
    expect_exit(2 "${usage_line}" --frames 0)
    expect_exit(2 "${usage_line}" --frames 1 --hash-frame 0)
    expect_exit(2 "beyond --frames.*${usage_line}" --frames 5 --hash-frame 6)
  elseif(CASE STREQUAL "usage.dump_syntax")
    expect_exit(2 "N:FILE.*${usage_line}" --frames 1 --dump-frame 1)
    expect_exit(2 "N:FILE.*${usage_line}" --frames 1 --dump-frame x:f)
  elseif(CASE STREQUAL "exit.failure")
    # Fault prevented: a file the runner cannot write is reported as success.
    expect_exit(1 "cannot open" --frames 1 --dump-frame "1:${OUT}/missing/x.png")
  else()
    message(FATAL_ERROR "unknown CASE '${CASE}'")
  endif()
  return()
endif()

set(runner_check ${CMAKE_COMMAND} -DRUN=$<TARGET_FILE:muhtari-run>)
foreach(case IN ITEMS hash hash.repeat usage.no_args usage.unknown usage.positional
    usage.frame_range usage.dump_syntax)
  muhtari_add_test(NAME runner.${case}
    COMMAND ${runner_check} -DCASE=${case} -DFRAMES=${MUHTARI_LONG_RUN} -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS runner portable TIMEOUT 10)
endforeach()
muhtari_add_test(NAME runner.dump_png
  COMMAND ${runner_check} -DCASE=dump_png -DOUT=${PROJECT_BINARY_DIR}/tests/dump.png
    -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 10)
muhtari_add_test(NAME runner.exit.failure
  COMMAND ${runner_check} -DCASE=exit.failure -DOUT=${PROJECT_BINARY_DIR}/tests
    -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 10)
muhtari_add_test(NAME runner.version
  COMMAND ${runner_check} -DCASE=version -DVERSION_FILE=${PROJECT_SOURCE_DIR}/version.txt
    -P ${CMAKE_CURRENT_LIST_FILE}
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

# The bench statistics and the stop flag.
add_executable(muhtari-test-runner
  ${CMAKE_CURRENT_LIST_DIR}/test_runner.c
  ${PROJECT_SOURCE_DIR}/runner/bench.c
  ${PROJECT_SOURCE_DIR}/runner/stop.c)
target_compile_definitions(muhtari-test-runner PRIVATE _POSIX_C_SOURCE=200809L)
set_target_properties(muhtari-test-runner PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-runner)
muhtari_add_test(NAME runner.bench.percentile COMMAND muhtari-test-runner percentile
  LABELS runner bench portable TIMEOUT 10)
muhtari_add_test(NAME runner.bench.format COMMAND muhtari-test-runner format
  LABELS runner bench portable TIMEOUT 10)
muhtari_add_test(NAME runner.stop COMMAND muhtari-test-runner stop
  LABELS runner portable TIMEOUT 10)

# Not portable: it reads docs/runner.md from the source tree.
muhtari_add_test(NAME runner.exit.table
  COMMAND ${runner_check} -DDOC=${PROJECT_SOURCE_DIR}/docs/runner.md
    -P ${PROJECT_SOURCE_DIR}/tests/cmake/exit_table.cmake
  LABELS runner TIMEOUT 10)
