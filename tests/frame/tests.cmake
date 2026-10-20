# The run loop through the shipped archive. The program links muhtari and
# includes src/internal.h: it builds linear cartridges in memory from short
# programs of its own, loads them through the fast-boot hand-off and runs
# whole frames (docs/hardware/bus.md, The run loop).
add_executable(muhtari-test-frame
  ${CMAKE_CURRENT_LIST_DIR}/test_frame.c
  ${CMAKE_SOURCE_DIR}/tests/cart/images.c)
target_include_directories(muhtari-test-frame PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-frame PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-frame PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-frame)

# One case per argv.
foreach(frame_case IN ITEMS jam_ends)
  # provisional, phase 3
  muhtari_add_test(NAME frame.${frame_case} COMMAND muhtari-test-frame ${frame_case}
    LABELS frame portable TIMEOUT 10)
endforeach()
