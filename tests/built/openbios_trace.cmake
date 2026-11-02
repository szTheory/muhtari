# Verify completed assembly capture, a failing child's persistent diagnostics,
# and the duplicate-incbin fault's active-stage marker.
cmake_minimum_required(VERSION 3.25)
foreach(var SCRIPT WASMTIME PKG ASM OUT_DIR PASS_DIR PASS_TRACE FAULT_TRACE)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "openbios trace self-test: -D${var} is required")
  endif()
endforeach()

function(check_capture dir package)
  foreach(name assemble-command.txt assemble.log assemble.stderr.log)
    if(NOT EXISTS "${dir}/${name}")
      message(FATAL_ERROR "openbios trace self-test: missing capture ${dir}/${name}")
    endif()
  endforeach()
  file(TO_CMAKE_PATH "${package}" package)
  file(READ "${dir}/assemble-command.txt" command)
  set(expected "Prepared command; not proof of child launch.\nworking_directory=${dir}\n")
  string(APPEND expected "argv[0]=${WASMTIME}\nargv[1]=run\nargv[2]=--dir=.\n")
  string(APPEND expected "argv[3]=${package}/dasm.wasm\nargv[4]=7800openbios.asm\n")
  string(APPEND expected "argv[5]=-f3\nargv[6]=-oopenbios.bin\n")
  if(NOT command STREQUAL expected)
    message(FATAL_ERROR "openbios trace self-test: prepared command differs\n${command}")
  endif()
endfunction()

check_capture("${PASS_DIR}" "${PKG}")
file(READ "${PASS_DIR}/assemble.log" pass_stdout)
if(NOT pass_stdout MATCHES "Complete[.]")
  message(FATAL_ERROR "openbios trace self-test: successful assembler stdout is missing")
endif()

if(NOT EXISTS "${PASS_TRACE}")
  message(FATAL_ERROR "openbios trace self-test: successful build marker is missing")
endif()

# A missing WASM module starts Wasmtime but fails before assembly. Its stderr
# must be on disk and still appear in the ordinary fatal diagnostic.
set(child_dir "${OUT_DIR}/missing-module")
set(child_pkg "${OUT_DIR}/missing-package")
file(MAKE_DIRECTORY "${OUT_DIR}")
file(REMOVE_RECURSE "${child_pkg}")
execute_process(
  COMMAND "${CMAKE_COMMAND}"
    "-DWASMTIME=${WASMTIME}"
    "-DPKG=${child_pkg}"
    "-DASM=${ASM}"
    "-DFILL=FF"
    "-DOUT=${child_dir}/openbios.bin"
    "-DTRACE=${OUT_DIR}/missing-module-stage.txt"
    -P "${SCRIPT}"
  RESULT_VARIABLE child_result
  OUTPUT_VARIABLE child_stdout
  ERROR_VARIABLE child_stderr)
if(NOT "${child_result}" STREQUAL "1" OR
    NOT child_stderr MATCHES "stage assemble exited [1-9][0-9]*" OR
    NOT child_stderr MATCHES "dasm[.]wasm")
  message(FATAL_ERROR "openbios trace self-test: missing-module failure changed\n${child_result}\n${child_stdout}\n${child_stderr}")
endif()
check_capture("${child_dir}" "${child_pkg}")
file(READ "${child_dir}/assemble.stderr.log" child_errors)
if(NOT child_errors MATCHES "dasm[.]wasm")
  message(FATAL_ERROR "openbios trace self-test: failing child stderr is missing\n${child_errors}")
endif()
file(READ "${OUT_DIR}/missing-module-stage.txt" child_trace)
if(NOT child_trace MATCHES "stage=assemble" OR child_trace MATCHES "stage=verify-output|stage=complete")
  message(FATAL_ERROR "openbios trace self-test: failing child stage changed\n${child_trace}")
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
