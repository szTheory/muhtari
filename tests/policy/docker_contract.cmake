cmake_minimum_required(VERSION 3.25)

if(NOT DEFINED ROOT OR NOT DEFINED WORK OR NOT DEFINED CASE)
  message(FATAL_ERROR "docker_contract test requires ROOT, WORK and CASE")
endif()
file(MAKE_DIRECTORY "${WORK}")

if(CASE STREQUAL "object")
  set(input "${WORK}/object.json")
  set(output "${WORK}/normalized.json")
  file(WRITE "${input}" [=[{"Id":"sha256:aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa","Name":"/muhtari-control","Config":{"User":"1000:1000","Labels":{"org.muhtari.gap24.owner":"muhtari-gap24-control"}}}]=])
  execute_process(
    COMMAND "${CMAKE_COMMAND}" "-DMODE=container" "-DINPUT=${input}" "-DOUTPUT=${output}"
      -P "${ROOT}/tests/cmake/docker_contract.cmake"
    RESULT_VARIABLE rc OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr)
  if(NOT rc EQUAL 0)
    message(FATAL_ERROR "docker_contract.object_shape assertion failed: a valid object inspect must normalize successfully; observed ${stdout}${stderr}")
  endif()
  if(NOT EXISTS "${output}")
    message(FATAL_ERROR "docker_contract.object_shape assertion failed: normalized object output is missing")
  endif()
  file(READ "${output}" normalized)
  string(JSON id ERROR_VARIABLE json_error GET "${normalized}" Id)
  if(json_error OR NOT id STREQUAL "sha256:aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")
    message(FATAL_ERROR "docker_contract.object_shape assertion failed: normalized object did not preserve its full ID")
  endif()
  message(STATUS "docker_contract.object_shape passed")
  return()
endif()

message(FATAL_ERROR "docker_contract: unsupported test case ${CASE}")
