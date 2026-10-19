# The loader fuzz target. Two configurations include this file and register
# disjoint sets: the ci and wasm32 lanes run the replay on every platform,
# and the fuzz preset (MUHTARI_FUZZ, clang-21) runs libFuzzer itself.
set(fuzz_dir ${CMAKE_CURRENT_LIST_DIR})
set(fuzz_regress_dir ${fuzz_dir}/loader/regress)

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
  ${CMAKE_SOURCE_DIR}/tests/cart/images.c)
target_compile_definitions(muhtari-fuzz-loader-replay PRIVATE MUHTARI_FUZZ_REPLAY)
target_include_directories(muhtari-fuzz-loader-replay PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-fuzz-loader-replay PRIVATE muhtari::muhtari)
set_target_properties(muhtari-fuzz-loader-replay PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-fuzz-loader-replay)

# Committed regressions, each an input that once broke an invariant. The
# directory appears with the first one; the replay takes files, not
# directories, so they are listed here.
set(fuzz_regress "")
if(EXISTS ${fuzz_regress_dir})
  file(GLOB fuzz_regress CONFIGURE_DEPENDS ${fuzz_regress_dir}/*)
  list(SORT fuzz_regress)
endif()

if(MUHTARI_FUZZ)
  # The libFuzzer binary keeps libFuzzer's main. The fuzz preset builds every
  # object with ASan, UBSan and coverage instrumentation, so the core's own
  # branches guide the fuzzer.
  add_executable(muhtari-fuzz-loader ${fuzz_dir}/loader.c)
  target_include_directories(muhtari-fuzz-loader PRIVATE ${CMAKE_SOURCE_DIR}/src)
  target_link_libraries(muhtari-fuzz-loader PRIVATE muhtari::muhtari)
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
else()
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
endif()
