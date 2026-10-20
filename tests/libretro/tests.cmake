# The libretro core: its vendored header, its .info claims, and the core
# called in-process as a frontend calls it.
set(libretro_scripts ${CMAKE_CURRENT_LIST_DIR}/../cmake)

muhtari_add_test(NAME libretro.vendored
  COMMAND ${CMAKE_COMMAND} -P ${libretro_scripts}/vendored.cmake
  LABELS libretro TIMEOUT 10)

# The rest needs the built core.
if(TARGET muhtari_libretro)
  add_executable(muhtari-test-libretro-host ${CMAKE_CURRENT_LIST_DIR}/host.c
    ${PROJECT_SOURCE_DIR}/runner/sha256.c)
  target_include_directories(muhtari-test-libretro-host PRIVATE ${PROJECT_SOURCE_DIR}/libretro
    ${PROJECT_SOURCE_DIR}/runner)
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
  muhtari_add_test(NAME libretro.host.rgb565
    COMMAND muhtari-test-libretro-host $<TARGET_FILE:muhtari_libretro_rgb565> rgb565
    LABELS libretro portable TIMEOUT 30)
  # Fault prevented: a RetroPad press that never reaches the register the
  # cartridge reads, or content the core cannot load from memory.
  set(libretro_input_cart ${PROJECT_BINARY_DIR}/tests/input-carts/input.a78)
  muhtari_add_test(NAME libretro.host.input
    COMMAND muhtari-test-libretro-host $<TARGET_FILE:muhtari_libretro> input ${libretro_input_cart}
    LABELS libretro portable TIMEOUT 30)  # provisional, phase 3
  set_tests_properties(libretro.host.input PROPERTIES FIXTURES_REQUIRED input_carts)
  # Fault prevented: a control, device, option or Restart that behaves
  # otherwise than the RetroPad layout and options docs/libretro.md lists.
  foreach(libretro_case IN ITEMS layout devices reset options)
    muhtari_add_test(NAME libretro.host.${libretro_case}
      COMMAND muhtari-test-libretro-host $<TARGET_FILE:muhtari_libretro> ${libretro_case}
        ${libretro_input_cart}
      LABELS libretro portable TIMEOUT 30)  # provisional, phase 3
    set_tests_properties(libretro.host.${libretro_case} PROPERTIES FIXTURES_REQUIRED input_carts)
  endforeach()
  # Fault prevented: a BIOS file in the system directory that is ignored,
  # unlogged, or missing without a fast-boot fallback. The case writes the
  # file into its own directory under both names and removes it again.
  set(libretro_system ${PROJECT_BINARY_DIR}/tests/libretro-system)
  file(MAKE_DIRECTORY ${libretro_system})
  muhtari_add_test(NAME libretro.host.bios
    COMMAND muhtari-test-libretro-host $<TARGET_FILE:muhtari_libretro> bios
      ${MUHTARI_BUILT_DIR}/simple/simple.bas.a78 ${MUHTARI_BUILT_DIR}/openbios/openbios.bin
      ${libretro_system}
    LABELS libretro portable TIMEOUT 30)  # provisional, phase 3
  set_tests_properties(libretro.host.bios PROPERTIES
    FIXTURES_REQUIRED "built_simple;built_openbios")
  # Fault prevented: an achievement map that points rcheevos at the wrong
  # bytes, a register, or a banked window.
  muhtari_add_test(NAME libretro.host.memory_map
    COMMAND muhtari-test-libretro-host $<TARGET_FILE:muhtari_libretro> memory_map
      ${libretro_input_cart} ${MUHTARI_BUILT_DIR}/doublebuffer/doublebuffer.bas.a78
      ${PROJECT_SOURCE_DIR}/tests/diag/sg144.a78
    LABELS libretro portable TIMEOUT 30)  # provisional, phase 3
  set_tests_properties(libretro.host.memory_map PROPERTIES
    FIXTURES_REQUIRED "input_carts;built_doublebuffer")

  set(libretro_inventory ${PROJECT_SOURCE_DIR}/tests/inventory/${MUHTARI_INVENTORY}.txt)
  muhtari_add_test(NAME libretro.info_claims
    COMMAND ${CMAKE_COMMAND} -DINFO=${PROJECT_BINARY_DIR}/libretro/muhtari_libretro.info
      -DINVENTORY=${libretro_inventory} -P ${libretro_scripts}/info_claims.cmake
    LABELS libretro TIMEOUT 10)
  # The self test checks a .info whose "true" claim maps to no test and
  # expects that claim named.
  set(libretro_fixture ${PROJECT_BINARY_DIR}/libretro/unmapped_claim.info)
  file(WRITE ${libretro_fixture} "supports_no_game = \"true\"\nsavestate = \"true\"\n")
  set(libretro_cmd ${CMAKE_COMMAND} -DINFO=${libretro_fixture}
    -DINVENTORY=${libretro_inventory} -P ${libretro_scripts}/info_claims.cmake)
  string(REPLACE ";" "$<SEMICOLON>" libretro_cmd "${libretro_cmd}")
  muhtari_add_test(NAME libretro.info_claims.self_test
    COMMAND ${CMAKE_COMMAND} "-DCMD=${libretro_cmd}" -DCODE=1
      "-DREGEX=savestate = .true. maps to no test" -P ${libretro_scripts}/expect_fail.cmake
    LABELS libretro TIMEOUT 10)
endif()
