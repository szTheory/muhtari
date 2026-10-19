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

# muhtari_probe on images built in memory by images.c. The program links the
# core's object files with test_cartdb.c in place of the bundled tables
# (link-time substitution), so every table path runs on synthetic bytes and
# the shipped archive never holds a test entry.
add_executable(muhtari-test-cart
  ${CMAKE_CURRENT_LIST_DIR}/test_cart.c
  ${CMAKE_CURRENT_LIST_DIR}/images.c
  ${CMAKE_CURRENT_LIST_DIR}/test_cartdb.c
  $<TARGET_OBJECTS:muhtari_objs>
  $<TARGET_OBJECTS:muhtari_cpu>)
target_include_directories(muhtari-test-cart PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
set_target_properties(muhtari-test-cart PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-cart)
foreach(cart_case IN ITEMS probe.accepted probe.arguments probe.oversized probe.2600
    probe.unsupported_board probe.malformed probe.truncated probe.warnings
    identify.hash_override identify.slices identify.activision_swap identify.2600_prefix
    identify.not_a_cartridge identify.headerless_ports)
  muhtari_add_test(NAME cart.${cart_case} COMMAND muhtari-test-cart ${cart_case}
    LABELS cart portable TIMEOUT 10)
endforeach()

# No test-list key is in the bundled tables. The self test appends the test
# list's first key row to a copy of src/cartdb.c and expects it named.
set(cart_scripts ${PROJECT_SOURCE_DIR}/tests/cmake)
set(cart_test_list ${CMAKE_CURRENT_LIST_DIR}/test_cartdb.c)
muhtari_add_test(NAME cart.identify.bundled_has_no_test_entries
  COMMAND ${CMAKE_COMMAND} -DTEST_LIST=${cart_test_list}
    -DBUNDLED=${PROJECT_SOURCE_DIR}/src/cartdb.c -P ${cart_scripts}/no_test_entries.cmake
  LABELS cart portable TIMEOUT 10)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/src/cartdb.c ${cart_test_list})
file(READ ${PROJECT_SOURCE_DIR}/src/cartdb.c cart_leak)
file(READ ${cart_test_list} cart_test_text)
string(REGEX MATCH "{{0x[^}]*}" cart_test_key "${cart_test_text}")
file(WRITE ${PROJECT_BINARY_DIR}/tests/cart/cartdb_leak.c "${cart_leak}${cart_test_key},0,0},\n")
set(cart_cmd ${CMAKE_COMMAND} -DTEST_LIST=${cart_test_list}
  -DBUNDLED=${PROJECT_BINARY_DIR}/tests/cart/cartdb_leak.c -P ${cart_scripts}/no_test_entries.cmake)
string(REPLACE ";" "$<SEMICOLON>" cart_cmd "${cart_cmd}")
muhtari_add_test(NAME cart.identify.bundled_has_no_test_entries.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${cart_cmd}" -DCODE=1
    "-DREGEX=holds the test-list key 1aa9559b74eab35d4c5443b56bc4dfd7a445ac4a"
    -P ${cart_scripts}/expect_fail.cmake
  LABELS cart portable TIMEOUT 10)
