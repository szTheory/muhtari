# The RIOT through the shipped archive. The program links muhtari and
# includes src/internal.h, acting as the CPU: every access goes through the
# bus's two functions, so the timer counts the bus's own cycles
# (docs/hardware/riot.md).
add_executable(muhtari-test-riot ${CMAKE_CURRENT_LIST_DIR}/test_riot.c)
target_include_directories(muhtari-test-riot PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-riot PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-riot PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-riot)

# The timer at every interval, one case per argv.
foreach(riot_case IN ITEMS timer.reads_n_minus_1)
  muhtari_add_test(NAME riot.${riot_case} COMMAND muhtari-test-riot ${riot_case}
    LABELS riot portable TIMEOUT 10)
endforeach()
