# Cartridge identification. SHA-1 runs through the shipped archive against
# the FIPS 180-4 example messages and the one-million-'a' message, the last
# fed in uneven chunks so updates start and end at every place in a block.
add_executable(muhtari-test-sha1 ${CMAKE_CURRENT_LIST_DIR}/test_sha1.c)
target_include_directories(muhtari-test-sha1 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-sha1 PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-sha1 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-sha1)
muhtari_add_test(NAME cart.sha1.fips COMMAND muhtari-test-sha1
  LABELS cart portable TIMEOUT 10)
