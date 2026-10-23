# The generated retail batch reuses this file as a CMake script. Faults:
# absent/corrupt runner reports, incomplete mailbox protocols, and a missing
# admitted fixture must never produce a passing three-deliverable observation.
if(DIAG_RETAIL_RUN)
  function(diag_exec)
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" ${ARGV}
      RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 30)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "diag retail ${ARGV}: ${rc}\n${out}\n${err}")
    endif()
    message("${out}")
  endfunction()
  function(diag_observe path name smoke)
    if(smoke)
      diag_exec(--smoke "${path}")
    else()
      diag_exec("${path}" "${EXPECTED}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${RUNNER}" --probe "${path}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE probe TIMEOUT 10)
    if(name STREQUAL "smoke")
      set(board absolute)
    else()
      set(board "${name}")
    endif()
    if(NOT rc STREQUAL "0" OR NOT probe MATCHES "\nboard ${board}\n")
      message(FATAL_ERROR "diag retail wrong board: ${rc} ${probe}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${RUNNER}" --frames 600 --diagnostic "${path}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE report ERROR_VARIABLE err TIMEOUT 30)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "diag retail runner: ${rc} ${err}")
    endif()
    file(CONFIGURE OUTPUT "${OUT}/report.txt" CONTENT "${report}" @ONLY NEWLINE_STYLE UNIX)
    diag_exec(--mailbox "${OUT}/report.txt" "${name}" "${EXPECTED}")
    diag_exec(--libretro "${MODULE}" "${path}" "${EXPECTED}")
    message("${name}: library, runner and libretro numeric observations passed")
  endfunction()
  file(MAKE_DIRECTORY "${OUT}")
  diag_observe("${OUT}/smoke.a78" smoke TRUE)
  file(READ "${OUT}/report.txt" good_report)
  foreach(bad IN ITEMS "" "${good_report}extra" "diagnostic xyz\n")
    file(WRITE "${OUT}/bad-report.txt" "${bad}")
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" --mailbox "${OUT}/bad-report.txt" smoke
      "${EXPECTED}" RESULT_VARIABLE rc TIMEOUT 10)
    if(NOT rc STREQUAL "1")
      message(FATAL_ERROR "diag retail accepted malformed/absent report: ${rc}")
    endif()
  endforeach()
  execute_process(COMMAND ${EMULATOR} "${DRIVER}" --mailbox "${OUT}/absent-report" smoke
    "${EXPECTED}" RESULT_VARIABLE rc TIMEOUT 10)
  if(NOT rc STREQUAL "1")
    message(FATAL_ERROR "diag retail accepted missing report")
  endif()
  foreach(args IN ITEMS "--diagnostic" "--frames;1;--diagnostic"
      "--bench;1;--diagnostic;${OUT}/smoke.a78"
      "--frames;1;--diagnostic;--diagnostic;${OUT}/smoke.a78")
    execute_process(COMMAND ${EMULATOR} "${RUNNER}" ${args}
      RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET TIMEOUT 10)
    if(NOT rc STREQUAL "2")
      message(FATAL_ERROR "diag retail usage fault ${args}: ${rc}")
    endif()
  endforeach()
  file(STRINGS "${MANIFEST}" admitted REGEX "^tests/diag/(activision|absolute|fractalus)[.]a78\t")
  foreach(row IN LISTS admitted)
    string(REPLACE "\t" ";" fields "${row}")
    list(GET fields 0 relative)
    list(GET fields 1 source)
    list(GET fields 3 license)
    if(NOT source STREQUAL "this-repo" OR NOT license STREQUAL "MIT" OR
        NOT EXISTS "${ROOT}/${relative}")
      message(FATAL_ERROR "diag retail missing or unowned admitted fixture: ${relative}")
    endif()
    get_filename_component(name "${relative}" NAME_WE)
    diag_observe("${ROOT}/${relative}" "${name}" FALSE)
  endforeach()
  return()
endif()

# Our own diagnostic ROMs (docs/emulation-tests.md, diag). Each tests/diag/*.s
# is MIT code of this repository, assembled into the committed .a78 beside it
# by assemble.cmake. muhtari-test-diag runs one and judges its mailbox against
# expected.csv, whose values docs/hardware/maria.md derives before a ROM
# first runs.
set(diag_dir ${CMAKE_CURRENT_LIST_DIR})
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${diag_dir}/expected.csv
  ${diag_dir}/stats.csv)

# muhtari-test-diag: links muhtari and includes src/internal.h, to read the
# mailbox through muhtari__peek and the core's per-line statistics, which
# stats.csv gives for each DMA test, so each cost is recorded twice.
add_executable(muhtari-test-diag ${diag_dir}/driver.c)
target_include_directories(muhtari-test-diag PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-diag PRIVATE muhtari::muhtari)
if(TARGET muhtari_libretro)
  target_include_directories(muhtari-test-diag PRIVATE ${PROJECT_SOURCE_DIR}/libretro)
  target_compile_definitions(muhtari-test-diag PRIVATE DIAG_LIBRETRO
    $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
  target_link_libraries(muhtari-test-diag PRIVATE ${CMAKE_DL_LIBS})
  set(diag_retail ${PROJECT_BINARY_DIR}/tests/diag/retail.cmake)
  file(GENERATE OUTPUT ${diag_retail} CONTENT
"set(DIAG_RETAIL_RUN TRUE)
set(ROOT [==[${PROJECT_SOURCE_DIR}]==])
set(OUT [==[${PROJECT_BINARY_DIR}/tests/diag/retail]==])
set(DRIVER [==[$<TARGET_FILE:muhtari-test-diag>]==])
set(RUNNER [==[$<TARGET_FILE:muhtari-run>]==])
set(MODULE [==[$<TARGET_FILE:muhtari_libretro>]==])
set(EMULATOR [==[${CMAKE_CROSSCOMPILING_EMULATOR}]==])
set(EXPECTED [==[${diag_dir}/expected.csv]==])
set(MANIFEST [==[${PROJECT_SOURCE_DIR}/tests/roms/manifest.txt]==])
include([==[${CMAKE_CURRENT_LIST_FILE}]==])
")
  muhtari_add_test(NAME diag.retail COMMAND ${CMAKE_COMMAND} -P ${diag_retail}
    LABELS diag portable TIMEOUT 120)
endif()
set_target_properties(muhtari-test-diag PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-diag)

muhtari_add_test(NAME diag.dma
  COMMAND muhtari-test-diag ${diag_dir}/dma.a78 ${diag_dir}/expected.csv ${diag_dir}/stats.csv
  LABELS diag portable TIMEOUT 1)  # 2 x 0.10 s, macOS x64, slowest of 27 runs, run 39358378619
muhtari_add_test(NAME diag.cpu
  COMMAND muhtari-test-diag ${diag_dir}/cpu.a78 ${diag_dir}/expected.csv
  LABELS diag portable TIMEOUT 1)  # 2 x 0.05 s, Windows x64, slowest of 24 runs, run 39358378619
muhtari_add_test(NAME diag.dli
  COMMAND muhtari-test-diag ${diag_dir}/dli.a78 ${diag_dir}/expected.csv ${diag_dir}/stats.csv
  LABELS diag portable TIMEOUT 1)  # 2 x 0.09 s, macOS x64, slowest of 23 runs, run 39362291672
muhtari_add_test(NAME diag.modes
  COMMAND muhtari-test-diag ${diag_dir}/modes.a78 ${diag_dir}/expected.csv
  LABELS diag portable TIMEOUT 1)  # 2 x 0.04 s, Windows arm64, slowest of 19 runs, run 39382343202
# The board ROMs: the bytes each board maps (docs/hardware/cartridge.md,
# The board ROMs).
foreach(diag_board IN ITEMS linear16 bank6 sg144)
  muhtari_add_test(NAME diag.${diag_board}
    COMMAND muhtari-test-diag ${diag_dir}/${diag_board}.a78 ${diag_dir}/expected.csv
    LABELS diag portable TIMEOUT 1)  # 2 x 0.03 s, Windows arm64, slowest of 21 runs, run 39382343202
endforeach()

# diag.rebuild: every tests/diag/*.s that has a .cfg, and Bruce Clark's
# decimal test as printed and as its 65C02 control, assembled by the pinned
# tools and compared with the manifest (assemble.cmake). One generated script
# runs them in turn; a failure names the file. The tools come from
# tests/built/tests.cmake.
set(diag_out ${PROJECT_BINARY_DIR}/tests/diag)
set(diag_manifest ${CMAKE_SOURCE_DIR}/tests/roms/manifest.txt)
set(diag_decimal ${CMAKE_SOURCE_DIR}/tests/cpu/decimal)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${diag_manifest})
file(GLOB diag_sources CONFIGURE_DEPENDS ${diag_dir}/*.s)
set(diag_rebuild "")
foreach(diag_src IN LISTS diag_sources)
  string(REGEX REPLACE "\\.s$" ".cfg" diag_cfg "${diag_src}")
  if(EXISTS ${diag_cfg})
    string(APPEND diag_rebuild "set(SRC [==[${diag_src}]==])\nset(CFG [==[${diag_cfg}]==])\n"
      "set(PATH \"\")\nset(VARIANT \"\")\ninclude([==[${diag_dir}/assemble.cmake]==])\n")
  endif()
endforeach()
foreach(diag_variant IN ITEMS 6502 65c02)
  set(diag_v "")
  if(diag_variant STREQUAL "65c02")
    set(diag_v 65c02)
  endif()
  string(APPEND diag_rebuild "set(SRC [==[${diag_decimal}/clark.s]==])\nset(CFG \"\")\n"
    "set(PATH tests/cpu/decimal/clark-${diag_variant}.bin)\nset(VARIANT \"${diag_v}\")\n"
    "include([==[${diag_dir}/assemble.cmake]==])\n")
endforeach()
set(diag_tools "set(WASMTIME [==[${MUHTARI_WASMTIME}]==])\nset(PKG [==[${MUHTARI_7800BASIC}]==])\n")
set(diag_rebuild_script ${diag_out}/rebuild.cmake)
file(WRITE ${diag_rebuild_script} "${diag_tools}set(OUT [==[${diag_out}/rebuild]==])\n"
  "set(MANIFEST [==[${diag_manifest}]==])\n${diag_rebuild}")
muhtari_add_test(NAME diag.rebuild COMMAND ${CMAKE_COMMAND} -P ${diag_rebuild_script}
  LABELS built portable TIMEOUT 22)  # 2 x 10.66 s, macOS x64, slowest of 26 runs, run 39362291672

# Negative control: one digit of dma.a78's digest changed in a copy of the
# manifest must fail by name.
file(READ ${diag_manifest} diag_text)
string(REGEX MATCH "tests/diag/dma\\.a78\t[^\t]*\t[^\t]*\t[^\t]*\t([0-9a-f]+)" diag_row "${diag_text}")
set(diag_digest ${CMAKE_MATCH_1})
string(SUBSTRING ${diag_digest} 0 63 diag_head)
string(SUBSTRING ${diag_digest} 63 1 diag_tail)
if(diag_tail STREQUAL "0")
  set(diag_tail 1)
else()
  set(diag_tail 0)
endif()
string(REPLACE "${diag_digest}" "${diag_head}${diag_tail}" diag_text "${diag_text}")
file(WRITE ${diag_out}/manifest_wrong.txt "${diag_text}")
set(diag_wrong_script ${diag_out}/rebuild_wrong.cmake)
file(WRITE ${diag_wrong_script} "${diag_tools}set(OUT [==[${diag_out}/rebuild_wrong]==])\n"
  "set(MANIFEST [==[${diag_out}/manifest_wrong.txt]==])\n"
  "set(SRC [==[${diag_dir}/dma.s]==])\nset(CFG [==[${diag_dir}/dma.cfg]==])\n"
  "set(PATH tests/diag/dma.a78)\ninclude([==[${diag_dir}/assemble.cmake]==])\n")
muhtari_add_test(NAME diag.rebuild.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${CMAKE_COMMAND}$<SEMICOLON>-P$<SEMICOLON>${diag_wrong_script}"
    -DCODE=1 "-DREGEX=tests/diag/dma\\.a78: SHA-256 drift"
    -P ${PROJECT_SOURCE_DIR}/tests/cmake/expect_fail.cmake
  LABELS built portable TIMEOUT 3)  # 2 x 1.09 s, Windows arm64, slowest of 26 runs, run 39363678798

# Negative control: a copy of expected.csv with the first gating value
# changed must fail on that record. The driver runs from a generated script,
# so the emulator list needs no second level of escaping.
file(READ ${diag_dir}/expected.csv diag_text)
string(REPLACE "\ndma,null,241,hw\n" "\ndma,null,242,hw\n" diag_text "${diag_text}")
file(WRITE ${diag_out}/expected_wrong.csv "${diag_text}")
set(diag_expected_script ${diag_out}/expected_self_test.cmake)
file(GENERATE OUTPUT ${diag_expected_script} CONTENT
"execute_process(COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} [==[$<TARGET_FILE:muhtari-test-diag>]==]
  [==[${diag_dir}/dma.a78]==] [==[${diag_out}/expected_wrong.csv]==]
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE out)
message(\"\${out}\")
if(NOT rc STREQUAL \"0\")
  message(FATAL_ERROR \"muhtari-test-diag exited \${rc}\")
endif()
")
muhtari_add_test(NAME diag.expected.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${CMAKE_COMMAND}$<SEMICOLON>-P$<SEMICOLON>${diag_expected_script}"
    -DCODE=1 "-DREGEX=dma null: 241 expected 242 .hw. id 1 flags 01 FAIL.*exited 1"
    -P ${PROJECT_SOURCE_DIR}/tests/cmake/expect_fail.cmake
  LABELS diag portable TIMEOUT 1)  # 2 x 0.26 s, macOS x64, slowest of 26 runs, run 39354563718
