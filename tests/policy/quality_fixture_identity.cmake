if(DEFINED CASE)
  include("${MODULE}")
  quality_fixture_identity(identity "${CANDIDATE}" "${TRUSTED}")
  file(WRITE "${OUT}" "${identity}\n")
  return()
endif()

file(REMOVE_RECURSE "${WORK}")
file(MAKE_DIRECTORY "${WORK}")
set(module "${ROOT}/tests/cmake/quality_fixture_identity.cmake")
file(STRINGS "${ROOT}/tests/roms/manifest.txt" manifest_rows)
foreach(row IN LISTS manifest_rows)
  if(row MATCHES "^tests/diag/souper[.]a78\t")
    set(souper_row "${row}")
  endif()
endforeach()
set(expected "tests/diag/souper.a78\tthis-repo\t7ecbdf6\tMIT\tbb4a6315d7708dd0a8f57d0a533a8f84f3ca900e15fa9156bfbcd4116c6dfb72\t-")
if(NOT "${souper_row}" STREQUAL "${expected}")
  message(FATAL_ERROR "quality fixture control: Souper inventory identity drifted")
endif()
file(WRITE "${WORK}/trusted.txt" "# trusted\n")

function(run_case name row expected_status expected_error)
  file(WRITE "${WORK}/${name}.txt" "# trusted\n${row}\n")
  execute_process(COMMAND "${CMAKE_COMMAND}"
      "-DROOT=${ROOT}" "-DMODULE=${module}" "-DCASE=${name}"
      "-DCANDIDATE=${WORK}/${name}.txt" "-DTRUSTED=${WORK}/trusted.txt"
      "-DOUT=${WORK}/${name}.out" -P "${CMAKE_CURRENT_LIST_FILE}"
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
  if(expected_status STREQUAL "pass")
    if(NOT result STREQUAL "0")
      message(FATAL_ERROR "${name}: expected fixture identity pass, got ${result}: ${output}${error}")
    endif()
  elseif(result STREQUAL "0" OR NOT "${output}${error}" MATCHES "${expected_error}")
    message(FATAL_ERROR "${name}: expected '${expected_error}' rejection, got ${result}: ${output}${error}")
  endif()
endfunction()

run_case(valid "${souper_row}" pass "")
string(REPLACE "7ecbdf6" "7ecbdf7" forged_pin "${souper_row}")
run_case(forged_pin "${forged_pin}" fail "quality: owned Souper identity")
string(REPLACE "bb4a6315d7708dd0a8f57d0a533a8f84f3ca900e15fa9156bfbcd4116c6dfb72"
  "0000000000000000000000000000000000000000000000000000000000000000"
  forged_digest "${souper_row}")
run_case(forged_digest "${forged_digest}" fail "quality: owned Souper identity")
string(REPLACE "souper.a78" "souper-extra.a78" unrelated "${souper_row}")
run_case(unrelated "${unrelated}" fail "quality: unrelated fixture")
message(STATUS "quality fixture identity controls: exact Souper row admitted; forged pin, digest and unrelated path rejected")
