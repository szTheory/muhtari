# Focused runner trace acceptance. This file is included after runner targets
# exist and is also executed in script mode by CTest.
if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
  if(NOT CASE STREQUAL "smoke")
    message(FATAL_ERROR "unknown trace CASE '${CASE}'")
  endif()
  file(MAKE_DIRECTORY "${DIR}")
  execute_process(COMMAND ${EMULATOR} "${SAVE_TEST}" "${DIR}" roundtrip
    RESULT_VARIABLE generated OUTPUT_VARIABLE generated_out ERROR_VARIABLE generated_err
    TIMEOUT 20)
  if(NOT generated STREQUAL "0")
    message(FATAL_ERROR "trace cartridge generation failed: ${generated}\n${generated_out}${generated_err}")
  endif()

  set(trace "${DIR}/frame.jsonl")
  file(REMOVE "${trace}")
  execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --trace "${trace}"
      --trace-frame 1 --trace-range 0:65535 --hash-frame 1 --hash-state 1 "${DIR}/game.a78"
    RESULT_VARIABLE rc OUTPUT_VARIABLE output ERROR_VARIABLE err TIMEOUT 20)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "muhtari-run rejected the valid trace smoke invocation: ${rc}\n${output}${err}")
  endif()
  if(NOT EXISTS "${trace}")
    message(FATAL_ERROR "muhtari-run succeeded without publishing the trace JSONL")
  endif()
  file(READ "${trace}" records)
  if(NOT records MATCHES "\\\"format\\\":\\\"muhtari-trace\\\"" OR
      NOT records MATCHES "\\\"status\\\":\\\"(complete|truncated)\\\"")
    message(FATAL_ERROR "trace JSONL is missing its versioned header or terminal status")
  endif()
  return()
endif()

string(REPLACE ";" "$<SEMICOLON>" trace_emulator "${CMAKE_CROSSCOMPILING_EMULATOR}")
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/tests/trace-smoke")
muhtari_add_test(NAME runner.trace.smoke
  COMMAND ${CMAKE_COMMAND} "-DEMULATOR=${trace_emulator}"
    "-DSAVE_TEST=$<TARGET_FILE:muhtari-test-save-runner>" "-DRUN=$<TARGET_FILE:muhtari-run>"
    "-DDIR=${PROJECT_BINARY_DIR}/tests/trace-smoke" -DCASE=smoke -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 30)
