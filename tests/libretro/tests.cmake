# The libretro core, called in-process as a frontend calls it. Registered only
# when the core is built.
if(TARGET muhtari_libretro)
  add_executable(muhtari-test-libretro-host ${CMAKE_CURRENT_LIST_DIR}/host.c)
  target_include_directories(muhtari-test-libretro-host PRIVATE ${PROJECT_SOURCE_DIR}/libretro)
  target_link_libraries(muhtari-test-libretro-host PRIVATE
    muhtari::muhtari muhtari_host ${CMAKE_DL_LIBS})
  set_target_properties(muhtari-test-libretro-host PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  muhtari_warnings(muhtari-test-libretro-host)
  # Fault prevented: the core RetroArch loads shows a frame, timing or
  # geometry other than the library's, or needs content to start.
  muhtari_add_test(NAME libretro.host.xrgb8888
    COMMAND muhtari-test-libretro-host $<TARGET_FILE:muhtari_libretro> xrgb8888
    LABELS libretro portable TIMEOUT 30)
endif()
