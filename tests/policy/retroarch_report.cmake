# Fault prevented: a fail-fast input judge leaves only unrelated launch logs.
cmake_minimum_required(VERSION 3.25)
foreach(var ROOT BUILD WORK)
  if(NOT DEFINED ${var} OR "${${var}}" STREQUAL "")
    message(FATAL_ERROR "retroarch_report: -D${var} is required")
  endif()
endforeach()

file(REAL_PATH "${BUILD}" build_root)
set(work_root "${WORK}")
cmake_path(ABSOLUTE_PATH work_root BASE_DIRECTORY "${build_root}" NORMALIZE
  OUTPUT_VARIABLE work_root)
file(REAL_PATH "${work_root}" work_real_path)
cmake_path(IS_PREFIX build_root "${work_real_path}" NORMALIZE work_under_build)
if(NOT work_under_build OR work_real_path STREQUAL build_root)
  message(FATAL_ERROR "retroarch_report: WORK must be a strict child of BUILD")
endif()
set(WORK "${work_real_path}")

# The negative control uses a unique sibling of BUILD, still under the
# project's build tree. Its sentinel proves the child rejects WORK before it
# can remove or recreate that directory.
if(NOT MODE STREQUAL "unsafe-work-child")
  string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef suffix)
  get_filename_component(build_parent "${build_root}" DIRECTORY)
  set(unsafe_work "${build_parent}/retroarch_report_unsafe_${suffix}")
  if(EXISTS "${unsafe_work}" OR IS_SYMLINK "${unsafe_work}")
    message(FATAL_ERROR "retroarch_report: unsafe-work control path already exists")
  endif()
  file(MAKE_DIRECTORY "${unsafe_work}")
  set(sentinel "${unsafe_work}/sentinel")
  file(WRITE "${sentinel}" "preserve\n")
  execute_process(
    COMMAND "${CMAKE_COMMAND}" "-DROOT=${ROOT}" "-DBUILD=${BUILD}"
      "-DWORK=${unsafe_work}" -DMODE=unsafe-work-child
      -P "${CMAKE_CURRENT_LIST_FILE}"
    RESULT_VARIABLE unsafe_status
    OUTPUT_VARIABLE unsafe_output
    ERROR_VARIABLE unsafe_error
  )
  set(unsafe_result "${unsafe_output}${unsafe_error}")
  set(unsafe_ok TRUE)
  if("${unsafe_status}" STREQUAL "0" OR
      NOT "${unsafe_result}" MATCHES "WORK must be a strict child of BUILD" OR
      NOT EXISTS "${sentinel}")
    set(unsafe_ok FALSE)
  endif()
  file(REMOVE_RECURSE "${unsafe_work}")
  if(NOT unsafe_ok)
    message(FATAL_ERROR "retroarch_report: unsafe WORK control failed: ${unsafe_result}")
  endif()
endif()

file(REMOVE_RECURSE "${WORK}")
file(MAKE_DIRECTORY "${WORK}")
file(READ "${ROOT}/.github/workflows/ci.yml" workflow)
function(has_reporter_failure_step text result_var)
  set(header [=[      - name: RetroArch log
        if: failure()
        run: |
]=])
  string(FIND "${text}" "${header}" start)
  set(valid TRUE)
  if(start EQUAL -1)
    set(valid FALSE)
  else()
    string(SUBSTRING "${text}" ${start} -1 tail)
    string(FIND "${tail}" "\n      - uses:" end)
    if(end GREATER_EQUAL 0)
      string(SUBSTRING "${tail}" 0 ${end} step)
    else()
      set(step "${tail}")
    endif()
    foreach(required IN ITEMS
        "cmake -DBUILD=build -P tests/retroarch/report.cmake"
        "report_rc=0"
        "|| report_rc=\$?"
        "exit \"\$report_rc\"")
      string(FIND "${step}" "${required}" found)
      if(found EQUAL -1)
        set(valid FALSE)
      endif()
    endforeach()
  endif()
  set(${result_var} "${valid}" PARENT_SCOPE)
endfunction()

has_reporter_failure_step("${workflow}" reporter_ok)
if(NOT reporter_ok)
  message(FATAL_ERROR "RetroArch failure step must run the tested reporter and preserve its failure status")
endif()

set(no_reporter "${workflow}")
string(REPLACE "cmake -DBUILD=build -P tests/retroarch/report.cmake" "true"
  no_reporter "${no_reporter}")
has_reporter_failure_step("${no_reporter}" no_reporter_ok)
if(no_reporter_ok)
  message(FATAL_ERROR "RetroArch reporter control missed a removed reporter invocation")
endif()

set(swallowed_reporter_failure "${workflow}")
string(REPLACE "exit \"\$report_rc\"" "true"
  swallowed_reporter_failure "${swallowed_reporter_failure}")
has_reporter_failure_step("${swallowed_reporter_failure}" swallowed_failure_ok)
if(swallowed_failure_ok)
  message(FATAL_ERROR "RetroArch reporter control missed a swallowed reporter failure")
endif()
set(markers "")
foreach(case IN ITEMS empty dma input-held input-released battery-1 battery-2)
  if(NOT case STREQUAL empty)
    foreach(name IN ITEMS retroarch.log test.cfg)
      set(marker "evidence-${case}-${name}")
      file(WRITE "${WORK}/ra-${case}/${name}" "${marker}\n")
      list(APPEND markers "${marker}")
    endforeach()
    if(case STREQUAL input-held)
      file(WRITE "${WORK}/frames/input.ratst" "schedule-evidence\n")
      list(APPEND markers schedule-evidence)
    endif()
  endif()
  execute_process(COMMAND "${CMAKE_COMMAND}" "-DBUILD=${WORK}"
    -P "${ROOT}/tests/retroarch/report.cmake"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "${case} report failed: ${rc} ${out}${err}")
  endif()
  set(report "${out}${err}")
  if(case STREQUAL empty AND NOT report MATCHES "No RetroArch launch evidence recorded")
    message(FATAL_ERROR "empty report has no explanation")
  endif()
  foreach(marker IN LISTS markers)
    string(FIND "${report}" "${marker}" at)
    if(at EQUAL -1)
      message(FATAL_ERROR "${case} report omitted ${marker}")
    endif()
  endforeach()
endforeach()
message(STATUS "RetroArch report: empty, singleton and all launch evidence retained")
