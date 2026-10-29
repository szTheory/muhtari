# Controls for the compatibility ratchet, run as cmake -DRATCHET=<script>
#   -DGIT=<git> -DWORK=<scratch dir, wiped> -DLEDGER=<real ledger.csv> -P
# Fault prevented: a pull request lowering or demoting a passing ledger row
# without an announced, dated exception. The real ratchet runs against a
# fixture git repository whose one commit is the base; each case edits the
# ledger, supplies results equal to it (so only the base comparison can fail)
# and its own allow file, and must give the exit code and message below.
cmake_minimum_required(VERSION 3.25)

# One fixed clock for the ratchet and for the dates written here, so a run
# across midnight UTC cannot split them.
string(TIMESTAMP now "%s" UTC)
set(ENV{SOURCE_DATE_EPOCH} ${now})
string(TIMESTAMP today "%Y-%m-%d" UTC)
math(EXPR before "${now} - 86400")
set(ENV{SOURCE_DATE_EPOCH} ${before})
string(TIMESTAMP yesterday "%Y-%m-%d" UTC)
set(ENV{SOURCE_DATE_EPOCH} ${now})

file(STRINGS "${LEDGER}" head LIMIT_COUNT 1)
file(REMOVE_RECURSE "${WORK}")
set(repo "${WORK}/repo")
set(ledger "${repo}/tests/compat/ledger.csv")

# Writes path with the header and the rows "<id>:<status>:<tier>".
function(controls_ledger path)
  set(text "${head}\n")
  foreach(spec IN LISTS ARGN)
    string(REPLACE ":" ";" f "${spec}")
    list(GET f 0 id)
    list(GET f 1 status)
    list(GET f 2 tier)
    string(APPEND text "${id},Fixture ${id},-,-,-,-,linear,NTSC,-,boots@120,${status},${tier},-,ev-${id},-,-\n")
  endforeach()
  file(WRITE "${path}" "${text}")
endfunction()

function(controls_git)
  execute_process(COMMAND ${GIT} -C ${repo} -c user.name=fixture
    -c user.email=fixture@users.noreply.github.com -c commit.gpgsign=false
    -c core.hooksPath=/dev/null ${ARGN} COMMAND_ERROR_IS_FATAL ANY
    OUTPUT_QUIET)
endfunction()

controls_ledger("${ledger}" fx1:pass:pr fx2:pass:pr)
controls_git(init -q)
controls_git(add tests/compat/ledger.csv)
controls_git(commit -q -m base)
execute_process(COMMAND ${GIT} -C ${repo} rev-parse HEAD
  OUTPUT_VARIABLE base OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)

# controls_case(<name> <want exit> <want regex or -> <allow line or -> rows...)
function(controls_case name want regex allow)
  controls_ledger("${ledger}" ${ARGN})
  set(results "id,predicate,status,evidence\n")
  foreach(spec IN LISTS ARGN)
    string(REPLACE ":" ";" f "${spec}")
    list(GET f 0 id)
    list(GET f 1 status)
    list(GET f 2 tier)
    if(tier STREQUAL "pr")
      string(APPEND results "${id},boots@120,${status},ev-${id}\n")
    endif()
  endforeach()
  file(WRITE "${WORK}/results.csv" "${results}")
  set(text "# fixture allow file\n")
  if(NOT allow STREQUAL "-")
    string(REPLACE "DATE" "${allow}" line "fx1 boots@120 DATE fixture")
    string(APPEND text "${line}\n")
  endif()
  file(WRITE "${WORK}/allow.txt" "${text}")
  execute_process(COMMAND ${CMAKE_COMMAND} -DLEDGER=${ledger}
    -DRESULTS=${WORK}/results.csv -DBASE=${base} -DALLOW=${WORK}/allow.txt
    -P ${RATCHET} RESULT_VARIABLE code ERROR_VARIABLE err OUTPUT_QUIET)
  if(NOT code STREQUAL want)
    message(FATAL_ERROR "controls: ${name}: want exit ${want}, got ${code}\n${err}")
  endif()
  if(NOT regex STREQUAL "-" AND NOT err MATCHES "${regex}")
    message(FATAL_ERROR "controls: ${name}: output lacks '${regex}'\n${err}")
  endif()
  message(STATUS "controls: ${name}: exit ${code} as wanted")
endfunction()

controls_case("lowered row is named" 1
  "fx1 boots@120: pass at ${base}, now 'fail' with no allow line" -
  fx1:fail:pr fx2:pass:pr)
controls_case("exception expiring today passes" 0 - "${today}"
  fx1:fail:pr fx2:pass:pr)
controls_case("exception expired yesterday is named" 1
  "fx1 boots@120: pass at ${base}, now 'fail' with no allow line" "${yesterday}"
  fx1:fail:pr fx2:pass:pr)
controls_case("demotion to local is named" 1
  "fx1 boots@120: pr at ${base}, now 'local' with no allow line" -
  fx1:pass:local fx2:pass:pr)
controls_case("unchanged ledger passes" 0 - -
  fx1:pass:pr fx2:pass:pr)
