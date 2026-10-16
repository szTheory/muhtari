# Dependabot auto-merge decision, run as
#   cmake -DWORKFLOW=<dependabot-automerge.yml> -DJQ=<jq> -P dependabot.cmake
# Fault prevented: Dependabot majors merging unattended, a person's push to a
# Dependabot branch merging unattended, or patch and minor updates never
# merging. It runs the decide step's own jq program, with the
# step's own arguments, on synthetic pull request events.
cmake_minimum_required(VERSION 3.25)
foreach(var WORKFLOW JQ)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "dependabot: -D${var} is required")
  endif()
endforeach()
file(READ "${WORKFLOW}" yml)
string(FIND "${yml}" "id: decide" at)
if(at EQUAL -1)
  message(FATAL_ERROR "dependabot: ${WORKFLOW} has no decide step")
endif()
string(SUBSTRING "${yml}" ${at} -1 step)
if(NOT step MATCHES "jq -n -e -r ([^']*)'([^']*)'")
  message(FATAL_ERROR "dependabot: ${WORKFLOW} has no decide step running jq -n -e -r")
endif()
set(program "${CMAKE_MATCH_2}")
string(REGEX MATCHALL "--arg [a-z]+" args "${CMAKE_MATCH_1}")
if(NOT args STREQUAL "--arg login;--arg actor;--arg type")
  message(FATAL_ERROR "dependabot: the step passes '${args}', not login, actor and type")
endif()

# author|actor|update type|want
set(cases
  "dependabot[bot]|dependabot[bot]|version-update:semver-patch|merge"
  "dependabot[bot]|dependabot[bot]|version-update:semver-minor|merge"
  "dependabot[bot]|dependabot[bot]|version-update:semver-major|wait"
  "dependabot[bot]|dependabot[bot]||wait"
  "dependabot[bot]|a-person|version-update:semver-patch|skip"
  "a-person|dependabot[bot]|version-update:semver-patch|skip"
  "a-person|a-person|version-update:semver-patch|skip")
set(bad "")
foreach(case IN LISTS cases)
  string(REPLACE "|" ";" f "${case}")
  list(GET f 0 login)
  list(GET f 1 actor)
  list(GET f 2 type)
  list(GET f 3 want)
  execute_process(COMMAND ${JQ} -n -e -r --arg login "${login}" --arg actor "${actor}" --arg type "${type}"
    "${program}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE got ERROR_VARIABLE err OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT rc STREQUAL "0" OR NOT got STREQUAL want)
    list(APPEND bad "${login} ${actor} ${type}: want ${want}, got '${got}' exit ${rc} ${err}")
  endif()
endforeach()
list(LENGTH cases n)
if(bad)
  list(JOIN bad "\n  " bad)
  message(FATAL_ERROR "dependabot:\n  ${bad}")
endif()
message(STATUS "dependabot: ${n} cases")
