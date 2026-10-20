# Our own diagnostic ROMs (docs/emulation-tests.md, diag). Each tests/diag/*.s
# is MIT code of this repository, assembled into the committed .a78 beside it
# by assemble.cmake. muhtari-test-diag runs one and judges its mailbox against
# expected.csv, whose values docs/hardware/maria.md derives before a ROM
# first runs.
set(diag_dir ${CMAKE_CURRENT_LIST_DIR})
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${diag_dir}/expected.csv
  ${diag_dir}/stats.csv)

# muhtari-test-diag: links muhtari and includes src/internal.h, to read the
# mailbox through muhtari__peek and the core's per-line statistics, which
# stats.csv gives for each DMA test, so each cost is recorded twice.
add_executable(muhtari-test-diag ${diag_dir}/driver.c)
target_include_directories(muhtari-test-diag PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-diag PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-diag PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-diag)

muhtari_add_test(NAME diag.dma
  COMMAND muhtari-test-diag ${diag_dir}/dma.a78 ${diag_dir}/expected.csv ${diag_dir}/stats.csv
  LABELS diag portable TIMEOUT 30)  # provisional, phase 3
