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
# core's object files, not the archive, so a test list can stand in for the
# bundled tables.
add_executable(muhtari-test-cart
  ${CMAKE_CURRENT_LIST_DIR}/test_cart.c
  ${CMAKE_CURRENT_LIST_DIR}/images.c
  $<TARGET_OBJECTS:muhtari_objs>
  $<TARGET_OBJECTS:muhtari_cpu>
  $<TARGET_OBJECTS:muhtari_cartdb>)
target_include_directories(muhtari-test-cart PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
set_target_properties(muhtari-test-cart PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-cart)
foreach(cart_case IN ITEMS probe.accepted probe.arguments probe.oversized probe.2600
    probe.unsupported_board probe.malformed probe.truncated probe.warnings)
  muhtari_add_test(NAME cart.${cart_case} COMMAND muhtari-test-cart ${cart_case}
    LABELS cart portable TIMEOUT 10)
endforeach()
