# Budget checker controls, run as
#   cmake -DROOT=<source dir> -DWORK=<ignored work dir> -P budgets.cmake
# Fault prevented: relative ROOT paths omitting docs from the actual byte-budget
# policy, and controls that accept an unrelated child failure as a valid fault.
foreach(var ROOT WORK)
  if(NOT DEFINED ${var} OR "${${var}}" STREQUAL "")
    message(FATAL_ERROR "budgets.controls: -D${var} is required")
  endif()
endforeach()

get_filename_component(project_root "${ROOT}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
get_filename_component(work_root "${WORK}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(checker "${project_root}/tests/cmake/budgets.cmake")
set(expect_fail "${project_root}/tests/cmake/expect_fail.cmake")
if(NOT EXISTS "${checker}" OR NOT EXISTS "${expect_fail}")
  message(FATAL_ERROR "budgets.controls: checker files are missing from ROOT")
endif()

set(fixture_root "${work_root}/source tree")
set(caller_root "${work_root}/caller cwd with spaces")
set(relative_root "../source tree")
file(REMOVE_RECURSE "${fixture_root}" "${caller_root}")
file(MAKE_DIRECTORY "${fixture_root}/include" "${fixture_root}/docs/hardware"
  "${caller_root}")

function(write_sized_file path size)
  get_filename_component(parent "${path}" DIRECTORY)
  file(MAKE_DIRECTORY "${parent}")
  string(REPEAT "x" ${size} content)
  file(WRITE "${path}" "${content}")
endfunction()

set(files
  "README.md|12288"
  "include/muhtari.h|24576"
  "docs/guide.md|40960"
  "docs/hardware/chip.md|40960")
foreach(entry IN LISTS files)
  string(REGEX MATCH "^(.+)\\|([0-9]+)$" unused "${entry}")
  write_sized_file("${fixture_root}/${CMAKE_MATCH_1}" "${CMAKE_MATCH_2}")
endforeach()

function(run_checker root_arg result_var output_var)
  execute_process(COMMAND "${CMAKE_COMMAND}" "-DROOT=${root_arg}" -P "${checker}"
    WORKING_DIRECTORY "${caller_root}" RESULT_VARIABLE result
    OUTPUT_VARIABLE out ERROR_VARIABLE err)
  set(${result_var} "${result}" PARENT_SCOPE)
  set(${output_var} "${out}${err}" PARENT_SCOPE)
endfunction()

function(assert_pass root_arg root_label)
  run_checker("${root_arg}" result output)
  if(NOT "${result}" STREQUAL "0" OR NOT output MATCHES "budgets: 4 files within budget")
    message(FATAL_ERROR "budgets.controls: ${root_label} exact-limit files did not pass as 4 files; exit ${result}\n${output}")
  endif()
endfunction()

function(assert_fault root_arg root_label path limit)
  math(EXPR actual "${limit} + 1")
  string(REPLACE "." "[.]" escaped_path "${path}")
  set(regex "${escaped_path}: ${actual} bytes, budget ${limit}")
  set(command "${CMAKE_COMMAND}" "-DROOT=${root_arg}" -P "${checker}")
  execute_process(COMMAND "${CMAKE_COMMAND}" "-DCMD=${command}" -DCODE=1
      "-DREGEX=${regex}" -P "${expect_fail}"
    WORKING_DIRECTORY "${caller_root}" RESULT_VARIABLE result
    OUTPUT_VARIABLE out ERROR_VARIABLE err)
  if(NOT "${result}" STREQUAL "0")
    message(FATAL_ERROR "budgets.controls: ${root_label} ${path} over-limit fault was not attributed; expect_fail exit ${result}\n${out}${err}")
  endif()
endfunction()

function(assert_missing_root root_arg root_label)
  set(command "${CMAKE_COMMAND}" "-DROOT=${root_arg}" -P "${checker}")
  execute_process(COMMAND "${CMAKE_COMMAND}" "-DCMD=${command}" -DCODE=1
      "-DREGEX=budgets: -DROOT=<source dir> is required" -P "${expect_fail}"
    WORKING_DIRECTORY "${caller_root}" RESULT_VARIABLE result
    OUTPUT_VARIABLE out ERROR_VARIABLE err)
  if(NOT "${result}" STREQUAL "0")
    message(FATAL_ERROR "budgets.controls: ${root_label} nonexistent root did not keep the required-root failure; expect_fail exit ${result}\n${out}${err}")
  endif()
endfunction()

assert_pass("${fixture_root}" absolute)
assert_pass("${relative_root}" relative)

foreach(entry IN LISTS files)
  string(REGEX MATCH "^(.+)\\|([0-9]+)$" unused "${entry}")
  set(path "${CMAKE_MATCH_1}")
  set(limit "${CMAKE_MATCH_2}")
  math(EXPR oversized "${limit} + 1")
  write_sized_file("${fixture_root}/${path}" "${oversized}")
  assert_fault("${fixture_root}" absolute "${path}" "${limit}")
  assert_fault("${relative_root}" relative "${path}" "${limit}")
  write_sized_file("${fixture_root}/${path}" "${limit}")
endforeach()

assert_missing_root("${work_root}/missing source tree" absolute)
assert_missing_root("../missing source tree" relative)

message(STATUS "budgets.controls: absolute and relative roots passed four exact-limit files, eight attributed over-limit faults, and both missing-root controls")
