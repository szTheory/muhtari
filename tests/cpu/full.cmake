# The full SingleStepTests 6502 set at its pin, the vectors-full preset's
# only tests: the fetch, the sample match and one test per opcode over all
# 10,000 upstream tests of its file, through the same harness as the
# per-pull-request sample. The fetch is the fixture the other 257 require, so
# a failed or partial fetch leaves them not run and the JUnit check fails.
include(${CMAKE_CURRENT_LIST_DIR}/targets.cmake)

set(MUHTARI_VECTORS_DIR ${CMAKE_BINARY_DIR}/vectors-full CACHE PATH
  "Where cpu.vectors-full.fetch checks out the full vector set")
find_package(Git REQUIRED)

# The commit and repository come from the sst-6502-v1 row of pins.txt.
file(STRINGS ${CMAKE_SOURCE_DIR}/pins.txt cpu_full_pin REGEX "^sst-6502-v1 ")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/pins.txt)
if(NOT cpu_full_pin MATCHES "^sst-6502-v1 ([0-9a-f]+) ([^ ]+) ")
  message(FATAL_ERROR "vectors-full: pins.txt has no sst-6502-v1 row")
endif()
set(cpu_full_sha ${CMAKE_MATCH_1})
set(cpu_full_url ${CMAKE_MATCH_2})

# TIMEOUT 220 is twice the slowest fetch, 109.6 s in 15 runs, run 39197673159;
# nightly.yml's vectors-full timeout adds it to twice the job's slowest run.
muhtari_add_test(NAME cpu.vectors-full.fetch
  COMMAND ${CMAKE_COMMAND} -DDIR=${MUHTARI_VECTORS_DIR} -DSHA=${cpu_full_sha} -DURL=${cpu_full_url}
    -DLIST=${CMAKE_SOURCE_DIR}/tests/vectors/6502-v1.sha256 -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
    -DGIT=${GIT_EXECUTABLE} -P ${CMAKE_SOURCE_DIR}/tests/cmake/fetch_vectors.cmake
  LABELS vectors-full TIMEOUT 220)
set_tests_properties(cpu.vectors-full.fetch PROPERTIES FIXTURES_SETUP vectors_full)

# The committed sample is the first 100 tests of each fetched file.
# TIMEOUT 5 is twice the slowest, 2.2 s in 15 runs, run 39197673159.
muhtari_add_test(NAME cpu.vectors-full.sample_match
  COMMAND ${CMAKE_COMMAND} -DVECCONV=$<TARGET_FILE:muhtari-vecconv> -DDIR=${MUHTARI_VECTORS_DIR}
    -DSAMPLE=${CMAKE_SOURCE_DIR}/tests/vectors/6502-sample.n65v
    -DWORK=${PROJECT_BINARY_DIR}/tests/sample_match
    -P ${CMAKE_SOURCE_DIR}/tests/cmake/vectors_sample_match.cmake
  LABELS vectors-full TIMEOUT 5)
set_tests_properties(cpu.vectors-full.sample_match PROPERTIES FIXTURES_REQUIRED vectors_full)

# Final registers, raw P, RAM and every bus cycle of all 10,000 tests of each
# opcode's file. TIMEOUT 1 each: the slowest took 0.08 s in 15 runs, run
# 39195625393.
foreach(cpu_full_n RANGE 255)
  math(EXPR cpu_full_hi "${cpu_full_n} / 16")
  math(EXPR cpu_full_lo "${cpu_full_n} % 16")
  string(SUBSTRING "0123456789abcdef" ${cpu_full_hi} 1 cpu_full_h)
  string(SUBSTRING "0123456789abcdef" ${cpu_full_lo} 1 cpu_full_l)
  set(cpu_full_op "${cpu_full_h}${cpu_full_l}")
  muhtari_add_test(NAME cpu.vectors-full.${cpu_full_op}
    COMMAND muhtari-test-vectors ${MUHTARI_VECTORS_DIR}/6502/v1/${cpu_full_op}.json ${cpu_full_op}
    LABELS vectors-full TIMEOUT 1)
  set_tests_properties(cpu.vectors-full.${cpu_full_op} PROPERTIES FIXTURES_REQUIRED vectors_full)
endforeach()
