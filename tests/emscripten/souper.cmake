cmake_minimum_required(VERSION 3.25)

foreach(var NODE HOST MODULE NATIVE FIXTURE)
  if(NOT DEFINED ${var})
    message(FATAL_ERROR "portable Souper: -D${var} is required")
  endif()
endforeach()
execute_process(COMMAND "${NODE}" "${HOST}" --module "${MODULE}" --fixture "${FIXTURE}"
  --native "${NATIVE}" --mode verify RESULT_VARIABLE rc OUTPUT_VARIABLE output
  ERROR_VARIABLE error TIMEOUT 30)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "portable Souper production loader: ${rc}: ${output}${error}")
endif()
message(STATUS "${output}")
