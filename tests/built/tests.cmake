# Samples built from pinned tools (ASSET_POLICY.md section 3): 7800basic's
# samples, built by our own recipe and checked against expected.txt, and
# their 600-frame chains (frames.cmake). built.<name> writes
# build/<preset>/built/<name>/. Configure stops when a tool is missing, so no lane that
# lists these tests can skip them.
set(MUHTARI_BUILT_PINS ${CMAKE_SOURCE_DIR}/build/pins)
find_program(MUHTARI_WASMTIME wasmtime PATHS ${MUHTARI_BUILT_PINS}/wasmtime NO_DEFAULT_PATH)
set(MUHTARI_7800BASIC ${MUHTARI_BUILT_PINS}/7800basic)
if(NOT MUHTARI_WASMTIME OR NOT EXISTS ${MUHTARI_7800BASIC}/7800basic.wasm)
  message(FATAL_ERROR "built: the sample needs wasmtime in ${MUHTARI_BUILT_PINS}/wasmtime and "
    "the 7800basic package in ${MUHTARI_7800BASIC}; fetch both with\n"
    "  scripts/fetch-pin.sh wasmtime-<arch>-<os> build/pins/wasmtime\n"
    "  scripts/fetch-pin.sh 7800basic-wasm build/pins/7800basic")
endif()

set(MUHTARI_BUILT_DIR ${PROJECT_BINARY_DIR}/built)
set(built_expected ${CMAKE_CURRENT_LIST_DIR}/expected.txt)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${built_expected})
set(built_tools -DWASMTIME=${MUHTARI_WASMTIME} -DPKG=${MUHTARI_7800BASIC})
set(built_args ${built_tools} -DNAME=simple)

# muhtari-test-chain: the 600-frame SHA-256 chain of a fast-booted ROM, the
# value frames.cmake compares with a row's frame column.
add_executable(muhtari-test-chain ${CMAKE_CURRENT_LIST_DIR}/chain.c
  ${CMAKE_SOURCE_DIR}/runner/sha256.c)
target_include_directories(muhtari-test-chain PRIVATE ${CMAKE_SOURCE_DIR}/runner)
target_link_libraries(muhtari-test-chain PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-chain PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-chain)
string(REPLACE ";" "$<SEMICOLON>" built_emulator "${CMAKE_CROSSCOMPILING_EMULATOR}")

# One built.<name> and one basic.<name> per row. built.simple also sets up
# built_samples, the fixture the cart, runner and frame tests read.
file(STRINGS ${built_expected} built_rows REGEX "^[^#]")
foreach(built_row IN LISTS built_rows)
  string(REGEX REPLACE "[ \t]+" ";" built_row "${built_row}")
  list(GET built_row 0 name)
  list(GET built_row 2 source)
  get_filename_component(bas "${source}" NAME)
  if(name STREQUAL "simple")
    set(fixtures "built_simple;built_samples")
    set(built_timeout 5)  # 2 x 2.44 s, macOS x64, slowest of 13 runs, run 39195320937
  else()
    set(fixtures built_${name})
    set(built_timeout 10)  # provisional, phase 3
  endif()
  muhtari_add_test(NAME built.${name}
    COMMAND ${CMAKE_COMMAND} ${built_tools} -DNAME=${name} -DOUT=${MUHTARI_BUILT_DIR}
      -DEXPECTED=${built_expected} -P ${CMAKE_CURRENT_LIST_DIR}/build.cmake
    LABELS built portable TIMEOUT ${built_timeout})
  set_tests_properties(built.${name} PROPERTIES FIXTURES_SETUP "${fixtures}")
  muhtari_add_test(NAME basic.${name}
    COMMAND ${CMAKE_COMMAND} -DCHAIN=$<TARGET_FILE:muhtari-test-chain>
      "-DEMULATOR=${built_emulator}" -DROM=${MUHTARI_BUILT_DIR}/${name}/${bas}.a78
      -DNAME=${name} -DEXPECTED=${built_expected} -P ${CMAKE_CURRENT_LIST_DIR}/frames.cmake
    LABELS basic portable TIMEOUT 30)  # provisional, phase 3
  set_tests_properties(basic.${name} PROPERTIES FIXTURES_REQUIRED built_${name})
endforeach()

# Negative control for the revision guard: a copy of expected.txt that says
# revision 0 must fail by name. The call is a generated script, so the
# emulator list reaches frames.cmake without a second level of escaping.
file(READ ${built_expected} built_text)
string(REGEX REPLACE "# behaviour_revision [0-9]+" "# behaviour_revision 0" built_text "${built_text}")
set(built_rev0 ${PROJECT_BINARY_DIR}/tests/built/expected_rev0.txt)
file(WRITE ${built_rev0} "${built_text}")
set(built_rev0_script ${PROJECT_BINARY_DIR}/tests/built/revision_self_test.cmake)
file(GENERATE OUTPUT ${built_rev0_script} CONTENT
"set(CHAIN [==[$<TARGET_FILE:muhtari-test-chain>]==])
set(EMULATOR [==[${CMAKE_CROSSCOMPILING_EMULATOR}]==])
set(ROM [==[${MUHTARI_BUILT_DIR}/simple/simple.bas.a78]==])
set(NAME simple)
set(EXPECTED [==[${built_rev0}]==])
include([==[${CMAKE_CURRENT_LIST_DIR}/frames.cmake]==])
")
muhtari_add_test(NAME basic.revision.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${CMAKE_COMMAND}$<SEMICOLON>-P$<SEMICOLON>${built_rev0_script}"
    -DCODE=1 "-DREGEX=behaviour revision" -P ${PROJECT_SOURCE_DIR}/tests/cmake/expect_fail.cmake
  LABELS basic portable TIMEOUT 30)  # provisional, phase 3
set_tests_properties(basic.revision.self_test PROPERTIES FIXTURES_REQUIRED built_simple)

# Negative control: one digit of the .a78 digest changed must fail by name.
file(READ ${built_expected} built_text)
string(REPLACE "d09da3ed27fe82e0" "d09da3ed27fe82e1" built_text "${built_text}")
set(built_wrong ${PROJECT_BINARY_DIR}/tests/built/expected_wrong.txt)
file(WRITE ${built_wrong} "${built_text}")
set(built_cmd ${CMAKE_COMMAND} ${built_args} -DOUT=${PROJECT_BINARY_DIR}/tests/built
  -DEXPECTED=${built_wrong} -P ${CMAKE_CURRENT_LIST_DIR}/build.cmake)
string(REPLACE ";" "$<SEMICOLON>" built_cmd "${built_cmd}")
muhtari_add_test(NAME built.samples.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${built_cmd}" -DCODE=1
    "-DREGEX=simple: .a78 SHA-256 drift" -P ${PROJECT_SOURCE_DIR}/tests/cmake/expect_fail.cmake
  LABELS built portable TIMEOUT 4)  # 2 x 1.68 s, Windows arm64, slowest of 13 runs, run 39197737154

# The simple sample through the internal load and the fast-boot hand-off:
# 120 frames, two instances in step, and frame 120 a real picture
# (tests/frame/test_frame.c).
muhtari_add_test(NAME frame.sample_renders
  COMMAND muhtari-test-frame sample_renders ${MUHTARI_BUILT_DIR}/simple/simple.bas.a78
  LABELS built portable TIMEOUT 20)  # provisional, phase 3
set_tests_properties(frame.sample_renders PROPERTIES FIXTURES_REQUIRED built_samples)
