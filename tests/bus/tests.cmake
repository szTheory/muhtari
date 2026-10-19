# The bus through the shipped archive. The program links muhtari and includes
# src/internal.h, acting as the CPU: it calls the two bus functions and reads
# the tick count around each call, or runs a short program through the CPU
# object in the archive (docs/hardware/bus.md).
add_executable(muhtari-test-bus ${CMAKE_CURRENT_LIST_DIR}/test_bus.c)
target_include_directories(muhtari-test-bus PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-bus PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-bus PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-bus)

# Cycle length by address and MEN, and RDY from TIA WSYNC, one case per
# argv.
foreach(bus_case IN ITEMS cycle.fast_4_ticks cycle.slow_6_ticks cycle.men_off_all_slow
    rdy.holds_reads_only rdy.rmw_completes_both_writes)
  muhtari_add_test(NAME bus.${bus_case} COMMAND muhtari-test-bus ${bus_case}
    LABELS sally portable TIMEOUT 10)
endforeach()

# INPTCTRL and the open-bus byte, which the RIOT's label covers with the
# other cases of the board's glue.
foreach(bus_case IN ITEMS inptctrl.lock open_bus.unmapped open_bus.write_only)
  muhtari_add_test(NAME bus.${bus_case} COMMAND muhtari-test-bus ${bus_case}
    LABELS riot portable TIMEOUT 10)
endforeach()
