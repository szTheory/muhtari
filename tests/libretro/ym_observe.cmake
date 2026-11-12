if(NOT DEFINED OBSERVER_TIMEOUT)
  set(OBSERVER_TIMEOUT 25)
endif()
if(NOT "${OBSERVER_TIMEOUT}" MATCHES "^[1-9][0-9]*$")
  message(FATAL_ERROR "OBSERVER_TIMEOUT must be a positive whole number")
endif()

foreach(suffix IN ITEMS "" "-worst")
  execute_process(COMMAND ${EMULATOR} "${OBSERVER}"
    --ym-fixture "${FIXTURE_DIR}/ym2151${suffix}.a78"
    --ym-output "${OUT}/ym${suffix}-native.json"
    RESULT_VARIABLE rc TIMEOUT "${OBSERVER_TIMEOUT}")
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "YM native observer ${suffix} failed: ${rc}")
  endif()
endforeach()
