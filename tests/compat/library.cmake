# The library preset (docs/emulation-tests.md, ledger): every ledger row of
# tier pr that is not unsupported or skipped is judged afresh on the file
# whose SHA-1 the row names, on the fast boot and through a BIOS, and the
# ratchet compares the results with the ledger. MUHTARI_ROM_DIR (required)
# and tests/diag are searched; MUHTARI_BIOS names the BIOS image, by default
# the ci preset's OpenBIOS. A relative path is taken from the source tree.
# Configure fails on a missing directory, image or row file: nothing is
# skipped.
#
# Run with -P, this file is library.results: it joins the per-row results
# under -DDIR, which must number -DCOUNT, into -DOUT, sorted by id then
# predicate.
if(CMAKE_SCRIPT_MODE_FILE)
  file(GLOB files ${DIR}/*.csv)
  list(LENGTH files n)
  if(NOT n EQUAL COUNT)
    message(FATAL_ERROR "library: ${n} results in ${DIR}, not ${COUNT}")
  endif()
  set(rows "")
  foreach(file IN LISTS files)
    file(READ ${file} line)
    string(REPLACE ";" "<semicolon>" line "${line}")
    string(STRIP "${line}" line)
    string(REGEX MATCH "^([^,]*),([^,]*)," unused "${line}")
    list(APPEND rows "${CMAKE_MATCH_1} ${CMAKE_MATCH_2}\t${line}")
  endforeach()
  list(SORT rows)
  list(TRANSFORM rows REPLACE "^[^\t]*\t" "")
  list(JOIN rows "\n" text)
  string(REPLACE "<semicolon>" ";" text "id,predicate,status,evidence\n${text}\n")
  file(WRITE ${OUT} "${text}")
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/targets.cmake)

set(library_dir "$ENV{MUHTARI_ROM_DIR}")
if(library_dir STREQUAL "")
  message(FATAL_ERROR "library: set MUHTARI_ROM_DIR to the directory of ROMs to judge, "
    "such as build/ci/built after the ci preset's built tests")
endif()
cmake_path(ABSOLUTE_PATH library_dir BASE_DIRECTORY ${CMAKE_SOURCE_DIR} NORMALIZE)
if(NOT IS_DIRECTORY ${library_dir})
  message(FATAL_ERROR "library: MUHTARI_ROM_DIR ${library_dir} is not a directory")
endif()
set(library_bios "$ENV{MUHTARI_BIOS}")
if(library_bios STREQUAL "")
  set(library_bios build/ci/built/openbios/openbios.bin)
endif()
cmake_path(ABSOLUTE_PATH library_bios BASE_DIRECTORY ${CMAKE_SOURCE_DIR} NORMALIZE)
if(NOT EXISTS ${library_bios} OR IS_DIRECTORY ${library_bios})
  message(FATAL_ERROR "library: the BIOS image ${library_bios} is missing; build it with "
    "the ci preset's built tests or set MUHTARI_BIOS")
endif()

# A file's SHA-1 is the row's headered SHA-1 for a headered file and its
# headerless SHA-1 for a headerless one; a headered match is preferred.
file(GLOB_RECURSE library_files LIST_DIRECTORIES false ${library_dir}/*
  ${CMAKE_SOURCE_DIR}/tests/diag/*.a78)
list(SORT library_files)
foreach(file IN LISTS library_files)
  file(SHA1 ${file} sha)
  if(NOT DEFINED library_sha_${sha})
    set(library_sha_${sha} ${file})
  endif()
endforeach()

set(library_results ${PROJECT_BINARY_DIR}/results)
file(REMOVE_RECURSE ${library_results})
file(MAKE_DIRECTORY ${library_results})
file(READ ${compat_ledger} library_text)
string(REPLACE ";" "<semicolon>" library_text "${library_text}")
string(REGEX REPLACE "\n$" "" library_text "${library_text}")
string(REPLACE "\n" ";" library_lines "${library_text}")
list(POP_FRONT library_lines)
set(library_tests "")
set(library_flipped "id,predicate,status,evidence")
foreach(line IN LISTS library_lines)
  string(REPLACE "," ";" f "${line}")
  list(GET f 0 2 4 9 10 11 13 g)
  list(POP_FRONT g id raw a78 predicate status tier evidence)
  if(NOT tier STREQUAL "pr" OR status MATCHES "^(unsupported|skipped)$")
    continue()
  endif()
  # The self test's results: the ledger's, the first row's status flipped.
  if(NOT library_tests)
    set(library_first ${id})
    string(REGEX REPLACE "^pass$" "fail" status "${status}")
  endif()
  string(APPEND library_flipped "\n${id},${predicate},${status},${evidence}")
  if(DEFINED library_sha_${a78})
    set(rom ${library_sha_${a78}})
  elseif(DEFINED library_sha_${raw})
    set(rom ${library_sha_${raw}})
  else()
    message(FATAL_ERROR "library: ${id}: no file under ${library_dir} or tests/diag has "
      "SHA-1 ${a78} (headered) or ${raw} (headerless)")
  endif()
  string(REPLACE "/" "." name "library.${id}")
  string(REPLACE "<semicolon>" "$<SEMICOLON>" predicate "${predicate}")
  muhtari_add_test(NAME ${name}
    COMMAND muhtari-test-predicate ${rom} ${predicate} --bios ${library_bios} --id ${id}
      --result ${library_results}/${name}.csv
    LABELS library TIMEOUT 60)  # provisional, phase 3
  list(APPEND library_tests ${name})
endforeach()
set_tests_properties(${library_tests} PROPERTIES FIXTURES_REQUIRED library_rows)
list(LENGTH library_tests library_count)

muhtari_add_test(NAME library.results
  COMMAND ${CMAKE_COMMAND} -DDIR=${library_results} -DCOUNT=${library_count}
    -DOUT=${PROJECT_BINARY_DIR}/results.csv -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS library TIMEOUT 30)  # provisional, phase 3
set_tests_properties(library.results PROPERTIES
  FIXTURES_CLEANUP library_rows FIXTURES_SETUP library_results)

# The ratchet. MUHTARI_LEDGER_BASE, read from the environment, names the
# commit whose ledger a passing row may not fall from: in CI, the pull
# request's base.
set(library_ratchet ${CMAKE_SOURCE_DIR}/tests/cmake/compat_ratchet.cmake)
set(library_base "")
if(NOT "$ENV{MUHTARI_LEDGER_BASE}" STREQUAL "")
  set(library_base -DBASE=$ENV{MUHTARI_LEDGER_BASE} -DALLOW=${CMAKE_CURRENT_LIST_DIR}/allow.txt)
endif()
muhtari_add_test(NAME library.ratchet
  COMMAND ${CMAKE_COMMAND} -DLEDGER=${compat_ledger} -DRESULTS=${PROJECT_BINARY_DIR}/results.csv
    ${library_base} -P ${library_ratchet}
  LABELS library TIMEOUT 30)  # provisional, phase 3
set_tests_properties(library.ratchet PROPERTIES FIXTURES_REQUIRED library_results)

# Negative control: results that differ from the ledger in one status must
# fail the ratchet naming that row.
string(REPLACE "<semicolon>" ";" library_flipped "${library_flipped}\n")
file(WRITE ${PROJECT_BINARY_DIR}/ratchet_self_test.csv "${library_flipped}")
set(library_cmd ${CMAKE_COMMAND} -DLEDGER=${compat_ledger}
  -DRESULTS=${PROJECT_BINARY_DIR}/ratchet_self_test.csv -P ${library_ratchet})
string(REPLACE ";" "$<SEMICOLON>" library_cmd "${library_cmd}")
muhtari_add_test(NAME library.ratchet.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${library_cmd}" -DCODE=1
    "-DREGEX=${library_first} [^ ]+: ledger [a-z]+ [^,]+, results [a-z]+"
    -P ${CMAKE_SOURCE_DIR}/tests/cmake/expect_fail.cmake
  LABELS library TIMEOUT 30)  # provisional, phase 3
