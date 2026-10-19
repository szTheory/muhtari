# muhtari-run tests.
#
# This file is also the command-line checks themselves, run as
#   cmake -DRUN=<muhtari-run> [-DEMULATOR=<list>] -DCASE=<name> [-DOUT=<file>]
#     [-DVERSION_FILE=<file>] [-DFRAMES=<long run>] [-DDIR=<image directory>]
#     -P tests/runner/tests.cmake
# EMULATOR is the build's CMAKE_CROSSCOMPILING_EMULATOR, so an emulated lane
# runs its own guest binary.
if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
  set(card "9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4")

  # Runs muhtari-run with ARGN; fails unless it exits 0 with stdout want.
  function(expect_output want)
    execute_process(COMMAND ${EMULATOR} "${RUN}" ${ARGN}
      OUTPUT_VARIABLE out ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0" OR NOT out STREQUAL want)
      message(FATAL_ERROR "muhtari-run ${ARGN}: expected exit 0 and output\n${want}"
        "got exit ${rc} and output\n${out}${err}")
    endif()
  endfunction()

  # Runs muhtari-run with ARGN through expect_fail.cmake: exit code and
  # output matching regex, or the test fails.
  function(expect_exit code regex)
    set(CMD ${EMULATOR} "${RUN}" ${ARGN})
    set(CODE ${code})
    set(REGEX "${regex}")
    include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cmake/expect_fail.cmake)
  endfunction()


  # The --probe line keys, in the order the runner prints them.
  set(identity_keys kind source board board_options rom_size ram_size sound devices
    port1 port2 region)
  set(header_keys title a78_version a78_rom_size a78_cart_type a78_port1 a78_port2 a78_tv
    a78_save a78_xm a78_mapper a78_mapper_options a78_audio a78_irq)
  set(hash_keys sha1 sha256 md5 crc32 sha1_headerless sha256_headerless md5_headerless
    crc32_headerless)

  # Runs muhtari-run --probe on DIR/name; sets out, err and rc here.
  macro(probe name)
    execute_process(COMMAND ${EMULATOR} "${RUN}" --probe "${DIR}/${name}"
      OUTPUT_VARIABLE out ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(out MATCHES "\r")
      message(FATAL_ERROR "--probe ${name}: standard output holds a carriage return")
    endif()
  endmacro()

  # Fails unless rc is code and the keys of out's lines are, in order, ARGN.
  function(expect_keys name code)
    string(REGEX MATCHALL "[^\n]+" lines "${out}")
    set(keys "")
    foreach(line IN LISTS lines)
      string(REGEX REPLACE " .*" "" key "${line}")
      list(APPEND keys "${key}")
    endforeach()
    if(NOT rc STREQUAL code OR NOT keys STREQUAL "${ARGN}")
      message(FATAL_ERROR "--probe ${name}: expected exit ${code} and keys\n${ARGN}\n"
        "got exit ${rc} and keys\n${keys}\noutput\n${out}${err}")
    endif()
  endfunction()

  # Sets var to the value of key's line in out: everything after the first
  # space.
  function(probe_value key var)
    string(REGEX MATCH "(^|\n)${key} ([^\n]*)" match "${out}")
    set(${var} "${CMAKE_MATCH_2}" PARENT_SCOPE)
  endfunction()

  # Fails unless the sha1, sha256 and md5 lines with suffix equal CMake's
  # own digests of file and the crc32 line is eight lowercase hex digits.
  function(expect_hashes suffix file)
    foreach(alg IN ITEMS SHA1 SHA256 MD5)
      file(${alg} "${file}" want)
      string(TOLOWER "${alg}${suffix}" key)
      probe_value(${key} got)
      if(NOT got STREQUAL want)
        message(FATAL_ERROR "--probe: ${key} is '${got}', CMake's ${alg} of ${file} is ${want}")
      endif()
    endforeach()
    probe_value(crc32${suffix} got)
    if(NOT got MATCHES "^[0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f]$")
      message(FATAL_ERROR "--probe: crc32${suffix} is '${got}', not eight lowercase hex digits")
    endif()
  endfunction()

  set(usage_line "usage: muhtari-run ")
  if(CASE STREQUAL "hash")
    # Fault prevented: the runner's printed hash, the line hosts and CI
    # compare, drifts from the pinned test card or exits non-zero, while the
    # library test still passes.
    expect_output("frame 1 sha256 ${card}\n" --frames 1 --hash-frame 1)
  elseif(CASE STREQUAL "hash.repeat")
    # Fault prevented: repeated or unordered --hash-frame options print
    # duplicate or out-of-order lines, so a host comparing line by line
    # sees a mismatch.
    expect_output("frame 1 sha256 ${card}\nframe ${FRAMES} sha256 ${card}\n"
      --frames ${FRAMES} --hash-frame ${FRAMES} --hash-frame 1 --hash-frame 1)
  elseif(CASE STREQUAL "dump_png")
    # Fault prevented: the PNG the runner dumps, the image compared with
    # RetroArch's screenshot, changes bytes (palette, layout or writer) while
    # the native-frame hash still passes. The pin is the PNG of the pinned
    # test card through the warm NTSC palette.
    set(want_sha256 "1c0b16cb9c92d70869d3aee5cd581627f8902cb528e90f1f1d4252d9288ac956")
    file(REMOVE "${OUT}")
    expect_output("" --frames 1 --dump-frame "1:${OUT}")
    file(SIZE "${OUT}" size)
    file(SHA256 "${OUT}" sha256)
    if(NOT size STREQUAL "232645" OR NOT sha256 STREQUAL want_sha256)
      message(FATAL_ERROR "expected 232645 bytes with SHA-256 ${want_sha256}\n"
        "got ${size} bytes with SHA-256 ${sha256}")
    endif()
  elseif(CASE STREQUAL "version")
    # Fault prevented: --version reports a version other than the release's.
    file(STRINGS "${VERSION_FILE}" version LIMIT_COUNT 1)
    expect_output("muhtari-run ${version}\n" --version)
  # Fault prevented for the usage cases: an invalid command line runs
  # anyway, or fails with a status other than 2, so a script cannot tell a
  # typo from a failed run.
  elseif(CASE STREQUAL "usage.no_args")
    expect_exit(2 "${usage_line}")
  elseif(CASE STREQUAL "usage.unknown")
    expect_exit(2 "unknown option.*${usage_line}" --nope)
  elseif(CASE STREQUAL "usage.positional")
    expect_exit(2 "unexpected argument.*${usage_line}" card.a78)
  elseif(CASE STREQUAL "usage.frame_range")
    expect_exit(2 "${usage_line}" --frames 0)
    expect_exit(2 "${usage_line}" --frames 1 --hash-frame 0)
    expect_exit(2 "beyond --frames.*${usage_line}" --frames 5 --hash-frame 6)
  elseif(CASE STREQUAL "usage.dump_syntax")
    expect_exit(2 "N:FILE.*${usage_line}" --frames 1 --dump-frame 1)
    expect_exit(2 "N:FILE.*${usage_line}" --frames 1 --dump-frame x:f)
  elseif(CASE STREQUAL "exit.failure")
    # Fault prevented: a file the runner cannot write is reported as success.
    expect_exit(1 "cannot open" --frames 1 --dump-frame "1:${OUT}/missing/x.png")
  elseif(CASE STREQUAL "probe.accepted")
    # Fault prevented: --probe misnames a field, drops or reorders a line,
    # or prints a digest of the wrong bytes, so a host or a hash database
    # reads the wrong identity.
    probe(linear-v3-16k)
    expect_keys(linear-v3-16k 0 ${identity_keys} ${header_keys} ${hash_keys})
    string(FIND "${out}" "sha1 " at)
    string(SUBSTRING "${out}" 0 ${at} fields)
    set(want "kind 7800\nsource header\nboard linear\nboard_options none\nrom_size 16384\n"
      "ram_size 0\nsound none\ndevices none\nport1 proline\nport2 proline\nregion ntsc\n"
      "title Clean Linear\na78_version 0x03\na78_rom_size 16384\na78_cart_type 0x0000\n"
      "a78_port1 0x01\na78_port2 0x01\na78_tv 0x00\na78_save 0x00\na78_xm 0x00\n"
      "a78_mapper 0x00\na78_mapper_options 0x00\na78_audio 0x0000\na78_irq 0x00\n")
    string(CONCAT want ${want})
    if(NOT fields STREQUAL want)
      message(FATAL_ERROR "--probe linear-v3-16k: expected\n${want}got\n${fields}")
    endif()
    expect_hashes("" "${DIR}/linear-v3-16k")
    expect_hashes("_headerless" "${DIR}/linear-v3-16k-body")
  else()
    message(FATAL_ERROR "unknown CASE '${CASE}'")
  endif()
  return()
endif()

# Every script that starts muhtari-run gets the emulator, so an emulated lane
# never runs a host binary or fails to start a guest one.
string(REPLACE ";" "$<SEMICOLON>" runner_emulator "${CMAKE_CROSSCOMPILING_EMULATOR}")
set(runner_check ${CMAKE_COMMAND} -DRUN=$<TARGET_FILE:muhtari-run> "-DEMULATOR=${runner_emulator}")
foreach(case IN ITEMS hash hash.repeat usage.no_args usage.unknown usage.positional
    usage.frame_range usage.dump_syntax)
  muhtari_add_test(NAME runner.${case}
    COMMAND ${runner_check} -DCASE=${case} -DFRAMES=${MUHTARI_LONG_RUN} -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS runner portable TIMEOUT 10)
endforeach()
muhtari_add_test(NAME runner.dump_png
  COMMAND ${runner_check} -DCASE=dump_png -DOUT=${PROJECT_BINARY_DIR}/tests/dump.png
    -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 10)
muhtari_add_test(NAME runner.exit.failure
  COMMAND ${runner_check} -DCASE=exit.failure -DOUT=${PROJECT_BINARY_DIR}/tests
    -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 10)
muhtari_add_test(NAME runner.version
  COMMAND ${runner_check} -DCASE=version -DVERSION_FILE=${PROJECT_SOURCE_DIR}/version.txt
    -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 10)

# --probe on the images muhtari-test-images writes into the build directory;
# no image bytes are tracked.
set(runner_probe_dir ${CMAKE_BINARY_DIR}/runner-probe)
file(MAKE_DIRECTORY ${runner_probe_dir})
muhtari_add_test(NAME runner.probe.images
  COMMAND muhtari-test-images ${runner_probe_dir}
  LABELS runner portable TIMEOUT 30)
set_tests_properties(runner.probe.images PROPERTIES FIXTURES_SETUP probe_images)
foreach(case IN ITEMS accepted)
  muhtari_add_test(NAME runner.probe.${case}
    COMMAND ${runner_check} -DCASE=probe.${case} -DDIR=${runner_probe_dir}
      -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS runner portable TIMEOUT 10)
  set_tests_properties(runner.probe.${case} PROPERTIES FIXTURES_REQUIRED probe_images)
endforeach()

# hashes.txt, the file CI compares across lanes.
set(runner_hashes ${PROJECT_SOURCE_DIR}/tests/cmake/hashes.cmake)
muhtari_add_test(NAME runner.hashes_file
  COMMAND ${runner_check} -DMODE=write -DOUT=${PROJECT_BINARY_DIR} -P ${runner_hashes}
  LABELS runner portable TIMEOUT 20)
# The comparison's self tests: two files where three are required, and two
# files one byte apart.
set(runner_fixture ${PROJECT_BINARY_DIR}/tests/hashes)
file(WRITE ${runner_fixture}/a.txt "frame1 00\n")
file(WRITE ${runner_fixture}/b.txt "frame1 01\n")
# CMake wraps long messages, so the regexes allow a break between words.
foreach(case IN ITEMS "count;3;a;expected 3 files,[^/]*got 2" "byte;2;b;b.txt[^/]*differs")
  list(GET case 0 name)
  list(GET case 1 count)
  list(GET case 2 second)
  list(GET case 3 regex)
  set(runner_cmd ${CMAKE_COMMAND} "-DFILES=${runner_fixture}/a.txt\\;${runner_fixture}/${second}.txt"
    -DCOUNT=${count} -DMODE=compare -P ${runner_hashes})
  string(REPLACE ";" "$<SEMICOLON>" runner_cmd "${runner_cmd}")
  muhtari_add_test(NAME runner.hashes_compare.${name}.self_test
    COMMAND ${CMAKE_COMMAND} "-DCMD=${runner_cmd}" -DCODE=1 "-DREGEX=${regex}"
      -P ${PROJECT_SOURCE_DIR}/tests/cmake/expect_fail.cmake
    LABELS runner TIMEOUT 10)
endforeach()

# The PNG writer and its checksums, checked by an independent parse.
add_executable(muhtari-test-png
  ${CMAKE_CURRENT_LIST_DIR}/test_png.c
  ${PROJECT_SOURCE_DIR}/runner/png.c
  ${PROJECT_SOURCE_DIR}/runner/crc32.c)
target_link_libraries(muhtari-test-png PRIVATE muhtari::muhtari muhtari_host)
set_target_properties(muhtari-test-png PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-png)
muhtari_add_test(NAME runner.png
  COMMAND muhtari-test-png ${PROJECT_BINARY_DIR}/tests/png_test.png
  LABELS runner portable TIMEOUT 10)

# The bench statistics, the stop flag and MD5.
add_executable(muhtari-test-runner
  ${CMAKE_CURRENT_LIST_DIR}/test_runner.c
  ${PROJECT_SOURCE_DIR}/runner/bench.c
  ${PROJECT_SOURCE_DIR}/runner/md5.c
  ${PROJECT_SOURCE_DIR}/runner/stop.c)
target_compile_definitions(muhtari-test-runner PRIVATE _POSIX_C_SOURCE=200809L)
set_target_properties(muhtari-test-runner PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-runner)
muhtari_add_test(NAME runner.bench.percentile COMMAND muhtari-test-runner percentile
  LABELS runner bench portable TIMEOUT 10)
# Fault prevented: on a 32-bit size_t, a large --bench count wraps the
# sample buffer's size, and the bench writes past a too-small allocation.
muhtari_add_test(NAME runner.bench.bytes COMMAND muhtari-test-runner bytes
  LABELS runner bench portable TIMEOUT 10)
muhtari_add_test(NAME runner.bench.format COMMAND muhtari-test-runner format
  LABELS runner bench portable TIMEOUT 10)
muhtari_add_test(NAME runner.stop COMMAND muhtari-test-runner stop
  LABELS runner portable TIMEOUT 10)
# Fault prevented: a wrong MD5 constant or byte order changes the
# RetroAchievements hash --probe prints, with nothing else to notice it.
muhtari_add_test(NAME runner.md5 COMMAND muhtari-test-runner md5
  LABELS runner portable TIMEOUT 10)

# Not portable: it reads docs/runner.md from the source tree.
muhtari_add_test(NAME runner.exit.table
  COMMAND ${runner_check} -DDOC=${PROJECT_SOURCE_DIR}/docs/runner.md
    -P ${PROJECT_SOURCE_DIR}/tests/cmake/exit_table.cmake
  LABELS runner TIMEOUT 10)
