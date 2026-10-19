# Samples built from pinned tools (ASSET_POLICY.md section 3): 7800basic's
# simple sample, built by our own recipe and checked against expected.txt.
# built.samples writes build/<preset>/built/<name>/ and is the fixture the
# probe tests read. Configure stops when a tool is missing, so no lane that
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
set(built_args -DWASMTIME=${MUHTARI_WASMTIME} -DPKG=${MUHTARI_7800BASIC} -DNAME=simple)
muhtari_add_test(NAME built.samples
  COMMAND ${CMAKE_COMMAND} ${built_args} -DOUT=${MUHTARI_BUILT_DIR}
    -DEXPECTED=${built_expected} -P ${CMAKE_CURRENT_LIST_DIR}/build.cmake
  LABELS built portable TIMEOUT 60)
set_tests_properties(built.samples PROPERTIES FIXTURES_SETUP built_samples)

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
  LABELS built portable TIMEOUT 60)
