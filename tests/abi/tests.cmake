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

# The frozen API v1 program: every function, field and constant of version 1.
add_executable(muhtari-test-api-v1 ${CMAKE_SOURCE_DIR}/tests/api/v1.c)
target_link_libraries(muhtari-test-api-v1 PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-api-v1 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-api-v1)
muhtari_add_test(NAME abi.api_v1 COMMAND muhtari-test-api-v1
  LABELS abi portable TIMEOUT 10)

# The public header compiles alone as C17 and as C++17 with warnings fatal.
# Fault prevented: a header that only the core's own build, with its own
# flags and language, can compile. The targets are built only by these
# tests, which share the build tree and so take turns.
add_library(muhtari_header_c17 OBJECT EXCLUDE_FROM_ALL
  ${CMAKE_CURRENT_LIST_DIR}/../header/header.c)
add_library(muhtari_header_cxx17 OBJECT EXCLUDE_FROM_ALL
  ${CMAKE_CURRENT_LIST_DIR}/../header/header.cpp)
set_target_properties(muhtari_header_cxx17 PROPERTIES
  CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
muhtari_warnings(muhtari_header_c17)
foreach(abi_target IN ITEMS muhtari_header_c17 muhtari_header_cxx17)
  target_include_directories(${abi_target} PRIVATE ${PROJECT_SOURCE_DIR}/include)
  target_compile_options(${abi_target} PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<COMPILE_LANG_AND_ID:C,MSVC>:/W4 /WX /TC>
    $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX /TP>)
endforeach()
foreach(abi_lang IN ITEMS c17 cxx17)
  muhtari_add_test(NAME abi.header.${abi_lang}
    COMMAND ${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR}
      --target muhtari_header_${abi_lang} --config $<CONFIG>
    LABELS abi TIMEOUT 120)
  set_tests_properties(abi.header.${abi_lang} PROPERTIES RESOURCE_LOCK build_tree)
endforeach()

# On the nofp lane, a fixture holding one double division must fail to build
# under -mgeneral-regs-only with GCC's own diagnostic (AArch64 names the
# option; x86-64 refuses the SSE register a double travels in). Fault
# prevented: a flag that stopped rejecting floating point, so the lane passes
# on a core that uses it. The fixture is built only inside this test.
if(MUHTARI_NOFP)
  set(abi_cmd ${CMAKE_C_COMPILER} -std=c17 -mgeneral-regs-only
    -c ${CMAKE_CURRENT_LIST_DIR}/nofp_fixture.c -o ${PROJECT_BINARY_DIR}/tests/abi/nofp_fixture.o)
  string(REPLACE ";" "$<SEMICOLON>" abi_cmd "${abi_cmd}")
  muhtari_add_test(NAME abi.nofp_fixture
    COMMAND ${CMAKE_COMMAND} "-DCMD=${abi_cmd}" -DCODE=1
      "-DREGEX=general-regs-only[^ ]* is incompatible with the use of floating-point.types|SSE register (return|argument) with SSE disabled"
      -P ${abi_scripts}/expect_fail.cmake
    LABELS abi TIMEOUT 30)
endif()
