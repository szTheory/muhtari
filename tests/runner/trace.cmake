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

  function(trace_expect_cli_usage name)
    execute_process(COMMAND ${EMULATOR} "${RUN}" ${ARGN} "${DIR}/trace.a78"
      RESULT_VARIABLE rejected OUTPUT_VARIABLE output ERROR_VARIABLE err TIMEOUT 20)
    if(NOT rejected STREQUAL "2" OR NOT output STREQUAL "")
      message(FATAL_ERROR "invalid trace invocation ${name} did not fail as a silent usage error: ${rejected}\n${output}${err}")
    endif()
  endfunction()

  function(trace_compare_outputs name baseline actual frames states out)
    set(equal TRUE)
    if(NOT actual STREQUAL baseline)
      set(equal FALSE)
    endif()
    foreach(frame IN LISTS frames)
      if(NOT baseline MATCHES "frame ${frame} sha256 [0-9a-f]+\n")
        set(equal FALSE)
      endif()
    endforeach()
    foreach(point IN LISTS states)
      if(NOT baseline MATCHES "state ${point} sha256 [0-9a-f]+\n")
        set(equal FALSE)
      endif()
    endforeach()
    set(${out} "${equal}" PARENT_SCOPE)
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
  elseif(CASE STREQUAL "equality")
    file(MAKE_DIRECTORY "${DIR}/compat")
    execute_process(COMMAND ${EMULATOR} "${TRACE_TEST}" compat-fixtures "${DIR}/compat"
      RESULT_VARIABLE generated OUTPUT_VARIABLE generated_out ERROR_VARIABLE generated_err TIMEOUT 20)
    if(NOT generated STREQUAL "0")
      message(FATAL_ERROR "compatibility fixtures failed: ${generated}\n${generated_out}${generated_err}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${DIAG_DRIVER}" --souper-image "${SOUPER_TEMPLATE}"
        "${DIR}/compat/compat-souper.a78"
      RESULT_VARIABLE souper_rc OUTPUT_VARIABLE souper_out ERROR_VARIABLE souper_err TIMEOUT 30)
    if(NOT souper_rc STREQUAL "0")
      message(FATAL_ERROR "generated Souper compatibility fixture failed: ${souper_rc}\n${souper_out}${souper_err}")
    endif()

    set(frames 1 2)
    set(states 0 1 2)
    foreach(region IN ITEMS ntsc pal)
      foreach(topology IN ITEMS legacy xm ym souper)
        if(topology STREQUAL "souper")
          set(image "${DIR}/compat/compat-souper.a78")
        else()
          set(image "${DIR}/compat/compat-${topology}.a78")
        endif()
        set(extra_args "")
        if(topology STREQUAL "xm" OR topology STREQUAL "ym")
          list(APPEND extra_args --expansion xm)
        endif()
        foreach(run_kind IN ITEMS plain traced)
          set(prefix "${DIR}/compat/${region}-${topology}-${run_kind}")
          set(command ${EMULATOR} "${RUN}" --frames 2 --region "${region}"
            --hash-frame 1 --hash-frame 2 --hash-state 0 --hash-state 1 --hash-state 2
            --state-at-frame "0:${prefix}-state0.bin" --state-at-frame "1:${prefix}-state1.bin"
            --state-at-frame "2:${prefix}-state2.bin")
          if(run_kind STREQUAL "traced")
            list(APPEND command --trace "${prefix}.jsonl" --trace-frame 1 --trace-range 0:65535)
          endif()
          list(APPEND command ${extra_args} "${image}")
          execute_process(COMMAND ${command} RESULT_VARIABLE rc OUTPUT_VARIABLE output
            ERROR_VARIABLE err TIMEOUT 45)
          if(NOT rc STREQUAL "0")
            message(FATAL_ERROR "${region} ${topology} ${run_kind} run failed: ${rc}\n${output}${err}")
          endif()
          if(run_kind STREQUAL "plain")
            set(plain_output "${output}")
            foreach(point IN LISTS states)
              set(baseline_${region}_${topology}_${point} "${prefix}-state${point}.bin")
            endforeach()
          else()
            trace_compare_outputs("${region} ${topology}" "${plain_output}" "${output}"
              "${frames}" "${states}" output_equal)
            if(NOT output_equal)
              message(FATAL_ERROR "${region} ${topology} trace changed stdout or omitted requested hashes")
            endif()
            foreach(point IN LISTS states)
              execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
                  "${baseline_${region}_${topology}_${point}}" "${prefix}-state${point}.bin"
                RESULT_VARIABLE bytes_equal OUTPUT_VARIABLE compare_out ERROR_VARIABLE compare_err
                TIMEOUT 20)
              if(NOT bytes_equal STREQUAL "0")
                message(FATAL_ERROR "${region} ${topology} trace changed state bytes at ${point}: ${bytes_equal}\n${compare_out}${compare_err}")
              endif()
            endforeach()
            file(READ "${prefix}.jsonl" trace_header LIMIT 256)
            if(NOT trace_header MATCHES "\"frame\":1")
              message(FATAL_ERROR "${region} ${topology} selected invocation frame one was not recorded")
            endif()
          endif()
        endforeach()

        set(loaded_state "${DIR}/compat/${region}-${topology}-plain-state2.bin")
        set(loaded_trace "${DIR}/compat/${region}-${topology}-restored.jsonl")
        set(loaded_command ${EMULATOR} "${RUN}" --frames 2 --region "${region}" --load-state "${loaded_state}"
          --hash-frame 1 --hash-frame 2 --hash-state 0 --hash-state 1 --hash-state 2)
        list(APPEND loaded_command ${extra_args} "${image}")
        execute_process(COMMAND ${loaded_command} RESULT_VARIABLE plain_rc OUTPUT_VARIABLE loaded_plain
          ERROR_VARIABLE plain_err TIMEOUT 45)
        set(trace_command ${EMULATOR} "${RUN}" --frames 2 --region "${region}" --load-state "${loaded_state}"
          --hash-frame 1 --hash-frame 2 --hash-state 0 --hash-state 1 --hash-state 2
          --trace "${loaded_trace}" --trace-frame 1 --trace-range 0:65535)
        list(APPEND trace_command ${extra_args} "${image}")
        execute_process(COMMAND ${trace_command} RESULT_VARIABLE traced_rc OUTPUT_VARIABLE loaded_actual
          ERROR_VARIABLE trace_err TIMEOUT 45)
        if(NOT plain_rc STREQUAL "0" OR NOT traced_rc STREQUAL "0")
          message(FATAL_ERROR "${region} ${topology} restored run failed: ${plain_rc}/${traced_rc}\n${plain_err}${trace_err}")
        endif()
        trace_compare_outputs("${region} ${topology} restored" "${loaded_plain}" "${loaded_actual}"
          "${frames}" "${states}" restored_equal)
        if(NOT restored_equal)
          message(FATAL_ERROR "${region} ${topology} trace after restore changed stdout or hashes")
        endif()
        file(READ "${loaded_trace}" restored_header LIMIT 256)
        if(NOT restored_header MATCHES "\"frame\":1")
          message(FATAL_ERROR "${region} ${topology} restore did not reset selected frame to invocation one")
        endif()
      endforeach()

      set(movie "${DIR}/compat/${region}-default-count.movie")
      file(WRITE "${movie}" "muhtari-movie 1\nport1 savekey\nport2 none\n|||...BB|\n|||...BB|\n")
      foreach(run_kind IN ITEMS plain traced)
        set(prefix "${DIR}/compat/${region}-movie-${run_kind}")
        set(command ${EMULATOR} "${RUN}" --region "${region}" --movie "${movie}"
          --hash-frame 1 --hash-frame 2 --hash-state 0 --hash-state 1 --hash-state 2
          --state-at-frame "0:${prefix}-state0.bin" --state-at-frame "1:${prefix}-state1.bin"
          --state-at-frame "2:${prefix}-state2.bin")
        if(run_kind STREQUAL "traced")
          list(APPEND command --trace "${prefix}.jsonl" --trace-frame 2 --trace-range 0:65535)
        endif()
        list(APPEND command "${DIR}/compat/compat-legacy.a78")
        execute_process(COMMAND ${command} RESULT_VARIABLE rc OUTPUT_VARIABLE output
          ERROR_VARIABLE err TIMEOUT 45)
        if(NOT rc STREQUAL "0")
          message(FATAL_ERROR "${region} movie-default-count ${run_kind} run failed: ${rc}\n${output}${err}")
        endif()
        if(run_kind STREQUAL "plain")
          set(movie_plain_output "${output}")
        else()
          trace_compare_outputs("${region} movie-default-count" "${movie_plain_output}" "${output}"
            "${frames}" "${states}" movie_equal)
          if(NOT movie_equal)
            message(FATAL_ERROR "${region} movie-default-count trace changed output or requested hashes")
          endif()
          foreach(point IN LISTS states)
            execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
                "${DIR}/compat/${region}-movie-plain-state${point}.bin"
                "${prefix}-state${point}.bin"
              RESULT_VARIABLE bytes_equal OUTPUT_VARIABLE compare_out ERROR_VARIABLE compare_err
              TIMEOUT 20)
            if(NOT bytes_equal STREQUAL "0")
              message(FATAL_ERROR "${region} movie trace changed canonical state bytes at ${point}: ${bytes_equal}\n${compare_out}${compare_err}")
            endif()
          endforeach()
          file(READ "${prefix}.jsonl" movie_header LIMIT 256)
          if(NOT movie_header MATCHES "\"frame\":2")
            message(FATAL_ERROR "${region} movie-default-count did not select invocation frame two")
          endif()
        endif()
      endforeach()

      set(plain_prefix "${DIR}/compat/${region}-truncated-plain")
      set(trace_prefix "${DIR}/compat/${region}-truncated-traced")
      set(plain_command ${EMULATOR} "${RUN}" --frames 2 --region "${region}"
        --hash-frame 1 --hash-frame 2 --hash-state 0 --hash-state 1 --hash-state 2
        --state-at-frame "0:${plain_prefix}-state0.bin" --state-at-frame "1:${plain_prefix}-state1.bin"
        --state-at-frame "2:${plain_prefix}-state2.bin" "${DIR}/compat/compat-legacy.a78")
      execute_process(COMMAND ${plain_command} RESULT_VARIABLE plain_rc OUTPUT_VARIABLE plain_output
        ERROR_VARIABLE plain_err TIMEOUT 45)
      set(trace_command ${EMULATOR} "${RUN}" --frames 2 --region "${region}"
        --hash-frame 1 --hash-frame 2 --hash-state 0 --hash-state 1 --hash-state 2
        --state-at-frame "0:${trace_prefix}-state0.bin" --state-at-frame "1:${trace_prefix}-state1.bin"
        --state-at-frame "2:${trace_prefix}-state2.bin" --trace "${trace_prefix}.jsonl"
        --trace-frame 1 --trace-range 0:65535 --trace-limit 1 "${DIR}/compat/compat-legacy.a78")
      execute_process(COMMAND ${trace_command} RESULT_VARIABLE traced_rc OUTPUT_VARIABLE traced_output
        ERROR_VARIABLE trace_err TIMEOUT 45)
      trace_compare_outputs("${region} truncated" "${plain_output}" "${traced_output}"
        "${frames}" "${states}" truncated_equal)
      if(NOT plain_rc STREQUAL "0" OR NOT traced_rc STREQUAL "0" OR NOT truncated_equal)
        message(FATAL_ERROR "${region} truncated trace changed output/state hashes: ${plain_rc}/${traced_rc}\n${plain_err}${trace_err}")
      endif()
      file(STRINGS "${trace_prefix}.jsonl" truncated_rows)
      string(FIND "${truncated_rows}" "\"status\":\"truncated\"" truncated_footer)
      if(truncated_footer EQUAL -1)
        message(FATAL_ERROR "${region} one-extra-matching-event capture was not marked truncated")
      endif()
    endforeach()

    set(bench_trace "${DIR}/compat/bench.jsonl")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --bench 2 --trace "${bench_trace}"
        --trace-frame 1 --trace-range 0:65535 "${DIR}/compat/compat-legacy.a78"
      RESULT_VARIABLE bench_rc OUTPUT_VARIABLE bench_out ERROR_VARIABLE bench_err TIMEOUT 45)
    if(NOT bench_rc STREQUAL "0" OR NOT EXISTS "${bench_trace}")
      message(FATAL_ERROR "--bench trace option compatibility failed: ${bench_rc}\n${bench_out}${bench_err}")
    endif()

    set(fault_baseline "frame 1 sha256 0123456789abcdef\nstate 1 sha256 fedcba9876543210\n")
    set(fault_changed "frame 1 sha256 1123456789abcdef\nstate 1 sha256 fedcba9876543210\n")
    trace_compare_outputs("changed digest" "${fault_baseline}" "${fault_changed}" "1" "1" changed_equal)
    if(changed_equal)
      message(FATAL_ERROR "equality comparator accepted a changed frame digest")
    endif()
    set(fault_changed_state "frame 1 sha256 0123456789abcdef\nstate 1 sha256 0edcba9876543210\n")
    trace_compare_outputs("changed state digest" "${fault_baseline}" "${fault_changed_state}" "1" "1" changed_state_equal)
    if(changed_state_equal)
      message(FATAL_ERROR "equality comparator accepted a changed state digest")
    endif()
    set(fault_missing "frame 1 sha256 0123456789abcdef\n")
    trace_compare_outputs("missing hash" "${fault_baseline}" "${fault_missing}" "1" "1" missing_equal)
    if(missing_equal)
      message(FATAL_ERROR "equality comparator accepted a missing requested state hash")
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
  elseif(CASE STREQUAL "arguments")
    trace_fixture()
    execute_process(COMMAND ${EMULATOR} "${SAVE_TEST}" "${DIR}" trace_arguments
      RESULT_VARIABLE harness_rc OUTPUT_VARIABLE harness_out ERROR_VARIABLE harness_err TIMEOUT 30)
    if(NOT harness_rc STREQUAL "0" OR NOT harness_out STREQUAL "")
      message(FATAL_ERROR "actual-main trace argument matrix failed or wrote stdout: ${harness_rc}\n${harness_out}${harness_err}")
    endif()
    trace_expect_cli_usage("missing range" --frames 1 --trace "${DIR}/bad.jsonl"
      --trace-frame 1)
    trace_expect_cli_usage("signed frame" --frames 1 --trace "${DIR}/bad.jsonl"
      --trace-frame +1 --trace-range 0:65535)
    trace_expect_cli_usage("overflow range" --frames 1 --trace "${DIR}/bad.jsonl"
      --trace-frame 1 --trace-range 0:65536)
    trace_expect_cli_usage("over-limit" --frames 1 --trace "${DIR}/bad.jsonl"
      --trace-frame 1 --trace-range 0:65535 --trace-limit 131073)
    trace_expect_cli_usage("selector without destination" --frames 1 --trace-frame 1
      --trace-range 0:65535)
    trace_expect_cli_usage("probe with trace" --probe "${DIR}/trace.a78" --trace
      "${DIR}/bad.jsonl")
    set(endpoint_trace "${DIR}/endpoint.jsonl")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --trace "${endpoint_trace}"
        --trace-frame 1 --trace-range 0xFFFF:0xFFFF "${DIR}/trace.a78"
      RESULT_VARIABLE endpoint_rc OUTPUT_VARIABLE endpoint_out ERROR_VARIABLE endpoint_err TIMEOUT 30)
    if(NOT endpoint_rc STREQUAL "0" OR NOT EXISTS "${endpoint_trace}")
      message(FATAL_ERROR "trace address endpoints were not accepted: ${endpoint_rc}\n${endpoint_out}${endpoint_err}")
    endif()

    set(movie "${DIR}/one-frame.movie")
    set(movie_trace "${DIR}/movie-default.jsonl")
    set(state "${DIR}/saved.state")
    set(loaded_trace "${DIR}/loaded-invocation.jsonl")
    set(combined_trace "${DIR}/trace-with-outputs.jsonl")
    set(combined_state "${DIR}/combined.state")
    set(combined_png "${DIR}/combined.png")
    file(REMOVE "${movie_trace}" "${state}" "${loaded_trace}" "${combined_trace}"
      "${combined_state}" "${combined_png}")
    file(WRITE "${movie}" "muhtari-movie 1\nport1 savekey\nport2 none\n|||...BB|\n")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --movie "${movie}" --trace "${movie_trace}"
        --trace-frame 1 --trace-range 0:65535 "${DIR}/trace.a78"
      RESULT_VARIABLE movie_rc OUTPUT_VARIABLE movie_out ERROR_VARIABLE movie_err TIMEOUT 30)
    if(NOT movie_rc STREQUAL "0" OR NOT EXISTS "${movie_trace}")
      message(FATAL_ERROR "movie-default trace frame resolution failed: ${movie_rc}\n${movie_out}${movie_err}")
    endif()
    file(READ "${movie_trace}" movie_records LIMIT 256)
    string(FIND "${movie_records}" "\"frame\":1" movie_invocation)
    if(movie_invocation EQUAL -1)
      message(FATAL_ERROR "movie default did not select invocation frame one")
    endif()

    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --state-at-frame "1:${state}"
        "${DIR}/trace.a78"
      RESULT_VARIABLE state_rc OUTPUT_VARIABLE state_out ERROR_VARIABLE state_err TIMEOUT 30)
    if(NOT state_rc STREQUAL "0" OR NOT EXISTS "${state}")
      message(FATAL_ERROR "could not prepare loaded-state invocation fixture: ${state_rc}\n${state_out}${state_err}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --load-state "${state}"
        --trace "${loaded_trace}" --trace-frame 1 --trace-range 0:65535 "${DIR}/trace.a78"
      RESULT_VARIABLE loaded_rc OUTPUT_VARIABLE loaded_out ERROR_VARIABLE loaded_err TIMEOUT 30)
    if(NOT loaded_rc STREQUAL "0" OR NOT EXISTS "${loaded_trace}")
      message(FATAL_ERROR "loaded-state invocation trace failed: ${loaded_rc}\n${loaded_out}${loaded_err}")
    endif()
    file(READ "${loaded_trace}" loaded_records LIMIT 256)
    string(FIND "${loaded_records}" "\"frame\":1" loaded_invocation)
    if(loaded_invocation EQUAL -1)
      message(FATAL_ERROR "loaded-state tracing must number this run's first invocation as frame one")
    endif()

    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --trace "${combined_trace}"
        --trace-frame 1 --trace-range 0:65535 --state-at-frame "1:${combined_state}"
        --dump-frame "1:${combined_png}" "${DIR}/trace.a78"
      RESULT_VARIABLE combined_rc OUTPUT_VARIABLE combined_out ERROR_VARIABLE combined_err TIMEOUT 30)
    if(NOT combined_rc STREQUAL "0" OR NOT EXISTS "${combined_trace}" OR
        NOT EXISTS "${combined_state}" OR NOT EXISTS "${combined_png}")
      message(FATAL_ERROR "valid trace/state/PNG outputs did not run together: ${combined_rc}\n${combined_out}${combined_err}")
    endif()
  elseif(CASE STREQUAL "paths")
    file(MAKE_DIRECTORY "${DIR}")
    if(CMAKE_HOST_WIN32)
      set(junction_target "${DIR}/trace-junction-target")
      set(junction_alias "${DIR}/trace-junction-alias")
      file(REMOVE_RECURSE "${junction_alias}" "${junction_target}")
      file(MAKE_DIRECTORY "${junction_target}")
      string(REPLACE "'" "''" junction_target_ps "${junction_target}")
      string(REPLACE "'" "''" junction_alias_ps "${junction_alias}")
      execute_process(
        COMMAND powershell.exe -NoProfile -NonInteractive -Command
          "$ErrorActionPreference = 'Stop'; New-Item -ItemType Junction -Path '${junction_alias_ps}' -Target '${junction_target_ps}' | Out-Null"
        RESULT_VARIABLE junction_rc OUTPUT_VARIABLE junction_out ERROR_VARIABLE junction_err
        TIMEOUT 20)
      if(NOT junction_rc STREQUAL "0")
        file(REMOVE_RECURSE "${junction_alias}" "${junction_target}")
        message(FATAL_ERROR "could not create path-alias junction for trace conflict test: ${junction_rc}\n${junction_out}${junction_err}")
      endif()
      execute_process(COMMAND ${EMULATOR} "${SAVE_TEST}" "${DIR}" trace_paths
          "${junction_alias}" "${junction_target}"
        RESULT_VARIABLE harness_rc OUTPUT_VARIABLE harness_out ERROR_VARIABLE harness_err TIMEOUT 30)
      file(REMOVE_RECURSE "${junction_alias}" "${junction_target}")
    else()
      execute_process(COMMAND ${EMULATOR} "${SAVE_TEST}" "${DIR}" trace_paths
        RESULT_VARIABLE harness_rc OUTPUT_VARIABLE harness_out ERROR_VARIABLE harness_err TIMEOUT 30)
    endif()
    if(NOT harness_rc STREQUAL "0" OR NOT harness_out STREQUAL "")
      message(FATAL_ERROR "actual-main trace path ownership matrix failed or wrote stdout: ${harness_rc}\n${harness_out}${harness_err}")
    endif()
  elseif(CASE STREQUAL "io")
    file(MAKE_DIRECTORY "${DIR}")
    execute_process(COMMAND ${EMULATOR} "${SAVE_TEST}" "${DIR}" trace_io
      RESULT_VARIABLE harness_rc OUTPUT_VARIABLE harness_out ERROR_VARIABLE harness_err TIMEOUT 30)
    if(NOT harness_rc STREQUAL "0" OR NOT harness_out STREQUAL "")
      message(FATAL_ERROR "actual-main trace I/O fault matrix failed or wrote stdout: ${harness_rc}\n${harness_out}${harness_err}")
    endif()
    foreach(name_status IN ITEMS
        "trace-io.jsonl;complete"
        "trace-io-truncated.jsonl;truncated"
        "trace-io-later-failure.jsonl;complete")
      list(GET name_status 0 name)
      list(GET name_status 1 expected_status)
      execute_process(COMMAND ${EMULATOR} "${TRACE_TEST}" validate "${DIR}/${name}" "${expected_status}"
        RESULT_VARIABLE valid OUTPUT_VARIABLE valid_out ERROR_VARIABLE valid_err TIMEOUT 20)
      if(NOT valid STREQUAL "0")
        message(FATAL_ERROR "trace I/O case ${name} did not have a valid terminal footer: ${valid}\n${valid_out}${valid_err}")
      endif()
    endforeach()
    file(STRINGS "${DIR}/trace-io.jsonl" rows)
    list(POP_BACK rows)
    string(JOIN "\n" interrupted ${rows})
    file(WRITE "${DIR}/trace-interrupted.jsonl" "${interrupted}\n")
    execute_process(COMMAND ${EMULATOR} "${TRACE_TEST}" validate "${DIR}/trace-interrupted.jsonl" complete
      RESULT_VARIABLE rejected OUTPUT_VARIABLE rejected_out ERROR_VARIABLE rejected_err TIMEOUT 20)
    if(rejected STREQUAL "0")
      message(FATAL_ERROR "trace consumer accepted an interrupted JSONL prefix without a terminal footer:\n${rejected_out}${rejected_err}")
    endif()
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
foreach(case IN ITEMS arguments bounds equality filters io limits paths oracle)
  file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/tests/trace-${case}")
  muhtari_add_test(NAME runner.trace.${case}
    COMMAND ${CMAKE_COMMAND} "-DEMULATOR=${trace_emulator}"
      "-DTRACE_TEST=$<TARGET_FILE:muhtari-test-trace>" "-DRUN=$<TARGET_FILE:muhtari-run>"
      "-DSAVE_TEST=$<TARGET_FILE:muhtari-test-save-runner>"
      "-DDIAG_DRIVER=$<TARGET_FILE:muhtari-test-diag>"
      "-DSOUPER_TEMPLATE=${PROJECT_SOURCE_DIR}/tests/diag/souper.a78"
      "-DDIR=${PROJECT_BINARY_DIR}/tests/trace-${case}" "-DCASE=${case}"
      -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS runner portable TIMEOUT 60)
endforeach()
muhtari_add_test(NAME runner.trace.disabled_config COMMAND ${CMAKE_COMMAND}
  "-DCMAKE_COMMAND=${CMAKE_COMMAND}" "-DCHECK=${PROJECT_SOURCE_DIR}/cmake/TraceLoopConfig.cmake"
  -P ${PROJECT_SOURCE_DIR}/tests/cmake/trace_loop_config.cmake
  LABELS runner portable TIMEOUT 10)
muhtari_add_test(NAME core.trace.edges COMMAND muhtari-test-trace edges
  LABELS core portable TIMEOUT 20)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests/trace-memory)
muhtari_add_test(NAME runner.trace.memory COMMAND muhtari-test-save-runner
  ${PROJECT_BINARY_DIR}/tests/trace-memory trace_bounds LABELS runner portable TIMEOUT 30)
