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
foreach(input_case IN ITEMS directions proline_two_button proline_one_button joystick_fire
    ctlswb_no_swchb_write tia_en_disables_two_button switches unplugged pa7_edge
    lag_even_frames lag_dma_and_peek)
  # provisional, phase 3
  muhtari_add_test(NAME input.${input_case} COMMAND muhtari-test-input ${input_case}
    LABELS input portable TIMEOUT 10)
endforeach()

# The cartridge as files, for the runner's movie tests (tests/runner): the
# program reading every frame, and the variant reading only on even ones.
set(input_cart_dir ${PROJECT_BINARY_DIR}/tests/input-carts)
file(MAKE_DIRECTORY ${input_cart_dir})
# provisional, phase 3
muhtari_add_test(NAME input.cart_files
  COMMAND muhtari-test-input --write-cart ${input_cart_dir}/input.a78
    --write-cart ${input_cart_dir}/input-even.a78 even
  LABELS input portable TIMEOUT 10)
set_tests_properties(input.cart_files PROPERTIES FIXTURES_SETUP input_carts)
