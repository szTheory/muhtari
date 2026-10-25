if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
  file(STRINGS "${EXPECTED}" rows REGEX "^[^#]")
  list(LENGTH rows count)
  if(NOT count EQUAL 8)
    message(FATAL_ERROR "audio sample corpus must contain eight rows")
  endif()
  foreach(row IN LISTS rows)
    string(REGEX REPLACE "[ \t]+" ";" fields "${row}")
    list(GET fields 0 name)
    set(rom "${BUILT}/${name}/${name}.bas.a78")
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" --check "${rom}" "${name}" "${row}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE want ERROR_VARIABLE audit)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "audio sample ${name} failed: ${rc} ${want} ${audit}")
    endif()
    message(STATUS "${audit}")
    string(REPEAT "|....12|......|...BB|\n" 60 input)
    string(REPEAT "|......|......|...BB|\n" 540 released)
    string(APPEND input "${released}")
    file(WRITE "${CAPTURE}-${name}.movie" "muhtari-movie 1\nport1 proline\nport2 proline\n${input}")
    execute_process(COMMAND ${EMULATOR} "${RUNNER}" --movie "${CAPTURE}-${name}.movie" --hash-audio "${rom}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE got ERROR_VARIABLE err)
    string(REGEX REPLACE "^lag_frames [0-9]+\n" "" got "${got}")
    if(NOT rc STREQUAL "0" OR NOT got STREQUAL want)
      message(FATAL_ERROR "audio runner ${name} disagrees: ${rc} ${got} ${want} ${err}")
    endif()
  endforeach()
  return()
endif()

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
  # Complete two-region capture measured20.08s locally; start it early.
  # Existing120s deadline and slow-lane timeout scaling remain unchanged.
  LABELS audio portable TIMEOUT 120 COST 21)
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
  ${CMAKE_CURRENT_LIST_DIR}/../cart/images.c
  ${CMAKE_SOURCE_DIR}/runner/sha256.c)
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

add_executable(muhtari-test-irq ${CMAKE_CURRENT_LIST_DIR}/test_irq.c
  ${CMAKE_CURRENT_LIST_DIR}/../cart/images.c)
target_include_directories(muhtari-test-irq PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-irq PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-irq PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-irq)
muhtari_add_test(NAME sally.irq COMMAND muhtari-test-irq
  LABELS sally pokey portable TIMEOUT 10)
muhtari_add_test(NAME cpu.irq.timing COMMAND muhtari-test-cpu irq.timing
  LABELS sally portable TIMEOUT 10)

muhtari_add_test(NAME audio.observer COMMAND muhtari-test-audio-contracts observer
  LABELS audio portable TIMEOUT 10)

add_executable(muhtari-test-audio-samples ${CMAKE_CURRENT_LIST_DIR}/test_samples.c
  ${CMAKE_SOURCE_DIR}/runner/sha256.c)
target_include_directories(muhtari-test-audio-samples PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-audio-samples PRIVATE muhtari::muhtari)
target_compile_definitions(muhtari-test-audio-samples PRIVATE
  $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
set_target_properties(muhtari-test-audio-samples PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-audio-samples)

muhtari_add_test(NAME audio.samples
  COMMAND ${CMAKE_COMMAND} "-DDRIVER=$<TARGET_FILE:muhtari-test-audio-samples>"
    "-DRUNNER=$<TARGET_FILE:muhtari-run>" "-DEMULATOR=${CMAKE_CROSSCOMPILING_EMULATOR}"
    "-DEXPECTED=${CMAKE_CURRENT_LIST_DIR}/expected.txt" "-DBUILT=${PROJECT_BINARY_DIR}/built"
    "-DCAPTURE=${PROJECT_BINARY_DIR}/tests/audio-samples" -P ${CMAKE_CURRENT_LIST_FILE}
  # Eight 600-frame driver/runner pairs: 2 x 124.92 s, macOS Intel,
  # 11 successes on image 20260824.0482.1, run 39617761785. Cost starts early.
  LABELS audio basic portable TIMEOUT 250 COST 125)
set_tests_properties(audio.samples PROPERTIES FIXTURES_REQUIRED
  "built_soundtest;built_soundeffects;built_trackertest;built_eightysix;built_pokey;built_rmtramcompressiondemo;built_rmtramdemo;built_rmtromdemo")

add_executable(muhtari-test-audio-stress ${CMAKE_CURRENT_LIST_DIR}/test_stress.c
  ${CMAKE_CURRENT_LIST_DIR}/../cart/images.c ${CMAKE_SOURCE_DIR}/runner/clock.c)
target_include_directories(muhtari-test-audio-stress PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-audio-stress PRIVATE muhtari::muhtari)
target_compile_definitions(muhtari-test-audio-stress PRIVATE _POSIX_C_SOURCE=200809L)
set_target_properties(muhtari-test-audio-stress PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-audio-stress)
muhtari_add_test(NAME audio.stress COMMAND muhtari-test-audio-stress
  LABELS audio perf portable TIMEOUT 30)
