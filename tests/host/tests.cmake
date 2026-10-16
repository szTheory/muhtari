# Host helpers: code the runner and the libretro adapter share outside the core.
add_executable(muhtari-test-palette ${CMAKE_CURRENT_LIST_DIR}/test_palette.c)
target_link_libraries(muhtari-test-palette PRIVATE muhtari_host)
set_target_properties(muhtari-test-palette PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-palette)
muhtari_add_test(NAME host.palette COMMAND muhtari-test-palette
  LABELS host portable TIMEOUT 10)
