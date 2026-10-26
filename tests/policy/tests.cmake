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
foreach(case valid missing_artifact extra_artifact wrong_lane missing_member extra_member
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
  if(case STREQUAL valid)
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
