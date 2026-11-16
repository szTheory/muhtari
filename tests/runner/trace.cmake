# Focused runner trace acceptance. This file is included after runner targets
# exist and is also executed in script mode by CTest.
if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
  function(trace_fixture)
    file(MAKE_DIRECTORY "${DIR}")
    execute_process(COMMAND ${EMULATOR} "${TRACE_TEST}" fixture "${DIR}"
      RESULT_VARIABLE generated OUTPUT_VARIABLE generated_out ERROR_VARIABLE generated_err
      TIMEOUT 20)
    if(NOT generated STREQUAL "0" OR NOT EXISTS "${DIR}/trace.a78")
      message(FATAL_ERROR "generated trace cartridge failed: ${generated}\n${generated_out}${generated_err}")
    endif()
  endfunction()

  function(trace_verify name path)
    execute_process(COMMAND ${EMULATOR} "${TRACE_TEST}" filter "${name}" "${path}"
      RESULT_VARIABLE verified OUTPUT_VARIABLE verify_out ERROR_VARIABLE verify_err TIMEOUT 20)
    if(NOT verified STREQUAL "0")
      message(FATAL_ERROR "trace filter ${name} failed: ${verified}\n${verify_out}${verify_err}")
    endif()
  endfunction()

  function(trace_run name frames frame range limit)
    set(path "${DIR}/${name}.jsonl")
    file(REMOVE "${path}")
    set(command ${EMULATOR} "${RUN}" --frames "${frames}" --trace "${path}"
      --trace-frame "${frame}" --trace-range "${range}")
    if(NOT "${limit}" STREQUAL "")
      list(APPEND command --trace-limit "${limit}")
    endif()
    list(APPEND command "${DIR}/trace.a78")
    execute_process(COMMAND ${command} RESULT_VARIABLE rc OUTPUT_VARIABLE output
      ERROR_VARIABLE err TIMEOUT 30)
    if(NOT rc STREQUAL "0" OR NOT EXISTS "${path}")
      message(FATAL_ERROR "trace run ${name} failed: ${rc}\n${output}${err}")
    endif()
    trace_verify("${name}" "${path}")
  endfunction()

  function(trace_expect_oracle_rejected name path)
    execute_process(COMMAND ${EMULATOR} "${TRACE_TEST}" oracle "${path}"
      RESULT_VARIABLE rejected OUTPUT_VARIABLE rejected_out ERROR_VARIABLE rejected_err TIMEOUT 20)
    if(rejected STREQUAL "0")
      message(FATAL_ERROR "oracle accepted corrupted trace ${name}:\n${rejected_out}${rejected_err}")
    endif()
  endfunction()

  if(CASE STREQUAL "smoke")
    file(MAKE_DIRECTORY "${DIR}")
    execute_process(COMMAND ${EMULATOR} "${SAVE_TEST}" "${DIR}" roundtrip
      RESULT_VARIABLE generated OUTPUT_VARIABLE generated_out ERROR_VARIABLE generated_err
      TIMEOUT 20)
    if(NOT generated STREQUAL "0")
      message(FATAL_ERROR "trace cartridge generation failed: ${generated}\n${generated_out}${generated_err}")
    endif()
    set(trace "${DIR}/frame.jsonl")
    file(REMOVE "${trace}")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --hash-frame 1 --hash-state 1
        "${DIR}/game.a78"
      RESULT_VARIABLE baseline_rc OUTPUT_VARIABLE baseline ERROR_VARIABLE baseline_err TIMEOUT 20)
    if(NOT baseline_rc STREQUAL "0")
      message(FATAL_ERROR "baseline runner invocation failed: ${baseline_rc}\n${baseline}${baseline_err}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --trace "${trace}"
        --trace-frame 1 --trace-range 0:65535 --hash-frame 1 --hash-state 1 "${DIR}/game.a78"
      RESULT_VARIABLE rc OUTPUT_VARIABLE output ERROR_VARIABLE err TIMEOUT 20)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "muhtari-run rejected valid trace options: ${rc}\n${output}${err}")
    endif()
    if(NOT output STREQUAL baseline)
      message(FATAL_ERROR "trace changed ordinary stdout or hashes:\nbaseline:\n${baseline}\ntraced:\n${output}")
    endif()
    if(NOT EXISTS "${trace}")
      message(FATAL_ERROR "muhtari-run succeeded without publishing trace JSONL")
    endif()
    file(READ "${trace}" records)
    string(FIND "${records}" "\"format\":\"muhtari-trace\"" has_header)
    string(FIND "${records}" "\"kind\":0" has_instruction)
    string(FIND "${records}" "\"kind\":1" has_bus)
    string(FIND "${records}" "\"status\":\"complete\"" has_complete)
    if(has_header EQUAL -1 OR has_instruction EQUAL -1 OR has_bus EQUAL -1 OR has_complete EQUAL -1)
      message(FATAL_ERROR "trace JSONL is missing its versioned header, CPU records or complete status")
    endif()
  elseif(CASE STREQUAL "oracle")
    trace_fixture()
    set(path "${DIR}/oracle.jsonl")
    file(REMOVE "${path}")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --hash-frame 1 --hash-state 1
        "${DIR}/trace.a78"
      RESULT_VARIABLE baseline_rc OUTPUT_VARIABLE baseline ERROR_VARIABLE baseline_err TIMEOUT 20)
    if(NOT baseline_rc STREQUAL "0")
      message(FATAL_ERROR "oracle baseline failed: ${baseline_rc}\n${baseline}${baseline_err}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --trace "${path}"
        --trace-frame 1 --trace-range 0:65535 --hash-frame 1 --hash-state 1 "${DIR}/trace.a78"
      RESULT_VARIABLE rc OUTPUT_VARIABLE output ERROR_VARIABLE err TIMEOUT 30)
    if(NOT rc STREQUAL "0" OR NOT output STREQUAL baseline)
      message(FATAL_ERROR "actual-runner oracle invocation differed: ${rc}\n${output}${err}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${TRACE_TEST}" oracle "${path}"
      RESULT_VARIABLE verified OUTPUT_VARIABLE verify_out ERROR_VARIABLE verify_err TIMEOUT 20)
    if(NOT verified STREQUAL "0")
      message(FATAL_ERROR "independent numeric oracle failed: ${verified}\n${verify_out}${verify_err}")
    endif()

    file(READ "${path}" records)
    set(corrupt "${DIR}/wrong-tick.jsonl")
    string(REPLACE "\"tick\":108988" "\"tick\":108989" changed "${records}")
    file(WRITE "${corrupt}" "${changed}")
    trace_expect_oracle_rejected("wrong tick" "${corrupt}")

    file(STRINGS "${path}" rows)
    list(GET rows 1 first)
    list(GET rows 2 second)
    list(REMOVE_AT rows 1 2)
    list(INSERT rows 1 "${second}" "${first}")
    string(JOIN "\n" changed ${rows})
    set(corrupt "${DIR}/swapped-tie.jsonl")
    file(WRITE "${corrupt}" "${changed}\n")
    trace_expect_oracle_rejected("swapped same-tick records" "${corrupt}")

    file(STRINGS "${path}" rows)
    list(GET rows 1 first)
    string(REPLACE "\"pc\":61440,\"a\":0" "\"pc\":61440,\"a\":1" changed "${first}")
    list(REMOVE_AT rows 1)
    list(INSERT rows 1 "${changed}")
    string(JOIN "\n" changed ${rows})
    set(corrupt "${DIR}/wrong-register.jsonl")
    file(WRITE "${corrupt}" "${changed}\n")
    trace_expect_oracle_rejected("wrong CPU register" "${corrupt}")
  elseif(CASE STREQUAL "filters")
    trace_fixture()
    trace_run(pc 1 1 0xF000:0xF000 "")
    set(pc_trace "${DIR}/pc.jsonl")
    file(STRINGS "${pc_trace}" rows)
    list(GET rows 2 bus)
    string(REPLACE "\"addr\":61440" "\"addr\":61441" changed "${bus}")
    list(REMOVE_AT rows 2)
    list(INSERT rows 2 "${changed}")
    string(JOIN "\n" changed ${rows})
    set(corrupt "${DIR}/out-of-range-bus.jsonl")
    file(WRITE "${corrupt}" "${changed}\n")
    execute_process(COMMAND ${EMULATOR} "${TRACE_TEST}" filter pc "${corrupt}"
      RESULT_VARIABLE rejected OUTPUT_VARIABLE rejected_out ERROR_VARIABLE rejected_err TIMEOUT 20)
    if(rejected STREQUAL "0")
      message(FATAL_ERROR "PC-only range oracle accepted a bus event outside its inclusive range:\n${rejected_out}${rejected_err}")
    endif()
    trace_run(ram 1 1 6144:0x1800 "")
    trace_run(zero 1 1 0:0 "")
    trace_run(ffff 1 1 0xFFFF:65535 "")
    trace_run(empty 1 1 1:1 "")
    trace_run(truncated 1 1 0:65535 1)
    trace_run(frame2 2 2 0:65535 "")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --trace-frame 1
        "${DIR}/trace.a78"
      RESULT_VARIABLE rc OUTPUT_VARIABLE output ERROR_VARIABLE err TIMEOUT 10)
    if(NOT rc STREQUAL "2")
      message(FATAL_ERROR "trace selector without --trace must be a usage error: ${rc}\n${output}${err}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --trace "${DIR}/bad.jsonl"
        --trace-frame 1 "${DIR}/trace.a78"
      RESULT_VARIABLE rc OUTPUT_VARIABLE output ERROR_VARIABLE err TIMEOUT 10)
    if(NOT rc STREQUAL "2")
      message(FATAL_ERROR "trace without an explicit address range must be a usage error: ${rc}\n${output}${err}")
    endif()
  elseif(CASE STREQUAL "limits")
    trace_fixture()
    trace_run(exact 1 1 0:65535 2731)
    trace_run(one_extra 1 1 0:65535 2730)
    trace_run(nonmatching_after_limit 1 1 0xF000:0xF000 2)

    set(trace "${DIR}/truncated-then-continue.jsonl")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 2 --hash-frame 1 --hash-frame 2
        --hash-state 1 --hash-state 2 --trace "${trace}" --trace-frame 1
        --trace-range 0:65535 --trace-limit 2730 "${DIR}/trace.a78"
      RESULT_VARIABLE traced_rc OUTPUT_VARIABLE traced ERROR_VARIABLE traced_err TIMEOUT 30)
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 2 --hash-frame 1 --hash-frame 2
        --hash-state 1 --hash-state 2 "${DIR}/trace.a78"
      RESULT_VARIABLE plain_rc OUTPUT_VARIABLE plain ERROR_VARIABLE plain_err TIMEOUT 30)
    if(NOT plain_rc STREQUAL "0" OR NOT traced_rc STREQUAL "0" OR NOT traced STREQUAL plain)
      message(FATAL_ERROR "truncated capture changed later frame execution or hashes:\n"
        "plain ${plain_rc}: ${plain}${plain_err}\ntraced ${traced_rc}: ${traced}${traced_err}")
    endif()
    trace_verify(one_extra "${trace}")
  elseif(CASE STREQUAL "bounds")
    file(MAKE_DIRECTORY "${DIR}")
    execute_process(COMMAND ${EMULATOR} "${TRACE_TEST}" busy_fixture "${DIR}"
      RESULT_VARIABLE generated OUTPUT_VARIABLE generated_out ERROR_VARIABLE generated_err
      TIMEOUT 20)
    if(NOT generated STREQUAL "0" OR NOT EXISTS "${DIR}/trace-busy.a78")
      message(FATAL_ERROR "busy trace cartridge generation failed: ${generated}\\n${generated_out}${generated_err}")
    endif()
    foreach(region IN ITEMS ntsc pal)
      foreach(mode IN ITEMS default maximum)
        execute_process(COMMAND ${EMULATOR} "${SAVE_TEST}" "${DIR}" trace_measure
            "${DIR}/trace-busy.a78" "${region}" "${mode}"
          RESULT_VARIABLE rc OUTPUT_VARIABLE output ERROR_VARIABLE err TIMEOUT 30)
        if(NOT rc STREQUAL "0" OR NOT output MATCHES "serialization_write_ns=([0-9]+)")
          message(FATAL_ERROR "${region} ${mode} busy trace timing failed: ${rc}\\n${output}${err}")
        endif()
        set(serialization_ns "${CMAKE_MATCH_1}")
        execute_process(COMMAND ${EMULATOR} "${TRACE_TEST}" measure
            "${DIR}/trace-${region}-${mode}.jsonl"
          RESULT_VARIABLE measured OUTPUT_VARIABLE metrics ERROR_VARIABLE measure_err TIMEOUT 20)
        if(NOT measured STREQUAL "0" OR NOT metrics MATCHES
            "^records=([0-9]+) status=complete limit=([0-9]+) event_size=([0-9]+) capture_bytes=([0-9]+) max_line=([0-9]+) output_bytes=([0-9]+)")
          message(FATAL_ERROR "${region} ${mode} bounds evidence invalid: ${measured}\\n${metrics}${measure_err}")
        endif()
        set(records "${CMAKE_MATCH_1}")
        set(limit "${CMAKE_MATCH_2}")
        set(event_size "${CMAKE_MATCH_3}")
        set(capture_bytes "${CMAKE_MATCH_4}")
        set(max_line "${CMAKE_MATCH_5}")
        set(output_bytes "${CMAKE_MATCH_6}")
        if(records LESS 40000 OR NOT event_size EQUAL 24 OR max_line GREATER 149 OR
            output_bytes GREATER 19923136)
          message(FATAL_ERROR "${region} ${mode} capture/output exceeds its measured bounds: ${metrics}")
        endif()
        if(mode STREQUAL "default" AND
            (NOT limit EQUAL 65536 OR NOT capture_bytes EQUAL 1572864))
          message(FATAL_ERROR "${region} default allocation does not match 65,536 x 24 bytes: ${metrics}")
        elseif(mode STREQUAL "maximum" AND
            (NOT limit EQUAL 131072 OR NOT capture_bytes EQUAL 3145728))
          message(FATAL_ERROR "${region} hard allocation does not match 131,072 x 24 bytes: ${metrics}")
        endif()
        message(STATUS "${region} ${mode}: ${metrics} serialization_write_ns=${serialization_ns}")
      endforeach()
    endforeach()
  else()
    message(FATAL_ERROR "unknown trace CASE '${CASE}'")
  endif()
  return()
endif()

add_executable(muhtari-test-trace ${CMAKE_CURRENT_LIST_DIR}/test_trace.c)
target_include_directories(muhtari-test-trace PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-trace PRIVATE muhtari::muhtari muhtari_host)
set_target_properties(muhtari-test-trace PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-trace)

string(REPLACE ";" "$<SEMICOLON>" trace_emulator "${CMAKE_CROSSCOMPILING_EMULATOR}")
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/tests/trace-smoke")
muhtari_add_test(NAME runner.trace.smoke
  COMMAND ${CMAKE_COMMAND} "-DEMULATOR=${trace_emulator}"
    "-DSAVE_TEST=$<TARGET_FILE:muhtari-test-save-runner>" "-DRUN=$<TARGET_FILE:muhtari-run>"
    "-DDIR=${PROJECT_BINARY_DIR}/tests/trace-smoke" -DCASE=smoke -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 30)
foreach(case IN ITEMS oracle filters limits bounds)
  file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/tests/trace-${case}")
  muhtari_add_test(NAME runner.trace.${case}
    COMMAND ${CMAKE_COMMAND} "-DEMULATOR=${trace_emulator}"
      "-DTRACE_TEST=$<TARGET_FILE:muhtari-test-trace>" "-DRUN=$<TARGET_FILE:muhtari-run>"
      "-DSAVE_TEST=$<TARGET_FILE:muhtari-test-save-runner>"
      "-DDIR=${PROJECT_BINARY_DIR}/tests/trace-${case}" "-DCASE=${case}"
      -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS runner portable TIMEOUT 60)
endforeach()
muhtari_add_test(NAME core.trace.edges COMMAND muhtari-test-trace edges
  LABELS core portable TIMEOUT 20)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests/trace-memory)
muhtari_add_test(NAME runner.trace.memory COMMAND muhtari-test-save-runner
  ${PROJECT_BINARY_DIR}/tests/trace-memory trace_bounds LABELS runner portable TIMEOUT 30)
