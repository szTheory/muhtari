# Controllers and console switches as a game reads them. The program links
# muhtari and includes src/internal.h: it builds a cartridge in memory that
# reads every input register once a frame, drives it with rows written in
# muhtari-movie 1 and read by the runner's own runner/movie.c, and checks
# what the cartridge read (docs/hardware/controllers.md).
add_executable(muhtari-test-input
  ${CMAKE_CURRENT_LIST_DIR}/test_input.c
  ${CMAKE_SOURCE_DIR}/tests/cart/images.c
  ${CMAKE_SOURCE_DIR}/runner/movie.c)
target_include_directories(muhtari-test-input PRIVATE ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/runner)
target_link_libraries(muhtari-test-input PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-input PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-input)

# One case per argv.
foreach(input_case IN ITEMS directions)
  # provisional, phase 3
  muhtari_add_test(NAME input.${input_case} COMMAND muhtari-test-input ${input_case}
    LABELS input portable TIMEOUT 10)
endforeach()
