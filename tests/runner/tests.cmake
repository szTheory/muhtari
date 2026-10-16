# muhtari-run tests.
#
# This file is also the runner.hash check itself, run as
#   cmake -DRUN=<muhtari-run> -P tests/runner/tests.cmake
# Fault prevented: the runner's printed hash, the line hosts and CI compare,
# drifts from the pinned test card or exits non-zero, while the library test
# still passes.
if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
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
