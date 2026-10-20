# The ledger's driver, shared by the title tests and the library preset; it
# registers no test. muhtari-test-predicate evaluates one ledger predicate on
# a ROM, on the fast boot or through a BIOS; it includes src/internal.h to
# read the hand-off frame K, the CPU's jam flag and MARIA's line statistics.
add_executable(muhtari-test-predicate ${CMAKE_CURRENT_LIST_DIR}/predicate.c
  ${CMAKE_SOURCE_DIR}/runner/sha256.c)
target_include_directories(muhtari-test-predicate PRIVATE ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/runner)
target_link_libraries(muhtari-test-predicate PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-predicate PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-predicate)

set(compat_ledger ${CMAKE_CURRENT_LIST_DIR}/ledger.csv)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${compat_ledger})
