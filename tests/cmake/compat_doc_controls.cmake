# Controls for tools/compat_doc.cmake, run as
#   cmake -DTOOL=tools/compat_doc.cmake -DLEDGER=<ledger> -DPAGE=<committed page>
#     -DEXPECTED=<tests/diag/expected.csv> -DINVENTORY=<tests/inventory/ci.txt>
#     -DWORK=<scratch directory> -P tests/cmake/compat_doc_controls.cmake
# Fault prevented: a committed page that disagrees with the ledger, or a
# generator that accepts a ledger claiming more than it shows. Eight cases:
# a stale page is named; a fixture with one row of each kind gets exact counts;
# and each generator guard (status outside the vocabulary, skipped without
# why=, a pr row that is unknown, a pinned-list pass, a token with no claim
# entry, a backing test missing from the inventory) fails with its message.
cmake_minimum_required(VERSION 3.25)
foreach(required TOOL LEDGER PAGE EXPECTED INVENTORY WORK)
  if("${${required}}" STREQUAL "")
    message(FATAL_ERROR "compat_doc_controls: -D${required} is required")
  endif()
endforeach()
file(REMOVE_RECURSE "${WORK}")
file(MAKE_DIRECTORY "${WORK}")

# Runs the generator over a ledger and inventory; sets rc and log.
function(generate ledger inventory out check)
  set(args -DLEDGER=${ledger} -DEXPECTED=${EXPECTED} -DINVENTORY=${inventory} -DOUT=${out})
  if(NOT "${check}" STREQUAL "")
    list(APPEND args -DCHECK=${check})
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} ${args} -P ${TOOL}
    RESULT_VARIABLE rc OUTPUT_VARIABLE log ERROR_VARIABLE log)
  # CMake wraps long messages; compare them as one line.
  string(REGEX REPLACE "[ \n]+" " " log "${log}")
  set(rc "${rc}" PARENT_SCOPE)
  set(log "${log}" PARENT_SCOPE)
endfunction()

# A ledger of the given rows ("@@" stands for the semicolon of a predicate).
function(ledger_file path)
  set(text "id,title,sha1_raw,sha256_raw,sha1_a78,sha256_a78,board,region,peripherals,predicate,status,tier,evidence_commit,evidence,oracle,checked\n")
  foreach(row IN LISTS ARGN)
    string(APPEND text "${row}\n")
  endforeach()
  string(REPLACE "@@" ";" text "${text}")
  file(WRITE "${path}" "${text}")
endfunction()

# One ledger row from its distinguishing columns.
function(row result id board predicate status tier)
  set(${result} "${id},t,-,-,-,-,${board},NTSC,-,${predicate},${status},${tier},-,-,-,-" PARENT_SCOPE)
endfunction()

function(expect_fail name ledger inventory regex)
  generate("${ledger}" "${inventory}" "${WORK}/${name}.md" "")
  if(NOT rc EQUAL 1 OR NOT log MATCHES "${regex}")
    message(FATAL_ERROR "compat_doc_controls: ${name}: unexpected result (exit ${rc}, wanted 1 and ${regex}):\n${log}")
  endif()
  message(STATUS "compat_doc_controls: ${name} ok")
endfunction()

# 1. A copy of the page with one count changed is named stale.
file(READ "${PAGE}" page)
string(REGEX REPLACE ", [0-9]+ not evaluated, " ", 0 not evaluated, " stale "${page}")
if(stale STREQUAL page)
  message(FATAL_ERROR "compat_doc_controls: stale: the replacement left the page unchanged")
endif()
file(WRITE "${WORK}/stale.md" "${stale}")
generate("${LEDGER}" "${INVENTORY}" "${WORK}/stale_generated.md" "${WORK}/stale.md")
if(NOT rc EQUAL 1 OR NOT log MATCHES "stale[.]md is stale")
  message(FATAL_ERROR "compat_doc_controls: stale: unexpected result (exit ${rc}):\n${log}")
endif()
message(STATUS "compat_doc_controls: stale ok")

# 2. One row of each kind gives exact counts.
row(title "7800basic-0.41/a" linear "title@120=1" pass pr)
row(boot "7800basic-0.41/b" linear "boots@120" pass pr)
row(model "diag/c" linear "title@120=2" pass pr)
row(skipped "7800basic-0.41/d" linear "boots@120@@why=slow" skipped local)
row(unknown "pinned_a" linear "boots@120" unknown local)
row(held "pinned_b" linear "boots@120@@why=expansion-device" skipped local)
ledger_file("${WORK}/kinds.csv" "${title}" "${boot}" "${model}" "${skipped}" "${unknown}" "${held}")
generate("${WORK}/kinds.csv" "${INVENTORY}" "${WORK}/kinds.md" "")
file(READ "${WORK}/kinds.md" kinds)
set(zero "| pass | 0 | 0 | 0 | 0 |\n| fail | 0 | 0 | 0 | 0 |\n")
set(tail "| unsupported | 0 | 0 | 0 | 0 |\n| skipped | 0 | 0 | 1 | 1 |\n")
set(head "| Status | pr | nightly | local | total |\n|---|---|---|---|---|\n")
function(require text)
  string(FIND "${kinds}" "${text}" at)
  if(at EQUAL -1)
    message(FATAL_ERROR "compat_doc_controls: kinds: the page lacks\n${text}\nin\n${kinds}")
  endif()
endfunction()
require("2 of 2 pinned-list entries accounted for; 0 pass, 0 fail, 0 unsupported, 1 not evaluated, 1 skipped. Not evaluated means untested, not failing.\n")
require("Pinned CC0 list: 2 entries\n\n${head}${zero}| not evaluated | 0 | 0 | 1 | 1 |\n${tail}\n")
require("Owned samples and diagnostics: 4 rows\n\n${head}| pass | 3 | 0 | 0 | 3 |\n| fail | 0 | 0 | 0 | 0 |\n| not evaluated | 0 | 0 | 0 | 0 |\n${tail}\n")
require("| public title screen | 1 |\n| public boot only | 1 |\n| model conformance | 1 |\n")
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "compat_doc_controls: kinds: exit ${rc}:\n${log}")
endif()
message(STATUS "compat_doc_controls: kinds ok")

# 3 to 7. One fixture ledger per guard.
row(bad "diag/x" linear "title@120=1" maybe pr)
ledger_file("${WORK}/status.csv" "${bad}")
expect_fail(status "${WORK}/status.csv" "${INVENTORY}" "diag/x: status maybe is outside pass")

row(bad "diag/x" linear "boots@120" skipped local)
ledger_file("${WORK}/why.csv" "${bad}")
expect_fail(why "${WORK}/why.csv" "${INVENTORY}" "diag/x: skipped needs why=")

row(bad "diag/x" linear "boots@120" unknown pr)
ledger_file("${WORK}/pr_unknown.csv" "${bad}")
expect_fail(pr_unknown "${WORK}/pr_unknown.csv" "${INVENTORY}" "diag/x: a pr row cannot be unknown")

row(bad "pinned_x" linear "boots@120" pass local)
ledger_file("${WORK}/pinned_pass.csv" "${bad}")
expect_fail(pinned_pass "${WORK}/pinned_pass.csv" "${INVENTORY}" "pinned_x: a pinned-list row cannot be pass")

row(bad "diag/x" mystery "boots@120" unknown local)
ledger_file("${WORK}/token.csv" "${bad}")
expect_fail(token "${WORK}/token.csv" "${INVENTORY}" "mystery has no claim entry")

# 8. An inventory without a backing test fails naming it.
file(READ "${INVENTORY}" inventory)
string(REGEX REPLACE "(^|\n)diag[.]sg144\t[^\n]*" "" inventory "${inventory}")
file(WRITE "${WORK}/inventory.txt" "${inventory}")
expect_fail(inventory "${LEDGER}" "${WORK}/inventory.txt" "names diag[.]sg144, which is not in")
