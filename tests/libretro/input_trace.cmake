# Extend the existing offline preflight consumer, keeping exact inventories.
# Faults: corrupt/missing records, changed callbacks, descriptor leaks and lost kill evidence.
cmake_minimum_required(VERSION 3.25)
file(MAKE_DIRECTORY "${WORK}")
foreach(case IN ITEMS clean inherit cloexec helper helper-high directory core identity)
  if(case STREQUAL "identity")
    set(args "${HELPER}" --identity)
  else()
    set(args "${HELPER}" --descriptor-control "${case}" "${WORK}")
  endif()
  execute_process(COMMAND ${args} RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "descriptor ${case}: ${rc}\n${out}${err}")
  endif()
endforeach()
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
    if(NOT err MATCHES "input trace key=126 state=down count=50 first_ms=0 last_ms=4900" OR
        NOT err MATCHES "input trace key=126 state=up count=400 first_ms=5000 last_ms=44900")
      message(FATAL_ERROR "synthetic helper omitted exact bounded repeat summaries: ${err}")
    endif()
  endif()
endforeach()

# Exercise the production passive launcher and its strict synthetic child without a real app.
execute_process(COMMAND "${HOST}" --manual-control "${WORK}" "${MODULE}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT rc STREQUAL "0" OR NOT out MATCHES "manual control valid: valid trace" OR
    NOT out MATCHES "manual control timeout: exact-child SIGTERM/SIGKILL/reap" OR
    NOT out MATCHES "manual control interrupt: EINTR returns to bounded deadline and exact-child cleanup" OR
    NOT out MATCHES "manual control unreaped: bounded unreaped result" OR
    NOT out MATCHES "manual control existing-file: existing trace sentinel preserved; partial trace absent" OR
    NOT out MATCHES "manual control existing-core-file: existing trace preserved; partial helper absent" OR
    NOT out MATCHES "manual control existing-core-file retry: complete paired traces" OR
    NOT out MATCHES "manual control symlink: existing trace preserved; partial helper absent" OR
    NOT out MATCHES "manual control symlink retry: complete paired traces")
  message(FATAL_ERROR "passive manual launcher controls failed: ${rc}\n${out}${err}")
endif()
string(REGEX MATCH "manual_trace_dir=([^\r\n]+)" manual_match "${out}")
set(manual_trace_dir "${CMAKE_MATCH_1}")
if(manual_match STREQUAL "")
  message(FATAL_ERROR "manual launcher omitted its valid trace directory: ${out}")
endif()
execute_process(COMMAND "${PROTOCOL}" --collect "${manual_trace_dir}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT rc STREQUAL "0" OR NOT out MATCHES "helper=complete.*core=complete.*paired_identity_clock_ok=yes")
  message(FATAL_ERROR "synthetic passive traces did not pair: ${rc}\n${out}${err}")
endif()
execute_process(COMMAND "${PROTOCOL}" --manual-core "${manual_trace_dir}/core.trace"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT rc STREQUAL "0" OR NOT out MATCHES "callbacks sampled Up=0.*Up=1.*Up=0")
  message(FATAL_ERROR "manual callback validator rejected the synthetic sequence: ${rc}\n${out}${err}")
endif()

# Run the wrapper itself with the trace-host test child as the app executable.
execute_process(COMMAND "${CMAKE_COMMAND}" -E env
  "MUHTARI_BUILD_DIR=${BUILD}"
  "MUHTARI_MANUAL_INPUT_TEST_CHILD=1"
  "MUHTARI_MANUAL_INPUT_EXPECT_CORE=${MODULE}"
  "${ROOT}/scripts/retroarch-manual-input.sh" --run --retroarch "${HOST}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT rc STREQUAL "0" OR NOT out MATCHES "Callback result: RetroArch delivered" OR
    NOT out MATCHES "released sequence to the diagnostic core" OR
    NOT out MATCHES "record its waiting/held/complete color progression separately")
  message(FATAL_ERROR "owner wrapper's synthetic run did not pass the exact argv/trace contract: ${rc}\n${out}${err}")
endif()
string(REGEX MATCH "trace_dir=([^\r\n]+)" manual_match "${out}")
set(wrapper_trace_dir "${CMAKE_MATCH_1}")
if(manual_match STREQUAL "")
  message(FATAL_ERROR "owner wrapper omitted its trace directory: ${out}")
endif()
execute_process(COMMAND "${PROTOCOL}" --collect "${wrapper_trace_dir}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT rc STREQUAL "0" OR NOT out MATCHES "helper=complete.*core=complete.*paired_identity_clock_ok=yes")
  message(FATAL_ERROR "wrapper child traces did not pair: ${rc}\n${out}${err}")
endif()
execute_process(COMMAND "${PROTOCOL}" --manual-core "${wrapper_trace_dir}/core.trace"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT rc STREQUAL "0" OR NOT out MATCHES "callbacks sampled Up=0.*Up=1.*Up=0")
  message(FATAL_ERROR "wrapper's actual synthetic launch did not produce the requested callbacks: ${rc}\n${out}${err}")
endif()

# Preparation qualifies config/cart creation but must not start even the harmless app override.
execute_process(COMMAND "${ROOT}/scripts/retroarch-manual-input.sh"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT rc STREQUAL "2" OR NOT err MATCHES "Usage:" OR out MATCHES "output_dir=")
  message(FATAL_ERROR "wrapper without an explicit mode did not refuse to launch: ${rc}\n${out}${err}")
endif()
set(nonexecutable "${WORK}/retroarch-not-executable")
file(WRITE "${nonexecutable}" "synthetic non-executable path\n")
file(CHMOD "${nonexecutable}" PERMISSIONS OWNER_READ OWNER_WRITE)
foreach(rejected_app IN ITEMS "${WORK}/missing-retroarch" "${WORK}" "${nonexecutable}")
  execute_process(COMMAND "${ROOT}/scripts/retroarch-manual-input.sh"
    --prepare-only --retroarch "${rejected_app}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
  if(NOT rc STREQUAL "2" OR NOT err MATCHES "absent or not executable" OR out MATCHES "output_dir=")
    message(FATAL_ERROR "wrapper accepted an invalid app path ${rejected_app}: ${rc}\n${out}${err}")
  endif()
endforeach()
execute_process(COMMAND "${CMAKE_COMMAND}" -E env "MUHTARI_BUILD_DIR=${BUILD}"
  "${ROOT}/scripts/retroarch-manual-input.sh" --prepare-only --retroarch /usr/bin/true
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT rc STREQUAL "0" OR NOT out MATCHES "no frontend was launched" OR
    NOT out MATCHES "output_dir=([^\r\n]+)" OR NOT out MATCHES "trace_dir=([^\r\n]+)")
  message(FATAL_ERROR "safe prepare-only wrapper path failed: ${rc}\n${out}${err}")
endif()
string(REGEX MATCH "trace_dir=([^\r\n]+)" manual_match "${out}")
set(prepare_trace_dir "${CMAKE_MATCH_1}")
if(EXISTS "${prepare_trace_dir}/helper.trace" OR EXISTS "${prepare_trace_dir}/core.trace")
  message(FATAL_ERROR "prepare-only unexpectedly launched a frontend child")
endif()

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
if(NOT rc STREQUAL "1" OR NOT out MATCHES "Input diagnostic retained.*helper=complete.*core=unfinished" OR
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
  if(NOT rc STREQUAL "0" OR NOT out MATCHES "helper=complete.*core=unfinished.*last_type=6.*paired_identity_clock_ok=yes")
    message(FATAL_ERROR "killed child's final records not collected: ${rc}\n${out}${err}")
  endif()
message("${out}")
execute_process(COMMAND "${PROTOCOL}" --dump "${helper_trace}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
if(NOT rc STREQUAL "0" OR NOT out MATCHES "stream=core status=unfinished" OR
    NOT out MATCHES "stream=core type=POLL" OR NOT out MATCHES "stream=core type=STATE")
  message(FATAL_ERROR "actual killed child dump lost prefix: ${rc}\n${out}${err}")
endif()
