# Property tests (tests/prop.h): one executable, one property per test. A
# pull request runs 100 cases from the fixed seed; the nightly prop job sets
# MUHTARI_PROP_CASES and MUHTARI_PROP_SEED for the same tests. The pool
# includes the built simple and eightysix samples, so these run on the lanes
# that build them.
add_executable(muhtari-test-prop ${CMAKE_CURRENT_LIST_DIR}/test_prop.c
  ${CMAKE_SOURCE_DIR}/tests/fuzz/run_seeds.c ${CMAKE_SOURCE_DIR}/runner/sha256.c)
target_include_directories(muhtari-test-prop PRIVATE ${CMAKE_SOURCE_DIR}/runner
  ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-prop PRIVATE muhtari::muhtari)
# MSVC deprecates ISO C's fopen and getenv for its _s forms.
target_compile_definitions(muhtari-test-prop PRIVATE
  $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
set_target_properties(muhtari-test-prop PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-prop)

# Fault prevented, split_run: a frame whose result depends on how the host
# splits its calls, queries the instance between them or sizes its buffer.
foreach(prop IN ITEMS split_run)
  muhtari_add_test(NAME prop.${prop} COMMAND muhtari-test-prop ${prop} ${MUHTARI_BUILT_DIR}
    LABELS portable prop TIMEOUT 60)  # provisional, phase 3
  set_tests_properties(prop.${prop} PROPERTIES FIXTURES_REQUIRED "built_simple;built_eightysix")
endforeach()
