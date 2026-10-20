# MARIA's DMA burst through the shipped archive, with no CPU. The program
# links muhtari and includes src/internal.h: it writes display lists into
# system RAM and calls the burst directly, then checks the HALTRST tick, line
# RAM and the statistics against the costs in docs/hardware/maria.md.
add_executable(muhtari-test-maria ${CMAKE_CURRENT_LIST_DIR}/test_maria.c)
target_include_directories(muhtari-test-maria PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-maria PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-maria PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-maria)

# One burst per case, one case per argv.
foreach(maria_case IN ITEMS empty_dl headers widths indirect holes terminators last_line
    abort line_ram stats feature_mask)
  # 2 x 0.20 s, macOS x64, slowest of 31 runs, run 39351079824
  muhtari_add_test(NAME maria.dma_line.${maria_case} COMMAND muhtari-test-maria ${maria_case}
    LABELS maria portable TIMEOUT 1)
endforeach()
