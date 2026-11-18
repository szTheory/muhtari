# CPU-backed hooks-off instruction-cost comparison and its controls.
cmake_minimum_required(VERSION 3.25)

function(trace_error reason)
  message(FATAL_ERROR "trace: error ${reason}")
endfunction()

function(trace_control name expected_rc expected_output)
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -DMODE=measure -DTRACE_CONTROL=TRUE
      "-DTRACE_CASE=${name}" ${ARGN} -P "${CMAKE_CURRENT_LIST_FILE}"
    RESULT_VARIABLE actual_rc OUTPUT_VARIABLE actual_out ERROR_VARIABLE actual_err
    TIMEOUT 10)
  set(actual "${actual_out}${actual_err}")
  if(NOT "${actual_rc}" STREQUAL "${expected_rc}" OR NOT actual MATCHES "${expected_output}")
    message(FATAL_ERROR
      "trace control ${name}: expected exit ${expected_rc} and /${expected_output}/, "
      "got ${actual_rc}:\n${actual}")
  endif()
endfunction()

function(trace_sample value name)
  if(NOT "${value}" MATCHES "^[1-9][0-9]*$")
    trace_error("${name} '${value}' is not a positive Cachegrind Ir sample")
  endif()
  string(LENGTH "${value}" digits)
  if(digits GREATER 18)
    trace_error("${name} exceeds the checked integer range")
  endif()
endfunction()

function(trace_delta out side repeat)
  set(sample60 "${side}60_${repeat}")
  set(sample360 "${side}360_${repeat}")
  set(ir60 "${${sample60}}")
  set(ir360 "${${sample360}}")
  trace_sample("${ir60}" "${sample60}")
  trace_sample("${ir360}" "${sample360}")
  if(ir360 LESS_EQUAL ir60)
    trace_error("${sample360} must exceed ${sample60}; workload evidence is empty")
  endif()
  math(EXPR delta "${ir360} - ${ir60}")
  set(${out} "${delta}" PARENT_SCOPE)
endfunction()

function(trace_compare)
  trace_delta(on1 ON 1)
  trace_delta(on2 ON 2)
  trace_delta(off1 OFF 1)
  trace_delta(off2 OFF 2)
  if(NOT "${on1}" STREQUAL "${on2}")
    trace_error("repeated ON Ir/frame samples differ")
  endif()
  if(NOT "${off1}" STREQUAL "${off2}")
    trace_error("repeated OFF Ir/frame samples differ")
  endif()

  math(EXPR raw_delta "${on1} - ${off1}")
  if(raw_delta LESS 0)
    math(EXPR absolute_delta "-${raw_delta}")
  else()
    set(absolute_delta "${raw_delta}")
  endif()
  math(EXPR on_ir_frame "${on1} / 300")
  math(EXPR off_ir_frame "${off1} / 300")
  math(EXPR delta_ir "${raw_delta} / 300")
  if(absolute_delta GREATER 9600)
    set(verdict FAIL)
  else()
    set(verdict PASS)
  endif()
  set(line "trace hooks_off delta_ir=${delta_ir} raw_delta=${raw_delta} repeats=2 verdict=${verdict}")
  message(STATUS "${line}")

  if(NOT TRACE_CONTROL)
    set(receipt "{\n")
    string(APPEND receipt
      "  \"workload_sha256\": \"${TRACE_WORKLOAD_SHA}\",\n"
      "  \"on_ir60\": [${ON60_1}, ${ON60_2}],\n"
      "  \"on_ir360\": [${ON360_1}, ${ON360_2}],\n"
      "  \"off_ir60\": [${OFF60_1}, ${OFF60_2}],\n"
      "  \"off_ir360\": [${OFF360_1}, ${OFF360_2}],\n"
      "  \"on_raw_delta\": ${on1},\n"
      "  \"off_raw_delta\": ${off1},\n"
      "  \"on_ir_frame\": ${on_ir_frame},\n"
      "  \"off_ir_frame\": ${off_ir_frame},\n"
      "  \"delta_raw\": ${raw_delta},\n"
      "  \"delta_ir_frame\": ${delta_ir},\n"
      "  \"threshold_raw\": 9600,\n"
      "  \"repeats\": 2,\n"
      "  \"status\": \"${verdict}\"\n}\n")
    file(WRITE "${WORK}/trace-cost.json" "${receipt}")
  endif()

  if(NOT verdict STREQUAL "PASS")
    trace_error("hooks-off delta exceeds 32 Ir/frame (absolute raw delta ${absolute_delta} > 9600)")
  endif()
endfunction()

function(trace_ir_sample out runner variant frames repeat)
  set(cache_file "${WORK}/cache-${variant}-${frames}-${repeat}.out")
  file(REMOVE "${cache_file}")
  execute_process(
    COMMAND "${ENV_EXECUTABLE}" -i "${VALGRIND}" --tool=cachegrind --cache-sim=no
      "--cachegrind-out-file=${cache_file}" "${runner}" --frames "${frames}" "${ROM}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE run_out ERROR_VARIABLE run_err TIMEOUT 180)
  if(NOT "${rc}" STREQUAL "0")
    trace_error("Cachegrind ${variant} ${frames}-frame run ${repeat} failed (${rc}): ${run_out}${run_err}")
  endif()
  if(NOT EXISTS "${cache_file}")
    trace_error("Cachegrind ${variant} ${frames}-frame run ${repeat} produced no data file")
  endif()
  file(STRINGS "${cache_file}" summaries REGEX "^summary:")
  list(LENGTH summaries summary_count)
  if(NOT summary_count EQUAL 1)
    trace_error("Cachegrind ${variant} ${frames}-frame run ${repeat} has ${summary_count} Ir summaries")
  endif()
  list(GET summaries 0 summary)
  if(NOT "${summary}" MATCHES "^summary:[ \t]*([0-9]+)$")
    trace_error("Cachegrind ${variant} ${frames}-frame run ${repeat} has malformed Ir data '${summary}'")
  endif()
  set(value "${CMAKE_MATCH_1}")
  trace_sample("${value}" "${variant}${frames}_${repeat}")
  set(${out} "${value}" PARENT_SCOPE)
endfunction()

function(trace_check_cpu_workload runner variant)
  execute_process(
    COMMAND "${ENV_EXECUTABLE}" -i "${runner}" --frames 1 --diagnostic "${ROM}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE output ERROR_VARIABLE err TIMEOUT 60)
  if(NOT "${rc}" STREQUAL "0")
    trace_error("${variant} runner rejected generated CPU workload (${rc}): ${output}${err}")
  endif()
  string(FIND "${output}" "diagnostic a5" marker)
  if(marker EQUAL -1)
    trace_error("${variant} runner did not execute the generated CPU loop marker: ${output}${err}")
  endif()
  set(${variant}_output "${output}" PARENT_SCOPE)
  set(${variant}_error "${err}" PARENT_SCOPE)
endfunction()

function(trace_measure)
  foreach(required RUN_ON RUN_OFF ROM WORK VALGRIND)
    if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
      trace_error("missing -D${required} measurement input")
    endif()
  endforeach()
  if(RUN_ON STREQUAL RUN_OFF)
    trace_error("RUN_ON and RUN_OFF must name distinct installed runners")
  endif()
  if(NOT "${RUN_ON}" MATCHES "^/" OR NOT "${RUN_OFF}" MATCHES "^/")
    trace_error("RUN_ON and RUN_OFF must be absolute paths")
  endif()
  string(LENGTH "${RUN_ON}" on_path_length)
  string(LENGTH "${RUN_OFF}" off_path_length)
  if(NOT on_path_length EQUAL off_path_length)
    trace_error("installed runner paths have unequal lengths")
  endif()
  foreach(path IN ITEMS "${RUN_ON}" "${RUN_OFF}" "${ROM}" "${VALGRIND}")
    if(NOT EXISTS "${path}")
      trace_error("required measurement path does not exist: ${path}")
    endif()
  endforeach()
  file(SIZE "${ROM}" rom_size)
  if(rom_size LESS 4224)
    trace_error("generated CPU workload is shorter than an A78 image")
  endif()
  find_program(ENV_EXECUTABLE env)
  if(NOT ENV_EXECUTABLE)
    trace_error("env is unavailable for the empty-environment measurement")
  endif()
  file(MAKE_DIRECTORY "${WORK}")

  # This build-tree image writes the A5 diagnostic marker from its known CPU
  # loop; a no-content test-card run cannot pass this real-runner preflight.
  trace_check_cpu_workload("${RUN_ON}" on)
  trace_check_cpu_workload("${RUN_OFF}" off)
  if(NOT "${on_output}" STREQUAL "${off_output}" OR NOT "${on_error}" STREQUAL "${off_error}")
    trace_error("dual-loop and OFF workload preflights differ")
  endif()
  file(SHA256 "${ROM}" TRACE_WORKLOAD_SHA)

  foreach(repeat RANGE 1 2)
    trace_ir_sample(ON60_${repeat} "${RUN_ON}" on 60 "${repeat}")
    trace_ir_sample(ON360_${repeat} "${RUN_ON}" on 360 "${repeat}")
    trace_ir_sample(OFF60_${repeat} "${RUN_OFF}" of 60 "${repeat}")
    trace_ir_sample(OFF360_${repeat} "${RUN_OFF}" of 360 "${repeat}")
  endforeach()
  trace_compare()
  message(STATUS "trace workload_sha256=${TRACE_WORKLOAD_SHA}; both runner preflights executed the CPU marker")
endfunction()

if(MODE STREQUAL "controls")
  trace_control(equal "0" "verdict=PASS"
    -DON60_1=100000 -DON360_1=130000 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=130000 -DOFF60_2=200000 -DOFF360_2=230000)
  trace_control(positive_32 "0" "delta_ir=32 raw_delta=9600 repeats=2 verdict=PASS"
    -DON60_1=100000 -DON360_1=139600 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=139600 -DOFF60_2=200000 -DOFF360_2=230000)
  trace_control(negative_32 "0" "delta_ir=-32 raw_delta=-9600 repeats=2 verdict=PASS"
    -DON60_1=100000 -DON360_1=120400 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=120400 -DOFF60_2=200000 -DOFF360_2=230000)
  trace_control(positive_33 "1" "hooks-off delta exceeds 32 Ir/frame"
    -DON60_1=100000 -DON360_1=139900 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=139900 -DOFF60_2=200000 -DOFF360_2=230000)
  trace_control(negative_33 "1" "hooks-off delta exceeds 32 Ir/frame"
    -DON60_1=100000 -DON360_1=120100 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=120100 -DOFF60_2=200000 -DOFF360_2=230000)
  trace_control(fractional_over_32 "1" "hooks-off delta exceeds 32 Ir/frame"
    -DON60_1=100000 -DON360_1=139601 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=139601 -DOFF60_2=200000 -DOFF360_2=230000)
  trace_control(missing "1" "trace: error ON360_2"
    -DON60_1=100000 -DON360_1=130000 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2= -DOFF60_2=200000 -DOFF360_2=230000)
  trace_control(malformed "1" "trace: error ON360_2"
    -DON60_1=100000 -DON360_1=130000 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=bad -DOFF60_2=200000 -DOFF360_2=230000)
  trace_control(zero "1" "trace: error OFF360_1"
    -DON60_1=100000 -DON360_1=130000 -DOFF60_1=200000 -DOFF360_1=0
    -DON60_2=100000 -DON360_2=130000 -DOFF60_2=200000 -DOFF360_2=230000)
  trace_control(unstable "1" "trace: error repeated ON Ir/frame samples differ"
    -DON60_1=100000 -DON360_1=130000 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=130001 -DOFF60_2=200000 -DOFF360_2=230000)
  return()
elseif(MODE STREQUAL "measure")
  if(TRACE_CONTROL)
    trace_compare()
  else()
    trace_measure()
  endif()
  return()
endif()

message(FATAL_ERROR "trace: MODE must be controls or measure")
