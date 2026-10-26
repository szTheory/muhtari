# The loader and run fuzz targets. Two configurations include this file and
# register disjoint sets: the ci and wasm32 lanes run the replays on every
# platform, and the fuzz preset (MUHTARI_FUZZ, clang-21) runs libFuzzer itself.
set(fuzz_dir ${CMAKE_CURRENT_LIST_DIR})
set(fuzz_regress_dir ${fuzz_dir}/loader/regress)
set(fuzz_run_regress_dir ${fuzz_dir}/run/regress)

# muhtari-test-images DIR writes every seed image into a build directory,
# for the libFuzzer seeds and the runner tests.
add_executable(muhtari-test-images
  ${CMAKE_SOURCE_DIR}/tests/cart/write_images.c
  ${fuzz_dir}/seeds.c
  ${CMAKE_SOURCE_DIR}/tests/cart/images.c)
target_link_libraries(muhtari-test-images PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-images PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-images)

# muhtari-fuzz-loader-replay runs tests/fuzz/loader.c's invariants over the
# in-memory seeds from seeds.c and every committed regression input.
add_executable(muhtari-fuzz-loader-replay
  ${fuzz_dir}/loader.c
  ${fuzz_dir}/seeds.c
  ${CMAKE_SOURCE_DIR}/tests/cart/images.c
  ${CMAKE_SOURCE_DIR}/tests/cart/test_cartdb.c
  $<TARGET_OBJECTS:muhtari_objs> $<TARGET_OBJECTS:muhtari_cpu>)
target_compile_definitions(muhtari-fuzz-loader-replay PRIVATE MUHTARI_FUZZ_REPLAY)
target_include_directories(muhtari-fuzz-loader-replay PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)
set_target_properties(muhtari-fuzz-loader-replay PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-fuzz-loader-replay)

# muhtari-fuzz-run-replay runs tests/fuzz/run.c's invariants over the run
# target's seeds from run_seeds.c and every committed regression input.
add_executable(muhtari-fuzz-run-replay ${fuzz_dir}/run.c ${fuzz_dir}/run_seeds.c)
target_compile_definitions(muhtari-fuzz-run-replay PRIVATE MUHTARI_FUZZ_REPLAY
  $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
target_include_directories(muhtari-fuzz-run-replay PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-fuzz-run-replay PRIVATE muhtari::muhtari)
set_target_properties(muhtari-fuzz-run-replay PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-fuzz-run-replay)

# Fault prevented: malformed EEPROM pin packets publishing early, aliasing a
# second instance/HSC bytes, or making traffic allocate after load. The same
# bounded consumer runs in portable replay and the existing nightly fuzz lane.
add_executable(muhtari-fuzz-i2c-replay ${fuzz_dir}/i2c.c)
target_compile_definitions(muhtari-fuzz-i2c-replay PRIVATE MUHTARI_FUZZ_REPLAY
  $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
target_include_directories(muhtari-fuzz-i2c-replay PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-fuzz-i2c-replay PRIVATE muhtari::muhtari)
set_target_properties(muhtari-fuzz-i2c-replay PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-fuzz-i2c-replay)

add_executable(muhtari-fuzz-i2c-fault ${fuzz_dir}/i2c.c)
target_compile_definitions(muhtari-fuzz-i2c-fault PRIVATE MUHTARI_FUZZ_REPLAY MUHTARI_I2C_FAULT
  $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
target_include_directories(muhtari-fuzz-i2c-fault PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-fuzz-i2c-fault PRIVATE muhtari::muhtari)
set_target_properties(muhtari-fuzz-i2c-fault PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-fuzz-i2c-fault)

# Committed regressions, each an input that once broke an invariant. A
# directory appears with its first one; the replays take files, not
# directories, so they are listed here.
set(fuzz_regress "")
if(EXISTS ${fuzz_regress_dir})
  file(GLOB fuzz_regress CONFIGURE_DEPENDS ${fuzz_regress_dir}/*)
  list(SORT fuzz_regress)
endif()
set(fuzz_run_regress "")
if(EXISTS ${fuzz_run_regress_dir})
  file(GLOB fuzz_run_regress CONFIGURE_DEPENDS ${fuzz_run_regress_dir}/*)
  list(SORT fuzz_run_regress)
endif()

if(MUHTARI_FUZZ)
  add_executable(muhtari-fuzz-i2c ${fuzz_dir}/i2c.c)
  target_include_directories(muhtari-fuzz-i2c PRIVATE ${CMAKE_SOURCE_DIR}/src)
  target_link_libraries(muhtari-fuzz-i2c PRIVATE muhtari::muhtari)
  target_compile_options(muhtari-fuzz-i2c PRIVATE -fsanitize=fuzzer)
  target_link_options(muhtari-fuzz-i2c PRIVATE -fsanitize=fuzzer)
  set_target_properties(muhtari-fuzz-i2c PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  muhtari_warnings(muhtari-fuzz-i2c)
  set(i2c_work ${PROJECT_BINARY_DIR}/fuzz/i2c)
  file(MAKE_DIRECTORY ${i2c_work}/corpus ${i2c_work}/seeds ${i2c_work}/crash)
  muhtari_add_test(NAME fuzz.i2c.seeds
    COMMAND muhtari-fuzz-i2c-replay --write-seeds ${i2c_work}/seeds LABELS fuzz TIMEOUT 30)
  set_tests_properties(fuzz.i2c.seeds PROPERTIES FIXTURES_SETUP fuzz_i2c_seeds)
  muhtari_add_test(NAME fuzz.i2c COMMAND muhtari-fuzz-i2c
    ${i2c_work}/corpus ${i2c_work}/seeds -max_total_time=60 -timeout=2
    -max_len=4096 -artifact_prefix=crash/ LABELS fuzz TIMEOUT 123)
  set_tests_properties(fuzz.i2c PROPERTIES
    FIXTURES_REQUIRED fuzz_i2c_seeds WORKING_DIRECTORY ${i2c_work})
  # Native seed replay: 0.03 s in three repeats, versus run replay 0.04 s.
  # The existing integer-second timeout floor leaves more than 2x headroom.
  muhtari_add_test(NAME fuzz.regress.i2c COMMAND muhtari-fuzz-i2c-replay --seeds
    LABELS fuzz TIMEOUT 1)
  # The libFuzzer binary keeps libFuzzer's main. The fuzz preset builds every
  # object with ASan, UBSan and coverage instrumentation, so the core's own
  # branches guide the fuzzer.
  add_executable(muhtari-fuzz-loader ${fuzz_dir}/loader.c
    ${CMAKE_SOURCE_DIR}/tests/cart/test_cartdb.c
    $<TARGET_OBJECTS:muhtari_objs> $<TARGET_OBJECTS:muhtari_cpu>)
  target_include_directories(muhtari-fuzz-loader PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)
  target_compile_options(muhtari-fuzz-loader PRIVATE -fsanitize=fuzzer)
  target_link_options(muhtari-fuzz-loader PRIVATE -fsanitize=fuzzer)
  set_target_properties(muhtari-fuzz-loader PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  muhtari_warnings(muhtari-fuzz-loader)

  # libFuzzer writes new inputs into its first corpus directory, so that one
  # is in the build tree and never a tracked directory; the seeds and the
  # regressions follow, read only. Crashes land in crash/ beside them.
  set(fuzz_work ${PROJECT_BINARY_DIR}/fuzz)
  file(MAKE_DIRECTORY ${fuzz_work}/corpus ${fuzz_work}/seeds ${fuzz_work}/crash)
  set(fuzz_corpora ${fuzz_work}/corpus ${fuzz_work}/seeds)
  if(EXISTS ${fuzz_regress_dir})
    list(APPEND fuzz_corpora ${fuzz_regress_dir})
  endif()

  muhtari_add_test(NAME fuzz.seeds
    COMMAND muhtari-test-images ${fuzz_work}/seeds
    LABELS fuzz TIMEOUT 30)
  set_tests_properties(fuzz.seeds PROPERTIES FIXTURES_SETUP fuzz_seeds)
  # max_len is the image cap plus one, so no seed is cut short of SuperGame
  # geometry or the oversize check. TIMEOUT 123 is twice the slowest, 61.15 s
  # in 15 runs, run 39198059321.
  muhtari_add_test(NAME fuzz.loader
    COMMAND muhtari-fuzz-loader ${fuzz_corpora}
      -max_total_time=60 -timeout=2 -max_len=1048705 -artifact_prefix=crash/
    LABELS fuzz TIMEOUT 123)
  set_tests_properties(fuzz.loader PROPERTIES
    FIXTURES_REQUIRED fuzz_seeds WORKING_DIRECTORY ${fuzz_work})
  # The same invariants over the seeds and the regressions, built with the
  # same sanitizers, without libFuzzer.
  muhtari_add_test(NAME fuzz.regress.loader
    COMMAND muhtari-fuzz-loader-replay --seeds ${fuzz_regress}
    LABELS fuzz TIMEOUT 120)

  # The run target the same way, in its own directory: its seeds are the
  # run seeds (muhtari-test-images built with run_seeds.c), max_len is the
  # cartridge's size, and -timeout=2 is the bound a frame pair must meet.
  add_executable(muhtari-fuzz-run ${fuzz_dir}/run.c)
  target_include_directories(muhtari-fuzz-run PRIVATE ${CMAKE_SOURCE_DIR}/src)
  target_link_libraries(muhtari-fuzz-run PRIVATE muhtari::muhtari)
  target_compile_options(muhtari-fuzz-run PRIVATE -fsanitize=fuzzer)
  target_link_options(muhtari-fuzz-run PRIVATE -fsanitize=fuzzer)
  set_target_properties(muhtari-fuzz-run PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  muhtari_warnings(muhtari-fuzz-run)
  add_executable(muhtari-test-run-images
    ${CMAKE_SOURCE_DIR}/tests/cart/write_images.c
    ${fuzz_dir}/run_seeds.c
    ${CMAKE_SOURCE_DIR}/tests/cart/images.c)
  target_link_libraries(muhtari-test-run-images PRIVATE muhtari::muhtari)
  set_target_properties(muhtari-test-run-images PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  muhtari_warnings(muhtari-test-run-images)

  set(fuzz_run_work ${fuzz_work}/run)
  file(MAKE_DIRECTORY ${fuzz_run_work}/corpus ${fuzz_run_work}/seeds ${fuzz_run_work}/crash)
  set(fuzz_run_corpora ${fuzz_run_work}/corpus ${fuzz_run_work}/seeds)
  if(EXISTS ${fuzz_run_regress_dir})
    list(APPEND fuzz_run_corpora ${fuzz_run_regress_dir})
  endif()
  muhtari_add_test(NAME fuzz.run.seeds
    COMMAND muhtari-test-run-images ${fuzz_run_work}/seeds
    LABELS fuzz TIMEOUT 30)
  set_tests_properties(fuzz.run.seeds PROPERTIES FIXTURES_SETUP fuzz_run_seeds)
  muhtari_add_test(NAME fuzz.run
    COMMAND muhtari-fuzz-run ${fuzz_run_corpora}
      -max_total_time=60 -timeout=2 -max_len=1048704 -artifact_prefix=crash/
    LABELS fuzz TIMEOUT 123)  # 2 x 61.13 s, fuzz job, slowest of 11 runs, run 39381240735
  set_tests_properties(fuzz.run PROPERTIES
    FIXTURES_REQUIRED fuzz_run_seeds WORKING_DIRECTORY ${fuzz_run_work})
  muhtari_add_test(NAME fuzz.regress.run
    COMMAND muhtari-fuzz-run-replay --seeds ${fuzz_run_regress}
    LABELS fuzz TIMEOUT 30)  # Measurement allowance after censored 3s instrumented run40411221452; all122 inputs retained.
  # The regression's short deadline must not compete with three active
  # libFuzzer jobs (run 40152408965); keep the complete workload and bound.
  set_tests_properties(fuzz.regress.run PROPERTIES RUN_SERIAL TRUE)
else()
  muhtari_add_test(NAME fuzz.regress.i2c COMMAND muhtari-fuzz-i2c-replay --seeds
    LABELS fuzz portable TIMEOUT 1)
  muhtari_add_test(NAME fuzz.regress.loader
    COMMAND muhtari-fuzz-loader-replay --seeds ${fuzz_regress}
    LABELS fuzz portable TIMEOUT 60)
  # Fault prevented: a replay given no inputs that passes having run nothing.
  # An emulated lane runs the guest binary under its emulator.
  set(fuzz_cmd ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:muhtari-fuzz-loader-replay>)
  string(REPLACE ";" "$<SEMICOLON>" fuzz_cmd "${fuzz_cmd}")
  muhtari_add_test(NAME fuzz.regress.loader.self_test
    COMMAND ${CMAKE_COMMAND} "-DCMD=${fuzz_cmd}" -DCODE=1
      "-DREGEX=no corpus inputs" -P ${CMAKE_SOURCE_DIR}/tests/cmake/expect_fail.cmake
    LABELS fuzz portable TIMEOUT 10)

  # The run target's seeds, an empty input among them, and its regressions.
  muhtari_add_test(NAME fuzz.regress.run
    COMMAND muhtari-fuzz-run-replay --seeds ${fuzz_run_regress}
    LABELS fuzz portable TIMEOUT 30)  # Measurement allowance after censored 3s/6s run40411221332; same workload/scales.
  # Fault prevented: the run replay passing having run nothing.
  set(fuzz_cmd ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:muhtari-fuzz-run-replay>)
  string(REPLACE ";" "$<SEMICOLON>" fuzz_cmd "${fuzz_cmd}")
  muhtari_add_test(NAME fuzz.regress.run.self_test
    COMMAND ${CMAKE_COMMAND} "-DCMD=${fuzz_cmd}" -DCODE=1
      "-DREGEX=no corpus inputs" -P ${CMAKE_SOURCE_DIR}/tests/cmake/expect_fail.cmake
    LABELS fuzz portable TIMEOUT 10)
endif()

set(i2c_fault_cmd ${CMAKE_CROSSCOMPILING_EMULATOR}
  $<TARGET_FILE:muhtari-fuzz-i2c-fault> --seeds)
string(REPLACE ";" "$<SEMICOLON>" i2c_fault_cmd "${i2c_fault_cmd}")
if(MUHTARI_FUZZ)
  set(i2c_labels fuzz)
else()
  set(i2c_labels fuzz portable)
endif()
muhtari_add_test(NAME fuzz.regress.i2c.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${i2c_fault_cmd}" -DCODE=1
    "-DREGEX=i2c invariant: isolated instance changed"
    -P ${CMAKE_SOURCE_DIR}/tests/cmake/expect_fail.cmake
  LABELS ${i2c_labels} TIMEOUT 10)
