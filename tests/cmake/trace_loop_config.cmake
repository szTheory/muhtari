if(NOT DEFINED CMAKE_COMMAND OR NOT DEFINED CHECK)
  message(FATAL_ERROR "trace loop config control needs CMAKE_COMMAND and CHECK")
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" -DMUHTARI_TRACE_LOOP=OFF -DMUHTARI_TESTS=ON -P "${CHECK}"
  RESULT_VARIABLE rejected ERROR_VARIABLE rejected_err)
if(rejected STREQUAL "0" OR NOT rejected_err MATCHES
    "MUHTARI_TRACE_LOOP=OFF requires MUHTARI_TESTS=OFF")
  message(FATAL_ERROR "trace-off with tests enabled did not fail with its configuration diagnostic: ${rejected}\n${rejected_err}")
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" -DMUHTARI_TRACE_LOOP=OFF -DMUHTARI_TESTS=OFF -P "${CHECK}"
  RESULT_VARIABLE accepted ERROR_VARIABLE accepted_err)
if(NOT accepted STREQUAL "0")
  message(FATAL_ERROR "trace-off product-only configuration was rejected: ${accepted}\n${accepted_err}")
endif()
