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

foreach(case IN ITEMS
    testcard.pinned_hash testcard.all_colours testcard.static testcard.boundaries
    frame.pitch_336 frame.validate_first audio.silence api.size_tags
    create.allocator info)
  muhtari_add_test(NAME core.${case} COMMAND muhtari-test-core ${case}
    LABELS core portable TIMEOUT 10)
endforeach()
muhtari_add_test(NAME core.no_alloc_after_load COMMAND muhtari-test-core no_alloc_after_load
  LABELS perf portable TIMEOUT 10)

# The frozen API v1 program: every function, field and constant of version 1.
add_executable(muhtari-test-api-v1 ${CMAKE_SOURCE_DIR}/tests/api/v1.c)
target_link_libraries(muhtari-test-api-v1 PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-api-v1 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-api-v1)
muhtari_add_test(NAME abi.api_v1 COMMAND muhtari-test-api-v1
  LABELS abi portable TIMEOUT 10)
