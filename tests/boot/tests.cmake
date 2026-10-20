# The BIOS boot: OpenBIOS, which built.openbios assembles, booting the
# samples built.simple builds (tests/boot/test_boot.c). The program links
# muhtari and includes src/internal.h, to read the hand-off frame and the
# CPU's state after RES.
add_executable(muhtari-test-boot ${CMAKE_CURRENT_LIST_DIR}/test_boot.c)
target_include_directories(muhtari-test-boot PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-boot PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-boot PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-boot)

set(boot_bios ${MUHTARI_BUILT_DIR}/openbios/openbios.bin)
set(boot_simple ${MUHTARI_BUILT_DIR}/simple/simple.bas.a78)
foreach(boot_case IN ITEMS handoff_simple)
  muhtari_add_test(NAME bios.${boot_case}
    COMMAND muhtari-test-boot ${boot_case} ${boot_bios} ${boot_simple}
    LABELS bios portable TIMEOUT 30)  # provisional, phase 3
  set_tests_properties(bios.${boot_case} PROPERTIES
    FIXTURES_REQUIRED "built_openbios;built_simple")
endforeach()
