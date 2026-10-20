# The compatibility ledger's title predicates (docs/emulation-tests.md,
# title). muhtari-test-predicate evaluates one ledger predicate on a ROM, on
# the fast boot or through a BIOS; it includes src/internal.h to read the
# hand-off frame K, the CPU's jam flag and MARIA's line statistics.
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

# simple's title on both paths, its predicate read from the ledger here, so
# a re-pinned row reaches the tests at the next configure.
file(STRINGS ${compat_ledger} compat_simple REGEX "^7800basic-0[.]41/simple,")
string(REGEX MATCH ",(title@120=[0-9a-f]+)," compat_simple "${compat_simple}")
if(NOT CMAKE_MATCH_1)
  message(FATAL_ERROR "compat: ${compat_ledger} has no pinned title@120 row for 7800basic-0.41/simple")
endif()
set(compat_simple_title ${CMAKE_MATCH_1})
set(compat_simple_rom ${MUHTARI_BUILT_DIR}/simple/simple.bas.a78)
set(compat_bios ${MUHTARI_BUILT_DIR}/openbios/openbios.bin)
muhtari_add_test(NAME title.simple.fast
  COMMAND muhtari-test-predicate ${compat_simple_rom} ${compat_simple_title}
  LABELS portable title TIMEOUT 60)  # provisional, phase 3
muhtari_add_test(NAME title.simple.bios
  COMMAND muhtari-test-predicate ${compat_simple_rom} ${compat_simple_title} --bios ${compat_bios}
  LABELS portable title TIMEOUT 60)  # provisional, phase 3
set_tests_properties(title.simple.fast title.simple.bios PROPERTIES
  FIXTURES_REQUIRED "built_simple;built_openbios")
