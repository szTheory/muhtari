# The library as an outside project consumes it.
#
# Fault prevented: an install tree, package file or header that only works
# inside this build, so a program built against a release archive fails to
# configure, compile or link.
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
  muhtari_add_test(NAME ${install_name}
    COMMAND ${CMAKE_CTEST_COMMAND} --build-and-test
      ${CMAKE_CURRENT_LIST_DIR}/../consumer ${PROJECT_BINARY_DIR}/${install_dir}
      --build-generator ${CMAKE_GENERATOR}
      ${install_config}
      --build-options
        -DCMAKE_PREFIX_PATH=${PROJECT_BINARY_DIR}/stage
        -DCMAKE_MSVC_RUNTIME_LIBRARY=${install_runtime}
        ${install_build_options}
      --test-command consumer
    LABELS install TIMEOUT 120)
  set_tests_properties(${install_name} PROPERTIES
    FIXTURES_REQUIRED install_staged
    PASS_REGULAR_EXPRESSION "consumer: one frame, 800 samples")
endforeach()

# A C project embeds the source tree with add_subdirectory, gets only the
# library and its alias, and runs the consumer's frame with it.
muhtari_add_test(NAME embed.subdirectory
  COMMAND ${CMAKE_CTEST_COMMAND} --build-and-test
    ${CMAKE_CURRENT_LIST_DIR}/../embed ${PROJECT_BINARY_DIR}/embed
    --build-generator ${CMAKE_GENERATOR}
    ${install_config}
    --build-options
      -DMUHTARI_SOURCE_DIR=${PROJECT_SOURCE_DIR}
      ${install_build_options}
    --test-command embed
  LABELS install TIMEOUT 120)
set_tests_properties(embed.subdirectory PROPERTIES
  PASS_REGULAR_EXPRESSION "consumer: one frame, 800 samples")

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
foreach(install_target IN ITEMS muhtari_header_c17 muhtari_header_cxx17)
  target_include_directories(${install_target} PRIVATE ${PROJECT_SOURCE_DIR}/include)
  target_compile_options(${install_target} PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<COMPILE_LANG_AND_ID:C,MSVC>:/W4 /WX /TC>
    $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX /TP>)
endforeach()
foreach(install_lang IN ITEMS c17 cxx17)
  muhtari_add_test(NAME abi.header.${install_lang}
    COMMAND ${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR}
      --target muhtari_header_${install_lang} --config $<CONFIG>
    LABELS abi TIMEOUT 120)
  set_tests_properties(abi.header.${install_lang} PROPERTIES RESOURCE_LOCK build_tree)
endforeach()
