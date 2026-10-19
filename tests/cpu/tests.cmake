# The CPU's tests, run on the flat test bus.
#
# This file is also the vector tooling checks themselves, run as
#   cmake -DVECCONV=<muhtari-vecconv> -DVECTORS=<muhtari-test-vectors>
#     [-DEMULATOR=<list>] -DCASE=<name> -DIN=<fixture> -DOP=<xx> -DFIRST=<n>
#     -DOUT=<file> -P tests/cpu/tests.cmake
# EMULATOR is the build's CMAKE_CROSSCOMPILING_EMULATOR, so an emulated lane
# runs its own guest binaries.
if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
  # Runs ARGN under the emulator; fails unless it exits 0 and its output
  # matches regex.
  function(expect_pass regex)
    execute_process(COMMAND ${EMULATOR} ${ARGN}
      OUTPUT_VARIABLE out ERROR_VARIABLE out RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0" OR NOT out MATCHES "${regex}")
      message(FATAL_ERROR "${ARGN}: expected exit 0 and output matching\n  ${regex}\n"
        "got exit ${rc} and output\n${out}")
    endif()
  endfunction()

  # Runs ARGN under the emulator through expect_fail.cmake: exit code and
  # output matching regex, or the test fails.
  function(expect_exit code regex)
    set(CMD ${EMULATOR} ${ARGN})
    set(CODE ${code})
    set(REGEX "${regex}")
    include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cmake/expect_fail.cmake)
  endfunction()

  if(CASE STREQUAL "convert")
    # Fault prevented: the converter and the harness disagree about an
    # upstream file, so the committed sample, which the converter builds,
    # tests something other than the upstream vectors. The fixture's first
    # FIRST tests are converted, then run both from the converted chunk and
    # from the JSON directly; every one must pass both ways.
    file(REMOVE "${OUT}")
    expect_pass("${OP}: ${FIRST} tests written and re-read" "${VECCONV}" "${IN}" "${OUT}" ${OP}
      --first ${FIRST})
    expect_pass("6502/${OP}: 0 of ${FIRST} failed" "${VECTORS}" "${OUT}" ${OP})
    expect_pass("6502/${OP}: 0 of ${FIRST} failed" "${VECTORS}" "${IN}" ${OP} --first ${FIRST})
  elseif(CASE STREQUAL "tail")
    # Fault prevented: a prefix whose last test is cut off converts as if it
    # were whole, or fails without saying where. --first FIRST, the prefix's
    # whole tests, converts; one more must fail at the offset where the input
    # ends.
    file(SIZE "${IN}" size)
    file(REMOVE "${OUT}")
    expect_pass("${OP}: ${FIRST} tests written and re-read" "${VECCONV}" "${IN}" "${OUT}" ${OP}
      --first ${FIRST})
    math(EXPR more "${FIRST} + 1")
    expect_exit(1 "offset ${size}: input ends" "${VECCONV}" "${IN}" "${OUT}" ${OP} --first ${more})
  elseif(CASE STREQUAL "reject")
    # Fault prevented: the converter accepts JSON outside the vector schema,
    # so a damaged download converts into a sample that tests the wrong
    # thing. Each input below has its fault at byte 30, which the error must
    # name.
    set(lead "[{\"name\":\"a9\",\"initial\":{\"pc\":")
    foreach(case IN ITEMS "bad_byte|?|expected a number" "leading_zero|01|number with a leading zero"
        "negative|-1|signed number")
      string(REPLACE "|" ";" case "${case}")
      list(GET case 0 name)
      list(GET case 1 value)
      list(GET case 2 reason)
      file(WRITE "${OUT}/reject-${name}.json" "${lead}${value}}}]")
      expect_exit(1 "offset 30: ${reason}" "${VECCONV}" "${OUT}/reject-${name}.json"
        "${OUT}/reject-${name}.n65v" a9 --first 1)
    endforeach()
  elseif(CASE STREQUAL "self_test")
    # Fault prevented: the harness passes a test whose final state differs
    # from the vector's, or reports it without the file and test index. The
    # fixture is one a9 test with its final A changed.
    expect_exit(1 "a9\\.json\\[0\\]: a expected" "${VECTORS}" "${IN}" a9 --first 1)
  else()
    message(FATAL_ERROR "unknown CASE '${CASE}'")
  endif()
  return()
endif()

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

# The default ANE and LXA constant gives the value real SALLYs read.
muhtari_add_test(NAME cpu.unstable.sally_constant
  COMMAND muhtari-test-cpu unstable.sally_constant
  LABELS vectors portable TIMEOUT 10)

# NMI timing on the flat bus: the edge's phi2 count against each
# instruction's deciding cycle, with and without held cycles
# (docs/hardware/sally.md, NMI).
foreach(cpu_nmi_case IN ITEMS penultimate_taken_after last_cycle_taken_after_next
    branch_no_cross_polls_early hijacks_brk held_deciding_cycle_taken_after
    held_last_cycle_taken_after_next)
  muhtari_add_test(NAME cpu.nmi.${cpu_nmi_case}
    COMMAND muhtari-test-cpu nmi.${cpu_nmi_case}
    LABELS sally portable TIMEOUT 10)
endforeach()

# Bruce Clark's decimal mode test, assembled from tests/cpu/decimal/clark.s,
# over all 262,144 ADC and SBC cases on A, C, N, V and Z. The 65C02 build of
# the same listing predicts other flags, so SALLY must fail it: ERROR = 1.
# TIMEOUT 60 is provisional until measured.
set(cpu_decimal ${CMAKE_SOURCE_DIR}/tests/cpu/decimal)
muhtari_add_test(NAME cpu.decimal.clark
  COMMAND muhtari-test-cpu decimal.clark ${cpu_decimal}/clark-6502.bin 0
  LABELS vectors portable TIMEOUT 60)
muhtari_add_test(NAME cpu.decimal.clark.self_test
  COMMAND muhtari-test-cpu decimal.clark ${cpu_decimal}/clark-65c02.bin 1
  LABELS vectors portable TIMEOUT 60)

# The vector tooling, on byte prefixes of upstream files in
# tests/vectors/fixtures (see the README there).
set(cpu_fixtures ${CMAKE_SOURCE_DIR}/tests/vectors/fixtures)
string(REPLACE ";" "$<SEMICOLON>" cpu_emulator "${CMAKE_CROSSCOMPILING_EMULATOR}")
set(cpu_check ${CMAKE_COMMAND} -DVECCONV=$<TARGET_FILE:muhtari-vecconv>
  -DVECTORS=$<TARGET_FILE:muhtari-test-vectors> "-DEMULATOR=${cpu_emulator}")
set(cpu_out ${PROJECT_BINARY_DIR}/tests)
foreach(cpu_case IN ITEMS "convert.a9|convert|a9.json|a9|5" "convert.02|convert|02.json|02|5"
    "convert.tail|tail|a9.json|a9|5" "convert.reject|reject|-|a9|1"
    "self_test|self_test|a9-bad-a.json|a9|1")
  string(REPLACE "|" ";" cpu_case "${cpu_case}")
  list(GET cpu_case 0 cpu_name)
  list(GET cpu_case 1 cpu_script_case)
  list(GET cpu_case 2 cpu_in)
  list(GET cpu_case 3 cpu_op)
  list(GET cpu_case 4 cpu_first)
  set(cpu_target ${cpu_out}/${cpu_name}.n65v)
  if(cpu_script_case STREQUAL "reject")
    set(cpu_target ${cpu_out})
  endif()
  muhtari_add_test(NAME cpu.vectors.${cpu_name}
    COMMAND ${cpu_check} -DCASE=${cpu_script_case} -DIN=${cpu_fixtures}/${cpu_in} -DOP=${cpu_op}
      -DFIRST=${cpu_first} -DOUT=${cpu_target} -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS vectors portable TIMEOUT 10)
endforeach()

# The N65V reader refuses an empty buffer, a chunk with no tests, a wrong
# chunk or test count and every cut, naming the byte offset.
muhtari_add_test(NAME cpu.vectors.reader COMMAND muhtari-test-reader
  LABELS vectors portable TIMEOUT 10)

# The per-file list of the full set, its pins.txt row and the sample's
# manifest pin agree.
muhtari_add_test(NAME cpu.vectors.pin
  COMMAND ${CMAKE_COMMAND} -DLIST=${CMAKE_SOURCE_DIR}/tests/vectors/6502-v1.sha256
    -DPINS=${CMAKE_SOURCE_DIR}/pins.txt -DMANIFEST=${CMAKE_SOURCE_DIR}/tests/roms/manifest.txt
    -P ${CMAKE_SOURCE_DIR}/tests/cmake/vectors_pin.cmake
  LABELS vectors TIMEOUT 10)
