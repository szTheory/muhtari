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
    # Provisional ceil(2 x 45.49s), complete current/base controls,
    # run 40227151344/job121270952138; all semantic cases remain.
    set(quality_timeout 91)
  elseif(WIN32)
    # Ceil(2 x 40.12s), maximum successful current/base Windows control in
    # run40080473918; retain all producer/join cases and numeric predicates.
    set(quality_timeout 81)
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
string(REPEAT "a" 40 sha)
set(hashes "frame1 abc\nframe600 abc\npng abc\n")
foreach(name soundtest pokey)
  foreach(field transitions_sha256 pcm_sha256 frames samples rate behaviour_revision synthesis)
    string(APPEND hashes "${name}.audio_${field} abc\n")
  endforeach()
endforeach()
foreach(name activision absolute fractalus battery.first battery.second)
  set(last 600)
  if(name MATCHES "^battery[.]")
    set(last 120)
  endif()
  string(APPEND hashes "${name}.frame1 abc\n${name}.frame${last} abc\n")
  foreach(field transitions_sha256 pcm_sha256 frames samples rate behaviour_revision synthesis)
    string(APPEND hashes "${name}.audio_${field} abc\n")
  endforeach()
  string(APPEND hashes "${name}.diagnostic abc\n")
  if(last EQUAL 120)
    string(APPEND hashes "${name}.save_sha256 abc\n")
  endif()
endforeach()
string(REPEAT "a" 64 digest)
string(REPEAT "a" 512 mailbox)
foreach(scenario motion.driving light.lightgun analog.paddle analog.keypad
    motion.trakball motion.st-mouse motion.amiga-mouse
    motion.snes2atari.group0 motion.snes2atari.group1 motion.snes2atari.group2
    motion.mega7800.group0 motion.mega7800.group1 motion.mega7800.group2)
  set(prefix "controller.${scenario}.both")
  foreach(frame RANGE 1 12)
    string(APPEND hashes "${prefix}.frame${frame} ${digest}\n")
  endforeach()
  string(APPEND hashes "${prefix}.lag_frames 0\n${prefix}.audio_transitions_sha256 ${digest}\n"
    "${prefix}.audio_pcm_sha256 ${digest}\n${prefix}.audio_frames 12\n"
    "${prefix}.audio_samples 8861\n${prefix}.audio_rate 48000/1\n"
    "${prefix}.audio_behaviour_revision 5\n${prefix}.audio_synthesis exact-q23-16x64-dc20\n"
    "${prefix}.diagnostic ${mailbox}\n")
endforeach()
# The additive PAL schema must be complete even when two lanes agree on a fault.
foreach(frame RANGE 1 601)
  string(APPEND hashes "pal.frame${frame} ${digest}\n")
endforeach()
string(APPEND hashes "pal.audio_transitions_sha256 ${digest}\npal.audio_pcm_sha256 ${digest}\n"
  "pal.audio_frames 601\npal.audio_samples 577879\npal.audio_rate 48000/1\n"
  "pal.audio_behaviour_revision 5\npal.audio_synthesis exact-q23-16x64-dc20\n"
  "pal.diagnostic ${mailbox}\npal.full_frames 600\npal.startup_samples 960\n"
  "pal.startup_remainder 336960\npal.final_remainder 47029\npal.final_tick 85543176\n")
foreach(path absolute relative)
if(path STREQUAL "relative")
  file(RELATIVE_PATH WORK "${CMAKE_CURRENT_BINARY_DIR}" "${WORK}")
endif()
foreach(case valid sha config empty missing extra duplicate difference
    controller_missing controller_duplicate controller_order controller_numeric controller_family
    pal_missing pal_duplicate pal_order pal_numeric pal_digest pal_counter)
  file(REMOVE_RECURSE "${WORK}")
  foreach(lane native be)
    file(WRITE "${WORK}/${lane}/hashes.txt" "${hashes}")
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
  elseif(case MATCHES "^pal_")
    set(bad "${hashes}")
    set(row "pal.frame601 ${digest}\n")
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
    set(row "controller.light.lightgun.both.frame1 ${digest}\n")
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
]=])
  muhtari_add_test(NAME policy.lane_equality.controls
    COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR}
      -DWORK=${CMAKE_BINARY_DIR}/policy/lane_equality_fixture
      -P ${CMAKE_BINARY_DIR}/policy/lane_equality_controls.cmake
    LABELS policy TIMEOUT 30)
endif()
