# The documented exit statuses, run as
#   cmake -DRUN=<muhtari-run> -DDOC=<docs/runner.md> -P exit_table.cmake
# Fault prevented: the exit statuses in docs/runner.md drift from the ones
# the binary prints with --help. Every table line --help prints after
# "exit status:" must appear as a whole line in the document.
execute_process(COMMAND "${RUN}" --help OUTPUT_VARIABLE out RESULT_VARIABLE rc)
string(FIND "${out}" "exit status:\n" at)
if(NOT rc STREQUAL "0" OR at EQUAL -1)
  message(FATAL_ERROR "muhtari-run --help: expected exit 0 and an exit status table, got exit ${rc}\n${out}")
endif()
math(EXPR at "${at} + 13")
string(SUBSTRING "${out}" ${at} -1 table)
string(REGEX MATCHALL "[^\n]+" rows "${table}")
file(READ "${DOC}" doc)
foreach(row IN LISTS rows)
  string(FIND "\n${doc}" "\n${row}\n" found)
  if(found EQUAL -1)
    message(FATAL_ERROR "${DOC} lacks the exit status line --help prints:\n${row}")
  endif()
endforeach()
list(LENGTH rows n)
message(STATUS "exit_table: ${n} lines of --help found in ${DOC}")
