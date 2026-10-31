# Extend the existing offline preflight consumer, keeping exact inventories.
# Faults: corrupt/missing records, changed callbacks, descriptor leaks and lost kill evidence.
cmake_minimum_required(VERSION 3.25)
file(MAKE_DIRECTORY "${WORK}")
foreach(command IN ITEMS mock protocol host helper)
  if(command STREQUAL "mock")
    set(args "${MOCK}")
  elseif(command STREQUAL "protocol")
    set(args "${PROTOCOL}" "${WORK}")
  elseif(command STREQUAL "host")
    set(args "${HOST}" "${NORMAL}" "${MODULE}" "${WORK}")
  else()
    set(args "${HELPER}" "${WORK}" "${PROTOCOL}")
  endif()
  execute_process(COMMAND ${args} RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "input trace ${command}: ${rc}\n${out}${err}")
  endif()
  if(NOT command STREQUAL "mock")
    message("${out}${err}")
  endif()
  if(command STREQUAL "helper")
    string(REGEX MATCH "trace_dir=([^\r\n]+)" matched "${out}")
    set(helper_trace "${CMAKE_MATCH_1}")
    if(matched STREQUAL "")
      message(FATAL_ERROR "synthetic helper omitted its owned trace directory")
    endif()
  endif()
endforeach()

# Dump the actual bounded parser's whole validated prefix, not just its last record.
foreach(case IN ITEMS positive killed saturated malformed write-failure clock-failure invalid)
  execute_process(COMMAND "${PROTOCOL}" --dump "${WORK}/dump-${case}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
  set(expected 0)
  if(case STREQUAL "malformed" OR case STREQUAL "clock-failure" OR case STREQUAL "invalid")
    set(expected 1)
  endif()
  if(NOT rc STREQUAL "${expected}" OR NOT out MATCHES "unfinished_writer_health=unknown" OR
      NOT out MATCHES "same_host_clock_domain=caller-required" OR
      NOT out MATCHES "missing-records=inconclusive")
    message(FATAL_ERROR "bounded dump ${case}: ${rc}\n${out}${err}")
  endif()
  if(case STREQUAL "write-failure")
    if(NOT out MATCHES "stream=core status=unfinished valid_prefix_records=1" OR out MATCHES "type=POLL")
      message(FATAL_ERROR "dump invented evidence after write failure: ${out}")
    endif()
  elseif(NOT out MATCHES "stream=core type=POLL sequence=1[^\n]+a=1 b=0 c=0" OR
      NOT out MATCHES "stream=core type=STATE sequence=2[^\n]+a=4 b=1 c=0" OR
      NOT out MATCHES "stream=core type=STATE sequence=3[^\n]+a=0 b=1 c=0")
    message(FATAL_ERROR "dump lost early poll/state ${case}: ${out}")
  endif()
  if(case STREQUAL "positive" AND NOT out MATCHES "stream=core type=END sequence=4")
    message(FATAL_ERROR "dump omitted different terminal record: ${out}")
  elseif(case STREQUAL "saturated" AND NOT out MATCHES "stream=helper type=SATURATED sequence=63")
    message(FATAL_ERROR "dump hid saturation: ${out}")
  elseif(case STREQUAL "malformed" AND NOT out MATCHES "stream=core status=truncated")
    message(FATAL_ERROR "dump accepted malformed suffix: ${out}")
  elseif(case STREQUAL "clock-failure" AND NOT out MATCHES "paired_identity_clock_ok=no")
    message(FATAL_ERROR "dump accepted clock failure: ${out}")
  elseif(case STREQUAL "invalid" AND NOT out MATCHES "stream=core status=invalid")
    message(FATAL_ERROR "dump accepted invalid complete-record suffix: ${out}")
  endif()
endforeach()

# Exercise the actual wrapper's collection-before-error path with a synthetic child.
set(run "${WORK}/wrapper-run")
set(trace "${WORK}/wrapper-trace")
set(save "${WORK}/wrapper-save")
file(REMOVE_RECURSE "${trace}")
execute_process(COMMAND "${CMAKE_COMMAND}"
  "-DRETROARCH=${HELPER}" "-DKEYBOARD=${HELPER}" "-DCORE=${MODULE}" "-DINFO=${INFO}"
  "-DTEMPLATE=${ROOT}/tests/retroarch/test.cfg.in" "-DRA_DIR=${run}" "-DSAVE_DIR=${save}"
  "-DFRAMES=600" "-DSHOT=${WORK}/terminal.png" -DINPUT_MODE=probe
  "-DRAW_SHOTS=${run}/shots" "-DTRACE_DIR=${trace}" "-DTRACE_PARSER=${PROTOCOL}"
  -P "${ROOT}/tests/retroarch/run_retroarch.cmake"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT rc STREQUAL "1" OR NOT out MATCHES "Input diagnostic retained.*helper=saturated.*core=unfinished" OR
    NOT err MATCHES "RetroArch exited with '1'" OR NOT EXISTS "${trace}/core.trace")
  message(FATAL_ERROR "synthetic wrapper did not retain/collect before launch error: ${rc}\n${out}${err}")
endif()
set(fault_trace "${WORK}/wrapper-write-fault")
file(REMOVE_RECURSE "${fault_trace}")
execute_process(COMMAND "${CMAKE_COMMAND}" -E env MUHTARI_TRACE_FIXTURE_WRITE_FAILURE=1
  "${CMAKE_COMMAND}"
  "-DRETROARCH=${HELPER}" "-DKEYBOARD=${HELPER}" "-DCORE=${MODULE}" "-DINFO=${INFO}"
  "-DTEMPLATE=${ROOT}/tests/retroarch/test.cfg.in" "-DRA_DIR=${run}" "-DSAVE_DIR=${save}"
  -DFRAMES=600 "-DSHOT=${WORK}/terminal.png" -DINPUT_MODE=probe "-DRAW_SHOTS=${run}/shots"
  "-DTRACE_DIR=${fault_trace}" "-DTRACE_PARSER=${PROTOCOL}"
  -P "${ROOT}/tests/retroarch/run_retroarch.cmake"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT rc STREQUAL "1" OR NOT out MATCHES "core=unfinished records=1" OR
    NOT err MATCHES "Input diagnostic writer failure")
  message(FATAL_ERROR "record-boundary late write failure accepted: ${rc}\n${out}${err}")
endif()
foreach(fault IN ITEMS existing nested parent symlink)
  file(WRITE "${run}/sentinel" "keep")
  set(candidate "${trace}")
  if(fault STREQUAL "nested")
    set(candidate "${run}/trace")
  elseif(fault STREQUAL "parent")
    set(candidate "${WORK}")
  elseif(fault STREQUAL "symlink")
    set(candidate "${WORK}/trace-link")
    file(REMOVE "${candidate}")
    file(CREATE_LINK "${trace}" "${candidate}" SYMBOLIC)
  endif()
  execute_process(COMMAND "${CMAKE_COMMAND}"
    "-DRETROARCH=${HELPER}" "-DKEYBOARD=${HELPER}" "-DCORE=${MODULE}" "-DINFO=${INFO}"
    "-DTEMPLATE=${ROOT}/tests/retroarch/test.cfg.in" "-DRA_DIR=${run}" "-DSAVE_DIR=${save}"
    -DFRAMES=600 "-DSHOT=${WORK}/terminal.png" -DINPUT_MODE=probe "-DRAW_SHOTS=${run}/shots"
    "-DTRACE_DIR=${candidate}" "-DTRACE_PARSER=${PROTOCOL}"
    -P "${ROOT}/tests/retroarch/run_retroarch.cmake"
    RESULT_VARIABLE rc OUTPUT_QUIET ERROR_VARIABLE err TIMEOUT 5)
  if(NOT rc STREQUAL "1" OR NOT err MATCHES "trace directory must be fresh" OR NOT EXISTS "${run}/sentinel")
    message(FATAL_ERROR "unsafe trace ${fault} reached cleanup: ${rc}\n${err}")
  endif()
endforeach()
execute_process(COMMAND "${PROTOCOL}" --collect "${helper_trace}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
  if(NOT rc STREQUAL "0" OR NOT out MATCHES "helper=saturated.*core=unfinished.*last_type=6.*paired_identity_clock_ok=yes")
    message(FATAL_ERROR "killed child's final records not collected: ${rc}\n${out}${err}")
  endif()
message("${out}")
execute_process(COMMAND "${PROTOCOL}" --dump "${helper_trace}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT rc STREQUAL "0" OR NOT out MATCHES "stream=core status=unfinished" OR
    NOT out MATCHES "stream=core type=POLL" OR NOT out MATCHES "stream=core type=STATE")
  message(FATAL_ERROR "actual killed child dump lost prefix: ${rc}\n${out}${err}")
endif()
