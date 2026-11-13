# Fault prevented: the slow, non-gating 10,000-case property report consuming
# pull-request runner time after required CI already runs the default 100 cases.
cmake_minimum_required(VERSION 4.3)

file(READ "${ROOT}/.github/workflows/nightly.yml" workflow)
file(READ "${ROOT}/tests/prop.h" properties)

function(nightly_property_scope_valid content result)
  string(REGEX MATCH "\n  prop:\n((    [^\n]*\n|\n)*)" prop_job "${content}")
  set(valid TRUE)
  if(prop_job STREQUAL "" OR NOT prop_job MATCHES "\n    if: github.event_name != 'pull_request'\n")
    set(valid FALSE)
  endif()
  set(${result} "${valid}" PARENT_SCOPE)
endfunction()

nightly_property_scope_valid("${workflow}" valid)
if(NOT valid)
  message(FATAL_ERROR "nightly: the 10,000-case property report must skip pull requests")
endif()

string(FIND "${properties}" "prop_env(PROP_ENV_CASES, 100u)" default_case_count)
if(default_case_count LESS 0)
  message(FATAL_ERROR "nightly: retain the default 100-case property coverage used by required CI")
endif()

string(REPLACE "    if: github.event_name != 'pull_request'\n" "" unguarded "${workflow}")
nightly_property_scope_valid("${unguarded}" fault_detected)
if(fault_detected)
  message(FATAL_ERROR "nightly: control failed to reject the unguarded property report")
endif()
