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

# The timer at every interval, the flags, PA7, the ports, the power-on state
# and the RAM, one case per argv.
foreach(riot_case IN ITEMS timer.reads_n_minus_1 timer.flag_at_n_interval_plus_1
    timer.counts_past_underflow timer.read_in_setting_cycle_keeps_flag
    timer.access_clears_flag timer.counts_through_halt timer.counts_through_wsync
    timer.slow_cycle_counts_one pa7.edge_flag swcha.ddr swchb.ddr power_on.defaults
    ram.window)
  muhtari_add_test(NAME riot.${riot_case} COMMAND muhtari-test-riot ${riot_case}
    LABELS riot portable TIMEOUT 10)
endforeach()
