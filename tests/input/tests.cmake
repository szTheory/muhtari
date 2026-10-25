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
  # 2 x 0.20 s, macOS x64, slowest of 18 runs, run 39382016958
  muhtari_add_test(NAME input.${input_case} COMMAND muhtari-test-input ${input_case}
    LABELS input portable TIMEOUT 1)
endforeach()
# Provisional ceil(2 x 2.1942 s), complete NTSC/PAL/prefix/wrap workload
# co-scheduled with all ci tests, Apple Clang 21 arm64, 2026-10-25 local.
# The older 0.97 s calibration preceded the doubled raster/wrap matrix.
# All scenarios and normal lane scaling remain; hosted calibration is pending.
muhtari_add_test(NAME input.lightgun COMMAND muhtari-test-input lightgun
  LABELS input portable TIMEOUT 5)
foreach(input_case IN ITEMS pins protocol wrap recovery)
  muhtari_add_test(NAME input.savekey.${input_case}
    COMMAND muhtari-test-input savekey.${input_case} LABELS input portable TIMEOUT 10)
endforeach()

# The cartridge as files, for the runner's movie tests (tests/runner): the
# program reading every frame, and the variant reading only on even ones.
set(input_cart_dir ${PROJECT_BINARY_DIR}/tests/input-carts)
file(MAKE_DIRECTORY ${input_cart_dir})
# 2 x 0.14 s, Windows arm64, slowest of 17 runs, run 39368351340
muhtari_add_test(NAME input.cart_files
  COMMAND muhtari-test-input --write-cart ${input_cart_dir}/input.a78
    --write-cart ${input_cart_dir}/input-even.a78 even
  LABELS input portable TIMEOUT 1)
set_tests_properties(input.cart_files PROPERTIES FIXTURES_SETUP input_carts)
