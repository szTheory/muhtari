foreach(suffix IN ITEMS "" "-worst")
  execute_process(COMMAND "${NODE}" "${HOST}"
    --module "${MODULE}" --fixture "${FIXTURES}/ym2151${suffix}.a78"
    --native "${NATIVE_DIR}/ym${suffix}-native.json" --mode "${MODE}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 25)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "portable YM ${suffix} ${MODE}: ${rc} ${out}${err}")
  endif()
  message(STATUS "${out}")
endforeach()
