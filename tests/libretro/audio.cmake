# Fault prevented: actual host samples differ from canonical runner output.
string(REPEAT "|....12|......|...BB|\n" 30 input)
file(WRITE "${OUT}.movie" "muhtari-movie 1\nport1 proline\nport2 proline\n${input}")
execute_process(COMMAND ${EMULATOR} "${RUNNER}" --movie "${OUT}.movie" --hash-audio "${ROM}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE expected ERROR_VARIABLE err)
if(NOT rc STREQUAL "0" OR NOT expected MATCHES "audio_pcm_sha256 ([0-9a-f]+)")
  message(FATAL_ERROR "runner audio reference failed: ${rc} ${err}")
endif()
set(hash "${CMAKE_MATCH_1}")
execute_process(COMMAND ${EMULATOR} "${HOST}" "${MODULE}" audio "${ROM}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE actual ERROR_VARIABLE err)
if(NOT rc STREQUAL "0" OR NOT actual STREQUAL "audio_pcm_sha256 ${hash}\n")
  message(FATAL_ERROR "host audio mismatch: ${rc} ${actual} ${err}; expected ${hash}")
endif()
