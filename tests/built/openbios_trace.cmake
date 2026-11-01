# Verify that a complete OpenBIOS build leaves all stage markers and that the
# duplicate-incbin fault leaves its active-stage marker.
cmake_minimum_required(VERSION 3.25)
foreach(var SCRIPT WASMTIME PKG ASM OUT_DIR PASS_TRACE FAULT_TRACE)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "openbios trace self-test: -D${var} is required")
  endif()
endforeach()

if(NOT EXISTS "${PASS_TRACE}")
  message(FATAL_ERROR "openbios trace self-test: successful build marker is missing")
endif()
file(READ "${PASS_TRACE}" pass_trace)
foreach(stage prepare-output transform-source assemble verify-output complete)
  string(FIND "${pass_trace}" "stage=${stage}" marker_position)
  if(marker_position EQUAL -1)
    message(FATAL_ERROR "openbios trace self-test: successful trace lacks ${stage}")
  endif()
endforeach()

file(READ "${ASM}" broken_asm)
set(original_asm "${broken_asm}")
string(REPLACE "incbin \"kiloparsec.bl1\""
  "incbin \"kiloparsec.bl1\"\n incbin \"kiloparsec.bl1\"" broken_asm "${broken_asm}")
if("${broken_asm}" STREQUAL "${original_asm}")
  message(FATAL_ERROR "openbios trace self-test: failed to prepare source fault")
endif()
set(fault_asm "${OUT_DIR}/openbios_twice.asm")
file(MAKE_DIRECTORY "${OUT_DIR}")
file(WRITE "${fault_asm}" "${broken_asm}")

execute_process(
  COMMAND "${CMAKE_COMMAND}"
    "-DWASMTIME=${WASMTIME}"
    "-DPKG=${PKG}"
    "-DASM=${fault_asm}"
    "-DFILL=FF"
    "-DOUT=${OUT_DIR}/openbios_twice/openbios.bin"
    "-DTRACE=${FAULT_TRACE}"
    -P "${SCRIPT}"
  RESULT_VARIABLE fault_result
  OUTPUT_VARIABLE fault_stdout
  ERROR_VARIABLE fault_stderr)
if("${fault_result}" STREQUAL "0")
  message(FATAL_ERROR "openbios trace self-test: duplicate-incbin fault unexpectedly passed")
endif()
string(CONCAT fault_output "${fault_stdout}" "${fault_stderr}")
if(NOT fault_output MATCHES "kiloparsec[.]bl1[^ ]* occurs 2 times")
  message(FATAL_ERROR "openbios trace self-test: duplicate-incbin diagnostic is missing\n${fault_output}")
endif()
if(NOT EXISTS "${FAULT_TRACE}")
  message(FATAL_ERROR "openbios trace self-test: fault marker is missing")
endif()
file(READ "${FAULT_TRACE}" fault_trace)
if(NOT fault_trace MATCHES "stage=transform-source")
  message(FATAL_ERROR "openbios trace self-test: fault did not retain transform-source stage\n${fault_trace}")
endif()
if(fault_trace MATCHES "stage=assemble" OR fault_trace MATCHES "stage=complete")
  message(FATAL_ERROR "openbios trace self-test: fault trace advanced beyond source transform\n${fault_trace}")
endif()
