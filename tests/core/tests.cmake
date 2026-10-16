# The core library through its public header. One executable runs each case
# named on its command line; the runner's SHA-256 is linked as a test helper.
add_executable(muhtari-test-core
  ${CMAKE_CURRENT_LIST_DIR}/test_core.c
  ${CMAKE_SOURCE_DIR}/runner/sha256.c)
target_include_directories(muhtari-test-core PRIVATE ${CMAKE_SOURCE_DIR}/runner)
target_link_libraries(muhtari-test-core PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-core PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-core)

foreach(case IN ITEMS testcard.pinned_hash)
  string(REGEX REPLACE "^[a-z]+\\." "" arg "${case}")
  muhtari_add_test(NAME core.${case} COMMAND muhtari-test-core ${arg}
    LABELS core portable TIMEOUT 10)
endforeach()
