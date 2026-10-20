# The run loop through the shipped archive. The program links muhtari and
# includes src/internal.h: it builds linear cartridges in memory from short
# programs of its own, loads them through the fast-boot hand-off and runs
# whole frames (docs/hardware/bus.md, The run loop). The runner's SHA-256
# is linked as a test helper.
add_executable(muhtari-test-frame
  ${CMAKE_CURRENT_LIST_DIR}/test_frame.c
  ${CMAKE_SOURCE_DIR}/tests/cart/images.c
  ${CMAKE_SOURCE_DIR}/runner/sha256.c)
target_include_directories(muhtari-test-frame PRIVATE ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/runner)
target_link_libraries(muhtari-test-frame PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-frame PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-frame)

# One case per argv.
foreach(frame_case IN ITEMS jam_ends dma_off_ends hold_split_wsync_across_dma
    halt_c1_write_visible maria_wsync_releases_at_lrc mstat_edges handoff_state
    first_frame_partial dma_blocked_flag dli_queued null_video_same_state)
  # provisional, phase 3
  muhtari_add_test(NAME frame.${frame_case} COMMAND muhtari-test-frame ${frame_case}
    LABELS frame portable TIMEOUT 10)
endforeach()
