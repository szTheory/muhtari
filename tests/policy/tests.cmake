# Repository policy tests: rules about the tests themselves.
set(policy_scripts ${CMAKE_CURRENT_LIST_DIR}/../cmake)

muhtari_add_test(NAME policy.no_skip
  COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR} -P ${policy_scripts}/no_skip.cmake
  LABELS policy TIMEOUT 10)

# The self test scans a tree holding one forbidden line and expects that line
# named. The property name is built in pieces so policy.no_skip passes here.
set(policy_rule SKIP_)
string(APPEND policy_rule RETURN_CODE)
set(policy_fixture ${CMAKE_BINARY_DIR}/policy/no_skip_fixture)
file(WRITE ${policy_fixture}/CMakeLists.txt
  "set_tests_properties(t PROPERTIES ${policy_rule} 77)\n")
set(policy_cmd ${CMAKE_COMMAND} -DROOT=${policy_fixture} -P ${policy_scripts}/no_skip.cmake)
string(REPLACE ";" "$<SEMICOLON>" policy_cmd "${policy_cmd}")
muhtari_add_test(NAME policy.no_skip.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${policy_cmd}" -DCODE=1
    "-DREGEX=CMakeLists.txt:1: ${policy_rule}" -P ${policy_scripts}/expect_fail.cmake
  LABELS policy TIMEOUT 10)

# One version source. The tree is checked at the current version and at the
# next minor and next patch, the versions a release pull request writes into
# version.txt; the self test is a fixture repository holding its version in
# a second tracked file and expects that file named.
muhtari_add_test(NAME release.single_version_source
  COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR} -P ${policy_scripts}/single_version.cmake
  LABELS policy TIMEOUT 30)
muhtari_add_test(NAME release.single_version_source.next
  COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR} -DNEXT=ON
    -P ${policy_scripts}/single_version.cmake
  LABELS policy TIMEOUT 30)

find_package(Git REQUIRED)
set(policy_repo ${CMAKE_BINARY_DIR}/policy/version_fixture)
file(REMOVE_RECURSE ${policy_repo})
file(WRITE ${policy_repo}/version.txt "1.2.3\n")
file(WRITE ${policy_repo}/notes.md "Released as 1.2.3 today.\n")
execute_process(COMMAND ${GIT_EXECUTABLE} init -q WORKING_DIRECTORY ${policy_repo}
  COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND ${GIT_EXECUTABLE} add version.txt notes.md
  WORKING_DIRECTORY ${policy_repo} COMMAND_ERROR_IS_FATAL ANY)
set(policy_cmd ${CMAKE_COMMAND} -DROOT=${policy_repo} -P ${policy_scripts}/single_version.cmake)
string(REPLACE ";" "$<SEMICOLON>" policy_cmd "${policy_cmd}")
muhtari_add_test(NAME release.single_version_source.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${policy_cmd}" -DCODE=1
    "-DREGEX=notes.md holds 1.2.3" -P ${policy_scripts}/expect_fail.cmake
  LABELS policy TIMEOUT 30)

muhtari_add_test(NAME docs.budgets
  COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR} -P ${policy_scripts}/budgets.cmake
  LABELS policy TIMEOUT 10)

# Every public name in the header appears in the embedding guide, and the
# guide names nothing the header lacks. The self test is a header declaring a
# call its guide omits beside a guide naming a call its header lacks, and
# expects both named.
muhtari_add_test(NAME docs.api_names
  COMMAND ${CMAKE_COMMAND} -DHEADER=${CMAKE_SOURCE_DIR}/include/muhtari.h
    -DDOC=${CMAKE_SOURCE_DIR}/docs/embedding.md -P ${policy_scripts}/api_names.cmake
  LABELS policy TIMEOUT 10)
set(policy_names ${CMAKE_BINARY_DIR}/policy/api_names_fixture)
file(WRITE ${policy_names}/muhtari.h "#define MUHTARI_KEPT 1\nint muhtari_added(void);\n")
file(WRITE ${policy_names}/embedding.md "Use `MUHTARI_KEPT` and `muhtari_removed`.\n")
set(policy_cmd ${CMAKE_COMMAND} -DHEADER=${policy_names}/muhtari.h
  -DDOC=${policy_names}/embedding.md -P ${policy_scripts}/api_names.cmake)
string(REPLACE ";" "$<SEMICOLON>" policy_cmd "${policy_cmd}")
muhtari_add_test(NAME docs.api_names.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${policy_cmd}" -DCODE=1
    "-DREGEX=muhtari_added is declared in the header but the guide never names it.*muhtari_removed is named in the guide but the header does not declare it"
    -P ${policy_scripts}/expect_fail.cmake
  LABELS policy TIMEOUT 10)

# Each derived file has a notice, each notice has a file, and the libretro
# .info licence field agrees with both.
muhtari_add_test(NAME policy.notices
  COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR} -P ${policy_scripts}/notices.cmake
  LABELS policy TIMEOUT 10)

# docs/compatibility.md equals tools/compat_doc.cmake's output over the
# ledger. The self test checks a copy of the page whose pass count is changed
# and expects that copy named stale.
set(policy_compat_cmd ${CMAKE_COMMAND} -DLEDGER=${CMAKE_SOURCE_DIR}/tests/compat/ledger.csv
  -DOUT=${CMAKE_BINARY_DIR}/policy/compatibility.md)
set(policy_compat_tool -P ${CMAKE_SOURCE_DIR}/tools/compat_doc.cmake)
muhtari_add_test(NAME policy.compat_doc_fresh
  COMMAND ${policy_compat_cmd} -DCHECK=${CMAKE_SOURCE_DIR}/docs/compatibility.md ${policy_compat_tool}
  LABELS policy TIMEOUT 10)
file(READ ${CMAKE_SOURCE_DIR}/docs/compatibility.md policy_page)
string(REGEX REPLACE "\n[|] pass [|] [0-9]+ [|]\n" "\n| pass | 0 |\n" policy_page "${policy_page}")
file(WRITE ${CMAKE_BINARY_DIR}/policy/compat_doc_fixture/compatibility.md "${policy_page}")
set(policy_cmd ${CMAKE_COMMAND} -DLEDGER=${CMAKE_SOURCE_DIR}/tests/compat/ledger.csv
  -DOUT=${CMAKE_BINARY_DIR}/policy/compat_doc_fixture/generated.md
  -DCHECK=${CMAKE_BINARY_DIR}/policy/compat_doc_fixture/compatibility.md ${policy_compat_tool})
string(REPLACE ";" "$<SEMICOLON>" policy_cmd "${policy_cmd}")
muhtari_add_test(NAME policy.compat_doc_fresh.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${policy_cmd}" -DCODE=1
    "-DREGEX=compat_doc_fixture/compatibility.md.+is stale" -P ${policy_scripts}/expect_fail.cmake
  LABELS policy TIMEOUT 10)

if(MUHTARI_INVENTORY STREQUAL "ci")
  muhtari_add_test(NAME policy.quality.controls
    COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR}
      -DWORK=${CMAKE_BINARY_DIR}/policy/quality_fixture -P ${CMAKE_CURRENT_LIST_DIR}/quality.cmake
    LABELS policy TIMEOUT 30)
  # Provisional n=1: Windows arm64 image 20261018.189.1, run 39699026057.
  # Complete control max 96.22 s; ceil(2 x 96.22)=193 s, with all semantic cases retained.
  muhtari_add_test(NAME policy.digest.controls
    COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR}
      -DWORK=${CMAKE_BINARY_DIR}/policy/digest_fixture -P ${CMAKE_CURRENT_LIST_DIR}/digest.cmake
    LABELS policy TIMEOUT 193)
  # Fault prevented: comparing empty, duplicate or unrelated lane evidence.
  file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/policy/lane_equality_controls.cmake CONTENT [=[
string(REPEAT "a" 40 sha)
foreach(path absolute relative)
if(path STREQUAL "relative")
  file(RELATIVE_PATH WORK "${CMAKE_CURRENT_BINARY_DIR}" "${WORK}")
endif()
foreach(case valid sha config empty missing extra duplicate difference)
  file(REMOVE_RECURSE "${WORK}")
  foreach(lane native be)
    file(WRITE "${WORK}/${lane}/hashes.txt" "frame1 abc\n")
    file(WRITE "${WORK}/${lane}/commit.txt" "${sha}\n")
    file(WRITE "${WORK}/${lane}/identity.txt" "fixtures abc\noptions Release full\n")
  endforeach()
  set(be "${WORK}/be")
  if(case STREQUAL "sha")
    file(WRITE "${be}/commit.txt" "wrong\n")
    set(regex "lane_equality: commit")
  elseif(case STREQUAL "config")
    file(WRITE "${be}/identity.txt" "different\n")
    set(regex "lane_equality: configuration")
  elseif(case STREQUAL "empty")
    file(WRITE "${be}/hashes.txt" "")
    set(regex "lane_equality: empty")
  elseif(case STREQUAL "missing")
    file(REMOVE "${be}/commit.txt")
    set(regex "lane_equality: members")
  elseif(case STREQUAL "extra")
    file(WRITE "${be}/extra.txt" "extra\n")
    set(regex "lane_equality: members")
  elseif(case STREQUAL "duplicate")
    set(be "${WORK}/native")
    set(regex "lane_equality: duplicate")
  elseif(case STREQUAL "difference")
    file(APPEND "${be}/hashes.txt" "changed\n")
    set(regex "differs from")
  endif()
  set(cmd "${CMAKE_COMMAND}" -DNATIVE=${WORK}/native -DBE=${be} -DEXPECTED_SHA=${sha}
    -P "${ROOT}/tests/cmake/lane_equality.cmake")
  if(case STREQUAL "valid")
    execute_process(COMMAND ${cmd} COMMAND_ERROR_IS_FATAL ANY)
  else()
    execute_process(COMMAND "${CMAKE_COMMAND}" "-DCMD=${cmd}" -DCODE=1 "-DREGEX=${regex}"
      -P "${ROOT}/tests/cmake/expect_fail.cmake" COMMAND_ERROR_IS_FATAL ANY)
  endif()
endforeach()
endforeach()
]=])
  muhtari_add_test(NAME policy.lane_equality.controls
    COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR}
      -DWORK=${CMAKE_BINARY_DIR}/policy/lane_equality_fixture
      -P ${CMAKE_BINARY_DIR}/policy/lane_equality_controls.cmake
    LABELS policy TIMEOUT 30)
  # Fault prevented: incorrect physical counts or invalid inventories accepted as evidence.
  file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/policy/gate_count_controls.cmake CONTENT [=[
file(REMOVE_RECURSE "${WORK}")
file(MAKE_DIRECTORY "${WORK}/tests/cmake" "${WORK}/tests/policy" "${WORK}/src")
file(COPY "${ROOT}/tests/cmake/gate_count.cmake" DESTINATION "${WORK}/tests/cmake")
file(WRITE "${WORK}/sample.txt" "# comment\n\nvalue\nlast")
file(WRITE "${WORK}/src/core.c" "one\n\ntwo\n")
file(WRITE "${WORK}/src/cartdb.c" "generated\n")
set(base "tests/cmake/gate_count.cmake\ntests/policy/gate-lines.txt\n")
file(WRITE "${WORK}/tests/policy/gate-lines.txt" "${base}sample.txt:1:3\n")
execute_process(COMMAND "${CMAKE_COMMAND}" -DROOT=${WORK}
  -DINVENTORY=tests/policy/gate-lines.txt -DOUT=${WORK}/count.json
  -P "${ROOT}/tests/cmake/gate_count.cmake" RESULT_VARIABLE rc)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "gate_count controls: valid fixture failed")
endif()
file(READ "${WORK}/tests/cmake/gate_count.cmake" counter)
string(REGEX MATCHALL "\n" newlines "${counter}")
list(LENGTH newlines counter_lines)
math(EXPR expected "${counter_lines} + 3 + 3")
file(READ "${WORK}/count.json" report)
string(JSON got GET "${report}" gate_lines)
if(NOT got EQUAL expected)
  message(FATAL_ERROR "physical count: expected ${expected}, got ${got}")
endif()
foreach(field full_source authored_source)
  string(JSON got GET "${report}" ${field})
  set(expected 4)
  if(field STREQUAL "authored_source")
    set(expected 3)
  endif()
  if(NOT got EQUAL expected)
    message(FATAL_ERROR "${field}: expected ${expected}, got ${got}")
  endif()
endforeach()

foreach(case missing empty duplicate overlap whole alias endpoint reversed counter inventory unterminated)
  set(rows "${base}sample.txt:1:3\n")
  set(regex "gate_count: missing file")
  if(case STREQUAL "missing")
    string(APPEND rows "absent.txt\n")
  elseif(case STREQUAL "empty")
    set(rows "# empty\n")
    set(regex "gate_count: empty inventory")
  elseif(case STREQUAL "duplicate")
    string(APPEND rows "sample.txt:1:3\n")
    set(regex "gate_count: overlapping ranges")
  elseif(case STREQUAL "overlap")
    string(APPEND rows "sample.txt:3:4\n")
    set(regex "gate_count: overlapping ranges")
  elseif(case STREQUAL "whole")
    string(APPEND rows "sample.txt\n")
    set(regex "gate_count: overlapping ranges")
  elseif(case STREQUAL "alias")
    string(APPEND rows "./sample.txt:1:3\n")
    set(regex "gate_count: overlapping ranges")
  elseif(case STREQUAL "endpoint")
    string(APPEND rows "sample.txt:5:6\n")
    set(regex "gate_count: invalid endpoints")
  elseif(case STREQUAL "reversed")
    string(APPEND rows "sample.txt:3:2\n")
    set(regex "gate_count: invalid endpoints")
  elseif(case STREQUAL "counter")
    string(REPLACE "tests/cmake/gate_count.cmake\n" "" rows "${rows}")
    set(regex "gate_count: counter/inventory must be whole files")
  elseif(case STREQUAL "inventory")
    string(REPLACE "tests/policy/gate-lines.txt\n" "" rows "${rows}")
    set(regex "gate_count: counter/inventory must be whole files")
  elseif(case STREQUAL "unterminated")
    set(rows "${base}sample.txt\n")
    file(WRITE "${WORK}/tests/policy/gate-lines.txt" "${rows}")
    execute_process(COMMAND "${CMAKE_COMMAND}" -DROOT=${WORK}
      -DINVENTORY=tests/policy/gate-lines.txt -DOUT=${WORK}/count.json
      -P "${ROOT}/tests/cmake/gate_count.cmake" COMMAND_ERROR_IS_FATAL ANY)
    file(READ "${WORK}/count.json" report)
    string(JSON got GET "${report}" gate_lines)
    math(EXPR expected "${counter_lines} + 3 + 4")
    if(NOT got EQUAL expected)
      message(FATAL_ERROR "unterminated line: expected ${expected}, got ${got}")
    endif()
    continue()
  endif()
  file(WRITE "${WORK}/tests/policy/gate-lines.txt" "${rows}")
  set(cmd "${CMAKE_COMMAND}" -DROOT=${WORK} -DINVENTORY=tests/policy/gate-lines.txt
    -DOUT=${WORK}/count.json -P "${ROOT}/tests/cmake/gate_count.cmake")
  execute_process(COMMAND "${CMAKE_COMMAND}" "-DCMD=${cmd}" -DCODE=1 "-DREGEX=${regex}"
    -P "${ROOT}/tests/cmake/expect_fail.cmake" COMMAND_ERROR_IS_FATAL ANY)
endforeach()
]=])
  muhtari_add_test(NAME policy.gate_count.controls
    COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR}
      -DWORK=${CMAKE_BINARY_DIR}/policy/gate_count_fixture
      -P ${CMAKE_BINARY_DIR}/policy/gate_count_controls.cmake
    LABELS policy TIMEOUT 30)
endif()
