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

add_executable(muhtari-test-tia ${CMAKE_CURRENT_LIST_DIR}/test_tia.c)
target_include_directories(muhtari-test-tia PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-tia PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-tia PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-tia)
muhtari_add_test(NAME tia.logic COMMAND muhtari-test-tia
  LABELS tia portable TIMEOUT 10)

add_executable(muhtari-test-audio-contracts ${CMAKE_CURRENT_LIST_DIR}/test_contracts.c
  ${CMAKE_CURRENT_LIST_DIR}/../cart/images.c)
target_include_directories(muhtari-test-audio-contracts PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-audio-contracts PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-audio-contracts PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-audio-contracts)
muhtari_add_test(NAME audio.frames COMMAND muhtari-test-audio-contracts
  LABELS audio portable TIMEOUT 10)

add_executable(muhtari-test-pokey ${CMAKE_CURRENT_LIST_DIR}/test_pokey.c
  ${CMAKE_CURRENT_LIST_DIR}/../cart/images.c)
target_include_directories(muhtari-test-pokey PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-pokey PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-pokey PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-pokey)
muhtari_add_test(NAME pokey.logic COMMAND muhtari-test-pokey logic
  LABELS pokey portable TIMEOUT 10)
muhtari_add_test(NAME pokey.clock COMMAND muhtari-test-pokey clock
  LABELS pokey portable TIMEOUT 10)
muhtari_add_test(NAME pokey.maps COMMAND muhtari-test-pokey maps
  LABELS pokey portable TIMEOUT 10)

add_executable(muhtari-test-irq ${CMAKE_CURRENT_LIST_DIR}/test_irq.c)
target_include_directories(muhtari-test-irq PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-irq PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-irq PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-irq)
muhtari_add_test(NAME sally.irq COMMAND muhtari-test-irq
  LABELS sally portable TIMEOUT 10)
muhtari_add_test(NAME cpu.irq.timing COMMAND muhtari-test-cpu irq.timing
  LABELS sally portable TIMEOUT 10)
