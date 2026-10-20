# The compatibility ledger's ratchet, run as
#   cmake -DLEDGER=<ledger.csv> -DRESULTS=<results.csv> -P compat_ratchet.cmake
# Fault prevented: a ledger that claims what the core no longer does, or
# that hides what it now does. Fresh results must equal the ledger's
# evaluated rows (tier pr, neither unsupported nor skipped) in both
# directions by status and evidence (CONFORMANCE section 5), so an
# improvement is committed with the change that makes it. A field may hold
# no comma, quote or newline: a row with another field count, a quote or a
# carriage return fails.
cmake_minimum_required(VERSION 3.25)
foreach(var LEDGER RESULTS)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "ratchet: -D${var} is required")
  endif()
endforeach()
set(bad "")

# Sets rows to file's rows after its header, each a list of fields with a
# ";" kept as <semicolon>; a row without want fields is reported.
macro(ratchet_read file want)
  file(READ "${file}" text)
  if(text MATCHES "[\"\r]")
    list(APPEND bad "${file} holds a quote or a carriage return")
  endif()
  string(REPLACE ";" "<semicolon>" text "${text}")
  string(REGEX REPLACE "\n$" "" text "${text}")
  string(REPLACE "\n" ";" rows "${text}")
  list(POP_FRONT rows)
  set(n 1)
  foreach(row IN LISTS rows)
    math(EXPR n "${n} + 1")
    string(REPLACE "," ";" f "${row}")
    list(LENGTH f count)
    if(NOT count EQUAL ${want})
      list(APPEND bad "${file} line ${n} has ${count} fields, not ${want}")
    endif()
  endforeach()
endmacro()

set(keys "")
ratchet_read("${LEDGER}" 16)
foreach(row IN LISTS rows)
  string(REPLACE "," ";" f "${row}")
  list(GET f 0 9 10 11 13 g)
  list(POP_FRONT g id predicate status tier evidence)
  if(tier STREQUAL "pr" AND NOT status MATCHES "^(unsupported|skipped)$")
    set("ledger ${id} ${predicate}" "${status} ${evidence}")
    list(APPEND keys "${id} ${predicate}")
  endif()
endforeach()
ratchet_read("${RESULTS}" 4)
foreach(row IN LISTS rows)
  string(REPLACE "," ";" f "${row}")
  list(POP_FRONT f id predicate status evidence)
  set(key "${id} ${predicate}")
  set(name "ledger ${key}")
  if(NOT DEFINED "${name}")
    list(APPEND bad "${key}: a result, but no evaluated ledger row")
  elseif(NOT "${${name}}" STREQUAL "${status} ${evidence}")
    list(APPEND bad "${key}: ledger ${${name}}, results ${status} ${evidence}")
  endif()
  list(REMOVE_ITEM keys "${key}")
endforeach()
foreach(key IN LISTS keys)
  list(APPEND bad "${key}: an evaluated ledger row, but no result")
endforeach()

if(bad)
  list(JOIN bad "\n  " bad)
  string(REPLACE "<semicolon>" ";" bad "${bad}")
  message(FATAL_ERROR "ratchet:\n  ${bad}")
endif()
message(STATUS "ratchet: results equal the ledger")
