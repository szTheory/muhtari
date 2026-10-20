# Instructions-per-frame gate, run as
#   cmake -DROM=<name> -DHEAD=<n> -DBASE1=<n> -DBASE2=<n> -DALLOW=<allow file>
#     -DARMED=<true|false> [-DNO_BASE=true] -P perf_compare.cmake
# Fault prevented: a pull request making the core slower unnoticed. HEAD is
# the Cachegrind Ir/frame of the merge commit, BASE1 and BASE2 two measurements
# of its first parent. Prints one line,
#   perf <rom> head=<n> base=<n> delta_bp=<n> verdict=<PASS|FAIL|UNSTABLE>
# or, with NO_BASE true (the first parent has no tree to build),
#   perf <rom> head=<n> base=none delta_bp=none verdict=NO_BASE
# The limit is +100 basis points, exactly +1.00 % passing; a dated line in the
# allow file raises it for one ROM until its expiry. PASS exits 0; FAIL,
# UNSTABLE and NO_BASE exit 1 only when ARMED is true. An input error prints
# "perf: error <reason>" and exits 1 whatever ARMED is: a missing measurement
# is never a pass. Integer arithmetic only.
cmake_minimum_required(VERSION 3.25)

function(perf_error reason)
  message(FATAL_ERROR "perf: error ${reason}")
endfunction()

# A measurement is a decimal integer without a leading zero; 0 is no
# measurement either.
function(perf_number name)
  if(NOT "${${name}}" MATCHES "^[1-9][0-9]*$")
    perf_error("${name} '${${name}}' is not a measurement")
  endif()
endfunction()

if(NOT ROM MATCHES "^[A-Za-z0-9_.-]+$")
  perf_error("ROM '${ROM}' is not a ROM name")
endif()
perf_number(HEAD)
if(NO_BASE STREQUAL "true")
  if(NOT "${BASE1}${BASE2}" STREQUAL "")
    perf_error("NO_BASE is true but a base was measured")
  endif()
else()
  perf_number(BASE1)
  perf_number(BASE2)
endif()
if(NOT EXISTS "${ALLOW}")
  perf_error("allow file '${ALLOW}' does not exist")
endif()

# Allow lines: <rom> <max % x 100> <expiry YYYY-MM-DD> <reason>, each applying
# from today (UTC, or SOURCE_DATE_EPOCH) through its expiry, at most 60 days
# on. The reason is never printed, so a semicolon or bracket in it is replaced
# before the text is split into lines.
string(TIMESTAMP now "%s" UTC)
string(TIMESTAMP today "%Y-%m-%d" UTC)
math(EXPR later "${now} + 60 * 86400")
set(ENV{SOURCE_DATE_EPOCH} ${later})
string(TIMESTAMP last "%Y-%m-%d" UTC)
set(limit 100)
file(READ "${ALLOW}" text)
string(REGEX REPLACE "[][;\r]" "_" text "${text}")
string(REPLACE "\n" ";" lines "${text}")
set(n 0)
foreach(line IN LISTS lines)
  math(EXPR n "${n} + 1")
  if(line MATCHES "^[ \t_]*$" OR line MATCHES "^#")
    continue()
  endif()
  if(NOT line MATCHES "^([A-Za-z0-9_.-]+) (0|[1-9][0-9]*) ([0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]) [^ ]")
    perf_error("malformed allow line ${n} of ${ALLOW}")
  endif()
  if(CMAKE_MATCH_1 STREQUAL ROM AND NOT CMAKE_MATCH_3 STRLESS today
      AND NOT CMAKE_MATCH_3 STRGREATER last AND CMAKE_MATCH_2 GREATER limit)
    set(limit ${CMAKE_MATCH_2})
  endif()
endforeach()

if(NO_BASE STREQUAL "true")
  set(verdict NO_BASE)
  set(line "perf ${ROM} head=${HEAD} base=none delta_bp=none verdict=NO_BASE")
else()
  math(EXPR delta "${HEAD} - ${BASE1}")
  # Truncated toward zero for the printed value; the decision below is exact.
  math(EXPR delta_bp "${delta} * 10000 / ${BASE1}")
  math(EXPR scaled "${delta} * 10000")
  math(EXPR allowed "${limit} * ${BASE1}")
  if(NOT BASE1 EQUAL BASE2)
    set(verdict UNSTABLE)
  elseif(scaled GREATER allowed)
    set(verdict FAIL)
  else()
    set(verdict PASS)
  endif()
  set(line "perf ${ROM} head=${HEAD} base=${BASE1} delta_bp=${delta_bp} verdict=${verdict}")
endif()

message(NOTICE "${line}")
if(NOT verdict STREQUAL "PASS" AND ARMED STREQUAL "true")
  message(FATAL_ERROR "perf: verdict ${verdict} with the gate armed")
endif()
