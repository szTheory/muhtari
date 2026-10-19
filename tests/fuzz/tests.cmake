# The loader fuzz target's replay. muhtari-fuzz-loader-replay runs
# tests/fuzz/loader.c's invariants over the in-memory seeds from seeds.c and
# every committed regression input, on every platform; libFuzzer itself runs
# in the fuzz preset.
set(fuzz_dir ${CMAKE_CURRENT_LIST_DIR})
set(fuzz_regress_dir ${fuzz_dir}/loader/regress)

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
