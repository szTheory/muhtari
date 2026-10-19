# The CPU's tests, run on the flat test bus.
include(${CMAKE_CURRENT_LIST_DIR}/targets.cmake)

# SALLY against the committed sample of the public SingleStepTests vectors:
# final registers, raw P, RAM and every bus cycle of each test of opcode a9.
muhtari_add_test(NAME cpu.vectors.a9
  COMMAND muhtari-test-vectors ${CMAKE_SOURCE_DIR}/tests/vectors/6502-sample.n65v a9
  LABELS vectors portable TIMEOUT 10)
