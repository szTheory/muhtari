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

# The vector tooling, on byte prefixes of upstream files in
# tests/vectors/fixtures (see the README there).
set(cpu_fixtures ${CMAKE_SOURCE_DIR}/tests/vectors/fixtures)
string(REPLACE ";" "$<SEMICOLON>" cpu_emulator "${CMAKE_CROSSCOMPILING_EMULATOR}")
set(cpu_check ${CMAKE_COMMAND} -DVECCONV=$<TARGET_FILE:muhtari-vecconv>
  -DVECTORS=$<TARGET_FILE:muhtari-test-vectors> "-DEMULATOR=${cpu_emulator}")
muhtari_add_test(NAME cpu.vectors.convert.a9
  COMMAND ${cpu_check} -DCASE=convert -DIN=${cpu_fixtures}/a9.json -DOP=a9 -DFIRST=5
    -DOUT=${PROJECT_BINARY_DIR}/tests/convert-a9.n65v -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS vectors portable TIMEOUT 10)
