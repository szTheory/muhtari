foreach(suffix IN ITEMS "" "-worst")
  execute_process(COMMAND ${EMULATOR} "${OBSERVER}"
    --ym-fixture "${FIXTURE_DIR}/ym2151${suffix}.a78"
    --ym-output "${OUT}/ym${suffix}-native.json"
    RESULT_VARIABLE rc TIMEOUT 25)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "YM native observer ${suffix} failed: ${rc}")
  endif()
endforeach()
