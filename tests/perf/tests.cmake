# Instructions-per-frame gate tests.
#
# This file is also the checks themselves, run as
#   cmake -DSCRIPT=<perf_compare.cmake> -DDIR=<fixture dir> -DCASE=<name> -P tests/perf/tests.cmake
# Each case runs the gate on synthetic Ir/frame values through
# expect_fail.cmake, so it passes only on the exact exit status and verdict.
# The date is fixed, so no case depends on the clock.
if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
  # 2026-01-02 UTC.
  set(ENV{SOURCE_DATE_EPOCH} 1767312000)

  # Runs the gate with ARGN; requires exit code and output matching regex.
  function(gate code regex)
    set(CMD ${CMAKE_COMMAND} -DROM=testcard -DALLOW=${DIR}/empty.txt ${ARGN} -P "${SCRIPT}")
    set(CODE ${code})
    set(REGEX "${regex}")
    include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cmake/expect_fail.cmake)
  endfunction()

  set(empty -DPOLICY=${DIR}/empty.json -DBASE_POLICY=${DIR}/empty.json)
  set(base -DBASE1=1000000 -DBASE2=1000000)
  if(CASE STREQUAL "identical")
    # Fault prevented: an unchanged core reported slower.
    gate(0 "head=1000000 base=1000000 delta_bp=0 verdict=PASS" -DHEAD=1000000 ${base} ${empty} -DARMED=true)
  elseif(CASE STREQUAL "plus_1_00")
    # Fault prevented: the boundary moving inside the limit, so exactly +1.00 % fails.
    gate(0 "delta_bp=100 verdict=PASS" -DHEAD=1010000 ${base} ${empty} -DARMED=true)
  elseif(CASE STREQUAL "plus_1_01")
    # Fault prevented: a slowdown just past +1.00 % passing an armed gate, or
    # failing a gate that only reports.
    gate(1 "delta_bp=101 verdict=FAIL" -DHEAD=1010100 ${base} ${empty} -DARMED=true)
    gate(0 "delta_bp=101 verdict=FAIL" -DHEAD=1010100 ${base} ${empty} -DARMED=false)
  elseif(CASE STREQUAL "unstable")
    # Fault prevented: a noisy base measurement read as a verdict.
    gate(1 "base=1000000 delta_bp=0 verdict=UNSTABLE"
      -DHEAD=1000000 -DBASE1=1000000 -DBASE2=1000001 ${empty} -DARMED=true)
  elseif(CASE STREQUAL "allowed")
    # Fault prevented: central reasons ignored, inherited forever or applied to another bound.
    gate(0 "delta_bp=200 verdict=PASS" -DHEAD=1020000 ${base} ${empty} -DPOLICY=${DIR}/current.json -DARMED=true)
    foreach(case expired lifetime unrelated)
      gate(1 "quality: .*reason" -DHEAD=1020000 ${base} ${empty} -DPOLICY=${DIR}/${case}.json -DARMED=true)
    endforeach()
    gate(1 "quality: .*reason" -DHEAD=1020000 ${base} ${empty}
      -DBASE_POLICY=${DIR}/expired.json -DARMED=false)
  elseif(CASE STREQUAL "no_base")
    # Fault prevented: a missing base tree passing an armed gate, failing a
    # gate that only reports, or hiding a missing head measurement.
    set(none -DNO_BASE=true -DBASE1= -DBASE2=)
    gate(0 "head=1000000 base=none delta_bp=none verdict=NO_BASE" -DHEAD=1000000 ${none} ${empty} -DARMED=false)
    gate(1 "verdict=NO_BASE" -DHEAD=1000000 ${none} ${empty} -DARMED=true)
    gate(1 "perf: error HEAD" -DHEAD= ${none} ${empty} -DARMED=false)
  elseif(CASE STREQUAL "missing")
    # Fault prevented: an empty base measurement read as a pass.
    gate(1 "perf: error BASE1" -DHEAD=1000000 -DBASE1= -DBASE2=1000000 ${empty} -DARMED=false)
  else()
    message(FATAL_ERROR "perf tests: unknown CASE '${CASE}'")
  endif()
  return()
endif()

set(perf_dir ${CMAKE_BINARY_DIR}/perf)
file(WRITE ${perf_dir}/empty.txt "# no exceptions\n")
set(policy [=[{"domains":{"metrics":{"ir_per_frame.testcard":{"ceiling":100,"direction":"ceiling"}}}}]=])
file(WRITE ${perf_dir}/empty.json "${policy}")
string(JSON policy SET "${policy}" domains metrics ir_per_frame.testcard ceiling 300)
set(reason [=[{"key":"ir_per_frame.testcard","from":100,"to":300,"date":"2026-01-01","expires":"2026-02-01","why":"reviewed slowdown"}]=])
foreach(case current expired lifetime unrelated)
  set(r "${reason}")
  if(case STREQUAL "expired")
    string(JSON r SET "${r}" expires "\"2026-01-01\"")
  elseif(case STREQUAL "lifetime")
    string(JSON r SET "${r}" expires "\"2026-04-01\"")
  elseif(case STREQUAL "unrelated")
    string(JSON r SET "${r}" to 301)
  endif()
  string(JSON p SET "${policy}" domains reasons "[${r}]")
  file(WRITE ${perf_dir}/${case}.json "${p}")
endforeach()
foreach(case allowed identical missing no_base plus_1_00 plus_1_01 unstable)
  muhtari_add_test(NAME perf.gate.${case}
    COMMAND ${CMAKE_COMMAND} -DSCRIPT=${CMAKE_CURRENT_LIST_DIR}/../cmake/perf_compare.cmake
      -DDIR=${perf_dir} -DCASE=${case} -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS perf TIMEOUT 10)
endforeach()
