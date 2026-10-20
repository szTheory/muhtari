add_executable(muhtari-test-spectral ${CMAKE_CURRENT_LIST_DIR}/test_spectral.c)
target_include_directories(muhtari-test-spectral PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-spectral PRIVATE muhtari::muhtari)
if(NOT MSVC)
  target_link_libraries(muhtari-test-spectral PRIVATE m)
endif()
set_target_properties(muhtari-test-spectral PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-spectral)
muhtari_add_test(NAME audio.spectral COMMAND muhtari-test-spectral production
  LABELS audio portable TIMEOUT 120)
muhtari_add_test(NAME audio.spectral.controls COMMAND muhtari-test-spectral controls
  LABELS audio portable TIMEOUT 120)
