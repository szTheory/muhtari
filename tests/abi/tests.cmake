# Binary checks on the core library: its undefined symbols, its writable data
# and its public names, each run by tests/cmake/abi.cmake on the built
# archive. The two fixtures are negative controls built with the core's own
# flags, so each check fails on every lane where it runs.
set(abi_scripts ${CMAKE_CURRENT_LIST_DIR}/../cmake)
# MSVC sets no CMAKE_NM; llvm-nm reads its COFF archives.
set(abi_nm ${CMAKE_NM})
if(NOT abi_nm)
  find_program(MUHTARI_NM NAMES llvm-nm nm REQUIRED)
  set(abi_nm ${MUHTARI_NM})
endif()
set(abi_args -DNM=${abi_nm} "-DSYSTEM=${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}")

get_target_property(abi_options muhtari COMPILE_OPTIONS)
foreach(fixture IN ITEMS libc global)
  add_library(muhtari-abi-${fixture} STATIC ${CMAKE_CURRENT_LIST_DIR}/${fixture}_fixture.c)
  target_compile_options(muhtari-abi-${fixture} PRIVATE ${abi_options})
  set_target_properties(muhtari-abi-${fixture} PROPERTIES
    C_VISIBILITY_PRESET hidden
    POSITION_INDEPENDENT_CODE ON
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests/abi)
  muhtari_warnings(muhtari-abi-${fixture})
endforeach()

foreach(mode IN ITEMS undefined globals symbols)
  muhtari_add_test(NAME abi.${mode}
    COMMAND ${CMAKE_COMMAND} -DMODE=${mode} -DLIB=$<TARGET_FILE:muhtari> ${abi_args}
      -DHEADER=${PROJECT_SOURCE_DIR}/include/muhtari.h -P ${abi_scripts}/abi.cmake
    LABELS abi TIMEOUT 30)
endforeach()

# CMake wraps a long message at a column that depends on the build path, so
# the regexes allow a line break anywhere in the finding's title.
foreach(case IN ITEMS "undefined;libc;unexpected[^:]*symbol:.*abort"
                      "globals;global;writable[^:]*symbol:.*counter")
  list(GET case 0 mode)
  list(GET case 1 fixture)
  list(GET case 2 regex)
  set(abi_cmd ${CMAKE_COMMAND} -DMODE=${mode} -DLIB=$<TARGET_FILE:muhtari-abi-${fixture}>
    ${abi_args} -P ${abi_scripts}/abi.cmake)
  string(REPLACE ";" "$<SEMICOLON>" abi_cmd "${abi_cmd}")
  muhtari_add_test(NAME abi.${mode}.self_test
    COMMAND ${CMAKE_COMMAND} "-DCMD=${abi_cmd}" -DCODE=1 "-DREGEX=${regex}"
      -P ${abi_scripts}/expect_fail.cmake
    LABELS abi TIMEOUT 30)
endforeach()

# Every header name is in a versioned API test. The self test reads a copy of
# the header with one extra field and expects that field named.
muhtari_add_test(NAME abi.api_version
  COMMAND ${CMAKE_COMMAND} -DHEADER=${PROJECT_SOURCE_DIR}/include/muhtari.h
    -DAPI_DIR=${PROJECT_SOURCE_DIR}/tests/api -P ${abi_scripts}/api_version.cmake
  LABELS abi TIMEOUT 10)
file(READ ${PROJECT_SOURCE_DIR}/include/muhtari.h abi_header)
string(REPLACE "uint32_t max_audio_samples;"
  "uint32_t max_audio_samples;\n    uint32_t untested_field;" abi_header "${abi_header}")
file(WRITE ${PROJECT_BINARY_DIR}/tests/abi/extra_field.h "${abi_header}")
set(abi_cmd ${CMAKE_COMMAND} -DHEADER=${PROJECT_BINARY_DIR}/tests/abi/extra_field.h
  -DAPI_DIR=${PROJECT_SOURCE_DIR}/tests/api -P ${abi_scripts}/api_version.cmake)
string(REPLACE ";" "$<SEMICOLON>" abi_cmd "${abi_cmd}")
muhtari_add_test(NAME abi.api_version.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${abi_cmd}" -DCODE=1
    "-DREGEX=in no tests/api/vK.c with K <= 1: untested_field" -P ${abi_scripts}/expect_fail.cmake
  LABELS abi TIMEOUT 10)
