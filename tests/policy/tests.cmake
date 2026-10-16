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

# Each derived file has a notice, each notice has a file, and the libretro
# .info licence field agrees with both.
muhtari_add_test(NAME policy.notices
  COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR} -P ${policy_scripts}/notices.cmake
  LABELS policy TIMEOUT 10)
