# Repository policy tests: rules about the tests themselves.
set(policy_scripts ${CMAKE_CURRENT_LIST_DIR}/../cmake)

muhtari_add_test(NAME policy.ci_run.controls
  COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR}
    -DWORK=${CMAKE_BINARY_DIR}/policy/ci_run_fixture -P ${CMAKE_CURRENT_LIST_DIR}/ci_run.cmake
  LABELS policy TIMEOUT 10)

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

# User-facing files carry the disclaimer and none overclaims. The self test is
# a tree whose README drops the disclaimer and whose page overclaims in mixed
# case; both files must be named.
muhtari_add_test(NAME policy.claims_wording
  COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR} -P ${policy_scripts}/claims_wording.cmake
  LABELS policy TIMEOUT 10)
set(policy_claims ${CMAKE_BINARY_DIR}/policy/claims_wording_fixture)
file(WRITE ${policy_claims}/README.md "A readme without the disclaimer.\n")
file(WRITE ${policy_claims}/libretro/muhtari_libretro.info.in
  "description = \"No universal title compatibility.\"\n")
file(WRITE ${policy_claims}/docs/example.md "Plays Every game at speed.\n")
set(policy_cmd ${CMAKE_COMMAND} -DROOT=${policy_claims} -P ${policy_scripts}/claims_wording.cmake)
string(REPLACE ";" "$<SEMICOLON>" policy_cmd "${policy_cmd}")
muhtari_add_test(NAME policy.claims_wording.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${policy_cmd}" -DCODE=1
    "-DREGEX=README[.]md: missing.*docs/example[.]md: overclaims" -P ${policy_scripts}/expect_fail.cmake
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

# Upstream release citations are independent of the product's release version.
set(policy_provenance ${CMAKE_BINARY_DIR}/policy/version_provenance_fixture)
file(REMOVE_RECURSE ${policy_provenance})
file(WRITE ${policy_provenance}/version.txt "1.2.3\n")
file(WRITE ${policy_provenance}/PROVENANCE.md "Upstream current (v1.2.3)\nUpstream minor (v1.3.0)\nUpstream patch (v1.2.4)\n")
execute_process(COMMAND ${GIT_EXECUTABLE} init -q WORKING_DIRECTORY ${policy_provenance}
  COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND ${GIT_EXECUTABLE} add version.txt PROVENANCE.md
  WORKING_DIRECTORY ${policy_provenance} COMMAND_ERROR_IS_FATAL ANY)
foreach(mode IN ITEMS current next)
  set(policy_next OFF)
  if(mode STREQUAL next)
    set(policy_next ON)
  endif()
  muhtari_add_test(NAME release.single_version_source.provenance.${mode}
    COMMAND ${CMAKE_COMMAND} -DROOT=${policy_provenance} -DNEXT=${policy_next}
      -P ${policy_scripts}/single_version.cmake
    LABELS policy TIMEOUT 30)
endforeach()

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
# ledger. The self test names a stale page and a
# ledger that claims more than it shows.
set(policy_compat_in -DLEDGER=${CMAKE_SOURCE_DIR}/tests/compat/ledger.csv
  -DEXPECTED=${CMAKE_SOURCE_DIR}/tests/diag/expected.csv
  -DINVENTORY=${CMAKE_SOURCE_DIR}/tests/inventory/ci.txt)
set(policy_compat_cmd ${CMAKE_COMMAND} ${policy_compat_in}
  -DOUT=${CMAKE_BINARY_DIR}/policy/compatibility.md)
set(policy_compat_tool -P ${CMAKE_SOURCE_DIR}/tools/compat_doc.cmake)
muhtari_add_test(NAME policy.compat_doc_fresh
  COMMAND ${policy_compat_cmd} -DCHECK=${CMAKE_SOURCE_DIR}/docs/compatibility.md ${policy_compat_tool}
  LABELS policy TIMEOUT 10)
# provisional: 10 s kept, ceil(2 x 0.38 s) is 1 s for nine generator runs; macOS arm64 local, slowest of 10 runs alone (0.32 s) and one full ci run (0.38 s)
muhtari_add_test(NAME policy.compat_doc_fresh.self_test
  COMMAND ${CMAKE_COMMAND} -DTOOL=${CMAKE_SOURCE_DIR}/tools/compat_doc.cmake
    ${policy_compat_in} -DPAGE=${CMAKE_SOURCE_DIR}/docs/compatibility.md
    -DWORK=${CMAKE_BINARY_DIR}/policy/compat_doc_controls
    -P ${policy_scripts}/compat_doc_controls.cmake
  LABELS policy TIMEOUT 10)

# The ratchet against a fixture repository: a lowered row, an exception
# expiring today or yesterday, a demotion to local and an unchanged ledger.
muhtari_add_test(NAME policy.compat_ratchet.controls
  COMMAND ${CMAKE_COMMAND} -DRATCHET=${CMAKE_SOURCE_DIR}/tests/cmake/compat_ratchet.cmake
    -DGIT=${GIT_EXECUTABLE} -DWORK=${CMAKE_BINARY_DIR}/policy/ratchet_controls
    -DLEDGER=${CMAKE_SOURCE_DIR}/tests/compat/ledger.csv
    -P ${CMAKE_SOURCE_DIR}/tests/cmake/compat_ratchet_controls.cmake
  LABELS policy TIMEOUT 30)  # provisional, n=1, local

if(MUHTARI_INVENTORY MATCHES "^(ci|wasm32)$")
  # Prevents a retired source-line ceiling from making the hosted state consumer unreachable.
  file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/policy/host_support_controls.cmake CONTENT [=[
cmake_minimum_required(VERSION 3.25)

function(inspect_workflow workflow_path)
  if(NOT EXISTS "${workflow_path}" OR IS_DIRECTORY "${workflow_path}")
    message(FATAL_ERROR "policy.host_support.controls: workflow is absent: ${workflow_path}")
  endif()
  file(READ "${workflow_path}" workflow)
  string(FIND "${workflow}" "\n  retroarch:\n" job_start)
  if(job_start LESS 0)
    message(FATAL_ERROR "policy.host_support.controls: retroarch job is absent")
  endif()
  math(EXPR job_start "${job_start} + 1")
  string(SUBSTRING "${workflow}" ${job_start} -1 after_job_start)
  string(REGEX MATCH "\n  [A-Za-z0-9_-]+:\n" next_job_heading "${after_job_start}")
  if(next_job_heading STREQUAL "")
    set(job_text "${after_job_start}")
  else()
    string(FIND "${after_job_start}" "${next_job_heading}" job_end)
    string(SUBSTRING "${after_job_start}" 0 ${job_end} job_text)
  endif()

  string(FIND "${job_text}" "      - name: Fresh frontend and runner battery roundtrip" battery_start)
  if(battery_start LESS 0)
    message(FATAL_ERROR "policy.host_support.controls: battery step is absent from retroarch job")
  endif()
  string(SUBSTRING "${job_text}" ${battery_start} -1 after_battery_start)
  string(REGEX MATCH "\n      - name: " next_step_heading "${after_battery_start}")
  if(next_step_heading STREQUAL "")
    set(battery_text "${after_battery_start}")
  else()
    string(FIND "${after_battery_start}" "${next_step_heading}" battery_end)
    string(SUBSTRING "${after_battery_start}" 0 ${battery_end} battery_text)
  endif()
  string(REGEX REPLACE "(^|\n)[ \t]*#[^\n]*" "" active_battery "${battery_text}")

  if("${active_battery}" MATCHES "wc[ \t]+-l" AND
      "${active_battery}" MATCHES "(-le|-lt|-ge|-gt|-eq|<=|>=|<|>)[ \t]*[0-9]+")
    string(REGEX MATCH "[^\n]*wc[ \t]+-l[^\n]*" source_gate "${active_battery}")
    message(FATAL_ERROR "policy.host_support.controls: battery source-line ceiling detected: ${source_gate}")
  endif()
  if("${active_battery}" MATCHES "(source|support)[_-]?(line|total|count)" AND
      "${active_battery}" MATCHES "(-le|-lt|-ge|-gt|-eq|<=|>=|<|>)[ \t]*[0-9]+")
    message(FATAL_ERROR "policy.host_support.controls: numeric battery source total gate detected")
  endif()

  set(reporter [==[cmake -DMODE=support-report -DROOT="$PWD" -P tests/retroarch/run_retroarch.cmake]==])
  string(FIND "${active_battery}" "${reporter}" reporter_position)
  if(reporter_position LESS 0)
    message(FATAL_ERROR "policy.host_support.controls: battery support reporter invocation is missing")
  endif()
endfunction()

if(MODE STREQUAL "inspect")
  if(NOT DEFINED WORKFLOW OR "${WORKFLOW}" STREQUAL "")
    message(FATAL_ERROR "policy.host_support.controls: -DWORKFLOW is required in inspect mode")
  endif()
  inspect_workflow("${WORKFLOW}")
  return()
endif()

if(NOT DEFINED ROOT OR "${ROOT}" STREQUAL "" OR
    NOT DEFINED BUILD OR "${BUILD}" STREQUAL "" OR
    NOT DEFINED WORK OR "${WORK}" STREQUAL "")
  message(FATAL_ERROR "policy.host_support.controls: ROOT, BUILD and WORK are required")
endif()
set(root_path "${ROOT}")
set(build_path "${BUILD}")
set(work_path "${WORK}")
cmake_path(ABSOLUTE_PATH root_path NORMALIZE OUTPUT_VARIABLE root_path_normalized)
cmake_path(ABSOLUTE_PATH build_path NORMALIZE OUTPUT_VARIABLE build_path_normalized)
cmake_path(ABSOLUTE_PATH work_path NORMALIZE OUTPUT_VARIABLE work_path_normalized)
set(project_build_root "${root_path_normalized}/build")
cmake_path(IS_PREFIX project_build_root "${build_path_normalized}" NORMALIZE build_under_project)
cmake_path(IS_PREFIX build_path_normalized "${work_path_normalized}" NORMALIZE work_under_build)
if(NOT build_under_project OR NOT work_under_build OR
    work_path_normalized STREQUAL build_path_normalized)
  message(FATAL_ERROR "policy.host_support.controls: scratch path must be a strict child of the project build directory: ${WORK}")
endif()
set(WORK "${work_path_normalized}")
file(REMOVE_RECURSE "${WORK}")
set(fixture_root "${WORK}/copied-support")
file(MAKE_DIRECTORY "${fixture_root}/tests")
file(COPY "${ROOT}/tests/retroarch" DESTINATION "${fixture_root}/tests")

set(preserve_dir "${WORK}/preserved-frontend")
set(unused_save_dir "${WORK}/unused-save")
set(unused_shot "${WORK}/unused-shot.png")
file(MAKE_DIRECTORY "${preserve_dir}")
file(WRITE "${preserve_dir}/sentinel" "preserve\n")

function(run_report report_root result_var output_var)
  execute_process(
    COMMAND "${CMAKE_COMMAND}" "-DMODE=support-report" "-DROOT=${report_root}"
      "-DRETROARCH=${CMAKE_COMMAND}"
      "-DCORE=${report_root}/tests/retroarch/run_retroarch.cmake"
      "-DINFO=${report_root}/tests/retroarch/test.cfg.in"
      "-DTEMPLATE=${report_root}/tests/retroarch/test.cfg.in"
      "-DRA_DIR=${preserve_dir}" "-DSAVE_DIR=${unused_save_dir}"
      -DFRAMES=1 "-DSHOT=${unused_shot}"
      -P "${report_root}/tests/retroarch/run_retroarch.cmake"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
  )
  set("${result_var}" "${result}" PARENT_SCOPE)
  set("${output_var}" "${output}${error}" PARENT_SCOPE)
endfunction()

function(count_newlines path result_var)
  file(READ "${path}" contents)
  string(REGEX REPLACE "[^\n]" "" newline_chars "${contents}")
  string(LENGTH "${newline_chars}" count)
  set("${result_var}" "${count}" PARENT_SCOPE)
endfunction()

set(support_dir "${fixture_root}/tests/retroarch")
file(GLOB support_paths LIST_DIRECTORIES FALSE "${support_dir}/*")
list(SORT support_paths)
if(NOT support_paths)
  message(FATAL_ERROR "policy.host_support.controls: copied support tree is empty")
endif()
set(expected_total 0)
foreach(path IN LISTS support_paths)
  if(IS_SYMLINK "${path}" OR IS_DIRECTORY "${path}" OR NOT EXISTS "${path}")
    message(FATAL_ERROR "policy.host_support.controls: copied support input is not a regular file: ${path}")
  endif()
  count_newlines("${path}" expected_lines)
  get_filename_component(name "${path}" NAME)
  math(EXPR expected_total "${expected_total} + ${expected_lines}")
endforeach()

run_report("${fixture_root}" report_status baseline_report)
if(NOT "${report_status}" STREQUAL "0")
  message(FATAL_ERROR "policy.host_support.controls: actual reporter rejected the copied support tree: ${baseline_report}")
endif()
foreach(path IN LISTS support_paths)
  count_newlines("${path}" expected_lines)
  get_filename_component(name "${path}" NAME)
  set(expected_row "support-report: tests/retroarch/${name}: ${expected_lines} newline(s)")
  string(FIND "${baseline_report}" "${expected_row}" row_position)
  if(row_position LESS 0)
    message(FATAL_ERROR "policy.host_support.controls: actual reporter omitted or miscounted ${name}: expected ${expected_lines} newlines")
  endif()
endforeach()
set(expected_total_row "support-report: total ${expected_total} newline(s)")
string(FIND "${baseline_report}" "${expected_total_row}" total_position)
if(total_position LESS 0)
  message(FATAL_ERROR "policy.host_support.controls: actual reporter total differs from independent count ${expected_total}")
endif()
if(NOT EXISTS "${preserve_dir}/sentinel" OR EXISTS "${unused_save_dir}" OR EXISTS "${unused_shot}")
  message(FATAL_ERROR "policy.host_support.controls: support reporting performed frontend cleanup or output work")
endif()

string(REPEAT "growth\n" 301 growth_content)
file(WRITE "${support_dir}/policy-growth.txt" "${growth_content}")
math(EXPR grown_total "${expected_total} + 301")
if(NOT grown_total GREATER 300)
  message(FATAL_ERROR "policy.host_support.controls: growth fixture did not exceed the historical ceiling")
endif()
run_report("${fixture_root}" report_status grown_report)
if(NOT "${report_status}" STREQUAL "0")
  message(FATAL_ERROR "policy.host_support.controls: actual reporter rejected known support growth: ${grown_report}")
endif()
set(growth_row "support-report: tests/retroarch/policy-growth.txt: 301 newline(s)")
string(FIND "${grown_report}" "${growth_row}" growth_position)
set(grown_total_row "support-report: total ${grown_total} newline(s)")
string(FIND "${grown_report}" "${grown_total_row}" grown_total_position)
if(growth_position LESS 0 OR grown_total_position LESS 0)
  message(FATAL_ERROR "policy.host_support.controls: growth report did not increase by exactly 301 newline characters")
endif()
if(NOT EXISTS "${preserve_dir}/sentinel" OR EXISTS "${unused_save_dir}" OR EXISTS "${unused_shot}")
  message(FATAL_ERROR "policy.host_support.controls: growth reporting performed frontend cleanup or output work")
endif()

file(REMOVE "${support_dir}/states.c")
run_report("${fixture_root}" missing_status missing_report)
set(missing_input "${support_dir}/states.c")
string(FIND "${missing_report}" "missing required support input" missing_message_position)
string(FIND "${missing_report}" "${missing_input}" missing_path_position)
if("${missing_status}" STREQUAL "0" OR
    missing_message_position LESS 0 OR missing_path_position LESS 0)
  message(FATAL_ERROR "policy.host_support.controls: missing required input was not rejected by path: ${missing_report}")
endif()

set(workflow "${ROOT}/.github/workflows/ci.yml")
execute_process(COMMAND "${CMAKE_COMMAND}" "-DMODE=inspect" "-DWORKFLOW=${workflow}"
  -P "${CMAKE_CURRENT_LIST_FILE}"
  RESULT_VARIABLE inspect_status OUTPUT_VARIABLE inspect_output ERROR_VARIABLE inspect_error)
if(NOT "${inspect_status}" STREQUAL "0")
  message(FATAL_ERROR "policy.host_support.controls: actual retroarch battery region failed policy inspection: ${inspect_output}${inspect_error}")
endif()

file(READ "${workflow}" workflow_text)
set(reporter [==[cmake -DMODE=support-report -DROOT="$PWD" -P tests/retroarch/run_retroarch.cmake]==])
set(historical_gate [==[test "$(cat tests/retroarch/* | wc -l | tr -d ' ')" -le 300]==])
set(other_gate [==[test "$(cat tests/retroarch/* | wc -l | tr -d ' ')" -le 400]==])

function(expect_inspection_failure candidate expected)
  execute_process(COMMAND "${CMAKE_COMMAND}" "-DMODE=inspect" "-DWORKFLOW=${candidate}"
    -P "${CMAKE_CURRENT_LIST_FILE}"
    RESULT_VARIABLE status OUTPUT_VARIABLE output ERROR_VARIABLE error)
  set(combined "${output}${error}")
  string(REGEX REPLACE "[ \t\r\n]+" " " combined "${combined}")
  if("${status}" STREQUAL "0" OR NOT "${combined}" MATCHES "${expected}")
    message(FATAL_ERROR "policy.host_support.controls: workflow mutation missed ${expected}: ${combined}")
  endif()
endfunction()

set(noisy_workflow "${WORK}/workflow-comments-and-other-jobs.yml")
set(noisy_text "${workflow_text}")
string(REPLACE "${reporter}" "${reporter}\n          # Historical source-line check: ${historical_gate}"
  noisy_text "${noisy_text}")
string(APPEND noisy_text [==[

  unrelated-policy-probe:
    steps:
      - run: test "$(cat tests/retroarch/* | wc -l | tr -d ' ')" -le 400
]==])
file(WRITE "${noisy_workflow}" "${noisy_text}")
execute_process(COMMAND "${CMAKE_COMMAND}" "-DMODE=inspect" "-DWORKFLOW=${noisy_workflow}"
  -P "${CMAKE_CURRENT_LIST_FILE}"
  RESULT_VARIABLE noisy_status OUTPUT_VARIABLE noisy_output ERROR_VARIABLE noisy_error)
if(NOT "${noisy_status}" STREQUAL "0")
  message(FATAL_ERROR "policy.host_support.controls: comments or unrelated jobs affected the bounded inspection: ${noisy_output}${noisy_error}")
endif()

set(historical_workflow "${WORK}/workflow-historical-ceiling.yml")
set(historical_text "${workflow_text}")
string(REPLACE "${reporter}" "${historical_gate}" historical_text "${historical_text}")
file(WRITE "${historical_workflow}" "${historical_text}")
expect_inspection_failure("${historical_workflow}" "battery source-line ceiling detected")

set(other_workflow "${WORK}/workflow-different-ceiling.yml")
set(other_text "${workflow_text}")
string(REPLACE "${reporter}" "${other_gate}" other_text "${other_text}")
file(WRITE "${other_workflow}" "${other_text}")
expect_inspection_failure("${other_workflow}" "battery source-line ceiling detected")

set(missing_reporter_workflow "${WORK}/workflow-missing-reporter.yml")
set(missing_reporter_text "${workflow_text}")
string(REPLACE "${reporter}" "" missing_reporter_text "${missing_reporter_text}")
file(WRITE "${missing_reporter_workflow}" "${missing_reporter_text}")
expect_inspection_failure("${missing_reporter_workflow}" "battery support reporter invocation is missing")
]=])
  muhtari_add_test(NAME policy.host_support.controls
    COMMAND ${CMAKE_COMMAND} "-DROOT=${CMAKE_SOURCE_DIR}" "-DBUILD=${CMAKE_BINARY_DIR}"
      "-DWORK=${CMAKE_BINARY_DIR}/policy/host_support_controls"
      -P ${CMAKE_BINARY_DIR}/policy/host_support_controls.cmake
    LABELS policy TIMEOUT 20)
endif()

if(MUHTARI_INVENTORY STREQUAL "ci")
  set(publish_timeout 130)
  if(WIN32)
    set(publish_timeout 1450)
  endif()
  muhtari_add_test(NAME policy.publish.controls
    COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR}
      -DWORK=${CMAKE_BINARY_DIR}/policy/publish_fixture -P ${CMAKE_CURRENT_LIST_DIR}/publish.cmake
    # Provisional max724.35s Windows/63.21s other, runs39876385414/39874751162.
    LABELS policy TIMEOUT ${publish_timeout} COST 725)
  set(quality_timeout 30)
  if(APPLE)
    # ceil(2 x 70.36s), complete expanded Intel current/base controls,
    # run40412835933/job121887675359; all semantic cases remain.
    set(quality_timeout 141)
  elseif(WIN32)
    # ceil(2 x 78.61s), complete expanded Windows x64 controls,
    # run40414104123/job121892328427; retain every producer/join predicate.
    set(quality_timeout 158)
  endif()
  muhtari_add_test(NAME policy.quality.controls
    COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR}
      -DRUN=$<TARGET_FILE:muhtari-run> -DDRIVER=$<TARGET_FILE:muhtari-test-diag>
      "-DEMULATOR=${CMAKE_CROSSCOMPILING_EMULATOR}"
      -DWORK=${CMAKE_BINARY_DIR}/policy/quality_fixture -P ${CMAKE_CURRENT_LIST_DIR}/quality.cmake
    LABELS policy TIMEOUT ${quality_timeout})
  # Provisional n=1 per arm64 image: 20261013.183.4/20261018.189.1, runs 39699892851/39699026057.
  # Complete control max 103.28 s; ceil(2 x 103.28)=207 s, with all semantic cases retained.
  muhtari_add_test(NAME policy.digest.controls
    COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR}
      -DWORK=${CMAKE_BINARY_DIR}/policy/digest_fixture -P ${CMAKE_CURRENT_LIST_DIR}/digest.cmake
    LABELS policy TIMEOUT 207)
  # Fault prevented: comparing empty, duplicate or unrelated lane evidence.
  file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/policy/lane_equality_controls.cmake CONTENT [=[
cmake_minimum_required(VERSION 3.25)
execute_process(COMMAND git -C "${ROOT}" rev-parse HEAD OUTPUT_VARIABLE sha
  OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
set(producer "${WORK}-producer")
file(MAKE_DIRECTORY "${producer}")
execute_process(COMMAND "${CMAKE_COMMAND}" -DMODE=write -DRUN=${RUN} -DDRIVER=${DRIVER}
  -DDIAG=${ROOT}/tests/diag -DOUT=${producer} "-DEMULATOR=${EMULATOR}"
  -DSOUNDTEST=${SOUNDTEST} -DPOKEY=${POKEY} -P "${ROOT}/tests/cmake/hashes.cmake"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "lane controls actual writer: ${rc} ${out}${err}")
endif()
file(READ "${producer}/hashes.txt" hashes)
set(HASHES_FUNCTIONS_ONLY ON)
include("${ROOT}/tests/cmake/hashes.cmake")
hashes_lane_identity("${sha}" identity)
# Mutate actual runner reports at the production writer boundary, keeping the
# real numeric judge and independently executed library reference intact.
foreach(family bankset expansion)
  set(prefix bankset.32n)
  set(scenario bankset32n)
  set(dir "${producer}/hashes-bankset/32n")
  set(image "${dir}/bankset32n.a78")
  set(last_fault 11)
  if(family STREQUAL expansion)
    set(prefix expansion.1-0-0-0-0-2-pages)
    set(scenario expansion-x0)
    set(dir "${producer}/hashes-expansion/1-0-0-0-0-2")
    set(image "${dir}/expansion.a78")
    set(last_fault 14)
  endif()
  file(READ "${dir}/hashes-report.txt" clean)
  set(cases valid missing truncated extra duplicate reordered wrong_region wrong_scenario frame raw pcm samples)
  foreach(fault RANGE 0 ${last_fault})
    list(APPEND cases "mailbox-${fault}")
  endforeach()
  foreach(case IN LISTS cases)
    set(observed "${clean}")
    set(selected "${prefix}")
    set(tag "${scenario}")
    string(REGEX MATCH "frame 1 sha256 [^\n]+\n" row "${clean}")
    if(case STREQUAL missing OR case STREQUAL reordered)
      string(REPLACE "${row}" "" observed "${clean}")
      if(case STREQUAL reordered)
        string(APPEND observed "${row}")
      endif()
    elseif(case STREQUAL truncated)
      string(SUBSTRING "${clean}" 0 100 observed)
    elseif(case STREQUAL extra OR case STREQUAL duplicate)
      string(APPEND observed "${row}")
    elseif(case STREQUAL wrong_region)
      string(REPLACE "audio_samples 8861" "audio_samples 11537" observed "${clean}")
    elseif(case STREQUAL wrong_scenario)
      set(tag unrelated)
    elseif(case MATCHES "^(frame|raw|pcm)$")
      set(field "frame 1 sha256")
      if(case STREQUAL raw)
        set(field audio_transitions_sha256)
      elseif(case STREQUAL pcm)
        set(field audio_pcm_sha256)
      endif()
      string(REPEAT a 64 forged)
      string(REGEX REPLACE "${field} [^\n]+\n" "${field} ${forged}\n" observed "${clean}")
    elseif(case STREQUAL samples)
      string(REPLACE "audio_samples 8861" "audio_samples 8860" observed "${clean}")
    elseif(case MATCHES "^mailbox-([0-9]+)$")
      file(READ "${dir}/fault-${CMAKE_MATCH_1}.txt" fault)
      string(REGEX REPLACE "diagnostic [^\n]+\n" "${fault}" observed "${clean}")
    endif()
    file(WRITE "${producer}/control-report.txt" "${observed}")
    file(CONFIGURE OUTPUT "${producer}/control.cmake" @ONLY CONTENT [==[
cmake_minimum_required(VERSION 3.25)
set(HASHES_FUNCTIONS_ONLY ON)
include("@ROOT@/tests/cmake/hashes.cmake")
set(DRIVER [===[@DRIVER@]===])
set(EMULATOR [===[@EMULATOR@]===])
set(DIAG "@ROOT@/tests/diag")
set(OUT "@producer@")
file(READ "@producer@/control-report.txt" observed)
hashes_memory_report("@selected@" "@tag@" "@image@" "@dir@/reference.txt" "${observed}" rows)
]==])
    set(cmd "${CMAKE_COMMAND}" -P "${producer}/control.cmake")
    if(case STREQUAL valid)
      execute_process(COMMAND ${cmd} COMMAND_ERROR_IS_FATAL ANY)
    else()
      execute_process(COMMAND "${CMAKE_COMMAND}" "-DCMD=${cmd}" -DCODE=1 "-DREGEX=hashes:"
        -P "${ROOT}/tests/cmake/expect_fail.cmake" COMMAND_ERROR_IS_FATAL ANY)
    endif()
  endforeach()
endforeach()
foreach(path absolute relative)
if(path STREQUAL "relative")
  file(RELATIVE_PATH WORK "${CMAKE_CURRENT_BINARY_DIR}" "${WORK}")
endif()
foreach(case valid sha config empty missing extra duplicate difference
    controller_missing controller_duplicate controller_order controller_numeric controller_family
    pal_missing pal_duplicate pal_order pal_numeric pal_digest pal_counter
    memory_missing memory_extra memory_duplicate memory_order memory_family memory_region
    memory_numeric memory_digest memory_source memory_template memory_config
    identity_source identity_fixture identity_options)
  file(REMOVE_RECURSE "${WORK}")
  foreach(lane native be)
    file(WRITE "${WORK}/${lane}/hashes.txt" "${hashes}")
    file(CONFIGURE OUTPUT "${WORK}/${lane}/commit.txt" CONTENT "${sha}\n" @ONLY NEWLINE_STYLE UNIX)
    file(CONFIGURE OUTPUT "${WORK}/${lane}/identity.txt" CONTENT "${identity}" @ONLY NEWLINE_STYLE UNIX)
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
  elseif(case MATCHES "^identity_")
    set(bad "${identity}")
    if(case STREQUAL identity_options)
      string(REPLACE "short_runs=OFF" "short_runs=ON" bad "${bad}")
    elseif(case STREQUAL identity_fixture)
      string(REPLACE "tests/diag/bankset.a78" "tests/diag/foreign.a78" bad "${bad}")
    else()
      string(REGEX REPLACE "[0-9a-f]+\t" "forged\t" bad "${bad}")
    endif()
    foreach(lane native be)
      file(CONFIGURE OUTPUT "${WORK}/${lane}/identity.txt" CONTENT "${bad}" @ONLY NEWLINE_STYLE UNIX)
    endforeach()
    set(regex "lane_equality: configuration/fixture identity")
  elseif(case MATCHES "^memory_")
    set(bad "${hashes}")
    string(REGEX MATCH "bankset[.]32n[.]frame1 [^\n]+\n" row "${hashes}")
    if(case STREQUAL memory_missing OR case STREQUAL memory_order)
      string(REPLACE "${row}" "" bad "${bad}")
      if(case STREQUAL memory_order)
        string(APPEND bad "${row}")
      endif()
    elseif(case STREQUAL memory_duplicate)
      string(APPEND bad "${row}")
    elseif(case STREQUAL memory_extra)
      string(APPEND bad "bankset.extra.frame1 abc\n")
    elseif(case STREQUAL memory_family)
      string(REPLACE "bankset.32n." "bankset.64n." bad "${bad}")
    elseif(case STREQUAL memory_region)
      string(REPLACE "bankset.32h-pal.audio_samples 11537" "bankset.32h-pal.audio_samples 8861" bad "${bad}")
    elseif(case STREQUAL memory_numeric)
      string(REPLACE "bankset.32n.audio_samples 8861" "bankset.32n.audio_samples 8860" bad "${bad}")
    elseif(case STREQUAL memory_digest)
      string(REPLACE "${row}" "bankset.32n.frame1 abc\n" bad "${bad}")
    else()
      set(field source_commit)
      if(case STREQUAL memory_template)
        set(field template_sha256)
      elseif(case STREQUAL memory_config)
        set(field config_sha256)
      endif()
      string(REPEAT a 64 forged)
      string(REGEX REPLACE "bankset[.]32n[.]${field} [^\n]+\n"
        "bankset.32n.${field} ${forged}\n" bad "${bad}")
    endif()
    foreach(lane native be)
      file(WRITE "${WORK}/${lane}/hashes.txt" "${bad}")
    endforeach()
    set(regex "hashes:")
  elseif(case MATCHES "^pal_")
    set(bad "${hashes}")
    string(REGEX MATCH "pal[.]frame601 [^\n]+\n" row "${hashes}")
    if(case STREQUAL pal_missing OR case STREQUAL pal_order)
      string(REPLACE "${row}" "" bad "${bad}")
      if(case STREQUAL pal_order)
        string(APPEND bad "${row}")
      endif()
    elseif(case STREQUAL pal_duplicate)
      string(APPEND bad "${row}")
    elseif(case STREQUAL pal_numeric)
      string(REPLACE "pal.audio_samples 577879" "pal.audio_samples 577878" bad "${bad}")
    elseif(case STREQUAL pal_digest)
      string(REPLACE "${row}" "pal.frame601 abc\n" bad "${bad}")
    else()
      string(REPLACE "pal.full_frames 600" "pal.full_frames 599" bad "${bad}")
    endif()
    foreach(lane native be)
      file(WRITE "${WORK}/${lane}/hashes.txt" "${bad}")
    endforeach()
    set(regex "hashes:")
  elseif(case MATCHES "^controller_")
    set(bad "${hashes}")
    string(REGEX MATCH "controller[.]light[.]lightgun[.]both[.]frame1 [^\n]+\n" row "${hashes}")
    if(case STREQUAL "controller_missing" OR case STREQUAL "controller_order")
      string(REPLACE "${row}" "" bad "${bad}")
      if(case STREQUAL "controller_order")
        string(APPEND bad "${row}")
      endif()
    elseif(case STREQUAL "controller_duplicate")
      string(APPEND bad "${row}")
    elseif(case STREQUAL "controller_numeric")
      string(REPLACE "audio_frames 12" "audio_frames x" bad "${bad}")
    else()
      string(REPLACE "controller.light.lightgun." "controller.light.foreign." bad "${bad}")
    endif()
    foreach(lane native be)
      file(WRITE "${WORK}/${lane}/hashes.txt" "${bad}")
    endforeach()
    set(regex "hashes:")
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
# The real six-native consumer requires exactly the existing named artifacts,
# each with the same three members as native/PowerPC evidence.
set(native "${producer}/native-artifacts")
foreach(case valid relative missing_artifact extra_artifact wrong_lane missing_member extra_member
    source configuration fixture empty partial duplicate_file)
  file(REMOVE_RECURSE "${native}")
  foreach(lane linux-arm64 linux-x64 macos-arm64 macos-x64 windows-arm64 windows-x64)
    set(dir "${native}/hashes-${lane}")
    file(CONFIGURE OUTPUT "${dir}/hashes.txt" CONTENT "${hashes}" @ONLY NEWLINE_STYLE UNIX)
    file(CONFIGURE OUTPUT "${dir}/commit.txt" CONTENT "${sha}\n" @ONLY NEWLINE_STYLE UNIX)
    file(CONFIGURE OUTPUT "${dir}/identity.txt" CONTENT "${identity}" @ONLY NEWLINE_STYLE UNIX)
  endforeach()
  set(dir "${native}/hashes-linux-x64")
  if(case STREQUAL missing_artifact)
    file(REMOVE_RECURSE "${dir}")
  elseif(case STREQUAL extra_artifact)
    file(MAKE_DIRECTORY "${native}/hashes-foreign")
  elseif(case STREQUAL wrong_lane)
    file(RENAME "${dir}" "${native}/hashes-linux-foreign")
  elseif(case STREQUAL missing_member)
    file(REMOVE "${dir}/identity.txt")
  elseif(case STREQUAL extra_member)
    file(WRITE "${dir}/extra.txt" "extra\n")
  elseif(case STREQUAL source)
    file(WRITE "${dir}/commit.txt" "forged\n")
  elseif(case STREQUAL configuration OR case STREQUAL fixture)
    set(bad "${identity}")
    if(case STREQUAL configuration)
      string(REPLACE "tests=ON" "tests=OFF" bad "${bad}")
    else()
      string(REPLACE "tests/diag/bankset.a78" "tests/diag/foreign.a78" bad "${bad}")
    endif()
    # Even all six agreeing on a forgery must fail source authentication.
    foreach(lane linux-arm64 linux-x64 macos-arm64 macos-x64 windows-arm64 windows-x64)
      file(CONFIGURE OUTPUT "${native}/hashes-${lane}/identity.txt" CONTENT "${bad}" @ONLY NEWLINE_STYLE UNIX)
    endforeach()
  elseif(case STREQUAL empty OR case STREQUAL partial)
    set(bad "")
    if(case STREQUAL partial)
      string(REGEX REPLACE "bankset[.]32n[.]frame1 [^\n]+\n" "" bad "${hashes}")
    endif()
    foreach(lane linux-arm64 linux-x64 macos-arm64 macos-x64 windows-arm64 windows-x64)
      file(CONFIGURE OUTPUT "${native}/hashes-${lane}/hashes.txt" CONTENT "${bad}" @ONLY NEWLINE_STYLE UNIX)
    endforeach()
  endif()
  set(cmd "${CMAKE_COMMAND}" -DDIRECTORY=${native} -DEXPECTED_SHA=${sha} -DMODE=compare
    -P "${ROOT}/tests/cmake/hashes.cmake")
  if(case STREQUAL duplicate_file)
    set(cmd "${CMAKE_COMMAND}" "-DFILES=${dir}/hashes.txt;${dir}/hashes.txt" -DCOUNT=2 -DMODE=compare
      -P "${ROOT}/tests/cmake/hashes.cmake")
  endif()
  if(case STREQUAL relative)
    # Fault prevented: the workflow's relative artifact directory (-DDIRECTORY=hashes)
    # listing no lanes, because CMake does not resolve a relative GLOB base.
    cmake_path(GET native PARENT_PATH parent)
    execute_process(COMMAND "${CMAKE_COMMAND}" -DDIRECTORY=native-artifacts -DEXPECTED_SHA=${sha}
      -DMODE=compare -P "${ROOT}/tests/cmake/hashes.cmake" WORKING_DIRECTORY "${parent}"
      COMMAND_ERROR_IS_FATAL ANY)
  elseif(case STREQUAL valid)
    execute_process(COMMAND ${cmd} COMMAND_ERROR_IS_FATAL ANY)
  else()
    execute_process(COMMAND "${CMAKE_COMMAND}" "-DCMD=${cmd}" -DCODE=1 "-DREGEX=hashes:"
      -P "${ROOT}/tests/cmake/expect_fail.cmake" COMMAND_ERROR_IS_FATAL ANY)
  endif()
endforeach()
]=])
  muhtari_add_test(NAME policy.lane_equality.controls
    COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR}
      -DRUN=$<TARGET_FILE:muhtari-run> -DDRIVER=$<TARGET_FILE:muhtari-test-diag>
      "-DEMULATOR=${CMAKE_CROSSCOMPILING_EMULATOR}"
      -DSOUNDTEST=${PROJECT_BINARY_DIR}/built/soundtest/soundtest.bas.a78
      -DPOKEY=${PROJECT_BINARY_DIR}/built/pokey/pokey.bas.a78
      -DWORK=${CMAKE_BINARY_DIR}/policy/lane_equality_fixture
      -P ${CMAKE_BINARY_DIR}/policy/lane_equality_controls.cmake
    # ceil(2 x 104.66s), complete actual writer/native/PPC controls,
    # Intel run40412835933/job121887675359; workload and lane scales unchanged.
    LABELS policy TIMEOUT 210)
  set_tests_properties(policy.lane_equality.controls PROPERTIES FIXTURES_REQUIRED "built_soundtest;built_pokey")
endif()
