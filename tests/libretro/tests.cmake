# The libretro core: its vendored header, its .info claims, and the core
# called in-process as a frontend calls it.
if(LIBRETRO_RETAIL_RUN OR LIBRETRO_EXPANSION_RUN)
  function(retail_command)
    execute_process(COMMAND ${EMULATOR} ${ARGV} RESULT_VARIABLE rc
      OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT ${HOST_TIMEOUT})
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "retail host command ${ARGV}: ${rc}\n${out}\n${err}")
    endif()
    message("${out}")
  endfunction()
  if(LIBRETRO_EXPANSION_RUN)
    retail_command("${RESOLVER}" --expansion)
    return()
  endif()
  file(MAKE_DIRECTORY "${OUT}")
  file(MAKE_DIRECTORY "${OUT}/hsc")
  retail_command("${RESOLVER}" --hsc-dir "${OUT}/hsc" ${CARTS})
  retail_command("${RESOLVER}" --pair "${ROOT}/tests/diag/activision.a78" "${MODULE}" "${OUT}")
  foreach(extension IN ITEMS a78 bin)
    foreach(layout IN ITEMS om am)
      set(image "${OUT}/activision.${layout}.${extension}")
      # Numeric expectations remain owned by the existing diagnostic harness.
      retail_command("${DRIVER}" "${image}" "${EXPECTED}")
      retail_command("${DRIVER}" --libretro "${MODULE}" "${image}" "${EXPECTED}")
      execute_process(COMMAND ${EMULATOR} "${RUNNER}" --probe "${image}"
        RESULT_VARIABLE rc OUTPUT_VARIABLE probe ERROR_VARIABLE err TIMEOUT 10)
      if(NOT rc STREQUAL "0" OR NOT probe MATCHES "\nsource hash\nboard activision\n" OR
          (layout STREQUAL "am" AND NOT probe MATCHES "\nflag activision_half_swapped\n") OR
          (layout STREQUAL "om" AND probe MATCHES "activision_half_swapped"))
        message(FATAL_ERROR "identified Activision input misreported: ${rc}\n${probe}${err}")
      endif()
      foreach(suffix IN ITEMS "" "_headerless")
        if(suffix STREQUAL "" OR extension STREQUAL "bin")
          set(hashed "${image}")
        else()
          set(hashed "${OUT}/activision.${layout}.bin")
        endif()
        foreach(alg IN ITEMS SHA1 SHA256 MD5)
          file(${alg} "${hashed}" digest)
          string(TOLOWER "${alg}${suffix}" key)
          if(NOT probe MATCHES "(^|\n)${key} ${digest}\n")
            message(FATAL_ERROR "raw ${key} identity changed for ${image}")
          endif()
        endforeach()
      endforeach()
      execute_process(COMMAND ${EMULATOR} "${RUNNER}" --frames 600 --hash-frame 600
        --hash-audio --diagnostic "${image}" RESULT_VARIABLE rc
        OUTPUT_VARIABLE run ERROR_VARIABLE err TIMEOUT 90)
      string(REGEX MATCH "diagnostic [0-9a-f]+\n" report "${run}")
      if(NOT rc STREQUAL "0" OR NOT run MATCHES "^frame 600 sha256 [0-9a-f]+\n" OR
          NOT run MATCHES "\naudio_transitions_sha256 [0-9a-f]+\n" OR
          NOT run MATCHES "\naudio_pcm_sha256 [0-9a-f]+\n" OR report STREQUAL "")
        message(FATAL_ERROR "missing frame/raw/PCM/mailbox observation: ${rc}\n${run}${err}")
      endif()
      if(NOT DEFINED reference)
        set(reference "${run}")
      elseif(NOT run STREQUAL reference)
        message(FATAL_ERROR "AM/OM runner mailbox/frame/raw/PCM observations differ")
      endif()
      file(CONFIGURE OUTPUT "${OUT}/report.txt" CONTENT "${report}" @ONLY NEWLINE_STYLE UNIX)
      retail_command("${DRIVER}" --mailbox "${OUT}/report.txt" activision "${EXPECTED}")
    endforeach()
  endforeach()
  # The same real numeric parser must reject a report shortened by one byte.
  string(LENGTH "${report}" length)
  math(EXPR short_length "${length} - 1")
  string(SUBSTRING "${report}" 0 ${short_length} short)
  file(WRITE "${OUT}/truncated-report.txt" "${short}")
  execute_process(COMMAND ${EMULATOR} "${DRIVER}" --mailbox "${OUT}/truncated-report.txt"
    activision "${EXPECTED}" RESULT_VARIABLE rc TIMEOUT 10)
  if(NOT rc STREQUAL "1")
    message(FATAL_ERROR "truncated identified Activision report was accepted: ${rc}")
  endif()
  message("Identified AM/OM: library, actual runner and loaded testdb module agree; raw identities remain distinct")
  return()
endif()
set(libretro_scripts ${CMAKE_CURRENT_LIST_DIR}/../cmake)


muhtari_add_test(NAME libretro.vendored
  COMMAND ${CMAKE_COMMAND} -P ${libretro_scripts}/vendored.cmake
  LABELS libretro TIMEOUT 10)

# The rest needs the built core.
if(TARGET muhtari_libretro)
  if(TARGET muhtari_libretro_input_trace)
    foreach(trace_target IN ITEMS protocol host helper)
      if(trace_target STREQUAL "protocol")
        set(trace_source ${CMAKE_CURRENT_LIST_DIR}/test_input_trace.c)
        set(trace_extra "")
      elseif(trace_target STREQUAL "host")
        set(trace_source ${CMAKE_CURRENT_LIST_DIR}/input_trace_host.c)
        set(trace_extra ${CMAKE_CURRENT_LIST_DIR}/input_trace.c)
      else()
        set(trace_source ${PROJECT_SOURCE_DIR}/tests/retroarch/test_input_trace_helper.c)
        set(trace_extra ${CMAKE_CURRENT_LIST_DIR}/input_trace.c)
      endif()
      add_executable(muhtari-test-trace-${trace_target} ${trace_source} ${trace_extra})
      target_include_directories(muhtari-test-trace-${trace_target} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR} ${PROJECT_SOURCE_DIR}/libretro)
      target_link_libraries(muhtari-test-trace-${trace_target} PRIVATE ${CMAKE_DL_LIBS})
      set_target_properties(muhtari-test-trace-${trace_target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
      muhtari_warnings(muhtari-test-trace-${trace_target})
    endforeach()
    # Only the platform API declarations are mocked; POSIX descriptors/fork/exec are real.
    set(trace_include ${PROJECT_BINARY_DIR}/tests/trace-include)
    target_include_directories(muhtari-test-trace-helper PRIVATE ${trace_include})
  endif()
  # Fault prevented: unusable BIOS candidates hiding the next file, or
  # allocation/I/O/path failures being mistaken for missing firmware.
  add_executable(muhtari-test-libretro-bios ${CMAKE_CURRENT_LIST_DIR}/bios.c
    ${PROJECT_SOURCE_DIR}/tests/cart/images.c ${PROJECT_SOURCE_DIR}/runner/sha256.c)
  target_include_directories(muhtari-test-libretro-bios PRIVATE ${PROJECT_SOURCE_DIR}/libretro
    ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/runner ${PROJECT_BINARY_DIR}/generated)
  target_compile_definitions(muhtari-test-libretro-bios PRIVATE
    $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
  target_link_libraries(muhtari-test-libretro-bios PRIVATE muhtari::muhtari muhtari_host)
  set_target_properties(muhtari-test-libretro-bios PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  muhtari_warnings(muhtari-test-libretro-bios)
  muhtari_add_test(NAME libretro.bios_failures COMMAND muhtari-test-libretro-bios
    LABELS libretro portable TIMEOUT 10)

  add_executable(muhtari-test-libretro-host ${CMAKE_CURRENT_LIST_DIR}/host.c
    ${PROJECT_SOURCE_DIR}/tests/cart/images.c
    ${PROJECT_SOURCE_DIR}/runner/sha256.c)
  target_include_directories(muhtari-test-libretro-host PRIVATE ${PROJECT_SOURCE_DIR}/libretro
    ${PROJECT_SOURCE_DIR}/runner)
  target_link_libraries(muhtari-test-libretro-host PRIVATE
    muhtari::muhtari muhtari_host ${CMAKE_DL_LIBS})
  # MSVC deprecates ISO C's fopen for its _s forms.
  target_compile_definitions(muhtari-test-libretro-host PRIVATE
    $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
  set_target_properties(muhtari-test-libretro-host PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  muhtari_warnings(muhtari-test-libretro-host)
  # Final save storage must be visible before the first emulated frame.
  muhtari_add_test(NAME libretro.host.battery
    COMMAND muhtari-test-libretro-host $<TARGET_FILE:muhtari_libretro> battery
    LABELS libretro portable TIMEOUT 5)
  muhtari_add_test(NAME libretro.host.battery_order
    COMMAND muhtari-test-libretro-host $<TARGET_FILE:muhtari_libretro> battery_order
    LABELS libretro portable TIMEOUT 5)
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
    LABELS libretro portable TIMEOUT 5)
  # Provisional: ceil(2 x 2.1913 s), 12 complete AppleClang21 ASan/UBSan
  # runs in batches of four; native Linux arm64 0.90 s, run 40152879574.
  # The Linux 1 s / sanitizer 2 s timeouts are censored, not measurements.
  set_tests_properties(libretro.host.input PROPERTIES FIXTURES_REQUIRED input_carts)
  # Fault prevented: a control, device, option or Restart that behaves
  # otherwise than the RetroPad layout and options docs/libretro.md lists.
  foreach(libretro_case IN ITEMS layout devices reset options)
    muhtari_add_test(NAME libretro.host.${libretro_case}
      COMMAND muhtari-test-libretro-host $<TARGET_FILE:muhtari_libretro> ${libretro_case}
        ${libretro_input_cart}
      LABELS libretro portable TIMEOUT 2)  # ceil(2 x 0.87 s), Windows x64 devices, slowest complete of 4 runs, runs 40414104123 and 40419374588; a 1 s deadline timed out at 1.09 s, run 40698007354
    set_tests_properties(libretro.host.${libretro_case} PROPERTIES FIXTURES_REQUIRED input_carts)
  endforeach()
  # Fault prevented: a BIOS file in the system directory that is ignored,
  # unlogged, or missing without a fast-boot fallback. The case writes the
  # file into its own directory under both names and removes it again.
  set(libretro_system ${PROJECT_BINARY_DIR}/tests/libretro-system)
  file(MAKE_DIRECTORY ${libretro_system})
  set(libretro_hsc_system ${PROJECT_BINARY_DIR}/tests/libretro-hsc-system)
  file(MAKE_DIRECTORY ${libretro_hsc_system})
  muhtari_add_test(NAME libretro.host.hsc_firmware
    COMMAND muhtari-test-libretro-host $<TARGET_FILE:muhtari_libretro> hsc_firmware ${libretro_hsc_system}
    LABELS libretro portable TIMEOUT 5)
  muhtari_add_test(NAME libretro.host.bios
    COMMAND muhtari-test-libretro-host $<TARGET_FILE:muhtari_libretro> bios
      ${MUHTARI_BUILT_DIR}/simple/simple.bas.a78 ${MUHTARI_BUILT_DIR}/openbios/openbios.bin
      ${libretro_system}
    LABELS libretro portable TIMEOUT 8)  # 2 x 3.53 s, macOS x64, slowest of 10 runs, run 39382016958
  set_tests_properties(libretro.host.bios PROPERTIES
    FIXTURES_REQUIRED "built_simple;built_openbios")
  # Fault prevented: an achievement map that points rcheevos at the wrong
  # bytes, a register, or a banked window.
  muhtari_add_test(NAME libretro.host.memory_map
    COMMAND muhtari-test-libretro-host $<TARGET_FILE:muhtari_libretro> memory_map
      ${libretro_input_cart} ${MUHTARI_BUILT_DIR}/doublebuffer/doublebuffer.bas.a78
      ${PROJECT_SOURCE_DIR}/tests/diag/sg144.a78
    LABELS libretro portable TIMEOUT 1)  # 2 x 0.08 s, Linux x64, slowest of 10 runs, run 39373524239
  set_tests_properties(libretro.host.memory_map PROPERTIES
    FIXTURES_REQUIRED "input_carts;built_doublebuffer")

  muhtari_add_test(NAME libretro.host.audio
    COMMAND ${CMAKE_COMMAND} "-DHOST=$<TARGET_FILE:muhtari-test-libretro-host>"
      "-DMODULE=$<TARGET_FILE:muhtari_libretro>" "-DRUNNER=$<TARGET_FILE:muhtari-run>"
      "-DROM=${PROJECT_BINARY_DIR}/built/soundtest/soundtest.bas.a78"
      "-DOUT=${PROJECT_BINARY_DIR}/tests/libretro-audio" "-DEMULATOR=${CMAKE_CROSSCOMPILING_EMULATOR}"
      -P ${CMAKE_CURRENT_LIST_DIR}/audio.cmake
    LABELS audio libretro portable TIMEOUT 30)
  set_tests_properties(libretro.host.audio PROPERTIES FIXTURES_REQUIRED built_soundtest)

  # rcheevos' resolver over the core's real map (rcheevos v12.1.0, MIT,
  # fetched by pins.txt, never vendored or shipped). Its three files build
  # with warnings and sanitizers off, for this target only (rc_libretro.c
  # adds an offset to a null region pointer, which UBSan reports); the two
  # rhash functions rc_libretro.c calls are stubbed. libretro.c is compiled into the test with
  # MUHTARI_LIBRETRO_TEST, which adds the accessor to its instance.
  # Fault prevented: a map that RetroArch's achievements read wrongly: a
  # pointer off by a byte, a register with memory, more than rcheevos' 32
  # regions, or a hole that hides the RAM shadows or the fixed ROM.
  set(libretro_rc ${PROJECT_SOURCE_DIR}/build/pins/rcheevos)
  if(NOT EXISTS ${libretro_rc}/src/rc_libretro.c)
    message(FATAL_ERROR "libretro: libretro.rcheevos needs rcheevos in ${libretro_rc}; fetch it with\n"
      "  scripts/fetch-pin.sh rcheevos build/pins/rcheevos")
  endif()
  set(libretro_rc_stub ${PROJECT_BINARY_DIR}/tests/rc_hash_stub.c)
  file(WRITE ${libretro_rc_stub} "#include \"rc_hash_internal.h\"\n"
    "int rc_hash_iterator_error(const rc_hash_iterator_t* iterator, const char* message)\n"
    "{ (void)iterator; (void)message; return 0; }\n"
    "int rc_path_compare_extension(const char* path, const char* ext)\n"
    "{ (void)path; (void)ext; return 0; }\n")
  add_library(muhtari_test_rcheevos STATIC ${libretro_rc}/src/rc_libretro.c
    ${libretro_rc}/src/rcheevos/consoleinfo.c ${libretro_rc}/src/rc_compat.c ${libretro_rc_stub})
  target_include_directories(muhtari_test_rcheevos PRIVATE ${libretro_rc}/include
    ${libretro_rc}/src ${libretro_rc}/src/rhash ${PROJECT_SOURCE_DIR}/libretro)
  set_target_properties(muhtari_test_rcheevos PROPERTIES COMPILE_WARNING_AS_ERROR OFF)
  target_compile_options(muhtari_test_rcheevos PRIVATE $<$<C_COMPILER_ID:MSVC>:/W0>
    $<$<NOT:$<C_COMPILER_ID:MSVC>>:-w -fno-sanitize=all>)
  add_executable(muhtari-test-rcheevos ${CMAKE_CURRENT_LIST_DIR}/rcheevos.c
    ${PROJECT_SOURCE_DIR}/tests/cart/images.c
    ${PROJECT_SOURCE_DIR}/libretro/libretro.c ${PROJECT_SOURCE_DIR}/runner/sha256.c
    ${PROJECT_SOURCE_DIR}/tests/cart/test_cartdb.c
    $<TARGET_OBJECTS:muhtari_objs> $<TARGET_OBJECTS:muhtari_cpu>)
  target_compile_definitions(muhtari-test-rcheevos PRIVATE MUHTARI_LIBRETRO_TEST
    $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
  target_include_directories(muhtari-test-rcheevos PRIVATE ${PROJECT_SOURCE_DIR}/libretro
    ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/runner ${PROJECT_BINARY_DIR}/generated)
  target_include_directories(muhtari-test-rcheevos SYSTEM PRIVATE ${libretro_rc}/include
    ${libretro_rc}/src)
  target_link_libraries(muhtari-test-rcheevos PRIVATE muhtari_test_rcheevos muhtari_host ${CMAKE_DL_LIBS})
  set_target_properties(muhtari-test-rcheevos PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  muhtari_warnings(muhtari-test-rcheevos)
  # Isolated synthetic tables never enter an installed module or package.
  add_library(muhtari_libretro_testdb MODULE ${PROJECT_SOURCE_DIR}/libretro/libretro.c
    ${PROJECT_SOURCE_DIR}/runner/sha256.c ${PROJECT_SOURCE_DIR}/tests/cart/test_cartdb.c
    $<TARGET_OBJECTS:muhtari_objs> $<TARGET_OBJECTS:muhtari_cpu>)
  target_compile_definitions(muhtari_libretro_testdb PRIVATE MUHTARI_LIBRETRO_TEST
    $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
  target_include_directories(muhtari_libretro_testdb PRIVATE ${PROJECT_SOURCE_DIR}/libretro
    ${PROJECT_SOURCE_DIR}/runner ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/generated)
  target_link_libraries(muhtari_libretro_testdb PRIVATE muhtari_host)
  set_target_properties(muhtari_libretro_testdb PROPERTIES PREFIX "" C_VISIBILITY_PRESET hidden
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  if(APPLE)
    set_target_properties(muhtari_libretro_testdb PROPERTIES SUFFIX ".dylib")
  endif()
  muhtari_warnings(muhtari_libretro_testdb)
  # Same test-only observation seam in the second pixel format; never installed.
  add_library(muhtari_libretro_testdb565 MODULE ${PROJECT_SOURCE_DIR}/libretro/libretro.c
    ${PROJECT_SOURCE_DIR}/runner/sha256.c ${PROJECT_SOURCE_DIR}/tests/cart/test_cartdb.c
    $<TARGET_OBJECTS:muhtari_objs> $<TARGET_OBJECTS:muhtari_cpu>)
  target_compile_definitions(muhtari_libretro_testdb565 PRIVATE MUHTARI_LIBRETRO_TEST USE_RGB565
    $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
  target_include_directories(muhtari_libretro_testdb565 PRIVATE ${PROJECT_SOURCE_DIR}/libretro
    ${PROJECT_SOURCE_DIR}/runner ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/generated)
  target_link_libraries(muhtari_libretro_testdb565 PRIVATE muhtari_host)
  set_target_properties(muhtari_libretro_testdb565 PROPERTIES PREFIX "" C_VISIBILITY_PRESET hidden
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  if(APPLE)
    set_target_properties(muhtari_libretro_testdb565 PROPERTIES SUFFIX ".dylib")
  endif()
  muhtari_warnings(muhtari_libretro_testdb565)

  add_executable(muhtari-test-diag-testdb ${PROJECT_SOURCE_DIR}/tests/diag/driver.c
    ${PROJECT_SOURCE_DIR}/tests/runner/png_read.c
    ${PROJECT_SOURCE_DIR}/runner/sha256.c
    ${PROJECT_SOURCE_DIR}/tests/cart/test_cartdb.c
    $<TARGET_OBJECTS:muhtari_objs> $<TARGET_OBJECTS:muhtari_cpu>)
  target_compile_definitions(muhtari-test-diag-testdb PRIVATE DIAG_LIBRETRO
    $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
  target_include_directories(muhtari-test-diag-testdb PRIVATE ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/libretro ${PROJECT_SOURCE_DIR}/src)
  target_link_libraries(muhtari-test-diag-testdb PRIVATE muhtari_host ${CMAKE_DL_LIBS})
  set_target_properties(muhtari-test-diag-testdb PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  muhtari_warnings(muhtari-test-diag-testdb)
  set(libretro_retail ${PROJECT_BINARY_DIR}/tests/libretro-retail.cmake)
  math(EXPR libretro_host_timeout "90 * ${MUHTARI_TIMEOUT_SCALE}")
  file(GENERATE OUTPUT ${libretro_retail} CONTENT
"set(LIBRETRO_RETAIL_RUN TRUE)
set(ROOT [==[${PROJECT_SOURCE_DIR}]==])
set(OUT [==[${PROJECT_BINARY_DIR}/tests/libretro-retail]==])
set(RESOLVER [==[$<TARGET_FILE:muhtari-test-rcheevos>]==])
set(MODULE [==[$<TARGET_FILE:muhtari_libretro_testdb>]==])
set(DRIVER [==[$<TARGET_FILE:muhtari-test-diag-testdb>]==])
set(RUNNER [==[$<TARGET_FILE:muhtari-run-testdb>]==])
set(EXPECTED [==[${PROJECT_SOURCE_DIR}/tests/diag/expected.csv]==])
set(EMULATOR [==[${CMAKE_CROSSCOMPILING_EMULATOR}]==])
set(HOST_TIMEOUT ${libretro_host_timeout})
set(CARTS [==[${MUHTARI_BUILT_DIR}/simple/simple.bas.a78]==]
  [==[${MUHTARI_BUILT_DIR}/doublebuffer/doublebuffer.bas.a78]==]
  [==[${PROJECT_SOURCE_DIR}/tests/diag/activision.a78]==]
  [==[${PROJECT_SOURCE_DIR}/tests/diag/absolute.a78]==]
  [==[${PROJECT_SOURCE_DIR}/tests/diag/fractalus.a78]==])
include([==[${CMAKE_CURRENT_LIST_FILE}]==])
")
  muhtari_add_test(NAME libretro.rcheevos COMMAND ${CMAKE_COMMAND} -P ${libretro_retail}
    LABELS libretro portable TIMEOUT 120) # bounded 600-frame comparisons, as diag.retail
  set_tests_properties(libretro.rcheevos PROPERTIES
    FIXTURES_REQUIRED "built_simple;built_doublebuffer")
  # Faults: HALT/overlay storage claimed as fixed CPU memory, XM HSC exposed
  # while gated, wrong CPU-half offsets, or a hole hiding later fixed ROM.
  set(libretro_expansion ${PROJECT_BINARY_DIR}/tests/libretro-expansion.cmake)
  file(GENERATE OUTPUT ${libretro_expansion} CONTENT
"set(LIBRETRO_EXPANSION_RUN TRUE)
set(RESOLVER [==[$<TARGET_FILE:muhtari-test-rcheevos>]==])
set(EMULATOR [==[${CMAKE_CROSSCOMPILING_EMULATOR}]==])
set(HOST_TIMEOUT ${libretro_host_timeout})
include([==[${CMAKE_CURRENT_LIST_FILE}]==])
")
  muhtari_add_test(NAME libretro.rcheevos.expansion
    COMMAND ${CMAKE_COMMAND} -P ${libretro_expansion}
    LABELS libretro portable TIMEOUT 120)

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
  # A description one character over the limit must be named with its length.
  set(libretro_fixture ${PROJECT_BINARY_DIR}/libretro/long_description.info)
  string(REPEAT x 513 libretro_long)
  file(WRITE ${libretro_fixture}
    "supports_no_game = \"true\"\ndescription = \"${libretro_long}\"\n")
  set(libretro_cmd ${CMAKE_COMMAND} -DINFO=${libretro_fixture}
    -DINVENTORY=${libretro_inventory} -P ${libretro_scripts}/info_claims.cmake)
  string(REPLACE ";" "$<SEMICOLON>" libretro_cmd "${libretro_cmd}")
  muhtari_add_test(NAME libretro.info_claims.description.self_test
    COMMAND ${CMAKE_COMMAND} "-DCMD=${libretro_cmd}" -DCODE=1
      "-DREGEX=description is 513 characters, over 512" -P ${libretro_scripts}/expect_fail.cmake
    LABELS libretro TIMEOUT 10)
endif()
