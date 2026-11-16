# Controls for the CPU-backed hooks-off instruction-cost comparison.
cmake_minimum_required(VERSION 3.25)

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

if(MODE STREQUAL "controls")
  trace_control(equal "0" "verdict=PASS"
    -DON60_1=100000 -DON360_1=130000 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=130000 -DOFF60_2=200000 -DOFF360_2=230000)
  trace_control(positive_32 "0" "delta_ir=32 raw_delta=9600 verdict=PASS"
    -DON60_1=100000 -DON360_1=139600 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=139600 -DOFF60_2=200000 -DOFF360_2=230000)
  trace_control(negative_32 "0" "delta_ir=-32 raw_delta=-9600 verdict=PASS"
    -DON60_1=100000 -DON360_1=120400 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=120400 -DOFF60_2=200000 -DOFF360_2=230000)
  trace_control(positive_33 "1" "hooks-off delta exceeds 32 Ir/frame"
    -DON60_1=100000 -DON360_1=139900 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=139900 -DOFF60_2=200000 -DOFF360_2=230000)
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
  trace_control(unstable "1" "trace: repeated ON Ir/frame samples differ"
    -DON60_1=100000 -DON360_1=130000 -DOFF60_1=200000 -DOFF360_1=230000
    -DON60_2=100000 -DON360_2=130001 -DOFF60_2=200000 -DOFF360_2=230000)
  return()
endif()

if(MODE STREQUAL "measure")
  message(FATAL_ERROR "trace: hooks-off measurement consumer is not implemented")
endif()

message(FATAL_ERROR "trace: MODE must be controls or measure")
