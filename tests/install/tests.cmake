# Report each outside consumer's actual status and output because CTest's
# build-and-test wrapper can omit a child's exit diagnostic.
if(DEFINED EMBED_PROGRAM OR DEFINED CONSUMER_PROGRAM OR DEFINED QUICKSTART_PROGRAM)
  if(DEFINED EMBED_PROGRAM)
    execute_process(COMMAND ${EMULATOR} "${EMBED_PROGRAM}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
    message("embed consumer exit ${rc}; output follows:\n${out}${err}")
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "embed consumer exited ${rc}")
    endif()
  endif()
  if(DEFINED CONSUMER_PROGRAM)
    execute_process(COMMAND ${EMULATOR} "${CONSUMER_PROGRAM}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
    string(STRIP "${out}" consumer_output)
    if(NOT rc STREQUAL "0" OR NOT consumer_output STREQUAL "consumer: one frame, 800 samples")
      message(FATAL_ERROR "staged consumer: ${rc}; expected one frame, 800 samples; got ${out}${err}")
    endif()
    message("${consumer_output}")
  endif()
  if(DEFINED QUICKSTART_PROGRAM)
    execute_process(COMMAND ${EMULATOR} "${QUICKSTART_PROGRAM}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
    string(STRIP "${out}" quickstart_output)
    if(NOT rc STREQUAL "0" OR NOT quickstart_output STREQUAL "quickstart: one frame, 800 samples")
      message(FATAL_ERROR "staged quickstart: ${rc}; expected one frame, 800 samples; got ${out}${err}")
    endif()
    message("${quickstart_output}")
  endif()
  return()
endif()

# The library as an outside project consumes it.
#
# Fault prevented: an install tree, package export or header that lets the
# existing consumer work but prevents the documented quickstart from building
# and running outside the source tree.
#
# install.stage installs the library component into <build>/stage. The two
# consumer tests build tests/consumer against it with find_package and run it,
# once on the static C runtime and once on the DLL runtime (MSVC only; other
# compilers ignore the variable, so every lane registers both). The consumer
# gets this build's compiler, build type and flags, each as one argument so
# an empty value survives.
muhtari_add_test(NAME install.stage
  COMMAND ${CMAKE_COMMAND} --install ${PROJECT_BINARY_DIR}
    --component library --prefix ${PROJECT_BINARY_DIR}/stage --config $<CONFIG>
  LABELS install TIMEOUT 60)
set_tests_properties(install.stage PROPERTIES FIXTURES_SETUP install_staged)

get_property(install_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
set(install_config)
if(install_multi_config)
  set(install_config --build-config $<CONFIG>)
endif()
set(install_build_options
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
  "-DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}")
if(CMAKE_CROSSCOMPILING)
  list(APPEND install_build_options -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_STAGING_PREFIX=${PROJECT_BINARY_DIR}/stage)
endif()
string(REPLACE ";" "$<SEMICOLON>" install_emulator "${CMAKE_CROSSCOMPILING_EMULATOR}")

foreach(install_crt IN ITEMS static dll)
  if(install_crt STREQUAL "static")
    set(install_name install.consumer)
    set(install_dir consumer)
    set(install_runtime "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  else()
    set(install_name install.consumer.dll_crt)
    set(install_dir consumer_dll_crt)
    set(install_runtime "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  endif()
  set(install_program_dir "${PROJECT_BINARY_DIR}/${install_dir}")
  if(install_multi_config)
    set(install_program_dir "${install_program_dir}/$<CONFIG>")
  endif()
  muhtari_add_test(NAME ${install_name}
    COMMAND ${CMAKE_CTEST_COMMAND} --build-and-test
      ${CMAKE_CURRENT_LIST_DIR}/../consumer ${PROJECT_BINARY_DIR}/${install_dir}
      --build-generator ${CMAKE_GENERATOR}
      ${install_config}
      --build-options
        -DCMAKE_PREFIX_PATH=${PROJECT_BINARY_DIR}/stage
        -DCMAKE_MSVC_RUNTIME_LIBRARY=${install_runtime}
        ${install_build_options}
      --test-command "${CMAKE_COMMAND}"
        "-DCONSUMER_PROGRAM=${install_program_dir}/consumer${CMAKE_EXECUTABLE_SUFFIX}"
        "-DQUICKSTART_PROGRAM=${install_program_dir}/quickstart${CMAKE_EXECUTABLE_SUFFIX}"
        "-DEMULATOR=${install_emulator}"
        -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS install TIMEOUT 120)
  set_tests_properties(${install_name} PROPERTIES
    FIXTURES_REQUIRED install_staged
    PASS_REGULAR_EXPRESSION "quickstart: one frame, 800 samples")
  muhtari_add_test(NAME ${install_name}.ym
    COMMAND ${CMAKE_CTEST_COMMAND} --build-and-test
      ${CMAKE_CURRENT_LIST_DIR}/../consumer ${PROJECT_BINARY_DIR}/${install_dir}_ym
      --build-generator ${CMAKE_GENERATOR}
      ${install_config}
      --build-options
        -DCMAKE_PREFIX_PATH=${PROJECT_BINARY_DIR}/stage
        -DCMAKE_MSVC_RUNTIME_LIBRARY=${install_runtime}
        ${install_build_options}
      --test-command ${CMAKE_CROSSCOMPILING_EMULATOR} consumer --ym-fixture
        ${PROJECT_BINARY_DIR}/tests/ym2151/ym2151.a78
    LABELS install TIMEOUT 120)
  set_tests_properties(${install_name}.ym PROPERTIES
    FIXTURES_REQUIRED "install_staged;ym_carts"
    PASS_REGULAR_EXPRESSION "consumer: YM NTSC and PAL")
endforeach()

# A C project embeds the source tree with add_subdirectory, gets only the
# library and its alias, and runs the consumer's frame with it.
set(install_embed_program ${PROJECT_BINARY_DIR}/embed/embed${CMAKE_EXECUTABLE_SUFFIX})
if(install_multi_config)
  set(install_embed_program ${PROJECT_BINARY_DIR}/embed/$<CONFIG>/embed${CMAKE_EXECUTABLE_SUFFIX})
endif()
muhtari_add_test(NAME embed.subdirectory
  COMMAND ${CMAKE_CTEST_COMMAND} --build-and-test
    ${CMAKE_CURRENT_LIST_DIR}/../embed ${PROJECT_BINARY_DIR}/embed
    --build-generator ${CMAKE_GENERATOR}
    ${install_config}
    --build-options
      -DMUHTARI_SOURCE_DIR=${PROJECT_SOURCE_DIR}
      ${install_build_options}
    --test-command ${CMAKE_COMMAND}
      "-DEMBED_PROGRAM=${install_embed_program}"
      "-DEMULATOR=${CMAKE_CROSSCOMPILING_EMULATOR}" -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS install TIMEOUT 120)
set_tests_properties(embed.subdirectory PROPERTIES
  PASS_REGULAR_EXPRESSION "consumer: one frame, 800 samples")
