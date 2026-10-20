# The BIOS boot: OpenBIOS, which built.openbios assembles, booting the
# samples built.simple builds, and a BIOS and cartridge of its own built in
# memory (tests/boot/test_boot.c). The program links muhtari and includes
# src/internal.h, to read the hand-off frame and the CPU's state after RES.
add_executable(muhtari-test-boot ${CMAKE_CURRENT_LIST_DIR}/test_boot.c
  ${CMAKE_SOURCE_DIR}/tests/cart/images.c)
target_include_directories(muhtari-test-boot PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-boot PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-boot PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-boot)

set(boot_bios ${MUHTARI_BUILT_DIR}/openbios/openbios.bin)
set(boot_simple ${MUHTARI_BUILT_DIR}/simple/simple.bas.a78)
# OpenBIOS booting simple, and simple with its region byte cleared.
foreach(boot_case IN ITEMS handoff_simple splash_skip rejects_cart)
  muhtari_add_test(NAME bios.${boot_case}
    COMMAND muhtari-test-boot ${boot_case} ${boot_bios} ${boot_simple}
    LABELS bios portable TIMEOUT 1)  # 2 x 0.40 s, macOS x64, slowest of 14 runs, run 39382016958
  set_tests_properties(bios.${boot_case} PROPERTIES
    FIXTURES_REQUIRED "built_openbios;built_simple")
endforeach()
# The window's edges, and a BIOS and cartridge built in memory.
foreach(boot_case IN ITEMS k_window k_first_fetch)
  muhtari_add_test(NAME bios.${boot_case} COMMAND muhtari-test-boot ${boot_case}
    LABELS bios portable TIMEOUT 1)  # 2 x 0.10 s, macOS x64, slowest of 13 runs, run 39373524239
endforeach()
