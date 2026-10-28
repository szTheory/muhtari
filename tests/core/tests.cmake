# The core library through its public header. One executable runs each case
# named on its command line; the runner's SHA-256 and the in-memory image
# builders are linked as test helpers, and src/ is on the include path for
# the cases that look inside the instance.
add_executable(muhtari-test-core
  ${CMAKE_CURRENT_LIST_DIR}/test_core.c
  ${CMAKE_SOURCE_DIR}/tests/cart/images.c
  ${CMAKE_SOURCE_DIR}/runner/sha256.c)
target_include_directories(muhtari-test-core PRIVATE ${CMAKE_SOURCE_DIR}/runner
  ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-core PRIVATE muhtari::muhtari)
target_compile_definitions(muhtari-test-core PRIVATE LONG_RUN=${MUHTARI_LONG_RUN}u)
set_target_properties(muhtari-test-core PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-core)

foreach(case IN ITEMS
    testcard.pinned_hash testcard.all_colours testcard.static testcard.boundaries
    frame.pitch_336 frame.validate_first audio.silence api.size_tags
    create.allocator info)
  muhtari_add_test(NAME core.${case} COMMAND muhtari-test-core ${case}
    LABELS core portable TIMEOUT 10)
endforeach()
# Loading, input, memory spans and counters.
foreach(case IN ITEMS
    load.empty_refused load.invalid_bios set_input.validates
    set_input.latched_at_vblank get_memory.spans counters.frames_and_lag
    load.supergame_one_allocation get_memory.cart_ram)
  muhtari_add_test(NAME core.${case} COMMAND muhtari-test-core ${case}
    LABELS core portable TIMEOUT 1) # 2 x 0.06 s, Windows arm64, slowest of 31 runs, run 39375738182
endforeach()
muhtari_add_test(NAME core.load.keeps_previous COMMAND muhtari-test-core load.keeps_previous
  LABELS core portable TIMEOUT 66) # ceil(2 x 32.86s), complete Windows x64 run40414104123/job121892328427; scales retained.
foreach(case IN ITEMS tracer decode firmware topology sizes)
  muhtari_add_test(NAME core.hsc.${case} COMMAND muhtari-test-core hsc.${case}
    LABELS core portable TIMEOUT 10)
endforeach()
foreach(case IN ITEMS deadline no_poll reset)
  muhtari_add_test(NAME core.battery.${case} COMMAND muhtari-test-core battery.${case}
    LABELS core portable TIMEOUT 10)
endforeach()
muhtari_add_test(NAME core.no_alloc_after_load COMMAND muhtari-test-core no_alloc_after_load
  LABELS perf portable TIMEOUT 63) # ceil(2 x 31.26s), complete Windows x64 run40414104123/job121892328427; scales retained.

add_executable(muhtari-test-pal ${CMAKE_CURRENT_LIST_DIR}/test_pal.c
  ${CMAKE_SOURCE_DIR}/tests/cart/images.c ${CMAKE_SOURCE_DIR}/tests/cart/test_cartdb.c)
target_include_directories(muhtari-test-pal PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-pal PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-pal PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-pal)
foreach(case IN ITEMS tracer bounds lifecycle)
  muhtari_add_test(NAME core.pal.${case} COMMAND muhtari-test-pal ${case}
    LABELS core portable TIMEOUT 10)
endforeach()
