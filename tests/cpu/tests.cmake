# The CPU's tests, run on the flat test bus.
include(${CMAKE_CURRENT_LIST_DIR}/targets.cmake)

# SALLY against the committed sample of the public SingleStepTests vectors:
# final registers, raw P, RAM and every bus cycle of each test, one test per
# opcode from cpu.vectors.00 to cpu.vectors.ff. TIMEOUT 10 is provisional
# until measured.
foreach(cpu_vectors_n RANGE 255)
  math(EXPR cpu_vectors_hi "${cpu_vectors_n} / 16")
  math(EXPR cpu_vectors_lo "${cpu_vectors_n} % 16")
  string(SUBSTRING "0123456789abcdef" ${cpu_vectors_hi} 1 cpu_vectors_h)
  string(SUBSTRING "0123456789abcdef" ${cpu_vectors_lo} 1 cpu_vectors_l)
  set(cpu_vectors_op "${cpu_vectors_h}${cpu_vectors_l}")
  muhtari_add_test(NAME cpu.vectors.${cpu_vectors_op}
    COMMAND muhtari-test-vectors ${CMAKE_SOURCE_DIR}/tests/vectors/6502-sample.n65v
      ${cpu_vectors_op}
    LABELS vectors portable TIMEOUT 10)
endforeach()
